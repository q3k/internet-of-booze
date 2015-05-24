#include <FreeRTOS.h>
#include <task.h>

#include "debug.h"
#include "io.h"

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>

#define PINCDZIESIOT 0b1101

void xVend(void)
{
    vDebugSpewString("[acpt] Vending!\r\n");
    GPIO_ResetBits(IO_MOTOR1_PORT, IO_MOTOR1_PIN);
    vTaskDelay(15000);
    GPIO_SetBits(IO_MOTOR1_PORT, IO_MOTOR1_PIN);
}

void xAcceptorCommunicationTask(void *Parameter)
{
    (void) Parameter; 

    vDebugSpewString("[acpt] Task start.\r\n");
    uint8_t PreviousState = 0b1111;
    for (;;)
    {
        uint8_t VEND1 = GPIO_ReadInputDataBit(IO_VEND1_PORT, IO_VEND1_PIN);
        uint8_t VEND2 = GPIO_ReadInputDataBit(IO_VEND2_PORT, IO_VEND2_PIN);
        uint8_t VEND3 = GPIO_ReadInputDataBit(IO_VEND3_PORT, IO_VEND3_PIN);
        uint8_t VEND4 = GPIO_ReadInputDataBit(IO_VEND4_PORT, IO_VEND4_PIN);
        uint8_t State = (VEND1 << 3) | (VEND2 << 2) | (VEND3 << 1) | VEND4;
        if (State != PreviousState)
        {
            if (State == 0b1111 && PreviousState == PINCDZIESIOT)
            {
                vDebugSpewString("[acpt] Vending amount inserted.\r\n");
                xVend();
            }
        }
        PreviousState = State;
        vTaskDelay(10);
    }
}
