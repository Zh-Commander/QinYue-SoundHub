/********************************** (C) COPYRIGHT *******************************
* 文件名          : ch32v30x_wwdg.c
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 本文件提供了窗口看门狗(WWDG)的所有固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意: 本软件(修改或未修改)及二进制文件用于
*       南京沁恒微电子制造的微控制器。
*******************************************************************************/

#include "ch32v30x_wwdg.h"
#include "ch32v30x_rcc.h"

/* CTLR寄存器位掩码 */
#define CTLR_WDGA_Set      ((uint32_t)0x00000080) /* 使能窗口看门狗(WWDG激活)的位掩码 */

/* CFGR寄存器位掩码 */
#define CFGR_WDGTB_Mask    ((uint32_t)0xFFFFFE7F) /* 预分频器(WDGTB)位域的掩码，用于清零该字段 */
#define CFGR_W_Mask        ((uint32_t)0xFFFFFF80) /* 窗口值(W)位域的掩码，用于清零该字段 */
#define BIT_Mask           ((uint8_t)0x7F)        /* 计数值和窗口值的有效位掩码(低7位有效) */

/*********************************************************************
 * @fn      WWDG_DeInit
 *
 * @brief   将WWDG外设寄存器复位为默认初始值
 *
 * @return  无
 */
void WWDG_DeInit(void)
{
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, ENABLE);  /* 使能WWDG外设复位 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, DISABLE); /* 禁用WWDG外设复位，完成复位 */
}

/*********************************************************************
 * @fn      WWDG_SetPrescaler
 *
 * @brief   设置WWDG的预分频值
 *
 * @param   WWDG_Prescaler - 指定WWDG预分频值
 *            WWDG_Prescaler_1 - WWDG计数器时钟 = (PCLK1/4096)/1
 *            WWDG_Prescaler_2 - WWDG计数器时钟 = (PCLK1/4096)/2
 *            WWDG_Prescaler_4 - WWDG计数器时钟 = (PCLK1/4096)/4
 *            WWDG_Prescaler_8 - WWDG计数器时钟 = (PCLK1/4096)/8
 *
 * @return  无
 */
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler)
{
    uint32_t tmpreg = 0;
    tmpreg = WWDG->CFGR & CFGR_WDGTB_Mask; /* 读取当前CFGR寄存器值，并清零预分频器位域 */
    tmpreg |= WWDG_Prescaler;               /* 设置新的预分频值 */
    WWDG->CFGR = tmpreg;                    /* 写回CFGR寄存器 */
}

/*********************************************************************
 * @fn      WWDG_SetWindowValue
 *
 * @brief   设置WWDG的窗口值
 *
 * @param   WindowValue - 指定要与递减计数器比较的窗口值，必须小于0x80
 *
 * @return  无
 */
void WWDG_SetWindowValue(uint8_t WindowValue)
{
    __IO uint32_t tmpreg = 0;

    tmpreg = WWDG->CFGR & CFGR_W_Mask; /* 读取当前CFGR寄存器值，并清零窗口值位域 */

    tmpreg |= WindowValue & (uint32_t)BIT_Mask; /* 设置新的窗口值，确保只使用低7位 */

    WWDG->CFGR = tmpreg; /* 写回CFGR寄存器 */
}

/*********************************************************************
 * @fn      WWDG_EnableIT
 *
 * @brief   使能WWDG提前唤醒中断(EWI)
 *
 * @return  无
 */
void WWDG_EnableIT(void)
{
    WWDG->CFGR |= (1 << 9); /* 设置CFGR寄存器的第9位(EWI位)为1，使能提前唤醒中断 */
}

/*********************************************************************
 * @fn      WWDG_SetCounter
 *
 * @brief   设置WWDG的计数器值
 *
 * @param   Counter - 指定看门狗计数器值，必须是0x40到0x7F之间的数值
 *
 * @return  无
 */
void WWDG_SetCounter(uint8_t Counter)
{
    WWDG->CTLR = Counter & BIT_Mask; /* 设置计数器值，确保只使用低7位 */
}

/*********************************************************************
 * @fn      WWDG_Enable
 *
 * @brief   使能WWDG并加载计数器值
 *
 * @param   Counter - 指定看门狗计数器值，必须是0x40到0x7F之间的数值
 * @return  无
 */
void WWDG_Enable(uint8_t Counter)
{
    WWDG->CTLR = CTLR_WDGA_Set | Counter; /* 设置WDGA位为1使能WWDG，并设置计数器值 */
}

/*********************************************************************
 * @fn      WWDG_GetFlagStatus
 *
 * @brief   检查提前唤醒中断标志是否被设置
 *
 * @return  提前唤醒中断标志的新状态(SET或RESET)
 */
FlagStatus WWDG_GetFlagStatus(void)
{
    return (FlagStatus)(WWDG->STATR); /* 读取STATR寄存器值并返回，该寄存器只有最低位有效 */
}

/*********************************************************************
 * @fn      WWDG_ClearFlag
 *
 * @brief   清除提前唤醒中断标志
 *
 * @return  无
 */
void WWDG_ClearFlag(void)
{
    WWDG->STATR = (uint32_t)RESET; /* 向STATR寄存器写入0来清除中断标志 */
}