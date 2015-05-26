#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "debug.h"
#include "modem.h"
#include "modem_command.h"

char *x2r34ds = "93lf4s";
char *i32u89xn = "\xbc\xed\xe6\xb5"; // c29j ^ 0xDF
char *d024x20 = "DrgnS{REALFLAGONDEVICEBUTNOTHERE}";

extern int iShotsSold;

static uint8_t qe94ynxf87(char Ascii)
{
    if (Ascii >= '0' && Ascii <= '9')
        return Ascii - '0';
    if (Ascii >= 'A' && Ascii <= 'F')
        return (Ascii - 'A') + 10;
    if (Ascii >= 'a' && Ascii <= 'f')
        return (Ascii - 'a') + 10;
    return 0;
}

OutgoingSMS_TypeDef NewSMS;

void prvConcatNumber(char *String, unsigned int Number)
{
    char S[10];

    int i = 0;
    do {
        S[i++] = Number % 10 + '0';
    } while (( Number /= 10) > 0);

    i--;
    
    for (s32 j = i; j >= 0; j--)
    {
        (*String++) = S[j];
    }
    *String = 0;
}

void vControlCommand_Status(char *Source)
{
    (void) Source;
    vDebugSpewString("[ctrl] Command 0 - Status\r\n");
    strncpy(NewSMS.Recipient, Source, 16);
    strncpy(NewSMS.Body, "Shots dealt: ", 160);
    prvConcatNumber(NewSMS.Body + strlen(NewSMS.Body), iShotsSold);
    strcpy(NewSMS.Body + strlen(NewSMS.Body), " ");
    strcpy(NewSMS.Body + strlen(NewSMS.Body), d024x20);
    xQueueSendToBack(xModemOutgoingSMSQueue, &NewSMS, portMAX_DELAY);
}

void vControlCommand_ResetCounter(char *Source)
{
    vDebugSpewString("[ctrl] Command 1 - ResetCounter\r\n");
    iShotsSold = 0;
    strncpy(NewSMS.Recipient, Source, 16);
    snprintf(NewSMS.Body, 160, "Okay.");
    xQueueSendToBack(xModemOutgoingSMSQueue, &NewSMS, portMAX_DELAY);
}

void vControlCommand_Ping(char *Source)
{
    vDebugSpewString("[ctrl] Command 2 - Ping\r\n");
    strncpy(NewSMS.Recipient, Source, 16);
    snprintf(NewSMS.Body, 160, "Echo.");
    xQueueSendToBack(xModemOutgoingSMSQueue, &NewSMS, portMAX_DELAY);
}

void vControlParseSMS(char *Source, char *Message)
{
    char Decoded[16];

    if (Message[0] != 'C' ||
        Message[1] != 'T' ||
        Message[2] != 'R' || 
        Message[3] != 'L')
    {
        vDebugSpewString("[ctrl] Invalid header.\r\n");
        return;
    }

    Message += 4;
    uint32_t Length = strlen(Message)/2;
    for (uint32_t i = 0; i < Length; i++)
    {
        char NibbleHigher = Message[i*2];
        char NibbleLower = Message[i*2+1];
        uint8_t Value = qe94ynxf87(NibbleHigher) << 4;
        Value |= qe94ynxf87(NibbleLower);
        Message[i] = Value;
    }
    
    for (uint32_t i = 0; i < Length; i++)
    {
        uint8_t Byte = Message[i] ^ ((x2r34ds[i%6] + i) & 0xFF);
        Byte = Byte ^ 0xDD;
        if (i < 4)
        {
            Byte = Byte ^ 0xDD;
            if (Byte != (i32u89xn[i] ^ 0xDF))
            {
                vDebugSpewString("[ctrl] Security check failed!\r\n");
                return;
            }
        }
        else
        {
            Byte = Byte ^ 0xDD;
            Decoded[i-4] = Byte;
        }
    }

    switch (Decoded[0])
    {
    case 0:
        vControlCommand_Status(Source);
        break;
    case 1:
        vControlCommand_ResetCounter(Source);
        break;
    case 3:
        vControlCommand_Ping(Source);
        break;
    default:
        vDebugSpewString("[ctrl] Unknown command.\r\n");
        break;
    }
}
