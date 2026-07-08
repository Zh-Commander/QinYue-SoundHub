/********************************** (C) COPYRIGHT *******************************
* 文件名            : system_ch32v30x.h
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2024/05/22
* 描述              : CH32V30x 器件外设访问层系统头文件。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）和二进制文件可用于南京沁恒微电子股份有限公司生产的微控制器。
*******************************************************************************/

#ifndef __CH32V30x_USB_H
#define __CH32V30x_USB_H

#ifdef __cplusplus
 extern "C" {
#endif

/*******************************************************************************/
/* 头文件 */
#include "stdint.h"

/*******************************************************************************/
/* USB 通信相关宏定义 */
/* USB 端点0最大包大小 */
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE          8          // 端点0默认最大包大小（字节）
#endif
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE             64         // 最大包大小（字节）
#endif

/* USB 令牌包 PID（Packet Identifier）定义 */
#ifndef USB_PID_SETUP
#define USB_PID_NULL                0x00       // 空令牌
#define USB_PID_SOF                 0x05       // 帧起始令牌
#define USB_PID_SETUP               0x0D       // SETUP 令牌
#define USB_PID_IN                  0x09       // IN 令牌
#define USB_PID_OUT                 0x01       // OUT 令牌
#define USB_PID_NYET                0x06       // NYET 握手包（高速模式）
#define USB_PID_ACK                 0x02       // ACK 握手包
#define USB_PID_NAK                 0x0A       // NAK 握手包
#define USB_PID_STALL               0x0E       // STALL 握手包
#define USB_PID_DATA0               0x03       // DATA0 数据包
#define USB_PID_DATA1               0x0B       // DATA1 数据包
#define USB_PID_DATA2               0x07       // DATA2 数据包（高速高带宽）
#define USB_PID_MDATA               0x0F       // MDATA 数据包（高速 split 事务）
#define USB_PID_PRE                 0x0C       // PRE（前导）令牌（低速设备通过集线器）
#endif

/* USB 标准设备请求码 */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS              0x00       // 获取状态请求
#define USB_CLEAR_FEATURE           0x01       // 清除特性请求
#define USB_SET_FEATURE             0x03       // 设置特性请求
#define USB_SET_ADDRESS             0x05       // 设置地址请求
#define USB_GET_DESCRIPTOR          0x06       // 获取描述符请求
#define USB_SET_DESCRIPTOR          0x07       // 设置描述符请求
#define USB_GET_CONFIGURATION       0x08       // 获取配置请求
#define USB_SET_CONFIGURATION       0x09       // 设置配置请求
#define USB_GET_INTERFACE           0x0A       // 获取接口请求
#define USB_SET_INTERFACE           0x0B       // 设置接口请求
#define USB_SYNCH_FRAME             0x0C       // 同步帧请求
#endif

/* 字符串描述符索引 */
#define DEF_STRING_DESC_LANG        0x00       // 语言ID字符串索引
#define DEF_STRING_DESC_MANU        0x01       // 制造商字符串索引
#define DEF_STRING_DESC_PROD        0x02       // 产品字符串索引
#define DEF_STRING_DESC_SERN        0x03       // 序列号字符串索引

/* USB 集线器类请求码 */
#ifndef HUB_GET_DESCRIPTOR
#define HUB_GET_STATUS              0x00       // 获取集线器/端口状态
#define HUB_CLEAR_FEATURE           0x01       // 清除集线器/端口特性
#define HUB_GET_STATE               0x02       // 获取集线器状态（USB 2.0 规范中未使用）
#define HUB_SET_FEATURE             0x03       // 设置集线器/端口特性
#define HUB_GET_DESCRIPTOR          0x06       // 获取集线器描述符
#define HUB_SET_DESCRIPTOR          0x07       // 设置集线器描述符
#endif

/* USB HID 类请求码 */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT              0x01       // 获取报告
#define HID_GET_IDLE                0x02       // 获取空闲率
#define HID_GET_PROTOCOL            0x03       // 获取协议
#define HID_SET_REPORT              0x09       // 设置报告
#define HID_SET_IDLE                0x0A       // 设置空闲率
#define HID_SET_PROTOCOL            0x0B       // 设置协议
#endif

/* USB CDC 类请求码 */
#ifndef CDC_GET_LINE_CODING
#define CDC_GET_LINE_CODING         0x21       // 获取当前线路编码（波特率、停止位等）
#define CDC_SET_LINE_CODING         0x20       // 设置线路编码
#define CDC_SET_LINE_CTLSTE         0x22       // 设置控制线路状态
#define CDC_SEND_BREAK              0x23       // 发送中断信号
#endif

/* USB 请求类型字段位定义 */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN              0x80       // 数据传输方向：设备到主机
#define USB_REQ_TYP_OUT             0x00       // 数据传输方向：主机到设备
#define USB_REQ_TYP_READ            0x80       // 读操作（同 IN）
#define USB_REQ_TYP_WRITE           0x00       // 写操作（同 OUT）
#define USB_REQ_TYP_MASK            0x60       // 请求类型掩码（bit5-6）
#define USB_REQ_TYP_STANDARD        0x00       // 标准请求
#define USB_REQ_TYP_CLASS           0x20       // 类请求
#define USB_REQ_TYP_VENDOR          0x40       // 厂商请求
#define USB_REQ_TYP_RESERVED        0x60       // 保留
#define USB_REQ_RECIP_MASK          0x1F       // 接收者掩码（bit0-4）
#define USB_REQ_RECIP_DEVICE        0x00       // 接收者为设备
#define USB_REQ_RECIP_INTERF        0x01       // 接收者为接口
#define USB_REQ_RECIP_ENDP          0x02       // 接收者为端点
#define USB_REQ_RECIP_OTHER         0x03       // 接收者为其他
#define USB_REQ_FEAT_REMOTE_WAKEUP  0x01       // 特性选择：远程唤醒
#define USB_REQ_FEAT_ENDP_HALT      0x00       // 特性选择：端点停止
#endif

/* USB 描述符类型 */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE        0x01       // 设备描述符
#define USB_DESCR_TYP_CONFIG        0x02       // 配置描述符
#define USB_DESCR_TYP_STRING        0x03       // 字符串描述符
#define USB_DESCR_TYP_INTERF        0x04       // 接口描述符
#define USB_DESCR_TYP_ENDP          0x05       // 端点描述符
#define USB_DESCR_TYP_QUALIF        0x06       // 设备限定符描述符
#define USB_DESCR_TYP_SPEED         0x07       // 其他速度配置描述符
#define USB_DESCR_TYP_OTG           0x09       // OTG 描述符
#define USB_DESCR_TYP_BOS           0X0F       // BOS 描述符
#define USB_DESCR_TYP_HID           0x21       // HID 描述符
#define USB_DESCR_TYP_REPORT        0x22       // 报告描述符
#define USB_DESCR_TYP_PHYSIC        0x23       // 物理描述符
#define USB_DESCR_TYP_CS_INTF       0x24       // 类特定接口描述符
#define USB_DESCR_TYP_CS_ENDP       0x25       // 类特定端点描述符
#define USB_DESCR_TYP_HUB           0x29       // 集线器描述符
#endif

/* USB 设备类代码 */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED      0x00       // 保留（在接口描述符中使用）
#define USB_DEV_CLASS_AUDIO         0x01       // 音频类
#define USB_DEV_CLASS_COMMUNIC      0x02       // 通信和 CDC 控制类
#define USB_DEV_CLASS_HID           0x03       // 人机接口设备类
#define USB_DEV_CLASS_MONITOR       0x04       // 显示器类
#define USB_DEV_CLASS_PHYSIC_IF     0x05       // 物理接口类
#define USB_DEV_CLASS_POWER         0x06       // 电源类（如智能电池）
#define USB_DEV_CLASS_IMAGE         0x06       // 图像类（与电源类共用代码，实际通过子类区分）
#define USB_DEV_CLASS_PRINTER       0x07       // 打印机类
#define USB_DEV_CLASS_STORAGE       0x08       // 大容量存储类
#define USB_DEV_CLASS_HUB           0x09       // 集线器类
#define USB_DEV_CLASS_VEN_SPEC      0xFF       // 厂商特定类
#endif

/* USB 集线器类请求（补充） */
#ifndef HUB_GET_HUB_DESCRIPTOR
#define HUB_CLEAR_HUB_FEATURE       0x20       // 清除集线器特性
#define HUB_CLEAR_PORT_FEATURE      0x23       // 清除端口特性
#define HUB_GET_BUS_STATE           0xA3       // 获取总线状态
#define HUB_GET_HUB_DESCRIPTOR      0xA0       // 获取集线器描述符
#define HUB_GET_HUB_STATUS          0xA0       // 获取集线器状态
#define HUB_GET_PORT_STATUS         0xA3       // 获取端口状态
#define HUB_SET_HUB_DESCRIPTOR      0x20       // 设置集线器描述符
#define HUB_SET_HUB_FEATURE         0x20       // 设置集线器特性
#define HUB_SET_PORT_FEATURE        0x23       // 设置端口特性
#endif

/* 集线器类特性选择符（Feature Selectors） */
#ifndef HUB_PORT_RESET
#define HUB_C_HUB_LOCAL_POWER       0          // 本地电源状态变化
#define HUB_C_HUB_OVER_CURRENT      1          // 过流状态变化
#define HUB_PORT_CONNECTION         0          // 端口连接状态
#define HUB_PORT_ENABLE             1          // 端口使能
#define HUB_PORT_SUSPEND            2          // 端口挂起
#define HUB_PORT_OVER_CURRENT       3          // 端口过流
#define HUB_PORT_RESET              4          // 端口复位
#define HUB_PORT_POWER              8          // 端口电源
#define HUB_PORT_LOW_SPEED          9          // 端口低速设备
#define HUB_C_PORT_CONNECTION       16         // 连接状态变化
#define HUB_C_PORT_ENABLE           17         // 使能状态变化
#define HUB_C_PORT_SUSPEND          18         // 挂起状态变化
#define HUB_C_PORT_OVER_CURRENT     19         // 过流状态变化
#define HUB_C_PORT_RESET            20         // 复位状态变化
#endif

/* USB 大容量存储（U盘）相关宏定义 */
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE             0x1F       // CBW（命令块包）长度（31字节）
#define USB_BO_CSW_SIZE             0x0D       // CSW（命令状态包）长度（13字节）
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0             0x55       // CBW 签名低字节 'U'（实际为 0x43425355）
#define USB_BO_CBW_SIG1             0x53       // CBW 签名 'S'
#define USB_BO_CBW_SIG2             0x42       // CBW 签名 'B'
#define USB_BO_CBW_SIG3             0x43       // CBW 签名 'C'
#define USB_BO_CSW_SIG0             0x55       // CSW 签名低字节 'U'（实际为 0x53425355）
#define USB_BO_CSW_SIG1             0x53       // CSW 签名 'S'
#define USB_BO_CSW_SIG2             0x42       // CSW 签名 'B'
#define USB_BO_CSW_SIG3             0x53       // CSW 签名 'S'
#endif


/******************************************************************************/
/* USBHS 时钟配置相关宏定义 */
#define USB_CLK_SRC                 0x80000000  // USB 时钟源选择（HS）
#define USBHS_PLL_ALIVE             0x40000000  // PLL 活跃标志
#define USBHS_PLL_CKREF_MASK        0x30000000  // PLL 参考时钟频率掩码
#define USBHS_PLL_CKREF_3M          0x00000000  // 参考时钟 3MHz
#define USBHS_PLL_CKREF_4M          0x10000000  // 参考时钟 4MHz
#define USBHS_PLL_CKREF_8M          0x20000000  // 参考时钟 8MHz
#define USBHS_PLL_CKREF_5M          0x30000000  // 参考时钟 5MHz
#define USBHS_PLL_SRC               0x08000000  // PLL 输入时钟源选择
#define USBHS_PLL_SRC_PRE_MASK      0x07000000  // 预分频系数掩码
#define USBHS_PLL_SRC_PRE_DIV1      0x00000000  // 不分频
#define USBHS_PLL_SRC_PRE_DIV2      0x01000000  // 2 分频
#define USBHS_PLL_SRC_PRE_DIV3      0x02000000  // 3 分频
#define USBHS_PLL_SRC_PRE_DIV4      0x03000000  // 4 分频
#define USBHS_PLL_SRC_PRE_DIV5      0x04000000  // 5 分频
#define USBHS_PLL_SRC_PRE_DIV6      0x05000000  // 6 分频
#define USBHS_PLL_SRC_PRE_DIV7      0x06000000  // 7 分频
#define USBHS_PLL_SRC_PRE_DIV8      0x07000000  // 8 分频


/*******************************************************************************/
/* USBHS 相关寄存器位定义 */

/* R8_USB_CTRL - USB 控制寄存器（8位） */
#define USBHS_UC_HOST_MODE          0x80        // 主机模式选择：1=主机模式，0=设备模式
#define USBHS_UC_SPEED_TYPE         0x60        // 速度类型掩码
#define USBHS_UC_SPEED_LOW          0x40        // 低速模式
#define USBHS_UC_SPEED_FULL         0x00        // 全速模式
#define USBHS_UC_SPEED_HIGH         0x20        // 高速模式
#define USBHS_UC_DEV_PU_EN          0x10        // 设备上拉电阻使能（设备模式）
#define USBHS_UC_INT_BUSY           0x08        // 中断繁忙标志（表示中断正在处理）
#define USBHS_UC_RESET_SIE          0x04        // 复位 SIE（串行接口引擎）
#define USBHS_UC_CLR_ALL            0x02        // 清除所有配置和状态
#define USBHS_UC_DMA_EN             0x01        // DMA 使能

/* R8_USB_INT_EN - 中断使能寄存器（8位） */
#define USBHS_UIE_DEV_NAK           0x80        // 设备 NAK 中断使能
#define USBHS_UIE_ISO_ACT           0x40        // 同步传输活动中断使能
#define USBHS_UIE_SETUP_ACT         0x20        // SETUP 事务活动中断使能
#define USBHS_UIE_FIFO_OV           0x10        // FIFO 溢出中断使能
#define USBHS_UIE_SOF_ACT           0x08        // SOF 活动中断使能
#define USBHS_UIE_SUSPEND           0x04        // 挂起/唤醒中断使能
#define USBHS_UIE_TRANSFER          0x02        // 传输完成中断使能
#define USBHS_UIE_DETECT            0x01        // 设备检测中断使能（主机模式）
#define USBHS_UIE_BUS_RST           0x01        // 总线复位中断使能（设备模式）

/* R16_USB_DEV_AD - 设备地址寄存器（16位） */
#define USBHS_MASK_USB_ADDR         0x7F        // USB 设备地址掩码（bit0-6）

/* R16_USB_FRAME_NO - 帧号寄存器（16位） */
#define USBHS_MICRO_FRAME_NUM       0xE000      // 微帧号（高速模式 bit13-15）
#define USBHS_SOF_FRAME_NUM         0x07FF      // SOF 帧号（bit0-10）

/* R8_USB_SUSPEND - 挂起控制寄存器（8位） */
#define USBHS_USB_LINESTATE         0x30        // USB 线状态（DP/DM 电平）
#define USBHS_USB_WAKEUP_ST         0x04        // 唤醒状态
#define USBHS_USB_SYS_MOD           0x03        // 系统模式选择

/* R8_USB_SPEED_TYPE - 速度类型寄存器（8位） */
#define USBHS_USB_SPEED_TYPE        0x03        // 当前速度类型掩码
#define USBHS_USB_SPEED_LOW         0x02        // 低速
#define USBHS_USB_SPEED_FULL        0x00        // 全速
#define USBHS_USB_SPEED_HIGH        0x01        // 高速

/* R8_USB_MIS_ST - 杂项状态寄存器（8位） */
#define USBHS_UMS_SOF_PRES          0x80        // SOF 正在发送/接收
#define USBHS_UMS_SOF_ACT           0x40        // SOF 活动指示
#define USBHS_UMS_SIE_FREE          0x20        // SIE 空闲
#define USBHS_UMS_R_FIFO_RDY        0x10        // 接收 FIFO 就绪
#define USBHS_UMS_BUS_RESET         0x08        // 总线复位状态
#define USBHS_UMS_SUSPEND           0x04        // 挂起状态
#define USBHS_UMS_DEV_ATTACH        0x02        // 设备连接（主机模式）
#define USBHS_UMS_SPLIT_CAN         0x01        // split 事务状态

/* R8_USB_INT_FG - 中断标志寄存器（8位） */
#define USBHS_UIF_ISO_ACT           0x40        // 同步传输活动中断标志
#define USBHS_UIF_SETUP_ACT         0x20        // SETUP 事务中断标志
#define USBHS_UIF_FIFO_OV           0x10        // FIFO 溢出中断标志
#define USBHS_UIF_HST_SOF           0x08        // 主机 SOF 中断标志
#define USBHS_UIF_SUSPEND           0x04        // 挂起/唤醒中断标志
#define USBHS_UIF_TRANSFER          0x02        // 传输完成中断标志
#define USBHS_UIF_DETECT            0x01        // 设备检测中断标志（主机模式）
#define USBHS_UIF_BUS_RST           0x01        // 总线复位中断标志（设备模式）

/* R8_USB_INT_ST - 中断状态寄存器（8位） */
#define USBHS_UIS_IS_NAK            0x80        // 当前传输收到 NAK
#define USBHS_UIS_TOG_OK            0x40        // 数据切换同步正确
#define USBHS_UIS_TOKEN_MASK        0x30        // 当前令牌 PID 掩码
#define USBHS_UIS_TOKEN_OUT         0x00        // OUT 令牌
#define USBHS_UIS_TOKEN_SOF         0x10        // SOF 令牌
#define USBHS_UIS_TOKEN_IN          0x20        // IN 令牌
#define USBHS_UIS_TOKEN_SETUP       0x30        // SETUP 令牌
#define USBHS_UIS_ENDP_MASK         0x0F        // 当前传输的端点号掩码
#define USBHS_UIS_H_RES_MASK        0x0F        // 主机模式握手响应掩码（值=握手PID）

/* R16_USB_RX_LEN - 接收长度寄存器（16位） */
#define USBHS_USB_RX_LEN            0xFFFF      // 接收到的数据字节数

/* R32_UEP_CONFIG - 端点配置寄存器（32位） */
#define USBHS_UEP15_R_EN            0x80000000  // 端点15 接收使能
#define USBHS_UEP14_R_EN            0x40000000  // 端点14 接收使能
#define USBHS_UEP13_R_EN            0x20000000  // 端点13 接收使能
#define USBHS_UEP12_R_EN            0x10000000  // 端点12 接收使能
#define USBHS_UEP11_R_EN            0x08000000  // 端点11 接收使能
#define USBHS_UEP10_R_EN            0x04000000  // 端点10 接收使能
#define USBHS_UEP9_R_EN             0x02000000  // 端点9 接收使能
#define USBHS_UEP8_R_EN             0x01000000  // 端点8 接收使能
#define USBHS_UEP7_R_EN             0x00800000  // 端点7 接收使能
#define USBHS_UEP6_R_EN             0x00400000  // 端点6 接收使能
#define USBHS_UEP5_R_EN             0x00200000  // 端点5 接收使能
#define USBHS_UEP4_R_EN             0x00100000  // 端点4 接收使能
#define USBHS_UEP3_R_EN             0x00080000  // 端点3 接收使能
#define USBHS_UEP2_R_EN             0x00040000  // 端点2 接收使能
#define USBHS_UEP1_R_EN             0x00020000  // 端点1 接收使能
#define USBHS_UEP0_R_EN             0x00010000  // 端点0 接收使能
#define USBHS_UEP15_T_EN            0x00008000  // 端点15 发送使能
#define USBHS_UEP14_T_EN            0x00004000  // 端点14 发送使能
#define USBHS_UEP13_T_EN            0x00002000  // 端点13 发送使能
#define USBHS_UEP12_T_EN            0x00001000  // 端点12 发送使能
#define USBHS_UEP11_T_EN            0x00000800  // 端点11 发送使能
#define USBHS_UEP10_T_EN            0x00000400  // 端点10 发送使能
#define USBHS_UEP9_T_EN             0x00000200  // 端点9 发送使能
#define USBHS_UEP8_T_EN             0x00000100  // 端点8 发送使能
#define USBHS_UEP7_T_EN             0x00000080  // 端点7 发送使能
#define USBHS_UEP6_T_EN             0x00000040  // 端点6 发送使能
#define USBHS_UEP5_T_EN             0x00000020  // 端点5 发送使能
#define USBHS_UEP4_T_EN             0x00000010  // 端点4 发送使能
#define USBHS_UEP3_T_EN             0x00000008  // 端点3 发送使能
#define USBHS_UEP2_T_EN             0x00000004  // 端点2 发送使能
#define USBHS_UEP1_T_EN             0x00000002  // 端点1 发送使能
#define USBHS_UEP0_T_EN             0x00000001  // 端点0 发送使能

/* R32_UEP_TYPE - 端点类型寄存器（32位） */
#define USBHS_UEP15_R_TYPE          0x80000000  // 端点15 接收类型（0=控制/批量/中断，1=同步）
#define USBHS_UEP14_R_TYPE          0x40000000  // 端点14 接收类型
#define USBHS_UEP13_R_TYPE          0x20000000  // 端点13 接收类型
#define USBHS_UEP12_R_TYPE          0x10000000  // 端点12 接收类型
#define USBHS_UEP11_R_TYPE          0x08000000  // 端点11 接收类型
#define USBHS_UEP10_R_TYPE          0x04000000  // 端点10 接收类型
#define USBHS_UEP9_R_TYPE           0x02000000  // 端点9 接收类型
#define USBHS_UEP8_R_TYPE           0x01000000  // 端点8 接收类型
#define USBHS_UEP7_R_TYPE           0x00800000  // 端点7 接收类型
#define USBHS_UEP6_R_TYPE           0x00400000  // 端点6 接收类型
#define USBHS_UEP5_R_TYPE           0x00200000  // 端点5 接收类型
#define USBHS_UEP4_R_TYPE           0x00100000  // 端点4 接收类型
#define USBHS_UEP3_R_TYPE           0x00080000  // 端点3 接收类型
#define USBHS_UEP2_R_TYPE           0x00040000  // 端点2 接收类型
#define USBHS_UEP1_R_TYPE           0x00020000  // 端点1 接收类型
#define USBHS_UEP0_R_TYPE           0x00010000  // 端点0 接收类型
#define USBHS_UEP15_T_TYPE          0x00008000  // 端点15 发送类型
#define USBHS_UEP14_T_TYPE          0x00004000  // 端点14 发送类型
#define USBHS_UEP13_T_TYPE          0x00002000  // 端点13 发送类型
#define USBHS_UEP12_T_TYPE          0x00001000  // 端点12 发送类型
#define USBHS_UEP11_T_TYPE          0x00000800  // 端点11 发送类型
#define USBHS_UEP10_T_TYPE          0x00000400  // 端点10 发送类型
#define USBHS_UEP9_T_TYPE           0x00000200  // 端点9 发送类型
#define USBHS_UEP8_T_TYPE           0x00000100  // 端点8 发送类型
#define USBHS_UEP7_T_TYPE           0x00000080  // 端点7 发送类型
#define USBHS_UEP6_T_TYPE           0x00000040  // 端点6 发送类型
#define USBHS_UEP5_T_TYPE           0x00000020  // 端点5 发送类型
#define USBHS_UEP4_T_TYPE           0x00000010  // 端点4 发送类型
#define USBHS_UEP3_T_TYPE           0x00000008  // 端点3 发送类型
#define USBHS_UEP2_T_TYPE           0x00000004  // 端点2 发送类型
#define USBHS_UEP1_T_TYPE           0x00000002  // 端点1 发送类型
#define USBHS_UEP0_T_TYPE           0x00000001  // 端点0 发送类型

/* R32_UEP_BUF_MOD - 端点缓冲模式寄存器（32位） */
#define USBHS_UEP15_ISO_BUF_MOD     0x80000000  // 端点15 同步缓冲模式（双缓冲）
#define USBHS_UEP14_ISO_BUF_MOD     0x40000000  // 端点14 同步缓冲模式
#define USBHS_UEP13_ISO_BUF_MOD     0x20000000  // 端点13 同步缓冲模式
#define USBHS_UEP12_ISO_BUF_MOD     0x10000000  // 端点12 同步缓冲模式
#define USBHS_UEP11_ISO_BUF_MOD     0x08000000  // 端点11 同步缓冲模式
#define USBHS_UEP10_ISO_BUF_MOD     0x04000000  // 端点10 同步缓冲模式
#define USBHS_UEP9_ISO_BUF_MOD      0x02000000  // 端点9 同步缓冲模式
#define USBHS_UEP8_ISO_BUF_MOD      0x01000000  // 端点8 同步缓冲模式
#define USBHS_UEP7_ISO_BUF_MOD      0x00800000  // 端点7 同步缓冲模式
#define USBHS_UEP6_ISO_BUF_MOD      0x00400000  // 端点6 同步缓冲模式
#define USBHS_UEP5_ISO_BUF_MOD      0x00200000  // 端点5 同步缓冲模式
#define USBHS_UEP4_ISO_BUF_MOD      0x00100000  // 端点4 同步缓冲模式
#define USBHS_UEP3_ISO_BUF_MOD      0x00080000  // 端点3 同步缓冲模式
#define USBHS_UEP2_ISO_BUF_MOD      0x00040000  // 端点2 同步缓冲模式
#define USBHS_UEP1_ISO_BUF_MOD      0x00020000  // 端点1 同步缓冲模式
#define USBHS_UEP0_ISO_BUF_MOD      0x00010000  // 端点0 同步缓冲模式
#define USBHS_UEP15_BUF_MOD         0x00008000  // 端点15 缓冲模式（0=单缓冲，1=双缓冲）
#define USBHS_UEP14_BUF_MOD         0x00004000  // 端点14 缓冲模式
#define USBHS_UEP13_BUF_MOD         0x00002000  // 端点13 缓冲模式
#define USBHS_UEP12_BUF_MOD         0x00001000  // 端点12 缓冲模式
#define USBHS_UEP11_BUF_MOD         0x00000800  // 端点11 缓冲模式
#define USBHS_UEP10_BUF_MOD         0x00000400  // 端点10 缓冲模式
#define USBHS_UEP9_BUF_MOD          0x00000200  // 端点9 缓冲模式
#define USBHS_UEP8_BUF_MOD          0x00000100  // 端点8 缓冲模式
#define USBHS_UEP7_BUF_MOD          0x00000080  // 端点7 缓冲模式
#define USBHS_UEP6_BUF_MOD          0x00000040  // 端点6 缓冲模式
#define USBHS_UEP5_BUF_MOD          0x00000020  // 端点5 缓冲模式
#define USBHS_UEP4_BUF_MOD          0x00000010  // 端点4 缓冲模式
#define USBHS_UEP3_BUF_MOD          0x00000008  // 端点3 缓冲模式
#define USBHS_UEP2_BUF_MOD          0x00000004  // 端点2 缓冲模式
#define USBHS_UEP1_BUF_MOD          0x00000002  // 端点1 缓冲模式
#define USBHS_UEP0_BUF_MOD          0x00000001  // 端点0 缓冲模式

/* R32_UEP0_DMA - 端点0 DMA 地址寄存器（32位）低16位有效 */
#define USBHS_UEP0_DMA              0x0000FFFF  // 端点0 DMA 缓冲区地址（低16位）

/* R32_UEPn_TX_DMA - 端点n 发送 DMA 地址寄存器，n=1-15 */
#define USBHS_UEPn_TX_DMA           0x0000FFFF  // 端点n 发送 DMA 缓冲区地址

/* R32_UEPn_RX_DMA - 端点n 接收 DMA 地址寄存器，n=1-15 */
#define USBHS_UEPn_RX_DMA           0x0000FFFF  // 端点n 接收 DMA 缓冲区地址

/* R16_UEPn_MAX_LEN - 端点n 最大包长寄存器，n=0-15 */
#define USBHS_UEPn_MAX_LEN          0x07FF      // 端点n 支持的最大包长度（11位）

/* R16_UEPn_T_LEN - 端点n 发送长度寄存器，n=0-15 */
#define USBHS_UEPn_T_LEN            0x07FF      // 端点n 本次要发送的数据长度

/* R8_UEPn_TX_CTRL - 端点n 发送控制寄存器，n=0-15 */
#define USBHS_UEP_T_TOG_AUTO        0x20        // 发送数据切换自动翻转使能
#define USBHS_UEP_T_TOG_MASK        0x18        // 发送数据切换值掩码
#define USBHS_UEP_T_TOG_DATA0       0x00        // 发送 DATA0
#define USBHS_UEP_T_TOG_DATA1       0x08        // 发送 DATA1
#define USBHS_UEP_T_TOG_DATA2       0x10        // 发送 DATA2（高速同步）
#define USBHS_UEP_T_TOG_MDATA       0x18        // 发送 MDATA（高速 split）
#define USBHS_UEP_T_RES_MASK        0x03        // 发送握手响应类型掩码
#define USBHS_UEP_T_RES_ACK         0x00        // 期望 ACK
#define USBHS_UEP_T_RES_NYET        0x01        // 期望 NYET（高速）
#define USBHS_UEP_T_RES_NAK         0x02        // 期望 NAK
#define USBHS_UEP_T_RES_STALL       0x03        // 期望 STALL

/* R8_UEPn_RX_CTRL - 端点n 接收控制寄存器，n=0-15 */
#define USBHS_UEP_R_TOG_AUTO        0x20        // 接收数据切换自动翻转使能
#define USBHS_UEP_R_TOG_MASK        0x18        // 接收数据切换值掩码
#define USBHS_UEP_R_TOG_DATA0       0x00        // 期望 DATA0
#define USBHS_UEP_R_TOG_DATA1       0x08        // 期望 DATA1
#define USBHS_UEP_R_TOG_DATA2       0x10        // 期望 DATA2
#define USBHS_UEP_R_TOG_MDATA       0x18        // 期望 MDATA
#define USBHS_UEP_R_RES_MASK        0x03        // 接收握手响应类型掩码
#define USBHS_UEP_R_RES_ACK         0x00        // 应答 ACK
#define USBHS_UEP_R_RES_NYET        0x01        // 应答 NYET
#define USBHS_UEP_R_RES_NAK         0x02        // 应答 NAK
#define USBHS_UEP_R_RES_STALL       0x03        // 应答 STALL

/* R8_UHOST_CTRL - 主机控制寄存器（8位） */
#define USBHS_UH_SOF_EN             0x80        // SOF 发送使能
#define USBHS_UH_SOF_FREE           0x40        // SOF 空闲状态
#define USBHS_UH_PHY_SUSPENDM       0x10        // PHY 挂起（低功耗）
#define USBHS_UH_REMOTE_WKUP        0x08        // 远程唤醒使能
#define USBHS_UH_TX_BUS_RESUME      0x04        // 发送总线恢复信号
#define USBHS_UH_TX_BUS_SUSPEND     0x02        // 发送总线挂起信号
#define USBHS_UH_TX_BUS_RESET       0x01        // 发送总线复位信号

/* R32_UH_CONFIG - 主机端点配置寄存器（32位） */
#define USBHS_UH_EP_RX_EN           0x00040000  // 主机接收端点使能（IN）
#define USBHS_UH_EP_TX_EN           0x00000008  // 主机发送端点使能（OUT/SETUP）

/* R32_UH_EP_TYPE - 主机端点类型寄存器（32位） */
#define USBHS_UH_EP_RX_TYPE         0x00040000  // 主机接收端点类型（0=控制/批量/中断，1=同步）
#define USBHS_UH_EP_TX_TYPE         0x00000008  // 主机发送端点类型

/* R32_UH_RX_DMA - 主机接收 DMA 地址寄存器（32位） */
#define USBHS_UH_RX_DMA             0x0000FFFC  // 主机接收 DMA 缓冲区地址（低16位，但要求4字节对齐）

/* R32_UH_TX_DMA - 主机发送 DMA 地址寄存器（32位） */
#define USBHS_UH_TX_DMA             0x0000FFFF  // 主机发送 DMA 缓冲区地址

/* R16_UH_RX_MAX_LEN - 主机接收最大包长寄存器（16位） */
#define USBHS_UH_RX_MAX_LEN         0x07FF      // 主机接收最大包长度

/* R8_UH_EP_PID - 主机端点 PID 寄存器（8位） */
#define USBHS_UH_TOKEN_MASK         0xF0        // 令牌 PID 掩码（IN/OUT/SETUP）
#define USBHS_UH_ENDP_MASK          0x0F        // 端点号掩码

/* R8_UH_RX_CTRL - 主机接收控制寄存器（8位） */
#define USBHS_UH_R_DATA_NO          0x40        // 无数据（用于 PING）
#define USBHS_UH_R_TOG_AUTO         0x20        // 接收数据切换自动翻转使能
#define USBHS_UH_R_TOG_MASK         0x18        // 接收数据切换值掩码
#define USBHS_UH_R_TOG_DATA0        0x00        // 期望 DATA0
#define USBHS_UH_R_TOG_DATA1        0x08        // 期望 DATA1
#define USBHS_UH_R_TOG_DATA2        0x10        // 期望 DATA2
#define USBHS_UH_R_TOG_MDATA        0x18        // 期望 MDATA
#define USBHS_UH_R_RES_NO           0x04        // 无响应（用于同步）
#define USBHS_UH_R_RES_MASK         0x03        // 接收握手响应类型掩码
#define USBHS_UH_R_RES_ACK          0x00        // 应答 ACK
#define USBHS_UH_R_RES_NYET         0x01        // 应答 NYET
#define USBHS_UH_R_RES_NAK          0x02        // 应答 NAK
#define USBHS_UH_R_RES_STALL        0x03        // 应答 STALL

/* R16_UH_TX_LEN - 主机发送长度寄存器（16位） */
#define USBHS_UH_TX_LEN             0x07FF      // 主机本次发送的数据长度

/* R8_UH_TX_CTRL - 主机发送控制寄存器（8位） */
#define USBHS_UH_T_DATA_NO          0x40        // 无数据（用于 PING）
#define USBHS_UH_T_AUTO_TOG         0x20        // 发送数据切换自动翻转使能
#define USBHS_UH_T_TOG_MASK         0x18        // 发送数据切换值掩码
#define USBHS_UH_T_TOG_DATA0        0x00        // 发送 DATA0
#define USBHS_UH_T_TOG_DATA1        0x08        // 发送 DATA1
#define USBHS_UH_T_TOG_DATA2        0x10        // 发送 DATA2
#define USBHS_UH_T_TOG_MDATA        0x18        // 发送 MDATA
#define USBHS_UH_T_RES_NO           0x04        // 无响应（用于同步）
#define USBHS_UH_T_RES_MASK         0x03        // 发送握手响应类型掩码
#define USBHS_UH_T_RES_ACK          0x00        // 期望 ACK
#define USBHS_UH_T_RES_NYET         0x01        // 期望 NYET
#define USBHS_UH_T_RES_NAK          0x02        // 期望 NAK
#define USBHS_UH_T_RES_STALL        0x03        // 期望 STALL

/* R16_UH_SPLIT_DATA - 主机 split 事务数据寄存器（16位） */
#define USBHS_UH_SPLIT_DATA         0x0FFF      // split 事务的额外信息


/*******************************************************************************/
/* USBFS 相关寄存器位定义 */

/* R8_USB_CTRL - USB 控制寄存器（8位） */
#define USBFS_UC_HOST_MODE          0x80        // 主机模式选择
#define USBFS_UC_LOW_SPEED          0x40        // 低速模式使能
#define USBFS_UC_DEV_PU_EN          0x20        // 设备上拉电阻使能
#define USBFS_UC_SYS_CTRL_MASK      0x30        // 系统控制模式掩码
#define USBFS_UC_SYS_CTRL0          0x00        // 系统控制模式0
#define USBFS_UC_SYS_CTRL1          0x10        // 系统控制模式1
#define USBFS_UC_SYS_CTRL2          0x20        // 系统控制模式2
#define USBFS_UC_SYS_CTRL3          0x30        // 系统控制模式3
#define USBFS_UC_INT_BUSY           0x08        // 中断繁忙标志
#define USBFS_UC_RESET_SIE          0x04        // 复位 SIE
#define USBFS_UC_CLR_ALL            0x02        // 清除所有
#define USBFS_UC_DMA_EN             0x01        // DMA 使能

/* R8_USB_INT_EN - 中断使能寄存器（8位） */
#define USBFS_UIE_DEV_SOF           0x80        // 设备 SOF 中断使能
#define USBFS_UIE_DEV_NAK           0x40        // 设备 NAK 中断使能
#define USBFS_1WIRE_MODE            0x20        // 1-Wire 模式使能
#define USBFS_UIE_FIFO_OV           0x10        // FIFO 溢出中断使能
#define USBFS_UIE_HST_SOF           0x08        // 主机 SOF 中断使能
#define USBFS_UIE_SUSPEND           0x04        // 挂起/唤醒中断使能
#define USBFS_UIE_TRANSFER          0x02        // 传输完成中断使能
#define USBFS_UIE_DETECT            0x01        // 设备检测中断使能
#define USBFS_UIE_BUS_RST           0x01        // 总线复位中断使能

/* R8_USB_DEV_AD - 设备地址寄存器（8位） */
#define USBFS_UDA_GP_BIT            0x80        // 通用位
#define USBFS_USB_ADDR_MASK         0x7F        // USB 设备地址掩码

/* R8_USB_MIS_ST - 杂项状态寄存器（8位） */
#define USBFS_UMS_SOF_PRES          0x80        // SOF 存在
#define USBFS_UMS_SOF_ACT           0x40        // SOF 活动
#define USBFS_UMS_SIE_FREE          0x20        // SIE 空闲
#define USBFS_UMS_R_FIFO_RDY        0x10        // 接收 FIFO 就绪
#define USBFS_UMS_BUS_RESET         0x08        // 总线复位状态
#define USBFS_UMS_SUSPEND           0x04        // 挂起状态
#define USBFS_UMS_DM_LEVEL          0x02        // DM 线电平
#define USBFS_UMS_DEV_ATTACH        0x01        // 设备连接

/* R8_USB_INT_FG - 中断标志寄存器（8位） */
#define USBFS_U_IS_NAK              0x80        // 当前传输收到 NAK（只读）
#define USBFS_U_TOG_OK              0x40        // 数据切换正确（只读）
#define USBFS_U_SIE_FREE            0x20        // SIE 空闲（只读）
#define USBFS_UIF_FIFO_OV           0x10        // FIFO 溢出中断标志（写1清除）
#define USBFS_UIF_HST_SOF           0x08        // 主机 SOF 中断标志
#define USBFS_UIF_SUSPEND           0x04        // 挂起/唤醒中断标志
#define USBFS_UIF_TRANSFER          0x02        // 传输完成中断标志
#define USBFS_UIF_DETECT            0x01        // 设备检测中断标志
#define USBFS_UIF_BUS_RST           0x01        // 总线复位中断标志

/* R8_USB_INT_ST - 中断状态寄存器（8位） */
#define USBFS_UIS_IS_NAK            0x80        // 当前传输收到 NAK（设备模式）
#define USBFS_UIS_TOG_OK            0x40        // 数据切换正确
#define USBFS_UIS_TOKEN_MASK        0x30        // 令牌 PID 掩码
#define USBFS_UIS_TOKEN_OUT         0x00        // OUT 令牌
#define USBFS_UIS_TOKEN_SOF         0x10        // SOF 令牌
#define USBFS_UIS_TOKEN_IN          0x20        // IN 令牌
#define USBFS_UIS_TOKEN_SETUP       0x30        // SETUP 令牌
#define USBFS_UIS_ENDP_MASK         0x0F        // 端点号掩码
#define USBFS_UIS_H_RES_MASK        0x0F        // 主机握手响应掩码

/* R32_USB_OTG_CR - OTG 控制寄存器（32位） */
#define USBFS_CR_SESS_VTH           0x20        // 会话有效阈值
#define USBFS_CR_VBUS_VTH           0x10        // VBUS 有效阈值
#define USBFS_CR_OTG_EN             0x08        // OTG 功能使能
#define USBFS_CR_IDPU               0x04        // ID 引脚上拉使能
#define USBFS_CR_CHARGE_VBUS        0x02        // 对 VBUS 充电
#define USBFS_CR_DISCHAR_VBUS       0x01        // 对 VBUS 放电

/* R32_USB_OTG_SR - OTG 状态寄存器（32位） */
#define USBFS_SR_ID_DIG             0x08        // ID 引脚数字值
#define USBFS_SR_SESS_END           0x04        // 会话结束
#define USBFS_SR_SESS_VLD           0x02        // 会话有效
#define USBFS_SR_VBUS_VLD           0x01        // VBUS 有效

/* R8_UDEV_CTRL - 设备控制寄存器（8位） */
#define USBFS_UD_PD_DIS             0x80        // 禁用下拉电阻
#define USBFS_UD_DP_PIN             0x20        // DP 引脚电平（只读）
#define USBFS_UD_DM_PIN             0x10        // DM 引脚电平（只读）
#define USBFS_UD_LOW_SPEED          0x04        // 低速模式使能
#define USBFS_UD_GP_BIT             0x02        // 通用位
#define USBFS_UD_PORT_EN            0x01        // 端口使能

/* R8_UEP4_1_MOD - 端点4/1 模式寄存器（8位） */
#define USBFS_UEP1_RX_EN            0x80        // 端点1 接收使能
#define USBFS_UEP1_TX_EN            0x40        // 端点1 发送使能
#define USBFS_UEP1_BUF_MOD          0x10        // 端点1 缓冲模式（0=单缓冲，1=双缓冲）
#define USBFS_UEP4_RX_EN            0x08        // 端点4 接收使能
#define USBFS_UEP4_TX_EN            0x04        // 端点4 发送使能
#define USBFS_UEP4_BUF_MOD          0x01        // 端点4 缓冲模式

/* R8_UEP2_3_MOD - 端点2/3 模式寄存器（8位） */
#define USBFS_UEP3_RX_EN            0x80        // 端点3 接收使能
#define USBFS_UEP3_TX_EN            0x40        // 端点3 发送使能
#define USBFS_UEP3_BUF_MOD          0x10        // 端点3 缓冲模式
#define USBFS_UEP2_RX_EN            0x08        // 端点2 接收使能
#define USBFS_UEP2_TX_EN            0x04        // 端点2 发送使能
#define USBFS_UEP2_BUF_MOD          0x01        // 端点2 缓冲模式

/* R8_UEP5_6_MOD - 端点5/6 模式寄存器（8位） */
#define USBFS_UEP6_RX_EN            0x80        // 端点6 接收使能
#define USBFS_UEP6_TX_EN            0x40        // 端点6 发送使能
#define USBFS_UEP6_BUF_MOD          0x10        // 端点6 缓冲模式
#define USBFS_UEP5_RX_EN            0x08        // 端点5 接收使能
#define USBFS_UEP5_TX_EN            0x04        // 端点5 发送使能
#define USBFS_UEP5_BUF_MOD          0x01        // 端点5 缓冲模式

/* R8_UEP7_MOD - 端点7 模式寄存器（8位） */
#define USBFS_UEP7_RX_EN            0x08        // 端点7 接收使能
#define USBFS_UEP7_TX_EN            0x04        // 端点7 发送使能
#define USBFS_UEP7_BUF_MOD          0x01        // 端点7 缓冲模式

/* R8_UEPn_TX_CTRL - 端点n 发送控制寄存器（n=0-7） */
#define USBFS_UEP_T_AUTO_TOG        0x08        // 发送数据切换自动翻转使能
#define USBFS_UEP_T_TOG             0x04        // 发送数据切换值（0=DATA0，1=DATA1）
#define USBFS_UEP_T_RES_MASK        0x03        // 发送握手响应类型掩码
#define USBFS_UEP_T_RES_ACK         0x00        // 期望 ACK
#define USBFS_UEP_T_RES_NONE        0x01        // 无响应（用于同步）
#define USBFS_UEP_T_RES_NAK         0x02        // 期望 NAK
#define USBFS_UEP_T_RES_STALL       0x03        // 期望 STALL

/* R8_UEPn_RX_CTRL - 端点n 接收控制寄存器（n=0-7） */
#define USBFS_UEP_R_AUTO_TOG        0x08        // 接收数据切换自动翻转使能
#define USBFS_UEP_R_TOG             0x04        // 期望数据切换值（0=DATA0，1=DATA1）
#define USBFS_UEP_R_RES_MASK        0x03        // 接收握手响应类型掩码
#define USBFS_UEP_R_RES_ACK         0x00        // 应答 ACK
#define USBFS_UEP_R_RES_NONE        0x01        // 无响应（同步）
#define USBFS_UEP_R_RES_NAK         0x02        // 应答 NAK
#define USBFS_UEP_R_RES_STALL       0x03        // 应答 STALL

/* R8_UHOST_CTRL - 主机控制寄存器（8位，USBFS） */
#define USBFS_UH_PD_DIS             0x80        // 禁用下拉电阻
#define USBFS_UH_DP_PIN             0x20        // DP 引脚电平（只读）
#define USBFS_UH_DM_PIN             0x10        // DM 引脚电平（只读）
#define USBFS_UH_LOW_SPEED          0x04        // 低速模式使能
#define USBFS_UH_BUS_RESET          0x02        // 总线复位
#define USBFS_UH_PORT_EN            0x01        // 端口使能

/* R32_UH_EP_MOD - 主机端点模式寄存器（32位） */
#define USBFS_UH_EP_TX_EN           0x40        // 主机发送端点使能
#define USBFS_UH_EP_TBUF_MOD        0x10        // 主机发送缓冲模式
#define USBFS_UH_EP_RX_EN           0x08        // 主机接收端点使能
#define USBFS_UH_EP_RBUF_MOD        0x01        // 主机接收缓冲模式

/* R16_UH_SETUP - 主机设置寄存器（16位） */
#define USBFS_UH_PRE_PID_EN         0x0400      // PRE 令牌使能（用于低速设备通过集线器）
#define USBFS_UH_SOF_EN             0x0004      // 自动 SOF 使能

/* R8_UH_EP_PID - 主机端点 PID 寄存器（8位） */
#define USBFS_UH_TOKEN_MASK         0xF0        // 令牌 PID 掩码
#define USBFS_UH_ENDP_MASK          0x0F        // 端点号掩码

/* R8_UH_RX_CTRL - 主机接收控制寄存器（8位） */
#define USBFS_UH_R_AUTO_TOG         0x08        // 接收数据切换自动翻转使能
#define USBFS_UH_R_TOG              0x04        // 期望数据切换值
#define USBFS_UH_R_RES              0x01        // 握手响应类型（0=ACK，1=无响应）

/* R8_UH_TX_CTRL - 主机发送控制寄存器（8位） */
#define USBFS_UH_T_AUTO_TOG         0x08        // 发送数据切换自动翻转使能
#define USBFS_UH_T_TOG              0x04        // 发送数据切换值
#define USBFS_UH_T_RES              0x01        // 握手响应类型（0=期望 ACK，1=无响应）


/*******************************************************************************/
/* 结构体定义 */

/* USB 设置请求包结构（符合 USB 规范） */
typedef struct __attribute__((packed)) _USB_SETUP_REQ
{
    uint8_t  bRequestType;   // 请求类型（方向、类型、接收者）
    uint8_t  bRequest;       // 请求码
    uint16_t wValue;         // 值字段（2字节）
    uint16_t wIndex;         // 索引字段（2字节）
    uint16_t wLength;        // 数据长度（如果无数据阶段则为0）
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

/* USB 设备描述符结构 */
typedef struct __attribute__((packed)) _USB_DEVICE_DESCR
{
    uint8_t  bLength;            // 描述符长度（18字节）
    uint8_t  bDescriptorType;    // 描述符类型（0x01）
    uint16_t bcdUSB;             // USB 规范版本号（BCD码）
    uint8_t  bDeviceClass;       // 设备类代码
    uint8_t  bDeviceSubClass;    // 设备子类代码
    uint8_t  bDeviceProtocol;    // 设备协议代码
    uint8_t  bMaxPacketSize0;    // 端点0最大包大小
    uint16_t idVendor;           // 厂商ID
    uint16_t idProduct;          // 产品ID
    uint16_t bcdDevice;          // 设备版本号（BCD码）
    uint8_t  iManufacturer;      // 制造商字符串索引
    uint8_t  iProduct;           // 产品字符串索引
    uint8_t  iSerialNumber;      // 序列号字符串索引
    uint8_t  bNumConfigurations; // 配置描述符数量
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

/* USB 配置描述符结构 */
typedef struct __attribute__((packed)) _USB_CONFIG_DESCR
{
    uint8_t  bLength;             // 描述符长度（9字节）
    uint8_t  bDescriptorType;     // 描述符类型（0x02）
    uint16_t wTotalLength;        // 整个配置描述符集的总长度（包括接口、端点等）
    uint8_t  bNumInterfaces;      // 该配置下的接口数量
    uint8_t  bConfigurationValue; // 配置值（用于 SetConfiguration）
    uint8_t  iConfiguration;      // 配置字符串索引
    uint8_t  bmAttributes;        // 配置特性（如总线供电、远程唤醒等）
    uint8_t  MaxPower;            // 最大功耗（单位 2mA）
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

/* USB 接口描述符结构 */
typedef struct __attribute__((packed)) _USB_INTERF_DESCR
{
    uint8_t  bLength;             // 描述符长度（9字节）
    uint8_t  bDescriptorType;     // 描述符类型（0x04）
    uint8_t  bInterfaceNumber;    // 接口编号
    uint8_t  bAlternateSetting;   // 备用设置值
    uint8_t  bNumEndpoints;       // 该接口使用的端点数量（不包括端点0）
    uint8_t  bInterfaceClass;     // 接口类代码
    uint8_t  bInterfaceSubClass;  // 接口子类代码
    uint8_t  bInterfaceProtocol;  // 接口协议代码
    uint8_t  iInterface;          // 接口字符串索引
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

/* USB 端点描述符结构 */
typedef struct __attribute__((packed)) _USB_ENDPOINT_DESCR
{
    uint8_t  bLength;             // 描述符长度（7字节）
    uint8_t  bDescriptorType;     // 描述符类型（0x05）
    uint8_t  bEndpointAddress;    // 端点地址（bit7: 方向，0-3: 端点号）
    uint8_t  bmAttributes;        // 端点属性（传输类型等）
    uint8_t  wMaxPacketSizeL;     // 最大包大小低8位
    uint8_t  wMaxPacketSizeH;     // 最大包大小高8位（含附加同步信息）
    uint8_t  bInterval;           // 轮询间隔（毫秒或微帧）
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

/* USB 配置描述符集简化结构（用于单个接口的简单设备） */
typedef struct __attribute__((packed)) _USB_CONFIG_DESCR_LONG
{
    USB_CFG_DESCR  cfg_descr;     // 配置描述符
    USB_ITF_DESCR  itf_descr;     // 接口描述符
    USB_ENDP_DESCR endp_descr[ 1 ]; // 端点描述符数组（此处为1个，实际根据需要）
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

/* USB 集线器描述符结构 */
typedef struct __attribute__((packed)) _USB_HUB_DESCR
{
    uint8_t  bDescLength;          // 描述符长度
    uint8_t  bDescriptorType;      // 描述符类型（0x29）
    uint8_t  bNbrPorts;            // 端口数量
    uint8_t  wHubCharacteristicsL; // 集线器特性低8位
    uint8_t  wHubCharacteristicsH; // 集线器特性高8位
    uint8_t  bPwrOn2PwrGood;       // 端口上电到电源稳定所需时间（2ms为单位）
    uint8_t  bHubContrCurrent;     // 集线器控制器最大电流（mA）
    uint8_t  DeviceRemovable;      // 端口是否可拆卸（位图）
    uint8_t  PortPwrCtrlMask;      // 端口电源控制掩码
} USB_HUB_DESCR, *PUSB_HUB_DESCR;

/* USB HID 描述符结构 */
typedef struct __attribute__((packed)) _USB_HID_DESCR
{
    uint8_t  bLength;              // 描述符长度（9字节）
    uint8_t  bDescriptorType;      // 描述符类型（0x21）
    uint16_t bcdHID;               // HID 规范版本号（BCD码）
    uint8_t  bCountryCode;         // 国家代码
    uint8_t  bNumDescriptors;      // 下级描述符数量
    uint8_t  bDescriptorTypeX;     // 下级描述符类型（如报告描述符）
    uint8_t  wDescriptorLengthL;   // 下级描述符长度低8位
    uint8_t  wDescriptorLengthH;   // 下级描述符长度高8位
} USB_HID_DESCR, *PUSB_HID_DESCR;

/* USB 大容量存储 Bulk-Only 传输的 CBW（命令块包）结构 */
typedef struct __attribute__((packed)) _UDISK_BOC_CBW
{
    uint32_t mCBW_Sig;             // 签名 "USBC" = 0x43425355
    uint32_t mCBW_Tag;             // 命令标签，由主机产生，设备在 CSW 中返回
    uint32_t mCBW_DataLen;         // 数据传输阶段长度
    uint8_t  mCBW_Flag;            // 标志（bit7: 0=主机到设备，1=设备到主机）
    uint8_t  mCBW_LUN;             // 逻辑单元号
    uint8_t  mCBW_CB_Len;          // 命令块长度（有效字节数）
    uint8_t  mCBW_CB_Buf[ 16 ];    // 命令块（SCSI 命令）
} UDISK_BOC_CBW, *PXUDISK_BOC_CBW;

/* USB 大容量存储 Bulk-Only 传输的 CSW（命令状态包）结构 */
typedef struct __attribute__((packed)) _UDISK_BOC_CSW
{
    uint32_t mCBW_Sig;             // 签名 "USBS" = 0x53425355
    uint32_t mCBW_Tag;             // 应与 CBW 中的标签相同
    uint32_t mCSW_Residue;         // 剩余数据长度（未传输完的数据）
    uint8_t  mCSW_Status;          // 命令执行状态（0=成功，1=失败，2=阶段错误）
} UDISK_BOC_CSW, *PXUDISK_BOC_CSW;


#ifdef __cplusplus
}
#endif

#endif /* __CH32V30x_USB_H */