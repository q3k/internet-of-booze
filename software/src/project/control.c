#include <stdint.h>
#include <string.h>

#include "debug.h"

char *ControlKey = "secret";
char *ControlMagic = "c29j";

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

void vControlCommand_Status(char *Source)
{
    (void) Source;
    vDebugSpewString("[ctrl] Command 0 - Status\r\n");
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
        uint8_t Value = prvASCIIToNibble(NibbleHigher) << 4;
        Value |= prvASCIIToNibble(NibbleLower);
        Message[i] = Value;
    }
    
    for (uint32_t i = 0; i < Length; i++)
    {
        uint8_t Byte = Message[i] ^ ControlKey[i%6];
        if (i < 4)
        {
            if (Byte != ControlMagic[i])
            {
                vDebugSpewString("[ctrl] Security check failed!\r\n");
                return;
            }
        }
        else
        {
            Decoded[i-4] = Byte;
        }
    }

    switch (Decoded[0])
    {
    case 0:
        vControlCommand_Status(Source);
        break;
    default:
        vDebugSpewString("[ctrl] Unknown command.\r\n");
        break;
    }
}
