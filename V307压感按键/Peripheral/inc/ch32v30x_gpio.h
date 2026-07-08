/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_gpio.h
* Author             : WCH
* Version            : V1.0.1
* Date               : 2025/04/09
* Description        : 此文件包含了所有GPIO固件库的函数原型。
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: 本软件（无论是否被修改）及二进制文件仅供
*            南京沁恒微电子生产的微控制器使用。
*******************************************************************************/
#ifndef __CH32V30x_GPIO_H
#define __CH32V30x_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"
                                 
/* 输出最大频率选择枚举 */
typedef enum
{ 
  GPIO_Speed_10MHz = 1,  /* 最大输出速度10MHz */
  GPIO_Speed_2MHz,       /* 最大输出速度2MHz */
  GPIO_Speed_50MHz       /* 最大输出速度50MHz */
}GPIOSpeed_TypeDef;

/* GPIO配置模式枚举 */
typedef enum
{ GPIO_Mode_AIN = 0x0,       /* 模拟输入模式 */
  GPIO_Mode_IN_FLOATING = 0x04, /* 浮空输入模式 */
  GPIO_Mode_IPD = 0x28,      /* 下拉输入模式 */
  GPIO_Mode_IPU = 0x48,      /* 上拉输入模式 */
  GPIO_Mode_Out_OD = 0x14,   /* 通用开漏输出模式 */
  GPIO_Mode_Out_PP = 0x10,   /* 通用推挽输出模式 */
  GPIO_Mode_AF_OD = 0x1C,    /* 复用功能开漏输出模式 */
  GPIO_Mode_AF_PP = 0x18     /* 复用功能推挽输出模式 */
}GPIOMode_TypeDef;

/* GPIO初始化结构体定义 */
typedef struct
{
  uint16_t GPIO_Pin;             /* 指定要配置的GPIO引脚。
                                    此参数可以是 @ref GPIO_pins_define 中的任意值 */

  GPIOSpeed_TypeDef GPIO_Speed;  /* 指定选定引脚的速度。
                                    此参数可以是 @ref GPIOSpeed_TypeDef 中的一个值 */

  GPIOMode_TypeDef GPIO_Mode;    /* 指定选定引脚的工作模式。
                                    此参数可以是 @ref GPIOMode_TypeDef 中的一个值 */
}GPIO_InitTypeDef;

/* 位设置与位清零枚举 */
typedef enum
{
	Bit_RESET = 0,  /* 清零引脚电平 */
    Bit_SET         /* 设置引脚电平 */
}BitAction;

/* GPIO引脚定义（使用位掩码） */
#define GPIO_Pin_0                  ((uint16_t)0x0001)  /* 选择引脚0 */
#define GPIO_Pin_1                  ((uint16_t)0x0002)  /* 选择引脚1 */
#define GPIO_Pin_2                  ((uint16_t)0x0004)  /* 选择引脚2 */
#define GPIO_Pin_3                  ((uint16_t)0x0008)  /* 选择引脚3 */
#define GPIO_Pin_4                  ((uint16_t)0x0010)  /* 选择引脚4 */
#define GPIO_Pin_5                  ((uint16_t)0x0020)  /* 选择引脚5 */
#define GPIO_Pin_6                  ((uint16_t)0x0040)  /* 选择引脚6 */
#define GPIO_Pin_7                  ((uint16_t)0x0080)  /* 选择引脚7 */
#define GPIO_Pin_8                  ((uint16_t)0x0100)  /* 选择引脚8 */
#define GPIO_Pin_9                  ((uint16_t)0x0200)  /* 选择引脚9 */
#define GPIO_Pin_10                 ((uint16_t)0x0400)  /* 选择引脚10 */
#define GPIO_Pin_11                 ((uint16_t)0x0800)  /* 选择引脚11 */
#define GPIO_Pin_12                 ((uint16_t)0x1000)  /* 选择引脚12 */
#define GPIO_Pin_13                 ((uint16_t)0x2000)  /* 选择引脚13 */
#define GPIO_Pin_14                 ((uint16_t)0x4000)  /* 选择引脚14 */
#define GPIO_Pin_15                 ((uint16_t)0x8000)  /* 选择引脚15 */
#define GPIO_Pin_All                ((uint16_t)0xFFFF)  /* 选择所有引脚 */

/* GPIO引脚功能重映射定义 */
/* PCFR1寄存器相关重映射 */
#define GPIO_Remap_SPI1             ((uint32_t)0x00000001)  /* SPI1复用功能重映射 */
#define GPIO_Remap_I2C1             ((uint32_t)0x00000002)  /* I2C1复用功能重映射 */
#define GPIO_Remap_USART1           ((uint32_t)0x00000004)  /* USART1复用功能重映射（低位） */
#define GPIO_Remap_USART2           ((uint32_t)0x00000008)  /* USART2复用功能重映射 */
#define GPIO_PartialRemap_USART3    ((uint32_t)0x00140010)  /* USART3部分复用功能重映射 */
#define GPIO_PartialRemap1_USART3   ((uint32_t)0x00140020)  /* USART3部分复用功能重映射1 */
#define GPIO_FullRemap_USART3       ((uint32_t)0x00140030)  /* USART3完全复用功能重映射 */
#define GPIO_PartialRemap_TIM1      ((uint32_t)0x00160040)  /* TIM1部分复用功能重映射 */
#define GPIO_FullRemap_TIM1         ((uint32_t)0x001600C0)  /* TIM1完全复用功能重映射 */
#define GPIO_PartialRemap1_TIM2     ((uint32_t)0x00180100)  /* TIM2部分复用功能重映射1 */
#define GPIO_PartialRemap2_TIM2     ((uint32_t)0x00180200)  /* TIM2部分复用功能重映射2 */
#define GPIO_FullRemap_TIM2         ((uint32_t)0x00180300)  /* TIM2完全复用功能重映射 */
#define GPIO_PartialRemap_TIM3      ((uint32_t)0x001A0800)  /* TIM3部分复用功能重映射 */
#define GPIO_FullRemap_TIM3         ((uint32_t)0x001A0C00)  /* TIM3完全复用功能重映射 */
#define GPIO_Remap_TIM4             ((uint32_t)0x00001000)  /* TIM4复用功能重映射 */
#define GPIO_Remap1_CAN1            ((uint32_t)0x001D4000)  /* CAN1复用功能重映射1 */
#define GPIO_Remap2_CAN1            ((uint32_t)0x001D6000)  /* CAN1复用功能重映射2 */
#define GPIO_Remap_PD0PD1           ((uint32_t)0x00008000)  /* PD0和PD1复用功能重映射 */
#define GPIO_Remap_TIM5CH4_LSI      ((uint32_t)0x00200001)  /* 将LSI连接到TIM5通道4输入捕获用于校准 */
#define GPIO_Remap_ADC1_ETRGINJ     ((uint32_t)0x00200002)  /* ADC1外部触发注入转换重映射 */
#define GPIO_Remap_ADC1_ETRGREG     ((uint32_t)0x00200004)  /* ADC1外部触发规则转换重映射 */
#define GPIO_Remap_ADC2_ETRGINJ     ((uint32_t)0x00200008)  /* ADC2外部触发注入转换重映射 */
#define GPIO_Remap_ADC2_ETRGREG     ((uint32_t)0x00200010)  /* ADC2外部触发规则转换重映射 */
#define GPIO_Remap_ETH              ((uint32_t)0x00200020)  /* 以太网重映射（仅用于互联系列器件） */
#define GPIO_Remap_CAN2             ((uint32_t)0x00200040)  /* CAN2重映射（仅用于互联系列器件） */
#define GPIO_Remap_MII_RMII_SEL     ((uint32_t)0x00200080)  /* MII或RMII接口选择 */
#define GPIO_Remap_SWJ_Disable      ((uint32_t)0x00300400)  /* 完全禁用SWJ调试接口（JTAG+SWD） */
#define GPIO_Remap_SPI3             ((uint32_t)0x00201000)  /* SPI3/I2S3复用功能重映射（仅用于互联系列器件） */
#define GPIO_Remap_TIM2ITR1_PTP_SOF ((uint32_t)0x00202000)  /* 将以太网PTP输出或USB OTG SOF（帧起始）连接到TIM2内部触发1用于校准（仅用于互联系列器件） */
#define GPIO_Remap_PTP_PPS          ((uint32_t)0x00204000)  /* 以太网MAC PPS_PTS输出重映射到PB05（仅用于互联系列器件） */
#define GPIO_Remap_PD01             GPIO_Remap_PD0PD1      /* PD0和PD1复用功能重映射（别名） */

/* PCFR2寄存器相关重映射 */
#define GPIO_Remap_TIM8             ((uint32_t)0x80000004)  /* TIM8复用功能重映射 */
#define GPIO_PartialRemap_TIM9      ((uint32_t)0x80130008)  /* TIM9部分复用功能重映射 */
#define GPIO_FullRemap_TIM9         ((uint32_t)0x80130010)  /* TIM9完全复用功能重映射 */
#define GPIO_PartialRemap_TIM10     ((uint32_t)0x80150020)  /* TIM10部分复用功能重映射 */
#define GPIO_FullRemap_TIM10        ((uint32_t)0x80150040)  /* TIM10完全复用功能重映射 */
#define GPIO_Remap_FSMC_NADV        ((uint32_t)0x80000400)  /* FSMC_NADV复用功能重映射 */
#define GPIO_PartialRemap_USART4    ((uint32_t)0x80300001)  /* USART4部分复用功能重映射 */
#define GPIO_FullRemap_USART4       ((uint32_t)0x80300002)  /* USART4完全复用功能重映射 */
#define GPIO_PartialRemap_USART5    ((uint32_t)0x80320004)  /* USART5部分复用功能重映射 */
#define GPIO_FullRemap_USART5       ((uint32_t)0x80320008)  /* USART5完全复用功能重映射 */
#define GPIO_PartialRemap_USART6    ((uint32_t)0x80340010)  /* USART6部分复用功能重映射 */
#define GPIO_FullRemap_USART6       ((uint32_t)0x80340020)  /* USART6完全复用功能重映射 */
#define GPIO_PartialRemap_USART7    ((uint32_t)0x80360040)  /* USART7部分复用功能重映射 */
#define GPIO_FullRemap_USART7       ((uint32_t)0x80360080)  /* USART7完全复用功能重映射 */
#define GPIO_PartialRemap_USART8    ((uint32_t)0x80380100)  /* USART8部分复用功能重映射 */
#define GPIO_FullRemap_USART8       ((uint32_t)0x80380200)  /* USART8完全复用功能重映射 */
#define GPIO_Remap_USART1_HighBit   ((uint32_t)0x80200400)  /* USART1复用功能重映射（高位） */


/* GPIO端口源定义（用于事件输出、外部中断等配置） */
#define GPIO_PortSourceGPIOA        ((uint8_t)0x00)  /* GPIOA端口 */
#define GPIO_PortSourceGPIOB        ((uint8_t)0x01)  /* GPIOB端口 */
#define GPIO_PortSourceGPIOC        ((uint8_t)0x02)  /* GPIOC端口 */
#define GPIO_PortSourceGPIOD        ((uint8_t)0x03)  /* GPIOD端口 */
#define GPIO_PortSourceGPIOE        ((uint8_t)0x04)  /* GPIOE端口 */

/* GPIO引脚源定义（用于事件输出、外部中断等配置） */
#define GPIO_PinSource0             ((uint8_t)0x00)  /* 引脚源0 */
#define GPIO_PinSource1             ((uint8_t)0x01)  /* 引脚源1 */
#define GPIO_PinSource2             ((uint8_t)0x02)  /* 引脚源2 */
#define GPIO_PinSource3             ((uint8_t)0x03)  /* 引脚源3 */
#define GPIO_PinSource4             ((uint8_t)0x04)  /* 引脚源4 */
#define GPIO_PinSource5             ((uint8_t)0x05)  /* 引脚源5 */
#define GPIO_PinSource6             ((uint8_t)0x06)  /* 引脚源6 */
#define GPIO_PinSource7             ((uint8_t)0x07)  /* 引脚源7 */
#define GPIO_PinSource8             ((uint8_t)0x08)  /* 引脚源8 */
#define GPIO_PinSource9             ((uint8_t)0x09)  /* 引脚源9 */
#define GPIO_PinSource10            ((uint8_t)0x0A)  /* 引脚源10 */
#define GPIO_PinSource11            ((uint8_t)0x0B)  /* 引脚源11 */
#define GPIO_PinSource12            ((uint8_t)0x0C)  /* 引脚源12 */
#define GPIO_PinSource13            ((uint8_t)0x0D)  /* 引脚源13 */
#define GPIO_PinSource14            ((uint8_t)0x0E)  /* 引脚源14 */
#define GPIO_PinSource15            ((uint8_t)0x0F)  /* 引脚源15 */

/* 以太网物理层接口模式选择 */
#define GPIO_ETH_MediaInterface_MII    ((u32)0x00000000)  /* 选择MII接口 */
#define GPIO_ETH_MediaInterface_RMII   ((u32)0x00000001)  /* 选择RMII接口 */


/* 函数声明 */
void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_AFIODeInit(void);
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_ToggleBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_EventOutputCmd(FunctionalState NewState);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_ETH_MediaInterfaceConfig(uint32_t GPIO_ETH_MediaInterface);

#ifdef __cplusplus
}
#endif

#endif