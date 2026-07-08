/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_fsmc.h
* 作者             : WCH
* 版本            : V1.0.1
* 日期               : 2025/03/06
* 描述        : 该文件包含了FSMC固件库的所有函数原型
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V30x_FSMC_H
#define __CH32V30x_FSMC_H

#ifdef __cplusplus
 extern "C" {
#endif


#include "ch32v30x.h"


/* FSMC初始化结构体定义 */
typedef struct
{
  uint32_t FSMC_AddressSetupTime;       /* 定义地址建立时间持续多少个HCLK时钟周期
                                           该参数取值范围为0到0xF
                                           @注意: 该参数不用于同步NOR Flash存储器 */

  uint32_t FSMC_AddressHoldTime;        /* 定义地址保持时间持续多少个HCLK时钟周期
                                           该参数取值范围为0到0xF
                                           @注意: 该参数不用于同步NOR Flash存储器 */

  uint32_t FSMC_DataSetupTime;          /* 定义数据建立时间持续多少个HCLK时钟周期
                                           该参数取值范围为0到0xFF
                                           @注意: 用于SRAM、ROM和异步多路复用NOR Flash存储器 */

  uint32_t FSMC_BusTurnAroundDuration;  /* 定义总线翻转持续时间持续多少个HCLK时钟周期
                                           该参数取值范围为0到0xF
                                           @注意: 仅用于多路复用NOR Flash存储器 */

  uint32_t FSMC_CLKDivision;            /* 定义CLK时钟输出信号的周期，以HCLK时钟周期数表示
                                           该参数取值范围为1到0xF
                                           @注意: 该参数不用于异步NOR Flash、SRAM或ROM访问 */

  uint32_t FSMC_DataLatency;            /* 定义在获取第一个数据之前，向存储器发出多少个存储器时钟周期
                                           该参数的值取决于存储器类型：
                                              - 对于CRAM必须设置为0
                                              - 对于异步NOR、SRAM或ROM访问，此参数无关紧要
                                              - 对于启用同步突发模式的NOR Flash存储器，可以取0到0xF之间的值 */

  uint32_t FSMC_AccessMode;             /* 指定异步访问模式
                                           该参数可以是@ref FSMC_Access_Mode中的值 */
}FSMC_NORSRAMTimingInitTypeDef;


typedef struct
{
  uint32_t FSMC_Bank;                /* 指定要使用的NOR/SRAM存储器块
                                        该参数可以是@ref FSMC_NORSRAM_Bank中的值 */

  uint32_t FSMC_DataAddressMux;      /* 指定地址和数据值是否在数据总线上复用
                                        该参数可以是@ref FSMC_Data_Address_Bus_Multiplexing中的值 */

  uint32_t FSMC_MemoryType;          /* 指定连接到对应存储器块的外部存储器类型
                                        该参数可以是@ref FSMC_Memory_Type中的值 */

  uint32_t FSMC_MemoryDataWidth;     /* 指定外部存储器设备宽度
                                        该参数可以是@ref FSMC_Data_Width中的值 */

  uint32_t FSMC_BurstAccessMode;     /* 启用或禁用Flash存储器的突发访问模式，
                                        仅对同步突发Flash存储器有效
                                        该参数可以是@ref FSMC_Burst_Access_Mode中的值 */
                                       
  uint32_t FSMC_AsynchronousWait;    /* 启用或禁用异步传输期间的等待信号，
                                        仅对异步Flash存储器有效
                                        该参数可以是@ref FSMC_AsynchronousWait中的值 */

  uint32_t FSMC_WaitSignalPolarity;  /* 指定等待信号的极性，仅在以突发模式访问Flash存储器时有效
                                        该参数可以是@ref FSMC_Wait_Signal_Polarity中的值 */

  uint32_t FSMC_WaitSignalActive;    /* 指定等待信号是在等待状态前一个时钟周期还是在等待状态期间由存储器断言，
                                        仅在以突发模式访问存储器时有效
                                        该参数可以是@ref FSMC_Wait_Timing中的值 */

  uint32_t FSMC_WriteOperation;      /* 启用或禁用FSMC在选定存储块中的写操作
                                        该参数可以是@ref FSMC_Write_Operation中的值 */

  uint32_t FSMC_WaitSignal;          /* 启用或禁用通过等待信号插入等待状态，对Flash存储器的突发模式访问有效
                                        该参数可以是@ref FSMC_Wait_Signal中的值 */

  uint32_t FSMC_ExtendedMode;        /* 启用或禁用扩展模式
                                        该参数可以是@ref FSMC_Extended_Mode中的值 */

  uint32_t FSMC_WriteBurst;          /* 启用或禁用写突发操作
                                        该参数可以是@ref FSMC_Write_Burst中的值 */

  FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct; /* 如果未使用扩展模式，读写访问的时序参数 */

  FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;     /* 如果使用扩展模式，写访问的时序参数 */
}FSMC_NORSRAMInitTypeDef;


typedef struct
{
  uint32_t FSMC_SetupTime;      /* 定义在命令断言之前，为NAND-Flash读取或写入公共/属性或I/O存储器空间（取决于要配置的存储器空间时序）
                                   地址建立所需的HCLK时钟周期数
                                   该参数取值范围为0到0xFF */

  uint32_t FSMC_WaitSetupTime;  /* 定义为NAND-Flash读取或写入公共/属性或I/O存储器空间（取决于要配置的存储器空间时序）
                                   命令断言所需的最小HCLK时钟周期数
                                   该参数取值范围为0x00到0xFF */

  uint32_t FSMC_HoldSetupTime;  /* 定义在命令解除断言后，为NAND-Flash读取或写入公共/属性或I/O存储器空间（取决于要配置的存储器空间时序）
                                   地址（和写访问的数据）保持所需的HCLK时钟周期数
                                   该参数取值范围为0x00到0xFF */

  uint32_t FSMC_HiZSetupTime;   /* 定义在NAND-Flash写入公共/属性或I/O存储器空间（取决于要配置的存储器空间时序）开始后，
                                   数据总线保持在高阻态（HiZ）的HCLK时钟周期数
                                   该参数取值范围为0x00到0xFF */
}FSMC_NAND_PCCARDTimingInitTypeDef;


typedef struct
{
  uint32_t FSMC_Bank;             /* 指定要使用的NAND存储器块
                                     该参数可以是@ref FSMC_NAND_Bank中的值 */

  uint32_t FSMC_Waitfeature;      /* 启用或禁用NAND存储器块的等待功能
                                     该参数可以是@ref FSMC_Wait_feature中的任何值 */

  uint32_t FSMC_MemoryDataWidth;  /* 指定外部存储器设备宽度
                                     该参数可以是@ref FSMC_Data_Width中的任何值 */

  uint32_t FSMC_ECC;              /* 启用或禁用ECC计算
                                     该参数可以是@ref FSMC_ECC中的任何值 */

  uint32_t FSMC_ECCPageSize;      /* 定义扩展ECC的页大小
                                     该参数可以是@ref FSMC_ECC_Page_Size中的任何值 */

  uint32_t FSMC_TCLRSetupTime;    /* 定义配置CLE低电平和RE低电平之间延迟的HCLK时钟周期数
                                     该参数取值范围为0到0xFF */

  uint32_t FSMC_TARSetupTime;     /* 定义配置ALE低电平和RE低电平之间延迟的HCLK时钟周期数
                                     该参数取值范围为0x0到0xFF */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_CommonSpaceTimingStruct;   /* FSMC公共空间时序 */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_AttributeSpaceTimingStruct; /* FSMC属性空间时序 */
}FSMC_NANDInitTypeDef;


/* FSMC_NORSRAM_Bank - NOR/SRAM存储器块选择 */
#define FSMC_Bank1_NORSRAM1                             ((uint32_t)0x00000000) /* 选择NOR/SRAM存储器块1 */

/* FSMC_NAND_Bank - NAND存储器块选择 */
#define FSMC_Bank2_NAND                                 ((uint32_t)0x00000010) /* 选择NAND存储器块2 */

/* FSMC_Data_Address_Bus_Multiplexing - 数据地址总线复用配置 */
#define FSMC_DataAddressMux_Disable                     ((uint32_t)0x00000000) /* 禁用数据地址总线复用 */
#define FSMC_DataAddressMux_Enable                      ((uint32_t)0x00000002) /* 启用数据地址总线复用 */

/* FSMC_Memory_Type - 存储器类型选择 */
#define FSMC_MemoryType_SRAM                            ((uint32_t)0x00000000) /* SRAM存储器 */
#define FSMC_MemoryType_PSRAM                           ((uint32_t)0x00000004) /* PSRAM存储器 */
#define FSMC_MemoryType_NOR                             ((uint32_t)0x00000008) /* NOR Flash存储器 */

/* FSMC_Data_Width - 数据宽度配置 */
#define FSMC_MemoryDataWidth_8b                         ((uint32_t)0x00000000) /* 8位数据宽度 */
#define FSMC_MemoryDataWidth_16b                        ((uint32_t)0x00000010) /* 16位数据宽度 */

/* FSMC_Burst_Access_Mode - 突发访问模式配置 */
#define FSMC_BurstAccessMode_Disable                    ((uint32_t)0x00000000) /* 禁用突发访问模式 */ 
#define FSMC_BurstAccessMode_Enable                     ((uint32_t)0x00000100) /* 启用突发访问模式 */

/* FSMC_AsynchronousWait - 异步等待配置 */
#define FSMC_AsynchronousWait_Disable                   ((uint32_t)0x00000000) /* 禁用异步等待 */
#define FSMC_AsynchronousWait_Enable                    ((uint32_t)0x00008000) /* 启用异步等待 */

/* FSMC_Wait_Signal_Polarity - 等待信号极性配置 */
#define FSMC_WaitSignalPolarity_Low                     ((uint32_t)0x00000000) /* 等待信号低电平有效 */
#define FSMC_WaitSignalPolarity_High                    ((uint32_t)0x00000200) /* 等待信号高电平有效 */

/* FSMC_Wait_Timing - 等待信号时序配置 */
#define FSMC_WaitSignalActive_BeforeWaitState           ((uint32_t)0x00000000) /* 等待状态前一个时钟周期断言等待信号 */
#define FSMC_WaitSignalActive_DuringWaitState           ((uint32_t)0x00000800) /* 等待状态期间断言等待信号 */

/* FSMC_Write_Operation - 写操作配置 */
#define FSMC_WriteOperation_Disable                     ((uint32_t)0x00000000) /* 禁用写操作 */
#define FSMC_WriteOperation_Enable                      ((uint32_t)0x00001000) /* 启用写操作 */

/* FSMC_Wait_Signal - 等待信号配置 */
#define FSMC_WaitSignal_Disable                         ((uint32_t)0x00000000) /* 禁用等待信号 */
#define FSMC_WaitSignal_Enable                          ((uint32_t)0x00002000) /* 启用等待信号 */

/* FSMC_Extended_Mode - 扩展模式配置 */
#define FSMC_ExtendedMode_Disable                       ((uint32_t)0x00000000) /* 禁用扩展模式 */
#define FSMC_ExtendedMode_Enable                        ((uint32_t)0x00004000) /* 启用扩展模式 */

/* FSMC_Write_Burst - 写突发配置 */
#define FSMC_WriteBurst_Disable                         ((uint32_t)0x00000000) /* 禁用写突发 */
#define FSMC_WriteBurst_Enable                          ((uint32_t)0x00080000) /* 启用写突发 */

/* FSMC_Access_Mode - 访问模式配置 */
#define FSMC_AccessMode_A                               ((uint32_t)0x00000000) /* 模式A访问 */
#define FSMC_AccessMode_B                               ((uint32_t)0x10000000) /* 模式B访问 */
#define FSMC_AccessMode_C                               ((uint32_t)0x20000000) /* 模式C访问 */
#define FSMC_AccessMode_D                               ((uint32_t)0x30000000) /* 模式D访问 */

/* FSMC_Wait_feature - 等待功能配置 */
#define FSMC_Waitfeature_Disable                        ((uint32_t)0x00000000) /* 禁用等待功能 */
#define FSMC_Waitfeature_Enable                         ((uint32_t)0x00000002) /* 启用等待功能 */

/* FSMC_ECC - ECC配置 */
#define FSMC_ECC_Disable                                ((uint32_t)0x00000000) /* 禁用ECC */
#define FSMC_ECC_Enable                                 ((uint32_t)0x00000040) /* 启用ECC */

/* FSMC_ECC_Page_Size - ECC页大小配置 */
#define FSMC_ECCPageSize_256Bytes                       ((uint32_t)0x00000000) /* ECC页大小为256字节 */
#define FSMC_ECCPageSize_512Bytes                       ((uint32_t)0x00020000) /* ECC页大小为512字节 */
#define FSMC_ECCPageSize_1024Bytes                      ((uint32_t)0x00040000) /* ECC页大小为1024字节 */
#define FSMC_ECCPageSize_2048Bytes                      ((uint32_t)0x00060000) /* ECC页大小为2048字节 */
#define FSMC_ECCPageSize_4096Bytes                      ((uint32_t)0x00080000) /* ECC页大小为4096字节 */
#define FSMC_ECCPageSize_8192Bytes                      ((uint32_t)0x000A0000) /* ECC页大小为8192字节 */

/* FSMC标志位定义 */
#define FSMC_FLAG_FEMPT                                 ((uint32_t)0x00000040) /* FIFO空标志 */


/* 函数声明 */
void FSMC_NORSRAMDeInit(uint32_t FSMC_Bank);                                               /* NOR/SRAM存储器块反初始化 */
void FSMC_NANDDeInit(uint32_t FSMC_Bank);                                                  /* NAND存储器块反初始化 */
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct);                    /* NOR/SRAM存储器块初始化 */
void FSMC_NANDInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct);                             /* NAND存储器块初始化 */
void FSMC_NORSRAMStructInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct);              /* 初始化NOR/SRAM结构体为默认值 */
void FSMC_NANDStructInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct);                       /* 初始化NAND结构体为默认值 */
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState);                        /* 使能或禁用NOR/SRAM存储器块 */
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState);                           /* 使能或禁用NAND存储器块 */
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState);                        /* 使能或禁用NAND存储器块的ECC */
uint32_t FSMC_GetECC(uint32_t FSMC_Bank);                                                  /* 获取NAND存储器块的ECC值 */
FlagStatus FSMC_GetFlagStatus(uint32_t FSMC_Bank, uint32_t FSMC_FLAG);                     /* 获取指定FSMC标志位的状态 */

#ifdef __cplusplus
}
#endif

#endif 
