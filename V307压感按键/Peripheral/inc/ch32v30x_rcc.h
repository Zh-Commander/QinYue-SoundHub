/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_rcc.h
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2024/03/06
* 描述        : 此文件提供了所有RCC固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意：本软件（修改或未修改）及二进制文件仅用于南京沁恒微电子生产的单片机。
*******************************************************************************/
#ifndef __CH32V30x_RCC_H
#define __CH32V30x_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* RCC导出的类型定义 */
typedef struct
{
  uint32_t SYSCLK_Frequency;  /* 返回以Hz为单位的SYSCLK时钟频率 */
  uint32_t HCLK_Frequency;    /* 返回以Hz为单位的HCLK时钟频率 */
  uint32_t PCLK1_Frequency;   /* 返回以Hz为单位的PCLK1时钟频率 */
  uint32_t PCLK2_Frequency;   /* 返回以Hz为单位的PCLK2时钟频率 */
  uint32_t ADCCLK_Frequency;  /* 返回以Hz为单位的ADCCLK时钟频率 */
}RCC_ClocksTypeDef;

/* HSE（外部高速时钟）配置 */
#define RCC_HSE_OFF                      ((uint32_t)0x00000000) /* 关闭HSE */
#define RCC_HSE_ON                       ((uint32_t)0x00010000) /* 开启HSE */
#define RCC_HSE_Bypass                   ((uint32_t)0x00040000) /* HSE旁路模式，使用外部时钟源直接输入 */

/* PLL（锁相环）输入时钟源选择 */
#define RCC_PLLSource_HSI_Div2           ((uint32_t)0x00000000) /* PLL时钟源为HSI（内部高速时钟）除以2 */

#ifdef CH32V30x_D8
/* 对于D8系列，PLL时钟源可以是HSE分频 */
#define RCC_PLLSource_HSE_Div1           ((uint32_t)0x00010000) /* PLL时钟源为HSE（外部高速时钟）除以1 */
#define RCC_PLLSource_HSE_Div2           ((uint32_t)0x00030000) /* PLL时钟源为HSE除以2 */

#else
/* 对于非D8系列，PLL时钟源为PREDIV1输出 */
#define RCC_PLLSource_PREDIV1            ((uint32_t)0x00010000) /* PLL时钟源为PREDIV1分频器输出 */

#endif

/* PLL倍频系数选择 */
#ifdef CH32V30x_D8
/* D8系列的PLL倍频系数 */
#define RCC_PLLMul_2                     ((uint32_t)0x00000000) /* PLL倍频系数为2 */
#define RCC_PLLMul_3                     ((uint32_t)0x00040000) /* PLL倍频系数为3 */
#define RCC_PLLMul_4                     ((uint32_t)0x00080000) /* PLL倍频系数为4 */
#define RCC_PLLMul_5                     ((uint32_t)0x000C0000) /* PLL倍频系数为5 */
#define RCC_PLLMul_6                     ((uint32_t)0x00100000) /* PLL倍频系数为6 */
#define RCC_PLLMul_7                     ((uint32_t)0x00140000) /* PLL倍频系数为7 */
#define RCC_PLLMul_8                     ((uint32_t)0x00180000) /* PLL倍频系数为8 */
#define RCC_PLLMul_9                     ((uint32_t)0x001C0000) /* PLL倍频系数为9 */
#define RCC_PLLMul_10                    ((uint32_t)0x00200000) /* PLL倍频系数为10 */
#define RCC_PLLMul_11                    ((uint32_t)0x00240000) /* PLL倍频系数为11 */
#define RCC_PLLMul_12                    ((uint32_t)0x00280000) /* PLL倍频系数为12 */
#define RCC_PLLMul_13                    ((uint32_t)0x002C0000) /* PLL倍频系数为13 */
#define RCC_PLLMul_14                    ((uint32_t)0x00300000) /* PLL倍频系数为14 */
#define RCC_PLLMul_15                    ((uint32_t)0x00340000) /* PLL倍频系数为15 */
#define RCC_PLLMul_16                    ((uint32_t)0x00380000) /* PLL倍频系数为16 */
#define RCC_PLLMul_18                    ((uint32_t)0x003C0000) /* PLL倍频系数为18 */

#else
/* 非D8系列的PLL倍频系数 */
#define RCC_PLLMul_18_EXTEN              ((uint32_t)0x00000000) /* PLL倍频系数为18（扩展） */
#define RCC_PLLMul_3_EXTEN               ((uint32_t)0x00040000) /* PLL倍频系数为3（扩展） */
#define RCC_PLLMul_4_EXTEN               ((uint32_t)0x00080000) /* PLL倍频系数为4（扩展） */
#define RCC_PLLMul_5_EXTEN               ((uint32_t)0x000C0000) /* PLL倍频系数为5（扩展） */
#define RCC_PLLMul_6_EXTEN               ((uint32_t)0x00100000) /* PLL倍频系数为6（扩展） */
#define RCC_PLLMul_7_EXTEN               ((uint32_t)0x00140000) /* PLL倍频系数为7（扩展） */
#define RCC_PLLMul_8_EXTEN               ((uint32_t)0x00180000) /* PLL倍频系数为8（扩展） */
#define RCC_PLLMul_9_EXTEN               ((uint32_t)0x001C0000) /* PLL倍频系数为9（扩展） */
#define RCC_PLLMul_10_EXTEN              ((uint32_t)0x00200000) /* PLL倍频系数为10（扩展） */
#define RCC_PLLMul_11_EXTEN              ((uint32_t)0x00240000) /* PLL倍频系数为11（扩展） */
#define RCC_PLLMul_12_EXTEN              ((uint32_t)0x00280000) /* PLL倍频系数为12（扩展） */
#define RCC_PLLMul_13_EXTEN              ((uint32_t)0x002C0000) /* PLL倍频系数为13（扩展） */
#define RCC_PLLMul_14_EXTEN              ((uint32_t)0x00300000) /* PLL倍频系数为14（扩展） */
#define RCC_PLLMul_6_5_EXTEN             ((uint32_t)0x00340000) /* PLL倍频系数为6.5（扩展） */
#define RCC_PLLMul_15_EXTEN              ((uint32_t)0x00380000) /* PLL倍频系数为15（扩展） */
#define RCC_PLLMul_16_EXTEN              ((uint32_t)0x003C0000) /* PLL倍频系数为16（扩展） */

#endif

/* PREDIV1分频系数（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_PREDIV1_Div1                 ((uint32_t)0x00000000) /* PREDIV1分频系数为1 */
#define RCC_PREDIV1_Div2                 ((uint32_t)0x00000001) /* PREDIV1分频系数为2 */
#define RCC_PREDIV1_Div3                 ((uint32_t)0x00000002) /* PREDIV1分频系数为3 */
#define RCC_PREDIV1_Div4                 ((uint32_t)0x00000003) /* PREDIV1分频系数为4 */
#define RCC_PREDIV1_Div5                 ((uint32_t)0x00000004) /* PREDIV1分频系数为5 */
#define RCC_PREDIV1_Div6                 ((uint32_t)0x00000005) /* PREDIV1分频系数为6 */
#define RCC_PREDIV1_Div7                 ((uint32_t)0x00000006) /* PREDIV1分频系数为7 */
#define RCC_PREDIV1_Div8                 ((uint32_t)0x00000007) /* PREDIV1分频系数为8 */
#define RCC_PREDIV1_Div9                 ((uint32_t)0x00000008) /* PREDIV1分频系数为9 */
#define RCC_PREDIV1_Div10                ((uint32_t)0x00000009) /* PREDIV1分频系数为10 */
#define RCC_PREDIV1_Div11                ((uint32_t)0x0000000A) /* PREDIV1分频系数为11 */
#define RCC_PREDIV1_Div12                ((uint32_t)0x0000000B) /* PREDIV1分频系数为12 */
#define RCC_PREDIV1_Div13                ((uint32_t)0x0000000C) /* PREDIV1分频系数为13 */
#define RCC_PREDIV1_Div14                ((uint32_t)0x0000000D) /* PREDIV1分频系数为14 */
#define RCC_PREDIV1_Div15                ((uint32_t)0x0000000E) /* PREDIV1分频系数为15 */
#define RCC_PREDIV1_Div16                ((uint32_t)0x0000000F) /* PREDIV1分频系数为16 */

#endif

/* PREDIV1时钟源选择（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_PREDIV1_Source_HSE           ((uint32_t)0x00000000) /* PREDIV1时钟源为HSE */
#define RCC_PREDIV1_Source_PLL2          ((uint32_t)0x00010000) /* PREDIV1时钟源为PLL2输出 */

#endif

/* PREDIV2分频系数（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_PREDIV2_Div1                 ((uint32_t)0x00000000) /* PREDIV2分频系数为1 */
#define RCC_PREDIV2_Div2                 ((uint32_t)0x00000010) /* PREDIV2分频系数为2 */
#define RCC_PREDIV2_Div3                 ((uint32_t)0x00000020) /* PREDIV2分频系数为3 */
#define RCC_PREDIV2_Div4                 ((uint32_t)0x00000030) /* PREDIV2分频系数为4 */
#define RCC_PREDIV2_Div5                 ((uint32_t)0x00000040) /* PREDIV2分频系数为5 */
#define RCC_PREDIV2_Div6                 ((uint32_t)0x00000050) /* PREDIV2分频系数为6 */
#define RCC_PREDIV2_Div7                 ((uint32_t)0x00000060) /* PREDIV2分频系数为7 */
#define RCC_PREDIV2_Div8                 ((uint32_t)0x00000070) /* PREDIV2分频系数为8 */
#define RCC_PREDIV2_Div9                 ((uint32_t)0x00000080) /* PREDIV2分频系数为9 */
#define RCC_PREDIV2_Div10                ((uint32_t)0x00000090) /* PREDIV2分频系数为10 */
#define RCC_PREDIV2_Div11                ((uint32_t)0x000000A0) /* PREDIV2分频系数为11 */
#define RCC_PREDIV2_Div12                ((uint32_t)0x000000B0) /* PREDIV2分频系数为12 */
#define RCC_PREDIV2_Div13                ((uint32_t)0x000000C0) /* PREDIV2分频系数为13 */
#define RCC_PREDIV2_Div14                ((uint32_t)0x000000D0) /* PREDIV2分频系数为14 */
#define RCC_PREDIV2_Div15                ((uint32_t)0x000000E0) /* PREDIV2分频系数为15 */
#define RCC_PREDIV2_Div16                ((uint32_t)0x000000F0) /* PREDIV2分频系数为16 */

#endif

/* PLL2倍频系数（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_PLL2Mul_2_5                  ((uint32_t)0x00000000) /* PLL2倍频系数为2.5 */
#define RCC_PLL2Mul_12_5                 ((uint32_t)0x00000100) /* PLL2倍频系数为12.5 */
#define RCC_PLL2Mul_4                    ((uint32_t)0x00000200) /* PLL2倍频系数为4 */
#define RCC_PLL2Mul_5                    ((uint32_t)0x00000300) /* PLL2倍频系数为5 */
#define RCC_PLL2Mul_6                    ((uint32_t)0x00000400) /* PLL2倍频系数为6 */
#define RCC_PLL2Mul_7                    ((uint32_t)0x00000500) /* PLL2倍频系数为7 */
#define RCC_PLL2Mul_8                    ((uint32_t)0x00000600) /* PLL2倍频系数为8 */
#define RCC_PLL2Mul_9                    ((uint32_t)0x00000700) /* PLL2倍频系数为9 */
#define RCC_PLL2Mul_10                   ((uint32_t)0x00000800) /* PLL2倍频系数为10 */
#define RCC_PLL2Mul_11                   ((uint32_t)0x00000900) /* PLL2倍频系数为11 */
#define RCC_PLL2Mul_12                   ((uint32_t)0x00000A00) /* PLL2倍频系数为12 */
#define RCC_PLL2Mul_13                   ((uint32_t)0x00000B00) /* PLL2倍频系数为13 */
#define RCC_PLL2Mul_14                   ((uint32_t)0x00000C00) /* PLL2倍频系数为14 */
#define RCC_PLL2Mul_15                   ((uint32_t)0x00000D00) /* PLL2倍频系数为15 */
#define RCC_PLL2Mul_16                   ((uint32_t)0x00000E00) /* PLL2倍频系数为16 */
#define RCC_PLL2Mul_20                   ((uint32_t)0x00000F00) /* PLL2倍频系数为20 */

#endif

/* PLL3倍频系数（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_PLL3Mul_2_5                  ((uint32_t)0x00000000) /* PLL3倍频系数为2.5 */
#define RCC_PLL3Mul_12_5                 ((uint32_t)0x00001000) /* PLL3倍频系数为12.5 */
#define RCC_PLL3Mul_4                    ((uint32_t)0x00002000) /* PLL3倍频系数为4 */
#define RCC_PLL3Mul_5                    ((uint32_t)0x00003000) /* PLL3倍频系数为5 */
#define RCC_PLL3Mul_6                    ((uint32_t)0x00004000) /* PLL3倍频系数为6 */
#define RCC_PLL3Mul_7                    ((uint32_t)0x00005000) /* PLL3倍频系数为7 */
#define RCC_PLL3Mul_8                    ((uint32_t)0x00006000) /* PLL3倍频系数为8 */
#define RCC_PLL3Mul_9                    ((uint32_t)0x00007000) /* PLL3倍频系数为9 */
#define RCC_PLL3Mul_10                   ((uint32_t)0x00008000) /* PLL3倍频系数为10 */
#define RCC_PLL3Mul_11                   ((uint32_t)0x00009000) /* PLL3倍频系数为11 */
#define RCC_PLL3Mul_12                   ((uint32_t)0x0000A000) /* PLL3倍频系数为12 */
#define RCC_PLL3Mul_13                   ((uint32_t)0x0000B000) /* PLL3倍频系数为13 */
#define RCC_PLL3Mul_14                   ((uint32_t)0x0000C000) /* PLL3倍频系数为14 */
#define RCC_PLL3Mul_15                   ((uint32_t)0x0000D000) /* PLL3倍频系数为15 */
#define RCC_PLL3Mul_16                   ((uint32_t)0x0000E000) /* PLL3倍频系数为16 */
#define RCC_PLL3Mul_20                   ((uint32_t)0x0000F000) /* PLL3倍频系数为20 */

#endif

/* 系统时钟源选择 */
#define RCC_SYSCLKSource_HSI             ((uint32_t)0x00000000) /* 系统时钟源为HSI */
#define RCC_SYSCLKSource_HSE             ((uint32_t)0x00000001) /* 系统时钟源为HSE */
#define RCC_SYSCLKSource_PLLCLK          ((uint32_t)0x00000002) /* 系统时钟源为PLL输出 */

/* AHB总线时钟分频系数（HCLK） */
#define RCC_SYSCLK_Div1                  ((uint32_t)0x00000000) /* 系统时钟不分频作为HCLK */
#define RCC_SYSCLK_Div2                  ((uint32_t)0x00000080) /* 系统时钟2分频作为HCLK */
#define RCC_SYSCLK_Div4                  ((uint32_t)0x00000090) /* 系统时钟4分频作为HCLK */
#define RCC_SYSCLK_Div8                  ((uint32_t)0x000000A0) /* 系统时钟8分频作为HCLK */
#define RCC_SYSCLK_Div16                 ((uint32_t)0x000000B0) /* 系统时钟16分频作为HCLK */
#define RCC_SYSCLK_Div64                 ((uint32_t)0x000000C0) /* 系统时钟64分频作为HCLK */
#define RCC_SYSCLK_Div128                ((uint32_t)0x000000D0) /* 系统时钟128分频作为HCLK */
#define RCC_SYSCLK_Div256                ((uint32_t)0x000000E0) /* 系统时钟256分频作为HCLK */
#define RCC_SYSCLK_Div512                ((uint32_t)0x000000F0) /* 系统时钟512分频作为HCLK */

/* APB1和APB2总线时钟分频系数（PCLK1和PCLK2） */
#define RCC_HCLK_Div1                    ((uint32_t)0x00000000) /* HCLK不分频作为APB时钟 */
#define RCC_HCLK_Div2                    ((uint32_t)0x00000400) /* HCLK2分频作为APB时钟 */
#define RCC_HCLK_Div4                    ((uint32_t)0x00000500) /* HCLK4分频作为APB时钟 */
#define RCC_HCLK_Div8                    ((uint32_t)0x00000600) /* HCLK8分频作为APB时钟 */
#define RCC_HCLK_Div16                   ((uint32_t)0x00000700) /* HCLK16分频作为APB时钟 */

/* RCC中断源定义 */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01) /* LSI就绪中断 */
#define RCC_IT_LSERDY                    ((uint8_t)0x02) /* LSE就绪中断 */
#define RCC_IT_HSIRDY                    ((uint8_t)0x04) /* HSI就绪中断 */
#define RCC_IT_HSERDY                    ((uint8_t)0x08) /* HSE就绪中断 */
#define RCC_IT_PLLRDY                    ((uint8_t)0x10) /* PLL就绪中断 */
#define RCC_IT_CSS                       ((uint8_t)0x80) /* 时钟安全系统中断 */

#ifdef CH32V30x_D8C
/* 仅D8C系列支持的中断源 */
#define RCC_IT_PLL2RDY                   ((uint8_t)0x20) /* PLL2就绪中断 */
#define RCC_IT_PLL3RDY                   ((uint8_t)0x40) /* PLL3就绪中断 */

#endif

/* USB全速时钟源选择 */
#define RCC_USBFSCLKSource_PLLCLK_Div1   ((uint8_t)0x00) /* USB全速时钟源为PLL时钟不分频 */
#define RCC_USBFSCLKSource_PLLCLK_Div2   ((uint8_t)0x01) /* USB全速时钟源为PLL时钟2分频 */
#define RCC_USBFSCLKSource_PLLCLK_Div3   ((uint8_t)0x02) /* USB全速时钟源为PLL时钟3分频 */

/* OTG全速时钟源选择（与USB全速相同） */
#define RCC_OTGFSCLKSource_PLLCLK_Div1   RCC_USBFSCLKSource_PLLCLK_Div1
#define RCC_OTGFSCLKSource_PLLCLK_Div2   RCC_USBFSCLKSource_PLLCLK_Div2
#define RCC_OTGFSCLKSource_PLLCLK_Div3   RCC_USBFSCLKSource_PLLCLK_Div3

/* I2S2时钟源选择（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_I2S2CLKSource_SYSCLK         ((uint8_t)0x00) /* I2S2时钟源为系统时钟 */
#define RCC_I2S2CLKSource_PLL3_VCO       ((uint8_t)0x01) /* I2S2时钟源为PLL3 VCO输出 */

#endif

/* I2S3时钟源选择（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_I2S3CLKSource_SYSCLK         ((uint8_t)0x00) /* I2S3时钟源为系统时钟 */
#define RCC_I2S3CLKSource_PLL3_VCO       ((uint8_t)0x01) /* I2S3时钟源为PLL3 VCO输出 */

#endif

/* ADC时钟分频系数（来自PCLK2） */
#define RCC_PCLK2_Div2                   ((uint32_t)0x00000000) /* ADC时钟为PCLK2的2分频 */
#define RCC_PCLK2_Div4                   ((uint32_t)0x00004000) /* ADC时钟为PCLK2的4分频 */
#define RCC_PCLK2_Div6                   ((uint32_t)0x00008000) /* ADC时钟为PCLK2的6分频 */
#define RCC_PCLK2_Div8                   ((uint32_t)0x0000C000) /* ADC时钟为PCLK2的8分频 */

/* LSE（外部低速时钟）配置 */
#define RCC_LSE_OFF                      ((uint8_t)0x00) /* 关闭LSE */
#define RCC_LSE_ON                       ((uint8_t)0x01) /* 开启LSE */
#define RCC_LSE_Bypass                   ((uint8_t)0x04) /* LSE旁路模式，使用外部时钟源直接输入 */

/* RTC时钟源选择 */
#define RCC_RTCCLKSource_LSE             ((uint32_t)0x00000100) /* RTC时钟源为LSE */
#define RCC_RTCCLKSource_LSI             ((uint32_t)0x00000200) /* RTC时钟源为LSI */
#define RCC_RTCCLKSource_HSE_Div128      ((uint32_t)0x00000300) /* RTC时钟源为HSE128分频 */

/* AHB外设时钟使能/禁用定义 */
#define RCC_AHBPeriph_DMA1               ((uint32_t)0x00000001) /* DMA1时钟 */
#define RCC_AHBPeriph_DMA2               ((uint32_t)0x00000002) /* DMA2时钟 */
#define RCC_AHBPeriph_SRAM               ((uint32_t)0x00000004) /* SRAM时钟 */
#define RCC_AHBPeriph_CRC                ((uint32_t)0x00000040) /* CRC时钟 */
#define RCC_AHBPeriph_FSMC               ((uint32_t)0x00000100) /* FSMC时钟 */
#define RCC_AHBPeriph_RNG                ((uint32_t)0x00000200) /* RNG（随机数发生器）时钟 */
#define RCC_AHBPeriph_SDIO               ((uint32_t)0x00000400) /* SDIO时钟 */
#define RCC_AHBPeriph_USBHS              ((uint32_t)0x00000800) /* USB高速时钟 */
#define RCC_AHBPeriph_USBFS              ((uint32_t)0x00001000) /* USB全速时钟 */
#define RCC_AHBPeriph_DVP                ((uint32_t)0x00002000) /* DVP（数字视频端口）时钟 */
#define RCC_AHBPeriph_ETH_MAC            ((uint32_t)0x00004000) /* 以太网MAC时钟 */
#define RCC_AHBPeriph_ETH_MAC_Tx         ((uint32_t)0x00008000) /* 以太网MAC发送时钟 */
#define RCC_AHBPeriph_ETH_MAC_Rx         ((uint32_t)0x00010000) /* 以太网MAC接收时钟 */
#define RCC_AHBPeriph_OTG_FS             RCC_AHBPeriph_USBFS /* OTG全速时钟（与USB全速相同） */

/* APB2外设时钟使能/禁用定义 */
#define RCC_APB2Periph_AFIO              ((uint32_t)0x00000001) /* AFIO（复用功能IO）时钟 */
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004) /* GPIOA时钟 */
#define RCC_APB2Periph_GPIOB             ((uint32_t)0x00000008) /* GPIOB时钟 */
#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010) /* GPIOC时钟 */
#define RCC_APB2Periph_GPIOD             ((uint32_t)0x00000020) /* GPIOD时钟 */
#define RCC_APB2Periph_GPIOE             ((uint32_t)0x00000040) /* GPIOE时钟 */
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000200) /* ADC1时钟 */
#define RCC_APB2Periph_ADC2              ((uint32_t)0x00000400) /* ADC2时钟 */
#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000800) /* TIM1时钟 */
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000) /* SPI1时钟 */
#define RCC_APB2Periph_TIM8              ((uint32_t)0x00002000) /* TIM8时钟 */
#define RCC_APB2Periph_USART1            ((uint32_t)0x00004000) /* USART1时钟 */
#define RCC_APB2Periph_TIM9              ((uint32_t)0x00080000) /* TIM9时钟 */
#define RCC_APB2Periph_TIM10             ((uint32_t)0x00100000) /* TIM10时钟 */

/* APB1外设时钟使能/禁用定义 */
#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001) /* TIM2时钟 */
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000002) /* TIM3时钟 */
#define RCC_APB1Periph_TIM4              ((uint32_t)0x00000004) /* TIM4时钟 */
#define RCC_APB1Periph_TIM5              ((uint32_t)0x00000008) /* TIM5时钟 */
#define RCC_APB1Periph_TIM6              ((uint32_t)0x00000010) /* TIM6时钟 */
#define RCC_APB1Periph_TIM7              ((uint32_t)0x00000020) /* TIM7时钟 */
#define RCC_APB1Periph_UART6             ((uint32_t)0x00000040) /* UART6时钟 */
#define RCC_APB1Periph_UART7             ((uint32_t)0x00000080) /* UART7时钟 */
#define RCC_APB1Periph_UART8             ((uint32_t)0x00000100) /* UART8时钟 */
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800) /* WWDG（窗口看门狗）时钟 */
#define RCC_APB1Periph_SPI2              ((uint32_t)0x00004000) /* SPI2时钟 */
#define RCC_APB1Periph_SPI3              ((uint32_t)0x00008000) /* SPI3时钟 */
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000) /* USART2时钟 */
#define RCC_APB1Periph_USART3            ((uint32_t)0x00040000) /* USART3时钟 */
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000) /* UART4时钟 */
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000) /* UART5时钟 */
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000) /* I2C1时钟 */
#define RCC_APB1Periph_I2C2              ((uint32_t)0x00400000) /* I2C2时钟 */
#define RCC_APB1Periph_USB               ((uint32_t)0x00800000) /* USB时钟 */
#define RCC_APB1Periph_CAN1              ((uint32_t)0x02000000) /* CAN1时钟 */
#define RCC_APB1Periph_CAN2              ((uint32_t)0x04000000) /* CAN2时钟 */
#define RCC_APB1Periph_BKP               ((uint32_t)0x08000000) /* BKP（备份寄存器）时钟 */
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000) /* PWR（电源控制）时钟 */
#define RCC_APB1Periph_DAC               ((uint32_t)0x20000000) /* DAC时钟 */

/* 微控制器时钟输出（MCO）引脚时钟源选择 */
#define RCC_MCO_NoClock                  ((uint8_t)0x00) /* 无时钟输出 */
#define RCC_MCO_SYSCLK                   ((uint8_t)0x04) /* 输出系统时钟 */
#define RCC_MCO_HSI                      ((uint8_t)0x05) /* 输出HSI时钟 */
#define RCC_MCO_HSE                      ((uint8_t)0x06) /* 输出HSE时钟 */
#define RCC_MCO_PLLCLK_Div2              ((uint8_t)0x07) /* 输出PLL时钟的2分频 */

#ifdef CH32V30x_D8C
/* 仅D8C系列支持的MCO时钟源 */
#define RCC_MCO_PLL2CLK                  ((uint8_t)0x08) /* 输出PLL2时钟 */
#define RCC_MCO_PLL3CLK_Div2             ((uint8_t)0x09) /* 输出PLL3时钟的2分频 */
#define RCC_MCO_XT1                      ((uint8_t)0x0A) /* 输出XT1时钟（外部时钟） */
#define RCC_MCO_PLL3CLK                  ((uint8_t)0x0B) /* 输出PLL3时钟 */

#endif

/* RCC标志位定义 */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21) /* HSI就绪标志 */
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31) /* HSE就绪标志 */
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39) /* PLL就绪标志 */
#define RCC_FLAG_LSERDY                  ((uint8_t)0x41) /* LSE就绪标志 */
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61) /* LSI就绪标志 */
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A) /* 引脚复位标志 */
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B) /* 上电/掉电复位标志 */
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C) /* 软件复位标志 */
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D) /* 独立看门狗复位标志 */
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E) /* 窗口看门狗复位标志 */
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F) /* 低功耗复位标志 */

#ifdef CH32V30x_D8C
/* 仅D8C系列支持的标志位 */
#define RCC_FLAG_PLL2RDY                 ((uint8_t)0x3B) /* PLL2就绪标志 */
#define RCC_FLAG_PLL3RDY                 ((uint8_t)0x3D) /* PLL3就绪标志 */

#endif

/* SysTick时钟源选择 */
#define SysTick_CLKSource_HCLK_Div8      ((uint32_t)0xFFFFFFFB) /* SysTick时钟源为HCLK的8分频 */
#define SysTick_CLKSource_HCLK           ((uint32_t)0x00000004) /* SysTick时钟源为HCLK */

/* RNG（随机数发生器）时钟源选择（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_RNGCLKSource_SYSCLK          ((uint32_t)0x00) /* RNG时钟源为系统时钟 */
#define RCC_RNGCLKSource_PLL3_VCO        ((uint32_t)0x01) /* RNG时钟源为PLL3 VCO输出 */

#endif

/* 以太网1G时钟源选择（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_ETH1GCLKSource_PLL2_VCO      ((uint32_t)0x00) /* 以太网1G时钟源为PLL2 VCO输出 */
#define RCC_ETH1GCLKSource_PLL3_VCO      ((uint32_t)0x01) /* 以太网1G时钟源为PLL3 VCO输出 */
#define RCC_ETH1GCLKSource_PB1_IN        ((uint32_t)0x02) /* 以太网1G时钟源为PB1引脚输入 */

#endif

/* USB PLL分频系数（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_USBPLL_Div1                  ((uint32_t)0x00) /* USB PLL输出不分频 */
#define RCC_USBPLL_Div2                  ((uint32_t)0x01) /* USB PLL输出2分频 */
#define RCC_USBPLL_Div3                  ((uint32_t)0x02) /* USB PLL输出3分频 */
#define RCC_USBPLL_Div4                  ((uint32_t)0x03) /* USB PLL输出4分频 */
#define RCC_USBPLL_Div5                  ((uint32_t)0x04) /* USB PLL输出5分频 */
#define RCC_USBPLL_Div6                  ((uint32_t)0x05) /* USB PLL输出6分频 */
#define RCC_USBPLL_Div7                  ((uint32_t)0x06) /* USB PLL输出7分频 */
#define RCC_USBPLL_Div8                  ((uint32_t)0x07) /* USB PLL输出8分频 */

#endif

/* USB高速PLL时钟源选择（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_HSBHSPLLCLKSource_HSE        ((uint32_t)0x00) /* USB高速PLL时钟源为HSE */
#define RCC_HSBHSPLLCLKSource_HSI        ((uint32_t)0x01) /* USB高速PLL时钟源为HSI */

#endif

/* USB高速PLL参考时钟选择（仅D8C系列支持） */
#ifdef CH32V30x_D8C
#define RCC_USBHSPLLCKREFCLK_3M          ((uint32_t)0x00) /* USB高速PLL参考时钟为3MHz */
#define RCC_USBHSPLLCKREFCLK_4M          ((uint32_t)0x01) /* USB高速PLL参考时钟为4MHz */
#define RCC_USBHSPLLCKREFCLK_8M          ((uint32_t)0x02) /* USB高速PLL参考时钟为8MHz */
#define RCC_USBHSPLLCKREFCLK_5M          ((uint32_t)0x03) /* USB高速PLL参考时钟为5MHz */

#endif

/* OTG USB 48MHz时钟源选择 */
#define RCC_USBCLK48MCLKSource_PLLCLK    ((uint32_t)0x00) /* USB 48MHz时钟源为PLL时钟 */
#define RCC_USBCLK48MCLKSource_USBPHY    ((uint32_t)0x01) /* USB 48MHz时钟源为USB PHY（物理层） */


/* 函数声明部分 */
void RCC_DeInit(void); /* 将RCC寄存器重置为默认值 */
void RCC_HSEConfig(uint32_t RCC_HSE); /* 配置外部高速振荡器（HSE） */
ErrorStatus RCC_WaitForHSEStartUp(void); /* 等待HSE启动就绪 */
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue); /* 调整HSI校准值 */
void RCC_HSICmd(FunctionalState NewState); /* 使能或禁用内部高速振荡器（HSI） */
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul); /* 配置PLL时钟源和倍频系数 */
void RCC_PLLCmd(FunctionalState NewState); /* 使能或禁用PLL */
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource); /* 配置系统时钟源 */
uint8_t RCC_GetSYSCLKSource(void); /* 获取当前系统时钟源 */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK); /* 配置AHB总线时钟（HCLK）分频系数 */
void RCC_PCLK1Config(uint32_t RCC_HCLK); /* 配置APB1总线时钟（PCLK1）分频系数 */
void RCC_PCLK2Config(uint32_t RCC_HCLK); /* 配置APB2总线时钟（PCLK2）分频系数 */
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState); /* 使能或禁用指定的RCC中断 */
void RCC_ADCCLKConfig(uint32_t RCC_PCLK2); /* 配置ADC时钟分频系数 */
void RCC_LSEConfig(uint8_t RCC_LSE); /* 配置外部低速振荡器（LSE） */
void RCC_LSICmd(FunctionalState NewState); /* 使能或禁用内部低速振荡器（LSI） */
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource); /* 配置RTC时钟源 */
void RCC_RTCCLKCmd(FunctionalState NewState); /* 使能或禁用RTC时钟 */
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks); /* 获取各个时钟频率 */
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState); /* 使能或禁用AHB外设时钟 */
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState); /* 使能或禁用APB2外设时钟 */
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState); /* 使能或禁用APB1外设时钟 */
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState); /* 使能或禁用APB2外设复位 */
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState); /* 使能或禁用APB1外设复位 */
void RCC_BackupResetCmd(FunctionalState NewState); /* 使能或禁用备份域复位 */
void RCC_ClockSecuritySystemCmd(FunctionalState NewState); /* 使能或禁用时钟安全系统（CSS） */
void RCC_MCOConfig(uint8_t RCC_MCO); /* 配置微控制器时钟输出（MCO）引脚 */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG); /* 检查指定的RCC标志位是否设置 */
void RCC_ClearFlag(void); /* 清除RCC复位标志位 */
ITStatus RCC_GetITStatus(uint8_t RCC_IT); /* 检查指定的RCC中断是否发生 */
void RCC_ClearITPendingBit(uint8_t RCC_IT); /* 清除RCC中断挂起位 */
void RCC_ADCCLKADJcmd(FunctionalState NewState); /* 使能或禁用ADC时钟调整 */
void RCC_USBFSCLKConfig(uint32_t RCC_USBFSCLKSource); /* 配置USB全速时钟源 */
void RCC_USBCLK48MConfig(uint32_t RCC_USBCLK48MSource); /* 配置USB 48MHz时钟源 */
#define RCC_OTGFSCLKConfig  RCC_USBFSCLKConfig /* OTG全速时钟配置宏，与USB全速相同 */

#ifdef CH32V30x_D8C
/* 仅D8C系列支持的函数 */
void RCC_PREDIV1Config(uint32_t RCC_PREDIV1_Source, uint32_t RCC_PREDIV1_Div); /* 配置PREDIV1时钟源和分频系数 */
void RCC_PREDIV2Config(uint32_t RCC_PREDIV2_Div); /* 配置PREDIV2分频系数 */
void RCC_PLL2Config(uint32_t RCC_PLL2Mul); /* 配置PLL2倍频系数 */
void RCC_PLL2Cmd(FunctionalState NewState); /* 使能或禁用PLL2 */
void RCC_PLL3Config(uint32_t RCC_PLL3Mul); /* 配置PLL3倍频系数 */
void RCC_PLL3Cmd(FunctionalState NewState); /* 使能或禁用PLL3 */
void RCC_I2S2CLKConfig(uint32_t RCC_I2S2CLKSource); /* 配置I2S2时钟源 */
void RCC_I2S3CLKConfig(uint32_t RCC_I2S3CLKSource); /* 配置I2S3时钟源 */
void RCC_AHBPeriphResetCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState); /* 使能或禁用AHB外设复位 */
void RCC_RNGCLKConfig(uint32_t RCC_RNGCLKSource); /* 配置RNG时钟源 */
void RCC_ETH1GCLKConfig(uint32_t RCC_ETH1GCLKSource); /* 配置以太网1G时钟源 */
void RCC_ETH1G_125Mcmd(FunctionalState NewState); /* 使能或禁用以太网125MHz时钟输出 */
void RCC_USBHSConfig(uint32_t RCC_USBHS); /* 配置USB高速时钟 */
void RCC_USBHSPLLCLKConfig(uint32_t RCC_USBHSPLLCLKSource); /* 配置USB高速PLL时钟源 */
void RCC_USBHSPLLCKREFCLKConfig(uint32_t RCC_USBHSPLLCKREFCLKSource); /* 配置USB高速PLL参考时钟 */
void RCC_USBHSPHYPLLALIVEcmd(FunctionalState NewState); /* 使能或禁用USB高速PHY PLL保持活动状态 */

#endif

#ifdef __cplusplus
}
#endif

#endif