/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.

*/

#include "debug.h"
#include "system.h"
#include "system_config.h"
#include "param.h"
#include "key.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_rcc.h"

// ADC采样+DMA搬运初始化
void Sys_DMA1_Init(void);
void Sys_ADC_Init(void);
void Sys_TIM4_Init(void);

// 与主芯片通信（SPI）初始化
void Sys_DMA2_Init(void);
void Sys_UART5_Init(void);
void Sys_UART7_Init(void);

// ADC校准值
// int16_t Calibrattion_Val1,Calibrattion_Val2 = 0;

/* Global typedef */

/* Global define */

/* Global Variable */


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    SysTick_Init();
    USART_Printf_Init(115200);  // 调试输出波特率
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("This is printf example\r\n");

    // User Init
    // === 初始化系统参数 ===
    InitParam();  // 从芯片内置Flash中读取系统参数

    // === 按键扫描、计算力度部分初始化 ===
    Sys_ADC_Init();
    Sys_DMA1_Init();
    Key_Init();       // 按键初始化
    Sys_TIM4_Init();  // 配置和使能定时器，开始工作

    // === 收发部分初始化 ===
    Sys_UART5_Init();
    Sys_UART7_Init();
    Sys_DMA2_Init();

    System_Init();

    while (1)
    {
        // User Loop
        System_Loop();
    }
}

void Sys_DMA1_Init(void)
{
    DMA1->INTFCR = DMA1_IT_GL1;                         // 清除DMA1标志

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能RCC时钟

    DMA_DeInit(DMA1_Channel1);                          // 复位DMA1通道

    DMA_InitTypeDef DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->RDATAR;           // ADC寄存器地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Collection_Buffer;           // 内存地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                       // ADC到内存
    DMA_InitStructure.DMA_BufferSize = (KEY_NUM + 1) / 2 + 1;                // (KEY_NUM+1)/2个字
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         // ADC地址不自增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                  // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;  // 传输单位一个字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;          // 传输单位一个字
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                          // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                  // 最高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                             // 禁用内存到内存模式
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);  // 使能DMA1
}

void Sys_ADC_Init(void)
{
    // === GPIO配置 ===
    // 使能RCC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;      // 模拟输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // GPIO速度

    // PA0-PA7: 8个ADC通道(0-7)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                  GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PB0-PB1: 2个ADC通道(8-9)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PC0-PC5: 6个ADC通道(10-15)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
                                  GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // === 双ADC规则采样配置 ===
    // 使能RCC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div4);  // 144/4 = 36 > 14，若不稳定可调低频率

    // 复位ADC
    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);

    // 配置ADC参数
    ADC_InitTypeDef ADC_InitStructure = {0};
    ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult;                     // 规则同时采样模式
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;                         // 扫描模式使能
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  // 单次转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = (KEY_NUM + 1) / 2 + 1;          // 8个规则通道
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;       // 禁用输出缓冲
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;                               // PGA增益1x

    // 初始化ADC1
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_BufferCmd(ADC1, DISABLE);  // Disable buffer

    // 初始化ADC2（使用相同的配置）
    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);  // 使能ADC软件触发（仅ADC2需要）？
    ADC_Cmd(ADC2, ENABLE);
    ADC_BufferCmd(ADC2, DISABLE);            // Disable buffer

    // 配置规则通道
    for (int i = 0; i < KEY_NUM; i += 2)
    {
        // ADC1采样偶数通道：0,2,4,6,8,10,12,14
        ADC_RegularChannelConfig(ADC1, i, i / 2 + 1, ADC_SampleTime_28Cycles5);
        // ADC2采样奇数通道：1,3,5,7,9,11,13,15
        ADC_RegularChannelConfig(ADC2, i + 1, i / 2 + 1, ADC_SampleTime_28Cycles5);
    }
    ADC_RegularChannelConfig(ADC1, 17, KEY_NUM / 2 + 1, ADC_SampleTime_28Cycles5);  // VREFINT内部参考电压通道采样，用于校准
    ADC_RegularChannelConfig(ADC2, 17, KEY_NUM / 2 + 1, ADC_SampleTime_28Cycles5);  // VREFINT内部参考电压通道采样，用于校准
    ADC_TempSensorVrefintCmd(ENABLE);                                               // 内部参考电压使能


    // 延时1us（ADC校准必须在上电后至少两个ADC时钟周期后进行）
    Delay_Us(10);
    // ADC1校准
    ADC_BufferCmd(ADC1, DISABLE);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
    // Calibrattion_Val1 = Get_CalibrationValue(ADC1); // 获取校准偏移值（int16_t），理论上测量结果+偏移值能得到校准结果

    // ADC2校准
    ADC_BufferCmd(ADC2, DISABLE);
    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2));
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2));
    // Calibrattion_Val2 = Get_CalibrationValue(ADC2); // 获取校准偏移值（int16_t），理论上测量结果+偏移值能得到校准结果
}

void Sys_TIM4_Init(void)
{
    // 使能RCC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    // 计算预分频值和自动重装载值
    uint32_t period_tick = SAMPLING_INTERVAL_TIME * (SystemCoreClock / 1000000);
    uint16_t freq_div = (period_tick >> 15);                // 计算预分频
    uint16_t period_temp = (period_tick / (freq_div + 1));  // 计算自动重装载值

    // 配置TIM4计时器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};
    TIM_TimeBaseStructure.TIM_Period = period_temp;
    TIM_TimeBaseStructure.TIM_Prescaler = freq_div;              // 分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM向上计数模式
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;             // 重复计数器设置为0
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              // 根据指定的参数初始化TIMx的时间基数单位

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  // 使能指定的TIM中断,允许更新中断

    // 中断设置
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = TIM4_IRQn;            // TIM4 中断
    nvic.NVIC_IRQChannelPreemptionPriority = 5;  // 抢占优先级
    nvic.NVIC_IRQChannelSubPriority = 5;         // 响应优先级
    nvic.NVIC_IRQChannelCmd = ENABLE;            // IRQ 通道使能
    NVIC_Init(&nvic);

    // 使能定时器
    // TIM_Cmd(TIM4, ENABLE);
}

void Sys_DMA2_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);  // 使能RCC时钟

    // === UART5 ===
    DMA_DeInit(DMA2_Channel4);  // 复位DMA2通道4，UART5TX，与主芯片通信
    DMA_InitTypeDef DMA_InitStructure1;
    DMA_StructInit(&DMA_InitStructure1);
    DMA_InitStructure1.DMA_PeripheralBaseAddr = (u32)(&UART5->DATAR);         // ADC寄存器地址
    DMA_InitStructure1.DMA_MemoryBaseAddr = (u32)KeyData_Tx_Buffer;           // 内存地址
    DMA_InitStructure1.DMA_DIR = DMA_DIR_PeripheralDST;                       // 内存到UART5数据寄存器
    DMA_InitStructure1.DMA_BufferSize = 2 + KEY_NUM;                          // 2 + KEY_NUM个字节
    DMA_InitStructure1.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         // 外设地址不自增
    DMA_InitStructure1.DMA_MemoryInc = DMA_MemoryInc_Enable;                  // 内存地址递增
    DMA_InitStructure1.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 传输单位一个字节
    DMA_InitStructure1.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // 传输单位一个字节
    DMA_InitStructure1.DMA_Mode = DMA_Mode_Normal;                            // 正常模式
    DMA_InitStructure1.DMA_Priority = DMA_Priority_VeryHigh;                  // 最高优先级
    DMA_InitStructure1.DMA_M2M = DMA_M2M_Disable;                             // 禁用内存到内存模式
    DMA_Init(DMA2_Channel4, &DMA_InitStructure1);
    DMA_Cmd(DMA2_Channel4, ENABLE);

    // === UART7 ===
    DMA_DeInit(DMA2_Channel8);  // 复位DMA2通道4，UART7TX，与V003通信
    DMA_InitTypeDef DMA_InitStructure2;
    DMA_StructInit(&DMA_InitStructure2);
    DMA_InitStructure2.DMA_PeripheralBaseAddr = (u32)(&UART7->DATAR);         // ADC寄存器地址
    DMA_InitStructure2.DMA_MemoryBaseAddr = (u32)KeyData_Tx_Buffer;           // 内存地址
    DMA_InitStructure2.DMA_DIR = DMA_DIR_PeripheralDST;                       // 内存到UART5数据寄存器
    DMA_InitStructure2.DMA_BufferSize = 2;                                    // 前 2 个字节
    DMA_InitStructure2.DMA_PeripheralInc = DMA_PeripheralInc_Disable;         // 外设地址不自增
    DMA_InitStructure2.DMA_MemoryInc = DMA_MemoryInc_Enable;                  // 内存地址递增
    DMA_InitStructure2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 传输单位一个字节
    DMA_InitStructure2.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;          // 传输单位一个字节
    DMA_InitStructure2.DMA_Mode = DMA_Mode_Normal;                            // 正常模式
    DMA_InitStructure2.DMA_Priority = DMA_Priority_High;                      // 高优先级
    DMA_InitStructure2.DMA_M2M = DMA_M2M_Disable;                             // 禁用内存到内存模式
    DMA_Init(DMA2_Channel8, &DMA_InitStructure2);

    DMA_Cmd(DMA2_Channel8, ENABLE);  // 启用DMA2_Channel8
}

void Sys_UART5_Init(void)
{
    // 使能RCC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

    // GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // TX引脚配置为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // RX引脚配置为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // USART参数配置
    USART_InitTypeDef USART_InitStructure = {0};

    USART_InitStructure.USART_BaudRate = 1843200;  // 最大9Mhz
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    // 初始化USART
    USART_Init(UART5, &USART_InitStructure);

    // 中断设置(不再使用)
    /* NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = UART5_IRQn;          // UART5 中断
    nvic.NVIC_IRQChannelPreemptionPriority = 3;  // 抢占优先级
    nvic.NVIC_IRQChannelSubPriority = 3;         // 响应优先级
    nvic.NVIC_IRQChannelCmd = ENABLE;            // IRQ 通道使能
    NVIC_Init(&nvic); */

    // 使能中断
    // USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); // 接收中断

    // 启动USART
    USART_Cmd(UART5, ENABLE);
}

void Sys_UART7_Init(void)
{
    // 使能RCC时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    // GPIO配置
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // TX引脚配置为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // RX引脚配置为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // USART参数配置
    USART_InitTypeDef USART_InitStructure = {0};

    USART_InitStructure.USART_BaudRate = 3000000;  // V003最大3Mhz
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

    // 初始化USART
    USART_Init(UART7, &USART_InitStructure);

    // 启动USART
    USART_Cmd(UART7, ENABLE);
}
