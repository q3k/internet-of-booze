#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <stdint.h>
#include <string.h>
#include <assert.h>

xQueueHandle xModemReceiveQueue;
xQueueHandle xModemTransmitQueue;

typedef enum {
    STATE_INIT = 0,
    STATE_IDLE = 1
} ModemMachineState_Typedef;

ModemMachineState_Typedef xState;

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
}

static char prvModemReceiveChar(void)
{
    char Data;
    xQueueReceive(xModemReceiveQueue, &Data, portMAX_DELAY);
    return Data;
}

static void prvModemReceiveUntil(char *Part, char *Result, int ResultLen)
{
    char *PartOffset = Part;
    char *ResultOffset = Result;
    while (*Part != 0)
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

static void prvModemStateInit(void)
{
    char ReceiveBuffer[128];
    // Send a few AT's to sync baud
    prvModemSendLine("AT");
    prvModemSendLine("AT");
    prvModemSendLine("AT");
    prvModemSendLine("AT");
    // Disable local echo
    prvModemSendLine("ATE0");
    while (1)
    {
        prvModemReceiveUntil("\r", ReceiveBuffer, sizeof(ReceiveBuffer));
        if (strncmp(ReceiveBuffer, "ATE0", 4))
            break;
    }
    prvModemReceiveUntil("\r", ReceiveBuffer, sizeof(ReceiveBuffer));
    assert(!strncmp(ReceiveBuffer, "OK", 2));
    
    while (1)
    {
    }
}

static void prvModemStateIdle(void)
{

}

void xModemCommunicationTask(void *Parameter)
{
    (void) Parameter;

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
    }
}

