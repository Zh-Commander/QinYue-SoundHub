/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32v30x_it.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/03/06
 * Description        : Main Interrupt Service Routines.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32v30x_it.h"
#include "key.h"
#include "system.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM4_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        if (isAdjustMode && isAdjustFinished == false)
        {
            Key_Scan();
            Adjust_Process();
            return;
        }
        if (USART_GetFlagStatus(UART5, USART_FLAG_RXNE) == SET)
        {
            uint8_t Rx_CMD = (uint8_t)(USART_ReceiveData(UART5) & 0xFF);
            if (Rx_CMD == UART5_SCANCMD_EXIT_SCAN)
            {
                // while (DMA_GetFlagStatus(DMA2_FLAG_TC4) == RESET);  // 等待上次DMA传输完成
                Scan_Disable();
                return;
            }
        }
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        Key_Scan();
        Data_Process();
        Send_Data();
    }
}

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
