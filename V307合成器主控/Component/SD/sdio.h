/********************************** (C) COPYRIGHT *******************************
 * File Name          : sdio.h
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : 本文件包含SDIO接口的头文件定义。
 *********************************************************************************
 * Copyright (c) 2021 南京沁恒微电子股份有限公司
 * 注意: 本软件（修改或不修改）和二进制代码仅可用于南京沁恒微电子生产的微控制器。
 *******************************************************************************/
#ifndef __SDIO_H
#define __SDIO_H

#include "debug.h"

#define delay_ms(x) Delay_Ms(x)  // 毫秒延时宏，调用底层延时函数

/* SDIO 状态标志 */
#define SDIO_FLAG_CCRCFAIL ((uint32_t)0x00000001)  // 命令响应CRC校验失败
#define SDIO_FLAG_DCRCFAIL ((uint32_t)0x00000002)  // 数据CRC校验失败
#define SDIO_FLAG_CTIMEOUT ((uint32_t)0x00000004)  // 命令响应超时
#define SDIO_FLAG_DTIMEOUT ((uint32_t)0x00000008)  // 数据超时
#define SDIO_FLAG_TXUNDERR ((uint32_t)0x00000010)  // 发送下溢（FIFO空时继续发送）
#define SDIO_FLAG_RXOVERR ((uint32_t)0x00000020)   // 接收上溢（FIFO满时继续接收）
#define SDIO_FLAG_CMDREND ((uint32_t)0x00000040)   // 命令响应接收完成
#define SDIO_FLAG_CMDSENT ((uint32_t)0x00000080)   // 命令发送完成
#define SDIO_FLAG_DATAEND ((uint32_t)0x00000100)   // 数据传输结束
#define SDIO_FLAG_STBITERR ((uint32_t)0x00000200)  // 起始位错误（仅用于SDIO的4位模式）
#define SDIO_FLAG_DBCKEND ((uint32_t)0x00000400)   // 数据块结束（数据CRC完成）
#define SDIO_FLAG_CMDACT ((uint32_t)0x00000800)    // 命令正在传输
#define SDIO_FLAG_TXACT ((uint32_t)0x00001000)     // 数据正在发送
#define SDIO_FLAG_RXACT ((uint32_t)0x00002000)     // 数据正在接收
#define SDIO_FLAG_TXFIFOHE ((uint32_t)0x00004000)  // 发送FIFO半空
#define SDIO_FLAG_RXFIFOHF ((uint32_t)0x00008000)  // 接收FIFO半满
#define SDIO_FLAG_TXFIFOF ((uint32_t)0x00010000)   // 发送FIFO满
#define SDIO_FLAG_RXFIFOF ((uint32_t)0x00020000)   // 接收FIFO满
#define SDIO_FLAG_TXFIFOE ((uint32_t)0x00040000)   // 发送FIFO空
#define SDIO_FLAG_RXFIFOE ((uint32_t)0x00080000)   // 接收FIFO空
#define SDIO_FLAG_TXDAVL ((uint32_t)0x00100000)    // 发送FIFO中数据可用（非空）
#define SDIO_FLAG_RXDAVL ((uint32_t)0x00200000)    // 接收FIFO中数据可用（非空）
#define SDIO_FLAG_SDIOIT ((uint32_t)0x00400000)    // SDIO中断（仅用于I/O卡）
#define SDIO_FLAG_CEATAEND ((uint32_t)0x00800000)  // CE-ATA命令完成

/* SDIO 时钟分频 */
#define SDIO_INIT_CLK_DIV 0xB2      // 初始化时钟分频（低速模式，约400kHz）
#define SDIO_TRANSFER_CLK_DIV 0x00  // 传输时钟分频（高速模式，最大时钟）

/* SDIO 工作模式 */
#define SD_POLLING_MODE 0  // 轮询模式（非DMA）
#define SD_DMA_MODE 1      // DMA模式

/* SDIO 错误码定义 */
typedef enum
{
    /* 特定错误码（来自卡状态R1或响应） */
    SD_CMD_CRC_FAIL = (1),            // 命令响应CRC校验失败
    SD_DATA_CRC_FAIL = (2),           // 数据CRC校验失败
    SD_CMD_RSP_TIMEOUT = (3),         // 命令响应超时
    SD_DATA_TIMEOUT = (4),            // 数据传输超时
    SD_TX_UNDERRUN = (5),             // 发送下溢
    SD_RX_OVERRUN = (6),              // 接收上溢
    SD_START_BIT_ERR = (7),           // 起始位错误
    SD_CMD_OUT_OF_RANGE = (8),        // 命令中地址超出范围
    SD_ADDR_MISALIGNED = (9),         // 地址未对齐
    SD_BLOCK_LEN_ERR = (10),          // 块长度错误
    SD_ERASE_SEQ_ERR = (11),          // 擦除序列错误
    SD_BAD_ERASE_PARAM = (12),        // 无效擦除参数
    SD_WRITE_PROT_VIOLATION = (13),   // 写保护违规
    SD_LOCK_UNLOCK_FAILED = (14),     // 锁定/解锁失败
    SD_COM_CRC_FAILED = (15),         // 命令CRC校验失败（卡端）
    SD_ILLEGAL_CMD = (16),            // 非法命令
    SD_CARD_ECC_FAILED = (17),        // 卡ECC校验失败
    SD_CC_ERROR = (18),               // 卡控制器错误
    SD_GENERAL_UNKNOWN_ERROR = (19),  // 未知错误
    SD_STREAM_READ_UNDERRUN = (20),   // 流式读下溢
    SD_STREAM_WRITE_OVERRUN = (21),   // 流式写上溢
    SD_CID_CSD_OVERWRITE = (22),      // CID/CSD寄存器被覆写
    SD_WP_ERASE_SKIP = (23),          // 写保护擦除跳过
    SD_CARD_ECC_DISABLED = (24),      // 卡ECC已禁用
    SD_ERASE_RESET = (25),            // 擦除复位
    SD_AKE_SEQ_ERROR = (26),          // 认证密钥交换序列错误
    SD_INVALID_VOLTRANGE = (27),      // 无效电压范围
    SD_ADDR_OUT_OF_RANGE = (28),      // 地址超出范围（扩展）
    SD_SWITCH_ERROR = (29),           // 切换功能错误
    SD_SDIO_DISABLED = (30),          // SDIO功能禁用
    SD_SDIO_FUNCTION_BUSY = (31),     // SDIO功能忙
    SD_SDIO_FUNCTION_FAILED = (32),   // SDIO功能失败
    SD_SDIO_UNKNOWN_FUNCTION = (33),  // 未知SDIO功能
    /* 标准错误码（库内部使用） */
    SD_INTERNAL_ERROR,          // 内部错误
    SD_NOT_CONFIGURED,          // 未配置
    SD_REQUEST_PENDING,         // 请求挂起
    SD_REQUEST_NOT_APPLICABLE,  // 请求不适用
    SD_INVALID_PARAMETER,       // 无效参数
    SD_UNSUPPORTED_FEATURE,     // 不支持的特性
    SD_UNSUPPORTED_HW,          // 不支持的硬件
    SD_ERROR,                   // 一般错误
    SD_OK = 0                   // 成功
} SD_Error;

/* SD 卡 CSD 寄存器结构体 */
typedef struct
{
    u8 CSDStruct;            // CSD结构版本
    u8 SysSpecVersion;       // 系统规范版本
    u8 Reserved1;            // 保留位
    u8 TAAC;                 // 读数据时间（TAAC）
    u8 NSAC;                 // 时钟依赖的读时间（NSAC）
    u8 MaxBusClkFrec;        // 最大总线时钟频率
    u16 CardComdClasses;     // 卡支持的命令类
    u8 RdBlockLen;           // 读块最大长度
    u8 PartBlockRead;        // 部分块读允许
    u8 WrBlockMisalign;      // 写块未对齐
    u8 RdBlockMisalign;      // 读块未对齐
    u8 DSRImpl;              // DSR寄存器实现
    u8 Reserved2;            // 保留位
    u32 DeviceSize;          // 设备大小
    u8 MaxRdCurrentVDDMin;   // 最小VDD读最大电流
    u8 MaxRdCurrentVDDMax;   // 最大VDD读最大电流
    u8 MaxWrCurrentVDDMin;   // 最小VDD写最大电流
    u8 MaxWrCurrentVDDMax;   // 最大VDD写最大电流
    u8 DeviceSizeMul;        // 设备大小乘数
    u8 EraseGrSize;          // 擦除组大小
    u8 EraseGrMul;           // 擦除组乘数
    u8 WrProtectGrSize;      // 写保护组大小
    u8 WrProtectGrEnable;    // 写保护组使能
    u8 ManDeflECC;           // 制造商默认ECC
    u8 WrSpeedFact;          // 写速度因子
    u8 MaxWrBlockLen;        // 最大写块长度
    u8 WriteBlockPaPartial;  // 部分块写允许
    u8 Reserved3;            // 保留位
    u8 ContentProtectAppli;  // 内容保护应用
    u8 FileFormatGrouop;     // 文件格式组
    u8 CopyFlag;             // 复制标志（OTP）
    u8 PermWrProtect;        // 永久写保护
    u8 TempWrProtect;        // 临时写保护
    u8 FileFormat;           // 文件格式
    u8 ECC;                  // ECC类型
    u8 CSD_CRC;              // CSD CRC
    u8 Reserved4;            // 保留位
} SD_CSD;

/* SD 卡 CID 寄存器结构体 */
typedef struct
{
    u8 ManufacturerID;  // 制造商ID
    u16 OEM_AppliID;    // OEM/应用ID
    u32 ProdName1;      // 产品名称（前4字节）
    u8 ProdName2;       // 产品名称（第5字节）
    u8 ProdRev;         // 产品修订版本
    u32 ProdSN;         // 产品序列号
    u8 Reserved1;       // 保留位
    u16 ManufactDate;   // 生产日期
    u8 CID_CRC;         // CID CRC
    u8 Reserved2;       // 保留位
} SD_CID;

/* SD 卡状态枚举 */
typedef enum
{
    SD_CARD_READY = ((uint32_t)0x00000001),           // 卡就绪
    SD_CARD_IDENTIFICATION = ((uint32_t)0x00000002),  // 卡识别模式
    SD_CARD_STANDBY = ((uint32_t)0x00000003),         // 待机模式
    SD_CARD_TRANSFER = ((uint32_t)0x00000004),        // 数据传输模式
    SD_CARD_SENDING = ((uint32_t)0x00000005),         // 正在发送数据
    SD_CARD_RECEIVING = ((uint32_t)0x00000006),       // 正在接收数据
    SD_CARD_PROGRAMMING = ((uint32_t)0x00000007),     // 正在编程
    SD_CARD_DISCONNECTED = ((uint32_t)0x00000008),    // 断开连接
    SD_CARD_ERROR = ((uint32_t)0x000000FF)            // 错误状态
} SDCardState;

/* SD 卡信息结构体 */
typedef struct
{
    SD_CSD SD_csd;           // CSD寄存器信息
    SD_CID SD_cid;           // CID寄存器信息
    long long CardCapacity;  // 卡总容量（字节）
    u32 CardBlockSize;       // 卡块大小（字节）
    u16 RCA;                 // 相对卡地址
    u8 CardType;             // 卡类型（如SDSC、SDHC等）
} SD_CardInfo;

extern SD_CardInfo SDCardInfo;  // 全局SD卡信息变量

/* SDIO 指令定义（CMD索引） */
#define SD_CMD_GO_IDLE_STATE ((u8)0)          // CMD0: 使卡进入空闲状态
#define SD_CMD_SEND_OP_COND ((u8)1)           // CMD1: 发送操作条件（MMC专用）
#define SD_CMD_ALL_SEND_CID ((u8)2)           // CMD2: 请求所有卡的CID
#define SD_CMD_SET_REL_ADDR ((u8)3)           // CMD3: 设置相对卡地址（仅SD）
#define SD_CMD_SET_DSR ((u8)4)                // CMD4: 设置DSR寄存器
#define SD_CMD_SDIO_SEN_OP_COND ((u8)5)       // CMD5: SDIO卡发送操作条件
#define SD_CMD_HS_SWITCH ((u8)6)              // CMD6: 切换功能/高速模式
#define SD_CMD_SEL_DESEL_CARD ((u8)7)         // CMD7: 选择/取消选择卡
#define SD_CMD_HS_SEND_EXT_CSD ((u8)8)        // CMD8: 发送扩展CSD（MMC）或接口条件（SD V2.0）
#define SD_CMD_SEND_CSD ((u8)9)               // CMD9: 发送CSD寄存器
#define SD_CMD_SEND_CID ((u8)10)              // CMD10: 发送CID寄存器
#define SD_CMD_READ_DAT_UNTIL_STOP ((u8)11)   // CMD11: 读数据直到停止（流式读）
#define SD_CMD_STOP_TRANSMISSION ((u8)12)     // CMD12: 停止传输
#define SD_CMD_SEND_STATUS ((u8)13)           // CMD13: 发送卡状态
#define SD_CMD_HS_BUSTEST_READ ((u8)14)       // CMD14: 高速总线测试读
#define SD_CMD_GO_INACTIVE_STATE ((u8)15)     // CMD15: 使卡进入非活动状态
#define SD_CMD_SET_BLOCKLEN ((u8)16)          // CMD16: 设置块长度
#define SD_CMD_READ_SINGLE_BLOCK ((u8)17)     // CMD17: 读单块
#define SD_CMD_READ_MULT_BLOCK ((u8)18)       // CMD18: 读多块
#define SD_CMD_HS_BUSTEST_WRITE ((u8)19)      // CMD19: 高速总线测试写
#define SD_CMD_WRITE_DAT_UNTIL_STOP ((u8)20)  // CMD20: 写数据直到停止（流式写）
#define SD_CMD_SET_BLOCK_COUNT ((u8)23)       // CMD23: 设置多块传输的块数
#define SD_CMD_WRITE_SINGLE_BLOCK ((u8)24)    // CMD24: 写单块
#define SD_CMD_WRITE_MULT_BLOCK ((u8)25)      // CMD25: 写多块
#define SD_CMD_PROG_CID ((u8)26)              // CMD26: 编程CID
#define SD_CMD_PROG_CSD ((u8)27)              // CMD27: 编程CSD
#define SD_CMD_SET_WRITE_PROT ((u8)28)        // CMD28: 设置写保护
#define SD_CMD_CLR_WRITE_PROT ((u8)29)        // CMD29: 清除写保护
#define SD_CMD_SEND_WRITE_PROT ((u8)30)       // CMD30: 发送写保护状态
#define SD_CMD_SD_ERASE_GRP_START ((u8)32)    // CMD32: 设置擦除组起始地址
#define SD_CMD_SD_ERASE_GRP_END ((u8)33)      // CMD33: 设置擦除组结束地址
#define SD_CMD_ERASE_GRP_START ((u8)35)       // CMD35: 设置擦除组起始（MMC）
#define SD_CMD_ERASE_GRP_END ((u8)36)         // CMD36: 设置擦除组结束（MMC）
#define SD_CMD_ERASE ((u8)38)                 // CMD38: 执行擦除
#define SD_CMD_FAST_IO ((u8)39)               // CMD39: 快速I/O
#define SD_CMD_GO_IRQ_STATE ((u8)40)          // CMD40: 进入中断模式
#define SD_CMD_LOCK_UNLOCK ((u8)42)           // CMD42: 锁定/解锁卡
#define SD_CMD_APP_CMD ((u8)55)               // CMD55: 应用特定命令前缀
#define SD_CMD_GEN_CMD ((u8)56)               // CMD56: 通用命令
#define SD_CMD_NO_CMD ((u8)64)                // 无命令

/* 以下为SD卡应用特定命令（需要先发CMD55） */
#define SD_CMD_APP_SD_SET_BUSWIDTH ((u8)6)            // ACMD6: 设置总线宽度
#define SD_CMD_SD_APP_STAUS ((u8)13)                  // ACMD13: 发送SD状态
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS ((u8)22)  // ACMD22: 发送已写块数
#define SD_CMD_SD_APP_OP_COND ((u8)41)                // ACMD41: 发送操作条件（SD卡）
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT ((u8)42)    // ACMD42: 设置/清除卡检测
#define SD_CMD_SD_APP_SEND_SCR ((u8)51)               // ACMD51: 发送SCR寄存器
#define SD_CMD_SDIO_RW_DIRECT ((u8)52)                // CMD52: SDIO直接读写
#define SD_CMD_SDIO_RW_EXTENDED ((u8)53)              // CMD53: SDIO扩展读写

/* SD卡安全应用命令（需要先发CMD55） */
#define SD_CMD_SD_APP_GET_MKB ((u8)43)                      // ACMD43: 获取MKB
#define SD_CMD_SD_APP_GET_MID ((u8)44)                      // ACMD44: 获取MID
#define SD_CMD_SD_APP_SET_CER_RN1 ((u8)45)                  // ACMD45: 设置CER_RN1
#define SD_CMD_SD_APP_GET_CER_RN2 ((u8)46)                  // ACMD46: 获取CER_RN2
#define SD_CMD_SD_APP_SET_CER_RES2 ((u8)47)                 // ACMD47: 设置CER_RES2
#define SD_CMD_SD_APP_GET_CER_RES1 ((u8)48)                 // ACMD48: 获取CER_RES1
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK ((u8)18)   // ACMD18: 安全读多块
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK ((u8)25)  // ACMD25: 安全写多块
#define SD_CMD_SD_APP_SECURE_ERASE ((u8)38)                 // ACMD38: 安全擦除
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA ((u8)49)           // ACMD49: 更改安全区域
#define SD_CMD_SD_APP_SECURE_WRITE_MKB ((u8)48)             // ACMD48: 安全写MKB（与上重复？注意索引）

/* SD 卡类型定义 */
#define SDIO_STD_CAPACITY_SD_CARD_V1_1 ((u32)0x00000000)     // 标准容量SD卡V1.1
#define SDIO_STD_CAPACITY_SD_CARD_V2_0 ((u32)0x00000001)     // 标准容量SD卡V2.0
#define SDIO_HIGH_CAPACITY_SD_CARD ((u32)0x00000002)         // 高容量SD卡（SDHC）
#define SDIO_MULTIMEDIA_CARD ((u32)0x00000003)               // MMC卡
#define SDIO_SECURE_DIGITAL_IO_CARD ((u32)0x00000004)        // SDIO卡
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD ((u32)0x00000005)    // 高速MMC
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD ((u32)0x00000006)  // SDIO组合卡
#define SDIO_HIGH_CAPACITY_MMC_CARD ((u32)0x00000007)        // 高容量MMC

/* SDIO 参数定义 */
#define SDIO_STATIC_FLAGS ((u32)0x000005FF)  // 静态标志掩码（清除用）
#define SDIO_CMD0TIMEOUT ((u32)0x00010000)   // CMD0超时值
#define SDIO_DATATIMEOUT ((u32)0xFFFFFFFF)   // 数据超时值（最大）
#define SDIO_FIFO_Address ((u32)0x40018080)  // SDIO FIFO地址

/* 卡状态R1（OCR）中的错误掩码 */
#define SD_OCR_ADDR_OUT_OF_RANGE ((u32)0x80000000)      // 地址超出范围
#define SD_OCR_ADDR_MISALIGNED ((u32)0x40000000)        // 地址未对齐
#define SD_OCR_BLOCK_LEN_ERR ((u32)0x20000000)          // 块长度错误
#define SD_OCR_ERASE_SEQ_ERR ((u32)0x10000000)          // 擦除序列错误
#define SD_OCR_BAD_ERASE_PARAM ((u32)0x08000000)        // 无效擦除参数
#define SD_OCR_WRITE_PROT_VIOLATION ((u32)0x04000000)   // 写保护违规
#define SD_OCR_LOCK_UNLOCK_FAILED ((u32)0x01000000)     // 锁定/解锁失败
#define SD_OCR_COM_CRC_FAILED ((u32)0x00800000)         // 命令CRC校验失败
#define SD_OCR_ILLEGAL_CMD ((u32)0x00400000)            // 非法命令
#define SD_OCR_CARD_ECC_FAILED ((u32)0x00200000)        // 卡ECC错误
#define SD_OCR_CC_ERROR ((u32)0x00100000)               // 卡控制器错误
#define SD_OCR_GENERAL_UNKNOWN_ERROR ((u32)0x00080000)  // 未知错误
#define SD_OCR_STREAM_READ_UNDERRUN ((u32)0x00040000)   // 流读下溢
#define SD_OCR_STREAM_WRITE_OVERRUN ((u32)0x00020000)   // 流写上溢
#define SD_OCR_CID_CSD_OVERWRIETE ((u32)0x00010000)     // CID/CSD被覆写
#define SD_OCR_WP_ERASE_SKIP ((u32)0x00008000)          // 写保护擦除跳过
#define SD_OCR_CARD_ECC_DISABLED ((u32)0x00004000)      // 卡ECC禁用
#define SD_OCR_ERASE_RESET ((u32)0x00002000)            // 擦除复位
#define SD_OCR_AKE_SEQ_ERROR ((u32)0x00000008)          // 认证密钥交换序列错误
#define SD_OCR_ERRORBITS ((u32)0xFDFFE008)              // 所有错误位掩码

/* R6响应中的掩码 */
#define SD_R6_GENERAL_UNKNOWN_ERROR ((u32)0x00002000)  // 未知错误
#define SD_R6_ILLEGAL_CMD ((u32)0x00004000)            // 非法命令
#define SD_R6_COM_CRC_FAILED ((u32)0x00008000)         // 命令CRC失败

/* 电压范围与卡容量标志 */
#define SD_VOLTAGE_WINDOW_SD ((u32)0x80100000)   // SD卡支持的电压范围（2.7-3.6V）
#define SD_HIGH_CAPACITY ((u32)0x40000000)       // 高容量卡标志（SDHC/SDXC）
#define SD_STD_CAPACITY ((u32)0x00000000)        // 标准容量卡标志
#define SD_CHECK_PATTERN ((u32)0x000001AA)       // CMD8的检查模式
#define SD_VOLTAGE_WINDOW_MMC ((u32)0x80FF8000)  // MMC卡电压范围

#define SD_MAX_VOLT_TRIAL ((u32)0x0000FFFF)      // 最大电压尝试次数
#define SD_ALLZERO ((u32)0x00000000)             // 全零

#define SD_WIDE_BUS_SUPPORT ((u32)0x00040000)    // 宽总线（4位）支持
#define SD_SINGLE_BUS_SUPPORT ((u32)0x00010000)  // 单总线（1位）支持
#define SD_CARD_LOCKED ((u32)0x02000000)         // 卡锁定标志
#define SD_CARD_PROGRAMMING ((u32)0x00000007)    // 卡编程状态
#define SD_CARD_RECEIVING ((u32)0x00000006)      // 卡接收状态
#define SD_DATATIMEOUT ((u32)0xFFFFFFFF)         // 数据超时值
#define SD_0TO7BITS ((u32)0x000000FF)            // 低8位掩码
#define SD_8TO15BITS ((u32)0x0000FF00)           // 8-15位掩码
#define SD_16TO23BITS ((u32)0x00FF0000)          // 16-23位掩码
#define SD_24TO31BITS ((u32)0xFF000000)          // 24-31位掩码
#define SD_MAX_DATA_LENGTH ((u32)0x01FFFFFF)     // 最大数据长度（约32MB）

#define SD_HALFFIFO ((u32)0x00000008)            // FIFO半满/半空标志（用于中断）
#define SD_HALFFIFOBYTES ((u32)0x00000020)       // 半FIFO字节数（32字节）

/* 命令类支持位 */
#define SD_CCCC_LOCK_UNLOCK ((u32)0x00000080)  // 支持锁定/解锁命令类
#define SD_CCCC_WRITE_PROT ((u32)0x00000040)   // 支持写保护命令类
#define SD_CCCC_ERASE ((u32)0x00000020)        // 支持擦除命令类

/* CMD8 定义 */
#define SDIO_SEND_IF_COND ((u32)0x00000008)  // CMD8 发送接口条件

/* 函数声明 */

/**
 * @brief 初始化SD卡（包括时钟、电源、卡识别等）
 * @return SD_Error 操作结果
 */
SD_Error SD_Init(void);

/**
 * @brief 设置SDIO时钟分频
 * @param clkdiv 时钟分频值（0~0xFF，实际时钟 = SDIOCLK / (clkdiv+2)）
 */
void SDIO_Clock_Set(u8 clkdiv);

/**
 * @brief 发送SDIO命令
 * @param cmdindex 命令索引（0-63）
 * @param waitrsp 等待响应类型（0=无响应，1=短响应，2=长响应）
 * @param arg 命令参数（32位）
 */
void SDIO_Send_Cmd(u8 cmdindex, u8 waitrsp, u32 arg);

/**
 * @brief 配置SDIO数据传输
 * @param datatimeout 数据超时值
 * @param datalen 数据长度（字节）
 * @param blksize 块大小（如512字节）
 * @param dir 传输方向（0=从卡读，1=向卡写）
 */
void SDIO_Send_Data_Cfg(u32 datatimeout, u32 datalen, u8 blksize, u8 dir);

/**
 * @brief 上电并复位SD卡
 * @return SD_Error 操作结果
 */
SD_Error SD_PowerON(void);

/**
 * @brief 关闭SD卡电源
 * @return SD_Error 操作结果
 */
SD_Error SD_PowerOFF(void);

/**
 * @brief 初始化并识别卡（获取CID、RCA等）
 * @return SD_Error 操作结果
 */
SD_Error SD_InitializeCards(void);

/**
 * @brief 获取SD卡信息（CSD、CID、容量等）
 * @param cardinfo 指向SD_CardInfo结构体的指针，用于存储信息
 * @return SD_Error 操作结果
 */
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo);

/**
 * @brief 使能宽总线模式（4位模式）
 * @param wmode 总线模式（0=1位，1=4位）
 * @return SD_Error 操作结果
 */
SD_Error SD_EnableWideBusOperation(u32 wmode);

/**
 * @brief 设置SDIO工作模式（轮询或DMA）
 * @param mode SD_POLLING_MODE 或 SD_DMA_MODE
 * @return SD_Error 操作结果
 */
SD_Error SD_SetDeviceMode(u32 mode);

/**
 * @brief 选择或取消选择指定的卡
 * @param addr 卡的RCA地址
 * @return SD_Error 操作结果
 */
SD_Error SD_SelectDeselect(u32 addr);

/**
 * @brief 发送CMD13获取卡状态
 * @param pcardstatus 指向存储卡状态的指针
 * @return SD_Error 操作结果
 */
SD_Error SD_SendStatus(uint32_t *pcardstatus);

/**
 * @brief 获取当前卡状态
 * @return SDCardState 卡状态枚举
 */
SDCardState SD_GetState(void);

/**
 * @brief 读单个块
 * @param buf 数据缓冲区
 * @param addr 起始地址（字节地址，SDSC时，SDHC/SDXC时为块地址）
 * @param blksize 块大小（通常512）
 * @return SD_Error 操作结果
 */
SD_Error SD_ReadBlock(u8 *buf, long long addr, u16 blksize);

/**
 * @brief 读多个块
 * @param buf 数据缓冲区
 * @param addr 起始地址
 * @param blksize 块大小
 * @param nblks 块数量
 * @return SD_Error 操作结果
 */
SD_Error SD_ReadMultiBlocks(u8 *buf, long long addr, u16 blksize, u32 nblks);

/**
 * @brief 写单个块
 * @param buf 数据缓冲区
 * @param addr 起始地址
 * @param blksize 块大小
 * @return SD_Error 操作结果
 */
SD_Error SD_WriteBlock(u8 *buf, long long addr, u16 blksize);

/**
 * @brief 写多个块
 * @param buf 数据缓冲区
 * @param addr 起始地址
 * @param blksize 块大小
 * @param nblks 块数量
 * @return SD_Error 操作结果
 */
SD_Error SD_WriteMultiBlocks(u8 *buf, long long addr, u16 blksize, u32 nblks);

/**
 * @brief SDIO中断源处理（在中断服务例程中调用）
 * @return SD_Error 操作结果
 */
SD_Error SD_ProcessIRQSrc(void);

/**
 * @brief 配置DMA用于SDIO数据传输(在单块/多块读写时被调用)
 * @param mbuf 内存缓冲区地址
 * @param bufsize 缓冲区大小
 * @param dir 方向（0=读，1=写）
 */
void SD_DMA_Config(u32 *mbuf, u32 bufsize, u32 dir);

/**
 * @brief 读磁盘（扇区为单位）
 * @param buf 数据缓冲区
 * @param sector 起始扇区号（通常512字节/扇区）
 * @param cnt 扇区数量
 * @return 0=成功，非0=失败
 */
u8 SD_ReadDisk(u8 *buf, u32 sector, u8 cnt);

/**
 * @brief 写磁盘（扇区为单位）
 * @param buf 数据缓冲区
 * @param sector 起始扇区号
 * @param cnt 扇区数量
 * @return 0=成功，非0=失败
 */
u8 SD_WriteDisk(u8 *buf, u32 sector, u8 cnt);

#endif
