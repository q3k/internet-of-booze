#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_it.h>
#include <misc.h>

static void prvSetupRCC(void)
{
    // Status LED GPIO (PB0) and USART3 GPIO Clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    // USART3 Clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
}

GPIO_InitTypeDef GPIO_InitStructure;
static void prvSetupGPIO(void)
{
    // Status LED GPIO (PB0) setup
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3 Tx GPIO (PB10) setup
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3 Rx GPIO (PB11) setup
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void vSetupHardware(void)
{
    // For FreeRTOS interrupt priority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // Clocks
    prvSetupRCC();
    // GPIO
    prvSetupGPIO();
}
