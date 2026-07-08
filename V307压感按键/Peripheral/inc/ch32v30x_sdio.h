/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_sdio.h
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 该文件包含了SDIO固件库的所有函数原型。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（无论是否修改过）及二进制文件仅用于南京沁恒微电子生产的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_SDIO_H
#define __CH32V30x_SDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* SDIO 初始化结构体定义 */
typedef struct
{
  uint32_t SDIO_ClockEdge;            /* 指定数据位捕获发生在时钟的哪个边沿。
                                         该参数可以是 @ref SDIO_Clock_Edge 中的值 */

  uint32_t SDIO_ClockBypass;          /* 指定是否使能SDIO时钟分频器旁路。
                                         该参数可以是 @ref SDIO_Clock_Bypass 中的值 */

  uint32_t SDIO_ClockPowerSave;       /* 指定总线空闲时SDIO时钟输出是否使能。
                                         该参数可以是 @ref SDIO_Clock_Power_Save 中的值 */

  uint32_t SDIO_BusWide;              /* 指定SDIO总线宽度。
                                         该参数可以是 @ref SDIO_Bus_Wide 中的值 */

  uint32_t SDIO_HardwareFlowControl;  /* 指定是否使能SDIO硬件流控制。
                                         该参数可以是 @ref SDIO_Hardware_Flow_Control 中的值 */

  uint8_t SDIO_ClockDiv;              /* 指定SDIO控制器的时钟频率分频系数。
                                         该参数可以是0x00到0xFF之间的值 */
                                           
} SDIO_InitTypeDef;

/* SDIO 命令初始化结构体定义 */
typedef struct
{
  uint32_t SDIO_Argument;  /* 指定作为命令消息一部分发送给卡片的命令参数。
                              如果命令包含参数，必须先将参数加载到此寄存器，再将命令写入命令寄存器 */

  uint32_t SDIO_CmdIndex;  /* 指定SDIO命令索引。必须小于0x40。 */

  uint32_t SDIO_Response;  /* 指定SDIO响应类型。
                              该参数可以是 @ref SDIO_Response_Type 中的值 */

  uint32_t SDIO_Wait;      /* 指定是否使能SDIO等待中断请求。
                              该参数可以是 @ref SDIO_Wait_Interrupt_State 中的值 */

  uint32_t SDIO_CPSM;      /* 指定是否使能SDIO命令路径状态机（CPSM）。
                              该参数可以是 @ref SDIO_CPSM_State 中的值 */
} SDIO_CmdInitTypeDef;

/* SDIO 数据初始化结构体定义 */
typedef struct
{
  uint32_t SDIO_DataTimeOut;    /* 指定数据超时周期（以卡总线时钟周期为单位）。 */

  uint32_t SDIO_DataLength;     /* 指定要传输的数据字节数。 */
 
  uint32_t SDIO_DataBlockSize;  /* 指定块传输的数据块大小。
                                  该参数可以是 @ref SDIO_Data_Block_Size 中的值 */
 
  uint32_t SDIO_TransferDir;    /* 指定数据传输方向，是读取还是写入。
                                  该参数可以是 @ref SDIO_Transfer_Direction 中的值 */
 
  uint32_t SDIO_TransferMode;   /* 指定数据传输是流模式还是块模式。
                                  该参数可以是 @ref SDIO_Transfer_Type 中的值 */
 
  uint32_t SDIO_DPSM;           /* 指定是否使能SDIO数据路径状态机（DPSM）。
                                  该参数可以是 @ref SDIO_DPSM_State 中的值 */
} SDIO_DataInitTypeDef;

/* SDIO时钟边沿选择 */
#define SDIO_ClockEdge_Rising               ((uint32_t)0x00000000) /* 上升沿捕获 */
#define SDIO_ClockEdge_Falling              ((uint32_t)0x00002000) /* 下降沿捕获 */

/* SDIO时钟分频器旁路设置 */
#define SDIO_ClockBypass_Disable             ((uint32_t)0x00000000) /* 禁用时钟分频器旁路 */
#define SDIO_ClockBypass_Enable              ((uint32_t)0x00000400) /* 使能时钟分频器旁路 */

/* SDIO时钟省电模式设置 */
#define SDIO_ClockPowerSave_Disable         ((uint32_t)0x00000000) /* 禁用时钟省电模式，总线空闲时仍输出时钟 */
#define SDIO_ClockPowerSave_Enable          ((uint32_t)0x00000200) /* 使能时钟省电模式，总线空闲时停止输出时钟 */

/* SDIO总线宽度设置 */
#define SDIO_BusWide_1b                     ((uint32_t)0x00000000) /* 1位总线宽度 */
#define SDIO_BusWide_4b                     ((uint32_t)0x00000800) /* 4位总线宽度 */
#define SDIO_BusWide_8b                     ((uint32_t)0x00001000) /* 8位总线宽度 */

/* SDIO硬件流控制设置 */
#define SDIO_HardwareFlowControl_Disable    ((uint32_t)0x00000000) /* 禁用硬件流控制 */
#define SDIO_HardwareFlowControl_Enable     ((uint32_t)0x00004000) /* 使能硬件流控制 */

/* SDIO电源状态设置 */
#define SDIO_PowerState_OFF                 ((uint32_t)0x00000000) /* 关闭SDIO电源 */
#define SDIO_PowerState_ON                  ((uint32_t)0x00000003) /* 开启SDIO电源 */

/* SDIO中断源定义 */
#define SDIO_IT_CCRCFAIL                    ((uint32_t)0x00000001) /* 命令响应接收失败（CRC校验失败） */
#define SDIO_IT_DCRCFAIL                    ((uint32_t)0x00000002) /* 数据块接收失败（CRC校验失败） */
#define SDIO_IT_CTIMEOUT                    ((uint32_t)0x00000004) /* 命令响应超时 */
#define SDIO_IT_DTIMEOUT                    ((uint32_t)0x00000008) /* 数据超时 */
#define SDIO_IT_TXUNDERR                    ((uint32_t)0x00000010) /* 发送FIFO下溢错误 */
#define SDIO_IT_RXOVERR                     ((uint32_t)0x00000020) /* 接收FIFO上溢错误 */
#define SDIO_IT_CMDREND                     ((uint32_t)0x00000040) /* 命令响应已接收（CRC校验通过） */
#define SDIO_IT_CMDSENT                     ((uint32_t)0x00000080) /* 命令已发送（无响应要求） */
#define SDIO_IT_DATAEND                     ((uint32_t)0x00000100) /* 数据传输结束（DCOUNT = 0） */
#define SDIO_IT_STBITERR                    ((uint32_t)0x00000200) /* 起始位错误 */
#define SDIO_IT_DBCKEND                     ((uint32_t)0x00000400) /* 数据块发送/接收结束（CRC校验通过） */
#define SDIO_IT_CMDACT                      ((uint32_t)0x00000800) /* 命令传输进行中 */
#define SDIO_IT_TXACT                       ((uint32_t)0x00001000) /* 数据传输进行中（发送） */
#define SDIO_IT_RXACT                       ((uint32_t)0x00002000) /* 数据传输进行中（接收） */
#define SDIO_IT_TXFIFOHE                    ((uint32_t)0x00004000) /* 发送FIFO半空：至少有8个字可用空间 */
#define SDIO_IT_RXFIFOHF                    ((uint32_t)0x00008000) /* 接收FIFO半满：至少有8个字数据 */
#define SDIO_IT_TXFIFOF                     ((uint32_t)0x00010000) /* 发送FIFO满：少于4个字可用空间 */
#define SDIO_IT_RXFIFOF                     ((uint32_t)0x00020000) /* 接收FIFO满：少于4个字空闲空间 */
#define SDIO_IT_TXFIFOE                     ((uint32_t)0x00040000) /* 发送FIFO空：至少有一个字可用空间 */
#define SDIO_IT_RXFIFOE                     ((uint32_t)0x00080000) /* 接收FIFO空：至少有一个字数据 */
#define SDIO_IT_TXDAVL                      ((uint32_t)0x00100000) /* 发送FIFO中有数据可用 */
#define SDIO_IT_RXDAVL                      ((uint32_t)0x00200000) /* 接收FIFO中有数据可用 */
#define SDIO_IT_SDIOIT                      ((uint32_t)0x00400000) /* SDIO中断已接收 */
#define SDIO_IT_CEATAEND                    ((uint32_t)0x00800000) /* CE-ATA命令结束信号已接收 */

/* SDIO响应类型定义 */
#define SDIO_Response_No                    ((uint32_t)0x00000000) /* 无响应 */
#define SDIO_Response_Short                 ((uint32_t)0x00000040) /* 短响应（48位） */
#define SDIO_Response_Long                  ((uint32_t)0x000000C0) /* 长响应（136位） */

/* SDIO等待中断状态定义 */
#define SDIO_Wait_No                        ((uint32_t)0x00000000) /* 不等待中断 */
#define SDIO_Wait_IT                        ((uint32_t)0x00000100) /* 等待中断请求 */
#define SDIO_Wait_Pend                      ((uint32_t)0x00000200) /* 等待挂起请求 */

/* SDIO命令路径状态机（CPSM）状态定义 */
#define SDIO_CPSM_Disable                    ((uint32_t)0x00000000) /* 禁用CPSM */
#define SDIO_CPSM_Enable                     ((uint32_t)0x00000400) /* 使能CPSM */

/* SDIO响应寄存器地址偏移定义 */
#define SDIO_RESP1                          ((uint32_t)0x00000000) /* 响应寄存器1地址偏移 */
#define SDIO_RESP2                          ((uint32_t)0x00000004) /* 响应寄存器2地址偏移 */
#define SDIO_RESP3                          ((uint32_t)0x00000008) /* 响应寄存器3地址偏移 */
#define SDIO_RESP4                          ((uint32_t)0x0000000C) /* 响应寄存器4地址偏移 */

/* SDIO数据块大小定义 */
#define SDIO_DataBlockSize_1b               ((uint32_t)0x00000000) /* 1字节数据块 */
#define SDIO_DataBlockSize_2b               ((uint32_t)0x00000010) /* 2字节数据块 */
#define SDIO_DataBlockSize_4b               ((uint32_t)0x00000020) /* 4字节数据块 */
#define SDIO_DataBlockSize_8b               ((uint32_t)0x00000030) /* 8字节数据块 */
#define SDIO_DataBlockSize_16b              ((uint32_t)0x00000040) /* 16字节数据块 */
#define SDIO_DataBlockSize_32b              ((uint32_t)0x00000050) /* 32字节数据块 */
#define SDIO_DataBlockSize_64b              ((uint32_t)0x00000060) /* 64字节数据块 */
#define SDIO_DataBlockSize_128b             ((uint32_t)0x00000070) /* 128字节数据块 */
#define SDIO_DataBlockSize_256b             ((uint32_t)0x00000080) /* 256字节数据块 */
#define SDIO_DataBlockSize_512b             ((uint32_t)0x00000090) /* 512字节数据块 */
#define SDIO_DataBlockSize_1024b            ((uint32_t)0x000000A0) /* 1024字节数据块 */
#define SDIO_DataBlockSize_2048b            ((uint32_t)0x000000B0) /* 2048字节数据块 */
#define SDIO_DataBlockSize_4096b            ((uint32_t)0x000000C0) /* 4096字节数据块 */
#define SDIO_DataBlockSize_8192b            ((uint32_t)0x000000D0) /* 8192字节数据块 */
#define SDIO_DataBlockSize_16384b           ((uint32_t)0x000000E0) /* 16384字节数据块 */

/* SDIO数据传输方向定义 */
#define SDIO_TransferDir_ToCard             ((uint32_t)0x00000000) /* 数据传输到卡（写入） */
#define SDIO_TransferDir_ToSDIO             ((uint32_t)0x00000002) /* 数据传输到SDIO（读取） */

/* SDIO传输类型定义 */
#define SDIO_TransferMode_Block             ((uint32_t)0x00000000) /* 块传输模式 */
#define SDIO_TransferMode_Stream            ((uint32_t)0x00000004) /* 流传输模式 */

/* SDIO数据路径状态机（DPSM）状态定义 */
#define SDIO_DPSM_Disable                    ((uint32_t)0x00000000) /* 禁用DPSM */
#define SDIO_DPSM_Enable                     ((uint32_t)0x00000001) /* 使能DPSM */

/* SDIO状态标志定义 */
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000001) /* 命令响应接收失败（CRC校验失败）标志 */
#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000002) /* 数据块接收失败（CRC校验失败）标志 */
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000004) /* 命令响应超时标志 */
#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000008) /* 数据超时标志 */
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000010) /* 发送FIFO下溢错误标志 */
#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000020) /* 接收FIFO上溢错误标志 */
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000040) /* 命令响应已接收（CRC校验通过）标志 */
#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000080) /* 命令已发送（无响应要求）标志 */
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000100) /* 数据传输结束（DCOUNT = 0）标志 */
#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000200) /* 起始位错误标志 */
#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000400) /* 数据块发送/接收结束（CRC校验通过）标志 */
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000800) /* 命令传输进行中标志 */
#define SDIO_FLAG_TXACT                     ((uint32_t)0x00001000) /* 数据传输进行中（发送）标志 */
#define SDIO_FLAG_RXACT                     ((uint32_t)0x00002000) /* 数据传输进行中（接收）标志 */
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00004000) /* 发送FIFO半空标志 */
#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00008000) /* 接收FIFO半满标志 */
#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00010000) /* 发送FIFO满标志 */
#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00020000) /* 接收FIFO满标志 */
#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00040000) /* 发送FIFO空标志 */
#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00080000) /* 接收FIFO空标志 */
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00100000) /* 发送FIFO数据可用标志 */
#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00200000) /* 接收FIFO数据可用标志 */
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00400000) /* SDIO中断标志 */
#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00800000) /* CE-ATA命令结束标志 */

/* SDIO读取等待模式定义 */
#define SDIO_ReadWaitMode_CLK               ((uint32_t)0x00000001) /* 使用SDIO时钟模式进行读取等待 */
#define SDIO_ReadWaitMode_DATA2             ((uint32_t)0x00000000) /* 使用DATA2线模式进行读取等待 */

/* SDIO数据控制寄存器位定义 */
#define SDIO_DataControl_DTEN               ((uint32_t)0x00000001) /* 数据使能位 */
#define SDIO_DataControl_DTDIR              ((uint32_t)0x00000002) /* 数据传输方向位 */
#define SDIO_DataControl_DTMODE             ((uint32_t)0x00000004) /* 数据传输模式位 */
#define SDIO_DataControl_DMAEN              ((uint32_t)0x00000008) /* DMA使能位 */
#define SDIO_DataControl_DBLOCKSIZE         ((uint32_t)0x000000F0) /* 数据块大小位段 */
#define SDIO_DataControl_RWSTART            ((uint32_t)0x00000100) /* 读取等待开始位 */
#define SDIO_DataControl_RWSTOP             ((uint32_t)0x00000200) /* 读取等待停止位 */
#define SDIO_DataControl_RWMOD              ((uint32_t)0x00000400) /* 读取等待模式位 */
#define SDIO_DataControl_SDIOEN             ((uint32_t)0x00000800) /* SDIO使能位 */


void SDIO_DeInit(void);
void SDIO_Init(SDIO_InitTypeDef* SDIO_InitStruct);
void SDIO_StructInit(SDIO_InitTypeDef* SDIO_InitStruct);
void SDIO_ClockCmd(FunctionalState NewState);
void SDIO_SetPowerState(uint32_t SDIO_PowerState);
uint32_t SDIO_GetPowerState(void);
void SDIO_ITConfig(uint32_t SDIO_IT, FunctionalState NewState);
void SDIO_DMACmd(FunctionalState NewState);
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef* SDIO_CmdInitStruct);
uint8_t SDIO_GetCommandResponse(void);
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP);
void SDIO_DataConfig(SDIO_DataInitTypeDef* SDIO_DataInitStruct);
void SDIO_DataStructInit(SDIO_DataInitTypeDef* SDIO_DataInitStruct);
uint32_t SDIO_GetDataCounter(void);
uint32_t SDIO_ReadData(void);
void SDIO_WriteData(uint32_t Data);
uint32_t SDIO_GetFIFOCount(void);
void SDIO_StartSDIOReadWait(FunctionalState NewState);
void SDIO_StopSDIOReadWait(FunctionalState NewState);
void SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode);
void SDIO_SetSDIOOperation(FunctionalState NewState);
void SDIO_SendSDIOSuspendCmd(FunctionalState NewState);
void SDIO_CommandCompletionCmd(FunctionalState NewState);
void SDIO_CEATAITCmd(FunctionalState NewState);
void SDIO_SendCEATACmd(FunctionalState NewState);
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG);
void SDIO_ClearFlag(uint32_t SDIO_FLAG);
ITStatus SDIO_GetITStatus(uint32_t SDIO_IT);
void SDIO_ClearITPendingBit(uint32_t SDIO_IT);

#ifdef __cplusplus
}
#endif

#endif