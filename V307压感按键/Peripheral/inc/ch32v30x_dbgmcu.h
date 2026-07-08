/********************************** (C) COPYRIGHT *******************************
* 文件名          : ch32v30x_dbgmcu.h
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 此文件包含了DBGMCU固件库的所有函数原型
*                      (调试MCU单元 - Debug MCU Unit)
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及二进制文件仅用于
*       南京沁恒微电子制造的微控制器。
*******************************************************************************/

#ifndef __CH32V30x_DBGMCU_H
#define __CH32V30x_DBGMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* DBGMCU配置位定义 - 用于控制调试期间的外设行为 */
/**
  * @brief  调试期间使能MCU SLEEP（睡眠）模式
  */
#define DBGMCU_SLEEP                 ((uint32_t)0x00000001)

/**
  * @brief  调试期间使能MCU STOP（停机）模式
  */
#define DBGMCU_STOP                  ((uint32_t)0x00000002)

/**
  * @brief  调试期间使能MCU STANDBY（待机）模式
  */
#define DBGMCU_STANDBY               ((uint32_t)0x00000004)

/**
  * @brief  调试期间独立看门狗（IWDG）计数器停止
  */
#define DBGMCU_IWDG_STOP             ((uint32_t)0x00000100)

/**
  * @brief  调试期间窗口看门狗（WWDG）计数器停止
  */
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000200)

/**
  * @brief  调试期间I2C1 SMBus超时计数器停止
  */
#define DBGMCU_I2C1_SMBUS_TIMEOUT    ((uint32_t)0x00000400)

/**
  * @brief  调试期间I2C2 SMBus超时计数器停止
  */
#define DBGMCU_I2C2_SMBUS_TIMEOUT    ((uint32_t)0x00000800)

/**
  * @brief  调试期间定时器1（TIM1）计数器停止
  */
#define DBGMCU_TIM1_STOP             ((uint32_t)0x00001000)

/**
  * @brief  调试期间定时器2（TIM2）计数器停止
  */
#define DBGMCU_TIM2_STOP             ((uint32_t)0x00002000)

/**
  * @brief  调试期间定时器3（TIM3）计数器停止
  */
#define DBGMCU_TIM3_STOP             ((uint32_t)0x00004000)

/**
  * @brief  调试期间定时器4（TIM4）计数器停止
  */
#define DBGMCU_TIM4_STOP             ((uint32_t)0x00008000)

/**
  * @brief  调试期间定时器5（TIM5）计数器停止
  */
#define DBGMCU_TIM5_STOP             ((uint32_t)0x00010000)

/**
  * @brief  调试期间定时器6（TIM6）计数器停止
  */
#define DBGMCU_TIM6_STOP             ((uint32_t)0x00020000)

/**
  * @brief  调试期间定时器7（TIM7）计数器停止
  */
#define DBGMCU_TIM7_STOP             ((uint32_t)0x00040000)

/**
  * @brief  调试期间定时器8（TIM8）计数器停止
  */
#define DBGMCU_TIM8_STOP             ((uint32_t)0x00080000)

/**
  * @brief  调试期间CAN1停止
  */
#define DBGMCU_CAN1_STOP             ((uint32_t)0x00100000)

/**
  * @brief  调试期间CAN2停止
  */
#define DBGMCU_CAN2_STOP             ((uint32_t)0x00200000)

/**
  * @brief  调试期间定时器9（TIM9）计数器停止
  */
#define DBGMCU_TIM9_STOP             ((uint32_t)0x00400000)

/**
  * @brief  调试期间定时器10（TIM10）计数器停止
  */
#define DBGMCU_TIM10_STOP            ((uint32_t)0x00800000)

/* 函数声明 */
/**
  * @brief  获取芯片版本标识（Revision ID）
  * @retval 返回芯片版本标识值
  */
uint32_t DBGMCU_GetREVID(void);

/**
  * @brief  获取设备标识（Device ID）
  * @retval 返回设备标识值
  */
uint32_t DBGMCU_GetDEVID(void);

/**
  * @brief  获取调试控制寄存器（DEBUG_CR）的值
  * @note   这是一个内部函数，用于直接读取寄存器
  * @retval 返回DEBUG_CR寄存器的值
  */
uint32_t __get_DEBUG_CR(void);

/**
  * @brief  设置调试控制寄存器（DEBUG_CR）的值
  * @note   这是一个内部函数，用于直接写入寄存器
  * @param  value：要写入DEBUG_CR寄存器的值
  * @retval 无
  */
void __set_DEBUG_CR(uint32_t value);

/**
  * @brief  配置DBGMCU外设在调试期间的行为
  * @param  DBGMCU_Periph：要配置的DBGMCU外设，可以是以下值的组合：
  *         @arg DBGMCU_SLEEP: 调试期间使能SLEEP模式
  *         @arg DBGMCU_STOP: 调试期间使能STOP模式
  *         @arg DBGMCU_STANDBY: 调试期间使能STANDBY模式
  *         @arg DBGMCU_IWDG_STOP: 调试期间IWDG停止
  *         @arg DBGMCU_WWDG_STOP: 调试期间WWDG停止
  *         @arg DBGMCU_I2C1_SMBUS_TIMEOUT: 调试期间I2C1 SMBus超时停止
  *         @arg DBGMCU_I2C2_SMBUS_TIMEOUT: 调试期间I2C2 SMBus超时停止
  *         @arg DBGMCU_TIMx_STOP: 调试期间定时器x停止
  *         @arg DBGMCU_CANx_STOP: 调试期间CANx停止
  * @param  NewState：外设的新状态
  *         @arg ENABLE: 使能调试期间的特殊行为
  *         @arg DISABLE: 禁用调试期间的特殊行为
  * @retval 无
  * @note   这个函数允许在调试期间保持某些外设运行或停止，便于调试
  */
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState);

/**
  * @brief  获取芯片ID
  * @retval 返回芯片ID值
  */
uint32_t DBGMCU_GetCHIPID( void );

#ifdef __cplusplus
}
#endif

#endif 
