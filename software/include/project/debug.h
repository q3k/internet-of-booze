#ifndef __PROJECT_DEBUG_H__
#define __PROJECT_DEBUG_H__

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>

#include "config.h"

void vDebugSetup(void);
void vDebugSpewChar(char c);
void vDebugSpewString(const char *string);

#endif
