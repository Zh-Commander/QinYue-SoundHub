/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_i2c.h
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2021/06/06
* 描述            : 该文件包含了I2C固件库的所有函数原型。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或未修改）及二进制文件仅用于
*       南京沁恒微电子制造的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_I2C_H
#define __CH32V30x_I2C_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* I2C 初始化结构体定义 */
typedef struct
{
  uint32_t I2C_ClockSpeed;          /* 指定时钟频率。
                                       此参数必须设置为低于400kHz的值 */

  uint16_t I2C_Mode;                /* 指定I2C模式。
                                       此参数可以是@ref I2C_mode 的值 */

  uint16_t I2C_DutyCycle;           /* 指定I2C快速模式占空比。
                                       此参数可以是@ref I2C_duty_cycle_in_fast_mode 的值 */

  uint16_t I2C_OwnAddress1;         /* 指定第一个设备自身地址。
                                       此参数可以是7位或10位地址。 */

  uint16_t I2C_Ack;                 /* 使能或禁用应答。
                                       此参数可以是@ref I2C_acknowledgement 的值 */

  uint16_t I2C_AcknowledgedAddress; /* 指定是7位还是10位地址被应答。
                                       此参数可以是@ref I2C_acknowledged_address 的值 */
}I2C_InitTypeDef;

/* I2C_mode - I2C工作模式定义 */
#define I2C_Mode_I2C                    ((uint16_t)0x0000)  /* I2C标准模式 */
#define I2C_Mode_SMBusDevice            ((uint16_t)0x0002)  /* SMBus设备模式 */  
#define I2C_Mode_SMBusHost              ((uint16_t)0x000A)  /* SMBus主机模式 */

/* I2C_duty_cycle_in_fast_mode - 快速模式占空比定义 */
#define I2C_DutyCycle_16_9              ((uint16_t)0x4000) /* I2C快速模式 Tlow/Thigh = 16/9 */
#define I2C_DutyCycle_2                 ((uint16_t)0xBFFF) /* I2C快速模式 Tlow/Thigh = 2 */

/* I2C_acknowledgement - 应答控制定义 */
#define I2C_Ack_Enable                  ((uint16_t)0x0400)  /* 使能应答 */
#define I2C_Ack_Disable                 ((uint16_t)0x0000)  /* 禁用应答 */

/* I2C_transfer_direction - 数据传输方向定义 */
#define I2C_Direction_Transmitter       ((uint8_t)0x00)     /* 发送器模式 */
#define I2C_Direction_Receiver          ((uint8_t)0x01)     /* 接收器模式 */

/* I2C_acknowledged_address - 应答地址模式定义 */
#define I2C_AcknowledgedAddress_7bit    ((uint16_t)0x4000)  /* 7位地址模式 */
#define I2C_AcknowledgedAddress_10bit   ((uint16_t)0xC000)  /* 10位地址模式 */

/* I2C_registers - I2C寄存器偏移地址定义 */
#define I2C_Register_CTLR1              ((uint8_t)0x00)     /* 控制寄存器1 */
#define I2C_Register_CTLR2              ((uint8_t)0x04)     /* 控制寄存器2 */
#define I2C_Register_OADDR1             ((uint8_t)0x08)     /* 自身地址寄存器1 */
#define I2C_Register_OADDR2             ((uint8_t)0x0C)     /* 自身地址寄存器2 */
#define I2C_Register_DATAR              ((uint8_t)0x10)     /* 数据寄存器 */
#define I2C_Register_STAR1              ((uint8_t)0x14)     /* 状态寄存器1 */
#define I2C_Register_STAR2              ((uint8_t)0x18)     /* 状态寄存器2 */
#define I2C_Register_CKCFGR             ((uint8_t)0x1C)     /* 时钟控制寄存器 */
#define I2C_Register_RTR                ((uint8_t)0x20)     /* 上升时间寄存器 */

/* I2C_SMBus_alert_pin_level - SMBus警报引脚电平定义 */
#define I2C_SMBusAlert_Low              ((uint16_t)0x2000)  /* 低电平警报 */
#define I2C_SMBusAlert_High             ((uint16_t)0xDFFF)  /* 高电平警报 */

/* I2C_PEC_position - PEC位置定义 */
#define I2C_PECPosition_Next            ((uint16_t)0x0800)  /* PEC在下一个字节 */
#define I2C_PECPosition_Current         ((uint16_t)0xF7FF)  /* PEC在当前字节 */

/* I2C_NACK_position - NACK位置定义 */
#define I2C_NACKPosition_Next           ((uint16_t)0x0800)  /* NACK在下一个字节 */
#define I2C_NACKPosition_Current        ((uint16_t)0xF7FF)  /* NACK在当前字节 */

/* I2C_interrupts_definition - I2C中断定义 (用于ITConfig函数) */
#define I2C_IT_BUF                      ((uint16_t)0x0400)  /* 缓冲区中断 */
#define I2C_IT_EVT                      ((uint16_t)0x0200)  /* 事件中断 */
#define I2C_IT_ERR                      ((uint16_t)0x0100)  /* 错误中断 */

/* I2C_interrupts_definition - I2C具体中断源定义 */
#define I2C_IT_SMBALERT                 ((uint32_t)0x01008000)  /* SMBus警报中断 */
#define I2C_IT_TIMEOUT                  ((uint32_t)0x01004000)  /* 超时中断 */
#define I2C_IT_PECERR                   ((uint32_t)0x01001000)  /* PEC错误中断 */
#define I2C_IT_OVR                      ((uint32_t)0x01000800)  /* 溢出错误中断 */
#define I2C_IT_AF                       ((uint32_t)0x01000400)  /* 应答失败中断 */
#define I2C_IT_ARLO                     ((uint32_t)0x01000200)  /* 仲裁丢失中断 */
#define I2C_IT_BERR                     ((uint32_t)0x01000100)  /* 总线错误中断 */
#define I2C_IT_TXE                      ((uint32_t)0x06000080)  /* 数据寄存器空中断 */
#define I2C_IT_RXNE                     ((uint32_t)0x06000040)  /* 数据寄存器非空中断 */
#define I2C_IT_STOPF                    ((uint32_t)0x02000010)  /* 停止位检测中断 */
#define I2C_IT_ADD10                    ((uint32_t)0x02000008)  /* 10位地址头发送完成中断 */
#define I2C_IT_BTF                      ((uint32_t)0x02000004)  /* 字节传输完成中断 */
#define I2C_IT_ADDR                     ((uint32_t)0x02000002)  /* 地址匹配中断 */
#define I2C_IT_SB                       ((uint32_t)0x02000001)  /* 起始位中断 */

/* SR2 register flags - 状态寄存器2标志位定义 */
#define I2C_FLAG_DUALF                  ((uint32_t)0x00800000)  /* 双地址标志位 */
#define I2C_FLAG_SMBHOST                ((uint32_t)0x00400000)  /* SMBus主机标志位 */
#define I2C_FLAG_SMBDEFAULT             ((uint32_t)0x00200000)  /* SMBus默认地址标志位 */
#define I2C_FLAG_GENCALL                ((uint32_t)0x00100000)  /* 广播呼叫标志位 */
#define I2C_FLAG_TRA                    ((uint32_t)0x00040000)  /* 发送/接收标志位 */
#define I2C_FLAG_BUSY                   ((uint32_t)0x00020000)  /* 总线忙标志位 */
#define I2C_FLAG_MSL                    ((uint32_t)0x00010000)  /* 主从模式标志位 */

/* SR1 register flags - 状态寄存器1标志位定义 */
#define I2C_FLAG_SMBALERT               ((uint32_t)0x10008000)  /* SMBus警报标志位 */
#define I2C_FLAG_TIMEOUT                ((uint32_t)0x10004000)  /* 超时标志位 */
#define I2C_FLAG_PECERR                 ((uint32_t)0x10001000)  /* PEC错误标志位 */
#define I2C_FLAG_OVR                    ((uint32_t)0x10000800)  /* 溢出错误标志位 */
#define I2C_FLAG_AF                     ((uint32_t)0x10000400)  /* 应答失败标志位 */
#define I2C_FLAG_ARLO                   ((uint32_t)0x10000200)  /* 仲裁丢失标志位 */
#define I2C_FLAG_BERR                   ((uint32_t)0x10000100)  /* 总线错误标志位 */
#define I2C_FLAG_TXE                    ((uint32_t)0x10000080)  /* 数据寄存器空标志位 */
#define I2C_FLAG_RXNE                   ((uint32_t)0x10000040)  /* 数据寄存器非空标志位 */
#define I2C_FLAG_STOPF                  ((uint32_t)0x10000010)  /* 停止位检测标志位 */
#define I2C_FLAG_ADD10                  ((uint32_t)0x10000008)  /* 10位地址头发送完成标志位 */
#define I2C_FLAG_BTF                    ((uint32_t)0x10000004)  /* 字节传输完成标志位 */
#define I2C_FLAG_ADDR                   ((uint32_t)0x10000002)  /* 地址匹配标志位 */
#define I2C_FLAG_SB                     ((uint32_t)0x10000001)  /* 起始位标志位 */


/****************I2C主模式事件（按通信顺序分组的事件）********************/

/******************************************************************************************************************** 
  * @brief  开始通信
  * 
  * 主机使用I2C_GenerateSTART()函数发送START条件后，
  * 主机必须等待事件5（START条件已在I2C总线上正确释放）。
  * 
  */
/* EVT5 - 事件5：主模式选择 */
#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY、MSL和SB标志位 */

/********************************************************************************************************************
  * @brief  地址应答
  * 
  * 当START条件在总线上正确释放后（检查EVT5），
  * 主机使用I2C_Send7bitAddress()函数发送要通信的从机地址，
  * 同时确定主机作为发送器或接收器。然后主机必须等待从机应答其地址。
  * 如果总线上发送了应答，将设置以下事件之一：
  * 
  *  1) 对于主接收器模式（7位寻址）：设置I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED事件。
  *  
  *  2) 对于主发送器模式（7位寻址）：设置I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED事件。
  *  
  *  3) 对于10位寻址模式，主机（生成START并检查EVT5后）使用I2C_SendData()函数发送10位寻址模式头。
  *     然后主机等待EVT9。EVT9表示10位寻址头已在总线上正确发送。
  *     然后主机应使用I2C_Send7bitAddress()函数发送10位地址的第二部分（LSB）。
  *     然后主机应等待事件6。
  */
/* EVT6 - 事件6：地址应答完成 */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY、MSL、ADDR、TXE和TRA标志位 */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY、MSL和ADDR标志位 */
/* EVT9 - 事件9：10位地址头发送完成 */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY、MSL和ADD10标志位 */

/******************************************************************************************************************** 
  * @brief  通信事件
  * 
  * 如果已生成START条件且从机地址已被应答，则主机必须检查以下事件之一进行通信过程：
  *  
  * 1) 主接收器模式：主机必须等待事件EVT7，然后使用I2C_ReceiveData()函数读取从机接收的数据。
  * 
  * 2) 主发送器模式：主机使用I2C_SendData()函数发送数据，然后等待事件EVT8或EVT8_2。
  *    这两个事件相似：
  *     - EVT8表示数据已写入数据寄存器并正在移位输出。
  *     - EVT8_2表示数据已物理移位输出并在总线上输出。
  *     在大多数情况下，使用EVT8对应用程序来说已足够。
  *     使用EVT8_2将导致通信速度变慢但更可靠。
  *     EVT8_2也比EVT8更适合测试最后一次数据传输。
  *    
  * 注意：
  * 如果用户软件不能保证在当前字节传输结束前处理EVT7事件，
  * 则用户可以同时检查I2C_EVENT_MASTER_BYTE_RECEIVED和I2C_FLAG_BTF标志位。
  * 但这样通信速度可能会变慢。
  */

/* 主接收模式 */ 
/* EVT7 - 事件7：主接收字节完成 */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY、MSL和RXNE标志位 */

/* 主发送模式 */
/* EVT8 - 事件8：主发送字节中 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA、BUSY、MSL、TXE标志位 */
/* EVT8_2 - 事件8_2：主发送字节完成 */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA、BUSY、MSL、TXE和BTF标志位 */


/******************I2C从模式事件（按通信顺序分组的事件）******************/

/******************************************************************************************************************** 
  * @brief  开始通信事件
  * 
  * 在通信开始时等待这些事件之一。这意味着I2C外设检测到主机设备在总线上生成的起始条件。
  * 如果通过I2C_AcknowledgeConfig()函数启用了应答功能，外设将在总线上生成ACK条件。
  *    
  * a) 正常情况下（从机只管理一个地址），当主机发送的地址与外设的自身地址匹配时
  *    （通过I2C_OwnAddress1字段配置），设置I2C_EVENT_SLAVE_XXX_ADDRESS_MATCHED事件
  *    （其中XXX可以是TRANSMITTER或RECEIVER）。
  *    
  * b) 如果主机发送的地址与外设的第二个地址匹配
  *    （通过I2C_OwnAddress2Config()函数配置并通过I2C_DualAddressCmd()函数使能），
  *    设置I2C_EVENT_SLAVE_XXX_SECONDADDRESS_MATCHED事件（其中XXX可以是TRANSMITTER或RECEIVER）。
  *   
  * c) 如果主机发送的地址是广播呼叫（地址0x00）并且外设启用了广播呼叫
  *    （使用I2C_GeneralCallCmd()函数），设置I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED事件。
  */

/* EVT1 - 事件1：从机地址匹配 */  
/* a) 从机管理单个地址的情况 */
#define  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED          ((uint32_t)0x00020002) /* BUSY和ADDR标志位 */
#define  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED       ((uint32_t)0x00060082) /* TRA、BUSY、TXE和ADDR标志位 */

/* b) 从机管理双地址的情况 */
#define  I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    ((uint32_t)0x00820000)  /* DUALF和BUSY标志位 */
#define  I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED ((uint32_t)0x00860080)  /* DUALF、TRA、BUSY和TXE标志位 */

/* c) 从机启用广播呼叫的情况 */
#define  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED        ((uint32_t)0x00120000)  /* GENCALL和BUSY标志位 */

/******************************************************************************************************************** 
  * @brief  通信事件
  * 
  * 当已检查EVT1时，等待以下事件之一：
  * 
  * - 从接收器模式：
  *     - EVT2：设备期望接收数据字节。
  *     - EVT4：设备期望通信结束：主机发送停止条件，数据传输停止。
  *    
  * - 从发送器模式：
  *    - EVT3：当从机已发送一个字节且主机期望字节传输结束时。
  *      I2C_EVENT_SLAVE_BYTE_TRANSMITTED和I2C_EVENT_SLAVE_BYTE_TRANSMITTING两个事件相似。
  *      如果用户软件不能保证在当前字节传输结束前处理EVT3，可以选择使用第二个事件。
  *    - EVT3_2：当主机发送NACK告诉从机设备数据传输应结束时。
  *      从机设备必须停止发送数据字节并等待总线上的停止条件。
  *      
  * 注意：
  * 如果用户软件不能保证在当前字节传输结束前处理事件2，
  * 用户可以同时检查I2C_EVENT_SLAVE_BYTE_RECEIVED和I2C_FLAG_BTF标志位。
  * 这样通信速度会变慢。
  */

/* 从接收器模式 */ 
/* EVT2 - 事件2：从接收字节完成 */
#define  I2C_EVENT_SLAVE_BYTE_RECEIVED                     ((uint32_t)0x00020040)  /* BUSY和RXNE标志位 */
/* EVT4 - 事件4：从机检测到停止位 */
#define  I2C_EVENT_SLAVE_STOP_DETECTED                     ((uint32_t)0x00000010)  /* STOPF标志位 */

/* 从发送器模式 */
/* EVT3 - 事件3：从发送字节事件 */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTED                  ((uint32_t)0x00060084)  /* TRA、BUSY、TXE和BTF标志位 */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTING                 ((uint32_t)0x00060080)  /* TRA、BUSY和TXE标志位 */
/* EVT3_2 - 事件3_2：从机应答失败 */
#define  I2C_EVENT_SLAVE_ACK_FAILURE                       ((uint32_t)0x00000400)  /* AF标志位 */


/* 函数原型声明 */

/* 初始化与配置函数 */
void I2C_DeInit(I2C_TypeDef* I2Cx);                                         /* I2C外设复位函数 */
void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct);         /* I2C初始化函数 */
void I2C_StructInit(I2C_InitTypeDef* I2C_InitStruct);                       /* I2C结构体初始化函数 */
void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);                  /* I2C使能控制函数 */
void I2C_DMACmd(I2C_TypeDef* I2Cx, FunctionalState NewState);               /* I2C DMA使能控制函数 */
void I2C_DMALastTransferCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);   /* I2C DMA最后一次传输控制函数 */

/* 通信控制函数 */
void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);        /* 产生START条件函数 */
void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);         /* 产生STOP条件函数 */
void I2C_AcknowledgeConfig(I2C_TypeDef* I2Cx, FunctionalState NewState);    /* 应答配置函数 */
void I2C_OwnAddress2Config(I2C_TypeDef* I2Cx, uint8_t Address);             /* 第二自身地址配置函数 */
void I2C_DualAddressCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);       /* 双地址模式使能控制函数 */
void I2C_GeneralCallCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);       /* 广播呼叫使能控制函数 */

/* 中断与数据函数 */
void I2C_ITConfig(I2C_TypeDef* I2Cx, uint16_t I2C_IT, FunctionalState NewState); /* I2C中断配置函数 */
void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);                          /* 发送数据函数 */
uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);                                  /* 接收数据函数 */
void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction); /* 发送7位地址函数 */
uint16_t I2C_ReadRegister(I2C_TypeDef* I2Cx, uint8_t I2C_Register);          /* 读取寄存器函数 */
void I2C_SoftwareResetCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);      /* 软件复位控制函数 */

/* 高级功能函数 */
void I2C_NACKPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_NACKPosition);   /* NACK位置配置函数 */
void I2C_SMBusAlertConfig(I2C_TypeDef* I2Cx, uint16_t I2C_SMBusAlert);       /* SMBus警报配置函数 */
void I2C_TransmitPEC(I2C_TypeDef* I2Cx, FunctionalState NewState);           /* 传输PEC控制函数 */
void I2C_PECPositionConfig(I2C_TypeDef* I2Cx, uint16_t I2C_PECPosition);     /* PEC位置配置函数 */
void I2C_CalculatePEC(I2C_TypeDef* I2Cx, FunctionalState NewState);          /* 计算PEC控制函数 */
uint8_t I2C_GetPEC(I2C_TypeDef* I2Cx);                                       /* 获取PEC值函数 */
void I2C_ARPCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);                /* ARP使能控制函数 */
void I2C_StretchClockCmd(I2C_TypeDef* I2Cx, FunctionalState NewState);       /* 时钟拉伸使能控制函数 */
void I2C_FastModeDutyCycleConfig(I2C_TypeDef* I2Cx, uint16_t I2C_DutyCycle); /* 快速模式占空比配置函数 */


/*****************************************************************************************
 *
 *                         I2C状态监控函数
 *                       
 ****************************************************************************************   
 * 该I2C驱动提供了三种不同的I2C状态监控方式，以满足应用程序的需求和约束：
 *        
 * a) 第一种方式：
 *    使用I2C_CheckEvent()函数：
 *    它将状态寄存器（STAR1和STAR2）的内容与给定事件（可以是多个标志位的组合）进行比较。
 *    如果当前状态寄存器包含给定的标志位，则返回SUCCESS。
 *    如果当前状态寄存器缺少标志位，则返回ERROR。
 *    - 使用时机：
 *      - 该函数适用于大多数应用程序以及启动活动，因为事件在产品参考手册（CH32FV2x-V3xRM）中有完整描述。
 *      - 也适用于需要定义自己事件的用户。
 *    - 限制：
 *      - 如果除了监控的错误之外还发生其他错误，I2C_CheckEvent()函数可能返回SUCCESS，尽管通信处于损坏状态。
 *        建议使用错误中断来监控错误事件并在IRQ处理程序中处理它们。
 *
 *        
 *        注意： 
 *        以下函数推荐用于错误管理：
 *          - I2C_ITConfig()：配置和使能错误中断的主要函数。
 *          - I2Cx_ER_IRQHandler()：错误中断发生时被调用。
 *            其中x是外设实例（I2C1、I2C2...）
 *          - I2Cx_ER_IRQHandler()将调用I2C_GetFlagStatus()或I2C_GetITStatus()函数来确定发生了哪个错误。
 *          - 使用I2C_ClearFlag()、I2C_ClearITPendingBit()、I2C_SoftwareResetCmd()、
 *            I2C_GenerateStop()来清除错误标志和错误源，并返回到正确的通信状态。
 *            
 *
 *  b) 第二种方式：
 *     使用函数获取由状态寄存器1和寄存器2组成的单个字（uint32_t）。
 *     （状态寄存器2的值左移16位并与状态寄存器1连接）。
 *     - 使用时机：
 *
 *       - 该函数适用于上述相同的应用程序，但没有I2C_GetFlagStatus()函数的限制。
 *         返回值可以与库中已定义的事件（CH32V30x_i2c.h）或用户定义的自定义值进行比较。
 *       - 该函数可用于同时监控多个标志位的状态。
 *       - 与I2C_CheckEvent()函数相反，该函数可以根据用户需求选择何时接受事件
 *         （当所有事件标志位都设置且没有其他标志位设置时，或仅当所需的标志位设置时）。
 *     
 *     - 限制：
 *       - 用户可能需要定义自己的事件。
 *       - 如果用户决定只检查常规通信标志位（而忽略错误标志位），则相同的错误管理注意事项适用于此函数。
 *     
 *
 *  c) 第三种方式：
 *     使用I2C_GetFlagStatus()函数获取单个标志位的状态。
 *     - 使用时机：
 *        - 该函数可用于特定应用程序或调试阶段。
 *        - 当只需要检查一个标志位时适用。
 *          
 *     - 限制： 
 *        - 调用此函数访问状态寄存器可能会清除某些标志位。
 *       - 可能需要调用两次或更多次来监控一个单一事件。
 */
            
 

/*********************************************************
 * 
 *  a) 基本状态监控（第一种方式）
 ********************************************************
 */
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);  /* 检查事件函数 */

/*********************************************************
 * 
 *  b) 高级状态监控（第二种方式）
 ********************************************************
 */
uint32_t I2C_GetLastEvent(I2C_TypeDef* I2Cx);                       /* 获取最后事件函数 */

/*********************************************************
 * 
 *  c) 基于标志位的状态监控（第三种方式）
 *********************************************************
 */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG); /* 获取标志位状态函数 */

/* 标志位与中断处理函数 */
void I2C_ClearFlag(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);           /* 清除标志位函数 */
ITStatus I2C_GetITStatus(I2C_TypeDef* I2Cx, uint32_t I2C_IT);       /* 获取中断状态函数 */
void I2C_ClearITPendingBit(I2C_TypeDef* I2Cx, uint32_t I2C_IT);     /* 清除中断挂起位函数 */

#ifdef __cplusplus
}
#endif

#endif 
