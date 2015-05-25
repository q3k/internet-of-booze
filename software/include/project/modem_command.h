#ifndef __PROJECT_MODEM_COMMAND_H__
#define __PROJECT_MODEM_COMMAND_H__

#include <stdint.h>

typedef struct {
    char Body[160];
    char Recipient[16];
} OutgoingSMS_TypeDef;

#endif
