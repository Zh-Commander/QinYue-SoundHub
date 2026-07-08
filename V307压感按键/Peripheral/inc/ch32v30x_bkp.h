/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_bkp.h
* 作者            : WCH
* 版本            : V1.0.0
* 发布日期        : 2021/06/06
* 描述            : 此文件包含备份寄存器(BKP)固件库的所有函数原型声明
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 此软件（无论是否修改）及二进制代码仅用于
*       南京沁恒微电子生产的微控制器
*******************************************************************************/
#ifndef __CH32V30x_BKP_H
#define __CH32V30x_BKP_H

#ifdef __cplusplus
 extern "C" {  // 如果是C++编译器，使用C语言规范编译
#endif

#include "ch32v30x.h"

/* 篡改检测引脚有效电平定义 */
#define BKP_TamperPinLevel_High           ((uint16_t)0x0000)  // 篡改检测引脚高电平触发
#define BKP_TamperPinLevel_Low            ((uint16_t)0x0001)  // 篡改检测引脚低电平触发

/* RTC输出源到篡改引脚的输出定义 */
#define BKP_RTCOutputSource_None          ((uint16_t)0x0000)  // 不输出RTC信号到篡改引脚
#define BKP_RTCOutputSource_CalibClock    ((uint16_t)0x0080)  // 输出RTC校准时钟到篡改引脚
#define BKP_RTCOutputSource_Alarm         ((uint16_t)0x0100)  // 输出RTC闹钟信号到篡改引脚
#define BKP_RTCOutputSource_Second        ((uint16_t)0x0300)  // 输出RTC秒脉冲到篡改引脚
	 
/* 数据备份寄存器地址偏移定义 */
#define BKP_DR1                           ((uint16_t)0x0004)  // 备份数据寄存器1
#define BKP_DR2                           ((uint16_t)0x0008)  // 备份数据寄存器2
#define BKP_DR3                           ((uint16_t)0x000C)  // 备份数据寄存器3
#define BKP_DR4                           ((uint16_t)0x0010)  // 备份数据寄存器4
#define BKP_DR5                           ((uint16_t)0x0014)  // 备份数据寄存器5
#define BKP_DR6                           ((uint16_t)0x0018)  // 备份数据寄存器6
#define BKP_DR7                           ((uint16_t)0x001C)  // 备份数据寄存器7
#define BKP_DR8                           ((uint16_t)0x0020)  // 备份数据寄存器8
#define BKP_DR9                           ((uint16_t)0x0024)  // 备份数据寄存器9
#define BKP_DR10                          ((uint16_t)0x0028)  // 备份数据寄存器10
#define BKP_DR11                          ((uint16_t)0x0040)  // 备份数据寄存器11
#define BKP_DR12                          ((uint16_t)0x0044)  // 备份数据寄存器12
#define BKP_DR13                          ((uint16_t)0x0048)  // 备份数据寄存器13
#define BKP_DR14                          ((uint16_t)0x004C)  // 备份数据寄存器14
#define BKP_DR15                          ((uint16_t)0x0050)  // 备份数据寄存器15
#define BKP_DR16                          ((uint16_t)0x0054)  // 备份数据寄存器16
#define BKP_DR17                          ((uint16_t)0x0058)  // 备份数据寄存器17
#define BKP_DR18                          ((uint16_t)0x005C)  // 备份数据寄存器18
#define BKP_DR19                          ((uint16_t)0x0060)  // 备份数据寄存器19
#define BKP_DR20                          ((uint16_t)0x0064)  // 备份数据寄存器20
#define BKP_DR21                          ((uint16_t)0x0068)  // 备份数据寄存器21
#define BKP_DR22                          ((uint16_t)0x006C)  // 备份数据寄存器22
#define BKP_DR23                          ((uint16_t)0x0070)  // 备份数据寄存器23
#define BKP_DR24                          ((uint16_t)0x0074)  // 备份数据寄存器24
#define BKP_DR25                          ((uint16_t)0x0078)  // 备份数据寄存器25
#define BKP_DR26                          ((uint16_t)0x007C)  // 备份数据寄存器26
#define BKP_DR27                          ((uint16_t)0x0080)  // 备份数据寄存器27
#define BKP_DR28                          ((uint16_t)0x0084)  // 备份数据寄存器28
#define BKP_DR29                          ((uint16_t)0x0088)  // 备份数据寄存器29
#define BKP_DR30                          ((uint16_t)0x008C)  // 备份数据寄存器30
#define BKP_DR31                          ((uint16_t)0x0090)  // 备份数据寄存器31
#define BKP_DR32                          ((uint16_t)0x0094)  // 备份数据寄存器32
#define BKP_DR33                          ((uint16_t)0x0098)  // 备份数据寄存器33
#define BKP_DR34                          ((uint16_t)0x009C)  // 备份数据寄存器34
#define BKP_DR35                          ((uint16_t)0x00A0)  // 备份数据寄存器35
#define BKP_DR36                          ((uint16_t)0x00A4)  // 备份数据寄存器36
#define BKP_DR37                          ((uint16_t)0x00A8)  // 备份数据寄存器37
#define BKP_DR38                          ((uint16_t)0x00AC)  // 备份数据寄存器38
#define BKP_DR39                          ((uint16_t)0x00B0)  // 备份数据寄存器39
#define BKP_DR40                          ((uint16_t)0x00B4)  // 备份数据寄存器40
#define BKP_DR41                          ((uint16_t)0x00B8)  // 备份数据寄存器41
#define BKP_DR42                          ((uint16_t)0x00BC)  // 备份数据寄存器42

// 函数原型声明
void BKP_DeInit(void);  // 将BKP寄存器重置为默认值
void BKP_TamperPinLevelConfig(uint16_t BKP_TamperPinLevel);  // 配置篡改检测引脚的有效电平
void BKP_TamperPinCmd(FunctionalState NewState);  // 使能或失能篡改检测引脚功能
void BKP_ITConfig(FunctionalState NewState);  // 使能或失能篡改检测中断
void BKP_RTCOutputConfig(uint16_t BKP_RTCOutputSource);  // 配置RTC输出到篡改引脚
void BKP_SetRTCCalibrationValue(uint8_t CalibrationValue);  // 设置RTC时钟校准值
void BKP_WriteBackupRegister(uint16_t BKP_DR, uint16_t Data);  // 向指定备份寄存器写入数据
uint16_t BKP_ReadBackupRegister(uint16_t BKP_DR);  // 从指定备份寄存器读取数据
FlagStatus BKP_GetFlagStatus(void);  // 获取篡改检测标志位状态
void BKP_ClearFlag(void);  // 清除篡改检测标志位
ITStatus BKP_GetITStatus(void);  // 获取篡改检测中断状态
void BKP_ClearITPendingBit(void);  // 清除篡改检测中断待处理位

#ifdef __cplusplus
}
#endif

#endif  // 头文件结束标记
