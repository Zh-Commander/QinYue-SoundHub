/********************************** (C) COPYRIGHT  *******************************
* 文件名称          : ch32v30x_adc.h
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 此文件包含了ADC固件库的所有函数原型。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（无论是否被修改）及其二进制文件用于南京沁恒微电子生产的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_ADC_H
#define __CH32V30x_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"


/* ADC 初始化结构体定义 */
typedef struct
{
  uint32_t ADC_Mode;                      /* 配置ADC工作在独立模式或双模式。
                                             此参数可以是 @ref ADC_mode 的一个值 */

  FunctionalState ADC_ScanConvMode;       /* 指定转换是在扫描（多通道）模式还是单（单通道）模式下进行。
                                             此参数可以设置为 ENABLE 或 DISABLE */

  FunctionalState ADC_ContinuousConvMode; /* 指定转换是在连续模式还是单次模式下进行。
                                             此参数可以设置为 ENABLE 或 DISABLE。 */

  uint32_t ADC_ExternalTrigConv;          /* 定义用于启动规则通道模数转换的外部触发源。
                                             此参数可以是 @ref ADC_external_trigger_sources_for_regular_channels_conversion 的一个值 */

  uint32_t ADC_DataAlign;                 /* 指定ADC数据对齐方式是左对齐还是右对齐。
                                             此参数可以是 @ref ADC_data_align 的一个值 */

  uint8_t ADC_NbrOfChannel;               /* 指定将使用序列器转换的规则通道组的ADC通道数量。
                                             此参数必须在1到16之间。 */

  uint32_t  ADC_OutputBuffer;             /* 指定ADC通道输出缓冲器是启用还是禁用。
                                             此参数可以是 @ref ADC_OutputBuffer 的一个值 */

  uint32_t ADC_Pga;                       /* 指定PGA增益倍数。
                                             此参数可以是 @ref ADC_Pga 的一个值 */
}ADC_InitTypeDef;

/* ADC模式定义 */
#define ADC_Mode_Independent                        ((uint32_t)0x00000000) /* 独立模式 */
#define ADC_Mode_RegInjecSimult                     ((uint32_t)0x00010000) /* 规则组和注入组同时转换 */
#define ADC_Mode_RegSimult_AlterTrig                ((uint32_t)0x00020000) /* 规则组同时转换，交替触发 */
#define ADC_Mode_InjecSimult_FastInterl             ((uint32_t)0x00030000) /* 注入组同时转换，快速交替 */
#define ADC_Mode_InjecSimult_SlowInterl             ((uint32_t)0x00040000) /* 注入组同时转换，慢速交替 */
#define ADC_Mode_InjecSimult                        ((uint32_t)0x00050000) /* 注入组同时转换 */
#define ADC_Mode_RegSimult                          ((uint32_t)0x00060000) /* 规则组同时转换 */
#define ADC_Mode_FastInterl                         ((uint32_t)0x00070000) /* 快速交替模式 */
#define ADC_Mode_SlowInterl                         ((uint32_t)0x00080000) /* 慢速交替模式 */
#define ADC_Mode_AlterTrig                          ((uint32_t)0x00090000) /* 交替触发模式 */

/* ADC规则通道转换外部触发源定义 */
#define ADC_ExternalTrigConv_T1_CC1                 ((uint32_t)0x00000000) /* 定时器1捕获比较1事件触发 */
#define ADC_ExternalTrigConv_T1_CC2                 ((uint32_t)0x00020000) /* 定时器1捕获比较2事件触发 */
#define ADC_ExternalTrigConv_T2_CC2                 ((uint32_t)0x00060000) /* 定时器2捕获比较2事件触发 */
#define ADC_ExternalTrigConv_T3_TRGO                ((uint32_t)0x00080000) /* 定时器3触发输出事件触发 */
#define ADC_ExternalTrigConv_T4_CC4                 ((uint32_t)0x000A0000) /* 定时器4捕获比较4事件触发 */
#define ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO     ((uint32_t)0x000C0000) /* 外部中断线11或定时器8触发输出事件触发 */

#define ADC_ExternalTrigConv_T1_CC3                 ((uint32_t)0x00040000) /* 定时器1捕获比较3事件触发 */
#define ADC_ExternalTrigConv_None                   ((uint32_t)0x000E0000) /* 不使用外部触发，软件触发 */

#define ADC_ExternalTrigConv_T3_CC1                ((uint32_t)0x00000000) /* 定时器3捕获比较1事件触发 */
#define ADC_ExternalTrigConv_T2_CC3                ((uint32_t)0x00020000) /* 定时器2捕获比较3事件触发 */
#define ADC_ExternalTrigConv_T8_CC1                ((uint32_t)0x00060000) /* 定时器8捕获比较1事件触发 */
#define ADC_ExternalTrigConv_T8_TRGO               ((uint32_t)0x00080000) /* 定时器8触发输出事件触发 */
#define ADC_ExternalTrigConv_T5_CC1                ((uint32_t)0x000A0000) /* 定时器5捕获比较1事件触发 */
#define ADC_ExternalTrigConv_T5_CC3                ((uint32_t)0x000C0000) /* 定时器5捕获比较3事件触发 */


/* ADC数据对齐方式定义 */
#define ADC_DataAlign_Right                         ((uint32_t)0x00000000) /* 数据右对齐 */
#define ADC_DataAlign_Left                          ((uint32_t)0x00000800) /* 数据左对齐 */

/* ADC通道定义 */
#define ADC_Channel_0                               ((uint8_t)0x00) /* 通道0 */
#define ADC_Channel_1                               ((uint8_t)0x01) /* 通道1 */
#define ADC_Channel_2                               ((uint8_t)0x02) /* 通道2 */
#define ADC_Channel_3                               ((uint8_t)0x03) /* 通道3 */
#define ADC_Channel_4                               ((uint8_t)0x04) /* 通道4 */
#define ADC_Channel_5                               ((uint8_t)0x05) /* 通道5 */
#define ADC_Channel_6                               ((uint8_t)0x06) /* 通道6 */
#define ADC_Channel_7                               ((uint8_t)0x07) /* 通道7 */
#define ADC_Channel_8                               ((uint8_t)0x08) /* 通道8 */
#define ADC_Channel_9                               ((uint8_t)0x09) /* 通道9 */
#define ADC_Channel_10                              ((uint8_t)0x0A) /* 通道10 */
#define ADC_Channel_11                              ((uint8_t)0x0B) /* 通道11 */
#define ADC_Channel_12                              ((uint8_t)0x0C) /* 通道12 */
#define ADC_Channel_13                              ((uint8_t)0x0D) /* 通道13 */
#define ADC_Channel_14                              ((uint8_t)0x0E) /* 通道14 */
#define ADC_Channel_15                              ((uint8_t)0x0F) /* 通道15 */
#define ADC_Channel_16                              ((uint8_t)0x10) /* 通道16 */
#define ADC_Channel_17                              ((uint8_t)0x11) /* 通道17 */

#define ADC_Channel_TempSensor                      ((uint8_t)ADC_Channel_16) /* 温度传感器通道 */
#define ADC_Channel_Vrefint                         ((uint8_t)ADC_Channel_17) /* 内部参考电压通道 */

/* ADC输出缓冲器定义 */
#define ADC_OutputBuffer_Enable                     ((uint32_t)0x04000000) /* 使能输出缓冲器 */
#define ADC_OutputBuffer_Disable                    ((uint32_t)0x00000000) /* 禁用输出缓冲器 */

/* ADC可编程增益放大器(PGA)增益倍数定义 */
#define ADC_Pga_1                                   ((uint32_t)0x00000000) /* PGA增益1倍 */
#define ADC_Pga_4                                   ((uint32_t)0x08000000) /* PGA增益4倍 */
#define ADC_Pga_16                                  ((uint32_t)0x10000000) /* PGA增益16倍 */
#define ADC_Pga_64                                  ((uint32_t)0x18000000) /* PGA增益64倍 */

/* ADC采样时间定义 */
#define ADC_SampleTime_1Cycles5                     ((uint8_t)0x00) /* 采样时间1.5个ADC时钟周期 */
#define ADC_SampleTime_7Cycles5                     ((uint8_t)0x01) /* 采样时间7.5个ADC时钟周期 */
#define ADC_SampleTime_13Cycles5                    ((uint8_t)0x02) /* 采样时间13.5个ADC时钟周期 */
#define ADC_SampleTime_28Cycles5                    ((uint8_t)0x03) /* 采样时间28.5个ADC时钟周期 */
#define ADC_SampleTime_41Cycles5                    ((uint8_t)0x04) /* 采样时间41.5个ADC时钟周期 */
#define ADC_SampleTime_55Cycles5                    ((uint8_t)0x05) /* 采样时间55.5个ADC时钟周期 */
#define ADC_SampleTime_71Cycles5                    ((uint8_t)0x06) /* 采样时间71.5个ADC时钟周期 */
#define ADC_SampleTime_239Cycles5                   ((uint8_t)0x07) /* 采样时间239.5个ADC时钟周期 */

/* ADC注入通道转换外部触发源定义 */
#define ADC_ExternalTrigInjecConv_T2_TRGO           ((uint32_t)0x00002000) /* 定时器2触发输出事件触发 */
#define ADC_ExternalTrigInjecConv_T2_CC1            ((uint32_t)0x00003000) /* 定时器2捕获比较1事件触发 */
#define ADC_ExternalTrigInjecConv_T3_CC4            ((uint32_t)0x00004000) /* 定时器3捕获比较4事件触发 */
#define ADC_ExternalTrigInjecConv_T4_TRGO           ((uint32_t)0x00005000) /* 定时器4触发输出事件触发 */
#define ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4 ((uint32_t)0x00006000) /* 外部中断线15或定时器8捕获比较4事件触发 */

#define ADC_ExternalTrigInjecConv_T1_TRGO           ((uint32_t)0x00000000) /* 定时器1触发输出事件触发 */
#define ADC_ExternalTrigInjecConv_T1_CC4            ((uint32_t)0x00001000) /* 定时器1捕获比较4事件触发 */
#define ADC_ExternalTrigInjecConv_None              ((uint32_t)0x00007000) /* 不使用外部触发，软件触发 */

#define ADC_ExternalTrigInjecConv_T4_CC3            ((uint32_t)0x00002000) /* 定时器4捕获比较3事件触发 */
#define ADC_ExternalTrigInjecConv_T8_CC2            ((uint32_t)0x00003000) /* 定时器8捕获比较2事件触发 */
#define ADC_ExternalTrigInjecConv_T8_CC4            ((uint32_t)0x00004000) /* 定时器8捕获比较4事件触发 */
#define ADC_ExternalTrigInjecConv_T5_TRGO           ((uint32_t)0x00005000) /* 定时器5触发输出事件触发 */
#define ADC_ExternalTrigInjecConv_T5_CC4            ((uint32_t)0x00006000) /* 定时器5捕获比较4事件触发 */


/* ADC注入通道选择定义 */
#define ADC_InjectedChannel_1                       ((uint8_t)0x14) /* 注入序列通道1 */
#define ADC_InjectedChannel_2                       ((uint8_t)0x18) /* 注入序列通道2 */
#define ADC_InjectedChannel_3                       ((uint8_t)0x1C) /* 注入序列通道3 */
#define ADC_InjectedChannel_4                       ((uint8_t)0x20) /* 注入序列通道4 */

/* ADC模拟看门狗选择定义 */
#define ADC_AnalogWatchdog_SingleRegEnable          ((uint32_t)0x00800200) /* 使能单个规则通道的模拟看门狗 */
#define ADC_AnalogWatchdog_SingleInjecEnable        ((uint32_t)0x00400200) /* 使能单个注入通道的模拟看门狗 */
#define ADC_AnalogWatchdog_SingleRegOrInjecEnable   ((uint32_t)0x00C00200) /* 使能单个规则或注入通道的模拟看门狗 */
#define ADC_AnalogWatchdog_AllRegEnable             ((uint32_t)0x00800000) /* 使能所有规则通道的模拟看门狗 */
#define ADC_AnalogWatchdog_AllInjecEnable           ((uint32_t)0x00400000) /* 使能所有注入通道的模拟看门狗 */
#define ADC_AnalogWatchdog_AllRegAllInjecEnable     ((uint32_t)0x00C00000) /* 使能所有规则和注入通道的模拟看门狗 */
#define ADC_AnalogWatchdog_None                     ((uint32_t)0x00000000) /* 禁用模拟看门狗 */

/* ADC中断定义 */
#define ADC_IT_EOC                                  ((uint16_t)0x0220) /* 转换结束中断 */
#define ADC_IT_AWD                                  ((uint16_t)0x0140) /* 模拟看门狗中断 */
#define ADC_IT_JEOC                                 ((uint16_t)0x0480) /* 注入组转换结束中断 */

/* ADC标志位定义 */
#define ADC_FLAG_AWD                                ((uint8_t)0x01) /* 模拟看门狗标志 */
#define ADC_FLAG_EOC                                ((uint8_t)0x02) /* 转换结束标志 */
#define ADC_FLAG_JEOC                               ((uint8_t)0x04) /* 注入组转换结束标志 */
#define ADC_FLAG_JSTRT                              ((uint8_t)0x08) /* 注入组转换开始标志 */
#define ADC_FLAG_STRT                               ((uint8_t)0x10) /* 规则组转换开始标志 */


void ADC_DeInit(ADC_TypeDef* ADCx); /* 将ADC外设寄存器重置为默认值 */
void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct); /* 根据ADC_InitStruct中的指定参数初始化ADC外设 */
void ADC_StructInit(ADC_InitTypeDef* ADC_InitStruct); /* 使用默认值填充每个ADC_InitStruct成员 */
void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定的ADC外设 */
void ADC_DMACmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC的DMA请求 */
void ADC_ITConfig(ADC_TypeDef* ADCx, uint16_t ADC_IT, FunctionalState NewState); /* 使能或禁用指定的ADC中断 */
void ADC_ResetCalibration(ADC_TypeDef* ADCx); /* 重置指定ADC的校准寄存器 */
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef* ADCx); /* 获取ADC重置校准寄存器的状态 */
void ADC_StartCalibration(ADC_TypeDef* ADCx); /* 开始指定ADC的校准过程 */
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef* ADCx); /* 获取ADC校准过程的状态 */
void ADC_SoftwareStartConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC的软件启动转换功能 */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef* ADCx); /* 获取ADC软件启动转换的状态 */
void ADC_DiscModeChannelCountConfig(ADC_TypeDef* ADCx, uint8_t Number); /* 配置指定ADC的不连续模式通道计数 */
void ADC_DiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC的不连续模式 */
void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime); /* 配置指定ADC的规则组通道，设置其转换顺序和采样时间 */
void ADC_ExternalTrigConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC规则组转换的外部触发 */
uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx); /* 返回指定ADC最近一次规则组转换的结果 */
uint32_t ADC_GetDualModeConversionValue(void); /* 返回双ADC模式下最近一次转换的结果 */
void ADC_AutoInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC注入组的自动转换 */
void ADC_InjectedDiscModeCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC注入组的不连续模式 */
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef* ADCx, uint32_t ADC_ExternalTrigInjecConv); /* 配置ADC注入组转换的外部触发源 */
void ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC注入组转换的外部触发 */
void ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用指定ADC注入组的软件启动转换功能 */
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef* ADCx); /* 获取ADC注入组软件启动转换的状态 */
void ADC_InjectedChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime); /* 配置指定ADC的注入组通道，设置其转换顺序和采样时间 */
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef* ADCx, uint8_t Length); /* 配置指定ADC注入组通道序列的长度 */
void ADC_SetInjectedOffset(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset); /* 设置指定ADC注入通道的转换值偏移量 */
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef* ADCx, uint8_t ADC_InjectedChannel); /* 返回指定ADC注入通道的转换结果 */
void ADC_AnalogWatchdogCmd(ADC_TypeDef* ADCx, uint32_t ADC_AnalogWatchdog); /* 使能或禁用指定ADC的模拟看门狗 */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef* ADCx, uint16_t HighThreshold, uint16_t LowThreshold); /* 配置模拟看门狗的高和低阈值 */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel); /* 配置模拟看门狗监视的单个通道 */
void ADC_TempSensorVrefintCmd(FunctionalState NewState); /* 使能或禁用温度传感器和内部参考电压通道 */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG); /* 检查指定的ADC标志位是否设置 */
void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG); /* 清除ADC的标志位 */
ITStatus ADC_GetITStatus(ADC_TypeDef* ADCx, uint16_t ADC_IT); /* 检查指定的ADC中断是否发生 */
void ADC_ClearITPendingBit(ADC_TypeDef* ADCx, uint16_t ADC_IT); /* 清除ADC的中断挂起位 */
s32 TempSensor_Volt_To_Temper(s32 Value); /* 将温度传感器电压值转换为温度值 */
void ADC_BufferCmd(ADC_TypeDef* ADCx, FunctionalState NewState); /* 使能或禁用ADC的输出缓冲器 */
int16_t Get_CalibrationValue(ADC_TypeDef* ADCx); /* 获取ADC的校准值 */

#ifdef __cplusplus
}
#endif

#endif 
