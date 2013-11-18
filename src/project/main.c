/* Standard includes. */
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Library includes. */
#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <misc.h>
#include "stm32f10x_it.h"

/* Task priorities. */
#define mainECHO_TASK_PRIORITY                ( tskIDLE_PRIORITY + 1 )

/* COM port and baud rate used by the echo task. */
#define mainCOM0                            ( 0 )
#define mainBAUD_RATE                        ( 115200 )

static void prvUSARTEchoTask( void *pvParameters );
static void prvSetupHardware(void);

/*-----------------------------------------------------------*/

int main( void )
{
    prvSetupHardware();

    /* Create the 'echo' task, which is also defined within this file. */
    xTaskCreate( prvUSARTEchoTask, ( signed char * ) "Echo", configMINIMAL_STACK_SIZE, NULL, mainECHO_TASK_PRIORITY, NULL );

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task.  The idle task is created within vTaskStartScheduler(). */
    for( ;; );
}
/*-----------------------------------------------------------*/

/* Described at the top of this file. */
void prvUSARTEchoTask( void *pvParameters )
{
    for (;;)
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
        vTaskDelay(1000);
        GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        vTaskDelay(1000);
    }
}
/*-----------------------------------------------------------*/

GPIO_InitTypeDef GPIO_InitStructure;
extern void SystemInit(void);
static void prvSetupHardware( void )
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
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
    /* This function will get called if a task overflows its stack.   If the
    parameters are corrupt then inspect pxCurrentTCB to find which was the
    offending task. */

    ( void ) pxTask;
    ( void ) pcTaskName;

    for( ;; );
}
/*-----------------------------------------------------------*/

void assert_failed( unsigned char *pucFile, unsigned long ulLine )
{
    ( void ) pucFile;
    ( void ) ulLine;

    for( ;; );
}

void vApplicationIdleHook(void)
{

}

void vApplicationMallocFailedHook( void )
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
