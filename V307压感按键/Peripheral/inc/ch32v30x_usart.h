/********************************** (C) COPYRIGHT  *******************************
 * 文件名           : ch32v30x_usart.h
 * 作者             : WCH
 * 版本             : V1.0.0
 * 日期             : 2024/03/06
 * 描述             : 本文件包含了USART(通用同步异步收发器)固件库的所有函数原型。
 *********************************************************************************
 * 版权所有 (c) 2021 南京沁恒微电子股份有限公司
 * 注意：本软件（修改或未修改）及二进制文件仅用于南京沁恒微电子制造的微控制器。
 *******************************************************************************/
#ifndef __CH32V30x_USART_H
#define __CH32V30x_USART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ch32v30x.h"

    /* USART初始化结构体定义 */
    typedef struct
    {
        uint32_t USART_BaudRate;            /* 配置USART通信波特率。
                                               波特率使用以下公式计算：
                                                - IntegerDivider = ((PCLKx) / (16 * (USART_InitStruct->USART_BaudRate)))
                                                - FractionalDivider = ((IntegerDivider - ((u32) IntegerDivider)) * 16) + 0.5 */

        uint16_t USART_WordLength;          /* 指定一帧中传输或接收的数据位数。
                                               此参数可以是@ref USART_Word_Length的值 */

        uint16_t USART_StopBits;            /* 指定传输的停止位数。
                                               此参数可以是@ref USART_Stop_Bits的值 */

        uint16_t USART_Parity;              /* 指定奇偶校验模式。
                                               此参数可以是@ref USART_Parity的值
                                               @注意：启用奇偶校验时，计算出的奇偶校验位会插入到
                                                     传输数据的最高有效位（当字长设置为9位数据时是第9位；
                                                     当字长设置为8位数据时是第8位）。 */

        uint16_t USART_Mode;                /* 指定接收或发送模式是启用还是禁用。
                                               此参数可以是@ref USART_Mode的值 */

        uint16_t USART_HardwareFlowControl; /* 指定硬件流控制模式是启用还是禁用。
                                               此参数可以是@ref USART_Hardware_Flow_Control的值 */
    } USART_InitTypeDef;

    /* USART时钟初始化结构体定义 */
    typedef struct
    {
        uint16_t USART_Clock;   /* 指定USART时钟是启用还是禁用。
                                   此参数可以是@ref USART_Clock的值 */

        uint16_t USART_CPOL;    /* 指定串行时钟的稳态值。
                                   此参数可以是@ref USART_Clock_Polarity的值 */

        uint16_t USART_CPHA;    /* 指定进行位捕获的时钟边沿。
                                   此参数可以是@ref USART_Clock_Phase的值 */

        uint16_t USART_LastBit; /* 指定同步模式下最后一个传输数据位（MSB）对应的时钟脉冲
                                   是否必须在SCLK引脚上输出。
                                   此参数可以是@ref USART_Last_Bit的值 */
    } USART_ClockInitTypeDef;

/* USART字长定义 */
#define USART_WordLength_8b ((uint16_t)0x0000) /* 8位数据位 */
#define USART_WordLength_9b ((uint16_t)0x1000) /* 9位数据位 */

/* USART停止位定义 */
#define USART_StopBits_1 ((uint16_t)0x0000)   /* 1个停止位 */
#define USART_StopBits_0_5 ((uint16_t)0x1000) /* 0.5个停止位 */
#define USART_StopBits_2 ((uint16_t)0x2000)   /* 2个停止位 */
#define USART_StopBits_1_5 ((uint16_t)0x3000) /* 1.5个停止位 */

/* USART奇偶校验定义 */
#define USART_Parity_No ((uint16_t)0x0000)   /* 无奇偶校验 */
#define USART_Parity_Even ((uint16_t)0x0400) /* 偶校验 */
#define USART_Parity_Odd ((uint16_t)0x0600)  /* 奇校验 */

/* USART模式定义 */
#define USART_Mode_Rx ((uint16_t)0x0004) /* 启用接收模式 */
#define USART_Mode_Tx ((uint16_t)0x0008) /* 启用发送模式 */

/* USART硬件流控制定义 */
#define USART_HardwareFlowControl_None ((uint16_t)0x0000)    /* 无硬件流控制 */
#define USART_HardwareFlowControl_RTS ((uint16_t)0x0100)     /* 启用RTS流控制 */
#define USART_HardwareFlowControl_CTS ((uint16_t)0x0200)     /* 启用CTS流控制 */
#define USART_HardwareFlowControl_RTS_CTS ((uint16_t)0x0300) /* 同时启用RTS和CTS流控制 */

/* USART时钟使能定义 */
#define USART_Clock_Disable ((uint16_t)0x0000) /* 禁用时钟输出 */
#define USART_Clock_Enable ((uint16_t)0x0800)  /* 启用时钟输出 */

/* USART时钟极性定义 */
#define USART_CPOL_Low ((uint16_t)0x0000)  /* 时钟低电平空闲 */
#define USART_CPOL_High ((uint16_t)0x0400) /* 时钟高电平空闲 */

/* USART时钟相位定义 */
#define USART_CPHA_1Edge ((uint16_t)0x0000) /* 在第一个时钟边沿捕获数据 */
#define USART_CPHA_2Edge ((uint16_t)0x0200) /* 在第二个时钟边沿捕获数据 */

/* USART最后一位时钟脉冲定义 */
#define USART_LastBit_Disable ((uint16_t)0x0000) /* 不输出最后一个数据位的时钟脉冲 */
#define USART_LastBit_Enable ((uint16_t)0x0100)  /* 输出最后一个数据位的时钟脉冲 */

/* USART中断源定义 */
#define USART_IT_PE ((uint16_t)0x0028)     /* 奇偶错误中断 */
#define USART_IT_TXE ((uint16_t)0x0727)    /* 发送数据寄存器空中断 */
#define USART_IT_TC ((uint16_t)0x0626)     /* 传输完成中断 */
#define USART_IT_RXNE ((uint16_t)0x0525)   /* 接收数据寄存器非空中断 */
#define USART_IT_ORE_RX ((uint16_t)0x0325) /* 接收溢出错误中断 */
#define USART_IT_IDLE ((uint16_t)0x0424)   /* 空闲线路检测中断 */
#define USART_IT_LBD ((uint16_t)0x0846)    /* LIN断点检测中断 */
#define USART_IT_CTS ((uint16_t)0x096A)    /* CTS状态变化中断 */
#define USART_IT_ERR ((uint16_t)0x0060)    /* 错误中断（包括溢出、噪声、帧错误） */
#define USART_IT_ORE_ER ((uint16_t)0x0360) /* 溢出错误中断（接收） */
#define USART_IT_NE ((uint16_t)0x0260)     /* 噪声错误中断 */
#define USART_IT_FE ((uint16_t)0x0160)     /* 帧错误中断 */

#define USART_IT_ORE USART_IT_ORE_ER       /* 溢出错误中断（别名） */

/* USART DMA请求定义 */
#define USART_DMAReq_Tx ((uint16_t)0x0080) /* 发送DMA请求 */
#define USART_DMAReq_Rx ((uint16_t)0x0040) /* 接收DMA请求 */

/* USART唤醒方法定义 */
#define USART_WakeUp_IdleLine ((uint16_t)0x0000)    /* 空闲线路唤醒 */
#define USART_WakeUp_AddressMark ((uint16_t)0x0800) /* 地址标记唤醒 */

/* USART LIN断点检测长度定义 */
#define USART_LINBreakDetectLength_10b ((uint16_t)0x0000) /* 10位LIN断点检测长度 */
#define USART_LINBreakDetectLength_11b ((uint16_t)0x0020) /* 11位LIN断点检测长度 */

/* USART IrDA低功耗模式定义 */
#define USART_IrDAMode_LowPower ((uint16_t)0x0004) /* 低功耗IrDA模式 */
#define USART_IrDAMode_Normal ((uint16_t)0x0000)   /* 正常IrDA模式 */

/* USART标志位定义 */
#define USART_FLAG_CTS ((uint16_t)0x0200)  /* CTS状态变化标志 */
#define USART_FLAG_LBD ((uint16_t)0x0100)  /* LIN断点检测标志 */
#define USART_FLAG_TXE ((uint16_t)0x0080)  /* 发送数据寄存器空标志 */
#define USART_FLAG_TC ((uint16_t)0x0040)   /* 传输完成标志 */
#define USART_FLAG_RXNE ((uint16_t)0x0020) /* 接收数据寄存器非空标志 */
#define USART_FLAG_IDLE ((uint16_t)0x0010) /* 空闲线路检测标志 */
#define USART_FLAG_ORE ((uint16_t)0x0008)  /* 溢出错误标志 */
#define USART_FLAG_NE ((uint16_t)0x0004)   /* 噪声错误标志 */
#define USART_FLAG_FE ((uint16_t)0x0002)   /* 帧错误标志 */
#define USART_FLAG_PE ((uint16_t)0x0001)   /* 奇偶错误标志 */


    /* 函数声明部分 */
    void USART_DeInit(USART_TypeDef *USARTx);                                                                            /* 将指定的USART外设恢复为默认状态 */
    void USART_Init(USART_TypeDef *USARTx, USART_InitTypeDef *USART_InitStruct);                                         /* 根据指定的参数初始化USART外设 */
    void USART_StructInit(USART_InitTypeDef *USART_InitStruct);                                                          /* 使用默认值填充USART_InitStruct结构体 */
    void USART_ClockInit(USART_TypeDef *USARTx, USART_ClockInitTypeDef *USART_ClockInitStruct);                          /* 初始化USART时钟相关参数 */
    void USART_ClockStructInit(USART_ClockInitTypeDef *USART_ClockInitStruct);                                           /* 使用默认值填充USART_ClockInitStruct结构体 */
    void USART_Cmd(USART_TypeDef *USARTx, FunctionalState NewState);                                                     /* 启用或禁用指定的USART外设 */
    void USART_ITConfig(USART_TypeDef *USARTx, uint16_t USART_IT, FunctionalState NewState);                             /* 启用或禁用指定的USART中断源 */
    void USART_DMACmd(USART_TypeDef *USARTx, uint16_t USART_DMAReq, FunctionalState NewState);                           /* 启用或禁用USART的DMA请求 */
    void USART_SetAddress(USART_TypeDef *USARTx, uint8_t USART_Address);                                                 /* 设置USART节点的地址（多处理器通信） */
    void USART_WakeUpConfig(USART_TypeDef *USARTx, uint16_t USART_WakeUp);                                               /* 配置USART唤醒模式 */
    void USART_ReceiverWakeUpCmd(USART_TypeDef *USARTx, FunctionalState NewState);                                       /* 启用或禁用接收器唤醒功能 */
    void USART_LINBreakDetectLengthConfig(USART_TypeDef *USARTx, uint16_t USART_LINBreakDetectLength);                   /* 配置LIN断点检测长度 */
    void USART_LINCmd(USART_TypeDef *USARTx, FunctionalState NewState);                                                  /* 启用或禁用LIN模式 */
    void USART_SendData(USART_TypeDef *USARTx, uint16_t Data);                                                           /* 通过USART发送一个数据 */
    uint16_t USART_ReceiveData(USART_TypeDef *USARTx);                                                                   /* 从USART接收一个数据 */
    void USART_SendBreak(USART_TypeDef *USARTx);                                                                         /* 发送断点字符 */
    void USART_SetGuardTime(USART_TypeDef *USARTx, uint8_t USART_GuardTime);                                             /* 设置智能卡模式下的保护时间 */
    void USART_SetPrescaler(USART_TypeDef *USARTx, uint8_t USART_Prescaler);                                             /* 设置USART预分频器（IrDA模式） */
    void USART_SmartCardCmd(USART_TypeDef *USARTx, FunctionalState NewState);                                            /* 启用或禁用智能卡模式 */
    void USART_SmartCardNACKCmd(USART_TypeDef *USARTx, FunctionalState NewState);                                        /* 启用或禁用智能卡NACK传输 */
    void USART_HalfDuplexCmd(USART_TypeDef *USARTx, FunctionalState NewState);                                           /* 启用或禁用半双工模式 */
    void USART_IrDAConfig(USART_TypeDef *USARTx, uint16_t USART_IrDAMode);                                               /* 配置IrDA模式 */
    void USART_IrDACmd(USART_TypeDef *USARTx, FunctionalState NewState);                                                 /* 启用或禁用IrDA模式 */
    FlagStatus USART_GetFlagStatus(USART_TypeDef *USARTx, uint16_t USART_FLAG);                                          /* 检查指定的USART标志位是否置位 */
    void USART_ClearFlag(USART_TypeDef *USARTx, uint16_t USART_FLAG);                                                    /* 清除指定的USART标志位 */
    ITStatus USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT);                                                /* 检查指定的USART中断是否发生 */
    void USART_ClearITPendingBit(USART_TypeDef *USARTx, uint16_t USART_IT);                                              /* 清除USART中断的挂起位 */
#ifdef __cplusplus
}
#endif

#endif