/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_iwdg.h
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 此文件包含了独立看门狗(IWDG)固件库的所有函数原型
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件(修改或未修改)及二进制文件用于南京沁恒微电子生产的微控制器
*******************************************************************************/
#ifndef __CH32V30x_IWDG_H
#define __CH32V30x_IWDG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* IWDG写访问控制 */
#define IWDG_WriteAccess_Enable     ((uint16_t)0x5555)   /* 启用写访问 */
#define IWDG_WriteAccess_Disable    ((uint16_t)0x0000)   /* 禁用写访问 */

/* IWDG预分频器设置 */
#define IWDG_Prescaler_4            ((uint8_t)0x00)      /* 预分频系数4 */
#define IWDG_Prescaler_8            ((uint8_t)0x01)      /* 预分频系数8 */
#define IWDG_Prescaler_16           ((uint8_t)0x02)      /* 预分频系数16 */
#define IWDG_Prescaler_32           ((uint8_t)0x03)      /* 预分频系数32 */
#define IWDG_Prescaler_64           ((uint8_t)0x04)      /* 预分频系数64 */
#define IWDG_Prescaler_128          ((uint8_t)0x05)      /* 预分频系数128 */
#define IWDG_Prescaler_256          ((uint8_t)0x06)      /* 预分频系数256 */

/* IWDG状态标志位 */
#define IWDG_FLAG_PVU               ((uint16_t)0x0001)   /* 预分频器更新标志 */
#define IWDG_FLAG_RVU               ((uint16_t)0x0002)   /* 重装载寄存器更新标志 */


/* 函数声明 */
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess);        /* 使能或禁用IWDG写访问 */
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler);             /* 设置IWDG预分频器值 */
void IWDG_SetReload(uint16_t Reload);                       /* 设置IWDG重装载值 */
void IWDG_ReloadCounter(void);                              /* 重载IWDG计数器(喂狗) */
void IWDG_Enable(void);                                     /* 使能IWDG */
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG);          /* 获取IWDG状态标志 */

#ifdef __cplusplus
}
#endif

#endif 
