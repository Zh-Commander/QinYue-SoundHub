/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_it.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains the headers of the interrupt handlers.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V30x_IT_H
#define __CH32V30x_IT_H

void USART1_IRQHandler(void);
void USART2_IRQHandler(void);
void USART3_IRQHandler(void);
void UART7_IRQHandler(void);
void DMA1_Channel5_IRQHandler(void);
void DMA2_Channel9_IRQHandler(void);

void DynamicAudio_AudioDmaTxIsr(void);
void DynamicAudio_Uart7DmaRxIsr(void);
void DynamicAudio_Uart7RxIsr(void);
void MidiMode_Uart7DmaRxIsr(void);
void MidiMode_Uart7RxIsr(void);

#endif /* __CH32V30x_IT_H */
