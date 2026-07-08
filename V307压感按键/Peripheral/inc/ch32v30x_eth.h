/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_eth.h
* 作者              : WCH
* 版本              : V1.0.1
* 日期              : 2025/01/08
* 描述              : 本文件包含ETH固件库的所有函数原型。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或不修改）和二进制代码用于南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_ETH_H
#define __CH32V30x_ETH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* 内部10BASE-T PHY链接状态定义 */
#define PHY_10BASE_T_LINKED       1    /* 内部PHY已链接 */
#define PHY_10BASE_T_NOT_LINKED   0    /* 内部PHY未链接 */

/* DMA发送进程状态掩码 */
#define DMA_TPS_Mask      ((uint32_t)0x00700000)     /* 发送进程状态位掩码（DMASR寄存器位20-22） */
/* DMA接收进程状态掩码 */
#define DMA_RPS_Mask      ((uint32_t)0x000E0000)     /* 接收进程状态位掩码（DMASR寄存器位17-19） */

/* ETH初始化结构体定义 */
typedef struct {
    uint32_t             ETH_AutoNegotiation;             /* 选择外部PHY的自动协商模式
                                                             自动协商允许自动设置速度(10/100Mbps)和模式(半双工/全双工)
                                                             可选值见 @ref ETH_AutoNegotiation */

    uint32_t             ETH_Watchdog;                    /* 选择是否使能看门狗定时器
                                                             使能时，MAC最多允许接收2048字节
                                                             禁用时，MAC最多可接收16384字节
                                                             可选值见 @ref ETH_watchdog */

    uint32_t             ETH_Jabber;                      /* 选择是否使能超长帧定时器
                                                             使能时，MAC最多允许发送2048字节
                                                             禁用时，MAC最多可发送16384字节
                                                             可选值见 @ref ETH_Jabber */

    uint32_t             ETH_InterFrameGap;               /* 选择发送时帧间最小间隔
                                                             可选值见 @ref ETH_Inter_Frame_Gap */

    uint32_t             ETH_CarrierSense;                /* 选择是否使能载波监听
                                                             可选值见 @ref ETH_Carrier_Sense */

    uint32_t             ETH_Speed;                       /* 设置以太网速度：10/100 Mbps
                                                             可选值见 @ref ETH_Speed */

    uint32_t             ETH_ReceiveOwn;                  /* 选择是否使能ReceiveOwn
                                                             在半双工模式下，当TX_EN信号有效时，允许接收帧
                                                             可选值见 @ref ETH_Receive_Own */

    uint32_t             ETH_LoopbackMode;                /* 选择是否使能内部MAC MII环回模式
                                                             可选值见 @ref ETH_Loop_Back_Mode */

    uint32_t             ETH_Mode;                        /* 选择MAC双工模式：半双工或全双工
                                                             可选值见 @ref ETH_Duplex_Mode */

    uint32_t             ETH_ChecksumOffload;             /* 选择是否使能接收帧负载的TCP/UDP/ICMP头部的IPv4校验和检查
                                                             可选值见 @ref ETH_Checksum_Offload */

    uint32_t             ETH_RetryTransmission;           /* 选择在半双工模式下发生冲突时，MAC是否根据BL设置尝试重传
                                                             可选值见 @ref ETH_Retry_Transmission */

    uint32_t             ETH_AutomaticPadCRCStrip;        /* 选择是否使能MAC自动填充/CRC剥离
                                                             可选值见 @ref ETH_Automatic_Pad_CRC_Strip */

    uint32_t             ETH_BackOffLimit;                /* 选择退避极限值
                                                             可选值见 @ref ETH_Back_Off_Limit */

    uint32_t             ETH_DeferralCheck;               /* 选择是否使能推迟检查功能（半双工模式）
                                                             可选值见 @ref ETH_Deferral_Check */

    uint32_t             ETH_ReceiveAll;                  /* 选择是否使能MAC接收所有帧（无过滤）
                                                             可选值见 @ref ETH_Receive_All */

    uint32_t             ETH_SourceAddrFilter;            /* 选择源地址过滤模式
                                                             可选值见 @ref ETH_Source_Addr_Filter */

    uint32_t             ETH_PassControlFrames;           /* 设置控制帧的转发模式（包括单播和多播PAUSE帧）
                                                             可选值见 @ref ETH_Pass_Control_Frames */

    uint32_t             ETH_BroadcastFramesReception;    /* 选择是否接收广播帧
                                                             可选值见 @ref ETH_Broadcast_Frames_Reception */

    uint32_t             ETH_DestinationAddrFilter;       /* 设置单播和多播帧的目的地址过滤模式
                                                             可选值见 @ref ETH_Destination_Addr_Filter */

    uint32_t             ETH_PromiscuousMode;             /* 选择是否使能混杂模式
                                                             可选值见 @ref ETH_Promiscuous_Mode */

    uint32_t             ETH_MulticastFramesFilter;       /* 选择多播帧过滤模式：无/哈希表过滤/完美过滤/完美+哈希过滤
                                                             可选值见 @ref ETH_Multicast_Frames_Filter */

    uint32_t             ETH_UnicastFramesFilter;         /* 选择单播帧过滤模式：哈希表过滤/完美过滤/完美+哈希过滤
                                                             可选值见 @ref ETH_Unicast_Frames_Filter */

    uint32_t             ETH_HashTableHigh;               /* 哈希表高32位 */

    uint32_t             ETH_HashTableLow;                /* 哈希表低32位 */

    uint32_t             ETH_PauseTime;                   /* 用于发送控制帧中暂停时间字段的值 */

    uint32_t             ETH_ZeroQuantaPause;             /* 选择是否自动生成零量子暂停控制帧
                                                             可选值见 @ref ETH_Zero_Quanta_Pause */

    uint32_t             ETH_PauseLowThreshold;           /* 配置PAUSE低阈值，用于自动重传PAUSE帧的检查
                                                             可选值见 @ref ETH_Pause_Low_Threshold */
                                                           
    uint32_t             ETH_UnicastPauseFrameDetect;     /* 选择是否使能MAC检测暂停帧（使用MAC地址0单播地址和唯一多播地址）
                                                             可选值见 @ref ETH_Unicast_Pause_Frame_Detect */

    uint32_t             ETH_ReceiveFlowControl;          /* 使能或禁用MAC解码接收到的暂停帧，并在指定时间内禁用其发送器
                                                             可选值见 @ref ETH_Receive_Flow_Control */

    uint32_t             ETH_TransmitFlowControl;         /* 使能或禁用MAC发送暂停帧（全双工模式）或MAC背压操作（半双工模式）
                                                             可选值见 @ref ETH_Transmit_Flow_Control */

    uint32_t             ETH_VLANTagComparison;           /* 选择用于比较和过滤的VLAN标识符：12位或完整的16位VLAN标签
                                                             可选值见 @ref ETH_VLAN_Tag_Comparison */

    uint32_t             ETH_VLANTagIdentifier;           /* 接收帧的VLAN标签标识符 */

    uint32_t             ETH_DropTCPIPChecksumErrorFrame; /* 选择是否丢弃TCP/IP校验和错误的帧
                                                             可选值见 @ref ETH_Drop_TCP_IP_Checksum_Error_Frame */ 

    uint32_t             ETH_ReceiveStoreForward;         /* 使能或禁用接收存储转发模式
                                                             可选值见 @ref ETH_Receive_Store_Forward */ 

    uint32_t             ETH_FlushReceivedFrame;          /* 使能或禁用刷新接收到的帧
                                                             可选值见 @ref ETH_Flush_Received_Frame */ 

    uint32_t             ETH_TransmitStoreForward;        /* 使能或禁用发送存储转发模式
                                                             可选值见 @ref ETH_Transmit_Store_Forward */ 

    uint32_t             ETH_TransmitThresholdControl;    /* 选择发送阈值控制
                                                             可选值见 @ref ETH_Transmit_Threshold_Control */

    uint32_t             ETH_ForwardErrorFrames;          /* 选择是否将错误帧转发给DMA
                                                             可选值见 @ref ETH_Forward_Error_Frames */

    uint32_t             ETH_ForwardUndersizedGoodFrames; /* 使能或禁用Rx FIFO转发过小帧（无错误且长度小于64字节，包括填充字节和CRC）
                                                             可选值见 @ref ETH_Forward_Undersized_Good_Frames */

    uint32_t             ETH_ReceiveThresholdControl;     /* 选择接收FIFO的阈值电平
                                                             可选值见 @ref ETH_Receive_Threshold_Control */

    uint32_t             ETH_SecondFrameOperate;          /* 选择是否使能第二帧操作模式，允许DMA在获得第一帧状态之前处理第二帧发送数据
                                                             可选值见 @ref ETH_Second_Frame_Operate */

    uint32_t             ETH_AddressAlignedBeats;         /* 使能或禁用地址对齐突发
                                                             可选值见 @ref ETH_Address_Aligned_Beats */

    uint32_t             ETH_FixedBurst;                  /* 使能或禁用AHB主接口固定突发传输
                                                             可选值见 @ref ETH_Fixed_Burst */
                       
    uint32_t             ETH_RxDMABurstLength;            /* 指示一次Rx DMA事务中最多传输的拍数
                                                             可选值见 @ref ETH_Rx_DMA_Burst_Length */ 

    uint32_t             ETH_TxDMABurstLength;            /* 指示一次Tx DMA事务中最多传输的拍数
                                                             可选值见 @ref ETH_Tx_DMA_Burst_Length */                                                   

    uint32_t             ETH_DescriptorSkipLength;        /* 指定在环模式下两个非链式描述符之间跳过的字数 */

    uint32_t             ETH_DMAArbitration;              /* 选择DMA发送/接收仲裁方式
                                                             可选值见 @ref ETH_DMA_Arbitration */  
}ETH_InitTypeDef;

/* ETH延时函数，默认使用精度较低的ETH_Delay函数 */
#define _eth_delay_    ETH_Delay       /* 默认 _eth_delay_ 函数，定时不精确 */

/* 以太网帧相关定义 */
#define ETH_MAX_PACKET_SIZE    1524    /* 以太网最大包长 = 以太网头 + 额外字节 + 最大负载 + CRC */
#define ETH_HEADER               14    /* 以太网头：6字节目的地址，6字节源地址，2字节长度/类型 */
#define ETH_CRC                   4    /* 以太网CRC */
#define ETH_EXTRA                 2    /* 某些情况下的额外字节 */
#define VLAN_TAG                  4    /* 可选的802.1q VLAN标签 */
#define MIN_ETH_PAYLOAD          46    /* 最小以太网负载 */
#define MAX_ETH_PAYLOAD        1500    /* 最大以太网负载 */
#define JUMBO_FRAME_PAYLOAD    9000    /* 巨帧负载大小 */

/* ETH DMA描述符结构体定义 */
typedef struct
{
  uint32_t   volatile Status;       /* 状态 */
  uint32_t   ControlBufferSize;     /* 控制以及缓冲区1和2的长度 */
  uint32_t   Buffer1Addr;           /* 缓冲区1地址指针 */
  uint32_t   Buffer2NextDescAddr;   /* 缓冲区2地址或下一个描述符地址指针 */
} ETH_DMADESCTypeDef;

/**
   DMA发送描述符结构
  -----------------------------------------------------------------------------------------------
  TDES0 | OWN(31) | CTRL[30:26] | Reserved[25:24] | CTRL[23:20] | Reserved[19:17] | Status[16:0] |
  -----------------------------------------------------------------------------------------------
  TDES1 | Reserved[31:29] | Buffer2字节数[28:16] | Reserved[15:13] | Buffer1字节数[12:0] |
  -----------------------------------------------------------------------------------------------
  TDES2 |                         缓冲区1地址 [31:0]                                           |
  -----------------------------------------------------------------------------------------------
  TDES3 |                   缓冲区2地址 [31:0] / 下一个描述符地址 [31:0]                         |
  ------------------------------------------------------------------------------------------------
*/

/* TDES0寄存器（DMA发送描述符状态寄存器）位或字段定义 */
#define ETH_DMATxDesc_OWN                     ((uint32_t)0x80000000)  /* OWN位：描述符由DMA引擎拥有 */
#define ETH_DMATxDesc_IC                      ((uint32_t)0x40000000)  /* 完成时中断 */
#define ETH_DMATxDesc_LS                      ((uint32_t)0x20000000)  /* 最后一个段 */
#define ETH_DMATxDesc_FS                      ((uint32_t)0x10000000)  /* 第一个段 */
#define ETH_DMATxDesc_DC                      ((uint32_t)0x08000000)  /* 禁用CRC */
#define ETH_DMATxDesc_DP                      ((uint32_t)0x04000000)  /* 禁用填充 */
#define ETH_DMATxDesc_TTSE                    ((uint32_t)0x02000000)  /* 发送时间戳使能 */
#define ETH_DMATxDesc_CIC                     ((uint32_t)0x00C00000)  /* 校验和插入控制：4种情况 */
#define ETH_DMATxDesc_CIC_ByPass              ((uint32_t)0x00000000)  /* 不做处理：绕过校验和引擎 */
#define ETH_DMATxDesc_CIC_IPV4Header          ((uint32_t)0x00400000)  /* IPv4头部校验和插入 */
#define ETH_DMATxDesc_CIC_TCPUDPICMP_Segment  ((uint32_t)0x00800000)  /* TCP/UDP/ICMP校验和（仅针对段计算）插入 */
#define ETH_DMATxDesc_CIC_TCPUDPICMP_Full     ((uint32_t)0x00C00000)  /* TCP/UDP/ICMP校验和完全计算插入 */
#define ETH_DMATxDesc_TER                     ((uint32_t)0x00200000)  /* 发送环尾 */
#define ETH_DMATxDesc_TCH                     ((uint32_t)0x00100000)  /* 第二地址链模式 */
#define ETH_DMATxDesc_TTSS                    ((uint32_t)0x00020000)  /* 发送时间戳状态 */
#define ETH_DMATxDesc_IHE                     ((uint32_t)0x00010000)  /* IP头部错误 */
#define ETH_DMATxDesc_ES                      ((uint32_t)0x00008000)  /* 错误汇总：以下位的或：UE || ED || EC || LCO || NC || LCA || FF || JT */
#define ETH_DMATxDesc_JT                      ((uint32_t)0x00004000)  /* 超长帧超时 */
#define ETH_DMATxDesc_FF                      ((uint32_t)0x00002000)  /* 帧被刷新：DMA/MTL因软件刷新而丢弃帧 */
#define ETH_DMATxDesc_PCE                     ((uint32_t)0x00001000)  /* 负载校验和错误 */
#define ETH_DMATxDesc_LCA                     ((uint32_t)0x00000800)  /* 载波丢失：传输过程中载波丢失 */
#define ETH_DMATxDesc_NC                      ((uint32_t)0x00000400)  /* 无载波：收发器无载波信号 */
#define ETH_DMATxDesc_LCO                     ((uint32_t)0x00000200)  /* 晚冲突：因冲突而中止传输 */
#define ETH_DMATxDesc_EC                      ((uint32_t)0x00000100)  /* 过度冲突：16次冲突后中止传输 */
#define ETH_DMATxDesc_VF                      ((uint32_t)0x00000080)  /* VLAN帧 */
#define ETH_DMATxDesc_CC                      ((uint32_t)0x00000078)  /* 冲突计数 */
#define ETH_DMATxDesc_ED                      ((uint32_t)0x00000004)  /* 过度推迟 */
#define ETH_DMATxDesc_UF                      ((uint32_t)0x00000002)  /* 下溢错误：内存数据到达延迟 */
#define ETH_DMATxDesc_DB                      ((uint32_t)0x00000001)  /* 推迟位 */

/* TDES1寄存器字段定义 */
#define ETH_DMATxDesc_TBS2  ((uint32_t)0x1FFF0000)  /* 发送缓冲区2大小 */
#define ETH_DMATxDesc_TBS1  ((uint32_t)0x00001FFF)  /* 发送缓冲区1大小 */

/* TDES2寄存器字段定义 */
#define ETH_DMATxDesc_B1AP  ((uint32_t)0xFFFFFFFF)  /* 缓冲区1地址指针 */

/* TDES3寄存器字段定义 */
#define ETH_DMATxDesc_B2AP  ((uint32_t)0xFFFFFFFF)  /* 缓冲区2地址指针 */

/**
  DMA接收描述符结构
  ---------------------------------------------------------------------------------------------------------------------
  RDES0 | OWN(31) |                                             状态 [30:0]                                            |
  ---------------------------------------------------------------------------------------------------------------------
  RDES1 | CTRL(31) | Reserved[30:29] | 缓冲区2字节数[28:16] | CTRL[15:14] | Reserved(13) | 缓冲区1字节数[12:0] |
  ---------------------------------------------------------------------------------------------------------------------
  RDES2 |                                       缓冲区1地址 [31:0]                                                  |
  ---------------------------------------------------------------------------------------------------------------------
  RDES3 |                          缓冲区2地址 [31:0] / 下一个描述符地址 [31:0]                                    |
  ----------------------------------------------------------------------------------------------------------------------
*/

/* RDES0寄存器（DMA接收描述符状态寄存器）位或字段定义 */
#define ETH_DMARxDesc_OWN         ((uint32_t)0x80000000)  /* OWN位：描述符由DMA引擎拥有 */
#define ETH_DMARxDesc_AFM         ((uint32_t)0x40000000)  /* 接收帧的DA过滤失败 */
#define ETH_DMARxDesc_FL          ((uint32_t)0x3FFF0000)  /* 接收描述符帧长度 */
#define ETH_DMARxDesc_ES          ((uint32_t)0x00008000)  /* 错误汇总：以下位的或：DE || OE || IPC || LC || RWT || RE || CE */
#define ETH_DMARxDesc_DE          ((uint32_t)0x00004000)  /* 描述符错误：无更多描述符用于接收帧 */
#define ETH_DMARxDesc_SAF         ((uint32_t)0x00002000)  /* 接收帧的SA过滤失败 */
#define ETH_DMARxDesc_LE          ((uint32_t)0x00001000)  /* 帧大小与长度字段不匹配 */
#define ETH_DMARxDesc_OE          ((uint32_t)0x00000800)  /* 溢出错误：因缓冲区溢出而损坏帧 */
#define ETH_DMARxDesc_VLAN        ((uint32_t)0x00000400)  /* VLAN标签：接收到的帧是VLAN帧 */
#define ETH_DMARxDesc_FS          ((uint32_t)0x00000200)  /* 帧的第一个描述符 */
#define ETH_DMARxDesc_LS          ((uint32_t)0x00000100)  /* 帧的最后一个描述符 */
#define ETH_DMARxDesc_IPV4HCE     ((uint32_t)0x00000080)  /* IPC校验和错误：接收到的IPv4头部校验和错误 */
#define ETH_DMARxDesc_LC          ((uint32_t)0x00000040)  /* 接收过程中发生晚冲突 */
#define ETH_DMARxDesc_FT          ((uint32_t)0x00000020)  /* 帧类型 - 以太网，否则为802.3 */
#define ETH_DMARxDesc_RWT         ((uint32_t)0x00000010)  /* 接收看门狗超时：接收过程中看门狗定时器超时 */
#define ETH_DMARxDesc_RE          ((uint32_t)0x00000008)  /* 接收错误：MII接口报告的错误 */
#define ETH_DMARxDesc_DBE         ((uint32_t)0x00000004)  /* 散位错误：帧包含非8位整数倍的数据 */
#define ETH_DMARxDesc_CE          ((uint32_t)0x00000002)  /* CRC错误 */
#define ETH_DMARxDesc_MAMPCE      ((uint32_t)0x00000001)  /* 接收MAC地址/负载校验和错误：接收MAC地址匹配/接收负载校验和错误 */

/* RDES1寄存器位或字段定义 */
#define ETH_DMARxDesc_DIC   ((uint32_t)0x80000000)  /* 完成时禁用中断 */
#define ETH_DMARxDesc_RBS2  ((uint32_t)0x1FFF0000)  /* 接收缓冲区2大小 */
#define ETH_DMARxDesc_RER   ((uint32_t)0x00008000)  /* 接收环尾 */
#define ETH_DMARxDesc_RCH   ((uint32_t)0x00004000)  /* 第二地址链模式 */
#define ETH_DMARxDesc_RBS1  ((uint32_t)0x00001FFF)  /* 接收缓冲区1大小 */

/* RDES2寄存器字段定义 */
#define ETH_DMARxDesc_B1AP  ((uint32_t)0xFFFFFFFF)  /* 缓冲区1地址指针 */

/* RDES3寄存器字段定义 */
#define ETH_DMARxDesc_B2AP  ((uint32_t)0xFFFFFFFF)  /* 缓冲区2地址指针 */

/* 读取或写入PHY寄存器的超时阈值 */
#define PHY_READ_TO                     ((uint32_t)0x004FFFFF)  /* PHY读超时 */
#define PHY_WRITE_TO                    ((uint32_t)0x0004FFFF)  /* PHY写超时 */

/* PHY复位后延时 */
#define PHY_ResetDelay                  ((uint32_t)0x000FFFFF) 

/* 配置PHY后延时 */
#define PHY_ConfigDelay                 ((uint32_t)0x00FFFFFF)

/* PHY基本寄存器 */
#define PHY_BCR                          0x0           /* PHY收发器基本控制寄存器 */
#define PHY_BSR                          0x01          /* PHY收发器基本状态寄存器 */
#define PHY_ANAR                         0x04          /* 自动协商通告寄存器 */
#define PHY_ANLPAR                       0x05          /* 自动协商链路伙伴基页能力寄存器 */
#define PHY_ANER                         0x06          /* 自动协商扩展寄存器 */
#define PHY_BMCR                         PHY_BCR       /* 基本模式控制寄存器 */
#define PHY_BMSR                         PHY_BSR       /* 基本模式状态寄存器 */
#define PHY_STATUS                       0x10          /* 状态寄存器 */
#define PHY_MDIX                         0x1E          /* MDIX控制寄存器 */

/* PHY基本控制寄存器位或字段定义 */
#define PHY_Reset                       ((uint16_t)0x8000)      /* PHY复位 */
#define PHY_Loopback                    ((uint16_t)0x4000)      /* 选择环回模式 */
#define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100)      /* 设置100M全双工模式 */
#define PHY_HALFDUPLEX_100M              ((uint16_t)0x2000)      /* 设置100M半双工模式 */
#define PHY_FULLDUPLEX_10M               ((uint16_t)0x0100)      /* 设置10M全双工模式 */
#define PHY_HALFDUPLEX_10M               ((uint16_t)0x0000)      /* 设置10M半双工模式 */
#define PHY_AutoNegotiation             ((uint16_t)0x1000)      /* 使能自动协商功能 */
#define PHY_Restart_AutoNegotiation     ((uint16_t)0x0200)      /* 重启自动协商功能 */
#define PHY_Powerdown                   ((uint16_t)0x0800)      /* 选择掉电模式 */
#define PHY_Isolate                     ((uint16_t)0x0400)      /* 从MII隔离PHY */

/* PHY基本状态寄存器位或字段定义 */
#define PHY_AutoNego_Complete           ((uint16_t)0x0020)      /* 自动协商过程完成 */
#define PHY_Linked_Status               ((uint16_t)0x0004)      /* 有效链接建立 */
#define PHY_Jabber_detection            ((uint16_t)0x0002)      /* 检测到超长条件 */
#define PHY_RMII_Mode                   ((uint16_t)0x0020)      /* RMII模式 */

/* 内部10BASE-T PHY 50R*4上拉电阻使能或禁用 */
#define ETH_Internal_Pull_Up_Res_Enable     ((uint32_t)0x00100000)  /* 使能内部上拉电阻 */
#define ETH_Internal_Pull_Up_Res_Disable    ((uint32_t)0x00000000)  /* 禁用内部上拉电阻 */

/* MAC自动协商使能或禁用 */
#define ETH_AutoNegotiation_Enable     ((uint32_t)0x00000001)  /* 使能自动协商 */
#define ETH_AutoNegotiation_Disable    ((uint32_t)0x00000000)  /* 禁用自动协商 */

/* MAC看门狗使能或禁用 */
#define ETH_Watchdog_Enable       ((uint32_t)0x00000000)  /* 使能看门狗（允许最多2048字节） */
#define ETH_Watchdog_Disable      ((uint32_t)0x00800000)  /* 禁用看门狗（允许最多16384字节） */

/* MAC超长帧定时器使能或禁用 */
#define ETH_Jabber_Enable    ((uint32_t)0x00000000)  /* 使能超长帧定时器（限制发送2048字节） */
#define ETH_Jabber_Disable   ((uint32_t)0x00400000)  /* 禁用超长帧定时器（允许发送最多16384字节） */

/* 发送时最小帧间间隔值 */
#define ETH_InterFrameGap_96Bit   ((uint32_t)0x00000000)  /* 最小帧间间隔为96位 */
#define ETH_InterFrameGap_88Bit   ((uint32_t)0x00020000)  /* 最小帧间间隔为88位 */
#define ETH_InterFrameGap_80Bit   ((uint32_t)0x00040000)  /* 最小帧间间隔为80位 */
#define ETH_InterFrameGap_72Bit   ((uint32_t)0x00060000)  /* 最小帧间间隔为72位 */
#define ETH_InterFrameGap_64Bit   ((uint32_t)0x00080000)  /* 最小帧间间隔为64位 */
#define ETH_InterFrameGap_56Bit   ((uint32_t)0x000A0000)  /* 最小帧间间隔为56位 */
#define ETH_InterFrameGap_48Bit   ((uint32_t)0x000C0000)  /* 最小帧间间隔为48位 */
#define ETH_InterFrameGap_40Bit   ((uint32_t)0x000E0000)  /* 最小帧间间隔为40位 */

/* MAC载波监听使能或禁用 */
#define ETH_CarrierSense_Enable   ((uint32_t)0x00000000)  /* 使能载波监听 */
#define ETH_CarrierSense_Disable  ((uint32_t)0x00010000)  /* 禁用载波监听 */

/* MAC速度 */
#define ETH_Speed_10M        ((uint32_t)0x00000000)  /* 10M速度 */
#define ETH_Speed_100M       ((uint32_t)0x00004000)  /* 100M速度 */
#define ETH_Speed_1000M      ((uint32_t)0x00008000)  /* 1000M速度 */

/* MAC接收自己发送的帧使能或禁用 */
#define ETH_ReceiveOwn_Enable     ((uint32_t)0x00000000)  /* 使能接收自己发送的帧（半双工） */
#define ETH_ReceiveOwn_Disable    ((uint32_t)0x00002000)  /* 禁用接收自己发送的帧 */

/* MAC环回模式使能或禁用 */
#define ETH_LoopbackMode_Enable        ((uint32_t)0x00001000)  /* 使能环回 */
#define ETH_LoopbackMode_Disable       ((uint32_t)0x00000000)  /* 禁用环回 */

/* MAC全双工或半双工模式 */
#define ETH_Mode_FullDuplex       ((uint32_t)0x00000800)  /* 全双工模式 */
#define ETH_Mode_HalfDuplex       ((uint32_t)0x00000000)  /* 半双工模式 */

/* MAC校验和卸载使能或禁用 */
#define ETH_ChecksumOffload_Enable     ((uint32_t)0x00000400)  /* 使能校验和卸载 */
#define ETH_ChecksumOffload_Disable    ((uint32_t)0x00000000)  /* 禁用校验和卸载 */

/* MAC发送重试使能或禁用 */
#define ETH_RetryTransmission_Enable   ((uint32_t)0x00000000)  /* 使能重试 */
#define ETH_RetryTransmission_Disable  ((uint32_t)0x00000200)  /* 禁用重试 */

/* MAC自动填充/CRC剥离使能或禁用 */
#define ETH_AutomaticPadCRCStrip_Enable     ((uint32_t)0x00000080)  /* 使能自动填充/CRC剥离 */
#define ETH_AutomaticPadCRCStrip_Disable    ((uint32_t)0x00000000)  /* 禁用自动填充/CRC剥离 */

/* MAC退避极限 */
#define ETH_BackOffLimit_10  ((uint32_t)0x00000000)  /* 退避极限10 */
#define ETH_BackOffLimit_8   ((uint32_t)0x00000020)  /* 退避极限8 */
#define ETH_BackOffLimit_4   ((uint32_t)0x00000040)  /* 退避极限4 */
#define ETH_BackOffLimit_1   ((uint32_t)0x00000060)  /* 退避极限1 */

/* MAC推迟检查使能或禁用 */
#define ETH_DeferralCheck_Enable       ((uint32_t)0x00000010)  /* 使能推迟检查 */
#define ETH_DeferralCheck_Disable      ((uint32_t)0x00000000)  /* 禁用推迟检查 */

/* MAC接收所有帧使能或禁用 */
#define ETH_ReceiveAll_Enable     ((uint32_t)0x80000000)  /* 使能接收所有帧 */
#define ETH_ReceiveAll_Disable    ((uint32_t)0x00000000)  /* 禁用接收所有帧 */

/* MAC源地址过滤 */
#define ETH_SourceAddrFilter_Normal_Enable       ((uint32_t)0x00000200)  /* 正常源地址过滤使能 */
#define ETH_SourceAddrFilter_Inverse_Enable      ((uint32_t)0x00000300)  /* 反向源地址过滤使能 */
#define ETH_SourceAddrFilter_Disable             ((uint32_t)0x00000000)  /* 禁用源地址过滤 */

/* MAC控制帧传递 */
#define ETH_PassControlFrames_BlockAll                ((uint32_t)0x00000040)  /* MAC过滤所有控制帧，不传递给应用 */
#define ETH_PassControlFrames_ForwardAll              ((uint32_t)0x00000080)  /* MAC将所有控制帧转发给应用，即使地址过滤失败 */
#define ETH_PassControlFrames_ForwardPassedAddrFilter ((uint32_t)0x000000C0)  /* MAC转发通过地址过滤的控制帧 */

/* MAC广播帧接收 */
#define ETH_BroadcastFramesReception_Enable      ((uint32_t)0x00000000)  /* 使能接收广播帧 */
#define ETH_BroadcastFramesReception_Disable     ((uint32_t)0x00000020)  /* 禁用接收广播帧 */

/* MAC目的地址过滤 */
#define ETH_DestinationAddrFilter_Normal    ((uint32_t)0x00000000)  /* 正常目的地址过滤 */
#define ETH_DestinationAddrFilter_Inverse   ((uint32_t)0x00000008)  /* 反向目的地址过滤 */

/* MAC混杂模式使能或禁用 */
#define ETH_PromiscuousMode_Enable     ((uint32_t)0x00000001)  /* 使能混杂模式 */
#define ETH_PromiscuousMode_Disable    ((uint32_t)0x00000000)  /* 禁用混杂模式 */

/* MAC多播帧过滤 */
#define ETH_MulticastFramesFilter_PerfectHashTable    ((uint32_t)0x00000404)  /* 完美+哈希表过滤多播帧 */
#define ETH_MulticastFramesFilter_HashTable           ((uint32_t)0x00000004)  /* 哈希表过滤多播帧 */
#define ETH_MulticastFramesFilter_Perfect             ((uint32_t)0x00000000)  /* 完美过滤多播帧 */
#define ETH_MulticastFramesFilter_None                ((uint32_t)0x00000010)  /* 不过滤多播帧 */

/* MAC单播帧过滤 */
#define ETH_UnicastFramesFilter_PerfectHashTable ((uint32_t)0x00000402)  /* 完美+哈希表过滤单播帧 */
#define ETH_UnicastFramesFilter_HashTable        ((uint32_t)0x00000002)  /* 哈希表过滤单播帧 */
#define ETH_UnicastFramesFilter_Perfect          ((uint32_t)0x00000000)  /* 完美过滤单播帧 */

/* MAC零量子暂停 */
#define ETH_ZeroQuantaPause_Enable     ((uint32_t)0x00000000)  /* 使能零量子暂停（允许生成） */
#define ETH_ZeroQuantaPause_Disable    ((uint32_t)0x00000080)  /* 禁用零量子暂停 */

/* MAC暂停低阈值 */
#define ETH_PauseLowThreshold_Minus4        ((uint32_t)0x00000000)  /* 暂停时间减4个时隙 */
#define ETH_PauseLowThreshold_Minus28       ((uint32_t)0x00000010)  /* 暂停时间减28个时隙 */
#define ETH_PauseLowThreshold_Minus144      ((uint32_t)0x00000020)  /* 暂停时间减144个时隙 */
#define ETH_PauseLowThreshold_Minus256      ((uint32_t)0x00000030)  /* 暂停时间减256个时隙 */

/* MAC单播暂停帧检测使能或禁用 */
#define ETH_UnicastPauseFrameDetect_Enable  ((uint32_t)0x00000008)  /* 使能单播暂停帧检测 */
#define ETH_UnicastPauseFrameDetect_Disable ((uint32_t)0x00000000)  /* 禁用单播暂停帧检测 */

/* MAC接收流控制使能或禁用 */
#define ETH_ReceiveFlowControl_Enable       ((uint32_t)0x00000004)  /* 使能接收流控制 */
#define ETH_ReceiveFlowControl_Disable      ((uint32_t)0x00000000)  /* 禁用接收流控制 */

/* MAC发送流控制使能或禁用 */
#define ETH_TransmitFlowControl_Enable      ((uint32_t)0x00000002)  /* 使能发送流控制 */
#define ETH_TransmitFlowControl_Disable     ((uint32_t)0x00000000)  /* 禁用发送流控制 */

/* MAC VLAN标签比较 */
#define ETH_VLANTagComparison_12Bit    ((uint32_t)0x00010000)  /* 12位VLAN标签比较 */
#define ETH_VLANTagComparison_16Bit    ((uint32_t)0x00000000)  /* 16位VLAN标签比较 */

/* MAC标志 */
#define ETH_MAC_FLAG_TST     ((uint32_t)0x00000200)  /* 时间戳触发标志（在MAC上） */
#define ETH_MAC_FLAG_MMCT    ((uint32_t)0x00000040)  /* MMC发送标志 */
#define ETH_MAC_FLAG_MMCR    ((uint32_t)0x00000020)  /* MMC接收标志 */
#define ETH_MAC_FLAG_MMC     ((uint32_t)0x00000010)  /* MMC标志（在MAC上） */
#define ETH_MAC_FLAG_PMT     ((uint32_t)0x00000008)  /* PMT标志（在MAC上） */

/* MAC中断 */
#define ETH_MAC_IT_TST       ((uint32_t)0x00000200)  /* 时间戳触发中断（在MAC上） */
#define ETH_MAC_IT_MMCT      ((uint32_t)0x00000040)  /* MMC发送中断 */
#define ETH_MAC_IT_MMCR      ((uint32_t)0x00000020)  /* MMC接收中断 */
#define ETH_MAC_IT_MMC       ((uint32_t)0x00000010)  /* MMC中断（在MAC上） */
#define ETH_MAC_IT_PMT       ((uint32_t)0x00000008)  /* PMT中断（在MAC上） */

/* MAC地址索引 */
#define ETH_MAC_Address0     ((uint32_t)0x00000000)  /* MAC地址0 */
#define ETH_MAC_Address1     ((uint32_t)0x00000008)  /* MAC地址1 */
#define ETH_MAC_Address2     ((uint32_t)0x00000010)  /* MAC地址2 */
#define ETH_MAC_Address3     ((uint32_t)0x00000018)  /* MAC地址3 */

/* MAC地址过滤选择 */
#define ETH_MAC_AddressFilter_SA       ((uint32_t)0x00000000)  /* 源地址过滤 */
#define ETH_MAC_AddressFilter_DA       ((uint32_t)0x00000008)  /* 目的地址过滤 */

/* MAC地址字节掩码 */
#define ETH_MAC_AddressMask_Byte6      ((uint32_t)0x20000000)  /* 屏蔽MAC地址高寄存器位[15:8]（字节6） */
#define ETH_MAC_AddressMask_Byte5      ((uint32_t)0x10000000)  /* 屏蔽MAC地址高寄存器位[7:0]（字节5） */
#define ETH_MAC_AddressMask_Byte4      ((uint32_t)0x08000000)  /* 屏蔽MAC地址低寄存器位[31:24]（字节4） */
#define ETH_MAC_AddressMask_Byte3      ((uint32_t)0x04000000)  /* 屏蔽MAC地址低寄存器位[23:16]（字节3） */
#define ETH_MAC_AddressMask_Byte2      ((uint32_t)0x02000000)  /* 屏蔽MAC地址低寄存器位[15:8]（字节2） */
#define ETH_MAC_AddressMask_Byte1      ((uint32_t)0x01000000)  /* 屏蔽MAC地址低寄存器位[7:0]（字节1） */


/******************************************************************************/
/*                                                                            */
/*                          MAC描述符寄存器                                                                                                               */
/*                                                                            */
/******************************************************************************/

/* DMA描述符段标志 */
#define ETH_DMATxDesc_LastSegment      ((uint32_t)0x40000000)  /* 最后一个段 */
#define ETH_DMATxDesc_FirstSegment     ((uint32_t)0x20000000)  /* 第一个段 */

/* DMA描述符校验和设置 */
#define ETH_DMATxDesc_ChecksumByPass             ((uint32_t)0x00000000)   /* 校验和引擎绕过 */
#define ETH_DMATxDesc_ChecksumIPV4Header         ((uint32_t)0x00400000)   /* IPv4头部校验和插入 */
#define ETH_DMATxDesc_ChecksumTCPUDPICMPSegment  ((uint32_t)0x00800000)   /* TCP/UDP/ICMP校验和插入，伪头部校验和假定已存在 */
#define ETH_DMATxDesc_ChecksumTCPUDPICMPFull     ((uint32_t)0x00C00000)   /* TCP/UDP/ICMP校验和完全由硬件计算，包括伪头部 */

/* DMA接收和发送缓冲区选择 */
#define ETH_DMARxDesc_Buffer1     ((uint32_t)0x00000000)  /* DMA接收描述符缓冲区1 */
#define ETH_DMARxDesc_Buffer2     ((uint32_t)0x00000001)  /* DMA接收描述符缓冲区2 */


/******************************************************************************/
/*                                                                            */
/*                          ETH DMA寄存器                                     */
/*                                                                            */
/******************************************************************************/

/* DMA丢弃TCP/IP校验和错误帧使能或禁用 */
#define ETH_DropTCPIPChecksumErrorFrame_Enable   ((uint32_t)0x00000000)  /* 使能丢弃校验和错误帧 */
#define ETH_DropTCPIPChecksumErrorFrame_Disable  ((uint32_t)0x04000000)  /* 禁用丢弃校验和错误帧 */

/* DMA接收存储转发使能或禁用 */
#define ETH_ReceiveStoreForward_Enable      ((uint32_t)0x02000000)  /* 使能接收存储转发 */
#define ETH_ReceiveStoreForward_Disable     ((uint32_t)0x00000000)  /* 禁用接收存储转发 */

/* DMA刷新接收帧使能或禁用 */
#define ETH_FlushReceivedFrame_Enable       ((uint32_t)0x00000000)  /* 使能刷新接收帧 */
#define ETH_FlushReceivedFrame_Disable      ((uint32_t)0x01000000)  /* 禁用刷新接收帧 */

/* DMA发送存储转发使能或禁用 */
#define ETH_TransmitStoreForward_Enable     ((uint32_t)0x00200000)  /* 使能发送存储转发 */
#define ETH_TransmitStoreForward_Disable    ((uint32_t)0x00000000)  /* 禁用发送存储转发 */

/* DMA发送阈值控制 */
#define ETH_TransmitThresholdControl_64Bytes     ((uint32_t)0x00000000)  /* MTL发送FIFO阈值为64字节 */
#define ETH_TransmitThresholdControl_128Bytes    ((uint32_t)0x00004000)  /* MTL发送FIFO阈值为128字节 */
#define ETH_TransmitThresholdControl_192Bytes    ((uint32_t)0x00008000)  /* MTL发送FIFO阈值为192字节 */
#define ETH_TransmitThresholdControl_256Bytes    ((uint32_t)0x0000C000)  /* MTL发送FIFO阈值为256字节 */
#define ETH_TransmitThresholdControl_40Bytes     ((uint32_t)0x00010000)  /* MTL发送FIFO阈值为40字节 */
#define ETH_TransmitThresholdControl_32Bytes     ((uint32_t)0x00014000)  /* MTL发送FIFO阈值为32字节 */
#define ETH_TransmitThresholdControl_24Bytes     ((uint32_t)0x00018000)  /* MTL发送FIFO阈值为24字节 */
#define ETH_TransmitThresholdControl_16Bytes     ((uint32_t)0x0001C000)  /* MTL发送FIFO阈值为16字节 */

/* DMA转发错误帧 */
#define ETH_ForwardErrorFrames_Enable       ((uint32_t)0x00000080)  /* 使能转发错误帧 */
#define ETH_ForwardErrorFrames_Disable      ((uint32_t)0x00000000)  /* 禁用转发错误帧 */

/* DMA转发过小好帧使能或禁用 */
#define ETH_ForwardUndersizedGoodFrames_Enable   ((uint32_t)0x00000040)  /* 使能转发过小好帧 */
#define ETH_ForwardUndersizedGoodFrames_Disable  ((uint32_t)0x00000000)  /* 禁用转发过小好帧 */

/* DMA接收阈值控制 */
#define ETH_ReceiveThresholdControl_64Bytes      ((uint32_t)0x00000000)  /* MTL接收FIFO阈值为64字节 */
#define ETH_ReceiveThresholdControl_32Bytes      ((uint32_t)0x00000008)  /* MTL接收FIFO阈值为32字节 */
#define ETH_ReceiveThresholdControl_96Bytes      ((uint32_t)0x00000010)  /* MTL接收FIFO阈值为96字节 */
#define ETH_ReceiveThresholdControl_128Bytes     ((uint32_t)0x00000018)  /* MTL接收FIFO阈值为128字节 */

/* DMA第二帧操作使能或禁用 */
#define ETH_SecondFrameOperate_Enable       ((uint32_t)0x00000004)  /* 使能第二帧操作 */
#define ETH_SecondFrameOperate_Disable      ((uint32_t)0x00000000)  /* 禁用第二帧操作 */

/* 地址对齐突发使能或禁用 */
#define ETH_AddressAlignedBeats_Enable      ((uint32_t)0x02000000)  /* 使能地址对齐突发 */
#define ETH_AddressAlignedBeats_Disable     ((uint32_t)0x00000000)  /* 禁用地址对齐突发 */

/* DMA固定突发使能或禁用 */
#define ETH_FixedBurst_Enable     ((uint32_t)0x00010000)  /* 使能固定突发 */
#define ETH_FixedBurst_Disable    ((uint32_t)0x00000000)  /* 禁用固定突发 */

/* 接收DMA突发长度 */
#define ETH_RxDMABurstLength_1Beat          ((uint32_t)0x00020000)  /* 一次RxDMA事务最多传输1拍 */
#define ETH_RxDMABurstLength_2Beat          ((uint32_t)0x00040000)  /* 一次RxDMA事务最多传输2拍 */
#define ETH_RxDMABurstLength_4Beat          ((uint32_t)0x00080000)  /* 一次RxDMA事务最多传输4拍 */
#define ETH_RxDMABurstLength_8Beat          ((uint32_t)0x00100000)  /* 一次RxDMA事务最多传输8拍 */
#define ETH_RxDMABurstLength_16Beat         ((uint32_t)0x00200000)  /* 一次RxDMA事务最多传输16拍 */
#define ETH_RxDMABurstLength_32Beat         ((uint32_t)0x00400000)  /* 一次RxDMA事务最多传输32拍 */
#define ETH_RxDMABurstLength_4xPBL_4Beat    ((uint32_t)0x01020000)  /* 4倍PBL模式，一次RxDMA事务最多传输4拍 */
#define ETH_RxDMABurstLength_4xPBL_8Beat    ((uint32_t)0x01040000)  /* 4倍PBL模式，一次RxDMA事务最多传输8拍 */
#define ETH_RxDMABurstLength_4xPBL_16Beat   ((uint32_t)0x01080000)  /* 4倍PBL模式，一次RxDMA事务最多传输16拍 */
#define ETH_RxDMABurstLength_4xPBL_32Beat   ((uint32_t)0x01100000)  /* 4倍PBL模式，一次RxDMA事务最多传输32拍 */
#define ETH_RxDMABurstLength_4xPBL_64Beat   ((uint32_t)0x01200000)  /* 4倍PBL模式，一次RxDMA事务最多传输64拍 */
#define ETH_RxDMABurstLength_4xPBL_128Beat  ((uint32_t)0x01400000)  /* 4倍PBL模式，一次RxDMA事务最多传输128拍 */

/* 发送DMA突发长度 */
#define ETH_TxDMABurstLength_1Beat          ((uint32_t)0x00000100)  /* 一次TxDMA事务最多传输1拍 */
#define ETH_TxDMABurstLength_2Beat          ((uint32_t)0x00000200)  /* 一次TxDMA事务最多传输2拍 */
#define ETH_TxDMABurstLength_4Beat          ((uint32_t)0x00000400)  /* 一次TxDMA事务最多传输4拍 */
#define ETH_TxDMABurstLength_8Beat          ((uint32_t)0x00000800)  /* 一次TxDMA事务最多传输8拍 */
#define ETH_TxDMABurstLength_16Beat         ((uint32_t)0x00001000)  /* 一次TxDMA事务最多传输16拍 */
#define ETH_TxDMABurstLength_32Beat         ((uint32_t)0x00002000)  /* 一次TxDMA事务最多传输32拍 */
#define ETH_TxDMABurstLength_4xPBL_4Beat    ((uint32_t)0x01000100)  /* 4倍PBL模式，一次TxDMA事务最多传输4拍 */
#define ETH_TxDMABurstLength_4xPBL_8Beat    ((uint32_t)0x01000200)  /* 4倍PBL模式，一次TxDMA事务最多传输8拍 */
#define ETH_TxDMABurstLength_4xPBL_16Beat   ((uint32_t)0x01000400)  /* 4倍PBL模式，一次TxDMA事务最多传输16拍 */
#define ETH_TxDMABurstLength_4xPBL_32Beat   ((uint32_t)0x01000800)  /* 4倍PBL模式，一次TxDMA事务最多传输32拍 */
#define ETH_TxDMABurstLength_4xPBL_64Beat   ((uint32_t)0x01001000)  /* 4倍PBL模式，一次TxDMA事务最多传输64拍 */
#define ETH_TxDMABurstLength_4xPBL_128Beat  ((uint32_t)0x01002000)  /* 4倍PBL模式，一次TxDMA事务最多传输128拍 */

/* DMA轮询仲裁 */
#define ETH_DMAArbitration_RoundRobin_RxTx_1_1   ((uint32_t)0x00000000)  /* 轮询 Rx:Tx = 1:1 */
#define ETH_DMAArbitration_RoundRobin_RxTx_2_1   ((uint32_t)0x00004000)  /* 轮询 Rx:Tx = 2:1 */
#define ETH_DMAArbitration_RoundRobin_RxTx_3_1   ((uint32_t)0x00008000)  /* 轮询 Rx:Tx = 3:1 */
#define ETH_DMAArbitration_RoundRobin_RxTx_4_1   ((uint32_t)0x0000C000)  /* 轮询 Rx:Tx = 4:1 */
#define ETH_DMAArbitration_RxPriorTx             ((uint32_t)0x00000002)  /* Rx优先于Tx */

/* DMA中断标志 */
#define ETH_DMA_FLAG_TST               ((uint32_t)0x20000000)  /* 时间戳触发中断（在DMA上） */
#define ETH_DMA_FLAG_PMT               ((uint32_t)0x10000000)  /* PMT中断（在DMA上） */
#define ETH_DMA_FLAG_MMC               ((uint32_t)0x08000000)  /* MMC中断（在DMA上） */
#define ETH_DMA_FLAG_DataTransferError ((uint32_t)0x00800000)  /* 错误位 0-Rx DMA, 1-Tx DMA */
#define ETH_DMA_FLAG_ReadWriteError    ((uint32_t)0x01000000)  /* 错误位 0-写传输, 1-读传输 */
#define ETH_DMA_FLAG_AccessError       ((uint32_t)0x02000000)  /* 错误位 0-数据缓冲区, 1-描述符访问 */
#define ETH_DMA_FLAG_NIS               ((uint32_t)0x00010000)  /* 正常中断汇总标志 */
#define ETH_DMA_FLAG_AIS               ((uint32_t)0x00008000)  /* 异常中断汇总标志 */
#define ETH_DMA_FLAG_ER                ((uint32_t)0x00004000)  /* 早接收标志 */
#define ETH_DMA_FLAG_FBE               ((uint32_t)0x00002000)  /* 致命总线错误标志 */
#define ETH_DMA_FLAG_ET                ((uint32_t)0x00000400)  /* 早发送标志 */
#define ETH_DMA_FLAG_RWT               ((uint32_t)0x00000200)  /* 接收看门狗超时标志 */
#define ETH_DMA_FLAG_RPS               ((uint32_t)0x00000100)  /* 接收进程停止标志 */
#define ETH_DMA_FLAG_RBU               ((uint32_t)0x00000080)  /* 接收缓冲区不可用标志 */
#define ETH_DMA_FLAG_R                 ((uint32_t)0x00000040)  /* 接收标志 */
#define ETH_DMA_FLAG_TU                ((uint32_t)0x00000020)  /* 下溢标志 */
#define ETH_DMA_FLAG_RO                ((uint32_t)0x00000010)  /* 溢出标志 */
#define ETH_DMA_FLAG_TJT               ((uint32_t)0x00000008)  /* 发送超长帧超时标志 */
#define ETH_DMA_FLAG_TBU               ((uint32_t)0x00000004)  /* 发送缓冲区不可用标志 */
#define ETH_DMA_FLAG_TPS               ((uint32_t)0x00000002)  /* 发送进程停止标志 */
#define ETH_DMA_FLAG_T                 ((uint32_t)0x00000001)  /* 发送标志 */

/* DMA中断 */
#define ETH_DMA_IT_PHYLINK   ((uint32_t)0x80000000)  /* 内部PHY链接状态变化中断 */
#define ETH_DMA_IT_TST       ((uint32_t)0x20000000)  /* 时间戳触发中断（在DMA上） */
#define ETH_DMA_IT_PMT       ((uint32_t)0x10000000)  /* PMT中断（在DMA上） */
#define ETH_DMA_IT_MMC       ((uint32_t)0x08000000)  /* MMC中断（在DMA上） */
#define ETH_DMA_IT_NIS       ((uint32_t)0x00010000)  /* 正常中断汇总 */
#define ETH_DMA_IT_AIS       ((uint32_t)0x00008000)  /* 异常中断汇总 */
#define ETH_DMA_IT_ER        ((uint32_t)0x00004000)  /* 早接收中断 */
#define ETH_DMA_IT_FBE       ((uint32_t)0x00002000)  /* 致命总线错误中断 */
#define ETH_DMA_IT_ET        ((uint32_t)0x00000400)  /* 早发送中断 */
#define ETH_DMA_IT_RWT       ((uint32_t)0x00000200)  /* 接收看门狗超时中断 */
#define ETH_DMA_IT_RPS       ((uint32_t)0x00000100)  /* 接收进程停止中断 */
#define ETH_DMA_IT_RBU       ((uint32_t)0x00000080)  /* 接收缓冲区不可用中断 */
#define ETH_DMA_IT_R         ((uint32_t)0x00000040)  /* 接收中断 */
#define ETH_DMA_IT_TU        ((uint32_t)0x00000020)  /* 下溢中断 */
#define ETH_DMA_IT_RO        ((uint32_t)0x00000010)  /* 溢出中断 */
#define ETH_DMA_IT_TJT       ((uint32_t)0x00000008)  /* 发送超长帧超时中断 */
#define ETH_DMA_IT_TBU       ((uint32_t)0x00000004)  /* 发送缓冲区不可用中断 */
#define ETH_DMA_IT_TPS       ((uint32_t)0x00000002)  /* 发送进程停止中断 */
#define ETH_DMA_IT_T         ((uint32_t)0x00000001)  /* 发送中断 */

/* DMA发送进程状态 */
#define ETH_DMA_TransmitProcess_Stopped     ((uint32_t)0x00000000)  /* 停止 - 复位或发出停止发送命令 */
#define ETH_DMA_TransmitProcess_Fetching    ((uint32_t)0x00100000)  /* 运行中 - 获取发送描述符 */
#define ETH_DMA_TransmitProcess_Waiting     ((uint32_t)0x00200000)  /* 运行中 - 等待状态 */
#define ETH_DMA_TransmitProcess_Reading     ((uint32_t)0x00300000)  /* 运行中 - 从主机内存读取数据 */
#define ETH_DMA_TransmitProcess_Suspended   ((uint32_t)0x00600000)  /* 暂停 - 发送描述符不可用 */
#define ETH_DMA_TransmitProcess_Closing     ((uint32_t)0x00700000)  /* 运行中 - 关闭接收描述符 */

/* DMA接收进程状态 */
#define ETH_DMA_ReceiveProcess_Stopped      ((uint32_t)0x00000000)  /* 停止 - 复位或发出停止接收命令 */
#define ETH_DMA_ReceiveProcess_Fetching     ((uint32_t)0x00020000)  /* 运行中 - 获取接收描述符 */
#define ETH_DMA_ReceiveProcess_Waiting      ((uint32_t)0x00060000)  /* 运行中 - 等待数据包 */
#define ETH_DMA_ReceiveProcess_Suspended    ((uint32_t)0x00080000)  /* 暂停 - 接收描述符不可用 */
#define ETH_DMA_ReceiveProcess_Closing      ((uint32_t)0x000A0000)  /* 运行中 - 关闭描述符 */
#define ETH_DMA_ReceiveProcess_Queuing      ((uint32_t)0x000E0000)  /* 运行中 - 将接收帧排入主机内存 */

/* DMA溢出 */
#define ETH_DMA_Overflow_RxFIFOCounter      ((uint32_t)0x10000000)  /* FIFO溢出计数器溢出位 */
#define ETH_DMA_Overflow_MissedFrameCounter ((uint32_t)0x00010000)  /* 丢帧计数器溢出位 */


/*********************************************************************************
*                            以太网PMT定义
**********************************************************************************/

/* PMT标志 */
#define ETH_PMT_FLAG_WUFFRPR      ((uint32_t)0x80000000)  /* 唤醒帧过滤寄存器指针复位 */
#define ETH_PMT_FLAG_WUFR         ((uint32_t)0x00000040)  /* 收到唤醒帧 */
#define ETH_PMT_FLAG_MPR          ((uint32_t)0x00000020)  /* 收到魔术包 */

/*********************************************************************************
*                            以太网MMC定义
**********************************************************************************/

/* MMC发送中断标志 */
#define ETH_MMC_IT_TGF       ((uint32_t)0x00200000)  /* 当发送好帧计数器达到最大值的一半时 */
#define ETH_MMC_IT_TGFMSC    ((uint32_t)0x00008000)  /* 当发送好多冲突计数器达到最大值的一半时 */
#define ETH_MMC_IT_TGFSC     ((uint32_t)0x00004000)  /* 当发送好单次冲突计数器达到最大值的一半时 */

/* MMC接收中断标志 */
#define ETH_MMC_IT_RGUF      ((uint32_t)0x10020000)  /* 当接收好单播帧计数器达到最大值的一半时 */
#define ETH_MMC_IT_RFAE      ((uint32_t)0x10000040)  /* 当接收对齐错误计数器达到最大值的一半时 */
#define ETH_MMC_IT_RFCE      ((uint32_t)0x10000020)  /* 当接收CRC错误计数器达到最大值的一半时 */

/* MMC寄存器描述 */
#define ETH_MMCCR            ((uint32_t)0x00000100)  /* MMC控制寄存器 */
#define ETH_MMCRIR           ((uint32_t)0x00000104)  /* MMC接收中断寄存器 */
#define ETH_MMCTIR           ((uint32_t)0x00000108)  /* MMC发送中断寄存器 */
#define ETH_MMCRIMR          ((uint32_t)0x0000010C)  /* MMC接收中断屏蔽寄存器 */
#define ETH_MMCTIMR          ((uint32_t)0x00000110)  /* MMC发送中断屏蔽寄存器 */
#define ETH_MMCTGFSCCR       ((uint32_t)0x0000014C)  /* MMC发送好单次冲突计数器寄存器 */
#define ETH_MMCTGFMSCCR      ((uint32_t)0x00000150)  /* MMC发送好多冲突计数器寄存器 */
#define ETH_MMCTGFCR         ((uint32_t)0x00000168)  /* MMC发送好帧计数器寄存器 */
#define ETH_MMCRFCECR        ((uint32_t)0x00000194)  /* MMC接收CRC错误计数器寄存器 */
#define ETH_MMCRFAECR        ((uint32_t)0x00000198)  /* MMC接收对齐错误计数器寄存器 */
#define ETH_MMCRGUFCR        ((uint32_t)0x000001C4)  /* MMC接收好单播帧计数器寄存器 */


/*********************************************************************************
*                            以太网PTP定义
**********************************************************************************/

/* PTP精细更新方法或粗略更新方法 */
#define ETH_PTP_FineUpdate        ((uint32_t)0x00000001)  /* 精细更新方法 */
#define ETH_PTP_CoarseUpdate      ((uint32_t)0x00000000)  /* 粗略更新方法 */

/* PTP时间戳控制 */
#define ETH_PTP_FLAG_TSARU        ((uint32_t)0x00000020)  /* 加数寄存器更新 */
#define ETH_PTP_FLAG_TSITE        ((uint32_t)0x00000010)  /* 时间戳中断触发 */
#define ETH_PTP_FLAG_TSSTU        ((uint32_t)0x00000008)  /* 时间戳更新 */
#define ETH_PTP_FLAG_TSSTI        ((uint32_t)0x00000004)  /* 时间戳初始化 */

/* PTP正/负时间值 */
#define ETH_PTP_PositiveTime      ((uint32_t)0x00000000)  /* 正时间值 */
#define ETH_PTP_NegativeTime      ((uint32_t)0x80000000)  /* 负时间值 */


/******************************************************************************/
/*                                                                            */
/*                                PTP寄存器                                   */
/*                                                                            */
/******************************************************************************/
#define ETH_PTPTSCR     ((uint32_t)0x00000700)  /* PTP时间戳控制寄存器 */
#define ETH_PTPSSIR     ((uint32_t)0x00000704)  /* PTP亚秒递增寄存器 */
#define ETH_PTPTSHR     ((uint32_t)0x00000708)  /* PTP时间戳高寄存器 */
#define ETH_PTPTSLR     ((uint32_t)0x0000070C)  /* PTP时间戳低寄存器 */
#define ETH_PTPTSHUR    ((uint32_t)0x00000710)  /* PTP时间戳高更新寄存器 */
#define ETH_PTPTSLUR    ((uint32_t)0x00000714)  /* PTP时间戳低更新寄存器 */
#define ETH_PTPTSAR     ((uint32_t)0x00000718)  /* PTP时间戳加数寄存器 */
#define ETH_PTPTTHR     ((uint32_t)0x0000071C)  /* PTP目标时间高寄存器 */
#define ETH_PTPTTLR     ((uint32_t)0x00000720)  /* PTP目标时间低寄存器 */

/* DMA状态寄存器各字段定义（部分已在上面，这里列出以便参考） */
#define ETH_DMASR_TSTS       ((unsigned int)0x20000000)  /* 时间戳触发状态 */
#define ETH_DMASR_PMTS       ((unsigned int)0x10000000)  /* PMT状态 */
#define ETH_DMASR_MMCS       ((unsigned int)0x08000000)  /* MMC状态 */
#define ETH_DMASR_EBS        ((unsigned int)0x03800000)  /* 错误位状态 */
  #define ETH_DMASR_EBS_DescAccess      ((unsigned int)0x02000000)  /* 错误位 0-数据缓冲区, 1-描述符访问 */
  #define ETH_DMASR_EBS_ReadTransf      ((unsigned int)0x01000000)  /* 错误位 0-写传输, 1-读传输 */
  #define ETH_DMASR_EBS_DataTransfTx    ((unsigned int)0x00800000)  /* 错误位 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         ((unsigned int)0x00700000)  /* 发送进程状态 */
  #define ETH_DMASR_TPS_Stopped         ((unsigned int)0x00000000)  /* 停止 - 复位或发出停止发送命令 */
  #define ETH_DMASR_TPS_Fetching        ((unsigned int)0x00100000)  /* 运行中 - 获取发送描述符 */
  #define ETH_DMASR_TPS_Waiting         ((unsigned int)0x00200000)  /* 运行中 - 等待状态 */
  #define ETH_DMASR_TPS_Reading         ((unsigned int)0x00300000)  /* 运行中 - 从主机内存读取数据 */
  #define ETH_DMASR_TPS_Suspended       ((unsigned int)0x00600000)  /* 暂停 - 发送描述符不可用 */
  #define ETH_DMASR_TPS_Closing         ((unsigned int)0x00700000)  /* 运行中 - 关闭接收描述符 */
#define ETH_DMASR_RPS         ((unsigned int)0x000E0000)  /* 接收进程状态 */
  #define ETH_DMASR_RPS_Stopped         ((unsigned int)0x00000000)  /* 停止 - 复位或发出停止接收命令 */
  #define ETH_DMASR_RPS_Fetching        ((unsigned int)0x00020000)  /* 运行中 - 获取接收描述符 */
  #define ETH_DMASR_RPS_Waiting         ((unsigned int)0x00060000)  /* 运行中 - 等待数据包 */
  #define ETH_DMASR_RPS_Suspended       ((unsigned int)0x00080000)  /* 暂停 - 接收描述符不可用 */
  #define ETH_DMASR_RPS_Closing         ((unsigned int)0x000A0000)  /* 运行中 - 关闭描述符 */
  #define ETH_DMASR_RPS_Queuing         ((unsigned int)0x000E0000)  /* 运行中 - 将接收帧排入主机内存 */
#define ETH_DMASR_NIS        ((unsigned int)0x00010000)  /* 正常中断汇总 */
#define ETH_DMASR_AIS        ((unsigned int)0x00008000)  /* 异常中断汇总 */
#define ETH_DMASR_ERS        ((unsigned int)0x00004000)  /* 早接收状态 */
#define ETH_DMASR_FBES       ((unsigned int)0x00002000)  /* 致命总线错误状态 */
#define ETH_DMASR_ETS        ((unsigned int)0x00000400)  /* 早发送状态 */
#define ETH_DMASR_RWTS       ((unsigned int)0x00000200)  /* 接收看门狗超时状态 */
#define ETH_DMASR_RPSS       ((unsigned int)0x00000100)  /* 接收进程停止状态 */
#define ETH_DMASR_RBUS       ((unsigned int)0x00000080)  /* 接收缓冲区不可用状态 */
#define ETH_DMASR_RS         ((unsigned int)0x00000040)  /* 接收状态 */
#define ETH_DMASR_TUS        ((unsigned int)0x00000020)  /* 发送下溢状态 */
#define ETH_DMASR_ROS        ((unsigned int)0x00000010)  /* 接收溢出状态 */
#define ETH_DMASR_TJTS       ((unsigned int)0x00000008)  /* 发送超长帧超时状态 */
#define ETH_DMASR_TBUS       ((unsigned int)0x00000004)  /* 发送缓冲区不可用状态 */
#define ETH_DMASR_TPSS       ((unsigned int)0x00000002)  /* 发送进程停止状态 */
#define ETH_DMASR_TS         ((unsigned int)0x00000001)  /* 发送状态 */


/******************************************************************************/
/*                                                                            */
/*                          ETH MAC寄存器                                     */
/*                                                                            */
/******************************************************************************/
#define ETH_MACCR_WD      ((unsigned int)0x00800000)  /* 看门狗禁用 */
#define ETH_MACCR_JD      ((unsigned int)0x00400000)  /* 超长帧禁用 */
#define ETH_MACCR_IFG     ((unsigned int)0x000E0000)  /* 帧间间隔 */
#define ETH_MACCR_IFG_96Bit     ((unsigned int)0x00000000)  /* 最小帧间间隔为96位 */
   #define ETH_MACCR_IFG_88Bit     ((unsigned int)0x00020000)  /* 最小帧间间隔为88位 */
   #define ETH_MACCR_IFG_80Bit     ((unsigned int)0x00040000)  /* 最小帧间间隔为80位 */
   #define ETH_MACCR_IFG_72Bit     ((unsigned int)0x00060000)  /* 最小帧间间隔为72位 */
   #define ETH_MACCR_IFG_64Bit     ((unsigned int)0x00080000)  /* 最小帧间间隔为64位 */
   #define ETH_MACCR_IFG_56Bit     ((unsigned int)0x000A0000)  /* 最小帧间间隔为56位 */
   #define ETH_MACCR_IFG_48Bit     ((unsigned int)0x000C0000)  /* 最小帧间间隔为48位 */
   #define ETH_MACCR_IFG_40Bit     ((unsigned int)0x000E0000)  /* 最小帧间间隔为40位 */
#define ETH_MACCR_CSD     ((unsigned int)0x00010000)  /* 载波监听禁用（传输期间） */
#define ETH_MACCR_FES     ((unsigned int)0x00004000)  /* 快速以太网速度 */
#define ETH_MACCR_ROD     ((unsigned int)0x00002000)  /* 接收自己发送的帧禁用 */
#define ETH_MACCR_LM      ((unsigned int)0x00001000)  /* 环回模式 */
#define ETH_MACCR_DM      ((unsigned int)0x00000800)  /* 双工模式 */
#define ETH_MACCR_IPCO    ((unsigned int)0x00000400)  /* IP校验和卸载 */
#define ETH_MACCR_RD      ((unsigned int)0x00000200)  /* 重传禁用 */
#define ETH_MACCR_APCS    ((unsigned int)0x00000080)  /* 自动填充/CRC剥离 */
#define ETH_MACCR_BL      ((unsigned int)0x00000060)  /* 退避极限：冲突后重试前随机整数时隙延迟数：0 =< r <2^k */
   #define ETH_MACCR_BL_10    ((unsigned int)0x00000000)  /* k = min (n, 10) */
   #define ETH_MACCR_BL_8     ((unsigned int)0x00000020)  /* k = min (n, 8) */
   #define ETH_MACCR_BL_4     ((unsigned int)0x00000040)  /* k = min (n, 4) */
   #define ETH_MACCR_BL_1     ((unsigned int)0x00000060)  /* k = min (n, 1) */
#define ETH_MACCR_DC      ((unsigned int)0x00000010)  /* 推迟检查 */
#define ETH_MACCR_TE      ((unsigned int)0x00000008)  /* 发送器使能 */
#define ETH_MACCR_RE      ((unsigned int)0x00000004)  /* 接收器使能 */

#define ETH_MACFFR_RA     ((unsigned int)0x80000000)  /* 接收所有帧 */
#define ETH_MACFFR_HPF    ((unsigned int)0x00000400)  /* 哈希或完美过滤 */
#define ETH_MACFFR_SAF    ((unsigned int)0x00000200)  /* 源地址过滤使能 */
#define ETH_MACFFR_SAIF   ((unsigned int)0x00000100)  /* SA反向过滤 */
#define ETH_MACFFR_PCF    ((unsigned int)0x000000C0)  /* 控制帧传递：3种情况 */
   #define ETH_MACFFR_PCF_BlockAll                ((unsigned int)0x00000040)  /* MAC过滤所有控制帧，不传递给应用 */
   #define ETH_MACFFR_PCF_ForwardAll              ((unsigned int)0x00000080)  /* MAC将所有控制帧转发给应用，即使地址过滤失败 */
   #define ETH_MACFFR_PCF_ForwardPassedAddrFilter ((unsigned int)0x000000C0)  /* MAC转发通过地址过滤的控制帧 */
#define ETH_MACFFR_BFD    ((unsigned int)0x00000020)  /* 广播帧禁用 */
#define ETH_MACFFR_PAM    ((unsigned int)0x00000010)  /* 传递所有多播 */
#define ETH_MACFFR_DAIF   ((unsigned int)0x00000008)  /* DA反向过滤 */
#define ETH_MACFFR_HM     ((unsigned int)0x00000004)  /* 哈希多播 */
#define ETH_MACFFR_HU     ((unsigned int)0x00000002)  /* 哈希单播 */
#define ETH_MACFFR_PM     ((unsigned int)0x00000001)  /* 混杂模式 */

#define ETH_MACHTHR_HTH   ((unsigned int)0xFFFFFFFF)  /* 哈希表高 */
#define ETH_MACHTLR_HTL   ((unsigned int)0xFFFFFFFF)  /* 哈希表低 */

#define ETH_MACMIIAR_PA   ((unsigned int)0x0000F800)  /* 物理层地址 */
#define ETH_MACMIIAR_MR   ((unsigned int)0x000007C0)  /* 所选PHY中的MII寄存器 */
#define ETH_MACMIIAR_CR   ((unsigned int)0x0000001C)  /* CR时钟范围：6种情况 */
   #define ETH_MACMIIAR_CR_Div42   ((unsigned int)0x00000000)  /* HCLK:60-100 MHz; MDC时钟= HCLK/42 */
   #define ETH_MACMIIAR_CR_Div16   ((unsigned int)0x00000008)  /* HCLK:20-35 MHz; MDC时钟= HCLK/16 */
   #define ETH_MACMIIAR_CR_Div26   ((unsigned int)0x0000000C)  /* HCLK:35-60 MHz; MDC时钟= HCLK/26 */
#define ETH_MACMIIAR_MW   ((unsigned int)0x00000002)  /* MII写 */
#define ETH_MACMIIAR_MB   ((unsigned int)0x00000001)  /* MII忙 */
#define ETH_MACMIIDR_MD   ((unsigned int)0x0000FFFF)  /* MII数据：从/向PHY读/写的数据 */
#define ETH_MACFCR_PT     ((unsigned int)0xFFFF0000)  /* 暂停时间 */
#define ETH_MACFCR_ZQPD   ((unsigned int)0x00000080)  /* 零量子暂停禁用 */
#define ETH_MACFCR_PLT    ((unsigned int)0x00000030)  /* 暂停低阈值：4种情况 */
   #define ETH_MACFCR_PLT_Minus4   ((unsigned int)0x00000000)  /* 暂停时间减4个时隙 */
   #define ETH_MACFCR_PLT_Minus28  ((unsigned int)0x00000010)  /* 暂停时间减28个时隙 */
   #define ETH_MACFCR_PLT_Minus144 ((unsigned int)0x00000020)  /* 暂停时间减144个时隙 */
   #define ETH_MACFCR_PLT_Minus256 ((unsigned int)0x00000030)  /* 暂停时间减256个时隙 */
#define ETH_MACFCR_UPFD   ((unsigned int)0x00000008)  /* 单播暂停帧检测 */
#define ETH_MACFCR_RFCE   ((unsigned int)0x00000004)  /* 接收流控制使能 */
#define ETH_MACFCR_TFCE   ((unsigned int)0x00000002)  /* 发送流控制使能 */
#define ETH_MACFCR_FCBBPA ((unsigned int)0x00000001)  /* 流控制忙/背压激活 */

#define ETH_MACVLANTR_VLANTC ((unsigned int)0x00010000)  /* 12位VLAN标签比较 */
#define ETH_MACVLANTR_VLANTI ((unsigned int)0x0000FFFF)  /* VLAN标签标识符（用于接收帧） */

#define ETH_MACRWUFFR_D   ((unsigned int)0xFFFFFFFF)  /* 唤醒帧过滤寄存器数据 */
/* 连续8次写入此地址（偏移0x28）将写入所有唤醒帧过滤寄存器。
   连续8次读取此地址将读取所有唤醒帧过滤寄存器。 */

/*
唤醒帧过滤寄存器0：过滤器0字节掩码
唤醒帧过滤寄存器1：过滤器1字节掩码
唤醒帧过滤寄存器2：过滤器2字节掩码
唤醒帧过滤寄存器3：过滤器3字节掩码
唤醒帧过滤寄存器4：保留 - 过滤器3命令 - 保留 - 过滤器2命令 -
                    保留 - 过滤器1命令 - 保留 - 过滤器0命令
唤醒帧过滤寄存器5：过滤器3偏移 - 过滤器2偏移 - 过滤器1偏移 - 过滤器0偏移
唤醒帧过滤寄存器6：过滤器1 CRC16 - 过滤器0 CRC16
唤醒帧过滤寄存器7：过滤器3 CRC16 - 过滤器2 CRC16 */

#define ETH_MACPMTCSR_WFFRPR ((unsigned int)0x80000000)  /* 唤醒帧过滤寄存器指针复位 */
#define ETH_MACPMTCSR_GU     ((unsigned int)0x00000200)  /* 全局单播 */
#define ETH_MACPMTCSR_WFR    ((unsigned int)0x00000040)  /* 收到唤醒帧 */
#define ETH_MACPMTCSR_MPR    ((unsigned int)0x00000020)  /* 收到魔术包 */
#define ETH_MACPMTCSR_WFE    ((unsigned int)0x00000004)  /* 唤醒帧使能 */
#define ETH_MACPMTCSR_MPE    ((unsigned int)0x00000002)  /* 魔术包使能 */
#define ETH_MACPMTCSR_PD     ((unsigned int)0x00000001)  /* 掉电 */

#define ETH_MACSR_TSTS      ((unsigned int)0x00000200)  /* 时间戳触发状态 */
#define ETH_MACSR_MMCTS     ((unsigned int)0x00000040)  /* MMC发送状态 */
#define ETH_MACSR_MMMCRS    ((unsigned int)0x00000020)  /* MMC接收状态 */
#define ETH_MACSR_MMCS      ((unsigned int)0x00000010)  /* MMC状态 */
#define ETH_MACSR_PMTS      ((unsigned int)0x00000008)  /* PMT状态 */

#define ETH_MACIMR_TSTIM     ((unsigned int)0x00000200)  /* 时间戳触发中断屏蔽 */
#define ETH_MACIMR_PMTIM     ((unsigned int)0x00000008)  /* PMT中断屏蔽 */

#define ETH_MACA0HR_MACA0H   ((unsigned int)0x0000FFFF)  /* MAC地址0高 */

#define ETH_MACA0LR_MACA0L   ((unsigned int)0xFFFFFFFF)  /* MAC地址0低 */

#define ETH_MACA1HR_AE       ((unsigned int)0x80000000)  /* 地址使能 */
#define ETH_MACA1HR_SA       ((unsigned int)0x40000000)  /* 源地址 */
#define ETH_MACA1HR_MBC      ((unsigned int)0x3F000000)  /* 字节掩码控制：用于比较MAC地址字节的掩码位 */
   #define ETH_MACA1HR_MBC_HBits15_8    ((unsigned int)0x20000000)  /* 屏蔽MAC地址高寄存器位[15:8] */
   #define ETH_MACA1HR_MBC_HBits7_0     ((unsigned int)0x10000000)  /* 屏蔽MAC地址高寄存器位[7:0] */
   #define ETH_MACA1HR_MBC_LBits31_24   ((unsigned int)0x08000000)  /* 屏蔽MAC地址低寄存器位[31:24] */
   #define ETH_MACA1HR_MBC_LBits23_16   ((unsigned int)0x04000000)  /* 屏蔽MAC地址低寄存器位[23:16] */
   #define ETH_MACA1HR_MBC_LBits15_8    ((unsigned int)0x02000000)  /* 屏蔽MAC地址低寄存器位[15:8] */
   #define ETH_MACA1HR_MBC_LBits7_0     ((unsigned int)0x01000000)  /* 屏蔽MAC地址低寄存器位[7:0] */
#define ETH_MACA1HR_MACA1H   ((unsigned int)0x0000FFFF)  /* MAC地址1高 */

#define ETH_MACA1LR_MACA1L   ((unsigned int)0xFFFFFFFF)  /* MAC地址1低 */

#define ETH_MACA2HR_AE       ((unsigned int)0x80000000)  /* 地址使能 */
#define ETH_MACA2HR_SA       ((unsigned int)0x40000000)  /* 源地址 */
#define ETH_MACA2HR_MBC      ((unsigned int)0x3F000000)  /* 字节掩码控制 */
   #define ETH_MACA2HR_MBC_HBits15_8    ((unsigned int)0x20000000)  /* 屏蔽MAC地址高寄存器位[15:8] */
   #define ETH_MACA2HR_MBC_HBits7_0     ((unsigned int)0x10000000)  /* 屏蔽MAC地址高寄存器位[7:0] */
   #define ETH_MACA2HR_MBC_LBits31_24   ((unsigned int)0x08000000)  /* 屏蔽MAC地址低寄存器位[31:24] */
   #define ETH_MACA2HR_MBC_LBits23_16   ((unsigned int)0x04000000)  /* 屏蔽MAC地址低寄存器位[23:16] */
   #define ETH_MACA2HR_MBC_LBits15_8    ((unsigned int)0x02000000)  /* 屏蔽MAC地址低寄存器位[15:8] */
   #define ETH_MACA2HR_MBC_LBits7_0     ((unsigned int)0x01000000)  /* 屏蔽MAC地址低寄存器位[70] */

#define ETH_MACA2HR_MACA2H   ((unsigned int)0x0000FFFF)  /* MAC地址2高 */
#define ETH_MACA2LR_MACA2L   ((unsigned int)0xFFFFFFFF)  /* MAC地址2低 */

#define ETH_MACA3HR_AE       ((unsigned int)0x80000000)  /* 地址使能 */
#define ETH_MACA3HR_SA       ((unsigned int)0x40000000)  /* 源地址 */
#define ETH_MACA3HR_MBC      ((unsigned int)0x3F000000)  /* 字节掩码控制 */
   #define ETH_MACA3HR_MBC_HBits15_8    ((unsigned int)0x20000000)  /* 屏蔽MAC地址高寄存器位[15:8] */
   #define ETH_MACA3HR_MBC_HBits7_0     ((unsigned int)0x10000000)  /* 屏蔽MAC地址高寄存器位[7:0] */
   #define ETH_MACA3HR_MBC_LBits31_24   ((unsigned int)0x08000000)  /* 屏蔽MAC地址低寄存器位[31:24] */
   #define ETH_MACA3HR_MBC_LBits23_16   ((unsigned int)0x04000000)  /* 屏蔽MAC地址低寄存器位[23:16] */
   #define ETH_MACA3HR_MBC_LBits15_8    ((unsigned int)0x02000000)  /* 屏蔽MAC地址低寄存器位[15:8] */
   #define ETH_MACA3HR_MBC_LBits7_0     ((unsigned int)0x01000000)  /* 屏蔽MAC地址低寄存器位[70] */
#define ETH_MACA3HR_MACA3H   ((unsigned int)0x0000FFFF)  /* MAC地址3高 */
#define ETH_MACA3LR_MACA3L   ((unsigned int)0xFFFFFFFF)  /* MAC地址3低 */

/******************************************************************************/
/*                                                                            */
/*                          ETH MMC寄存器                                     */
/*                                                                            */
/******************************************************************************/
#define ETH_MMCCR_MCFHP      ((unsigned int)0x00000020)  /* MMC计数器全半预设 */
#define ETH_MMCCR_MCP        ((unsigned int)0x00000010)  /* MMC计数器预设 */
#define ETH_MMCCR_MCF        ((unsigned int)0x00000008)  /* MMC计数器冻结 */
#define ETH_MMCCR_ROR        ((unsigned int)0x00000004)  /* 读时复位 */
#define ETH_MMCCR_CSR        ((unsigned int)0x00000002)  /* 计数器停止回滚 */
#define ETH_MMCCR_CR         ((unsigned int)0x00000001)  /* 计数器复位 */

#define ETH_MMCRIR_RGUFS     ((unsigned int)0x00020000)  /* 接收好单播帧计数器达到最大值的一半时置位 */
#define ETH_MMCRIR_RFAES     ((unsigned int)0x00000040)  /* 接收对齐错误计数器达到最大值的一半时置位 */
#define ETH_MMCRIR_RFCES     ((unsigned int)0x00000020)  /* 接收CRC错误计数器达到最大值的一半时置位 */

#define ETH_MMCTIR_TGFS      ((unsigned int)0x00200000)  /* 发送好帧计数器达到最大值的一半时置位 */
#define ETH_MMCTIR_TGFMSCS   ((unsigned int)0x00008000)  /* 发送好多冲突计数器达到最大值的一半时置位 */
#define ETH_MMCTIR_TGFSCS    ((unsigned int)0x00004000)  /* 发送好单次冲突计数器达到最大值的一半时置位 */

#define ETH_MMCRIMR_RGUFM    ((unsigned int)0x00020000)  /* 屏蔽接收好单播帧计数器达到一半的中断 */
#define ETH_MMCRIMR_RFAEM    ((unsigned int)0x00000040)  /* 屏蔽接收对齐错误计数器达到一半的中断 */
#define ETH_MMCRIMR_RFCEM    ((unsigned int)0x00000020)  /* 屏蔽接收CRC错误计数器达到一半的中断 */

#define ETH_MMCTIMR_TGFM     ((unsigned int)0x00200000)  /* 屏蔽发送好帧计数器达到一半的中断 */
#define ETH_MMCTIMR_TGFMSCM  ((unsigned int)0x00008000)  /* 屏蔽发送好多冲突计数器达到一半的中断 */
#define ETH_MMCTIMR_TGFSCM   ((unsigned int)0x00004000)  /* 屏蔽发送好单次冲突计数器达到一半的中断 */

#define ETH_MMCTGFSCCR_TGFSCC     ((unsigned int)0xFFFFFFFF)  /* 半双工模式下单次冲突后成功发送的帧数 */

#define ETH_MMCTGFMSCCR_TGFMSCC   ((unsigned int)0xFFFFFFFF)  /* 半双工模式下多次冲突后成功发送的帧数 */

#define ETH_MMCTGFCR_TGFC    ((unsigned int)0xFFFFFFFF)  /* 成功发送的好帧数 */

#define ETH_MMCRFCECR_RFCEC  ((unsigned int)0xFFFFFFFF)  /* 接收到的CRC错误帧数 */

#define ETH_MMCRFAECR_RFAEC  ((unsigned int)0xFFFFFFFF)  /* 接收到的对齐（散位）错误帧数 */

#define ETH_MMCRGUFCR_RGUFC  ((unsigned int)0xFFFFFFFF)  /* 接收到的良好单播帧数 */


/******************************************************************************/
/*                                                                            */
/*                          ETH精确时钟协议寄存器                             */
/*                                                                            */
/******************************************************************************/
#define ETH_PTPTSCR_TSCNT       ((unsigned int)0x00030000)  /* 时间戳时钟节点类型 */
#define ETH_PTPTSSR_TSSMRME     ((unsigned int)0x00008000)  /* 为主相关消息启用时间戳快照 */
#define ETH_PTPTSSR_TSSEME      ((unsigned int)0x00004000)  /* 为事件消息启用时间戳快照 */
#define ETH_PTPTSSR_TSSIPV4FE   ((unsigned int)0x00002000)  /* 为IPv4帧启用时间戳快照 */
#define ETH_PTPTSSR_TSSIPV6FE   ((unsigned int)0x00001000)  /* 为IPv6帧启用时间戳快照 */
#define ETH_PTPTSSR_TSSPTPOEFE  ((unsigned int)0x00000800)  /* 为以太网上的PTP帧启用时间戳快照 */
#define ETH_PTPTSSR_TSPTPPSV2E  ((unsigned int)0x00000400)  /* 为版本2格式启用PTP包探测 */
#define ETH_PTPTSSR_TSSSR       ((unsigned int)0x00000200)  /* 时间戳亚秒回滚 */
#define ETH_PTPTSSR_TSSARFE     ((unsigned int)0x00000100)  /* 为所有接收帧启用时间戳快照 */

#define ETH_PTPTSCR_TSARU    ((unsigned int)0x00000020)  /* 加数寄存器更新 */
#define ETH_PTPTSCR_TSITE    ((unsigned int)0x00000010)  /* 时间戳中断触发使能 */
#define ETH_PTPTSCR_TSSTU    ((unsigned int)0x00000008)  /* 时间戳更新 */
#define ETH_PTPTSCR_TSSTI    ((unsigned int)0x00000004)  /* 时间戳初始化 */
#define ETH_PTPTSCR_TSFCU    ((unsigned int)0x00000002)  /* 时间戳精细或粗略更新 */
#define ETH_PTPTSCR_TSE      ((unsigned int)0x00000001)  /* 时间戳使能 */

#define ETH_PTPSSIR_STSSI    ((unsigned int)0x000000FF)  /* 系统时间亚秒递增值 */

#define ETH_PTPTSHR_STS      ((unsigned int)0xFFFFFFFF)  /* 系统时间秒 */

#define ETH_PTPTSLR_STPNS    ((unsigned int)0x80000000)  /* 系统时间正或负 */
#define ETH_PTPTSLR_STSS     ((unsigned int)0x7FFFFFFF)  /* 系统时间亚秒 */

#define ETH_PTPTSHUR_TSUS    ((unsigned int)0xFFFFFFFF)  /* 时间戳更新秒 */

#define ETH_PTPTSLUR_TSUPNS  ((unsigned int)0x80000000)  /* 时间戳更新正或负 */
#define ETH_PTPTSLUR_TSUSS   ((unsigned int)0x7FFFFFFF)  /* 时间戳更新亚秒 */

#define ETH_PTPTSAR_TSA      ((unsigned int)0xFFFFFFFF)  /* 时间戳加数 */

#define ETH_PTPTTHR_TTSH     ((unsigned int)0xFFFFFFFF)  /* 目标时间戳高 */

#define ETH_PTPTTLR_TTSL     ((unsigned int)0xFFFFFFFF)  /* 目标时间戳低 */

#define ETH_PTPTSSR_TSTTR    ((unsigned int)0x00000020)  /* 时间戳目标时间到达 */
#define ETH_PTPTSSR_TSSO     ((unsigned int)0x00000010)  /* 时间戳秒溢出 */

/******************************************************************************/
/*                                                                            */
/*                       ETH DMA寄存器                                        */
/*                                                                            */
/******************************************************************************/
#define ETH_DMABMR_AAB       ((unsigned int)0x02000000)  /* 地址对齐突发 */
#define ETH_DMABMR_FPM        ((unsigned int)0x01000000)  /* 4xPBL模式 */
#define ETH_DMABMR_USP       ((unsigned int)0x00800000)  /* 使用独立的PBL */
#define ETH_DMABMR_RDP       ((unsigned int)0x007E0000)  /* RxDMA PBL */
   #define ETH_DMABMR_RDP_1Beat    ((unsigned int)0x00020000)  /* 一次RxDMA事务最多传输1拍 */
   #define ETH_DMABMR_RDP_2Beat    ((unsigned int)0x00040000)  /* 一次RxDMA事务最多传输2拍 */
   #define ETH_DMABMR_RDP_4Beat    ((unsigned int)0x00080000)  /* 一次RxDMA事务最多传输4拍 */
   #define ETH_DMABMR_RDP_8Beat    ((unsigned int)0x00100000)  /* 一次RxDMA事务最多传输8拍 */
   #define ETH_DMABMR_RDP_16Beat   ((unsigned int)0x00200000)  /* 一次RxDMA事务最多传输16拍 */
   #define ETH_DMABMR_RDP_32Beat   ((unsigned int)0x00400000)  /* 一次RxDMA事务最多传输32拍 */
   #define ETH_DMABMR_RDP_4xPBL_4Beat   ((unsigned int)0x01020000)  /* 4倍PBL模式，一次RxDMA事务最多传输4拍 */
   #define ETH_DMABMR_RDP_4xPBL_8Beat   ((unsigned int)0x01040000)  /* 4倍PBL模式，一次RxDMA事务最多传输8拍 */
   #define ETH_DMABMR_RDP_4xPBL_16Beat  ((unsigned int)0x01080000)  /* 4倍PBL模式，一次RxDMA事务最多传输16拍 */
   #define ETH_DMABMR_RDP_4xPBL_32Beat  ((unsigned int)0x01100000)  /* 4倍PBL模式，一次RxDMA事务最多传输32拍 */
   #define ETH_DMABMR_RDP_4xPBL_64Beat  ((unsigned int)0x01200000)  /* 4倍PBL模式，一次RxDMA事务最多传输64拍 */
   #define ETH_DMABMR_RDP_4xPBL_128Beat ((unsigned int)0x01400000)  /* 4倍PBL模式，一次RxDMA事务最多传输128拍 */
#define ETH_DMABMR_FB        ((unsigned int)0x00010000)  /* 固定突发 */
#define ETH_DMABMR_RTPR      ((unsigned int)0x0000C000)  /* Rx Tx优先级比 */
   #define ETH_DMABMR_RTPR_1_1     ((unsigned int)0x00000000)  /* Rx Tx优先级比 */
   #define ETH_DMABMR_RTPR_2_1     ((unsigned int)0x00004000)  /* Rx Tx优先级比 */
   #define ETH_DMABMR_RTPR_3_1     ((unsigned int)0x00008000)  /* Rx Tx优先级比 */
   #define ETH_DMABMR_RTPR_4_1     ((unsigned int)0x0000C000)  /* Rx Tx优先级比 */
#define ETH_DMABMR_PBL    ((unsigned int)0x00003F00)  /* 可编程突发长度 */
   #define ETH_DMABMR_PBL_1Beat    ((unsigned int)0x00000100)  /* 一次TxDMA事务最多传输1拍 */
   #define ETH_DMABMR_PBL_2Beat    ((unsigned int)0x00000200)  /* 一次TxDMA事务最多传输2拍 */
   #define ETH_DMABMR_PBL_4Beat    ((unsigned int)0x00000400)  /* 一次TxDMA事务最多传输4拍 */
   #define ETH_DMABMR_PBL_8Beat    ((unsigned int)0x00000800)  /* 一次TxDMA事务最多传输8拍 */
   #define ETH_DMABMR_PBL_16Beat   ((unsigned int)0x00001000)  /* 一次TxDMA事务最多传输16拍 */
   #define ETH_DMABMR_PBL_32Beat   ((unsigned int)0x00002000)  /* 一次TxDMA事务最多传输32拍 */
   #define ETH_DMABMR_PBL_4xPBL_4Beat   ((unsigned int)0x01000100)  /* 4倍PBL模式，一次TxDMA事务最多传输4拍 */
   #define ETH_DMABMR_PBL_4xPBL_8Beat   ((unsigned int)0x01000200)  /* 4倍PBL模式，一次TxDMA事务最多传输8拍 */
   #define ETH_DMABMR_PBL_4xPBL_16Beat  ((unsigned int)0x01000400)  /* 4倍PBL模式，一次TxDMA事务最多传输16拍 */
   #define ETH_DMABMR_PBL_4xPBL_32Beat  ((unsigned int)0x01000800)  /* 4倍PBL模式，一次TxDMA事务最多传输32拍 */
   #define ETH_DMABMR_PBL_4xPBL_64Beat  ((unsigned int)0x01001000)  /* 4倍PBL模式，一次TxDMA事务最多传输64拍 */
   #define ETH_DMABMR_PBL_4xPBL_128Beat ((unsigned int)0x01002000)  /* 4倍PBL模式，一次TxDMA事务最多传输128拍 */
#define ETH_DMABMR_EDE       ((unsigned int)0x00000080)  /* 增强描述符使能 */
#define ETH_DMABMR_DSL       ((unsigned int)0x0000007C)  /* 描述符跳过长 */
#define ETH_DMABMR_DA        ((unsigned int)0x00000002)  /* DMA仲裁方案 */
#define ETH_DMABMR_SR        ((unsigned int)0x00000001)  /* 软件复位 */

#define ETH_DMATPDR_TPD      ((unsigned int)0xFFFFFFFF)  /* 发送轮询请求 */

#define ETH_DMARPDR_RPD      ((unsigned int)0xFFFFFFFF)  /* 接收轮询请求 */

#define ETH_DMARDLAR_SRL     ((unsigned int)0xFFFFFFFF)  /* 接收列表起始地址 */

#define ETH_DMATDLAR_STL     ((unsigned int)0xFFFFFFFF)  /* 发送列表起始地址 */

/* 以下为重复定义，已在上面列出，但为保持完整性，保留 */
#define ETH_DMASR_TSTS       ((unsigned int)0x20000000)  /* 时间戳触发状态 */
#define ETH_DMASR_PMTS       ((unsigned int)0x10000000)  /* PMT状态 */
#define ETH_DMASR_MMCS       ((unsigned int)0x08000000)  /* MMC状态 */
#define ETH_DMASR_EBS        ((unsigned int)0x03800000)  /* 错误位状态 */
   #define ETH_DMASR_EBS_DescAccess      ((unsigned int)0x02000000)  /* 错误位 0-数据缓冲区, 1-描述符访问 */
   #define ETH_DMASR_EBS_ReadTransf      ((unsigned int)0x01000000)  /* 错误位 0-写传输, 1-读传输 */
   #define ETH_DMASR_EBS_DataTransfTx    ((unsigned int)0x00800000)  /* 错误位 0-Rx DMA, 1-Tx DMA */
#define ETH_DMASR_TPS         ((unsigned int)0x00700000)  /* 发送进程状态 */
   #define ETH_DMASR_TPS_Stopped         ((unsigned int)0x00000000)  /* 停止 - 复位或发出停止发送命令 */
   #define ETH_DMASR_TPS_Fetching        ((unsigned int)0x00100000)  /* 运行中 - 获取发送描述符 */
   #define ETH_DMASR_TPS_Waiting         ((unsigned int)0x00200000)  /* 运行中 - 等待状态 */
   #define ETH_DMASR_TPS_Reading         ((unsigned int)0x00300000)  /* 运行中 - 从主机内存读取数据 */
   #define ETH_DMASR_TPS_Suspended       ((unsigned int)0x00600000)  /* 暂停 - 发送描述符不可用 */
   #define ETH_DMASR_TPS_Closing         ((unsigned int)0x00700000)  /* 运行中 - 关闭接收描述符 */
#define ETH_DMASR_RPS         ((unsigned int)0x000E0000)  /* 接收进程状态 */
   #define ETH_DMASR_RPS_Stopped         ((unsigned int)0x00000000)  /* 停止 - 复位或发出停止接收命令 */
   #define ETH_DMASR_RPS_Fetching        ((unsigned int)0x00020000)  /* 运行中 - 获取接收描述符 */
   #define ETH_DMASR_RPS_Waiting         ((unsigned int)0x00060000)  /* 运行中 - 等待数据包 */
   #define ETH_DMASR_RPS_Suspended       ((unsigned int)0x00080000)  /* 暂停 - 接收描述符不可用 */
   #define ETH_DMASR_RPS_Closing         ((unsigned int)0x000A0000)  /* 运行中 - 关闭描述符 */
   #define ETH_DMASR_RPS_Queuing         ((unsigned int)0x000E0000)  /* 运行中 - 将接收帧排入主机内存 */
#define ETH_DMASR_NIS        ((unsigned int)0x00010000)  /* 正常中断汇总 */
#define ETH_DMASR_AIS        ((unsigned int)0x00008000)  /* 异常中断汇总 */
#define ETH_DMASR_ERS        ((unsigned int)0x00004000)  /* 早接收状态 */
#define ETH_DMASR_FBES       ((unsigned int)0x00002000)  /* 致命总线错误状态 */
#define ETH_DMASR_ETS        ((unsigned int)0x00000400)  /* 早发送状态 */
#define ETH_DMASR_RWTS       ((unsigned int)0x00000200)  /* 接收看门狗超时状态 */
#define ETH_DMASR_RPSS       ((unsigned int)0x00000100)  /* 接收进程停止状态 */
#define ETH_DMASR_RBUS       ((unsigned int)0x00000080)  /* 接收缓冲区不可用状态 */
#define ETH_DMASR_RS         ((unsigned int)0x00000040)  /* 接收状态 */
#define ETH_DMASR_TUS        ((unsigned int)0x00000020)  /* 发送下溢状态 */
#define ETH_DMASR_ROS        ((unsigned int)0x00000010)  /* 接收溢出状态 */
#define ETH_DMASR_TJTS       ((unsigned int)0x00000008)  /* 发送超长帧超时状态 */
#define ETH_DMASR_TBUS       ((unsigned int)0x00000004)  /* 发送缓冲区不可用状态 */
#define ETH_DMASR_TPSS       ((unsigned int)0x00000002)  /* 发送进程停止状态 */
#define ETH_DMASR_TS         ((unsigned int)0x00000001)  /* 发送状态 */

#define ETH_DMAOMR_DTCEFD    ((unsigned int)0x04000000)  /* 禁用丢弃TCP/IP校验和错误帧 */
#define ETH_DMAOMR_RSF       ((unsigned int)0x02000000)  /* 接收存储转发 */
#define ETH_DMAOMR_DFRF      ((unsigned int)0x01000000)  /* 禁用刷新接收帧 */
#define ETH_DMAOMR_TSF       ((unsigned int)0x00200000)  /* 发送存储转发 */
#define ETH_DMAOMR_FTF       ((unsigned int)0x00100000)  /* 刷新发送FIFO */
#define ETH_DMAOMR_TTC       ((unsigned int)0x0001C000)  /* 发送阈值控制 */
   #define ETH_DMAOMR_TTC_64Bytes       ((unsigned int)0x00000000)  /* MTL发送FIFO阈值为64字节 */
   #define ETH_DMAOMR_TTC_128Bytes      ((unsigned int)0x00004000)  /* MTL发送FIFO阈值为128字节 */
   #define ETH_DMAOMR_TTC_192Bytes      ((unsigned int)0x00008000)  /* MTL发送FIFO阈值为192字节 */
   #define ETH_DMAOMR_TTC_256Bytes      ((unsigned int)0x0000C000)  /* MTL发送FIFO阈值为256字节 */
   #define ETH_DMAOMR_TTC_40Bytes       ((unsigned int)0x00010000)  /* MTL发送FIFO阈值为40字节 */
   #define ETH_DMAOMR_TTC_32Bytes       ((unsigned int)0x00014000)  /* MTL发送FIFO阈值为32字节 */
   #define ETH_DMAOMR_TTC_24Bytes       ((unsigned int)0x00018000)  /* MTL发送FIFO阈值为24字节 */
   #define ETH_DMAOMR_TTC_16Bytes       ((unsigned int)0x0001C000)  /* MTL发送FIFO阈值为16字节 */
#define ETH_DMAOMR_ST        ((unsigned int)0x00002000)  /* 启动/停止发送命令 */
#define ETH_DMAOMR_FEF       ((unsigned int)0x00000080)  /* 转发错误帧 */
#define ETH_DMAOMR_FUGF      ((unsigned int)0x00000040)  /* 转发过小好帧 */
#define ETH_DMAOMR_RTC       ((unsigned int)0x00000018)  /* 接收阈值控制 */
   #define ETH_DMAOMR_RTC_64Bytes       ((unsigned int)0x00000000)  /* MTL接收FIFO阈值为64字节 */
   #define ETH_DMAOMR_RTC_32Bytes       ((unsigned int)0x00000008)  /* MTL接收FIFO阈值为32字节 */
   #define ETH_DMAOMR_RTC_96Bytes       ((unsigned int)0x00000010)  /* MTL接收FIFO阈值为96字节 */
   #define ETH_DMAOMR_RTC_128Bytes      ((unsigned int)0x00000018)  /* MTL接收FIFO阈值为128字节 */
#define ETH_DMAOMR_OSF       ((unsigned int)0x00000004)  /* 第二帧操作 */
#define ETH_DMAOMR_SR        ((unsigned int)0x00000002)  /* 启动/停止接收 */

#define ETH_DMAIER_NISE      ((unsigned int)0x00010000)  /* 正常中断汇总使能 */
#define ETH_DMAIER_AISE      ((unsigned int)0x00008000)  /* 异常中断汇总使能 */
#define ETH_DMAIER_ERIE      ((unsigned int)0x00004000)  /* 早接收中断使能 */
#define ETH_DMAIER_FBEIE     ((unsigned int)0x00002000)  /* 致命总线错误中断使能 */
#define ETH_DMAIER_ETIE      ((unsigned int)0x00000400)  /* 早发送中断使能 */
#define ETH_DMAIER_RWTIE     ((unsigned int)0x00000200)  /* 接收看门狗超时中断使能 */
#define ETH_DMAIER_RPSIE     ((unsigned int)0x00000100)  /* 接收进程停止中断使能 */
#define ETH_DMAIER_RBUIE     ((unsigned int)0x00000080)  /* 接收缓冲区不可用中断使能 */
#define ETH_DMAIER_RIE       ((unsigned int)0x00000040)  /* 接收中断使能 */
#define ETH_DMAIER_TUIE      ((unsigned int)0x00000020)  /* 发送下溢中断使能 */
#define ETH_DMAIER_ROIE      ((unsigned int)0x00000010)  /* 接收溢出中断使能 */
#define ETH_DMAIER_TJTIE     ((unsigned int)0x00000008)  /* 发送超长帧超时中断使能 */
#define ETH_DMAIER_TBUIE     ((unsigned int)0x00000004)  /* 发送缓冲区不可用中断使能 */
#define ETH_DMAIER_TPSIE     ((unsigned int)0x00000002)  /* 发送进程停止中断使能 */
#define ETH_DMAIER_TIE       ((unsigned int)0x00000001)  /* 发送中断使能 */

#define ETH_DMAMFBOCR_OFOC   ((unsigned int)0x10000000)  /* FIFO溢出计数器溢出位 */
#define ETH_DMAMFBOCR_MFA    ((unsigned int)0x0FFE0000)  /* 应用程序丢帧数 */
#define ETH_DMAMFBOCR_OMFC   ((unsigned int)0x00010000)  /* 丢帧计数器溢出位 */
#define ETH_DMAMFBOCR_MFC    ((unsigned int)0x0000FFFF)  /* 控制器丢帧数 */

#define ETH_DMACHTDR_HTDAP   ((unsigned int)0xFFFFFFFF)  /* 主机发送描述符地址指针 */
#define ETH_DMACHRDR_HRDAP   ((unsigned int)0xFFFFFFFF)  /* 主机接收描述符地址指针 */
#define ETH_DMACHTBAR_HTBAP  ((unsigned int)0xFFFFFFFF)  /* 主机发送缓冲区地址指针 */
#define ETH_DMACHRBAR_HRBAP  ((unsigned int)0xFFFFFFFF)  /* 主机接收缓冲区地址指针 */


#define ETH_MAC_ADDR_HBASE   (ETH_MAC_BASE + 0x40)   /* 以太网MAC地址高寄存器基地址 */
#define ETH_MAC_ADDR_LBASE    (ETH_MAC_BASE + 0x44)  /* 以太网MAC地址低寄存器基地址 */

/* 以太网MACMIIAR寄存器掩码 */
#define MACMIIAR_CR_MASK    ((uint32_t)0xFFFFFFE3)

/* 以太网MACCR寄存器掩码 */
#define MACCR_CLEAR_MASK    ((uint32_t)0xFF20810F)

/* 以太网MACFCR寄存器掩码 */
#define MACFCR_CLEAR_MASK   ((uint32_t)0x0000FF41)

/* 以太网DMAOMR寄存器掩码 */
#define DMAOMR_CLEAR_MASK   ((uint32_t)0xF8DE3F23)

/* 以太网远程唤醒帧寄存器长度 */
#define ETH_WAKEUP_REGISTER_LENGTH      8

/* 以太网丢帧计数器移位 */
#define  ETH_DMA_RX_OVERFLOW_MISSEDFRAMES_COUNTERSHIFT     17

/* 以太网DMA发送描述符冲突计数移位 */
#define  ETH_DMATXDESC_COLLISION_COUNTSHIFT        3

/* 以太网DMA发送描述符缓冲区2大小移位 */
#define  ETH_DMATXDESC_BUFFER2_SIZESHIFT           16

/* 以太网DMA接收描述符帧长度移位 */
#define  ETH_DMARXDESC_FRAME_LENGTHSHIFT           16

/* 以太网DMA接收描述符缓冲区2大小移位 */
#define  ETH_DMARXDESC_BUFFER2_SIZESHIFT           16

/* 以太网错误码 */
#define  ETH_ERROR              ((uint32_t)0)  /* 错误 */
#define  ETH_SUCCESS            ((uint32_t)1)  /* 成功 */

/* 函数声明 */
void ETH_DeInit(void);                                      // 反初始化以太网外设（复位寄存器）
void ETH_StructInit(ETH_InitTypeDef* ETH_InitStruct);       // 初始化ETH_InitTypeDef结构体为默认值
void ETH_SoftwareReset(void);                                // 触发以太网软件复位
FlagStatus ETH_GetSoftwareResetStatus(void);                 // 获取软件复位状态（是否正在进行）
FlagStatus ETH_GetlinkStaus (void);                          // 获取内部PHY链接状态
void  ETH_Start(void);                                       // 启动以太网传输和接收
uint32_t ETH_HandleTxPkt(uint8_t *ppkt, uint16_t FrameLength); // 发送一个数据包
void delay_clk (uint32_t nCount);                            // 时钟延时函数
void printf_dmasr (void);                                    // 打印DMA状态寄存器内容（调试用）
void print_dmasr_tbus(void);                                 // 打印DMA发送缓冲区不可用状态
void print_dmasr_rps(void);                                  // 打印DMA接收进程状态
void print_dmasr_tps(void);                                  // 打印DMA发送进程状态
uint32_t ETH_HandleRxPkt(uint8_t *ppkt);                     // 接收一个数据包
uint32_t ETH_GetRxPktSize(void);                             // 获取接收到的数据包大小
void ETH_DropRxPkt(void);                                    // 丢弃当前接收的数据包
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg); // 读取PHY寄存器
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue); // 写入PHY寄存器
uint32_t ETH_PHYLoopBackCmd(uint16_t PHYAddress, FunctionalState NewState); // 使能或禁用PHY环回模式

void ETH_MACTransmissionCmd(FunctionalState NewState);       // 使能或禁用MAC发送
void ETH_MACReceptionCmd(FunctionalState NewState);          // 使能或禁用MAC接收
FlagStatus ETH_GetFlowControlBusyStatus(void);               // 获取流控制忙状态
void ETH_InitiatePauseControlFrame(void);                    // 发起暂停控制帧
void ETH_BackPressureActivationCmd(FunctionalState NewState); // 使能或禁用背压操作
FlagStatus ETH_GetMACFlagStatus(uint32_t ETH_MAC_FLAG);      // 获取MAC标志状态
ITStatus ETH_GetMACITStatus(uint32_t ETH_MAC_IT);            // 获取MAC中断状态
void ETH_MACITConfig(uint32_t ETH_MAC_IT, FunctionalState NewState); // 配置MAC中断
void ETH_MACAddressConfig(uint32_t MacAddr, uint8_t *Addr);  // 配置MAC地址
void ETH_GetMACAddress(uint32_t MacAddr, uint8_t *Addr);     // 获取MAC地址
void ETH_MACAddressPerfectFilterCmd(uint32_t MacAddr, FunctionalState NewState); // 使能或禁用MAC地址完美过滤
void ETH_MACAddressFilterConfig(uint32_t MacAddr, uint32_t Filter); // 配置MAC地址过滤类型（源/目的）
void ETH_MACAddressMaskBytesFilterConfig(uint32_t MacAddr, uint32_t MaskByte); // 配置MAC地址字节掩码过滤

void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount); // 初始化DMA发送描述符链模式
void ETH_DMATxDescRingInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff1, uint8_t *TxBuff2, uint32_t TxBuffCount); // 初始化DMA发送描述符环模式
FlagStatus ETH_GetDMATxDescFlagStatus(ETH_DMADESCTypeDef *DMATxDesc, uint32_t ETH_DMATxDescFlag); // 获取发送描述符标志状态
uint32_t ETH_GetDMATxDescCollisionCount(ETH_DMADESCTypeDef *DMATxDesc); // 获取发送描述符的冲突计数
void ETH_SetDMATxDescOwnBit(ETH_DMADESCTypeDef *DMATxDesc);  // 设置发送描述符的OWN位（交给DMA）
void ETH_DMATxDescTransmitITConfig(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState); // 配置发送完成中断
void ETH_DMATxDescFrameSegmentConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t DMATxDesc_FrameSegment); // 配置帧段（首段/末段）
void ETH_DMATxDescChecksumInsertionConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t DMATxDesc_Checksum); // 配置校验和插入
void ETH_DMATxDescCRCCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState); // 使能或禁用CRC插入
void ETH_DMATxDescEndOfRingCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState); // 设置环尾标志
void ETH_DMATxDescSecondAddressChainedCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState); // 使能或禁用第二地址链模式
void ETH_DMATxDescShortFramePaddingCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState); // 使能或禁用短帧填充
void ETH_DMATxDescTimeStampCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState); // 使能或禁用时间戳
void ETH_DMATxDescBufferSizeConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t BufferSize1, uint32_t BufferSize2); // 配置发送缓冲区大小
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount); // 初始化DMA接收描述符链模式
void ETH_DMARxDescRingInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff1, uint8_t *RxBuff2, uint32_t RxBuffCount); // 初始化DMA接收描述符环模式
FlagStatus ETH_GetDMARxDescFlagStatus(ETH_DMADESCTypeDef *DMARxDesc, uint32_t ETH_DMARxDescFlag); // 获取接收描述符标志状态
void ETH_SetDMARxDescOwnBit(ETH_DMADESCTypeDef *DMARxDesc);  // 设置接收描述符的OWN位（交给DMA）
uint32_t ETH_GetDMARxDescFrameLength(ETH_DMADESCTypeDef *DMARxDesc); // 获取接收描述符的帧长度
void ETH_DMARxDescReceiveITConfig(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState); // 配置接收完成中断
void ETH_DMARxDescEndOfRingCmd(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState); // 设置接收环尾标志
void ETH_DMARxDescSecondAddressChainedCmd(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState); // 使能或禁用接收第二地址链模式
uint32_t ETH_GetDMARxDescBufferSize(ETH_DMADESCTypeDef *DMARxDesc, uint32_t DMARxDesc_Buffer); // 获取接收描述符缓冲区大小

FlagStatus ETH_GetDMAFlagStatus(uint32_t ETH_DMA_FLAG);      // 获取DMA标志状态
void ETH_DMAClearFlag(uint32_t ETH_DMA_FLAG);                // 清除DMA标志
ITStatus ETH_GetDMAITStatus(uint32_t ETH_DMA_IT);            // 获取DMA中断状态
void ETH_DMAClearITPendingBit(uint32_t ETH_DMA_IT);          // 清除DMA中断挂起位
uint32_t ETH_GetTransmitProcessState(void);                  // 获取DMA发送进程状态
uint32_t ETH_GetReceiveProcessState(void);                   // 获取DMA接收进程状态
void ETH_FlushTransmitFIFO(void);                             // 刷新发送FIFO
FlagStatus ETH_GetFlushTransmitFIFOStatus(void);             // 获取刷新发送FIFO状态
void ETH_DMATransmissionCmd(FunctionalState NewState);       // 使能或禁用DMA发送
void ETH_DMAReceptionCmd(FunctionalState NewState);          // 使能或禁用DMA接收
void ETH_DMAITConfig(uint32_t ETH_DMA_IT, FunctionalState NewState); // 配置DMA中断
FlagStatus ETH_GetDMAOverflowStatus(uint32_t ETH_DMA_Overflow); // 获取DMA溢出状态
uint32_t ETH_GetRxOverflowMissedFrameCounter(void);          // 获取接收溢出丢帧计数器
uint32_t ETH_GetBufferUnavailableMissedFrameCounter(void);   // 获取缓冲区不可用丢帧计数器
uint32_t ETH_GetCurrentTxDescStartAddress(void);             // 获取当前发送描述符起始地址
uint32_t ETH_GetCurrentRxDescStartAddress(void);             // 获取当前接收描述符起始地址
uint32_t ETH_GetCurrentTxBufferAddress(void);                // 获取当前发送缓冲区地址
uint32_t ETH_GetCurrentRxBufferAddress(void);                // 获取当前接收缓冲区地址
void ETH_ResumeDMATransmission(void);                         // 恢复DMA发送
void ETH_ResumeDMAReception(void);                            // 恢复DMA接收

void ETH_ResetWakeUpFrameFilterRegisterPointer(void);        // 复位唤醒帧过滤寄存器指针
void ETH_SetWakeUpFrameFilterRegister(uint32_t *Buffer);     // 设置唤醒帧过滤寄存器
void ETH_GlobalUnicastWakeUpCmd(FunctionalState NewState);   // 使能或禁用全局单播唤醒
FlagStatus ETH_GetPMTFlagStatus(uint32_t ETH_PMT_FLAG);      // 获取PMT标志状态
void ETH_WakeUpFrameDetectionCmd(FunctionalState NewState);  // 使能或禁用唤醒帧检测
void ETH_MagicPacketDetectionCmd(FunctionalState NewState);  // 使能或禁用魔术包检测
void ETH_PowerDownCmd(FunctionalState NewState);              // 使能或禁用掉电模式

void ETH_MMCCounterFreezeCmd(FunctionalState NewState);      // 使能或禁用MMC计数器冻结
void ETH_MMCResetOnReadCmd(FunctionalState NewState);        // 使能或禁用读时复位MMC计数器
void ETH_MMCCounterRolloverCmd(FunctionalState NewState);    // 使能或禁用MMC计数器回滚
void ETH_MMCCountersReset(void);                              // 复位MMC计数器
void ETH_MMCITConfig(uint32_t ETH_MMC_IT, FunctionalState NewState); // 配置MMC中断
ITStatus ETH_GetMMCITStatus(uint32_t ETH_MMC_IT);            // 获取MMC中断状态
uint32_t ETH_GetMMCRegister(uint32_t ETH_MMCReg);            // 读取MMC寄存器

uint32_t ETH_HandlePTPTxPkt(uint8_t *ppkt, uint16_t FrameLength, uint32_t *PTPTxTab); // 发送PTP数据包并获取时间戳
uint32_t ETH_HandlePTPRxPkt(uint8_t *ppkt, uint32_t *PTPRxTab); // 接收PTP数据包并获取时间戳
void ETH_DMAPTPTxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, ETH_DMADESCTypeDef *DMAPTPTxDescTab, uint8_t* TxBuff, uint32_t TxBuffCount); // 初始化PTP发送描述符链
void ETH_DMAPTPRxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, ETH_DMADESCTypeDef *DMAPTPRxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount); // 初始化PTP接收描述符链
void ETH_EnablePTPTimeStampAddend(void);                     // 使能PTP时间戳加数更新
void ETH_EnablePTPTimeStampInterruptTrigger(void);           // 使能PTP时间戳中断触发
void ETH_EnablePTPTimeStampUpdate(void);                     // 使能PTP时间戳更新
void ETH_InitializePTPTimeStamp(void);                       // 初始化PTP时间戳
void ETH_PTPUpdateMethodConfig(uint32_t UpdateMethod);       // 配置PTP更新方法（粗调/细调）
void ETH_PTPTimeStampCmd(FunctionalState NewState);          // 使能或禁用PTP时间戳
FlagStatus ETH_GetPTPFlagStatus(uint32_t ETH_PTP_FLAG);      // 获取PTP标志状态
void ETH_SetPTPSubSecondIncrement(uint32_t SubSecondValue);  // 设置PTP亚秒递增值
void ETH_SetPTPTimeStampUpdate(uint32_t Sign, uint32_t SecondValue, uint32_t SubSecondValue); // 设置PTP时间戳更新值
void ETH_SetPTPTimeStampAddend(uint32_t Value);              // 设置PTP时间戳加数
void ETH_SetPTPTargetTime(uint32_t HighValue, uint32_t LowValue); // 设置PTP目标时间
uint32_t ETH_GetPTPRegister(uint32_t ETH_PTPReg);            // 读取PTP寄存器
void RGMII_TXC_Delay(uint8_t clock_polarity,uint8_t delay_time); // 配置RGMII TXC时钟延迟

#ifdef __cplusplus
}
#endif

#endif