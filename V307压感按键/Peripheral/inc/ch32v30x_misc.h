/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_misc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/06
* Description        : 此文件包含了所有杂项固件库函数的函数原型
*                      
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/  
#ifndef __CH32V30X_MISC_H
#define __CH32V30X_MISC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* CSR_INTSYSCR_INEST_definition */
#define INTSYSCR_INEST_NoEN        0x00   /* 中断嵌套禁用 (CSR-0x804 bit1 = 0) */
#define INTSYSCR_INEST_EN_2Level   0x01   /* 中断嵌套使能 - 2级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 1) */
#define INTSYSCR_INEST_EN_4Level   0x02   /* 中断嵌套使能 - 4级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 2) */
#define INTSYSCR_INEST_EN_8Level   0x03   /* 中断嵌套使能 - 8级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 3) */

/* 检查启动文件(.S)中CSR(0x804)的配置
 *   中断嵌套使能-8级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 3)
 *     优先级 - bit[7:5] - 抢占优先级
 *              bit[4:0] - 保留位
 *   中断嵌套使能-4级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 2)
 *     优先级 - bit[7:6] - 抢占优先级
 *              bit[5]   - 子优先级
 *              bit[4:0] - 保留位
 *   中断嵌套使能-2级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 1)
 *     优先级 - bit[7]   - 抢占优先级
 *              bit[6:5] - 子优先级
 *              bit[4:0] - 保留位
 *   中断嵌套禁用 (CSR-0x804 bit1 = 0)
 *     优先级 - bit[7:5] - 子优先级
 *              bit[4:0] - 保留位
 */

#ifndef INTSYSCR_INEST
#define INTSYSCR_INEST   INTSYSCR_INEST_EN_4Level  /* 默认使用4级中断嵌套配置 */
#endif

/* NVIC 初始化结构体定义
 *   中断嵌套禁用 (CSR-0x804 bit1 = 0)
 *     NVIC_IRQChannelPreemptionPriority - 范围是0（没有抢占优先级）
 *     NVIC_IRQChannelSubPriority - 范围从0到7（8个子优先级级别）
 *
 *   中断嵌套使能-2级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 1)
 *     NVIC_IRQChannelPreemptionPriority - 范围从0到1（2个抢占优先级级别）
 *     NVIC_IRQChannelSubPriority - 范围从0到3（4个子优先级级别）
 *
 *   中断嵌套使能-4级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 2)
 *     NVIC_IRQChannelPreemptionPriority - 范围从0到3（4个抢占优先级级别）
 *     NVIC_IRQChannelSubPriority - 范围从0到1（2个子优先级级别）
 *
 *   中断嵌套使能-8级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 3)
 *     NVIC_IRQChannelPreemptionPriority - 范围从0到7（8个抢占优先级级别）
 *     NVIC_IRQChannelSubPriority - 范围是0（没有子优先级）
 */
typedef struct
{
    uint8_t NVIC_IRQChannel;                    /* 指定要配置的中断通道，参考IRQn_Type枚举 */
    uint8_t NVIC_IRQChannelPreemptionPriority;  /* 设置中断的抢占优先级 */
    uint8_t NVIC_IRQChannelSubPriority;         /* 设置中断的子优先级 */
    FunctionalState NVIC_IRQChannelCmd;         /* 使能或禁用指定的中断通道 */
} NVIC_InitTypeDef;

/* 优先级分组配置定义 */
#if (INTSYSCR_INEST == INTSYSCR_INEST_NoEN)
#define NVIC_PriorityGroup_0           ((uint32_t)0x00) /* 中断嵌套禁用 (CSR-0x804 bit1 = 0) */
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_2Level)
#define NVIC_PriorityGroup_1           ((uint32_t)0x01) /* 中断嵌套使能 - 2级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 1) */
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_8Level)
#define NVIC_PriorityGroup_3           ((uint32_t)0x03) /* 中断嵌套使能 - 8级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 3) */
#else
#define NVIC_PriorityGroup_2           ((uint32_t)0x02) /* 中断嵌套使能 - 4级嵌套 (CSR-0x804 bit1 = 1 bit[3:2] = 2) */
#endif

void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);  /* 配置优先级分组 */
void NVIC_Init(NVIC_InitTypeDef *NVIC_InitStruct);           /* 根据指定参数初始化NVIC */

#ifdef __cplusplus
}
#endif

#endif