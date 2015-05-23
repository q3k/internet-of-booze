#ifndef __PROJECT_MODEM_COMMAND_H__
#define __PROJECT_MODEM_COMMAND_H__

#include <stdint.h>

typedef struct {
    char Request[32];
    uint32_t Identifier;
} ModemRequest_TypeDef;

typedef struct {
    char Reply[256];
    uint32_t Identifier;
} ModemReply_TypeDef;

#endif
