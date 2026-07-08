/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dac.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : 此文件包含了DAC固件库的所有函数原型
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: 本软件（修改或未修改）及二进制文件用于南京沁恒微电子生产的微控制器
*******************************************************************************/
#ifndef __CH32V30x_DAC_H
#define __CH32V30x_DAC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* DAC 初始化结构体定义 */
typedef struct
{
  uint32_t DAC_Trigger;                      /* 指定所选DAC通道的外部触发源
                                                此参数可以是 @ref DAC_trigger_selection 的值 */

  uint32_t DAC_WaveGeneration;               /* 指定是否生成DAC通道的噪声波或三角波，
                                                或者不生成任何波形。
                                                此参数可以是 @ref DAC_wave_generation 的值 */

  uint32_t DAC_LFSRUnmask_TriangleAmplitude; /* 指定用于噪声波生成的LFSR掩码，或者
                                                DAC通道三角波的最大幅度。
                                                此参数可以是 @ref DAC_lfsrunmask_triangleamplitude 的值 */

  uint32_t DAC_OutputBuffer;                 /* 指定DAC通道输出缓冲区是启用还是禁用。
                                                此参数可以是 @ref DAC_output_buffer 的值 */
}DAC_InitTypeDef;


/* DAC 触发源选择 */
#define DAC_Trigger_None                   ((uint32_t)0x00000000) /* 转换在DAC1_DHRxxxx寄存器加载后自动进行，不由外部触发 */
#define DAC_Trigger_T6_TRGO                ((uint32_t)0x00000004) /* TIM6 TRGO 被选为DAC通道的外部转换触发 */
#define DAC_Trigger_T8_TRGO                ((uint32_t)0x0000000C) /* TIM8 TRGO 被选为DAC通道的外部转换触发（仅在高密度器件中）*/
#define DAC_Trigger_T7_TRGO                ((uint32_t)0x00000014) /* TIM7 TRGO 被选为DAC通道的外部转换触发 */
#define DAC_Trigger_T5_TRGO                ((uint32_t)0x0000001C) /* TIM5 TRGO 被选为DAC通道的外部转换触发 */
#define DAC_Trigger_T2_TRGO                ((uint32_t)0x00000024) /* TIM2 TRGO 被选为DAC通道的外部转换触发 */
#define DAC_Trigger_T4_TRGO                ((uint32_t)0x0000002C) /* TIM4 TRGO 被选为DAC通道的外部转换触发 */
#define DAC_Trigger_Ext_IT9                ((uint32_t)0x00000034) /* EXTI 线路9事件被选为DAC通道的外部转换触发 */
#define DAC_Trigger_Software               ((uint32_t)0x0000003C) /* DAC通道的转换由软件触发启动 */

/* DAC 波形生成模式 */
#define DAC_WaveGeneration_None            ((uint32_t)0x00000000) /* 不生成波形 */
#define DAC_WaveGeneration_Noise           ((uint32_t)0x00000040) /* 生成噪声波 */
#define DAC_WaveGeneration_Triangle        ((uint32_t)0x00000080) /* 生成三角波 */


/* DAC LFSR掩码/三角波幅度 */
#define DAC_LFSRUnmask_Bit0                ((uint32_t)0x00000000) /* 解除DAC通道LFSR的bit0用于噪声波生成 */
#define DAC_LFSRUnmask_Bits1_0             ((uint32_t)0x00000100) /* 解除DAC通道LFSR的bit[1:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits2_0             ((uint32_t)0x00000200) /* 解除DAC通道LFSR的bit[2:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits3_0             ((uint32_t)0x00000300) /* 解除DAC通道LFSR的bit[3:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits4_0             ((uint32_t)0x00000400) /* 解除DAC通道LFSR的bit[4:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits5_0             ((uint32_t)0x00000500) /* 解除DAC通道LFSR的bit[5:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits6_0             ((uint32_t)0x00000600) /* 解除DAC通道LFSR的bit[6:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits7_0             ((uint32_t)0x00000700) /* 解除DAC通道LFSR的bit[7:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits8_0             ((uint32_t)0x00000800) /* 解除DAC通道LFSR的bit[8:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits9_0             ((uint32_t)0x00000900) /* 解除DAC通道LFSR的bit[9:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits10_0            ((uint32_t)0x00000A00) /* 解除DAC通道LFSR的bit[10:0]用于噪声波生成 */
#define DAC_LFSRUnmask_Bits11_0            ((uint32_t)0x00000B00) /* 解除DAC通道LFSR的bit[11:0]用于噪声波生成 */
#define DAC_TriangleAmplitude_1            ((uint32_t)0x00000000) /* 选择最大三角波幅度为1 */
#define DAC_TriangleAmplitude_3            ((uint32_t)0x00000100) /* 选择最大三角波幅度为3 */
#define DAC_TriangleAmplitude_7            ((uint32_t)0x00000200) /* 选择最大三角波幅度为7 */
#define DAC_TriangleAmplitude_15           ((uint32_t)0x00000300) /* 选择最大三角波幅度为15 */
#define DAC_TriangleAmplitude_31           ((uint32_t)0x00000400) /* 选择最大三角波幅度为31 */
#define DAC_TriangleAmplitude_63           ((uint32_t)0x00000500) /* 选择最大三角波幅度为63 */
#define DAC_TriangleAmplitude_127          ((uint32_t)0x00000600) /* 选择最大三角波幅度为127 */
#define DAC_TriangleAmplitude_255          ((uint32_t)0x00000700) /* 选择最大三角波幅度为255 */
#define DAC_TriangleAmplitude_511          ((uint32_t)0x00000800) /* 选择最大三角波幅度为511 */
#define DAC_TriangleAmplitude_1023         ((uint32_t)0x00000900) /* 选择最大三角波幅度为1023 */
#define DAC_TriangleAmplitude_2047         ((uint32_t)0x00000A00) /* 选择最大三角波幅度为2047 */
#define DAC_TriangleAmplitude_4095         ((uint32_t)0x00000B00) /* 选择最大三角波幅度为4095 */

/* DAC 输出缓冲区控制 */
#define DAC_OutputBuffer_Enable            ((uint32_t)0x00000000) /* 启用DAC输出缓冲区 */
#define DAC_OutputBuffer_Disable           ((uint32_t)0x00000002) /* 禁用DAC输出缓冲区 */

/* DAC 通道选择 */
#define DAC_Channel_1                      ((uint32_t)0x00000000) /* DAC通道1 */
#define DAC_Channel_2                      ((uint32_t)0x00000010) /* DAC通道2 */
																 
/* DAC 数据对齐方式 */
#define DAC_Align_12b_R                    ((uint32_t)0x00000000) /* 12位右对齐 */
#define DAC_Align_12b_L                    ((uint32_t)0x00000004) /* 12位左对齐 */
#define DAC_Align_8b_R                     ((uint32_t)0x00000008) /* 8位右对齐 */

/* DAC 波形类型（用于波形生成函数） */
#define DAC_Wave_Noise                     ((uint32_t)0x00000040) /* 噪声波 */
#define DAC_Wave_Triangle                  ((uint32_t)0x00000080) /* 三角波 */

/* DAC库函数 */
void DAC_DeInit(void);                                                /* 将DAC外设寄存器重置为其默认值 */
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct); /* 根据DAC_InitStruct中的指定参数初始化DAC通道 */
void DAC_StructInit(DAC_InitTypeDef* DAC_InitStruct);                 /* 用默认值填充DAC_InitStruct的每个成员 */
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);         /* 启用或禁用指定的DAC通道 */
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);      /* 启用或禁用指定DAC通道的DMA请求 */
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState); /* 启用或禁用指定DAC通道的软件触发 */
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState);            /* 启用或禁用两个DAC通道的软件触发（双模式） */
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState); /* 启用或禁用指定DAC通道的波形生成 */
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);          /* 为DAC通道1设置12位右对齐、左对齐或8位右对齐的数据 */
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);          /* 为DAC通道2设置12位右对齐、左对齐或8位右对齐的数据 */
void DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1); /* 同时为DAC通道2和通道1设置数据（双模式） */
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);                /* 返回指定DAC通道的数据输出值 */

#ifdef __cplusplus
}
#endif

#endif 