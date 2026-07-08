/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_tim.h
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 本文件包含TIM固件库的所有函数原型。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或不修改）和二进制代码用于南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_TIM_H
#define __CH32V30x_TIM_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* 定时器时基初始化结构体定义 */
typedef struct
{
  uint16_t TIM_Prescaler;         /* 指定用于分频定时器时钟的预分频器值。
                                     此参数可以是0x0000到0xFFFF之间的数值。 */

  uint16_t TIM_CounterMode;       /* 指定计数器模式。
                                     此参数可以是 @ref TIM_Counter_Mode 中的一个值。 */

  uint16_t TIM_Period;            /* 指定在下一次更新事件时加载到自动重装载寄存器中的周期值。
                                     此参数必须是0x0000到0xFFFF之间的数值。 */ 

  uint16_t TIM_ClockDivision;     /* 指定时钟分频。
                                     此参数可以是 @ref TIM_Clock_Division_CKD 中的一个值。 */

  uint8_t TIM_RepetitionCounter;  /* 指定重复计数器值。每次RCR递减计数器达到零时，产生一个更新事件，
                                     并从RCR值（N）重新开始计数。
                                     这意味着在PWM模式中，(N+1)对应于：
                                        - 边沿对齐模式下的PWM周期数
                                        - 中心对齐模式下的半个PWM周期数
                                     此参数必须是0x00到0xFF之间的数值。
                                     @note 此参数仅对TIM1和TIM8有效。 */
} TIM_TimeBaseInitTypeDef;       

/* 定时器输出比较初始化结构体定义 */
typedef struct
{
  uint16_t TIM_OCMode;        /* 指定TIM模式。
                                 此参数可以是 @ref TIM_Output_Compare_and_PWM_modes 中的一个值。 */

  uint16_t TIM_OutputState;   /* 指定TIM输出比较状态。
                                 此参数可以是 @ref TIM_Output_Compare_state 中的一个值。 */

  uint16_t TIM_OutputNState;  /* 指定TIM互补输出比较状态。
                                 此参数可以是 @ref TIM_Output_Compare_N_state 中的一个值。
                                 @note 此参数仅对TIM1和TIM8有效。 */

  uint16_t TIM_Pulse;         /* 指定要加载到捕获比较寄存器中的脉冲值。
                                 此参数可以是0x0000到0xFFFF之间的数值。 */

  uint16_t TIM_OCPolarity;    /* 指定输出极性。
                                 此参数可以是 @ref TIM_Output_Compare_Polarity 中的一个值。 */

  uint16_t TIM_OCNPolarity;   /* 指定互补输出极性。
                                 此参数可以是 @ref TIM_Output_Compare_N_Polarity 中的一个值。
                                 @note 此参数仅对TIM1和TIM8有效。 */

  uint16_t TIM_OCIdleState;   /* 指定空闲状态下TIM输出比较引脚的状态。
                                 此参数可以是 @ref TIM_Output_Compare_Idle_State 中的一个值。
                                 @note 此参数仅对TIM1和TIM8有效。 */

  uint16_t TIM_OCNIdleState;  /* 指定空闲状态下TIM输出比较引脚的状态。
                                 此参数可以是 @ref TIM_Output_Compare_N_Idle_State 中的一个值。
                                 @note 此参数仅对TIM1和TIM8有效。 */
} TIM_OCInitTypeDef;

/* 定时器输入捕获初始化结构体定义 */
typedef struct
{
  uint16_t TIM_Channel;      /* 指定TIM通道。
                                此参数可以是 @ref TIM_Channel 中的一个值。 */

  uint16_t TIM_ICPolarity;   /* 指定输入信号的有效边沿。
                                此参数可以是 @ref TIM_Input_Capture_Polarity 中的一个值。 */

  uint16_t TIM_ICSelection;  /* 指定输入。
                                此参数可以是 @ref TIM_Input_Capture_Selection 中的一个值。 */

  uint16_t TIM_ICPrescaler;  /* 指定输入捕获预分频器。
                                此参数可以是 @ref TIM_Input_Capture_Prescaler 中的一个值。 */

  uint16_t TIM_ICFilter;     /* 指定输入捕获滤波器。
                                此参数可以是0x0到0xF之间的数值。 */
} TIM_ICInitTypeDef;

/* BDTR结构体定义（用于断路和死区） */
typedef struct
{
  uint16_t TIM_OSSRState;        /* 指定运行模式下的关闭状态选择。
                                    此参数可以是 @ref OSSR_Off_State_Selection_for_Run_mode_state 中的一个值。 */

  uint16_t TIM_OSSIState;        /* 指定空闲模式下的关闭状态选择。
                                    此参数可以是 @ref OSSI_Off_State_Selection_for_Idle_mode_state 中的一个值。 */

  uint16_t TIM_LOCKLevel;        /* 指定LOCK级别参数。
                                    此参数可以是 @ref Lock_level 中的一个值。 */ 

  uint16_t TIM_DeadTime;         /* 指定输出关闭和打开之间的延迟时间。
                                    此参数可以是0x00到0xFF之间的数值。 */

  uint16_t TIM_Break;            /* 指定TIM断路输入是否使能。
                                    此参数可以是 @ref Break_Input_enable_disable 中的一个值。 */

  uint16_t TIM_BreakPolarity;    /* 指定TIM断路输入引脚极性。
                                    此参数可以是 @ref Break_Polarity 中的一个值。 */

  uint16_t TIM_AutomaticOutput;  /* 指定TIM自动输出功能是否使能。
                                    此参数可以是 @ref TIM_AOE_Bit_Set_Reset 中的一个值。 */
} TIM_BDTRInitTypeDef;

/* @defgroup TIM_Output_Compare_and_PWM_modes - 输出比较和PWM模式 */
#define TIM_OCMode_Timing                  ((uint16_t)0x0000) /* 定时模式 */
#define TIM_OCMode_Active                  ((uint16_t)0x0010) /* 主动模式 */
#define TIM_OCMode_Inactive                ((uint16_t)0x0020) /* 非主动模式 */
#define TIM_OCMode_Toggle                   ((uint16_t)0x0030) /* 翻转模式 */
#define TIM_OCMode_PWM1                    ((uint16_t)0x0060) /* PWM模式1 */
#define TIM_OCMode_PWM2                    ((uint16_t)0x0070) /* PWM模式2 */

/* @defgroup TIM_One_Pulse_Mode - 单脉冲模式 */
#define TIM_OPMode_Single                  ((uint16_t)0x0008) /* 单脉冲模式 */
#define TIM_OPMode_Repetitive              ((uint16_t)0x0000) /* 重复脉冲模式 */

/* @defgroup TIM_Channel - 定时器通道 */
#define TIM_Channel_1                      ((uint16_t)0x0000) /* 通道1 */
#define TIM_Channel_2                      ((uint16_t)0x0004) /* 通道2 */
#define TIM_Channel_3                      ((uint16_t)0x0008) /* 通道3 */
#define TIM_Channel_4                      ((uint16_t)0x000C) /* 通道4 */

/* @defgroup TIM_Clock_Division_CKD - 时钟分频 */
#define TIM_CKD_DIV1                       ((uint16_t)0x0000) /* tDTS = 定时器时钟周期 */
#define TIM_CKD_DIV2                       ((uint16_t)0x0100) /* tDTS = 2 * 定时器时钟周期 */
#define TIM_CKD_DIV4                       ((uint16_t)0x0200) /* tDTS = 4 * 定时器时钟周期 */

/* @defgroup TIM_Counter_Mode - 计数器模式 */
#define TIM_CounterMode_Up                 ((uint16_t)0x0000) /* 向上计数 */
#define TIM_CounterMode_Down               ((uint16_t)0x0010) /* 向下计数 */
#define TIM_CounterMode_CenterAligned1     ((uint16_t)0x0020) /* 中心对齐模式1 */
#define TIM_CounterMode_CenterAligned2     ((uint16_t)0x0040) /* 中心对齐模式2 */
#define TIM_CounterMode_CenterAligned3     ((uint16_t)0x0060) /* 中心对齐模式3 */

/* @defgroup TIM_Output_Compare_Polarity - 输出比较极性 */
#define TIM_OCPolarity_High                ((uint16_t)0x0000) /* 高电平有效 */
#define TIM_OCPolarity_Low                 ((uint16_t)0x0002) /* 低电平有效 */

/* @defgroup TIM_Output_Compare_N_Polarity - 互补输出比较极性 */  
#define TIM_OCNPolarity_High               ((uint16_t)0x0000) /* 互补输出高电平有效 */
#define TIM_OCNPolarity_Low                ((uint16_t)0x0008) /* 互补输出低电平有效 */

/* @defgroup TIM_Output_Compare_state - 输出比较状态 */
#define TIM_OutputState_Disable            ((uint16_t)0x0000) /* 禁止输出 */
#define TIM_OutputState_Enable             ((uint16_t)0x0001) /* 使能输出 */

/* @defgroup TIM_Output_Compare_N_state - 互补输出比较状态 */
#define TIM_OutputNState_Disable           ((uint16_t)0x0000) /* 禁止互补输出 */
#define TIM_OutputNState_Enable            ((uint16_t)0x0004) /* 使能互补输出 */

/* @defgroup TIM_Capture_Compare_state - 捕获比较状态 */
#define TIM_CCx_Enable                     ((uint16_t)0x0001) /* 使能捕获比较通道 */
#define TIM_CCx_Disable                    ((uint16_t)0x0000) /* 禁止捕获比较通道 */

/* @defgroup TIM_Capture_Compare_N_state - 互补捕获比较状态 */
#define TIM_CCxN_Enable                    ((uint16_t)0x0004) /* 使能互补捕获比较通道 */
#define TIM_CCxN_Disable                   ((uint16_t)0x0000) /* 禁止互补捕获比较通道 */

/* @defgroup Break_Input_enable_disable - 断路输入使能/禁止 */
#define TIM_Break_Enable                   ((uint16_t)0x1000) /* 使能断路输入 */
#define TIM_Break_Disable                  ((uint16_t)0x0000) /* 禁止断路输入 */

/* @defgroup Break_Polarity - 断路极性 */
#define TIM_BreakPolarity_Low              ((uint16_t)0x0000) /* 低电平有效 */
#define TIM_BreakPolarity_High             ((uint16_t)0x2000) /* 高电平有效 */

/* @defgroup TIM_AOE_Bit_Set_Reset - 自动输出使能位设置/复位 */
#define TIM_AutomaticOutput_Enable         ((uint16_t)0x4000) /* 使能自动输出 */
#define TIM_AutomaticOutput_Disable        ((uint16_t)0x0000) /* 禁止自动输出 */

/* @defgroup Lock_level - 锁定级别 */
#define TIM_LOCKLevel_OFF                  ((uint16_t)0x0000) /* 无锁定 */
#define TIM_LOCKLevel_1                    ((uint16_t)0x0100) /* 锁定级别1 */
#define TIM_LOCKLevel_2                    ((uint16_t)0x0200) /* 锁定级别2 */
#define TIM_LOCKLevel_3                    ((uint16_t)0x0300) /* 锁定级别3 */

/* @defgroup OSSI_Off_State_Selection_for_Idle_mode_state - 空闲模式关闭状态选择 */
#define TIM_OSSIState_Enable               ((uint16_t)0x0400) /* 使能OSSI */
#define TIM_OSSIState_Disable              ((uint16_t)0x0000) /* 禁止OSSI */

/* @defgroup OSSR_Off_State_Selection_for_Run_mode_state - 运行模式关闭状态选择 */
#define TIM_OSSRState_Enable               ((uint16_t)0x0800) /* 使能OSSR */
#define TIM_OSSRState_Disable              ((uint16_t)0x0000) /* 禁止OSSR */

/* @defgroup TIM_Output_Compare_Idle_State - 输出比较空闲状态 */
#define TIM_OCIdleState_Set                ((uint16_t)0x0100) /* 空闲时置位 */
#define TIM_OCIdleState_Reset              ((uint16_t)0x0000) /* 空闲时复位 */

/* @defgroup TIM_Output_Compare_N_Idle_State - 互补输出比较空闲状态 */
#define TIM_OCNIdleState_Set               ((uint16_t)0x0200) /* 空闲时置位 */
#define TIM_OCNIdleState_Reset             ((uint16_t)0x0000) /* 空闲时复位 */

/* @defgroup TIM_Input_Capture_Polarity - 输入捕获极性 */
#define  TIM_ICPolarity_Rising             ((uint16_t)0x0000) /* 上升沿捕获 */
#define  TIM_ICPolarity_Falling            ((uint16_t)0x0002) /* 下降沿捕获 */
#define  TIM_ICPolarity_BothEdge           ((uint16_t)0x000A) /* 双边沿捕获 */
                                     
/* @defgroup TIM_Input_Capture_Selection - 输入捕获选择 */
#define TIM_ICSelection_DirectTI           ((uint16_t)0x0001) /* TIM输入1、2、3或4分别连接到IC1、IC2、IC3或IC4 */
#define TIM_ICSelection_IndirectTI         ((uint16_t)0x0002) /* TIM输入1、2、3或4分别连接到IC2、IC1、IC4或IC3（间接） */
#define TIM_ICSelection_TRC                ((uint16_t)0x0003) /* TIM输入1、2、3或4连接到TRC（内部触发输入） */

/* @defgroup TIM_Input_Capture_Prescaler - 输入捕获预分频器 */
#define TIM_ICPSC_DIV1                     ((uint16_t)0x0000) /* 每次检测到边沿都捕获 */
#define TIM_ICPSC_DIV2                     ((uint16_t)0x0004) /* 每2个事件捕获一次 */
#define TIM_ICPSC_DIV4                     ((uint16_t)0x0008) /* 每4个事件捕获一次 */
#define TIM_ICPSC_DIV8                     ((uint16_t)0x000C) /* 每8个事件捕获一次 */

/* @defgroup TIM_interrupt_sources - 定时器中断源 */
#define TIM_IT_Update                      ((uint16_t)0x0001) /* 更新中断 */
#define TIM_IT_CC1                         ((uint16_t)0x0002) /* 捕获/比较通道1中断 */
#define TIM_IT_CC2                         ((uint16_t)0x0004) /* 捕获/比较通道2中断 */
#define TIM_IT_CC3                         ((uint16_t)0x0008) /* 捕获/比较通道3中断 */
#define TIM_IT_CC4                         ((uint16_t)0x0010) /* 捕获/比较通道4中断 */
#define TIM_IT_COM                         ((uint16_t)0x0020) /* COM事件中断（仅TIM1、TIM8） */
#define TIM_IT_Trigger                     ((uint16_t)0x0040) /* 触发中断 */
#define TIM_IT_Break                       ((uint16_t)0x0080) /* 断路中断（仅TIM1、TIM8） */

/* @defgroup TIM_DMA_Base_address - TIM DMA基地址（用于DMA突发传输） */
#define TIM_DMABase_CR1                    ((uint16_t)0x0000) /* 控制寄存器1 */
#define TIM_DMABase_CR2                    ((uint16_t)0x0001) /* 控制寄存器2 */
#define TIM_DMABase_SMCR                   ((uint16_t)0x0002) /* 从模式控制寄存器 */
#define TIM_DMABase_DIER                   ((uint16_t)0x0003) /* DMA/中断使能寄存器 */
#define TIM_DMABase_SR                     ((uint16_t)0x0004) /* 状态寄存器 */
#define TIM_DMABase_EGR                    ((uint16_t)0x0005) /* 事件产生寄存器 */
#define TIM_DMABase_CCMR1                  ((uint16_t)0x0006) /* 捕获/比较模式寄存器1 */
#define TIM_DMABase_CCMR2                  ((uint16_t)0x0007) /* 捕获/比较模式寄存器2 */
#define TIM_DMABase_CCER                   ((uint16_t)0x0008) /* 捕获/比较使能寄存器 */
#define TIM_DMABase_CNT                    ((uint16_t)0x0009) /* 计数器 */
#define TIM_DMABase_PSC                    ((uint16_t)0x000A) /* 预分频器 */
#define TIM_DMABase_ARR                    ((uint16_t)0x000B) /* 自动重装载寄存器 */
#define TIM_DMABase_RCR                    ((uint16_t)0x000C) /* 重复计数器寄存器（仅TIM1、TIM8） */
#define TIM_DMABase_CCR1                   ((uint16_t)0x000D) /* 捕获/比较寄存器1 */
#define TIM_DMABase_CCR2                   ((uint16_t)0x000E) /* 捕获/比较寄存器2 */
#define TIM_DMABase_CCR3                   ((uint16_t)0x000F) /* 捕获/比较寄存器3 */
#define TIM_DMABase_CCR4                   ((uint16_t)0x0010) /* 捕获/比较寄存器4 */
#define TIM_DMABase_BDTR                   ((uint16_t)0x0011) /* 断路和死区寄存器（仅TIM1、TIM8） */
#define TIM_DMABase_DCR                    ((uint16_t)0x0012) /* DMA控制寄存器 */

/* @defgroup TIM_DMA_Burst_Length - TIM DMA突发传输长度 */
#define TIM_DMABurstLength_1Transfer       ((uint16_t)0x0000) /* 1次传输 */
#define TIM_DMABurstLength_2Transfers      ((uint16_t)0x0100) /* 2次传输 */
#define TIM_DMABurstLength_3Transfers      ((uint16_t)0x0200) /* 3次传输 */
#define TIM_DMABurstLength_4Transfers      ((uint16_t)0x0300) /* 4次传输 */
#define TIM_DMABurstLength_5Transfers      ((uint16_t)0x0400) /* 5次传输 */
#define TIM_DMABurstLength_6Transfers      ((uint16_t)0x0500) /* 6次传输 */
#define TIM_DMABurstLength_7Transfers      ((uint16_t)0x0600) /* 7次传输 */
#define TIM_DMABurstLength_8Transfers      ((uint16_t)0x0700) /* 8次传输 */
#define TIM_DMABurstLength_9Transfers      ((uint16_t)0x0800) /* 9次传输 */
#define TIM_DMABurstLength_10Transfers     ((uint16_t)0x0900) /* 10次传输 */
#define TIM_DMABurstLength_11Transfers     ((uint16_t)0x0A00) /* 11次传输 */
#define TIM_DMABurstLength_12Transfers     ((uint16_t)0x0B00) /* 12次传输 */
#define TIM_DMABurstLength_13Transfers     ((uint16_t)0x0C00) /* 13次传输 */
#define TIM_DMABurstLength_14Transfers     ((uint16_t)0x0D00) /* 14次传输 */
#define TIM_DMABurstLength_15Transfers     ((uint16_t)0x0E00) /* 15次传输 */
#define TIM_DMABurstLength_16Transfers     ((uint16_t)0x0F00) /* 16次传输 */
#define TIM_DMABurstLength_17Transfers     ((uint16_t)0x1000) /* 17次传输 */
#define TIM_DMABurstLength_18Transfers     ((uint16_t)0x1100) /* 18次传输 */

/* @defgroup TIM_DMA_sources - TIM DMA源 */
#define TIM_DMA_Update                     ((uint16_t)0x0100) /* 更新事件DMA请求 */
#define TIM_DMA_CC1                        ((uint16_t)0x0200) /* 捕获/比较通道1 DMA请求 */
#define TIM_DMA_CC2                        ((uint16_t)0x0400) /* 捕获/比较通道2 DMA请求 */
#define TIM_DMA_CC3                        ((uint16_t)0x0800) /* 捕获/比较通道3 DMA请求 */
#define TIM_DMA_CC4                        ((uint16_t)0x1000) /* 捕获/比较通道4 DMA请求 */
#define TIM_DMA_COM                        ((uint16_t)0x2000) /* COM事件DMA请求 */
#define TIM_DMA_Trigger                    ((uint16_t)0x4000) /* 触发事件DMA请求 */

/* @defgroup TIM_External_Trigger_Prescaler - 外部触发预分频器 */
#define TIM_ExtTRGPSC_OFF                  ((uint16_t)0x0000) /* 无预分频 */
#define TIM_ExtTRGPSC_DIV2                 ((uint16_t)0x1000) /* 2分频 */
#define TIM_ExtTRGPSC_DIV4                 ((uint16_t)0x2000) /* 4分频 */
#define TIM_ExtTRGPSC_DIV8                 ((uint16_t)0x3000) /* 8分频 */

/* @defgroup TIM_Internal_Trigger_Selection - 内部触发选择 */
#define TIM_TS_ITR0                        ((uint16_t)0x0000) /* 内部触发0 */
#define TIM_TS_ITR1                        ((uint16_t)0x0010) /* 内部触发1 */
#define TIM_TS_ITR2                        ((uint16_t)0x0020) /* 内部触发2 */
#define TIM_TS_ITR3                        ((uint16_t)0x0030) /* 内部触发3 */
#define TIM_TS_TI1F_ED                     ((uint16_t)0x0040) /* TI1的边沿检测 */
#define TIM_TS_TI1FP1                      ((uint16_t)0x0050) /* 滤波后的TI1 */
#define TIM_TS_TI2FP2                      ((uint16_t)0x0060) /* 滤波后的TI2 */
#define TIM_TS_ETRF                        ((uint16_t)0x0070) /* 外部触发输入（ETRF） */

/* @defgroup TIM_TIx_External_Clock_Source - TIx外部时钟源 */
#define TIM_TIxExternalCLK1Source_TI1      ((uint16_t)0x0050) /* TI1作为外部时钟 */
#define TIM_TIxExternalCLK1Source_TI2      ((uint16_t)0x0060) /* TI2作为外部时钟 */
#define TIM_TIxExternalCLK1Source_TI1ED    ((uint16_t)0x0040) /* TI1边沿检测作为外部时钟 */

/* @defgroup TIM_External_Trigger_Polarity - 外部触发极性 */
#define TIM_ExtTRGPolarity_Inverted        ((uint16_t)0x8000) /* 反向 */
#define TIM_ExtTRGPolarity_NonInverted     ((uint16_t)0x0000) /* 不反向 */

/* @defgroup TIM_Prescaler_Reload_Mode - 预分频器重装载模式 */
#define TIM_PSCReloadMode_Update           ((uint16_t)0x0000) /* 在更新事件时重装载 */
#define TIM_PSCReloadMode_Immediate        ((uint16_t)0x0001) /* 立即重装载 */

/* @defgroup TIM_Forced_Action - 强制动作 */
#define TIM_ForcedAction_Active            ((uint16_t)0x0050) /* 强制为有效电平 */
#define TIM_ForcedAction_InActive          ((uint16_t)0x0040) /* 强制为无效电平 */

/* @defgroup TIM_Encoder_Mode - 编码器模式 */
#define TIM_EncoderMode_TI1                ((uint16_t)0x0001) /* 根据TI1计数 */
#define TIM_EncoderMode_TI2                ((uint16_t)0x0002) /* 根据TI2计数 */
#define TIM_EncoderMode_TI12               ((uint16_t)0x0003) /* 根据TI1和TI2计数 */

/* @defgroup TIM_Event_Source - 事件源 */
#define TIM_EventSource_Update             ((uint16_t)0x0001) /* 更新事件 */
#define TIM_EventSource_CC1                ((uint16_t)0x0002) /* 捕获/比较通道1事件 */
#define TIM_EventSource_CC2                ((uint16_t)0x0004) /* 捕获/比较通道2事件 */
#define TIM_EventSource_CC3                ((uint16_t)0x0008) /* 捕获/比较通道3事件 */
#define TIM_EventSource_CC4                ((uint16_t)0x0010) /* 捕获/比较通道4事件 */
#define TIM_EventSource_COM                ((uint16_t)0x0020) /* COM事件 */
#define TIM_EventSource_Trigger            ((uint16_t)0x0040) /* 触发事件 */
#define TIM_EventSource_Break              ((uint16_t)0x0080) /* 断路事件 */

/* @defgroup TIM_Update_Source - 更新源 */
#define TIM_UpdateSource_Global            ((uint16_t)0x0000) /* 更新源为计数器溢出/下溢、设置UG位或从模式控制器产生的更新 */
#define TIM_UpdateSource_Regular           ((uint16_t)0x0001) /* 更新源仅为计数器溢出/下溢 */

/* @defgroup TIM_Output_Compare_Preload_State - 输出比较预装载状态 */
#define TIM_OCPreload_Enable               ((uint16_t)0x0008) /* 使能预装载 */
#define TIM_OCPreload_Disable              ((uint16_t)0x0000) /* 禁止预装载 */

/* @defgroup TIM_Output_Compare_Fast_State - 输出比较快速状态 */
#define TIM_OCFast_Enable                  ((uint16_t)0x0004) /* 使能快速输出 */
#define TIM_OCFast_Disable                 ((uint16_t)0x0000) /* 禁止快速输出 */

/* @defgroup TIM_Output_Compare_Clear_State - 输出比较清零状态 */
#define TIM_OCClear_Enable                 ((uint16_t)0x0080) /* 使能OCxREF清空（由ETR信号） */
#define TIM_OCClear_Disable                ((uint16_t)0x0000) /* 禁止OCxREF清空 */

/* @defgroup TIM_Trigger_Output_Source - 触发输出源 */
#define TIM_TRGOSource_Reset               ((uint16_t)0x0000) /* 复位作为触发输出（UG位） */
#define TIM_TRGOSource_Enable              ((uint16_t)0x0010) /* 使能作为触发输出（计数器使能） */
#define TIM_TRGOSource_Update              ((uint16_t)0x0020) /* 更新事件作为触发输出 */
#define TIM_TRGOSource_OC1                 ((uint16_t)0x0030) /* 捕获/比较通道1作为触发输出 */
#define TIM_TRGOSource_OC1Ref              ((uint16_t)0x0040) /* OC1REF作为触发输出 */
#define TIM_TRGOSource_OC2Ref              ((uint16_t)0x0050) /* OC2REF作为触发输出 */
#define TIM_TRGOSource_OC3Ref              ((uint16_t)0x0060) /* OC3REF作为触发输出 */
#define TIM_TRGOSource_OC4Ref              ((uint16_t)0x0070) /* OC4REF作为触发输出 */

/* @defgroup TIM_Slave_Mode - 从模式 */
#define TIM_SlaveMode_Reset                ((uint16_t)0x0004) /* 复位模式 */
#define TIM_SlaveMode_Gated                ((uint16_t)0x0005) /* 门控模式 */
#define TIM_SlaveMode_Trigger              ((uint16_t)0x0006) /* 触发模式 */
#define TIM_SlaveMode_External1            ((uint16_t)0x0007) /* 外部时钟模式1 */

/* @defgroup TIM_Master_Slave_Mode - 主从模式 */
#define TIM_MasterSlaveMode_Enable         ((uint16_t)0x0080) /* 使能主从模式 */
#define TIM_MasterSlaveMode_Disable        ((uint16_t)0x0000) /* 禁止主从模式 */

/* @defgroup TIM_Flags - 定时器标志位 */
#define TIM_FLAG_Update                    ((uint16_t)0x0001) /* 更新标志 */
#define TIM_FLAG_CC1                       ((uint16_t)0x0002) /* 捕获/比较通道1标志 */
#define TIM_FLAG_CC2                       ((uint16_t)0x0004) /* 捕获/比较通道2标志 */
#define TIM_FLAG_CC3                       ((uint16_t)0x0008) /* 捕获/比较通道3标志 */
#define TIM_FLAG_CC4                       ((uint16_t)0x0010) /* 捕获/比较通道4标志 */
#define TIM_FLAG_COM                       ((uint16_t)0x0020) /* COM事件标志 */
#define TIM_FLAG_Trigger                   ((uint16_t)0x0040) /* 触发事件标志 */
#define TIM_FLAG_Break                     ((uint16_t)0x0080) /* 断路事件标志 */
#define TIM_FLAG_CC1OF                     ((uint16_t)0x0200) /* 捕获/比较通道1溢出标志 */
#define TIM_FLAG_CC2OF                     ((uint16_t)0x0400) /* 捕获/比较通道2溢出标志 */
#define TIM_FLAG_CC3OF                     ((uint16_t)0x0800) /* 捕获/比较通道3溢出标志 */
#define TIM_FLAG_CC4OF                     ((uint16_t)0x1000) /* 捕获/比较通道4溢出标志 */

/* 遗留定义（为兼容旧版本） */
#define TIM_DMABurstLength_1Byte           TIM_DMABurstLength_1Transfer   /* 1字节突发传输长度（等同于1次传输） */
#define TIM_DMABurstLength_2Bytes          TIM_DMABurstLength_2Transfers  /* 2字节突发传输长度 */
#define TIM_DMABurstLength_3Bytes          TIM_DMABurstLength_3Transfers  /* 3字节突发传输长度 */
#define TIM_DMABurstLength_4Bytes          TIM_DMABurstLength_4Transfers  /* 4字节突发传输长度 */
#define TIM_DMABurstLength_5Bytes          TIM_DMABurstLength_5Transfers  /* 5字节突发传输长度 */
#define TIM_DMABurstLength_6Bytes          TIM_DMABurstLength_6Transfers  /* 6字节突发传输长度 */
#define TIM_DMABurstLength_7Bytes          TIM_DMABurstLength_7Transfers  /* 7字节突发传输长度 */
#define TIM_DMABurstLength_8Bytes          TIM_DMABurstLength_8Transfers  /* 8字节突发传输长度 */
#define TIM_DMABurstLength_9Bytes          TIM_DMABurstLength_9Transfers  /* 9字节突发传输长度 */
#define TIM_DMABurstLength_10Bytes         TIM_DMABurstLength_10Transfers /* 10字节突发传输长度 */
#define TIM_DMABurstLength_11Bytes         TIM_DMABurstLength_11Transfers /* 11字节突发传输长度 */
#define TIM_DMABurstLength_12Bytes         TIM_DMABurstLength_12Transfers /* 12字节突发传输长度 */
#define TIM_DMABurstLength_13Bytes         TIM_DMABurstLength_13Transfers /* 13字节突发传输长度 */
#define TIM_DMABurstLength_14Bytes         TIM_DMABurstLength_14Transfers /* 14字节突发传输长度 */
#define TIM_DMABurstLength_15Bytes         TIM_DMABurstLength_15Transfers /* 15字节突发传输长度 */
#define TIM_DMABurstLength_16Bytes         TIM_DMABurstLength_16Transfers /* 16字节突发传输长度 */
#define TIM_DMABurstLength_17Bytes         TIM_DMABurstLength_17Transfers /* 17字节突发传输长度 */
#define TIM_DMABurstLength_18Bytes         TIM_DMABurstLength_18Transfers /* 18字节突发传输长度 */

/* 以下是TIM外设驱动函数声明，每个函数后附简要说明 */
void TIM_DeInit(TIM_TypeDef* TIMx);                                          // 反初始化TIMx外设（复位寄存器）
void TIM_TimeBaseInit(TIM_TypeDef* TIMx, TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct); // 根据指定参数初始化TIMx时基单元
void TIM_OC1Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);   // 初始化TIMx通道1输出比较
void TIM_OC2Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);   // 初始化TIMx通道2输出比较
void TIM_OC3Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);   // 初始化TIMx通道3输出比较
void TIM_OC4Init(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);   // 初始化TIMx通道4输出比较
void TIM_ICInit(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct);    // 初始化TIMx输入捕获（单通道）
void TIM_PWMIConfig(TIM_TypeDef* TIMx, TIM_ICInitTypeDef* TIM_ICInitStruct); // 配置TIMx为PWM输入捕获模式（同时配置两个通道）
void TIM_BDTRConfig(TIM_TypeDef* TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct); // 配置断路和死区寄存器（仅TIM1、TIM8）
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef* TIM_TimeBaseInitStruct); // 将TIM_TimeBaseInitStruct结构体成员初始化为默认值
void TIM_OCStructInit(TIM_OCInitTypeDef* TIM_OCInitStruct);                 // 将TIM_OCInitStruct结构体成员初始化为默认值
void TIM_ICStructInit(TIM_ICInitTypeDef* TIM_ICInitStruct);                 // 将TIM_ICInitStruct结构体成员初始化为默认值
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef* TIM_BDTRInitStruct);           // 将TIM_BDTRInitStruct结构体成员初始化为默认值
void TIM_Cmd(TIM_TypeDef* TIMx, FunctionalState NewState);                  // 使能或禁用TIMx外设
void TIM_CtrlPWMOutputs(TIM_TypeDef* TIMx, FunctionalState NewState);       // 使能或禁用TIMx的PWM输出（MOE位，仅TIM1、TIM8）
void TIM_ITConfig(TIM_TypeDef* TIMx, uint16_t TIM_IT, FunctionalState NewState); // 使能或禁用指定TIMx中断
void TIM_GenerateEvent(TIM_TypeDef* TIMx, uint16_t TIM_EventSource);        // 通过软件产生指定事件
void TIM_DMAConfig(TIM_TypeDef* TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength); // 配置TIMx的DMA基地址和突发长度
void TIM_DMACmd(TIM_TypeDef* TIMx, uint16_t TIM_DMASource, FunctionalState NewState); // 使能或禁用指定TIMx的DMA请求源
void TIM_InternalClockConfig(TIM_TypeDef* TIMx);                            // 设置TIMx内部时钟作为计数时钟
void TIM_ITRxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource); // 配置TIMx内部触发作为外部时钟
void TIM_TIxExternalClockConfig(TIM_TypeDef* TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter); // 配置TIMx TIx外部时钟模式
void TIM_ETRClockMode1Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter);                         // 配置TIMx外部时钟模式1（从模式）
void TIM_ETRClockMode2Config(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, 
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter); // 配置TIMx外部时钟模式2
void TIM_ETRConfig(TIM_TypeDef* TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                   uint16_t ExtTRGFilter);                                   // 配置TIMx外部触发输入（ETR）参数
void TIM_PrescalerConfig(TIM_TypeDef* TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode); // 设置TIMx预分频值
void TIM_CounterModeConfig(TIM_TypeDef* TIMx, uint16_t TIM_CounterMode);    // 配置TIMx计数器模式
void TIM_SelectInputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_InputTriggerSource); // 选择TIMx输入触发源
void TIM_EncoderInterfaceConfig(TIM_TypeDef* TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity); // 配置TIMx编码器接口模式
void TIM_ForcedOC1Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);     // 强制TIMx通道1为有效或无效电平
void TIM_ForcedOC2Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);     // 强制TIMx通道2为有效或无效电平
void TIM_ForcedOC3Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);     // 强制TIMx通道3为有效或无效电平
void TIM_ForcedOC4Config(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);     // 强制TIMx通道4为有效或无效电平
void TIM_ARRPreloadConfig(TIM_TypeDef* TIMx, FunctionalState NewState);     // 使能或禁用TIMx自动重装载预装载功能
void TIM_SelectCOM(TIM_TypeDef* TIMx, FunctionalState NewState);            // 选择TIMx的COM事件（仅TIM1、TIM8）
void TIM_SelectCCDMA(TIM_TypeDef* TIMx, FunctionalState NewState);          // 选择TIMx捕获/比较通道的DMA请求（仅TIMx）
void TIM_CCPreloadControl(TIM_TypeDef* TIMx, FunctionalState NewState);     // 使能或禁用捕获/比较通道的预装载控制
void TIM_OC1PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);       // 配置TIMx通道1输出比较预装载使能
void TIM_OC2PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);       // 配置TIMx通道2输出比较预装载使能
void TIM_OC3PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);       // 配置TIMx通道3输出比较预装载使能
void TIM_OC4PreloadConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPreload);       // 配置TIMx通道4输出比较预装载使能
void TIM_OC1FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);             // 配置TIMx通道1快速输出使能
void TIM_OC2FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);             // 配置TIMx通道2快速输出使能
void TIM_OC3FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);             // 配置TIMx通道3快速输出使能
void TIM_OC4FastConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCFast);             // 配置TIMx通道4快速输出使能
void TIM_ClearOC1Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);              // 配置TIMx通道1的OC1REF是否由ETR清零
void TIM_ClearOC2Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);              // 配置TIMx通道2的OC2REF是否由ETR清零
void TIM_ClearOC3Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);              // 配置TIMx通道3的OC3REF是否由ETR清零
void TIM_ClearOC4Ref(TIM_TypeDef* TIMx, uint16_t TIM_OCClear);              // 配置TIMx通道4的OC4REF是否由ETR清零
void TIM_OC1PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);     // 配置TIMx通道1输出极性
void TIM_OC1NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);   // 配置TIMx通道1互补输出极性（仅TIM1、TIM8）
void TIM_OC2PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);     // 配置TIMx通道2输出极性
void TIM_OC2NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);   // 配置TIMx通道2互补输出极性（仅TIM1、TIM8）
void TIM_OC3PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);     // 配置TIMx通道3输出极性
void TIM_OC3NPolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCNPolarity);   // 配置TIMx通道3互补输出极性（仅TIM1、TIM8）
void TIM_OC4PolarityConfig(TIM_TypeDef* TIMx, uint16_t TIM_OCPolarity);     // 配置TIMx通道4输出极性
void TIM_CCxCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx); // 使能或禁用指定通道的捕获/比较功能
void TIM_CCxNCmd(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN); // 使能或禁用指定通道的互补捕获/比较功能（仅TIM1、TIM8）
void TIM_SelectOCxM(TIM_TypeDef* TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode); // 选择指定通道的输出比较模式
void TIM_UpdateDisableConfig(TIM_TypeDef* TIMx, FunctionalState NewState);  // 使能或禁用TIMx更新事件产生
void TIM_UpdateRequestConfig(TIM_TypeDef* TIMx, uint16_t TIM_UpdateSource); // 配置TIMx更新请求源
void TIM_SelectHallSensor(TIM_TypeDef* TIMx, FunctionalState NewState);     // 使能或禁用TIMx霍尔传感器接口
void TIM_SelectOnePulseMode(TIM_TypeDef* TIMx, uint16_t TIM_OPMode);        // 配置TIMx单脉冲模式
void TIM_SelectOutputTrigger(TIM_TypeDef* TIMx, uint16_t TIM_TRGOSource);   // 选择TIMx触发输出源
void TIM_SelectSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_SlaveMode);        // 配置TIMx从模式
void TIM_SelectMasterSlaveMode(TIM_TypeDef* TIMx, uint16_t TIM_MasterSlaveMode); // 配置TIMx主从模式
void TIM_SetCounter(TIM_TypeDef* TIMx, uint16_t Counter);                    // 设置TIMx计数器值
void TIM_SetAutoreload(TIM_TypeDef* TIMx, uint16_t Autoreload);              // 设置TIMx自动重装载值
void TIM_SetCompare1(TIM_TypeDef* TIMx, uint16_t Compare1);                  // 设置TIMx捕获比较寄存器1的值
void TIM_SetCompare2(TIM_TypeDef* TIMx, uint16_t Compare2);                  // 设置TIMx捕获比较寄存器2的值
void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare3);                  // 设置TIMx捕获比较寄存器3的值
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare4);                  // 设置TIMx捕获比较寄存器4的值
void TIM_SetIC1Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);             // 设置TIMx输入捕获1预分频器
void TIM_SetIC2Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);             // 设置TIMx输入捕获2预分频器
void TIM_SetIC3Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);             // 设置TIMx输入捕获3预分频器
void TIM_SetIC4Prescaler(TIM_TypeDef* TIMx, uint16_t TIM_ICPSC);             // 设置TIMx输入捕获4预分频器
void TIM_SetClockDivision(TIM_TypeDef* TIMx, uint16_t TIM_CKD);              // 设置TIMx时钟分频因子
uint16_t TIM_GetCapture1(TIM_TypeDef* TIMx);                                 // 获取TIMx输入捕获1的值
uint16_t TIM_GetCapture2(TIM_TypeDef* TIMx);                                 // 获取TIMx输入捕获2的值
uint16_t TIM_GetCapture3(TIM_TypeDef* TIMx);                                 // 获取TIMx输入捕获3的值
uint16_t TIM_GetCapture4(TIM_TypeDef* TIMx);                                 // 获取TIMx输入捕获4的值
uint16_t TIM_GetCounter(TIM_TypeDef* TIMx);                                  // 获取TIMx当前计数值
uint16_t TIM_GetPrescaler(TIM_TypeDef* TIMx);                                // 获取TIMx当前预分频值
FlagStatus TIM_GetFlagStatus(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);         // 检查指定TIMx标志位是否置位
void TIM_ClearFlag(TIM_TypeDef* TIMx, uint16_t TIM_FLAG);                    // 清除指定TIMx标志位
ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);               // 检查指定TIMx中断是否发生
void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);              // 清除指定TIMx中断挂起位

#ifdef __cplusplus
}
#endif

#endif 