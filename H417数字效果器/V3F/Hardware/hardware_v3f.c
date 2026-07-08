#include "hardware_v3f.h"

static void V3F_ScreenUSART5_Init(void);
static void V3F_TickTimer_Init(void);

/*********************************************************************
 * @fn      Hardware_V3f
 *
 * @brief   Initializes V3F side peripherals.
 *
 * @return  none
 */
void Hardware_V3f(void)
{
    printf("V3F Hardware Config\r\n");

    V3F_ScreenUSART5_Init();
    V3F_TickTimer_Init();
}

static void V3F_ScreenUSART5_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_HB2PeriphClockCmd(
        RCC_HB2Periph_AFIO | RCC_HB2Periph_GPIOE,
        ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_USART5, ENABLE);

    /* PE2 = USART5_RX(AF4), PE3 = USART5_TX(AF11). */
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource2, GPIO_AF4);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource3, GPIO_AF11);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl =
        USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART5, &USART_InitStructure);

    while (USART_GetFlagStatus(USART5, USART_FLAG_RXNE) != RESET)
    {
        (void)USART_ReceiveData(USART5);
    }

    USART_ITConfig(USART5, USART_IT_RXNE, ENABLE);

    NVIC_SetAllocateIRQ(USART5_IRQn, Core_ID_V3F);
    NVIC_SetPriority(USART5_IRQn, 0x20);
    NVIC_EnableIRQ(USART5_IRQn);

    USART_Cmd(USART5, ENABLE);
}

static void V3F_TickTimer_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    uint32_t timer_clk = HCLKClock;
    uint32_t prescaler;

    if (timer_clk == 0)
    {
        timer_clk = SystemCoreClock;
    }

    if (timer_clk < 1000000u)
    {
        timer_clk = 1000000u;
    }

    prescaler = (timer_clk / 1000000u) - 1u;

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_TIM6, ENABLE);

    TIM_Cmd(TIM6, DISABLE);

    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t)prescaler;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = 1000u - 1u;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    NVIC_SetAllocateIRQ(TIM6_IRQn, Core_ID_V3F);
    NVIC_SetPriority(TIM6_IRQn, 0x40);
    NVIC_EnableIRQ(TIM6_IRQn);

    TIM_Cmd(TIM6, ENABLE);
}
