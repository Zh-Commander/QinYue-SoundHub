/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_exti.h
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2021/06/06
* 描述            : 该文件包含了EXTI（外部中断/事件控制器）固件库的所有函数原型
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意：本软件（无论是否经过修改）及二进制文件均用于
*       南京沁恒微电子生产的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_EXTI_H
#define __CH32V30x_EXTI_H

#ifdef __cplusplus
 extern "C" {                // 如果是C++编译器，使用C的编译链接方式
#endif

#include "ch32v30x.h"        // 包含芯片公共头文件

/* EXTI模式枚举 */
typedef enum
{
  EXTI_Mode_Interrupt = 0x00,  // 中断模式
  EXTI_Mode_Event = 0x04       // 事件模式
}EXTIMode_TypeDef;

/* EXTI触发沿枚举 */
typedef enum
{
  EXTI_Trigger_Rising = 0x08,         // 上升沿触发
  EXTI_Trigger_Falling = 0x0C,        // 下降沿触发
  EXTI_Trigger_Rising_Falling = 0x10  // 上升沿和下降沿都触发
}EXTITrigger_TypeDef;

/* EXTI初始化结构体定义 */
typedef struct
{
  uint32_t EXTI_Line;               /* 指定要启用或禁用的EXTI线。
                                       此参数可以是 @ref EXTI_Lines 中定义的任意组合 */
   
  EXTIMode_TypeDef EXTI_Mode;       /* 指定EXTI线的工作模式。
                                       此参数可以是 @ref EXTIMode_TypeDef 中的一个值 */

  EXTITrigger_TypeDef EXTI_Trigger; /* 指定EXTI线的触发信号有效边沿。
                                       此参数可以是 @ref EXTIMode_TypeDef 中的一个值 */

  FunctionalState EXTI_LineCmd;     /* 指定所选EXTI线的新状态。
                                       此参数可以设置为ENABLE（使能）或DISABLE（禁用） */ 
}EXTI_InitTypeDef;


/* EXTI线定义 - 每个位对应一条外部中断线 */
#define EXTI_Line0       ((uint32_t)0x00001)  /* 外部中断线0 */
#define EXTI_Line1       ((uint32_t)0x00002)  /* 外部中断线1 */
#define EXTI_Line2       ((uint32_t)0x00004)  /* 外部中断线2 */
#define EXTI_Line3       ((uint32_t)0x00008)  /* 外部中断线3 */
#define EXTI_Line4       ((uint32_t)0x00010)  /* 外部中断线4 */
#define EXTI_Line5       ((uint32_t)0x00020)  /* 外部中断线5 */
#define EXTI_Line6       ((uint32_t)0x00040)  /* 外部中断线6 */
#define EXTI_Line7       ((uint32_t)0x00080)  /* 外部中断线7 */
#define EXTI_Line8       ((uint32_t)0x00100)  /* 外部中断线8 */
#define EXTI_Line9       ((uint32_t)0x00200)  /* 外部中断线9 */
#define EXTI_Line10      ((uint32_t)0x00400)  /* 外部中断线10 */
#define EXTI_Line11      ((uint32_t)0x00800)  /* 外部中断线11 */
#define EXTI_Line12      ((uint32_t)0x01000)  /* 外部中断线12 */
#define EXTI_Line13      ((uint32_t)0x02000)  /* 外部中断线13 */
#define EXTI_Line14      ((uint32_t)0x04000)  /* 外部中断线14 */
#define EXTI_Line15      ((uint32_t)0x08000)  /* 外部中断线15 */
#define EXTI_Line16      ((uint32_t)0x10000)  /* 外部中断线16，连接到PVD（可编程电压检测器）输出 */
#define EXTI_Line17      ((uint32_t)0x20000)  /* 外部中断线17，连接到RTC闹钟事件 */
#define EXTI_Line18      ((uint32_t)0x40000)  /* 外部中断线18，连接到USBD/USBFS OTG从挂起唤醒事件 */
#define EXTI_Line19      ((uint32_t)0x80000)  /* 外部中断线19，连接到以太网唤醒事件 */
#define EXTI_Line20      ((uint32_t)0x100000) /* 外部中断线20，连接到USBHS唤醒事件 */

/* 函数声明 */
void EXTI_DeInit(void);                          // 将EXTI外设寄存器复位为默认值
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);  // 根据EXTI_InitStruct中的参数初始化EXTI外设
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct); // 将EXTI_InitStruct中的每个参数填充为默认值
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line); // 产生一个软件中断（通过软件触发指定的EXTI线）
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line); // 检查指定EXTI线的标志位是否置位（中断挂起位）
void EXTI_ClearFlag(uint32_t EXTI_Line);          // 清除指定EXTI线的标志位（中断挂起位）
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);   // 检查指定EXTI线的中断状态（是否发生中断）
void EXTI_ClearITPendingBit(uint32_t EXTI_Line); // 清除指定EXTI线中断挂起位

#ifdef __cplusplus
}
#endif

#endif  /* __CH32V30x_EXTI_H */
