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
#include "modem_command.h"
#include "io.h"
#include "debug.h"
#include "acceptor.h"

#define BLINK_TASK_PRIORITY (tskIDLE_PRIORITY + 3)
#define MODEM_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define ACCEPTOR_TASK_PRIORITY (tskIDLE_PRIORITY + 1)

#define MODEM_QUEUE_SIZE 64

static void prvBlinkTask(void *Parameter);

int main(void)
{
    vSetupHardware();

    /// Create queues
    // Queue from USART receive ISR to ModemCommunicationTask
    xModemReceiveQueue = xQueueCreate(MODEM_QUEUE_SIZE, sizeof(char));
    // Queue from ModemCommunicationTask to USART TXE interrupt
    xModemTransmitQueue = xQueueCreate(MODEM_QUEUE_SIZE, sizeof(char));
    // Queue of outgoing messages for baseband to send
    xModemOutgoingSMSQueue = xQueueCreate(3, sizeof(OutgoingSMS_TypeDef));

    /// Create tasks
    // Blinky task!
    xTaskCreate(prvBlinkTask, (const signed char*)"blnk", 128, NULL, BLINK_TASK_PRIORITY, NULL);
    // Modem communication task
    xTaskCreate(xModemCommunicationTask, (const signed char*)"mdmc", 128, NULL, MODEM_TASK_PRIORITY, NULL);

    // Acceptor task
    xTaskCreate(xAcceptorCommunicationTask, (const signed char*)"acpt", 128, NULL, ACCEPTOR_TASK_PRIORITY, NULL);

    vTaskStartScheduler();
    for( ;; );
}

static void prvBlinkTask(void *Parameter)
{
    (void) Parameter;

    vDebugSpewString("[blnk] Task started.\r\n");
    for (;;)
    {
        GPIO_ResetBits(IO_MOTOR1_PORT, GPIO_Pin_9);
        vTaskDelay(1000);

        GPIO_SetBits(IO_MOTOR1_PORT, GPIO_Pin_9);
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
