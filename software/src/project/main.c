#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_it.h>
#include <misc.h>

#include "setup.h"
#include "modem.h"

#define BLINK_TASK_PRIORITY (tskIDLE_PRIORITY + 1)
#define MODEM_TASK_PRIORITY (tskIDLE_PRIORITY + 2)

#define MODEM_QUEUE_SIZE 256

static void prvBlinkTask(void *Parameter);

int main(void)
{
    vSetupHardware();

    /// Create queues
    // Queue from USART receive ISR to ModemCommunicationTask
    xModemReceiveQueue = xQueueCreate(MODEM_QUEUE_SIZE, sizeof(char));
    // Queue from ModemCommunicationTask to USART TXE interrupt
    xModemTransmitQueue = xQueueCreate(MODEM_QUEUE_SIZE, sizeof(char));

    /// Create tasks
    // Blinky task!
    xTaskCreate(prvBlinkTask, (const signed char*)"blnk", configMINIMAL_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);
    // Modem communication task
    xTaskCreate(xModemCommunicationTask, (const signed char*)"mdmc", configMINIMAL_STACK_SIZE, NULL, MODEM_TASK_PRIORITY, NULL);

    vTaskStartScheduler();
    for( ;; );
}

static void prvBlinkTask(void *Parameter)
{
    (void) Parameter;

    for (;;)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
        vTaskDelay(1000);
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        vTaskDelay(1000);
    }
}

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName)
{
    // Use these for debugging
    (void) pxTask;
    (void) pcTaskName;

    for( ;; );
}

void assert_failed(unsigned char *pucFile, unsigned long ulLine)
{
    (void) pucFile;
    (void) ulLine;

    for( ;; );
}

void vApplicationIdleHook(void)
{

}

void vApplicationMallocFailedHook(void)
{
    for (;;)
    {
    }
}

void _exit(void)
{
    for (;;)
    {
    }
}
