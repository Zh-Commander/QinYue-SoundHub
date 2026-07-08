/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_SDIO.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 该文件提供了所有 SDIO 固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）和二进制文件可用于南京沁恒微电子股份有限公司生产的微控制器。
*******************************************************************************/

#include "ch32v30x_sdio.h"
#include "ch32v30x_rcc.h"

/* SDIO 外设相对于外设基址的偏移量 */
#define SDIO_OFFSET         (SDIO_BASE - PERIPH_BASE)

/* CLKCR 寄存器清除掩码（用于配置时保留某些位，清除其他位） */
#define CLKCR_CLEAR_MASK    ((uint32_t)0xFFFF8100)

/* POWER 寄存器中电源控制位的掩码（用于修改电源状态时保留其他位） */
#define PWR_PWRCTRL_MASK    ((uint32_t)0xFFFFFFFC)

/* DCTRL 寄存器清除掩码（用于配置数据控制时保留某些位） */
#define DCTRL_CLEAR_MASK    ((uint32_t)0xFFFFFF08)

/* CMD 寄存器清除掩码（用于配置命令时保留某些位） */
#define CMD_CLEAR_MASK      ((uint32_t)0xFFFFF800)

/* 响应寄存器组的基地址（RESP1-RESP4 连续存放） */
#define SDIO_RESP_ADDR      ((uint32_t)(SDIO_BASE + 0x14))

/*********************************************************************
 * @fn      SDIO_DeInit
 *
 * @brief   将 SDIO 外设寄存器复位到默认值。
 *
 * @return  无
 */
void SDIO_DeInit(void)
{
    SDIO->POWER = 0x00000000;   /* 电源控制寄存器清零 */
    SDIO->CLKCR = 0x00000000;   /* 时钟控制寄存器清零 */
    SDIO->ARG   = 0x00000000;   /* 命令参数寄存器清零 */
    SDIO->CMD   = 0x00000000;   /* 命令寄存器清零 */
    SDIO->DTIMER= 0x00000000;   /* 数据超时定时器清零 */
    SDIO->DLEN  = 0x00000000;   /* 数据长度寄存器清零 */
    SDIO->DCTRL = 0x00000000;   /* 数据控制寄存器清零 */
    SDIO->ICR   = 0x00C007FF;   /* 中断清除寄存器写特定值清除所有标志 */
    SDIO->MASK  = 0x00000000;   /* 中断屏蔽寄存器清零 */
}

/*********************************************************************
 * @fn      SDIO_Init
 *
 * @brief   根据 SDIO_InitStruct 中的参数初始化 SDIO 外设。
 *
 * @param   SDIO_InitStruct - 指向 SDIO_InitTypeDef 结构体的指针，
 *                             包含 SDIO 外设的配置信息。
 *
 * @return  无
 */
void SDIO_Init(SDIO_InitTypeDef *SDIO_InitStruct)
{
    uint32_t tmpreg = 0;

    tmpreg = SDIO->CLKCR;
    tmpreg &= CLKCR_CLEAR_MASK;   /* 清除需要配置的位域 */
    /* 组合新的配置值 */
    tmpreg |= (SDIO_InitStruct->SDIO_ClockDiv | 
               SDIO_InitStruct->SDIO_ClockPowerSave |
               SDIO_InitStruct->SDIO_ClockBypass | 
               SDIO_InitStruct->SDIO_BusWide |
               SDIO_InitStruct->SDIO_ClockEdge | 
               SDIO_InitStruct->SDIO_HardwareFlowControl);

    SDIO->CLKCR = tmpreg;          /* 写入配置 */
}

/*********************************************************************
 * @fn      SDIO_StructInit
 *
 * @brief   将 SDIO_InitStruct 中的每个成员初始化为默认值。
 *
 * @param   SDIO_InitStruct - 指向 SDIO_InitTypeDef 结构体的指针，
 *                             将被初始化。
 *
 * @return  无
 */
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct)
{
    SDIO_InitStruct->SDIO_ClockDiv = 0x00;                      /* 时钟分频系数：0 */
    SDIO_InitStruct->SDIO_ClockEdge = SDIO_ClockEdge_Rising;    /* 上升沿采样 */
    SDIO_InitStruct->SDIO_ClockBypass = SDIO_ClockBypass_Disable; /* 禁止旁路模式 */
    SDIO_InitStruct->SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable; /* 禁止节能模式 */
    SDIO_InitStruct->SDIO_BusWide = SDIO_BusWide_1b;            /* 1位总线宽度 */
    SDIO_InitStruct->SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable; /* 禁止硬件流控 */
}

/*********************************************************************
 * @fn      SDIO_ClockCmd
 *
 * @brief   使能或禁用 SDIO 时钟。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_ClockCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CLKCR |= (1 << 8);      /* 设置 CLKCR 的 bit8 使能时钟 */
    else
        SDIO->CLKCR &= ~(1 << 8);     /* 清除 bit8 关闭时钟 */
}

/*********************************************************************
 * @fn      SDIO_SetPowerState
 *
 * @brief   设置 SDIO 控制器的电源状态。
 *
 * @param   SDIO_PowerState - 电源状态新值。
 *            SDIO_PowerState_OFF : 关闭电源
 *            SDIO_PowerState_ON  : 打开电源
 *
 * @return  无
 */
void SDIO_SetPowerState(uint32_t SDIO_PowerState)
{
    SDIO->POWER &= PWR_PWRCTRL_MASK;  /* 清除电源控制位（低2位） */
    SDIO->POWER |= SDIO_PowerState;   /* 设置新的电源状态 */
}

/*********************************************************************
 * @fn      SDIO_GetPowerState
 *
 * @brief   获取 SDIO 控制器的当前电源状态。
 *
 * @return  电源状态 -
 *            0x00 : 电源关闭
 *            0x02 : 电源启动中
 *            0x03 : 电源开启
 */
uint32_t SDIO_GetPowerState(void)
{
    /* 返回 POWER 寄存器的低2位（电源控制位） */
    return (SDIO->POWER & (~PWR_PWRCTRL_MASK));
}

/*********************************************************************
 * @fn      SDIO_ITConfig
 *
 * @brief   使能或禁用 SDIO 中断。
 *
 * @param   SDIO_IT - 指定要配置的中断源，可以是以下值的组合：
 *            SDIO_IT_CCRCFAIL   - 命令响应 CRC 校验失败中断
 *            SDIO_IT_DCRCFAIL   - 数据块 CRC 校验失败中断
 *            SDIO_IT_CTIMEOUT   - 命令响应超时中断
 *            SDIO_IT_DTIMEOUT   - 数据超时中断
 *            SDIO_IT_TXUNDERR   - 发送 FIFO 下溢错误中断
 *            SDIO_IT_RXOVERR    - 接收 FIFO 上溢错误中断
 *            SDIO_IT_CMDREND    - 命令响应接收完成（CRC 通过）中断
 *            SDIO_IT_CMDSENT    - 命令发送完成（无需响应）中断
 *            SDIO_IT_DATAEND    - 数据传输结束中断
 *            SDIO_IT_STBITERR   - 宽总线模式下起始位错误中断
 *            SDIO_IT_DBCKEND    - 数据块发送/接收完成（CRC 通过）中断
 *            SDIO_IT_CMDACT     - 命令传输进行中中断
 *            SDIO_IT_TXACT      - 数据发送进行中中断
 *            SDIO_IT_RXACT      - 数据接收进行中中断
 *            SDIO_IT_TXFIFOHE   - 发送 FIFO 半空中断
 *            SDIO_IT_RXFIFOHF   - 接收 FIFO 半满中断
 *            SDIO_IT_TXFIFOF    - 发送 FIFO 满中断
 *            SDIO_IT_RXFIFOF    - 接收 FIFO 满中断
 *            SDIO_IT_TXFIFOE    - 发送 FIFO 空中断
 *            SDIO_IT_RXFIFOE    - 接收 FIFO 空中断
 *            SDIO_IT_TXDAVL     - 发送 FIFO 数据可用中断
 *            SDIO_IT_RXDAVL     - 接收 FIFO 数据可用中断
 *            SDIO_IT_SDIOIT     - SD I/O 卡中断接收中断
 *            SDIO_IT_CEATAEND   - CE-ATA 命令完成信号接收中断
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_ITConfig(uint32_t SDIO_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        SDIO->MASK |= SDIO_IT;      /* 使能对应的中断屏蔽位 */
    }
    else
    {
        SDIO->MASK &= ~SDIO_IT;     /* 禁用对应的中断屏蔽位 */
    }
}

/*********************************************************************
 * @fn      SDIO_DMACmd
 *
 * @brief   使能或禁用 SDIO 的 DMA 请求。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_DMACmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 3);    /* 设置 DCTRL 的 bit3 使能 DMA */
    else
        SDIO->DCTRL &= ~(1 << 3);   /* 清除 bit3 禁用 DMA */
}

/*********************************************************************
 * @fn      SDIO_SendCommand
 *
 * @brief   根据 SDIO_CmdInitStruct 中的参数初始化命令并发送。
 *
 * @param   SDIO_CmdInitStruct - 指向 SDIO_CmdInitTypeDef 结构体的指针，
 *                                包含 SDIO 命令的配置信息。
 *
 * @return  无
 */
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct)
{
    uint32_t tmpreg = 0;

    /* 设置命令参数寄存器 */
    SDIO->ARG = SDIO_CmdInitStruct->SDIO_Argument;

    tmpreg = SDIO->CMD;
    tmpreg &= CMD_CLEAR_MASK;   /* 清除命令寄存器中需配置的位域 */
    /* 组合新的命令配置 */
    tmpreg |= (uint32_t)SDIO_CmdInitStruct->SDIO_CmdIndex | 
              SDIO_CmdInitStruct->SDIO_Response | 
              SDIO_CmdInitStruct->SDIO_Wait | 
              SDIO_CmdInitStruct->SDIO_CPSM;

    SDIO->CMD = tmpreg;         /* 写入命令寄存器，启动命令发送 */
}

/*********************************************************************
 * @fn      SDIO_CmdStructInit
 *
 * @brief   将 SDIO_CmdInitStruct 中的每个成员初始化为默认值。
 *
 * @param   SDIO_CmdInitStruct - 指向 SDIO_CmdInitTypeDef 结构体的指针，
 *                                将被初始化。
 *
 * @return  无
 */
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct)
{
    SDIO_CmdInitStruct->SDIO_Argument = 0x00;          /* 参数默认为0 */
    SDIO_CmdInitStruct->SDIO_CmdIndex = 0x00;          /* 命令索引默认为0 */
    SDIO_CmdInitStruct->SDIO_Response = SDIO_Response_No; /* 默认无需响应 */
    SDIO_CmdInitStruct->SDIO_Wait = SDIO_Wait_No;      /* 默认无等待 */
    SDIO_CmdInitStruct->SDIO_CPSM = SDIO_CPSM_Disable; /* 默认禁止命令状态机 */
}

/*********************************************************************
 * @fn      SDIO_GetCommandResponse
 *
 * @brief   返回最后接收到响应的命令的命令索引。
 *
 * @return  最后接收到响应的命令索引。
 */
uint8_t SDIO_GetCommandResponse(void)
{
    /* RESPCMD 寄存器保存了最后收到响应的命令索引 */
    return (uint8_t)(SDIO->RESPCMD);
}

/*********************************************************************
 * @fn      SDIO_GetResponse
 *
 * @brief   返回上次命令从卡接收到的响应。
 *
 * @param   SDIO_RESP - 指定要读取的响应寄存器。
 *            SDIO_RESP1 - 响应寄存器1
 *            SDIO_RESP2 - 响应寄存器2
 *            SDIO_RESP3 - 响应寄存器3
 *            SDIO_RESP4 - 响应寄存器4
 *
 * @return  响应寄存器的值。
 */
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP)
{
    __IO uint32_t tmp = 0;

    /* 计算对应响应寄存器的地址（偏移量） */
    tmp = SDIO_RESP_ADDR + SDIO_RESP;

    /* 返回该地址的内容 */
    return (*(__IO uint32_t *)tmp);
}

/*********************************************************************
 * @fn      SDIO_DataConfig
 *
 * @brief   根据 SDIO_DataInitStruct 中的参数配置 SDIO 数据路径。
 *
 * @param   SDIO_DataInitStruct - 指向 SDIO_DataInitTypeDef 结构体的指针，
 *                                 包含数据配置信息。
 *
 * @return  无
 */
void SDIO_DataConfig(SDIO_DataInitTypeDef *SDIO_DataInitStruct)
{
    uint32_t tmpreg = 0;

    /* 设置数据超时定时器 */
    SDIO->DTIMER = SDIO_DataInitStruct->SDIO_DataTimeOut;
    /* 设置数据长度 */
    SDIO->DLEN   = SDIO_DataInitStruct->SDIO_DataLength;

    tmpreg = SDIO->DCTRL;
    tmpreg &= DCTRL_CLEAR_MASK;   /* 清除需要配置的位域 */
    /* 组合新的数据控制配置 */
    tmpreg |= (uint32_t)SDIO_DataInitStruct->SDIO_DataBlockSize | 
              SDIO_DataInitStruct->SDIO_TransferDir | 
              SDIO_DataInitStruct->SDIO_TransferMode | 
              SDIO_DataInitStruct->SDIO_DPSM;

    SDIO->DCTRL = tmpreg;         /* 写入数据控制寄存器，启动数据状态机 */
}

/*********************************************************************
 * @fn      SDIO_DataStructInit
 *
 * @brief   将 SDIO_DataInitStruct 中的每个成员初始化为默认值。
 *
 * @param   SDIO_DataInitStruct - 指向 SDIO_DataInitTypeDef 结构体的指针，
 *                                 将被初始化。
 *
 * @return  无
 */
void SDIO_DataStructInit(SDIO_DataInitTypeDef *SDIO_DataInitStruct)
{
    SDIO_DataInitStruct->SDIO_DataTimeOut = 0xFFFFFFFF;           /* 超时值设为最大 */
    SDIO_DataInitStruct->SDIO_DataLength = 0x00;                  /* 数据长度默认为0 */
    SDIO_DataInitStruct->SDIO_DataBlockSize = SDIO_DataBlockSize_1b; /* 数据块大小：1字节 */
    SDIO_DataInitStruct->SDIO_TransferDir = SDIO_TransferDir_ToCard; /* 传输方向：到卡 */
    SDIO_DataInitStruct->SDIO_TransferMode = SDIO_TransferMode_Block; /* 传输模式：块传输 */
    SDIO_DataInitStruct->SDIO_DPSM = SDIO_DPSM_Disable;           /* 禁止数据路径状态机 */
}

/*********************************************************************
 * @fn      SDIO_GetDataCounter
 *
 * @brief   返回待传输的剩余数据字节数。
 *
 * @return  剩余待传输的数据字节数。
 */
uint32_t SDIO_GetDataCounter(void)
{
    /* DCOUNT 寄存器保存剩余字节数 */
    return SDIO->DCOUNT;
}

/*********************************************************************
 * @fn      SDIO_ReadData
 *
 * @brief   从接收 FIFO 中读取一个数据字（32位）。
 *
 * @return  接收到的数据。
 */
uint32_t SDIO_ReadData(void)
{
    /* 读 FIFO 寄存器将自动从 FIFO 中取出一个字 */
    return SDIO->FIFO;
}

/*********************************************************************
 * @fn      SDIO_WriteData
 *
 * @brief   向发送 FIFO 中写入一个数据字（32位）。
 *
 * @param   Data - 要写入的32位数据。
 *
 * @return  无
 */
void SDIO_WriteData(uint32_t Data)
{
    /* 写 FIFO 寄存器将数据压入发送 FIFO */
    SDIO->FIFO = Data;
}

/*********************************************************************
 * @fn      SDIO_GetFIFOCount
 *
 * @brief   返回 FIFO 中剩余待写入或待读取的字数。
 *
 * @return  剩余的字数。
 */
uint32_t SDIO_GetFIFOCount(void)
{
    /* FIFOCNT 寄存器表示 FIFO 中当前的字节数（以字为单位？实际上以字节为单位） */
    return SDIO->FIFOCNT;
}

/*********************************************************************
 * @fn      SDIO_StartSDIOReadWait
 *
 * @brief   启动 SD I/O 读等待操作。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_StartSDIOReadWait(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 8);      /* 设置 DCTRL 的 bit8 使能读等待开始 */
    else
        SDIO->DCTRL &= ~(1 << 8);
}

/*********************************************************************
 * @fn      SDIO_StopSDIOReadWait
 *
 * @brief   停止 SD I/O 读等待操作。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_StopSDIOReadWait(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 9);      /* 设置 DCTRL 的 bit9 使能读等待停止 */
    else
        SDIO->DCTRL &= ~(1 << 9);
}

/*********************************************************************
 * @fn      SDIO_SetSDIOReadWaitMode
 *
 * @brief   设置插入读等待间隔的两种模式之一。
 *
 * @param   SDIO_ReadWaitMode - SD I/O 读等待操作模式。
 *            SDIO_ReadWaitMode_CLK    : 通过停止 SDIOCLK 控制读等待
 *            SDIO_ReadWaitMode_DATA2  : 通过 SDIO_DATA2 信号控制读等待
 *
 * @return  无
 */
void SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode)
{
    if(SDIO_ReadWaitMode)
        SDIO->DCTRL |= (1 << 10);     /* 设置 DCTRL 的 bit10 选择模式 */
    else
        SDIO->DCTRL &= ~(1 << 10);
}

/*********************************************************************
 * @fn      SDIO_SetSDIOOperation
 *
 * @brief   使能或禁用 SD I/O 模式操作。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_SetSDIOOperation(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 11);     /* 设置 DCTRL 的 bit11 使能 SD I/O 操作 */
    else
        SDIO->DCTRL &= ~(1 << 11);
}

/*********************************************************************
 * @fn      SDIO_SendSDIOSuspendCmd
 *
 * @brief   使能或禁用发送 SD I/O 模式挂起命令。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_SendSDIOSuspendCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 11);       /* 设置 CMD 寄存器的 bit11 请求挂起 */
    else
        SDIO->CMD &= ~(1 << 11);
}

/*********************************************************************
 * @fn      SDIO_CommandCompletionCmd
 *
 * @brief   使能或禁用命令完成信号。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_CommandCompletionCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 12);       /* 设置 CMD 寄存器的 bit12 使能命令完成 */
    else
        SDIO->CMD &= ~(1 << 12);
}

/*********************************************************************
 * @fn      SDIO_CEATAITCmd
 *
 * @brief   使能或禁用 CE-ATA 中断。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_CEATAITCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 13);       /* 设置 CMD 寄存器的 bit13 使能 CE-ATA 中断 */
    else
        SDIO->CMD &= ~(1 << 13);
}

/*********************************************************************
 * @fn      SDIO_SendCEATACmd
 *
 * @brief   发送 CE-ATA 命令（CMD61）。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SDIO_SendCEATACmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 14);       /* 设置 CMD 寄存器的 bit14 使能 CE-ATA 命令 */
    else
        SDIO->CMD &= ~(1 << 14);
}

/*********************************************************************
 * @fn      SDIO_GetFlagStatus
 *
 * @brief   检查指定的 SDIO 标志是否被置位。
 *
 * @param   SDIO_FLAG - 指定要检查的标志，可以是以下之一：
 *            SDIO_FLAG_CCRCFAIL   - 命令响应 CRC 校验失败
 *            SDIO_FLAG_DCRCFAIL   - 数据块 CRC 校验失败
 *            SDIO_FLAG_CTIMEOUT   - 命令响应超时
 *            SDIO_FLAG_DTIMEOUT   - 数据超时
 *            SDIO_FLAG_TXUNDERR   - 发送 FIFO 下溢错误
 *            SDIO_FLAG_RXOVERR    - 接收 FIFO 上溢错误
 *            SDIO_FLAG_CMDREND    - 命令响应接收完成（CRC 通过）
 *            SDIO_FLAG_CMDSENT    - 命令发送完成（无响应）
 *            SDIO_FLAG_DATAEND    - 数据结束（数据计数器归零）
 *            SDIO_FLAG_STBITERR   - 宽总线模式下起始位错误
 *            SDIO_FLAG_DBCKEND    - 数据块发送/接收完成（CRC 通过）
 *            SDIO_FLAG_CMDACT     - 命令传输进行中
 *            SDIO_FLAG_TXACT      - 数据发送进行中
 *            SDIO_FLAG_RXACT      - 数据接收进行中
 *            SDIO_FLAG_TXFIFOHE   - 发送 FIFO 半空
 *            SDIO_FLAG_RXFIFOHF   - 接收 FIFO 半满
 *            SDIO_FLAG_TXFIFOF    - 发送 FIFO 满
 *            SDIO_FLAG_RXFIFOF    - 接收 FIFO 满
 *            SDIO_FLAG_TXFIFOE    - 发送 FIFO 空
 *            SDIO_FLAG_RXFIFOE    - 接收 FIFO 空
 *            SDIO_FLAG_TXDAVL     - 发送 FIFO 数据可用
 *            SDIO_FLAG_RXDAVL     - 接收 FIFO 数据可用
 *            SDIO_FLAG_SDIOIT     - SD I/O 卡中断接收
 *            SDIO_FLAG_CEATAEND   - CE-ATA 命令完成信号接收
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((SDIO->STA & SDIO_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      SDIO_ClearFlag
 *
 * @brief   清除 SDIO 的挂起标志。
 *
 * @param   SDIO_FLAG - 指定要清除的标志，可以是以下值的组合：
 *            SDIO_FLAG_CCRCFAIL   - 命令响应 CRC 校验失败
 *            SDIO_FLAG_DCRCFAIL   - 数据块 CRC 校验失败
 *            SDIO_FLAG_CTIMEOUT   - 命令响应超时
 *            SDIO_FLAG_DTIMEOUT   - 数据超时
 *            SDIO_FLAG_TXUNDERR   - 发送 FIFO 下溢错误
 *            SDIO_FLAG_RXOVERR    - 接收 FIFO 上溢错误
 *            SDIO_FLAG_CMDREND    - 命令响应接收完成（CRC 通过）
 *            SDIO_FLAG_CMDSENT    - 命令发送完成（无响应）
 *            SDIO_FLAG_DATAEND    - 数据结束
 *            SDIO_FLAG_STBITERR   - 宽总线模式下起始位错误
 *            SDIO_FLAG_DBCKEND    - 数据块发送/接收完成（CRC 通过）
 *            SDIO_FLAG_SDIOIT     - SD I/O 卡中断接收
 *            SDIO_FLAG_CEATAEND   - CE-ATA 命令完成信号接收
 *
 * @return  无
 */
void SDIO_ClearFlag(uint32_t SDIO_FLAG)
{
    /* 向中断清除寄存器写入要清除的标志位（写1清除） */
    SDIO->ICR = SDIO_FLAG;
}

/*********************************************************************
 * @fn      SDIO_GetITStatus
 *
 * @brief   检查指定的 SDIO 中断是否发生。
 *
 * @param   SDIO_IT - 指定要检查的中断源，可以是以下之一：
 *            SDIO_IT_CCRCFAIL   - 命令响应 CRC 校验失败中断
 *            SDIO_IT_DCRCFAIL   - 数据块 CRC 校验失败中断
 *            SDIO_IT_CTIMEOUT   - 命令响应超时中断
 *            SDIO_IT_DTIMEOUT   - 数据超时中断
 *            SDIO_IT_TXUNDERR   - 发送 FIFO 下溢错误中断
 *            SDIO_IT_RXOVERR    - 接收 FIFO 上溢错误中断
 *            SDIO_IT_CMDREND    - 命令响应接收完成中断
 *            SDIO_IT_CMDSENT    - 命令发送完成中断
 *            SDIO_IT_DATAEND    - 数据结束中断
 *            SDIO_IT_STBITERR   - 起始位错误中断
 *            SDIO_IT_DBCKEND    - 数据块发送/接收完成中断
 *            SDIO_IT_CMDACT     - 命令传输进行中中断
 *            SDIO_IT_TXACT      - 数据发送进行中中断
 *            SDIO_IT_RXACT      - 数据接收进行中中断
 *            SDIO_IT_TXFIFOHE   - 发送 FIFO 半空中断
 *            SDIO_IT_RXFIFOHF   - 接收 FIFO 半满中断
 *            SDIO_IT_TXFIFOF    - 发送 FIFO 满中断
 *            SDIO_IT_RXFIFOF    - 接收 FIFO 满中断
 *            SDIO_IT_TXFIFOE    - 发送 FIFO 空中断
 *            SDIO_IT_RXFIFOE    - 接收 FIFO 空中断
 *            SDIO_IT_TXDAVL     - 发送 FIFO 数据可用中断
 *            SDIO_IT_RXDAVL     - 接收 FIFO 数据可用中断
 *            SDIO_IT_SDIOIT     - SD I/O 卡中断接收中断
 *            SDIO_IT_CEATAEND   - CE-ATA 命令完成信号接收中断
 *
 * @return  ITStatus - SET 或 RESET。
 */
ITStatus SDIO_GetITStatus(uint32_t SDIO_IT)
{
    ITStatus bitstatus = RESET;

    if((SDIO->STA & SDIO_IT) != (uint32_t)RESET)
    {
        bitstatus = SET;   /* 状态寄存器中对应中断标志置位 */
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      SDIO_ClearITPendingBit
 *
 * @brief   清除 SDIO 的中断挂起位。
 *
 * @param   SDIO_IT - 指定要清除的中断挂起位，可以是以下值的组合：
 *            SDIO_IT_CCRCFAIL   - 命令响应 CRC 校验失败中断
 *            SDIO_IT_DCRCFAIL   - 数据块 CRC 校验失败中断
 *            SDIO_IT_CTIMEOUT   - 命令响应超时中断
 *            SDIO_IT_DTIMEOUT   - 数据超时中断
 *            SDIO_IT_TXUNDERR   - 发送 FIFO 下溢错误中断
 *            SDIO_IT_RXOVERR    - 接收 FIFO 上溢错误中断
 *            SDIO_IT_CMDREND    - 命令响应接收完成中断
 *            SDIO_IT_CMDSENT    - 命令发送完成中断
 *            SDIO_IT_DATAEND    - 数据结束中断
 *            SDIO_IT_STBITERR   - 起始位错误中断
 *            SDIO_IT_SDIOIT     - SD I/O 卡中断接收中断
 *            SDIO_IT_CEATAEND   - CE-ATA 命令完成信号接收中断
 *
 * @return  无
 */
void SDIO_ClearITPendingBit(uint32_t SDIO_IT)
{
    /* 向中断清除寄存器写入要清除的标志位（写1清除） */
    SDIO->ICR = SDIO_IT;
}