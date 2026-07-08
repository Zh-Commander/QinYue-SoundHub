/********************************** (C) COPYRIGHT  *******************************
* 文件名称          : ch32v30x_exti.c
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 该文件提供了所有EXTI(外部中断/事件控制器)固件函数
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）和二进制文件仅用于
*       南京沁恒微电子制造的单片机
*******************************************************************************/
#include "ch32v30x_exti.h"

/* 无中断线选择 */
#define EXTI_LINENONE    ((uint32_t)0x00000)  /**< 用于初始化时表示没有选择任何EXTI线 */

/*********************************************************************
 * @fn      EXTI_DeInit
 *
 * @brief   将EXTI外设寄存器初始化为默认复位值
 *
 * @return  无
 */
void EXTI_DeInit(void)
{
    EXTI->INTENR = 0x00000000;  /**< 禁用所有中断使能 */
    EXTI->EVENR = 0x00000000;   /**< 禁用所有事件使能 */
    EXTI->RTENR = 0x00000000;   /**< 禁用所有上升沿触发 */
    EXTI->FTENR = 0x00000000;   /**< 禁用所有下降沿触发 */
    EXTI->INTFR = 0x000FFFFF;   /**< 清除所有中断挂起标志（写1清除） */
}

/*********************************************************************
 * @fn      EXTI_Init
 *
 * @brief   根据EXTI_InitStruct中指定的参数初始化EXTI外设
 *
 * @param   EXTI_InitStruct - 指向EXTI_InitTypeDef结构的指针
 *
 * @return  无
 */
void EXTI_Init(EXTI_InitTypeDef *EXTI_InitStruct)
{
    uint32_t tmp = 0;

    tmp = (uint32_t)EXTI_BASE;  /**< 获取EXTI基地址 */
    if(EXTI_InitStruct->EXTI_LineCmd != DISABLE)  /**< 如果EXTI线使能 */
    {
        /* 先禁用相关配置，然后重新配置 */
        EXTI->INTENR &= ~EXTI_InitStruct->EXTI_Line;  /**< 清除中断使能位 */
        EXTI->EVENR &= ~EXTI_InitStruct->EXTI_Line;   /**< 清除事件使能位 */
        tmp += EXTI_InitStruct->EXTI_Mode;            /**< 计算模式寄存器地址偏移 */
        *(__IO uint32_t *)tmp |= EXTI_InitStruct->EXTI_Line;  /**< 设置中断或事件模式 */
        
        EXTI->RTENR &= ~EXTI_InitStruct->EXTI_Line;  /**< 清除上升沿触发 */
        EXTI->FTENR &= ~EXTI_InitStruct->EXTI_Line;  /**< 清除下降沿触发 */
        
        if(EXTI_InitStruct->EXTI_Trigger == EXTI_Trigger_Rising_Falling)  /**< 如果是上升沿和下降沿触发 */
        {
            EXTI->RTENR |= EXTI_InitStruct->EXTI_Line;  /**< 使能上升沿触发 */
            EXTI->FTENR |= EXTI_InitStruct->EXTI_Line;  /**< 使能下降沿触发 */
        }
        else  /**< 如果是单一沿触发 */
        {
            tmp = (uint32_t)EXTI_BASE;                  /**< 重新获取基地址 */
            tmp += EXTI_InitStruct->EXTI_Trigger;       /**< 计算触发寄存器地址偏移 */
            *(__IO uint32_t *)tmp |= EXTI_InitStruct->EXTI_Line;  /**< 设置触发沿 */
        }
    }
    else  /**< 如果EXTI线禁用 */
    {
        tmp += EXTI_InitStruct->EXTI_Mode;            /**< 计算模式寄存器地址偏移 */
        *(__IO uint32_t *)tmp &= ~EXTI_InitStruct->EXTI_Line;  /**< 清除中断或事件模式 */
    }
}

/*********************************************************************
 * @fn      EXTI_StructInit
 *
 * @brief   将EXTI_InitStruct的每个成员填充为其复位值
 *
 * @param   EXTI_InitStruct - 指向EXTI_InitTypeDef结构的指针
 *
 * @return  无
 */
void EXTI_StructInit(EXTI_InitTypeDef *EXTI_InitStruct)
{
    EXTI_InitStruct->EXTI_Line = EXTI_LINENONE;        /**< 没有选择EXTI线 */
    EXTI_InitStruct->EXTI_Mode = EXTI_Mode_Interrupt;  /**< 默认为中断模式 */
    EXTI_InitStruct->EXTI_Trigger = EXTI_Trigger_Falling;  /**< 默认为下降沿触发 */
    EXTI_InitStruct->EXTI_LineCmd = DISABLE;           /**< 默禁止能 */
}

/*********************************************************************
 * @fn      EXTI_GenerateSWInterrupt
 *
 * @brief   生成软件中断
 *
 * @param   EXTI_Line - 指定要生成软件中断的EXTI线
 *
 * @return  无
 */
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line)
{
    EXTI->SWIEVR |= EXTI_Line;  /**< 设置软件中断事件寄存器，触发软件中断 */
}

/*********************************************************************
 * @fn      EXTI_GetFlagStatus
 *
 * @brief   检查指定的EXTI线标志是否置位
 *
 * @param   EXTI_Line - 指定要检查的EXTI线
 *
 * @return  EXTI_Line的新状态（SET或RESET）
 */
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line)
{
    FlagStatus bitstatus = RESET;
    if((EXTI->INTFR & EXTI_Line) != (uint32_t)RESET)  /**< 检查中断标志寄存器对应位 */
    {
        bitstatus = SET;  /**< 标志置位 */
    }
    else
    {
        bitstatus = RESET;  /**< 标志未置位 */
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      EXTI_ClearFlag
 *
 * @brief   清除EXTI线的挂起标志
 *
 * @param   EXTI_Line - 指定要清除标志的EXTI线
 *
 * @return  无
 */
void EXTI_ClearFlag(uint32_t EXTI_Line)
{
    EXTI->INTFR = EXTI_Line;  /**< 写1清除对应中断标志位 */
}

/*********************************************************************
 * @fn      EXTI_GetITStatus
 *
 * @brief   检查指定的EXTI线中断是否发生（同时检查使能和标志位）
 *
 * @param   EXTI_Line - 指定要检查的EXTI线
 *
 * @return  EXTI_Line的新状态（SET或RESET）
 */
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line)
{
    ITStatus bitstatus = RESET;
    uint32_t enablestatus = 0;

    enablestatus = EXTI->INTENR & EXTI_Line;  /**< 检查中断是否使能 */
    if(((EXTI->INTFR & EXTI_Line) != (uint32_t)RESET) && (enablestatus != (uint32_t)RESET))
    {
        bitstatus = SET;  /**< 中断已使能且标志置位 */
    }
    else
    {
        bitstatus = RESET;  /**< 中断未发生 */
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      EXTI_ClearITPendingBit
 *
 * @brief   清除EXTI线的中断挂起位
 *
 * @param   EXTI_Line - 指定要清除挂起位的EXTI线
 *
 * @return  无
 */
void EXTI_ClearITPendingBit(uint32_t EXTI_Line)
{
    EXTI->INTFR = EXTI_Line;  /**< 写1清除对应中断挂起位 */
}