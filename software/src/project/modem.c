#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>
#include <stdio.h>

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "debug.h"
#include "control.h"

xQueueHandle xModemReceiveQueue;
xQueueHandle xModemTransmitQueue;

typedef enum {
    STATE_INIT = 0,
    STATE_IDLE = 1
} ModemMachineState_Typedef;

ModemMachineState_Typedef xState;

static void prvModemFlush(void)
{
    USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
}

static void prvModemSendChar(char Data)
{
    xQueueSendToBack(xModemTransmitQueue, &Data, portMAX_DELAY);
}

static void prvModemSendLine(const char *Line)
{
    while (*Line != 0)
    {
        prvModemSendChar(*Line);
        Line++;
    }
    prvModemSendChar('\r');
    prvModemFlush();
}

static char prvModemReceiveChar(void)
{
    char Data;
    xQueueReceive(xModemReceiveQueue, &Data, portMAX_DELAY);
    //vDebugSpewChar(Data);
    return Data;
}

static char prvModemReceiveCharTimeout(int32_t Timeout)
{
    char Data;
    if (xQueueReceive(xModemReceiveQueue, &Data, Timeout) == pdTRUE)
        return Data;
    else
        return 0;
}

static void prvModemReceiveUntil(char *Part, char *Result, int ResultLen)
{
    char *PartOffset = Part;
    char *ResultOffset = Result;
    while (*PartOffset != 0)
    {
        char Received = prvModemReceiveChar();

        if (*PartOffset == Received)
        {
            if ((ResultOffset - Result) < ResultLen)
            {
                *ResultOffset = Received;
                ResultOffset++;
            }
            PartOffset++;
        }
        else
        {
            if ((ResultOffset - Result) < ResultLen)
            {
                *ResultOffset = Received;
                ResultOffset++;
            }
            PartOffset = Part;
        }
    }
    uint32_t PartLen = strlen(Part);
    ResultOffset -= PartLen;
    *ResultOffset = 0;
}

static void prvIssueCommand(const char *Command)
{
    prvModemSendLine(Command);
    char ReceiveBuffer[128];
    uint32_t i = 0;

    vDebugSpewString(" -> ");
    vDebugSpewString(Command);
    vDebugSpewString("\r\n");

    while (1)
    {
        char Received = prvModemReceiveChar();
        if (i >= sizeof(ReceiveBuffer))
            return;
        ReceiveBuffer[i] = Received;

        const char *OK = "OK\r\n";
        const char *ERROR = "ERROR\r\n";
        if ((i > strlen(OK)) && (strncmp(&ReceiveBuffer[i-strlen(OK)+1], OK, strlen(OK)) == 0))
        {
            Received = prvModemReceiveCharTimeout(1000);
            if (Received == 0)
                break;
            else
            {
                i++;
                if (i >= sizeof(ReceiveBuffer))
                    return;
                ReceiveBuffer[i] = Received;
            }
        }
        else if ((i > strlen(ERROR)) && (strncmp(&ReceiveBuffer[i-strlen(ERROR)+1], ERROR, strlen(ERROR)) == 0))
        {
            Received = prvModemReceiveCharTimeout(1000);
            if (Received == 0)
                break;
            else
            {
                i++;
                if (i >= sizeof(ReceiveBuffer))
                    return;
                ReceiveBuffer[i] = Received;
            }
        }
        i++;
    }
    ReceiveBuffer[i+1] = 0;
    vDebugSpewString(" <- ");
    vDebugSpewString(ReceiveBuffer);
    vDebugSpewString("\r\n");
}

char Message[160];
static void prvParsePDU(uint8_t *PDU, uint32_t Length)
{
    vDebugSpewString("[mdmc] Parsing PDU...\r\n");
    if (Length < 1)
    {
        vDebugSpewString("[mdmc] Invalid length...\r\n");
        return;
    }
    uint8_t SMSC_Length = PDU[0];
    if (Length < (1 + SMSC_Length))
    {
        vDebugSpewString("[mdmc] Buffer too small...\r\n");
        return;
    }

    uint8_t *SMSDeliver = PDU + 1 + SMSC_Length;
    uint32_t SMSDeliverLength = Length - (SMSC_Length + 1);
    if (SMSDeliverLength < 1)
        return;
    if ((SMSDeliver[0] & 3) != 0)
    {
       vDebugSpewString("[mdmc] Invalid message type!\r\n");
        return;
    }
    vDebugSpewString("[mdmc] SMS Delivery message OK\r\n");
    uint8_t AddressLength = SMSDeliver[1];
    if (SMSDeliverLength < (2 + AddressLength))
    {
        vDebugSpewString("[mdmc] Buffer too small...\r\n");
        return;
    }

    if (SMSDeliver[2] != 0xC8 && SMSDeliver[2] != 0x91)
    {
        vDebugSpewString("[mdmc] Unrecognized sender type address.\r\n");
        return;
    }

    char PhoneNumber[16];
    for (uint8_t i = 0; i < 16; i++)
        PhoneNumber[i] = 0;
    char *pPhoneNumber = PhoneNumber;
    if (SMSDeliver[2] == 0x91)
    {
        *pPhoneNumber++ = '+';
    }

    for (uint8_t i = 0; i < (AddressLength); i++)
    {
        if ((pPhoneNumber - PhoneNumber) > 15)
        {
            break;
        }
        uint8_t Byte = SMSDeliver[3+(i/2)];
        if (i % 2 == 1)
            *pPhoneNumber++ = '0' + ((Byte >> 4));
        else
            *pPhoneNumber++ = '0' + (Byte & 0b1111);
    }
    vDebugSpewString("[mdmc] SMS from: ");
    vDebugSpewString(PhoneNumber);
    vDebugSpewString("\r\n");


    uint8_t *TP = SMSDeliver + 3 + (AddressLength / 2);
    if (AddressLength % 2 == 1)
        TP++;
    uint32_t TPLength = Length - (TP - PDU);
    if (TPLength < 10)
    {
        vDebugSpewString("[mdmc] Buffer too small...\r\n");
        return;
    }
    if (TP[0] != 0 || TP[1] != 0)
    {
        vDebugSpewString("[mdmc] TP-PID or TP-DCS wrong...\r\n");
        return;
    }
    uint8_t UserDataLength = TP[9];
    uint8_t *UserData = &TP[10];
    char *pMessage = Message;
    uint8_t LeftInByte = 8;
    for (uint32_t i = 0; i < UserDataLength; i++)
    {
        uint8_t Byte = 0;
        for (uint32_t j = 0; j < 7; j++)
        {
            uint8_t Bit = (*UserData) & 1;
            *UserData >>= 1;
            LeftInByte--;
            if (LeftInByte == 0)
            {
                LeftInByte = 8;
                UserData++;
            }
            Byte |= (Bit << 7);
            Byte >>= 1;
        }
        *(pMessage++) = Byte;
    }
    *pMessage = 0;
    vDebugSpewString("[mdmc] SMS Data: ");
    vDebugSpewString(Message);
    vDebugSpewString("\r\n");
    if (!strncmp(Message, "CTRL", 4))
    {
        vDebugSpewString("[mdmc] Passing message to Control subsystem.\r\n");
        vControlParseSMS(PhoneNumber, Message);
    }
}

static uint8_t prvASCIIToNibble(char Ascii)
{
    if (Ascii >= '0' && Ascii <= '9')
        return Ascii - '0';
    if (Ascii >= 'A' && Ascii <= 'F')
        return (Ascii - 'A') + 10;
    if (Ascii >= 'a' && Ascii <= 'f')
        return (Ascii - 'a') + 10;
    return 0;
}

char ReceiveBuffer[512];
static void prvReadMessages(void)
{
    uint8_t NextIsPDU = 0;
    prvModemSendLine("AT+CMGL");
    while (1)
    {
        prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
        vDebugSpewString("[mdmc] SMS lister got line: ");
        vDebugSpewString(ReceiveBuffer);
        vDebugSpewString("\r\n");
        if (NextIsPDU)
        {
            vDebugSpewString("[mdmc] PDU line, passing to parser\r\n");
            uint32_t Length = strlen(ReceiveBuffer);
            for (uint32_t i = 0; i < Length/2; i++)
            {
                char NibbleHigher = ReceiveBuffer[i*2];
                char NibbleLower = ReceiveBuffer[i*2+1];
                uint8_t Value = prvASCIIToNibble(NibbleHigher) << 4;
                Value |= prvASCIIToNibble(NibbleLower);
                ReceiveBuffer[i] = Value;
            }
            prvParsePDU((uint8_t*)ReceiveBuffer, Length/2);
            NextIsPDU = 0;
        }
        if (!strncmp(ReceiveBuffer, "+CMGL", 5))
            NextIsPDU = 1;
        if (!strncmp(ReceiveBuffer, "OK", 2))
            break;
    }
}

void prvHandleUnsolicited(const char *Line)
{
    if (!strncmp(Line, "RING", 4))
    {
        vDebugSpewString("[mdmc] Someone is calling us, hang up on them...\r\n");
        prvIssueCommand("ATH");
    }
    else if (!strncmp(Line, "+CMTI", 5))
    {
        vDebugSpewString("[mdmc] Got CMTI, reading messages...\r\n");
        prvReadMessages();
    }
}

static void prvSendSMS(char *Destination, char *Message)
{
    (void) Destination;
    (void) Message;
    vDebugSpewString("Starting to send message...\r\n");
    prvIssueCommand("AT+CMGF=1");
    prvModemSendLine("AT+CMGS=\"+48792973702\"");
    char recv[128];
    prvModemReceiveUntil("> ", recv, sizeof(recv));
    vDebugSpewString("Received until: ");
    vDebugSpewString(recv);
    vDebugSpewString("\r\n");
    vDebugSpewString("Sending message body...\r\n");
    prvModemSendLine("Fuck");
    prvModemSendLine("\x1a");
    prvIssueCommand("AT+CMGF=0");
}

static void prvModemStateInit(void)
{
    vDebugSpewString("[mdmc] Modem initializing...\r\n");
    // Send a few AT's to sync baud
    prvModemSendLine("AT");
    prvModemSendLine("AT");
    prvModemSendLine("AT");
    prvModemSendLine("AT");

    // flush shit...
    char Derp;
    while (xQueueReceive(xModemReceiveQueue, &Derp, 2000) != pdFALSE) {}

    // Disable local echo
    prvModemSendLine("AT&F");
    prvModemReceiveUntil("\r", ReceiveBuffer, sizeof(ReceiveBuffer));
    vDebugSpewString("[mdmc] pre-AT&F response: ");
    vDebugSpewString(ReceiveBuffer);
    vDebugSpewString("\r\n");
    prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
    prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
    vDebugSpewString("[mdmc] AT&F response: ");
    vDebugSpewString(ReceiveBuffer);
    vDebugSpewString("\r\n");

    prvModemSendLine("ATE0");
    vDebugSpewString("[mdmc] Waiting for ATE0 reply...\r\n");
    prvModemReceiveUntil("\r", ReceiveBuffer, sizeof(ReceiveBuffer));
    vDebugSpewString("[mdmc] pre-ATE0 response: ");
    vDebugSpewString(ReceiveBuffer);
    vDebugSpewString("\r\n");
    prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
    prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
    vDebugSpewString("[mdmc] ATE0 response: ");
    vDebugSpewString(ReceiveBuffer);
    vDebugSpewString("\r\n");
    assert(!strncmp(ReceiveBuffer, "OK", 2));

    prvModemSendLine("AT");
    prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
    prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
    vDebugSpewString("[mdmc] AT response: ");
    vDebugSpewString(ReceiveBuffer);
    vDebugSpewString("\r\n");
    assert(!strncmp(ReceiveBuffer, "OK", 2));

    // Set SMS PDU mode
    prvIssueCommand("AT+CPIN?");
    prvIssueCommand("AT+CSQ");
    prvIssueCommand("AT+CNUM");
    prvIssueCommand("AT+CMGF=0");
    prvIssueCommand("AT+CNMI=2,1");
    prvIssueCommand("AT+CNMI?");
    // Delete all text messages
    prvReadMessages();
    prvIssueCommand("AT+CMGDA=6");

    //prvSendSMS("", "");
    // Enter main loop
    while (1)
    {
        vDebugSpewString("[mdmc] Looping...\r\n");
        prvModemReceiveUntil("\r\n", ReceiveBuffer, sizeof(ReceiveBuffer));
        vDebugSpewString("[mdmc] Loop got: ");
        vDebugSpewString(ReceiveBuffer);
        vDebugSpewString("\r\n");
        prvHandleUnsolicited(ReceiveBuffer);
    }
}

static void prvModemStateIdle(void)
{

}

void xModemCommunicationTask(void *Parameter)
{
    (void) Parameter;

    vDebugSpewString("[mdmc] Task started.\r\n");
    xState = STATE_INIT;
    for (;;)
    {
        switch (xState)
        {
            case STATE_INIT:
                prvModemStateInit();
                break;
            case STATE_IDLE:
                prvModemStateIdle();
                break;
        }
        vTaskDelay(100);
    }
}

void USART3_IRQHandler(void)
{
    //vDebugSpewString("[USART3] ISR!\r\n");
    long Interrupted = pdFALSE;
    while (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        //vDebugSpewString("[USART3] Receiving byte!\r\n");
        // Receive a byte from USART into the queue
        char Byte = USART_ReceiveData(USART3);
        xQueueSendToBackFromISR(xModemReceiveQueue, &Byte, &Interrupted);
        if (Interrupted)
            portYIELD_FROM_ISR(Interrupted);
    }

    if (USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
    {
        char Byte;
        if (xQueueReceiveFromISR(xModemTransmitQueue, &Byte, &Interrupted) == pdTRUE)
        {
            /*vDebugSpewString("[USART3] -> ");
            vDebugSpewChar(Byte);
            vDebugSpewString("\r\n");*/
            USART_SendData(USART3, Byte);
            /*    if (Interrupted)
                {
                    vDebugSpewString("[USART3] TXE Interrupted\r\n");
                    portYIELD_FROM_ISR(Interrupted);
                }*/
            //}
            //USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }
        else
        {
            USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
        }
    }
}
