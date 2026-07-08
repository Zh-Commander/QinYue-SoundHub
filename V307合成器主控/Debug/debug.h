/********************************** (C) COPYRIGHT  *******************************
* File Name          : debug.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for UART
*                      Printf , Delay functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stdio.h"
#include "ch32v30x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdint.h>

/* UART Printf Definition */
#define DEBUG_UART1    1
#define DEBUG_UART2    2
#define DEBUG_UART3    3

#define DEBUG_RX_BUFFER_SIZE    128
#define DEBUG_RX_QUEUE_LENGTH   64

/* DEBUG UATR Definition */
#ifndef DEBUG
#define DEBUG   DEBUG_UART1
#endif

/* SDI Printf Definition */
#define SDI_PR_CLOSE   0
#define SDI_PR_OPEN    1

#ifndef SDI_PRINT
#define SDI_PRINT   SDI_PR_CLOSE
#endif


void Delay_Init(void);
void Delay_Us (uint32_t n);
void Delay_Ms (uint32_t n);
void USART_Printf_Init(uint32_t baudrate);
void SDI_Printf_Enable(void);
void Debug_Printf_Init(void);
void Debug_Printf(const char *format, ...);
int Debug_ReadChar(uint8_t *ch);
BaseType_t Debug_ReadCharBlocking(uint8_t *ch, TickType_t timeout_ticks);
void Debug_UART_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif 



