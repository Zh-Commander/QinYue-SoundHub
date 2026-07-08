/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32h417_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32h417_it.h"
#include "config.h"
#include "dsp_main.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

static void Audio_ProcessHalf(uint32_t half);

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
    while (1)
    {
    }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
    NVIC_SystemReset();
    while (1)
    {
    }
}

void DMA2_Channel1_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA2, DMA2_IT_HT1) != RESET)
    {
        DMA_ClearITPendingBit(DMA2, DMA2_IT_HT1);

        /* RX buffer first half ready */
        Audio_ProcessHalf(0);
    }

    if (DMA_GetITStatus(DMA2, DMA2_IT_TC1) != RESET)
    {
        DMA_ClearITPendingBit(DMA2, DMA2_IT_TC1);

        /* RX buffer second half ready */
        Audio_ProcessHalf(1);
    }

    if (DMA_GetITStatus(DMA2, DMA2_IT_TE1) != RESET)
    {
        DMA_ClearITPendingBit(DMA2, DMA2_IT_TE1);

        /* TODO: error counter */
    }
}

static void Audio_ProcessHalf(uint32_t half)
{
    // 空载CPU占用率3%,主要是用于数据编解码
    GPIO_SetBits(GPIOB,GPIO_Pin_3); // 用于CPU占用率计算

    uint32_t offset = 0;

    if (half != 0)
    {
        offset = I2S_BUFFER_DMA_CNT / 2;
    }

    Audio_DSP_Main(I2S_RX_DMA_Buffer + offset, I2S_TX_DMA_Buffer + offset);
    
    GPIO_ResetBits(GPIOB,GPIO_Pin_3); // 用于CPU占用率计算
}