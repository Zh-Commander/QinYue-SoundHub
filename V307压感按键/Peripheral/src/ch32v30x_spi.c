/********************************** (C) COPYRIGHT *******************************
* 文件名            : ch32v30x_spi.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 该文件提供了所有 SPI 固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）和二进制文件可用于南京沁恒微电子股份有限公司生产的微控制器。
*******************************************************************************/

#include "ch32v30x_spi.h"
#include "ch32v30x_rcc.h"

/* 以下为 SPI 寄存器位掩码定义 */

/* SPI 控制寄存器1 (CTLR1) 中 SPI 使能位 (SPE) 的置位和复位掩码 */
#define CTLR1_SPE_Set         ((uint16_t)0x0040)  /* 置位 SPE 位 */
#define CTLR1_SPE_Reset       ((uint16_t)0xFFBF)  /* 清除 SPE 位 (保留其他位) */

/* SPI I2S 配置寄存器 (I2SCFGR) 中 I2S 使能位 (I2SE) 的置位和复位掩码 */
#define I2SCFGR_I2SE_Set      ((uint16_t)0x0400)  /* 置位 I2SE 位 */
#define I2SCFGR_I2SE_Reset    ((uint16_t)0xFBFF)  /* 清除 I2SE 位 */

/* SPI CTLR1 中 CRC 发送下一个数据位 (CRCNext) 的置位掩码 */
#define CTLR1_CRCNext_Set     ((uint16_t)0x1000)  /* 置位 CRCNext，指示下一个数据发送 CRC */

/* SPI CTLR1 中 CRC 使能位 (CRCEN) 的置位和复位掩码 */
#define CTLR1_CRCEN_Set       ((uint16_t)0x2000)  /* 使能 CRC 计算 */
#define CTLR1_CRCEN_Reset     ((uint16_t)0xDFFF)  /* 禁用 CRC 计算 */

/* SPI 控制寄存器2 (CTLR2) 中 SS 输出使能位 (SSOE) 的置位和复位掩码 */
#define CTLR2_SSOE_Set        ((uint16_t)0x0004)  /* 使能 SS 输出 */
#define CTLR2_SSOE_Reset      ((uint16_t)0xFFFB)  /* 禁用 SS 输出 */

/* 用于配置 CTLR1 时清除相关位的掩码（保留其他位） */
#define CTLR1_CLEAR_Mask      ((uint16_t)0x3040)  /* 清除位: 位12,13,14? 具体为 BR[2:0], CPOL, CPHA, 等 */

/* I2SCFGR 寄存器清除掩码（保留位用于 I2S 配置） */
#define I2SCFGR_CLEAR_Mask    ((uint16_t)0xF040)  /* 清除位: 位6-9, 位0-5? 具体清除 I2S 相关位 */

/* 用于 SPI 或 I2S 模式选择的掩码 */
#define SPI_Mode_Select       ((uint16_t)0xF7FF)  /* 清除 I2S 模式位 (bit11) 以选择 SPI 模式 */
#define I2S_Mode_Select       ((uint16_t)0x0800)  /* 置位 I2S 模式位 (bit11) 以选择 I2S 模式 */

/* I2S 时钟源选择掩码（用于 SPI2 和 SPI3 的时钟源选择） */
#define I2S2_CLOCK_SRC        ((uint32_t)(0x00020000)) /* SPI2 的 I2S 时钟源选择位 (在 RCC 配置中) */
#define I2S3_CLOCK_SRC        ((uint32_t)(0x00040000)) /* SPI3 的 I2S 时钟源选择位 */
#define I2S_MUL_MASK          ((uint32_t)(0x0000F000)) /* I2S 倍频系数掩码 */
#define I2S_DIV_MASK          ((uint32_t)(0x000000F0)) /* I2S 分频系数掩码 */

/*********************************************************************
 * @fn      SPI_I2S_DeInit
 *
 * @brief   将 SPIx 外设寄存器复位到默认值（也会影响 I2S）。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *
 * @return  无
 */
void SPI_I2S_DeInit(SPI_TypeDef *SPIx)
{
    if(SPIx == SPI1)
    {
        /* 复位 SPI1 外设 (位于 APB2 总线上) */
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_SPI1, DISABLE);
    }
    else if(SPIx == SPI2)
    {
        /* 复位 SPI2 外设 (位于 APB1 总线上) */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2, DISABLE);
    }
    else
    {
        if(SPIx == SPI3)
        {
            /* 复位 SPI3 外设 (位于 APB1 总线上) */
            RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, ENABLE);
            RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI3, DISABLE);
        }
    }
}

/*********************************************************************
 * @fn      SPI_Init
 *
 * @brief   根据 SPI_InitStruct 中的参数初始化 SPIx 外设。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要初始化的 SPI 外设。
 *          SPI_InitStruct - 指向 SPI_InitTypeDef 结构体的指针，
 *                            包含指定 SPI 外设的配置信息。
 *
 * @return  无
 */
void SPI_Init(SPI_TypeDef *SPIx, SPI_InitTypeDef *SPI_InitStruct)
{
    uint16_t tmpreg = 0;

    /* 读取当前 CTLR1 寄存器值 */
    tmpreg = SPIx->CTLR1;
    /* 清除需要重新配置的位域 (BR[2:0], CPOL, CPHA, 等) */
    tmpreg &= CTLR1_CLEAR_Mask;
    /* 组合新的配置值 */
    tmpreg |= (uint16_t)((uint32_t)SPI_InitStruct->SPI_Direction | 
                         SPI_InitStruct->SPI_Mode |
                         SPI_InitStruct->SPI_DataSize | 
                         SPI_InitStruct->SPI_CPOL |
                         SPI_InitStruct->SPI_CPHA | 
                         SPI_InitStruct->SPI_NSS |
                         SPI_InitStruct->SPI_BaudRatePrescaler | 
                         SPI_InitStruct->SPI_FirstBit);

    /* 写入新的配置到 CTLR1 */
    SPIx->CTLR1 = tmpreg;
    /* 清除 I2S 模式位，确保工作在 SPI 模式 */
    SPIx->I2SCFGR &= SPI_Mode_Select;
    /* 设置 CRC 多项式 */
    SPIx->CRCR = SPI_InitStruct->SPI_CRCPolynomial;
}

/*********************************************************************
 * @fn      I2S_Init
 *
 * @brief   根据 I2S_InitStruct 中的参数初始化 SPIx 外设（配置为 I2S 模式）。
 *
 * @param   SPIx - 可以是 SPI2 或 SPI3，选择要初始化的 SPI 外设（配置为 I2S 模式）。
 *          I2S_InitStruct - 指向 I2S_InitTypeDef 结构体的指针，
 *                            包含指定 SPI 外设配置为 I2S 模式时的配置信息。
 *
 * @return  无
 */
void I2S_Init(SPI_TypeDef *SPIx, I2S_InitTypeDef *I2S_InitStruct)
{
    uint16_t          tmpreg = 0, i2sdiv = 2, i2sodd = 0, packetlength = 1;
    uint32_t          tmp = 0;
    RCC_ClocksTypeDef RCC_Clocks;
    uint32_t          sourceclock = 0;

    /* 清除 I2SCFGR 中需要重新配置的位域 */
    SPIx->I2SCFGR &= I2SCFGR_CLEAR_Mask;
    /* 设置 I2SPR 默认值 (分频系数为2，奇数因子为0) */
    SPIx->I2SPR = 0x0002;
    /* 读取当前 I2SCFGR 值 */
    tmpreg = SPIx->I2SCFGR;

    /* 计算 I2S 分频系数 */
    if(I2S_InitStruct->I2S_AudioFreq == I2S_AudioFreq_Default)
    {
        /* 使用默认频率：分频系数设为2，奇数因子0 */
        i2sodd = (uint16_t)0;
        i2sdiv = (uint16_t)2;
    }
    else
    {
        /* 根据数据格式确定每个数据包的长度（16位或32位） */
        if(I2S_InitStruct->I2S_DataFormat == I2S_DataFormat_16b)
        {
            packetlength = 1;  /* 16位数据，每包1个16位数据 */
        }
        else
        {
            packetlength = 2;  /* 24位或32位数据，每包2个16位数据 */
        }

        /* 获取 I2S 时钟源（根据 SPI 实例选择） */
        if(((uint32_t)SPIx) == SPI2_BASE)
        {
            tmp = I2S2_CLOCK_SRC;   /* SPI2 的 I2S 时钟源选择位 */
        }
        else
        {
            tmp = I2S3_CLOCK_SRC;   /* SPI3 的 I2S 时钟源选择位 */
        }

        /* 获取系统时钟频率 */
        RCC_GetClocksFreq(&RCC_Clocks);
        sourceclock = RCC_Clocks.SYSCLK_Frequency;  /* I2S 时钟通常来自系统时钟 */

        /* 计算所需的分频系数（包含小数部分） */
        if(I2S_InitStruct->I2S_MCLKOutput == I2S_MCLKOutput_Enable)
        {
            /* 使能主时钟输出 (MCLK)，公式: (sourceclock / 256) / AudioFreq */
            tmp = (uint16_t)(((((sourceclock / 256) * 10) / I2S_InitStruct->I2S_AudioFreq)) + 5);
        }
        else
        {
            /* 无 MCLK 输出，公式: (sourceclock / (32 * packetlength)) / AudioFreq */
            tmp = (uint16_t)(((((sourceclock / (32 * packetlength)) * 10) / I2S_InitStruct->I2S_AudioFreq)) + 5);
        }

        /* 四舍五入后除以10得到整型分频系数 */
        tmp = tmp / 10;
        i2sodd = (uint16_t)(tmp & (uint16_t)0x0001);  /* 取最低位作为奇数因子 */
        i2sdiv = (uint16_t)((tmp - i2sodd) / 2);      /* 计算分频系数 */
        i2sodd = (uint16_t)(i2sodd << 8);             /* 将奇数因子左移到 I2SPR 的正确位置 */
    }

    /* 检查分频系数是否在有效范围内，若无效则设为最小值2 */
    if((i2sdiv < 2) || (i2sdiv > 0xFF))
    {
        i2sdiv = 2;
        i2sodd = 0;
    }

    /* 设置 I2S 分频寄存器 I2SPR */
    SPIx->I2SPR = (uint16_t)(i2sdiv | (uint16_t)(i2sodd | (uint16_t)I2S_InitStruct->I2S_MCLKOutput));

    /* 配置 I2S 模式、标准、数据格式和时钟极性 */
    tmpreg |= (uint16_t)(I2S_Mode_Select |                    /* 选择 I2S 模式 */
                         (uint16_t)(I2S_InitStruct->I2S_Mode |
                         (uint16_t)(I2S_InitStruct->I2S_Standard | 
                         (uint16_t)(I2S_InitStruct->I2S_DataFormat |
                         (uint16_t)I2S_InitStruct->I2S_CPOL))));
    /* 写入 I2SCFGR */
    SPIx->I2SCFGR = tmpreg;
}

/*********************************************************************
 * @fn      SPI_StructInit
 *
 * @brief   将 SPI_InitStruct 中的每个成员初始化为默认值。
 *
 * @param   SPI_InitStruct - 指向 SPI_InitTypeDef 结构体的指针，将被初始化。
 *
 * @return  无
 */
void SPI_StructInit(SPI_InitTypeDef *SPI_InitStruct)
{
    /* 默认值：全双工、从模式、8位数据、低电平空闲、第一个边沿采样、分频2、高位在前、CRC多项式7 */
    SPI_InitStruct->SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct->SPI_Mode = SPI_Mode_Slave;
    SPI_InitStruct->SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct->SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct->SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct->SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    SPI_InitStruct->SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct->SPI_CRCPolynomial = 7;
}

/*********************************************************************
 * @fn      I2S_StructInit
 *
 * @brief   将 I2S_InitStruct 中的每个成员初始化为默认值。
 *
 * @param   I2S_InitStruct - 指向 I2S_InitTypeDef 结构体的指针，将被初始化。
 *
 * @return  无
 */
void I2S_StructInit(I2S_InitTypeDef *I2S_InitStruct)
{
    /* 默认值：从发送模式、飞利浦标准、16位数据、禁止MCLK输出、默认音频频率、低电平空闲 */
    I2S_InitStruct->I2S_Mode = I2S_Mode_SlaveTx;
    I2S_InitStruct->I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStruct->I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStruct->I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStruct->I2S_AudioFreq = I2S_AudioFreq_Default;
    I2S_InitStruct->I2S_CPOL = I2S_CPOL_Low;
}

/*********************************************************************
 * @fn      SPI_Cmd
 *
 * @brief   使能或禁用指定的 SPI 外设。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SPI_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        SPIx->CTLR1 |= CTLR1_SPE_Set;      /* 设置 SPE 位，使能 SPI */
    }
    else
    {
        SPIx->CTLR1 &= CTLR1_SPE_Reset;    /* 清除 SPE 位，禁用 SPI */
    }
}

/*********************************************************************
 * @fn      I2S_Cmd
 *
 * @brief   使能或禁用指定的 SPI 外设（在 I2S 模式下）。
 *
 * @param   SPIx - 可以是 SPI2 或 SPI3，选择要操作的 SPI 外设（I2S 模式）。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void I2S_Cmd(SPI_TypeDef *SPIx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        SPIx->I2SCFGR |= I2SCFGR_I2SE_Set;   /* 设置 I2SE 位，使能 I2S */
    }
    else
    {
        SPIx->I2SCFGR &= I2SCFGR_I2SE_Reset; /* 清除 I2SE 位，禁用 I2S */
    }
}

/*********************************************************************
 * @fn      SPI_I2S_ITConfig
 *
 * @brief   使能或禁用指定的 SPI/I2S 中断。
 *
 * @param   SPIx - 可以是
 *            - SPI1, SPI2 或 SPI3（SPI 模式）
 *            - SPI2 或 SPI3（I2S 模式）
 *          SPI_I2S_IT - 指定要配置的中断源，可以是以下值的组合：
 *            SPI_I2S_IT_TXE  - 发送缓冲区空中断
 *            SPI_I2S_IT_RXNE - 接收缓冲区非空中断
 *            SPI_I2S_IT_ERR  - 错误中断
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SPI_I2S_ITConfig(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT, FunctionalState NewState)
{
    uint16_t itpos = 0, itmask = 0;

    /* 解析中断位位置：高4位指示在 CTLR2 中的偏移 */
    itpos = SPI_I2S_IT >> 4;
    /* 生成对应位的掩码 */
    itmask = (uint16_t)1 << (uint16_t)itpos;

    if(NewState != DISABLE)
    {
        SPIx->CTLR2 |= itmask;      /* 使能中断 */
    }
    else
    {
        SPIx->CTLR2 &= (uint16_t)~itmask; /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      SPI_I2S_DMACmd
 *
 * @brief   使能或禁用 SPIx/I2Sx 的 DMA 接口。
 *
 * @param   SPIx - 可以是
 *            - SPI1, SPI2 或 SPI3（SPI 模式）
 *            - SPI2 或 SPI3（I2S 模式）
 *          SPI_I2S_DMAReq - 指定要配置的 DMA 请求，可以是以下值的组合：
 *            SPI_I2S_DMAReq_Tx - 发送缓冲区 DMA 请求
 *            SPI_I2S_DMAReq_Rx - 接收缓冲区 DMA 请求
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SPI_I2S_DMACmd(SPI_TypeDef *SPIx, uint16_t SPI_I2S_DMAReq, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        SPIx->CTLR2 |= SPI_I2S_DMAReq;   /* 使能 DMA 请求 */
    }
    else
    {
        SPIx->CTLR2 &= (uint16_t)~SPI_I2S_DMAReq; /* 禁用 DMA 请求 */
    }
}

/*********************************************************************
 * @fn      SPI_I2S_SendData
 *
 * @brief   通过 SPIx/I2Sx 外设发送一个数据。
 *
 * @param   SPIx - 可以是
 *            - SPI1, SPI2 或 SPI3（SPI 模式）
 *            - SPI2 或 SPI3（I2S 模式）
 *          Data - 要发送的数据。
 *
 * @return  无
 */
void SPI_I2S_SendData(SPI_TypeDef *SPIx, uint16_t Data)
{
    /* 向数据寄存器写入数据，启动发送 */
    SPIx->DATAR = Data;
}

/*********************************************************************
 * @fn      SPI_I2S_ReceiveData
 *
 * @brief   返回 SPIx/I2Sx 外设最近接收到的数据。
 *
 * @param   SPIx - 可以是
 *            - SPI1, SPI2 或 SPI3（SPI 模式）
 *            - SPI2 或 SPI3（I2S 模式）
 *
 * @return  接收到的数据值。
 */
uint16_t SPI_I2S_ReceiveData(SPI_TypeDef *SPIx)
{
    /* 读取数据寄存器，返回接收到的数据 */
    return SPIx->DATAR;
}

/*********************************************************************
 * @fn      SPI_NSSInternalSoftwareConfig
 *
 * @brief   通过软件配置指定 SPI 的 NSS 引脚（内部软件控制）。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *          SPI_NSSInternalSoft - 指定 NSS 引脚的状态：
 *            SPI_NSSInternalSoft_Set   - 内部置位 NSS（高电平）
 *            SPI_NSSInternalSoft_Reset - 内部复位 NSS（低电平）
 *
 * @return  无
 */
void SPI_NSSInternalSoftwareConfig(SPI_TypeDef *SPIx, uint16_t SPI_NSSInternalSoft)
{
    if(SPI_NSSInternalSoft != SPI_NSSInternalSoft_Reset)
    {
        /* 置位 NSS（设置 SSI 位） */
        SPIx->CTLR1 |= SPI_NSSInternalSoft_Set;
    }
    else
    {
        /* 复位 NSS（清除 SSI 位） */
        SPIx->CTLR1 &= SPI_NSSInternalSoft_Reset;
    }
}

/*********************************************************************
 * @fn      SPI_SSOutputCmd
 *
 * @brief   使能或禁用指定 SPI 的 SS 输出（用于主模式下的从选择）。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SPI_SSOutputCmd(SPI_TypeDef *SPIx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 使能 SS 输出（设置 SSOE 位） */
        SPIx->CTLR2 |= CTLR2_SSOE_Set;
    }
    else
    {
        /* 禁用 SS 输出（清除 SSOE 位） */
        SPIx->CTLR2 &= CTLR2_SSOE_Reset;
    }
}

/*********************************************************************
 * @fn      SPI_DataSizeConfig
 *
 * @brief   配置指定 SPI 的数据大小。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *          SPI_DataSize - 指定 SPI 数据大小：
 *            SPI_DataSize_16b - 设置数据帧格式为 16 位
 *            SPI_DataSize_8b  - 设置数据帧格式为 8 位
 *
 * @return  无
 */
void SPI_DataSizeConfig(SPI_TypeDef *SPIx, uint16_t SPI_DataSize)
{
    /* 先清除 DFF 位（数据帧格式位） */
    SPIx->CTLR1 &= (uint16_t)~SPI_DataSize_16b;
    /* 设置新的数据大小 */
    SPIx->CTLR1 |= SPI_DataSize;
}

/*********************************************************************
 * @fn      SPI_TransmitCRC
 *
 * @brief   发送 SPIx 的 CRC 值（在最后一个数据后发送 CRC）。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *
 * @return  无
 */
void SPI_TransmitCRC(SPI_TypeDef *SPIx)
{
    /* 设置 CRCNext 位，指示下一个要发送的是 CRC 值 */
    SPIx->CTLR1 |= CTLR1_CRCNext_Set;
}

/*********************************************************************
 * @fn      SPI_CalculateCRC
 *
 * @brief   使能或禁用传输字节的 CRC 值计算。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void SPI_CalculateCRC(SPI_TypeDef *SPIx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 使能 CRC 计算（设置 CRCEN 位） */
        SPIx->CTLR1 |= CTLR1_CRCEN_Set;
    }
    else
    {
        /* 禁用 CRC 计算（清除 CRCEN 位） */
        SPIx->CTLR1 &= CTLR1_CRCEN_Reset;
    }
}

/*********************************************************************
 * @fn      SPI_GetCRC
 *
 * @brief   返回指定 SPI 的发送或接收 CRC 寄存器值。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *          SPI_CRC - 指定要读取的 CRC 寄存器：
 *            SPI_CRC_Tx - 选择发送 CRC 寄存器 (Tx CRC)
 *            SPI_CRC_Rx - 选择接收 CRC 寄存器 (Rx CRC)
 *
 * @return  所选 CRC 寄存器的值。
 */
uint16_t SPI_GetCRC(SPI_TypeDef *SPIx, uint8_t SPI_CRC)
{
    uint16_t crcreg = 0;

    if(SPI_CRC != SPI_CRC_Rx)
    {
        crcreg = SPIx->TCRCR;   /* 读取发送 CRC 寄存器 */
    }
    else
    {
        crcreg = SPIx->RCRCR;   /* 读取接收 CRC 寄存器 */
    }

    return crcreg;
}

/*********************************************************************
 * @fn      SPI_GetCRCPolynomial
 *
 * @brief   返回指定 SPI 的 CRC 多项式寄存器值。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *
 * @return  CRC 多项式寄存器值。
 */
uint16_t SPI_GetCRCPolynomial(SPI_TypeDef *SPIx)
{
    return SPIx->CRCR;
}

/*********************************************************************
 * @fn      SPI_BiDirectionalLineConfig
 *
 * @brief   在双向模式下选择指定 SPI 的数据传输方向。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3，选择要操作的 SPI 外设。
 *          SPI_Direction - 指定双向模式下的数据传输方向：
 *            SPI_Direction_Tx - 选择发送方向
 *            SPI_Direction_Rx - 选择接收方向
 *
 * @return  无
 */
void SPI_BiDirectionalLineConfig(SPI_TypeDef *SPIx, uint16_t SPI_Direction)
{
    if(SPI_Direction == SPI_Direction_Tx)
    {
        /* 设置 BIDIMODE 和 BIDIOE 位为发送方向 */
        SPIx->CTLR1 |= SPI_Direction_Tx;
    }
    else
    {
        /* 清除 BIDIOE 位（或设置接收方向） */
        SPIx->CTLR1 &= SPI_Direction_Rx;
    }
}

/*********************************************************************
 * @fn      SPI_I2S_GetFlagStatus
 *
 * @brief   检查指定的 SPI/I2S 标志是否被置位。
 *
 * @param   SPIx - 可以是
 *            - SPI1, SPI2 或 SPI3（SPI 模式）
 *            - SPI2 或 SPI3（I2S 模式）
 *          SPI_I2S_FLAG - 指定要检查的标志，可以是以下之一：
 *            SPI_I2S_FLAG_TXE    - 发送缓冲区空标志
 *            SPI_I2S_FLAG_RXNE   - 接收缓冲区非空标志
 *            SPI_I2S_FLAG_BSY    - 忙标志
 *            SPI_I2S_FLAG_OVR    - 溢出错误标志
 *            SPI_FLAG_MODF       - 模式错误标志
 *            SPI_FLAG_CRCERR     - CRC 错误标志
 *            I2S_FLAG_UDR        - 下溢错误标志
 *            I2S_FLAG_CHSIDE     - 通道侧标志
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus SPI_I2S_GetFlagStatus(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* 检查状态寄存器中对应的标志位 */
    if((SPIx->STATR & SPI_I2S_FLAG) != (uint16_t)RESET)
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
 * @fn      SPI_I2S_ClearFlag
 *
 * @brief   清除 SPIx 的 CRC 错误标志 (CRCERR)。
 *
 * @param   SPIx - 可以是
 *            - SPI1, SPI2 或 SPI3（SPI 模式）
 *            - SPI2 或 SPI3（I2S 模式）
 *          SPI_I2S_FLAG - 指定要清除的标志（当前仅支持 SPI_FLAG_CRCERR）。
 *          注意：
 *          - OVR（溢出错误）标志通过软件序列清除：先读 SPI_DATAR 再读 SPI_STATR。
 *          - UDR（下溢错误）标志通过读 SPI_STATR 清除。
 *          - MODF（模式错误）标志通过软件序列清除：先读/写 SPI_STATR，再写 SPI_CTLR1 使能 SPI。
 *
 * @return  无
 */
void SPI_I2S_ClearFlag(SPI_TypeDef *SPIx, uint16_t SPI_I2S_FLAG)
{
    /* 写状态寄存器以清除指定的标志位（写1清除？实际硬件是写0？这里取反写入） */
    SPIx->STATR = (uint16_t)~SPI_I2S_FLAG;
}

/*********************************************************************
 * @fn      SPI_I2S_GetITStatus
 *
 * @brief   检查指定的 SPI/I2S 中断是否发生。
 *
 * @param   SPIx - 可以是
 *            - SPI1, SPI2 或 SPI3（SPI 模式）
 *            - SPI2 或 SPI3（I2S 模式）
 *          SPI_I2S_IT - 指定要检查的中断源，可以是以下之一：
 *            SPI_I2S_IT_TXE    - 发送缓冲区空中断
 *            SPI_I2S_IT_RXNE   - 接收缓冲区非空中断
 *            SPI_I2S_IT_OVR    - 溢出错误中断
 *            SPI_IT_MODF       - 模式错误中断
 *            SPI_IT_CRCERR     - CRC 错误中断
 *            I2S_IT_UDR        - 下溢错误中断
 *
 * @return  ITStatus - SET 或 RESET。
 */
ITStatus SPI_I2S_GetITStatus(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT)
{
    ITStatus bitstatus = RESET;
    uint16_t itpos = 0, itmask = 0, enablestatus = 0;

    /* 解析中断位位置：低4位为状态寄存器中的位偏移 */
    itpos = 0x01 << (SPI_I2S_IT & 0x0F);
    /* 高4位为控制寄存器中的位偏移 */
    itmask = SPI_I2S_IT >> 4;
    itmask = 0x01 << itmask;
    /* 检查中断是否使能 */
    enablestatus = (SPIx->CTLR2 & itmask);

    /* 如果状态位已置位且中断使能，则返回 SET */
    if(((SPIx->STATR & itpos) != (uint16_t)RESET) && enablestatus)
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
 * @fn      SPI_I2S_ClearITPendingBit
 *
 * @brief   清除 SPIx 的 CRC 错误中断挂起位。
 *
 * @param   SPIx - 可以是 SPI1, SPI2 或 SPI3（SPI 模式）。
 *          SPI_I2S_IT - 指定要清除的中断挂起位（当前仅支持 SPI_IT_CRCERR）。
 *          注意：
 *          - OVR（溢出错误）中断挂起位通过软件序列清除：先读 SPI_DATAR 再读 SPI_STATR。
 *          - UDR（下溢错误）中断挂起位通过读 SPI_STATR 清除。
 *          - MODF（模式错误）中断挂起位通过软件序列清除：先读/写 SPI_STATR，再写 SPI_CTLR1 使能 SPI。
 *
 * @return  无
 */
void SPI_I2S_ClearITPendingBit(SPI_TypeDef *SPIx, uint8_t SPI_I2S_IT)
{
    uint16_t itpos = 0;

    /* 计算状态寄存器中的位位置 */
    itpos = 0x01 << (SPI_I2S_IT & 0x0F);
    /* 写状态寄存器以清除该中断标志（写1清除？这里取反写入） */
    SPIx->STATR = (uint16_t)~itpos;
}