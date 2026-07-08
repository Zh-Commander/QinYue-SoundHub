/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_misc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/03/06
* Description        : 该文件提供了所有杂项固件函数（NVIC嵌套向量中断控制器相关配置）
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v30x_misc.h"

/* 全局变量声明 ----------------------------------------------------------------*/
__IO uint32_t NVIC_Priority_Group = 0; /**< 用于存储当前系统的优先级分组设置 */

/*********************************************************************
 * @fn      NVIC_PriorityGroupConfig
 *
 * @brief   配置优先级分组 - 抢占优先级和子优先级（响应优先级）
 *
 * @param   NVIC_PriorityGroup - 指定优先级分组位的长度
 *            NVIC_PriorityGroup_0 - 0位用于抢占优先级
 *                                   3位用于子优先级
 *            NVIC_PriorityGroup_1 - 1位用于抢占优先级
 *                                   2位用于子优先级
 *            NVIC_PriorityGroup_2 - 2位用于抢占优先级
 *                                   1位用于子优先级
 *            NVIC_PriorityGroup_3 - 3位用于抢占优先级
 *                                   0位用于子优先级
 *
 * @return  无
 */
void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup)
{
    NVIC_Priority_Group = NVIC_PriorityGroup; /**< 将优先级分组设置保存到全局变量中 */
}

/*********************************************************************
 * @fn      NVIC_Init
 *
 * @brief   根据NVIC_InitStruct中的指定参数初始化NVIC外设
 *
 * @param   NVIC_InitStruct - 指向NVIC_InitTypeDef结构体的指针，该结构体包含
 *                            指定NVIC外设的配置信息
 *
 * @note    根据中断嵌套配置（INTSYSCR寄存器）的不同，优先级范围有所不同：
 *          中断嵌套禁用(CSR-0x804 bit1 = 0)
 *              NVIC_IRQChannelPreemptionPriority - 范围固定为0
 *              NVIC_IRQChannelSubPriority - 范围从0到7
 *
 *          中断嵌套使能-2级(CSR-0x804 bit1 = 1 bit[3:2] = 1)
 *              NVIC_IRQChannelPreemptionPriority - 范围从0到1
 *              NVIC_IRQChannelSubPriority - 范围从0到3
 *
 *          中断嵌套使能-4级(CSR-0x804 bit1 = 1 bit[3:2] = 2)
 *              NVIC_IRQChannelPreemptionPriority - 范围从0到3
 *              NVIC_IRQChannelSubPriority - 范围从0到1
 *
 *          中断嵌套使能-8级(CSR-0x804 bit1 = 1 bit[3:2] = 3)
 *              NVIC_IRQChannelPreemptionPriority - 范围从0到7
 *              NVIC_IRQChannelSubPriority - 范围固定为0
 *
 * @return  无
 */
void NVIC_Init(NVIC_InitTypeDef *NVIC_InitStruct)
{
/* 根据编译时配置的中断嵌套级别进行条件编译 */
#if (INTSYSCR_INEST == INTSYSCR_INEST_NoEN) /**< 如果系统配置为不支持中断嵌套 */
    if(NVIC_Priority_Group == NVIC_PriorityGroup_0) /**< 检查是否匹配优先级分组0 */
    {
        /* 
         * 在不支持嵌套的情况下，只使用子优先级（响应优先级）
         * NVIC_SetPriority函数要求优先级值左移4位（因为优先级寄存器使用高4位）
         */
        NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, NVIC_InitStruct->NVIC_IRQChannelSubPriority << 4);
    }
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_2Level) /**< 如果系统配置为支持2级中断嵌套 */
    if(NVIC_Priority_Group == NVIC_PriorityGroup_1) /**< 检查是否匹配优先级分组1（1位抢占，2位响应） */
    {
        /* 检查抢占优先级是否在有效范围内（0-1） */
        if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority < 2)
        {
            /* 
             * 抢占优先级左移7位，子优先级左移5位
             * 因为优先级寄存器格式为：| 抢占优先级 | 子优先级 | (保留位)
             * 在2级嵌套配置下：抢占优先级占1位（位7），子优先级占2位（位6:5）
             */
            NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel, 
                (NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << 7) | 
                (NVIC_InitStruct->NVIC_IRQChannelSubPriority << 5));
        }
    }
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_4Level) /**< 如果系统配置为支持4级中断嵌套 */
    if(NVIC_Priority_Group == NVIC_PriorityGroup_2) /**< 检查是否匹配优先级分组2（2位抢占，1位响应） */
    {
        /* 检查抢占优先级是否在有效范围内（0-3） */
        if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority < 4)
        {
            /* 
             * 抢占优先级左移6位，子优先级左移5位
             * 在4级嵌套配置下：抢占优先级占2位（位7:6），子优先级占1位（位5）
             */
            NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel,
                (NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << 6) | 
                (NVIC_InitStruct->NVIC_IRQChannelSubPriority << 5));
        }
    }
#elif (INTSYSCR_INEST == INTSYSCR_INEST_EN_8Level) /**< 如果系统配置为支持8级中断嵌套 */
    if(NVIC_Priority_Group == NVIC_PriorityGroup_3) /**< 检查是否匹配优先级分组3（3位抢占，0位响应） */
    {
        /* 检查抢占优先级是否在有效范围内（0-7） */
        if(NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority < 8)
        {
            /* 
             * 抢占优先级左移5位
             * 在8级嵌套配置下：抢占优先级占3位（位7:5），没有子优先级
             */
            NVIC_SetPriority(NVIC_InitStruct->NVIC_IRQChannel,
                (NVIC_InitStruct->NVIC_IRQChannelPreemptionPriority << 5));
        }
    }
#endif

    /* 根据NVIC_InitStruct中的使能设置，启用或禁用指定的中断通道 */
    if(NVIC_InitStruct->NVIC_IRQChannelCmd != DISABLE)
    {
        NVIC_EnableIRQ(NVIC_InitStruct->NVIC_IRQChannel); /**< 使能指定中断 */
    }
    else
    {
        NVIC_DisableIRQ(NVIC_InitStruct->NVIC_IRQChannel); /**< 禁用指定中断 */
    }
}