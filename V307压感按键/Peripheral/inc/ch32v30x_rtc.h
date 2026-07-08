/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_rtc.h
* 作者             : WCH
* 版本            : V1.0.0
* 发布日期        : 2021/06/06
* 描述        : 此文件包含了RTC固件库的所有函数原型。
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及其二进制文件仅适用于南京沁恒微电子生产的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_RTC_H
#define __CH32V30x_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"


/* RTC中断定义 */
#define RTC_IT_OW            ((uint16_t)0x0004)  /* 溢出中断 */
#define RTC_IT_ALR           ((uint16_t)0x0002)  /* 闹钟中断 */
#define RTC_IT_SEC           ((uint16_t)0x0001)  /* 秒中断 */

/* RTC中断标志位 */
#define RTC_FLAG_RTOFF       ((uint16_t)0x0020)  /* RTC操作关闭标志位 */
#define RTC_FLAG_RSF         ((uint16_t)0x0008)  /* 寄存器同步标志位 */
#define RTC_FLAG_OW          ((uint16_t)0x0004)  /* 溢出标志位 */
#define RTC_FLAG_ALR         ((uint16_t)0x0002)  /* 闹钟标志位 */
#define RTC_FLAG_SEC         ((uint16_t)0x0001)  /* 秒标志位 */
	 
/**
  * @brief  使能或禁用指定的RTC中断
  * @param  RTC_IT: 要配置的RTC中断源。
  *         此参数可以是以下值之一：
  *         @arg RTC_IT_OW: 溢出中断
  *         @arg RTC_IT_ALR: 闹钟中断
  *         @arg RTC_IT_SEC: 秒中断
  * @param  NewState: 中断的新状态。
  *         此参数可以是：ENABLE或DISABLE。
  * @retval 无
  */
void RTC_ITConfig(uint16_t RTC_IT, FunctionalState NewState);

/**
  * @brief  进入RTC配置模式，允许写RTC的预分频器、计数器和闹钟寄存器。
  * @note   在对RTC的预分频器、计数器或闹钟寄存器进行写操作前，必须调用此函数。
  * @retval 无
  */
void RTC_EnterConfigMode(void);

/**
  * @brief  退出RTC配置模式，防止误写RTC的预分频器、计数器和闹钟寄存器。
  * @note   在完成对RTC的预分频器、计数器或闹钟寄存器的写操作后，应调用此函数。
  * @retval 无
  */
void RTC_ExitConfigMode(void);

/**
  * @brief  获取RTC计数器的当前值
  * @retval 当前RTC计数器的值（32位）
  */
uint32_t  RTC_GetCounter(void);

/**
  * @brief  设置RTC计数器的值
  * @param  CounterValue: 要设置的RTC计数器值（32位）
  * @retval 无
  */
void RTC_SetCounter(uint32_t CounterValue);

/**
  * @brief  设置RTC预分频器的值
  * @param  PrescalerValue: 要设置的RTC预分频器值（32位）
  * @retval 无
  */
void RTC_SetPrescaler(uint32_t PrescalerValue);

/**
  * @brief  设置RTC闹钟的值
  * @param  AlarmValue: 要设置的RTC闹钟值（32位）
  * @retval 无
  */
void RTC_SetAlarm(uint32_t AlarmValue);

/**
  * @brief  获取RTC分频器的当前值
  * @retval 当前RTC分频器的值（32位）
  */
uint32_t  RTC_GetDivider(void);

/**
  * @brief  等待上一次对RTC寄存器的写操作完成
  * @note   在写RTC的预分频器、计数器或闹钟寄存器后，需要调用此函数以确保写操作完成。
  * @retval 无
  */
void RTC_WaitForLastTask(void);

/**
  * @brief  等待RTC寄存器与APB时钟同步
  * @note   在读取RTC计数器、分频器或闹钟寄存器前，如果RTC_CTLRH寄存器中的RSF位为0，则需要调用此函数。
  * @retval 无
  */
void RTC_WaitForSynchro(void);

/**
  * @brief  检查指定的RTC标志位是否被设置
  * @param  RTC_FLAG: 要检查的RTC标志位。
  *         此参数可以是以下值之一：
  *         @arg RTC_FLAG_RTOFF: RTC操作关闭标志位
  *         @arg RTC_FLAG_RSF: 寄存器同步标志位
  *         @arg RTC_FLAG_OW: 溢出标志位
  *         @arg RTC_FLAG_ALR: 闹钟标志位
  *         @arg RTC_FLAG_SEC: 秒标志位
  * @retval FlagStatus: 标志位状态（SET或RESET）
  */
FlagStatus RTC_GetFlagStatus(uint16_t RTC_FLAG);

/**
  * @brief  清除RTC的标志位
  * @param  RTC_FLAG: 要清除的RTC标志位。
  *         此参数可以是以下值之一：
  *         @arg RTC_FLAG_OW: 溢出标志位
  *         @arg RTC_FLAG_ALR: 闹钟标志位
  *         @arg RTC_FLAG_SEC: 秒标志位
  * @note   RTC_FLAG_RTOFF和RTC_FLAG_RSF不能通过此函数清除，它们由硬件设置和清除。
  * @retval 无
  */
void RTC_ClearFlag(uint16_t RTC_FLAG);

/**
  * @brief  检查指定的RTC中断是否发生
  * @param  RTC_IT: 要检查的RTC中断源。
  *         此参数可以是以下值之一：
  *         @arg RTC_IT_OW: 溢出中断
  *         @arg RTC_IT_ALR: 闹钟中断
  *         @arg RTC_IT_SEC: 秒中断
  * @retval ITStatus: 中断状态（SET或RESET）
  */
ITStatus RTC_GetITStatus(uint16_t RTC_IT);

/**
  * @brief  清除RTC的中断挂起位
  * @param  RTC_IT: 要清除的RTC中断挂起位。
  *         此参数可以是以下值之一：
  *         @arg RTC_IT_OW: 溢出中断挂起位
  *         @arg RTC_IT_ALR: 闹钟中断挂起位
  *         @arg RTC_IT_SEC: 秒中断挂起位
  * @retval 无
  */
void RTC_ClearITPendingBit(uint16_t RTC_IT);

#ifdef __cplusplus
}
#endif

#endif 

/* 注释说明：
   1. 本头文件定义了RTC（实时时钟）模块的寄存器、中断、标志位和相关函数。
   2. RTC模块提供了日历、闹钟和自动唤醒功能，依赖于一个32位的可编程计数器。
   3. 使用RTC模块时，需要先使能PWR和BKP时钟，然后使能RTC时钟。
   4. 对RTC寄存器进行写操作时，必须先进入配置模式（RTC_EnterConfigMode），完成后退出配置模式（RTC_ExitConfigMode）。
   5. 每次写操作后，应调用RTC_WaitForLastTask等待操作完成。
   6. 在读取RTC计数器、分频器或闹钟寄存器前，如果RTC_CTLRH寄存器的RSF位为0，需要调用RTC_WaitForSynchro等待同步。
*/