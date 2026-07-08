/********************************** (C) COPYRIGHT  *******************************
 * 文件名          : ch32v30x_dma.h
 * 作者             : WCH
 * 版本            : V1.0.0
 * 日期               : 2021/06/06
 * 描述        : 该文件包含了所有DMA固件库的函数原型。
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * 注意: 本软件（修改或未修改）及二进制文件用于南京沁恒微电子生产的微控制器。
 *******************************************************************************/
#ifndef __CH32V30x_DMA_H
#define __CH32V30x_DMA_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ch32v30x.h"

    /* DMA 初始化结构体定义 */
    typedef struct
    {
        uint32_t DMA_PeripheralBaseAddr; /* 指定DMAy通道x的外设基地址。 */

        uint32_t DMA_MemoryBaseAddr;     /* 指定DMAy通道x的存储器基地址。 */

        uint32_t DMA_DIR;                /* 指定外设是数据传输的源还是目标。
                                            该参数可以是@ref DMA_data_transfer_direction中的一个值 */

        uint32_t DMA_BufferSize;         /* 以数据单元为单位，指定指定通道的缓冲区大小。
                                            数据单元的大小等于根据传输方向在DMA_PeripheralDataSize
                                            或DMA_MemoryDataSize成员中设置的配置。 */

        uint32_t DMA_PeripheralInc;      /* 指定外设地址寄存器是否递增。
                                            该参数可以是@ref DMA_peripheral_incremented_mode中的一个值 */

        uint32_t DMA_MemoryInc;          /* 指定存储器地址寄存器是否递增。
                                            该参数可以是@ref DMA_memory_incremented_mode中的一个值 */

        uint32_t DMA_PeripheralDataSize; /* 指定外设数据宽度。
                                            该参数可以是@ref DMA_peripheral_data_size中的一个值 */

        uint32_t DMA_MemoryDataSize;     /* 指定存储器数据宽度。
                                            该参数可以是@ref DMA_memory_data_size中的一个值 */

        uint32_t DMA_Mode;               /* 指定DMAy通道x的操作模式。
                                            该参数可以是@ref DMA_circular_normal_mode中的一个值。
                                            @注意：如果所选通道配置为存储器到存储器传输，则不能使用循环缓冲区模式 */

        uint32_t DMA_Priority;           /* 指定DMAy通道x的软件优先级。
                                            该参数可以是@ref DMA_priority_level中的一个值 */

        uint32_t DMA_M2M;                /* 指定DMAy通道x是否用于存储器到存储器传输。
                                            该参数可以是@ref DMA_memory_to_memory中的一个值 */
    } DMA_InitTypeDef;

/* DMA数据传输方向定义 */
#define DMA_DIR_PeripheralDST ((uint32_t)0x00000010) /* 外设作为目标 */
#define DMA_DIR_PeripheralSRC ((uint32_t)0x00000000) /* 外设作为源 */

/* DMA外设地址递增模式定义 */
#define DMA_PeripheralInc_Enable ((uint32_t)0x00000040)  /* 外设地址递增使能 */
#define DMA_PeripheralInc_Disable ((uint32_t)0x00000000) /* 外设地址递增禁止 */

/* DMA存储器地址递增模式定义 */
#define DMA_MemoryInc_Enable ((uint32_t)0x00000080)  /* 存储器地址递增使能 */
#define DMA_MemoryInc_Disable ((uint32_t)0x00000000) /* 存储器地址递增禁止 */

/* DMA外设数据宽度定义 */
#define DMA_PeripheralDataSize_Byte ((uint32_t)0x00000000)     /* 外设数据宽度：字节（8位） */
#define DMA_PeripheralDataSize_HalfWord ((uint32_t)0x00000100) /* 外设数据宽度：半字（16位） */
#define DMA_PeripheralDataSize_Word ((uint32_t)0x00000200)     /* 外设数据宽度：字（32位） */

/* DMA存储器数据宽度定义 */
#define DMA_MemoryDataSize_Byte ((uint32_t)0x00000000)     /* 存储器数据宽度：字节（8位） */
#define DMA_MemoryDataSize_HalfWord ((uint32_t)0x00000400) /* 存储器数据宽度：半字（16位） */
#define DMA_MemoryDataSize_Word ((uint32_t)0x00000800)     /* 存储器数据宽度：字（32位） */

/* DMA循环/正常模式定义 */
#define DMA_Mode_Circular ((uint32_t)0x00000020) /* 循环模式 */
#define DMA_Mode_Normal ((uint32_t)0x00000000)   /* 正常（单次）模式 */

/* DMA通道优先级定义 */
#define DMA_Priority_VeryHigh ((uint32_t)0x00003000) /* 优先级：非常高 */
#define DMA_Priority_High ((uint32_t)0x00002000)     /* 优先级：高 */
#define DMA_Priority_Medium ((uint32_t)0x00001000)   /* 优先级：中 */
#define DMA_Priority_Low ((uint32_t)0x00000000)      /* 优先级：低 */

/* DMA存储器到存储器模式使能定义 */
#define DMA_M2M_Enable ((uint32_t)0x00004000)  /* 存储器到存储器模式使能 */
#define DMA_M2M_Disable ((uint32_t)0x00000000) /* 存储器到存储器模式禁止 */

/* DMA中断定义（通用通道中断）*/
#define DMA_IT_TC ((uint32_t)0x00000002) /* 传输完成中断 */
#define DMA_IT_HT ((uint32_t)0x00000004) /* 半传输完成中断 */
#define DMA_IT_TE ((uint32_t)0x00000008) /* 传输错误中断 */

/* DMA1通道中断标志定义 */
#define DMA1_IT_GL1 ((uint32_t)0x00000001) /* DMA1通道1全局中断 */
#define DMA1_IT_TC1 ((uint32_t)0x00000002) /* DMA1通道1传输完成中断 */
#define DMA1_IT_HT1 ((uint32_t)0x00000004) /* DMA1通道1半传输完成中断 */
#define DMA1_IT_TE1 ((uint32_t)0x00000008) /* DMA1通道1传输错误中断 */
#define DMA1_IT_GL2 ((uint32_t)0x00000010) /* DMA1通道2全局中断 */
#define DMA1_IT_TC2 ((uint32_t)0x00000020) /* DMA1通道2传输完成中断 */
#define DMA1_IT_HT2 ((uint32_t)0x00000040) /* DMA1通道2半传输完成中断 */
#define DMA1_IT_TE2 ((uint32_t)0x00000080) /* DMA1通道2传输错误中断 */
#define DMA1_IT_GL3 ((uint32_t)0x00000100) /* DMA1通道3全局中断 */
#define DMA1_IT_TC3 ((uint32_t)0x00000200) /* DMA1通道3传输完成中断 */
#define DMA1_IT_HT3 ((uint32_t)0x00000400) /* DMA1通道3半传输完成中断 */
#define DMA1_IT_TE3 ((uint32_t)0x00000800) /* DMA1通道3传输错误中断 */
#define DMA1_IT_GL4 ((uint32_t)0x00001000) /* DMA1通道4全局中断 */
#define DMA1_IT_TC4 ((uint32_t)0x00002000) /* DMA1通道4传输完成中断 */
#define DMA1_IT_HT4 ((uint32_t)0x00004000) /* DMA1通道4半传输完成中断 */
#define DMA1_IT_TE4 ((uint32_t)0x00008000) /* DMA1通道4传输错误中断 */
#define DMA1_IT_GL5 ((uint32_t)0x00010000) /* DMA1通道5全局中断 */
#define DMA1_IT_TC5 ((uint32_t)0x00020000) /* DMA1通道5传输完成中断 */
#define DMA1_IT_HT5 ((uint32_t)0x00040000) /* DMA1通道5半传输完成中断 */
#define DMA1_IT_TE5 ((uint32_t)0x00080000) /* DMA1通道5传输错误中断 */
#define DMA1_IT_GL6 ((uint32_t)0x00100000) /* DMA1通道6全局中断 */
#define DMA1_IT_TC6 ((uint32_t)0x00200000) /* DMA1通道6传输完成中断 */
#define DMA1_IT_HT6 ((uint32_t)0x00400000) /* DMA1通道6半传输完成中断 */
#define DMA1_IT_TE6 ((uint32_t)0x00800000) /* DMA1通道6传输错误中断 */
#define DMA1_IT_GL7 ((uint32_t)0x01000000) /* DMA1通道7全局中断 */
#define DMA1_IT_TC7 ((uint32_t)0x02000000) /* DMA1通道7传输完成中断 */
#define DMA1_IT_HT7 ((uint32_t)0x04000000) /* DMA1通道7半传输完成中断 */
#define DMA1_IT_TE7 ((uint32_t)0x08000000) /* DMA1通道7传输错误中断 */

/* DMA2通道中断标志定义 (通道1-7) */
#define DMA2_IT_GL1 ((uint32_t)0x10000001) /* DMA2通道1全局中断 */
#define DMA2_IT_TC1 ((uint32_t)0x10000002) /* DMA2通道1传输完成中断 */
#define DMA2_IT_HT1 ((uint32_t)0x10000004) /* DMA2通道1半传输完成中断 */
#define DMA2_IT_TE1 ((uint32_t)0x10000008) /* DMA2通道1传输错误中断 */
#define DMA2_IT_GL2 ((uint32_t)0x10000010) /* DMA2通道2全局中断 */
#define DMA2_IT_TC2 ((uint32_t)0x10000020) /* DMA2通道2传输完成中断 */
#define DMA2_IT_HT2 ((uint32_t)0x10000040) /* DMA2通道2半传输完成中断 */
#define DMA2_IT_TE2 ((uint32_t)0x10000080) /* DMA2通道2传输错误中断 */
#define DMA2_IT_GL3 ((uint32_t)0x10000100) /* DMA2通道3全局中断 */
#define DMA2_IT_TC3 ((uint32_t)0x10000200) /* DMA2通道3传输完成中断 */
#define DMA2_IT_HT3 ((uint32_t)0x10000400) /* DMA2通道3半传输完成中断 */
#define DMA2_IT_TE3 ((uint32_t)0x10000800) /* DMA2通道3传输错误中断 */
#define DMA2_IT_GL4 ((uint32_t)0x10001000) /* DMA2通道4全局中断 */
#define DMA2_IT_TC4 ((uint32_t)0x10002000) /* DMA2通道4传输完成中断 */
#define DMA2_IT_HT4 ((uint32_t)0x10004000) /* DMA2通道4半传输完成中断 */
#define DMA2_IT_TE4 ((uint32_t)0x10008000) /* DMA2通道4传输错误中断 */
#define DMA2_IT_GL5 ((uint32_t)0x10010000) /* DMA2通道5全局中断 */
#define DMA2_IT_TC5 ((uint32_t)0x10020000) /* DMA2通道5传输完成中断 */
#define DMA2_IT_HT5 ((uint32_t)0x10040000) /* DMA2通道5半传输完成中断 */
#define DMA2_IT_TE5 ((uint32_t)0x10080000) /* DMA2通道5传输错误中断 */
#define DMA2_IT_GL6 ((uint32_t)0x10100000) /* DMA2通道6全局中断 */
#define DMA2_IT_TC6 ((uint32_t)0x10200000) /* DMA2通道6传输完成中断 */
#define DMA2_IT_HT6 ((uint32_t)0x10400000) /* DMA2通道6半传输完成中断 */
#define DMA2_IT_TE6 ((uint32_t)0x10800000) /* DMA2通道6传输错误中断 */
#define DMA2_IT_GL7 ((uint32_t)0x11000000) /* DMA2通道7全局中断 */
#define DMA2_IT_TC7 ((uint32_t)0x12000000) /* DMA2通道7传输完成中断 */
#define DMA2_IT_HT7 ((uint32_t)0x14000000) /* DMA2通道7半传输完成中断 */
#define DMA2_IT_TE7 ((uint32_t)0x18000000) /* DMA2通道7传输错误中断 */

/* DMA2通道中断标志定义 (通道8-11) */
#define DMA2_IT_GL8 ((uint32_t)0x20000001)  /* DMA2通道8全局中断 */
#define DMA2_IT_TC8 ((uint32_t)0x20000002)  /* DMA2通道8传输完成中断 */
#define DMA2_IT_HT8 ((uint32_t)0x20000004)  /* DMA2通道8半传输完成中断 */
#define DMA2_IT_TE8 ((uint32_t)0x20000008)  /* DMA2通道8传输错误中断 */
#define DMA2_IT_GL9 ((uint32_t)0x20000010)  /* DMA2通道9全局中断 */
#define DMA2_IT_TC9 ((uint32_t)0x20000020)  /* DMA2通道9传输完成中断 */
#define DMA2_IT_HT9 ((uint32_t)0x20000040)  /* DMA2通道9半传输完成中断 */
#define DMA2_IT_TE9 ((uint32_t)0x20000080)  /* DMA2通道9传输错误中断 */
#define DMA2_IT_GL10 ((uint32_t)0x20000100) /* DMA2通道10全局中断 */
#define DMA2_IT_TC10 ((uint32_t)0x20000200) /* DMA2通道10传输完成中断 */
#define DMA2_IT_HT10 ((uint32_t)0x20000400) /* DMA2通道10半传输完成中断 */
#define DMA2_IT_TE10 ((uint32_t)0x20000800) /* DMA2通道10传输错误中断 */
#define DMA2_IT_GL11 ((uint32_t)0x20001000) /* DMA2通道11全局中断 */
#define DMA2_IT_TC11 ((uint32_t)0x20002000) /* DMA2通道11传输完成中断 */
#define DMA2_IT_HT11 ((uint32_t)0x20004000) /* DMA2通道11半传输完成中断 */
#define DMA2_IT_TE11 ((uint32_t)0x20008000) /* DMA2通道11传输错误中断 */

/* DMA1状态标志定义 */
#define DMA1_FLAG_GL1 ((uint32_t)0x00000001) /* DMA1通道1全局标志 */
#define DMA1_FLAG_TC1 ((uint32_t)0x00000002) /* DMA1通道1传输完成标志 */
#define DMA1_FLAG_HT1 ((uint32_t)0x00000004) /* DMA1通道1半传输完成标志 */
#define DMA1_FLAG_TE1 ((uint32_t)0x00000008) /* DMA1通道1传输错误标志 */
#define DMA1_FLAG_GL2 ((uint32_t)0x00000010) /* DMA1通道2全局标志 */
#define DMA1_FLAG_TC2 ((uint32_t)0x00000020) /* DMA1通道2传输完成标志 */
#define DMA1_FLAG_HT2 ((uint32_t)0x00000040) /* DMA1通道2半传输完成标志 */
#define DMA1_FLAG_TE2 ((uint32_t)0x00000080) /* DMA1通道2传输错误标志 */
#define DMA1_FLAG_GL3 ((uint32_t)0x00000100) /* DMA1通道3全局标志 */
#define DMA1_FLAG_TC3 ((uint32_t)0x00000200) /* DMA1通道3传输完成标志 */
#define DMA1_FLAG_HT3 ((uint32_t)0x00000400) /* DMA1通道3半传输完成标志 */
#define DMA1_FLAG_TE3 ((uint32_t)0x00000800) /* DMA1通道3传输错误标志 */
#define DMA1_FLAG_GL4 ((uint32_t)0x00001000) /* DMA1通道4全局标志 */
#define DMA1_FLAG_TC4 ((uint32_t)0x00002000) /* DMA1通道4传输完成标志 */
#define DMA1_FLAG_HT4 ((uint32_t)0x00004000) /* DMA1通道4半传输完成标志 */
#define DMA1_FLAG_TE4 ((uint32_t)0x00008000) /* DMA1通道4传输错误标志 */
#define DMA1_FLAG_GL5 ((uint32_t)0x00010000) /* DMA1通道5全局标志 */
#define DMA1_FLAG_TC5 ((uint32_t)0x00020000) /* DMA1通道5传输完成标志 */
#define DMA1_FLAG_HT5 ((uint32_t)0x00040000) /* DMA1通道5半传输完成标志 */
#define DMA1_FLAG_TE5 ((uint32_t)0x00080000) /* DMA1通道5传输错误标志 */
#define DMA1_FLAG_GL6 ((uint32_t)0x00100000) /* DMA1通道6全局标志 */
#define DMA1_FLAG_TC6 ((uint32_t)0x00200000) /* DMA1通道6传输完成标志 */
#define DMA1_FLAG_HT6 ((uint32_t)0x00400000) /* DMA1通道6半传输完成标志 */
#define DMA1_FLAG_TE6 ((uint32_t)0x00800000) /* DMA1通道6传输错误标志 */
#define DMA1_FLAG_GL7 ((uint32_t)0x01000000) /* DMA1通道7全局标志 */
#define DMA1_FLAG_TC7 ((uint32_t)0x02000000) /* DMA1通道7传输完成标志 */
#define DMA1_FLAG_HT7 ((uint32_t)0x04000000) /* DMA1通道7半传输完成标志 */
#define DMA1_FLAG_TE7 ((uint32_t)0x08000000) /* DMA1通道7传输错误标志 */

/* DMA2状态标志定义 (通道1-7) */
#define DMA2_FLAG_GL1 ((uint32_t)0x10000001) /* DMA2通道1全局标志 */
#define DMA2_FLAG_TC1 ((uint32_t)0x10000002) /* DMA2通道1传输完成标志 */
#define DMA2_FLAG_HT1 ((uint32_t)0x10000004) /* DMA2通道1半传输完成标志 */
#define DMA2_FLAG_TE1 ((uint32_t)0x10000008) /* DMA2通道1传输错误标志 */
#define DMA2_FLAG_GL2 ((uint32_t)0x10000010) /* DMA2通道2全局标志 */
#define DMA2_FLAG_TC2 ((uint32_t)0x10000020) /* DMA2通道2传输完成标志 */
#define DMA2_FLAG_HT2 ((uint32_t)0x10000040) /* DMA2通道2半传输完成标志 */
#define DMA2_FLAG_TE2 ((uint32_t)0x10000080) /* DMA2通道2传输错误标志 */
#define DMA2_FLAG_GL3 ((uint32_t)0x10000100) /* DMA2通道3全局标志 */
#define DMA2_FLAG_TC3 ((uint32_t)0x10000200) /* DMA2通道3传输完成标志 */
#define DMA2_FLAG_HT3 ((uint32_t)0x10000400) /* DMA2通道3半传输完成标志 */
#define DMA2_FLAG_TE3 ((uint32_t)0x10000800) /* DMA2通道3传输错误标志 */
#define DMA2_FLAG_GL4 ((uint32_t)0x10001000) /* DMA2通道4全局标志 */
#define DMA2_FLAG_TC4 ((uint32_t)0x10002000) /* DMA2通道4传输完成标志 */
#define DMA2_FLAG_HT4 ((uint32_t)0x10004000) /* DMA2通道4半传输完成标志 */
#define DMA2_FLAG_TE4 ((uint32_t)0x10008000) /* DMA2通道4传输错误标志 */
#define DMA2_FLAG_GL5 ((uint32_t)0x10010000) /* DMA2通道5全局标志 */
#define DMA2_FLAG_TC5 ((uint32_t)0x10020000) /* DMA2通道5传输完成标志 */
#define DMA2_FLAG_HT5 ((uint32_t)0x10040000) /* DMA2通道5半传输完成标志 */
#define DMA2_FLAG_TE5 ((uint32_t)0x10080000) /* DMA2通道5传输错误标志 */
#define DMA2_FLAG_GL6 ((uint32_t)0x10100000) /* DMA2通道6全局标志 */
#define DMA2_FLAG_TC6 ((uint32_t)0x10200000) /* DMA2通道6传输完成标志 */
#define DMA2_FLAG_HT6 ((uint32_t)0x10400000) /* DMA2通道6半传输完成标志 */
#define DMA2_FLAG_TE6 ((uint32_t)0x10800000) /* DMA2通道6传输错误标志 */
#define DMA2_FLAG_GL7 ((uint32_t)0x11000000) /* DMA2通道7全局标志 */
#define DMA2_FLAG_TC7 ((uint32_t)0x12000000) /* DMA2通道7传输完成标志 */
#define DMA2_FLAG_HT7 ((uint32_t)0x14000000) /* DMA2通道7半传输完成标志 */
#define DMA2_FLAG_TE7 ((uint32_t)0x18000000) /* DMA2通道7传输错误标志 */

/* DMA2状态标志定义 (通道8-11) */
#define DMA2_FLAG_GL8 ((uint32_t)0x20000001)  /* DMA2通道8全局标志 */
#define DMA2_FLAG_TC8 ((uint32_t)0x20000002)  /* DMA2通道8传输完成标志 */
#define DMA2_FLAG_HT8 ((uint32_t)0x20000004)  /* DMA2通道8半传输完成标志 */
#define DMA2_FLAG_TE8 ((uint32_t)0x20000008)  /* DMA2通道8传输错误标志 */
#define DMA2_FLAG_GL9 ((uint32_t)0x20000010)  /* DMA2通道9全局标志 */
#define DMA2_FLAG_TC9 ((uint32_t)0x20000020)  /* DMA2通道9传输完成标志 */
#define DMA2_FLAG_HT9 ((uint32_t)0x20000040)  /* DMA2通道9半传输完成标志 */
#define DMA2_FLAG_TE9 ((uint32_t)0x20000080)  /* DMA2通道9传输错误标志 */
#define DMA2_FLAG_GL10 ((uint32_t)0x20000100) /* DMA2通道10全局标志 */
#define DMA2_FLAG_TC10 ((uint32_t)0x20000200) /* DMA2通道10传输完成标志 */
#define DMA2_FLAG_HT10 ((uint32_t)0x20000400) /* DMA2通道10半传输完成标志 */
#define DMA2_FLAG_TE10 ((uint32_t)0x20000800) /* DMA2通道10传输错误标志 */
#define DMA2_FLAG_GL11 ((uint32_t)0x20001000) /* DMA2通道11全局标志 */
#define DMA2_FLAG_TC11 ((uint32_t)0x20002000) /* DMA2通道11传输完成标志 */
#define DMA2_FLAG_HT11 ((uint32_t)0x20004000) /* DMA2通道11半传输完成标志 */
#define DMA2_FLAG_TE11 ((uint32_t)0x20008000) /* DMA2通道11传输错误标志 */

/* 函数声明 */
void DMA_DeInit(DMA_Channel_TypeDef *DMAy_Channelx);                                              /* 将DMAy通道x寄存器重设为缺省值 */
void DMA_Init(DMA_Channel_TypeDef *DMAy_Channelx, DMA_InitTypeDef *DMA_InitStruct);               /* 根据DMA_InitStruct中指定的参数初始化DMAy通道x */
void DMA_StructInit(DMA_InitTypeDef *DMA_InitStruct);                                             /* 把DMA_InitStruct中的每一个参数按缺省值填入 */
void DMA_Cmd(DMA_Channel_TypeDef *DMAy_Channelx, FunctionalState NewState);                       /* 使能或者失能指定的DMAy通道x */
void DMA_ITConfig(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState); /* 使能或者失能指定的DMAy通道x中断 */
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx, uint16_t DataNumber);             /* 设置DMAy通道x的当前数据单元数目 */
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx);                              /* 返回当前DMAy通道x剩余的数据单元数目 */
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);                                                 /* 检查指定的DMAy标志位设置与否 */
void DMA_ClearFlag(uint32_t DMAy_FLAG);                                                           /* 清除DMAy的待处理标志位 */
ITStatus DMA_GetITStatus(uint32_t DMAy_IT);                                                       /* 检查指定的DMAy中断发生与否 */
void DMA_ClearITPendingBit(uint32_t DMAy_IT);                                                     /* 清除DMAy的中断待处理位 */

#ifdef __cplusplus
}
#endif

#endif
