/********************************** (C) COPYRIGHT  *******************************
* 文件名称          : ch32v30x_wwdg.h
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 本文件包含窗口看门狗(WWDG)固件库的所有函数原型。
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: 本软件（无论是否修改）及其二进制文件仅用于南京沁恒微电子制造的微控制器。
*******************************************************************************/

#ifndef __CH32V30x_WWDG_H
#define __CH32V30x_WWDG_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"


/* WWDG预分频器定义 */
#define WWDG_Prescaler_1    ((uint32_t)0x00000000)  /* 预分频器 = 1 */
#define WWDG_Prescaler_2    ((uint32_t)0x00000080)  /* 预分频器 = 2 */
#define WWDG_Prescaler_4    ((uint32_t)0x00000100)  /* 预分频器 = 4 */
#define WWDG_Prescaler_8    ((uint32_t)0x00000180)  /* 预分频器 = 8 */

/**
 * 函数列表：
 * 以下函数用于配置和控制窗口看门狗(WWDG)模块。
 * WWDG用于监测软件故障，必须在设定的时间窗口内刷新计数器，否则将产生复位。
 */
  
void WWDG_DeInit(void);                                /* 将WWDG寄存器恢复为默认值 */
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler);       /* 设置WWDG预分频器 */
void WWDG_SetWindowValue(uint8_t WindowValue);         /* 设置窗口值，即允许刷新计数器的上限 */
void WWDG_EnableIT(void);                              /* 使能WWDG早期唤醒中断(EWI) */
void WWDG_SetCounter(uint8_t Counter);                 /* 设置WWDG计数器的值，必须在窗口期内刷新 */
void WWDG_Enable(uint8_t Counter);                     /* 使能WWDG并设置初始计数器值 */
FlagStatus WWDG_GetFlagStatus(void);                   /* 获取WWDG早期唤醒中断标志位状态 */
void WWDG_ClearFlag(void);                             /* 清除WWDG早期唤醒中断标志位 */

#ifdef __cplusplus
}
#endif

#endif 
