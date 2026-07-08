/********************************** (C) COPYRIGHT *******************************
* 文件名称          : ch32v30x_rng.c
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 此文件提供RNG(随机数生成器)的所有固件函数
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 此软件（修改或未修改）和二进制文件用于
*       南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/
#include "ch32v30x_rng.h"
#include "ch32v30x_rcc.h"

/*********************************************************************
 * @函数名      RNG_Cmd
 *
 * @描述       启用或禁用RNG外设
 *
 * @参数       NewState - ENABLE(启用) 或 DISABLE(禁用)
 *
 * @返回值     无
 */
void RNG_Cmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RNG->CR |= RNG_CR_RNGEN;  /* 设置RNG_CR寄存器的RNGEN位(随机数生成器使能位)为1 */
    }
    else
    {
        RNG->CR &= ~RNG_CR_RNGEN; /* 清除RNG_CR寄存器的RNGEN位为0 */
    }
}

/*********************************************************************
 * @函数名      RNG_GetRandomNumber
 *
 * @描述       返回一个32位随机数
 *
 * @返回值     32位随机数
 */
uint32_t RNG_GetRandomNumber(void)
{
    return RNG->DR;  /* 直接读取RNG数据寄存器(DR)的值作为随机数 */
}

/*********************************************************************
 * @函数名      RNG_ITConfig
 *
 * @描述       启用或禁用RNG中断
 *
 * @参数       NewState - ENABLE(启用) 或 DISABLE(禁用)
 *
 * @返回值     无
 */
void RNG_ITConfig(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RNG->CR |= RNG_CR_IE;  /* 设置RNG_CR寄存器的IE位(中断使能位)为1 */
    }
    else
    {
        RNG->CR &= ~RNG_CR_IE; /* 清除RNG_CR寄存器的IE位为0 */
    }
}

/*********************************************************************
 * @函数名      RNG_GetFlagStatus
 *
 * @描述       检查指定的RNG标志位是否置位
 *
 * @参数       RNG_FLAG - 指定要检查的RNG标志位
 *                        可能的值:
 *                        RNG_FLAG_DRDY - 数据就绪标志(随机数已准备好)
 *                        RNG_FLAG_CECS - 时钟错误当前标志(时钟错误发生)
 *                        RNG_FLAG_SECS - 种子错误当前标志(种子错误发生)
 *
 * @返回值     标志位状态: SET(置位) 或 RESET(复位)
 */
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((RNG->SR & RNG_FLAG) != (uint8_t)RESET) /* 检查RNG状态寄存器(SR)中指定标志位是否置位 */
    {
        bitstatus = SET;  /* 标志位置位 */
    }
    else
    {
        bitstatus = RESET; /* 标志位复位 */
    }

    return bitstatus;
}

/*********************************************************************
 * @函数名      RNG_ClearFlag
 *
 * @描述       清除RNG标志位
 *
 * @参数       RNG_FLAG - 指定要清除的标志位
 *                        可能的值:
 *                        RNG_FLAG_CECS - 时钟错误当前标志
 *                        RNG_FLAG_SECS - 种子错误当前标志
 *                        注意: RNG_FLAG_DRDY是只读标志，不能通过此函数清除
 *
 * @返回值     无
 */
void RNG_ClearFlag(uint8_t RNG_FLAG)
{
    /* 
     * 将标志位值左移4位后取反，然后写入状态寄存器(SR)
     * 注意: RNG状态寄存器的错误标志位(CECS和SECS)位于bit[6:5]，但清除时需要写入1到bit[10:9]
     * 这里通过左移4位来映射错误标志到正确的清除位
     */
    RNG->SR = ~(uint32_t)(((uint32_t)RNG_FLAG) << 4);
}

/*********************************************************************
 * @函数名      RNG_GetITStatus
 *
 * @描述       检查指定的RNG中断是否发生
 *
 * @参数       RNG_IT - 指定要检查的RNG中断源
 *                       可能的值:
 *                       RNG_IT_CEI - 时钟错误中断
 *                       RNG_IT_SEI - 种子错误中断
 *
 * @返回值     中断状态: SET(中断发生) 或 RESET(中断未发生)
 */
ITStatus RNG_GetITStatus(uint8_t RNG_IT)
{
    ITStatus bitstatus = RESET;

    if((RNG->SR & RNG_IT) != (uint8_t)RESET) /* 检查RNG状态寄存器(SR)中指定中断标志是否置位 */
    {
        bitstatus = SET;  /* 中断标志置位 */
    }
    else
    {
        bitstatus = RESET; /* 中断标志复位 */
    }

    return bitstatus;
}

/*********************************************************************
 * @函数名      RNG_ClearITPendingBit
 *
 * @描述       清除RNG中断挂起位
 *
 * @参数       RNG_IT - 指定要清除的RNG中断挂起位
 *                       可能的值:
 *                       RNG_IT_CEI - 时钟错误中断挂起位
 *                       RNG_IT_SEI - 种子错误中断挂起位
 *
 * @返回值     无
 */
void RNG_ClearITPendingBit(uint8_t RNG_IT)
{
    /* 清除中断挂起位: 将中断标志位取反后写入状态寄存器(SR) */
    RNG->SR = (uint8_t)~RNG_IT;
}

/* 
 * 注意: 以上函数中使用的宏定义说明:
 * 
 * 控制寄存器(CR)相关位:
 * - RNG_CR_RNGEN: 随机数生成器使能位
 * - RNG_CR_IE:    中断使能位
 * 
 * 状态寄存器(SR)相关标志位:
 * - RNG_FLAG_DRDY: 数据就绪标志(随机数数据已准备好)
 * - RNG_FLAG_CECS: 时钟错误当前状态标志
 * - RNG_FLAG_SECS: 种子错误当前状态标志
 * 
 * 状态寄存器(SR)相关中断标志位:
 * - RNG_IT_CEI: 时钟错误中断标志
 * - RNG_IT_SEI: 种子错误中断标志
 * 
 * 函数状态枚举:
 * - ENABLE/DISABLE: 用于控制外设或功能的启用/禁用
 * - SET/RESET:      用于表示标志位的置位/复位状态
 */