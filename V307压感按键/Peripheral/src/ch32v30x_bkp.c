/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_bkp.c
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2024/03/06
* 描述            : 该文件提供了BKP（备份寄存器）的所有固件函数。
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及二进制代码仅适用于
*       南京沁恒微电子有限公司生产的微控制器。
*******************************************************************************/

#include "ch32v30x_bkp.h"
#include "ch32v30x_rcc.h"

/* BKP寄存器位掩码定义 */

/* OCTLR寄存器位掩码 */
#define OCTLR_CAL_MASK    ((uint16_t)0xFF80)  /* RTC校准值掩码，保留低7位(0x7F) */
#define OCTLR_MASK        ((uint16_t)0xFC7F)  /* RTC输出配置掩码，清除相关控制位 */

/*********************************************************************
 * @函数名      BKP_DeInit
 *
 * @描述       将BKP外设寄存器恢复为默认复位值（复位备份域）。
 *
 * @注意       通过使能和禁止备份域复位功能来实现复位。
 *
 * @返回值     无
 */
void BKP_DeInit(void)
{
    RCC_BackupResetCmd(ENABLE);   /* 使能备份域复位 */
    RCC_BackupResetCmd(DISABLE);  /* 禁用备份域复位，完成复位操作 */
}

/*********************************************************************
 * @函数名      BKP_TamperPinLevelConfig
 *
 * @描述       配置侵入检测引脚的有效电平。
 *
 * @参数       BKP_TamperPinLevel: 指定侵入检测引脚的有效电平。
 *              BKP_TamperPinLevel_High - 侵入检测引脚高电平有效。
 *              BKP_TamperPinLevel_Low  - 侵入检测引脚低电平有效。
 *
 * @返回值     无
 */
void BKP_TamperPinLevelConfig(uint16_t BKP_TamperPinLevel)
{
    if(BKP_TamperPinLevel)  /* 配置为高电平有效 */
    {
        BKP->TPCTLR |= (1 << 1);  /* 设置TPAL位为1 */
    }
    else                     /* 配置为低电平有效 */
    {
        BKP->TPCTLR &= ~(1 << 1); /* 清除TPAL位 */
    }
}

/*********************************************************************
 * @函数名      BKP_TamperPinCmd
 *
 * @描述       使能或禁用侵入检测引脚功能。
 *
 * @参数       NewState - 功能状态：ENABLE 或 DISABLE。
 *
 * @返回值     无
 */
void BKP_TamperPinCmd(FunctionalState NewState)
{
    if(NewState)  /* 使能侵入检测功能 */
    {
        BKP->TPCTLR |= (1 << 0);  /* 设置TPE位为1 */
    }
    else          /* 禁用侵入检测功能 */
    {
        BKP->TPCTLR &= ~(1 << 0); /* 清除TPE位 */
    }
}

/*********************************************************************
 * @函数名      BKP_ITConfig
 *
 * @描述       使能或禁用侵入检测引脚中断。
 *
 * @参数       NewState - 中断状态：ENABLE 或 DISABLE。
 *
 * @返回值     无
 */
void BKP_ITConfig(FunctionalState NewState)
{
    if(NewState)  /* 使能侵入检测中断 */
    {
        BKP->TPCSR |= (1 << 2);  /* 设置TPIE位为1 */
    }
    else          /* 禁用侵入检测中断 */
    {
        BKP->TPCSR &= ~(1 << 2); /* 清除TPIE位 */
    }
}

/*********************************************************************
 * @函数名      BKP_RTCOutputConfig
 *
 * @描述       选择要在侵入检测引脚上输出的RTC输出源。
 *
 * @参数       BKP_RTCOutputSource - 指定RTC输出源。
 *              BKP_RTCOutputSource_None     - 侵入检测引脚无RTC输出。
 *              BKP_RTCOutputSource_CalibClock - 在侵入检测引脚输出经过64分频的RTC时钟。
 *              BKP_RTCOutputSource_Alarm    - 在侵入检测引脚输出RTC报警脉冲信号。
 *              BKP_RTCOutputSource_Second   - 在侵入检测引脚输出RTC秒脉冲信号。
 *
 * @返回值     无
 */
void BKP_RTCOutputConfig(uint16_t BKP_RTCOutputSource)
{
    uint16_t tmpreg = 0;  /* 临时寄存器变量 */

    tmpreg = BKP->OCTLR;       /* 读取当前OCTLR寄存器值 */
    tmpreg &= OCTLR_MASK;      /* 清除输出配置相关位 */
    tmpreg |= BKP_RTCOutputSource; /* 设置新的输出配置 */
    BKP->OCTLR = tmpreg;       /* 写回配置值 */
}

/*********************************************************************
 * @函数名      BKP_SetRTCCalibrationValue
 *
 * @描述       设置RTC时钟校准值。
 *
 * @参数       CalibrationValue - 指定RTC时钟校准值。
 *              该参数必须是0到0x7F之间的数值。
 *
 * @返回值     无
 */
void BKP_SetRTCCalibrationValue(uint8_t CalibrationValue)
{
    uint16_t tmpreg = 0;  /* 临时寄存器变量 */

    tmpreg = BKP->OCTLR;       /* 读取当前OCTLR寄存器值 */
    tmpreg &= OCTLR_CAL_MASK;  /* 清除校准值相关位（保留高9位） */
    tmpreg |= CalibrationValue; /* 设置新的校准值 */
    BKP->OCTLR = tmpreg;       /* 写回配置值 */
}

/*********************************************************************
 * @函数名      BKP_WriteBackupRegister
 *
 * @描述       向指定的数据备份寄存器写入用户数据。
 *
 * @参数       BKP_DR - 指定数据备份寄存器编号。
 *             Data   - 要写入的数据。
 *
 * @注意       备份寄存器在VBAT供电域中，系统复位或电源复位时数据不会丢失。
 *
 * @返回值     无
 */
void BKP_WriteBackupRegister(uint16_t BKP_DR, uint16_t Data)
{
    __IO uint32_t tmp = 0;  /* 定义易失性临时变量 */

    tmp = (uint32_t)BKP_BASE;  /* 获取BKP外设基地址 */
    tmp += BKP_DR;             /* 计算目标寄存器地址 */
    *(__IO uint32_t *)tmp = Data; /* 向该地址写入数据 */
}

/*********************************************************************
 * @函数名      BKP_ReadBackupRegister
 *
 * @描述       从指定的数据备份寄存器读取数据。
 *
 * @参数       BKP_DR - 指定数据备份寄存器编号。
 *              该参数可以是BKP_DRx，其中x=[1, 42]。
 *
 * @返回值     读取到的数据备份寄存器值。
 */
uint16_t BKP_ReadBackupRegister(uint16_t BKP_DR)
{
    __IO uint32_t tmp = 0;  /* 定义易失性临时变量 */

    tmp = (uint32_t)BKP_BASE;  /* 获取BKP外设基地址 */
    tmp += BKP_DR;             /* 计算目标寄存器地址 */

    return (*(__IO uint16_t *)tmp); /* 读取并返回该地址的数据 */
}

/*********************************************************************
 * @函数名      BKP_GetFlagStatus
 *
 * @描述       检查侵入检测引脚事件标志是否被置位。
 *
 * @返回值     FlagStatus - 标志状态：SET 或 RESET。
 */
FlagStatus BKP_GetFlagStatus(void)
{
    if(BKP->TPCSR & (1 << 8))  /* 检查TPEF位（位8）是否置位 */
    {
        return SET;   /* 标志置位 */
    }
    else
    {
        return RESET; /* 标志复位 */
    }
}

/*********************************************************************
 * @函数名      BKP_ClearFlag
 *
 * @描述       清除侵入检测引脚事件待处理标志。
 *
 * @返回值     无
 */
void BKP_ClearFlag(void)
{
    BKP->TPCSR |= BKP_CTE;  /* 写1清除侵入检测事件标志 */
}

/*********************************************************************
 * @函数名      BKP_GetITStatus
 *
 * @描述       检查侵入检测引脚中断是否发生。
 *
 * @返回值     ITStatus - 中断状态：SET 或 RESET。
 */
ITStatus BKP_GetITStatus(void)
{
    if(BKP->TPCSR & (1 << 9))  /* 检查TPIF位（位9）是否置位 */
    {
        return SET;   /* 中断发生 */
    }
    else
    {
        return RESET; /* 中断未发生 */
    }
}

/*********************************************************************
 * @函数名      BKP_ClearITPendingBit
 *
 * @描述       清除侵入检测引脚中断待处理位。
 *
 * @返回值     无
 */
void BKP_ClearITPendingBit(void)
{
    BKP->TPCSR |= BKP_CTI;  /* 写1清除侵入检测中断待处理位 */
}
