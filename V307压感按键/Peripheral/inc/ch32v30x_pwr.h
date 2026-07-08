/********************************** (C) COPYRIGHT  *******************************
* 文件名           : ch32v30x_pwr.h
* 作者             : WCH
* 版本             : V1.0.0
* 日期             : 2021/06/06
* 描述             : 本文件包含了PWR(电源控制)固件库的所有函数原型。
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意：本软件（修改或未修改）及二进制文件仅用于南京沁恒微电子制造的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_PWR_H
#define __CH32V30x_PWR_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* 电源电压检测(PVD)触发电平配置 */
#define PWR_PVDLevel_MODE0          ((uint32_t)0x00000000) /* PVD检测阈值: 2.2V */
#define PWR_PVDLevel_MODE1          ((uint32_t)0x00000020) /* PVD检测阈值: 2.3V */
#define PWR_PVDLevel_MODE2          ((uint32_t)0x00000040) /* PVD检测阈值: 2.4V */
#define PWR_PVDLevel_MODE3          ((uint32_t)0x00000060) /* PVD检测阈值: 2.5V */
#define PWR_PVDLevel_MODE4          ((uint32_t)0x00000080) /* PVD检测阈值: 2.6V */
#define PWR_PVDLevel_MODE5          ((uint32_t)0x000000A0) /* PVD检测阈值: 2.7V */
#define PWR_PVDLevel_MODE6          ((uint32_t)0x000000C0) /* PVD检测阈值: 2.8V */
#define PWR_PVDLevel_MODE7          ((uint32_t)0x000000E0) /* PVD检测阈值: 2.9V */

/* PVD检测阈值别名定义，与上述模式对应 */
#define PWR_PVDLevel_2V2            PWR_PVDLevel_MODE0 /* 2.2V检测阈值 */
#define PWR_PVDLevel_2V3            PWR_PVDLevel_MODE1 /* 2.3V检测阈值 */
#define PWR_PVDLevel_2V4            PWR_PVDLevel_MODE2 /* 2.4V检测阈值 */
#define PWR_PVDLevel_2V5            PWR_PVDLevel_MODE3 /* 2.5V检测阈值 */
#define PWR_PVDLevel_2V6            PWR_PVDLevel_MODE4 /* 2.6V检测阈值 */
#define PWR_PVDLevel_2V7            PWR_PVDLevel_MODE5 /* 2.7V检测阈值 */
#define PWR_PVDLevel_2V8            PWR_PVDLevel_MODE6 /* 2.8V检测阈值 */
#define PWR_PVDLevel_2V9            PWR_PVDLevel_MODE7 /* 2.9V检测阈值 */
	 
/* 停机模式下稳压器状态配置 */
#define PWR_Regulator_ON          ((uint32_t)0x00000000) /* 停机模式下稳压器保持开启 */
#define PWR_Regulator_LowPower    ((uint32_t)0x00000001) /* 停机模式下稳压器进入低功耗模式 */

/* 停机模式进入方式 */
#define PWR_STOPEntry_WFI         ((uint8_t)0x01) /* 通过WFI指令进入停机模式 */
#define PWR_STOPEntry_WFE         ((uint8_t)0x02) /* 通过WFE指令进入停机模式 */
 
/* 电源控制标志位定义 */
#define PWR_FLAG_WU               ((uint32_t)0x00000001) /* 唤醒标志 */
#define PWR_FLAG_SB               ((uint32_t)0x00000002) /* 待机模式标志 */
#define PWR_FLAG_PVDO             ((uint32_t)0x00000004) /* PVD输出标志(电源电压低于阈值) */

/* 函数声明部分 */
void PWR_DeInit(void); /* 将PWR寄存器重置为默认值 */
void PWR_BackupAccessCmd(FunctionalState NewState); /* 启用或禁用备份域访问 */
void PWR_PVDCmd(FunctionalState NewState); /* 启用或禁用电源电压检测器(PVD) */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel); /* 配置PVD检测阈值 */
void PWR_WakeUpPinCmd(FunctionalState NewState); /* 启用或禁用唤醒引脚功能 */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry); /* 进入停机模式 */
void PWR_EnterSTANDBYMode(void); /* 进入待机模式 */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG); /* 获取指定电源标志位的状态 */
void PWR_ClearFlag(uint32_t PWR_FLAG); /* 清除指定的电源标志位 */
void PWR_EnterSTANDBYMode_RAM(void); /* 从RAM运行进入待机模式 */
void PWR_EnterSTANDBYMode_RAM_LV(void); /* 从RAM运行进入低电压待机模式 */
void PWR_EnterSTANDBYMode_RAM_VBAT_EN(void); /* 从RAM运行进入待机模式并启用VBAT */
void PWR_EnterSTANDBYMode_RAM_LV_VBAT_EN(void); /* 从RAM运行进入低电压待机模式并启用VBAT */
void PWR_EnterSTOPMode_RAM_LV(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry); /* 从RAM运行进入低电压停机模式 */

#ifdef __cplusplus
}
#endif

#endif