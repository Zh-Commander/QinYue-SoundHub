/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/05
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_it.h"
#include "ch32v30x.h"
#include "debug.h"
#include "..\\Component\\dynamic\\dynamic_audio.h"
#include "..\\Component\\midi\\midi_mode.h"
#include "..\\Component\\SGL\\App\\sgl_demo_app.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART1_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void USART3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void UART7_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA1_Channel5_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void DMA2_Channel9_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

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

void USART1_IRQHandler(void)
{
#if(DEBUG == DEBUG_UART1)
    Debug_UART_IRQHandler();
#endif
}

void USART2_IRQHandler(void)
{
#if(DEBUG == DEBUG_UART2)
    Debug_UART_IRQHandler();
#endif
}

void USART3_IRQHandler(void)
{
#if(DEBUG == DEBUG_UART3)
    Debug_UART_IRQHandler();
#endif
}

void UART7_IRQHandler(void)
{
    if(dynamic_audio_is_running() != 0U)
    {
        DynamicAudio_Uart7RxIsr();
    }
    else if(app_mode_is_midi_uart_running() != 0U)
    {
        MidiMode_Uart7RxIsr();
    }
    else
    {
        if((UART7->STATR & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)) != 0U)
        {
            volatile uint32_t temp = UART7->STATR;
            temp = UART7->DATAR;
            (void)temp;
        }
        else if((UART7->STATR & USART_FLAG_RXNE) != 0U)
        {
            (void)UART7->DATAR;
        }
    }
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   SGL tick interrupt handler.
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
        SGL_DemoApp_TickISR();
    }
}

/*********************************************************************
 * @fn      TIM3_IRQHandler
 *
 * @brief   Touch scan interrupt handler.
 *
 * @return  none
 */
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        SGL_DemoApp_TouchScanISR();
    }
}

/*********************************************************************
 * @fn      DMA1_Channel3_IRQHandler
 *
 * @brief   LCD DMA flush complete interrupt handler.
 *
 * @return  none
 */
void DMA1_Channel3_IRQHandler(void)
{
    SGL_DemoApp_LcdDmaISR();
}

void DMA1_Channel5_IRQHandler(void)
{
    DynamicAudio_AudioDmaTxIsr();
}

void DMA2_Channel9_IRQHandler(void)
{
    if(dynamic_audio_is_running() != 0U)
    {
        DynamicAudio_Uart7DmaRxIsr();
    }
    else if(app_mode_is_midi_uart_running() != 0U)
    {
        MidiMode_Uart7DmaRxIsr();
    }
    else
    {
        DMA_ClearITPendingBit(DMA2_IT_TC9);
        DMA_ClearFlag(DMA2_FLAG_GL9);
    }
}
