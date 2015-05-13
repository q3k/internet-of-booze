#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_it.h>
#include <misc.h>

#define BLINK_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

static void BlinkTask(void *Parameters);
static void SetupHardware(void);

int main(void)
{
    SetupHardware();

    xTaskCreate(BlinkTask, "blnk", configMINIMAL_STACK_SIZE, NULL, BLINK_TASK_PRIORITY, NULL);

    vTaskStartScheduler();
    for( ;; );
}

static void BlinkTask(void *Parameters)
{
    for (;;)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
        vTaskDelay(1000);
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        vTaskDelay(1000);
    }
}

GPIO_InitTypeDef GPIO_InitStructure;
static void SetupHardware(void)
{
    // For FreeRTOS interrupt priority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // Status LED GPIO (PB0)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
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
