/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_spi.h
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 该文件包含了所有SPI固件库的函数原型声明。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意: 本软件（无论修改与否）及二进制文件用于南京沁恒微电子生产的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_SPI_H
#define __CH32V30x_SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* SPI初始化结构体定义 */
typedef struct
{
  uint16_t SPI_Direction;           /* 指定SPI的单向或双向数据模式。
                                       该参数可以是@ref SPI_data_direction 中的一个值 */

  uint16_t SPI_Mode;                /* 指定SPI工作模式（主/从）。
                                       该参数可以是@ref SPI_mode 中的一个值 */

  uint16_t SPI_DataSize;            /* 指定SPI数据帧大小（8位/16位）。
                                       该参数可以是@ref SPI_data_size 中的一个值 */

  uint16_t SPI_CPOL;                /* 指定串行时钟的稳态（空闲状态电平）。
                                       该参数可以是@ref SPI_Clock_Polarity 中的一个值 */

  uint16_t SPI_CPHA;                /* 指定数据捕获的时钟有效边沿（第一个还是第二个边沿）。
                                       该参数可以是@ref SPI_Clock_Phase 中的一个值 */

  uint16_t SPI_NSS;                 /* 指定NSS（从机选择）信号是由硬件（NSS引脚）管理还是软件管理（使用SSI位）。
                                       该参数可以是@ref SPI_Slave_Select_management 中的一个值 */
 
  uint16_t SPI_BaudRatePrescaler;   /* 指定波特率预分频值，用于配置发送和接收的SCK时钟频率。
                                       该参数可以是@ref SPI_BaudRate_Prescaler 中的一个值。
                                       @注意：通信时钟来自主时钟。从机时钟无需设置。 */

  uint16_t SPI_FirstBit;            /* 指定数据传输是从最高位（MSB）还是最低位（LSB）开始。
                                       该参数可以是@ref SPI_MSB_LSB_transmission 中的一个值 */

  uint16_t SPI_CRCPolynomial;       /* 指定用于CRC计算的多项式值。 */
}SPI_InitTypeDef;

/* I2S初始化结构体定义 */
typedef struct
{

  uint16_t I2S_Mode;         /* 指定I2S工作模式（主/从、发送/接收）。
                               该参数可以是@ref I2S_Mode 中的一个值 */

  uint16_t I2S_Standard;     /* 指定I2S通信使用的标准（如飞利浦标准等）。
                               该参数可以是@ref I2S_Standard 中的一个值 */

  uint16_t I2S_DataFormat;   /* 指定I2S通信的数据格式（数据位长度）。
                               该参数可以是@ref I2S_Data_Format 中的一个值 */

  uint16_t I2S_MCLKOutput;   /* 指定是否启用I2S主时钟（MCLK）输出。
                               该参数可以是@ref I2S_MCLK_Output 中的一个值 */

  uint32_t I2S_AudioFreq;    /* 指定I2S通信的音频频率。
                               该参数可以是@ref I2S_Audio_Frequency 中的一个值 */

  uint16_t I2S_CPOL;         /* 指定I2S时钟的空闲状态（时钟极性）。
                               该参数可以是@ref I2S_Clock_Polarity 中的一个值 */
}I2S_InitTypeDef;

/* SPI数据方向定义 */  
#define SPI_Direction_2Lines_FullDuplex ((uint16_t)0x0000)  /* 双线全双工模式 */
#define SPI_Direction_2Lines_RxOnly     ((uint16_t)0x0400)  /* 双线只接收模式 */
#define SPI_Direction_1Line_Rx          ((uint16_t)0x8000)  /* 单线只接收模式 */
#define SPI_Direction_1Line_Tx          ((uint16_t)0xC000)  /* 单线只发送模式 */

/* SPI模式定义 */
#define SPI_Mode_Master                 ((uint16_t)0x0104)  /* 主模式 */
#define SPI_Mode_Slave                  ((uint16_t)0x0000)  /* 从模式 */

/* SPI数据大小定义 */
#define SPI_DataSize_16b                ((uint16_t)0x0800)  /* 16位数据帧 */
#define SPI_DataSize_8b                 ((uint16_t)0x0000)  /* 8位数据帧 */

/* SPI时钟极性定义 */
#define SPI_CPOL_Low                    ((uint16_t)0x0000)  /* 时钟空闲时为低电平 */
#define SPI_CPOL_High                   ((uint16_t)0x0002)  /* 时钟空闲时为高电平 */

/* SPI时钟相位定义 */
#define SPI_CPHA_1Edge                  ((uint16_t)0x0000)  /* 在第一个时钟边沿捕获数据 */
#define SPI_CPHA_2Edge                  ((uint16_t)0x0001)  /* 在第二个时钟边沿捕获数据 */

/* SPI从机选择管理方式定义 */
#define SPI_NSS_Soft                    ((uint16_t)0x0200)  /* 软件管理NSS信号 */
#define SPI_NSS_Hard                    ((uint16_t)0x0000)  /* 硬件管理NSS信号 */

/* SPI波特率预分频器定义 */
#define SPI_BaudRatePrescaler_2         ((uint16_t)0x0000)  /* 2分频 */
#define SPI_BaudRatePrescaler_4         ((uint16_t)0x0008)  /* 4分频 */
#define SPI_BaudRatePrescaler_8         ((uint16_t)0x0010)  /* 8分频 */
#define SPI_BaudRatePrescaler_16        ((uint16_t)0x0018)  /* 16分频 */
#define SPI_BaudRatePrescaler_32        ((uint16_t)0x0020)  /* 32分频 */
#define SPI_BaudRatePrescaler_64        ((uint16_t)0x0028)  /* 64分频 */
#define SPI_BaudRatePrescaler_128       ((uint16_t)0x0030)  /* 128分频 */
#define SPI_BaudRatePrescaler_256       ((uint16_t)0x0038)  /* 256分频 */

/* SPI数据传输起始位定义 */
#define SPI_FirstBit_MSB                ((uint16_t)0x0000)  /* 从最高位（MSB）开始传输 */
#define SPI_FirstBit_LSB                ((uint16_t)0x0080)  /* 从最低位（LSB）开始传输 */

/* I2S工作模式定义 */
#define I2S_Mode_SlaveTx                ((uint16_t)0x0000)  /* 从机发送模式 */
#define I2S_Mode_SlaveRx                ((uint16_t)0x0100)  /* 从机接收模式 */
#define I2S_Mode_MasterTx               ((uint16_t)0x0200)  /* 主机发送模式 */
#define I2S_Mode_MasterRx               ((uint16_t)0x0300)  /* 主机接收模式 */

/* I2S标准定义 */
#define I2S_Standard_Phillips           ((uint16_t)0x0000)  /* 飞利浦标准 */
#define I2S_Standard_MSB                ((uint16_t)0x0010)  /* MSB对齐标准 */
#define I2S_Standard_LSB                ((uint16_t)0x0020)  /* LSB对齐标准 */
#define I2S_Standard_PCMShort           ((uint16_t)0x0030)  /* PCM短帧标准 */
#define I2S_Standard_PCMLong            ((uint16_t)0x00B0)  /* PCM长帧标准 */

/* I2S数据格式定义 */
#define I2S_DataFormat_16b              ((uint16_t)0x0000)  /* 16位数据长度 */
#define I2S_DataFormat_16bextended      ((uint16_t)0x0001)  /* 16位扩展数据长度（32位数据包中的16位数据） */
#define I2S_DataFormat_24b              ((uint16_t)0x0003)  /* 24位数据长度 */
#define I2S_DataFormat_32b              ((uint16_t)0x0005)  /* 32位数据长度 */

/* I2S主时钟输出使能定义 */
#define I2S_MCLKOutput_Enable           ((uint16_t)0x0200)  /* 使能主时钟（MCLK）输出 */
#define I2S_MCLKOutput_Disable          ((uint16_t)0x0000)  /* 禁用主时钟（MCLK）输出 */

/* I2S音频频率定义 */
#define I2S_AudioFreq_192k              ((uint32_t)192000)  /* 192 kHz */
#define I2S_AudioFreq_96k               ((uint32_t)96000)   /* 96 kHz */
#define I2S_AudioFreq_48k               ((uint32_t)48000)   /* 48 kHz */
#define I2S_AudioFreq_44k               ((uint32_t)44100)   /* 44.1 kHz */
#define I2S_AudioFreq_32k               ((uint32_t)32000)   /* 32 kHz */
#define I2S_AudioFreq_22k               ((uint32_t)22050)   /* 22.05 kHz */
#define I2S_AudioFreq_16k               ((uint32_t)16000)   /* 16 kHz */
#define I2S_AudioFreq_11k               ((uint32_t)11025)   /* 11.025 kHz */
#define I2S_AudioFreq_8k                ((uint32_t)8000)    /* 8 kHz */
#define I2S_AudioFreq_Default           ((uint32_t)2)       /* 默认频率（具体值参考手册） */

/* I2S时钟极性定义 */
#define I2S_CPOL_Low                    ((uint16_t)0x0000)  /* I2S时钟空闲时为低电平 */
#define I2S_CPOL_High                   ((uint16_t)0x0008)  /* I2S时钟空闲时为高电平 */

/* SPI/I2S DMA传输请求定义 */
#define SPI_I2S_DMAReq_Tx               ((uint16_t)0x0002)  /* 发送DMA请求使能 */
#define SPI_I2S_DMAReq_Rx               ((uint16_t)0x0001)  /* 接收DMA请求使能 */

/* SPI内部软件NSS管理定义（软件模式下控制内部NSS信号电平） */
#define SPI_NSSInternalSoft_Set         ((uint16_t)0x0100)  /* 内部NSS信号置1 */
#define SPI_NSSInternalSoft_Reset       ((uint16_t)0xFEFF)  /* 内部NSS信号清零 */

/* SPI CRC传输方向定义 */
#define SPI_CRC_Tx                      ((uint8_t)0x00)     /* 发送CRC值 */
#define SPI_CRC_Rx                      ((uint8_t)0x01)     /* 接收CRC值 */

/* SPI方向（单线模式下选择发送或接收） */
#define SPI_Direction_Rx                ((uint16_t)0xBFFF)  /* 单线只接收模式 */
#define SPI_Direction_Tx                ((uint16_t)0x4000)  /* 单线只发送模式 */

/* SPI/I2S中断定义 */
#define SPI_I2S_IT_TXE                  ((uint8_t)0x71)     /* 发送缓冲区空中断 */
#define SPI_I2S_IT_RXNE                 ((uint8_t)0x60)     /* 接收缓冲区非空中断 */
#define SPI_I2S_IT_ERR                  ((uint8_t)0x50)     /* 错误中断 */
#define SPI_I2S_IT_OVR                  ((uint8_t)0x56)     /* 溢出错误中断 */
#define SPI_IT_MODF                     ((uint8_t)0x55)     /* 模式错误中断 */
#define SPI_IT_CRCERR                   ((uint8_t)0x54)     /* CRC错误中断 */
#define I2S_IT_UDR                      ((uint8_t)0x53)     /* 下溢错误中断（I2S发送时数据提供不足） */

/* SPI/I2S标志位定义 */
#define SPI_I2S_FLAG_RXNE               ((uint16_t)0x0001)  /* 接收缓冲区非空标志 */
#define SPI_I2S_FLAG_TXE                ((uint16_t)0x0002)  /* 发送缓冲区空标志 */
#define I2S_FLAG_CHSIDE                 ((uint16_t)0x0004)  /* 声道侧标志（左/右声道） */
#define I2S_FLAG_UDR                    ((uint16_t)0x0008)  /* 下溢标志 */
#define SPI_FLAG_CRCERR                 ((uint16_t)0x0010)  /* CRC错误标志 */
#define SPI_FLAG_MODF                   ((uint16_t)0x0020)  /* 模式错误标志 */
#define SPI_I2S_FLAG_OVR                ((uint16_t)0x0040)  /* 溢出错误标志 */
#define SPI_I2S_FLAG_BSY                ((uint16_t)0x0080)  /* 忙标志（正在通信） */


void SPI_I2S_DeInit(SPI_TypeDef* SPIx);                                          /* 将SPI/I2S外设寄存器恢复为默认值 */
void SPI_Init(SPI_TypeDef* SPIx, SPI_InitTypeDef* SPI_InitStruct);               /* 根据SPI_InitStruct中的参数初始化SPI外设 */
void I2S_Init(SPI_TypeDef* SPIx, I2S_InitTypeDef* I2S_InitStruct);               /* 根据I2S_InitStruct中的参数初始化I2S外设 */
void SPI_StructInit(SPI_InitTypeDef* SPI_InitStruct);                            /* 将SPI_InitStruct的各个成员设置为默认值 */
void I2S_StructInit(I2S_InitTypeDef* I2S_InitStruct);                            /* 将I2S_InitStruct的各个成员设置为默认值 */
void SPI_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);                       /* 使能或禁用指定的SPI外设 */
void I2S_Cmd(SPI_TypeDef* SPIx, FunctionalState NewState);                       /* 使能或禁用指定的I2S外设 */
void SPI_I2S_ITConfig(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState); /* 使能或禁用指定的SPI/I2S中断 */
void SPI_I2S_DMACmd(SPI_TypeDef* SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState); /* 使能或禁用SPI/I2S的DMA请求 */
void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);                         /* 通过SPI/I2S发送一个数据 */
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);                                 /* 返回通过SPI/I2S最新接收到的数据 */
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef* SPIx, uint16_t SPI_NSSInternalSoft); /* 在软件从机管理模式下配置内部NSS信号电平 */
void SPI_SSOutputCmd(SPI_TypeDef* SPIx, FunctionalState NewState);               /* 使能或禁用SPI主模式下的SS输出（用于多主模式） */
void SPI_DataSizeConfig(SPI_TypeDef* SPIx, uint16_t SPI_DataSize);               /* 配置SPI数据帧大小（8位或16位） */
void SPI_TransmitCRC(SPI_TypeDef* SPIx);                                         /* 发送CRC值（用于SPI硬件CRC功能） */
void SPI_CalculateCRC(SPI_TypeDef* SPIx, FunctionalState NewState);              /* 使能或禁用SPI的CRC计算功能 */
uint16_t SPI_GetCRC(SPI_TypeDef* SPIx, uint8_t SPI_CRC);                         /* 获取指定CRC寄存器的值（发送或接收CRC） */
uint16_t SPI_GetCRCPolynomial(SPI_TypeDef* SPIx);                                /* 获取SPI CRC多项式寄存器的值 */
void SPI_BiDirectionalLineConfig(SPI_TypeDef* SPIx, uint16_t SPI_Direction);     /* 在单线双向模式下选择数据传输方向（发送或接收） */
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);      /* 检查指定的SPI/I2S标志位是否置位 */
void SPI_I2S_ClearFlag(SPI_TypeDef* SPIx, uint16_t SPI_I2S_FLAG);                /* 清除指定的SPI/I2S标志位 */
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);             /* 检查指定的SPI/I2S中断是否发生 */
void SPI_I2S_ClearITPendingBit(SPI_TypeDef* SPIx, uint8_t SPI_I2S_IT);           /* 清除指定的SPI/I2S中断挂起位 */

#ifdef __cplusplus
}
#endif

#endif
