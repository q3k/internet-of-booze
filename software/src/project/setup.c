#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_it.h>
#include <stm32f10x_usart.h>
#include <misc.h>
#include <FreeRTOS.h>

#include "io.h"
#include "debug.h"

static void prvSetupRCC(void)
{
    // Enable AFIO clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // Enable IO clocks
    IO_MOTOR1_RCC_CMD(IO_MOTOR1_RCC, ENABLE);
    IO_MOTOR2_RCC_CMD(IO_MOTOR2_RCC, ENABLE);

    // USART3 Clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
}

GPIO_InitTypeDef GPIO_InitStructure;
static void prvSetupGPIO(void)
{
    // MOTOR1 IO setup
    GPIO_InitStructure.GPIO_Pin = IO_MOTOR1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // MOTOR2 IO setup
    GPIO_InitStructure.GPIO_Pin = IO_MOTOR2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3 Tx GPIO (PB10) setup
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART3 Rx GPIO (PB11) setup
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

static void prvSetupUSART(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &USART_InitStructure);
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART3, ENABLE);

}

void vSetupHardware(void)
{
    // For FreeRTOS interrupt priority
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    // Clocks
    prvSetupRCC();
    // GPIO
    prvSetupGPIO();

    // USART3
    prvSetupUSART();


    vDebugSetup();
}
