/********************************** (C) COPYRIGHT  *******************************
* 文件名             : ch32v30x_can.h
* 作者               : WCH
* 版本               : V1.0.0
* 日期               : 2021/06/06
* 描述               : 此文件包含了CAN固件库的所有函数原型。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：此软件（无论是否修改）和二进制文件用于
*       南京沁恒微电子股份有限公司生产的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_CAN_H
#define __CH32V30x_CAN_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* CAN 初始化结构体定义 */
typedef struct
{
  uint16_t CAN_Prescaler;   /* 指定时间量子的长度。
                               范围为1到1024。*/
  
  uint8_t CAN_Mode;         /* 指定CAN操作模式。
                               此参数可以是 @ref CAN_operating_mode 的值 */

  uint8_t CAN_SJW;          /* 指定为了执行重新同步，CAN硬件允许
                               延长或缩短一个位的最大时间量子数。
                               此参数可以是 @ref CAN_synchronisation_jump_width 的值 */

  uint8_t CAN_BS1;          /* 指定位段1中的时间量子数。
                               此参数可以是 @ref CAN_time_quantum_in_bit_segment_1 的值 */

  uint8_t CAN_BS2;          /* 指定位段2中的时间量子数。
                               此参数可以是 @ref CAN_time_quantum_in_bit_segment_2 的值 */
  
  FunctionalState CAN_TTCM; /* 使能或禁用时间触发通信模式。
                               此参数可以设置为 ENABLE 或 DISABLE。*/
  
  FunctionalState CAN_ABOM;  /* 使能或禁用自动离线管理。
                               此参数可以设置为 ENABLE 或 DISABLE。*/

  FunctionalState CAN_AWUM;  /* 使能或禁用自动唤醒模式。
                               此参数可以设置为 ENABLE 或 DISABLE。*/

  FunctionalState CAN_NART;  /* 使能或禁用非自动重传模式。
                               此参数可以设置为 ENABLE 或 DISABLE。*/

  FunctionalState CAN_RFLM;  /* 使能或禁用接收FIFO锁定模式。
                               此参数可以设置为 ENABLE 或 DISABLE。*/

  FunctionalState CAN_TXFP;  /* 使能或禁用发送FIFO优先级。
                               此参数可以设置为 ENABLE 或 DISABLE。*/
} CAN_InitTypeDef;

/* CAN 过滤器初始化结构体定义 */
typedef struct
{
  uint16_t CAN_FilterIdHigh;         /* 指定过滤器标识号的高16位（对于32位配置）或第一个标识号（对于16位配置）。
                                           此参数可以是0x0000到0xFFFF之间的值。*/

  uint16_t CAN_FilterIdLow;          /* 指定过滤器标识号的低16位（对于32位配置）或第二个标识号（对于16位配置）。
                                           此参数可以是0x0000到0xFFFF之间的值。*/

  uint16_t CAN_FilterMaskIdHigh;     /* 根据模式指定过滤器掩码号或标识号的高16位（对于32位配置）
                                           或第一个掩码/标识号（对于16位配置）。
                                           此参数可以是0x0000到0xFFFF之间的值。*/

  uint16_t CAN_FilterMaskIdLow;      /* 根据模式指定过滤器掩码号或标识号的低16位（对于32位配置）
                                           或第二个掩码/标识号（对于16位配置）。
                                           此参数可以是0x0000到0xFFFF之间的值。*/

  uint16_t CAN_FilterFIFOAssignment; /* 指定将分配给过滤器的FIFO（0或1）。
                                           此参数可以是 @ref CAN_filter_FIFO 的值。*/
  
  uint8_t CAN_FilterNumber;          /* 指定要初始化的过滤器。范围为0到13。*/

  uint8_t CAN_FilterMode;            /* 指定要初始化的过滤器模式。
                                           此参数可以是 @ref CAN_filter_mode 的值。*/

  uint8_t CAN_FilterScale;           /* 指定过滤器尺度。
                                           此参数可以是 @ref CAN_filter_scale 的值。*/

  FunctionalState CAN_FilterActivation; /* 使能或禁用过滤器。
                                           此参数可以设置为 ENABLE 或 DISABLE。*/
} CAN_FilterInitTypeDef;

/* CAN 发送消息结构体定义 */
typedef struct
{
  uint32_t StdId;  /* 指定标准标识符。
                     此参数可以是0到0x7FF之间的值。*/

  uint32_t ExtId;  /* 指定扩展标识符。
                     此参数可以是0到0x1FFFFFFF之间的值。*/

  uint8_t IDE;     /* 指定要发送消息的标识符类型。
                     此参数可以是 @ref CAN_identifier_type 的值。*/

  uint8_t RTR;     /* 指定要发送消息的帧类型。
                     此参数可以是 @ref CAN_remote_transmission_request 的值。*/

  uint8_t DLC;     /* 指定要发送的帧长度。
                     此参数可以是0到8之间的值。*/

  uint8_t Data[8]; /* 包含要发送的数据。范围为0到0xFF。*/
} CanTxMsg;

/* CAN 接收消息结构体定义 */
typedef struct
{
  uint32_t StdId;  /* 指定标准标识符。
                     此参数可以是0到0x7FF之间的值。*/

  uint32_t ExtId;  /* 指定扩展标识符。
                     此参数可以是0到0x1FFFFFFF之间的值。*/

  uint8_t IDE;     /* 指定要接收消息的标识符类型。
                     此参数可以是 @ref CAN_identifier_type 的值。*/

  uint8_t RTR;     /* 指定接收消息的帧类型。
                     此参数可以是 @ref CAN_remote_transmission_request 的值。*/

  uint8_t DLC;     /* 指定要接收的帧长度。
                     此参数可以是0到8之间的值。*/

  uint8_t Data[8]; /* 包含要接收的数据。范围为0到0xFF。*/

  uint8_t FMI;     /* 指定存储在邮箱中的消息通过的过滤器索引。
                     此参数可以是0到0xFF之间的值。*/
} CanRxMsg;

/* CAN 初始化状态常量 */
#define CAN_InitStatus_Failed              ((uint8_t)0x00) /* CAN 初始化失败 */
#define CAN_InitStatus_Success             ((uint8_t)0x01) /* CAN 初始化成功 */

/* CAN 模式常量 */
#define CAN_Mode_Normal                    ((uint8_t)0x00)  /* 正常模式 */
#define CAN_Mode_LoopBack                  ((uint8_t)0x01)  /* 环回模式 */
#define CAN_Mode_Silent                    ((uint8_t)0x02)  /* 静默模式 */
#define CAN_Mode_Silent_LoopBack           ((uint8_t)0x03)  /* 静默环回模式 */

/* CAN 操作模式常量 */
#define CAN_OperatingMode_Initialization   ((uint8_t)0x00) /* 初始化模式 */
#define CAN_OperatingMode_Normal           ((uint8_t)0x01) /* 正常模式 */
#define CAN_OperatingMode_Sleep            ((uint8_t)0x02) /* 睡眠模式 */

/* CAN 模式状态常量 */
#define CAN_ModeStatus_Failed              ((uint8_t)0x00)                /* CAN 进入指定模式失败 */
#define CAN_ModeStatus_Success             ((uint8_t)!CAN_ModeStatus_Failed)   /* CAN 进入指定模式成功 */

/* CAN 同步跳转宽度常量 */
#define CAN_SJW_1tq                        ((uint8_t)0x00)  /* 1个时间量子 */
#define CAN_SJW_2tq                        ((uint8_t)0x01)  /* 2个时间量子 */
#define CAN_SJW_3tq                        ((uint8_t)0x02)  /* 3个时间量子 */
#define CAN_SJW_4tq                        ((uint8_t)0x03)  /* 4个时间量子 */

/* CAN 位段1时间量子数常量 */
#define CAN_BS1_1tq                        ((uint8_t)0x00)  /* 1个时间量子 */
#define CAN_BS1_2tq                        ((uint8_t)0x01)  /* 2个时间量子 */
#define CAN_BS1_3tq                        ((uint8_t)0x02)  /* 3个时间量子 */
#define CAN_BS1_4tq                        ((uint8_t)0x03)  /* 4个时间量子 */
#define CAN_BS1_5tq                        ((uint8_t)0x04)  /* 5个时间量子 */
#define CAN_BS1_6tq                        ((uint8_t)0x05)  /* 6个时间量子 */
#define CAN_BS1_7tq                        ((uint8_t)0x06)  /* 7个时间量子 */
#define CAN_BS1_8tq                        ((uint8_t)0x07)  /* 8个时间量子 */
#define CAN_BS1_9tq                        ((uint8_t)0x08)  /* 9个时间量子 */
#define CAN_BS1_10tq                       ((uint8_t)0x09)  /* 10个时间量子 */
#define CAN_BS1_11tq                       ((uint8_t)0x0A)  /* 11个时间量子 */
#define CAN_BS1_12tq                       ((uint8_t)0x0B)  /* 12个时间量子 */
#define CAN_BS1_13tq                       ((uint8_t)0x0C)  /* 13个时间量子 */
#define CAN_BS1_14tq                       ((uint8_t)0x0D)  /* 14个时间量子 */
#define CAN_BS1_15tq                       ((uint8_t)0x0E)  /* 15个时间量子 */
#define CAN_BS1_16tq                       ((uint8_t)0x0F)  /* 16个时间量子 */

/* CAN 位段2时间量子数常量 */
#define CAN_BS2_1tq                        ((uint8_t)0x00)  /* 1个时间量子 */
#define CAN_BS2_2tq                        ((uint8_t)0x01)  /* 2个时间量子 */
#define CAN_BS2_3tq                        ((uint8_t)0x02)  /* 3个时间量子 */
#define CAN_BS2_4tq                        ((uint8_t)0x03)  /* 4个时间量子 */
#define CAN_BS2_5tq                        ((uint8_t)0x04)  /* 5个时间量子 */
#define CAN_BS2_6tq                        ((uint8_t)0x05)  /* 6个时间量子 */
#define CAN_BS2_7tq                        ((uint8_t)0x06)  /* 7个时间量子 */
#define CAN_BS2_8tq                        ((uint8_t)0x07)  /* 8个时间量子 */

/* CAN 过滤器模式常量 */
#define CAN_FilterMode_IdMask              ((uint8_t)0x00)  /* 标识符/掩码模式 */
#define CAN_FilterMode_IdList              ((uint8_t)0x01)  /* 标识符列表模式 */

/* CAN 过滤器尺度常量 */
#define CAN_FilterScale_16bit              ((uint8_t)0x00) /* 两个16位过滤器 */
#define CAN_FilterScale_32bit              ((uint8_t)0x01) /* 一个32位过滤器 */

/* CAN 过滤器FIFO分配常量 */
#define CAN_Filter_FIFO0                   ((uint8_t)0x00)  /* 过滤器x分配给FIFO 0 */
#define CAN_Filter_FIFO1                   ((uint8_t)0x01)  /* 过滤器x分配给FIFO 1 */

/* CAN 标识符类型常量 */
#define CAN_Id_Standard                    ((uint32_t)0x00000000)  /* 标准标识符 */
#define CAN_Id_Extended                    ((uint32_t)0x00000004)  /* 扩展标识符 */

/* CAN 远程传输请求常量 */
#define CAN_RTR_Data                       ((uint32_t)0x00000000)  /* 数据帧 */
#define CAN_RTR_Remote                     ((uint32_t)0x00000002)  /* 远程帧 */

/* CAN 发送状态常量 */
#define CAN_TxStatus_Failed                ((uint8_t)0x00) /* CAN 发送失败 */
#define CAN_TxStatus_Ok                    ((uint8_t)0x01) /* CAN 发送成功 */
#define CAN_TxStatus_Pending               ((uint8_t)0x02) /* CAN 发送挂起 */
#define CAN_TxStatus_NoMailBox             ((uint8_t)0x04) /* CAN 单元没有提供空邮箱 */

/* CAN 接收FIFO编号常量 */
#define CAN_FIFO0                          ((uint8_t)0x00) /* 使用CAN FIFO 0接收 */
#define CAN_FIFO1                          ((uint8_t)0x01) /* 使用CAN FIFO 1接收 */

/* CAN 睡眠状态常量 */
#define CAN_Sleep_Failed                   ((uint8_t)0x00) /* CAN 未能进入睡眠模式 */
#define CAN_Sleep_Ok                       ((uint8_t)0x01) /* CAN 进入睡眠模式 */

/* CAN 唤醒状态常量 */
#define CAN_WakeUp_Failed                  ((uint8_t)0x00) /* CAN 未能离开睡眠模式 */
#define CAN_WakeUp_Ok                      ((uint8_t)0x01) /* CAN 离开睡眠模式 */
 
/* CAN 错误码常量 */                                                               
#define CAN_ErrorCode_NoErr                ((uint8_t)0x00) /* 无错误 */ 
#define	CAN_ErrorCode_StuffErr             ((uint8_t)0x10) /* 位填充错误 */ 
#define	CAN_ErrorCode_FormErr              ((uint8_t)0x20) /* 格式错误 */ 
#define	CAN_ErrorCode_ACKErr               ((uint8_t)0x30) /* 应答错误 */ 
#define	CAN_ErrorCode_BitRecessiveErr      ((uint8_t)0x40) /* 隐性位错误 */ 
#define	CAN_ErrorCode_BitDominantErr       ((uint8_t)0x50) /* 显性位错误 */ 
#define	CAN_ErrorCode_CRCErr               ((uint8_t)0x60) /* CRC错误 */ 
#define	CAN_ErrorCode_SoftwareSetErr       ((uint8_t)0x70) /* 软件设置错误 */ 

/* CAN 标志位 */
/* 如果标志是0x3XXXXXXX，意味着它可以与CAN_GetFlagStatus()和CAN_ClearFlag()函数一起使用。
 * 如果标志是0x1XXXXXXX，意味着它只能与CAN_GetFlagStatus()函数一起使用。
*/
/* 发送标志 */
#define CAN_FLAG_RQCP0                     ((uint32_t)0x38000001) /* 邮箱0请求完成标志 */
#define CAN_FLAG_RQCP1                     ((uint32_t)0x38000100) /* 邮箱1请求完成标志 */
#define CAN_FLAG_RQCP2                     ((uint32_t)0x38010000) /* 邮箱2请求完成标志 */

/* 接收标志 */ 
#define CAN_FLAG_FMP0                      ((uint32_t)0x12000003) /* FIFO 0 消息挂起标志 */
#define CAN_FLAG_FF0                       ((uint32_t)0x32000008) /* FIFO 0 满标志 */
#define CAN_FLAG_FOV0                      ((uint32_t)0x32000010) /* FIFO 0 溢出标志 */
#define CAN_FLAG_FMP1                      ((uint32_t)0x14000003) /* FIFO 1 消息挂起标志 */
#define CAN_FLAG_FF1                       ((uint32_t)0x34000008) /* FIFO 1 满标志 */
#define CAN_FLAG_FOV1                      ((uint32_t)0x34000010) /* FIFO 1 溢出标志 */

/* 操作模式标志 */
#define CAN_FLAG_WKU                       ((uint32_t)0x31000008) /* 唤醒标志 */
#define CAN_FLAG_SLAK                      ((uint32_t)0x31000012) /* 睡眠应答标志 */
/* 注意：
 * 当SLAK中断被禁用时（SLKIE=0），无法轮询SLAKI。
 * 在这种情况下，可以轮询SLAK位。
*/

/* 错误标志 */
#define CAN_FLAG_EWG                       ((uint32_t)0x10F00001) /* 错误警告标志 */
#define CAN_FLAG_EPV                       ((uint32_t)0x10F00002) /* 错误被动标志 */
#define CAN_FLAG_BOF                       ((uint32_t)0x10F00004) /* 离线标志 */
#define CAN_FLAG_LEC                       ((uint32_t)0x30F00070) /* 最后错误码标志 */

/* CAN 中断源 */
#define CAN_IT_TME                         ((uint32_t)0x00000001) /* 发送邮箱空中断 */

/* 接收中断 */
#define CAN_IT_FMP0                        ((uint32_t)0x00000002) /* FIFO 0 消息挂起中断 */
#define CAN_IT_FF0                         ((uint32_t)0x00000004) /* FIFO 0 满中断 */
#define CAN_IT_FOV0                        ((uint32_t)0x00000008) /* FIFO 0 溢出中断 */
#define CAN_IT_FMP1                        ((uint32_t)0x00000010) /* FIFO 1 消息挂起中断 */
#define CAN_IT_FF1                         ((uint32_t)0x00000020) /* FIFO 1 满中断 */
#define CAN_IT_FOV1                        ((uint32_t)0x00000040) /* FIFO 1 溢出中断 */

/* 操作模式中断 */
#define CAN_IT_WKU                         ((uint32_t)0x00010000) /* 唤醒中断 */
#define CAN_IT_SLK                         ((uint32_t)0x00020000) /* 睡眠应答中断 */

/* 错误中断 */
#define CAN_IT_EWG                         ((uint32_t)0x00000100) /* 错误警告中断 */
#define CAN_IT_EPV                         ((uint32_t)0x00000200) /* 错误被动中断 */
#define CAN_IT_BOF                         ((uint32_t)0x00000400) /* 离线中断 */
#define CAN_IT_LEC                         ((uint32_t)0x00000800) /* 最后错误码中断 */
#define CAN_IT_ERR                         ((uint32_t)0x00008000) /* 错误中断 */

/* 以中断命名的标志：仅用于固件兼容性 */
#define CAN_IT_RQCP0    CAN_IT_TME
#define CAN_IT_RQCP1    CAN_IT_TME
#define CAN_IT_RQCP2    CAN_IT_TME

/* CAN 旧定义（兼容性） */
#define CANINITFAILED               CAN_InitStatus_Failed
#define CANINITOK                   CAN_InitStatus_Success
#define CAN_FilterFIFO0             CAN_Filter_FIFO0
#define CAN_FilterFIFO1             CAN_Filter_FIFO1
#define CAN_ID_STD                  CAN_Id_Standard           
#define CAN_ID_EXT                  CAN_Id_Extended
#define CAN_RTR_DATA                CAN_RTR_Data         
#define CAN_RTR_REMOTE              CAN_RTR_Remote
#define CANTXFAILE                  CAN_TxStatus_Failed
#define CANTXOK                     CAN_TxStatus_Ok
#define CANTXPENDING                CAN_TxStatus_Pending
#define CAN_NO_MB                   CAN_TxStatus_NoMailBox
#define CANSLEEPFAILED              CAN_Sleep_Failed
#define CANSLEEPOK                  CAN_Sleep_Ok
#define CANWAKEUPFAILED             CAN_WakeUp_Failed        
#define CANWAKEUPOK                 CAN_WakeUp_Ok        

/* 函数声明 */
void CAN_DeInit(CAN_TypeDef* CANx);                                         /* 将CAN外设寄存器恢复为默认值 */
uint8_t CAN_Init(CAN_TypeDef* CANx, CAN_InitTypeDef* CAN_InitStruct);       /* 根据指定参数初始化CAN */
void CAN_FilterInit(CAN_FilterInitTypeDef* CAN_FilterInitStruct);           /* 根据指定参数初始化CAN过滤器 */
void CAN_StructInit(CAN_InitTypeDef* CAN_InitStruct);                       /* 使用默认值填充CAN_InitStruct结构体 */
void CAN_SlaveStartBank(uint8_t CAN_BankNumber);                            /* 选择从CAN过滤器组的起始编号 */
void CAN_DBGFreeze(CAN_TypeDef* CANx, FunctionalState NewState);            /* 在调试模式下冻结/解冻CAN */
void CAN_TTComModeCmd(CAN_TypeDef* CANx, FunctionalState NewState);         /* 使能或禁用时间触发通信模式 */
uint8_t CAN_Transmit(CAN_TypeDef* CANx, CanTxMsg* TxMessage);               /* 启动消息传输 */
uint8_t CAN_TransmitStatus(CAN_TypeDef* CANx, uint8_t TransmitMailbox);     /* 检查发送邮箱的传输状态 */
void CAN_CancelTransmit(CAN_TypeDef* CANx, uint8_t Mailbox);                /* 取消挂起的消息传输 */
void CAN_Receive(CAN_TypeDef* CANx, uint8_t FIFONumber, CanRxMsg* RxMessage); /* 接收消息 */
void CAN_FIFORelease(CAN_TypeDef* CANx, uint8_t FIFONumber);                /* 释放FIFO */
uint8_t CAN_MessagePending(CAN_TypeDef* CANx, uint8_t FIFONumber);          /* 返回指定FIFO中待处理消息的数量 */
uint8_t CAN_OperatingModeRequest(CAN_TypeDef* CANx, uint8_t CAN_OperatingMode); /* 选择CAN操作模式 */
uint8_t CAN_Sleep(CAN_TypeDef* CANx);                                       /* 使CAN进入睡眠模式 */
uint8_t CAN_WakeUp(CAN_TypeDef* CANx);                                      /* 唤醒CAN */
uint8_t CAN_GetLastErrorCode(CAN_TypeDef* CANx);                            /* 返回CAN控制器的最后错误码 */
uint8_t CAN_GetReceiveErrorCounter(CAN_TypeDef* CANx);                      /* 返回CAN接收错误计数器 */
uint8_t CAN_GetLSBTransmitErrorCounter(CAN_TypeDef* CANx);                  /* 返回CAN发送错误计数器的低8位 */
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState); /* 使能或禁用指定的CAN中断 */
FlagStatus CAN_GetFlagStatus(CAN_TypeDef* CANx, uint32_t CAN_FLAG);         /* 检查指定的CAN标志位是否置位 */
void CAN_ClearFlag(CAN_TypeDef* CANx, uint32_t CAN_FLAG);                   /* 清除指定的CAN标志位 */
ITStatus CAN_GetITStatus(CAN_TypeDef* CANx, uint32_t CAN_IT);               /* 检查指定的CAN中断是否发生 */
void CAN_ClearITPendingBit(CAN_TypeDef* CANx, uint32_t CAN_IT);             /* 清除指定的CAN中断挂起位 */

#ifdef __cplusplus
}
#endif

#endif