/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_opa.h
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 本文件包含所有运放(OPA)固件库函数的原型声明
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V30x_OPA_H
#define __CH32V30x_OPA_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* 寄存器位偏移量定义 */
#define OPA_PSEL_OFFSET 3  /* 正输入端选择位偏移 */
#define OPA_NSEL_OFFSET 2  /* 负输入端选择位偏移 */
#define OPA_MODE_OFFSET 1  /* 输出模式选择位偏移 */

/* 运放通道枚举 */
typedef enum
{
   OPA1=0,  /* 运放1 */
   OPA2,    /* 运放2 */
   OPA3,    /* 运放3 */
   OPA4     /* 运放4 */
}OPA_Num_TypeDef;

/* 正输入端选择枚举 */
typedef enum
{
   CHP0=0,  /* 正输入端选择通道0 */
   CHP1     /* 正输入端选择通道1 */
}OPA_PSEL_TypeDef;

/* 负输入端选择枚举 */
typedef enum
{
   CHN0=0,  /* 负输入端选择通道0 */
   CHN1     /* 负输入端选择通道1 */
}OPA_NSEL_TypeDef;

/* 输出通道选择枚举 */
typedef enum
{
   OUT_IO_OUT0=0,  /* 输出到IO_OUT0 */
   OUT_IO_OUT1     /* 输出到IO_OUT1 */
}OPA_Mode_TypeDef;

/* 运放初始化结构体定义 */
typedef struct
{
  OPA_Num_TypeDef   OPA_NUM;      /* 指定运放通道 */
  OPA_PSEL_TypeDef  PSEL;         /* 指定运放正输入端 */
  OPA_NSEL_TypeDef  NSEL;         /* 指定运放负输入端 */
  OPA_Mode_TypeDef  Mode;         /* 指定运放输出模式 */
}OPA_InitTypeDef;

/* 函数声明 */
void OPA_DeInit(void);  /* 复位运放寄存器到默认值 */
void OPA_Init(OPA_InitTypeDef* OPA_InitStruct);  /* 根据结构体参数初始化运放 */
void OPA_StructInit(OPA_InitTypeDef* OPA_InitStruct);  /* 用默认值填充运放初始化结构体 */
void OPA_Cmd(OPA_Num_TypeDef OPA_NUM, FunctionalState NewState);  /* 使能或失能指定运放 */

#ifdef __cplusplus
}
#endif

#endif 
