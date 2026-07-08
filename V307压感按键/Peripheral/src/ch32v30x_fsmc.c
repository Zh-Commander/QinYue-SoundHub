/********************************** (C) COPYRIGHT  *******************************
* 文件名称          : ch32v30x_fsmc.c
* 作者              : WCH
* 版本              : V1.0.1
* 日期              : 2025/04/06
* 描述              : 该文件提供了FSMC的所有固件功能函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意: 该软件（无论是否经过修改）及二进制文件仅用于
*       南京沁恒微电子生产的微控制器。
*******************************************************************************/

#include "ch32v30x_fsmc.h"
#include "ch32v30x_rcc.h"

/* FSMC 配置寄存器掩码定义（用于按位设置/清除特定功能位）*/
#define BCR_MBKEN_Set          ((uint32_t)0x00000001) /* 设置存储器块使能位（Bit 0）*/
#define BCR_MBKEN_Reset        ((uint32_t)0x000FFFFE) /* 清除存储器块使能位（Bit 0）*/
#define BCR_FACCEN_Set         ((uint32_t)0x00000040) /* 设置Flash访问使能位（Bit 6）*/

/* FSMC 通用寄存器掩码定义 */
#define PCR_PBKEN_Set          ((uint32_t)0x00000004) /* 设置NAND闪存块使能位（Bit 2）*/
#define PCR_PBKEN_Reset        ((uint32_t)0x000FFFFB) /* 清除NAND闪存块使能位（Bit 2）*/
#define PCR_ECCEN_Set          ((uint32_t)0x00000040) /* 设置ECC计算使能位（Bit 6）*/
#define PCR_ECCEN_Reset        ((uint32_t)0x000FFFBF) /* 清除ECC计算使能位（Bit 6）*/
#define PCR_MemoryType_NAND    ((uint32_t)0x00000008) /* NAND存储器类型标识位（Bit 3）*/

/*********************************************************************
 * 函数名称       : FSMC_NORSRAMDeInit
 * 功能描述       : 将FSMC NOR/SRAM存储器块寄存器恢复为默认复位值
 * 输入参数       : FSMC_Bank - 指定要复位的存储器块
 *                  可选值：FSMC_Bank1_NORSRAM1（Bank1 NOR/SRAM1）
 *                  （注：当前仅支持Bank1 NOR/SRAM1的复位，其他Bank复位值不同）
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 该函数会将指定Bank的控制寄存器和写时序寄存器全部复位
 */
void FSMC_NORSRAMDeInit(uint32_t FSMC_Bank)
{
    if(FSMC_Bank == FSMC_Bank1_NORSRAM1)
    {
        FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030DB;  /* NOR/SRAM1的默认复位值 */
    }
    else
    {
        FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030D2;  /* 其他NOR/SRAM Bank的默认复位值 */
    }
    FSMC_Bank1->BTCR[FSMC_Bank + 1] = 0x0FFFFFFF;  /* 复位读/写时序寄存器 */
    FSMC_Bank1E->BWTR[FSMC_Bank] = 0x0FFFFFFF;    /* 复位写时序寄存器（扩展模式） */
}

/*********************************************************************
 * 函数名称       : FSMC_NANDDeInit
 * 功能描述       : 将FSMC NAND存储器块寄存器恢复为默认复位值
 * 输入参数       : FSMC_Bank - 指定要复位的NAND存储器块
 *                  可选值：FSMC_Bank2_NAND（Bank2 NAND）
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 仅复位Bank2 NAND，包括控制寄存器、状态寄存器和时序寄存器
 */
void FSMC_NANDDeInit(uint32_t FSMC_Bank)
{
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        FSMC_Bank2->PCR2 = 0x00000018;    /* NAND控制寄存器默认值 */
        FSMC_Bank2->SR2 = 0x00000040;     /* 状态寄存器默认值（FIFO空标志位） */
        FSMC_Bank2->PMEM2 = 0xFCFCFCFC;   /* 通用存储空间时序寄存器默认值 */
        FSMC_Bank2->PATT2 = 0xFCFCFCFC;   /* 属性存储空间时序寄存器默认值 */
    }
}

/*********************************************************************
 * 函数名称       : FSMC_NORSRAMInit
 * 功能描述       : 根据FSMC_NORSRAMInitStruct中的参数初始化FSMC NOR/SRAM存储器块
 * 输入参数       : FSMC_NORSRAMInitStruct - 指向FSMC_NORSRAMInitTypeDef结构体的指针
 *                  该结构体包含了FSMC NOR/SRAM存储器块的配置信息
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 1. 需要先配置好时序结构体
 *                  2. 如果选择NOR闪存，会自动使能Flash访问
 *                  3. 扩展模式用于独立的读写时序控制
 */
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef *FSMC_NORSRAMInitStruct)
{
    /* 第一步：配置Bank控制寄存器（BCR）*/
    FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] =
        (uint32_t)FSMC_NORSRAMInitStruct->FSMC_DataAddressMux |    /* 地址/数据复用 */
        FSMC_NORSRAMInitStruct->FSMC_MemoryType |                  /* 存储器类型 */
        FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth |             /* 数据总线宽度 */
        FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode |             /* 突发访问模式 */
        FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait |            /* 异步等待 */
        FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity |          /* 等待信号极性 */
        FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive |            /* 等待信号有效时机 */
        FSMC_NORSRAMInitStruct->FSMC_WriteOperation |              /* 写操作使能 */
        FSMC_NORSRAMInitStruct->FSMC_WaitSignal |                  /* 等待信号使能 */
        FSMC_NORSRAMInitStruct->FSMC_ExtendedMode |                /* 扩展模式使能 */
        FSMC_NORSRAMInitStruct->FSMC_WriteBurst;                   /* 写突发使能 */

    /* 如果是NOR闪存，需要额外使能Flash访问 */
    if(FSMC_NORSRAMInitStruct->FSMC_MemoryType == FSMC_MemoryType_NOR)
    {
        FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] |= (uint32_t)BCR_FACCEN_Set;
    }

    /* 第二步：配置Bank时序寄存器（BTR）*/
    FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank + 1] =
        (uint32_t)FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime |  /* 地址建立时间 */
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime << 4) |      /* 地址保持时间 */
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime << 8) |        /* 数据建立时间 */
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration << 16) | /* 总线转换周期 */
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision << 20) |         /* CLK分频系数 */
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency << 24) |         /* 数据延迟 */
        FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode;                   /* 访问模式 */

    /* 第三步：如果使能了扩展模式，需要单独配置写时序 */
    if(FSMC_NORSRAMInitStruct->FSMC_ExtendedMode == FSMC_ExtendedMode_Enable)
    {
        FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] =
            (uint32_t)FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime |   /* 写地址建立时间 */
            (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime << 4) |       /* 写地址保持时间 */
            (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime << 8) |         /* 写数据建立时间 */
            FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode;                    /* 写访问模式 */
    }
    else
    {
        /* 禁用扩展模式时，写时序寄存器使用默认值 */
        FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] = 0x0FFFFFFF;
    }
}

/*********************************************************************
 * 函数名称       : FSMC_NANDInit
 * 功能描述       : 根据FSMC_NANDInitStruct中的参数初始化FSMC NAND存储器块
 * 输入参数       : FSMC_NANDInitStruct - 指向FSMC_NANDInitTypeDef结构体的指针
 *                  该结构体包含了FSMC NAND存储器块的配置信息
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 1. 需要分别配置控制寄存器、通用空间时序和属性空间时序
 *                  2. 当前仅支持Bank2 NAND的初始化
 */
void FSMC_NANDInit(FSMC_NANDInitTypeDef *FSMC_NANDInitStruct)
{
    uint32_t tmppcr = 0x00000000, tmppmem = 0x00000000, tmppatt = 0x00000000;

    /* 构建控制寄存器(PCR)值 */
    tmppcr = (uint32_t)FSMC_NANDInitStruct->FSMC_Waitfeature |     /* 等待特性使能 */
             PCR_MemoryType_NAND |                                 /* NAND存储器类型 */
             FSMC_NANDInitStruct->FSMC_MemoryDataWidth |           /* 数据总线宽度 */
             FSMC_NANDInitStruct->FSMC_ECC |                       /* ECC使能 */
             FSMC_NANDInitStruct->FSMC_ECCPageSize |               /* ECC页大小 */
             (FSMC_NANDInitStruct->FSMC_TCLRSetupTime << 9) |      /* CLE到RE的延迟 */
             (FSMC_NANDInitStruct->FSMC_TARSetupTime << 13);       /* ALE到RE的延迟 */

    /* 构建通用存储空间时序寄存器(PMEM)值 */
    tmppmem = (uint32_t)FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime |      /* 建立时间 */
              (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime << 8) |     /* 等待时间 */
              (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime << 16) |    /* 保持时间 */
              (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime << 24);      /* 高阻时间 */

    /* 构建属性存储空间时序寄存器(PATT)值 */
    tmppatt = (uint32_t)FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime |   /* 建立时间 */
              (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime << 8) |  /* 等待时间 */
              (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime << 16) | /* 保持时间 */
              (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime << 24);   /* 高阻时间 */

    /* 仅初始化Bank2 NAND */
    if(FSMC_NANDInitStruct->FSMC_Bank == FSMC_Bank2_NAND)
    {
        FSMC_Bank2->PCR2 = tmppcr;     /* 写入控制寄存器 */
        FSMC_Bank2->PMEM2 = tmppmem;   /* 写入通用存储空间时序寄存器 */
        FSMC_Bank2->PATT2 = tmppatt;   /* 写入属性存储空间时序寄存器 */
    }
}

/*********************************************************************
 * 函数名称       : FSMC_NORSRAMStructInit
 * 功能描述       : 使用默认值填充FSMC_NORSRAMInitStruct结构体的每个成员
 * 输入参数       : FSMC_NORSRAMInitStruct - 指向要初始化的FSMC_NORSRAMInitTypeDef结构体的指针
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 该函数用于快速初始化结构体，提供一组安全的默认配置
 */
void FSMC_NORSRAMStructInit(FSMC_NORSRAMInitTypeDef *FSMC_NORSRAMInitStruct)
{
    /* Bank选择 */
    FSMC_NORSRAMInitStruct->FSMC_Bank = FSMC_Bank1_NORSRAM1;
    
    /* 基本配置 */
    FSMC_NORSRAMInitStruct->FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;           /* 使能地址/数据复用 */
    FSMC_NORSRAMInitStruct->FSMC_MemoryType = FSMC_MemoryType_SRAM;                     /* SRAM存储器类型 */
    FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;             /* 8位数据宽度 */
    FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;        /* 禁用突发访问 */
    FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;      /* 禁用异步等待 */
    FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;      /* 等待信号低有效 */
    FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState; /* 在等待状态前有效 */
    FSMC_NORSRAMInitStruct->FSMC_WriteOperation = FSMC_WriteOperation_Enable;           /* 使能写操作 */
    FSMC_NORSRAMInitStruct->FSMC_WaitSignal = FSMC_WaitSignal_Enable;                   /* 使能等待信号 */
    FSMC_NORSRAMInitStruct->FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;              /* 禁用扩展模式 */
    FSMC_NORSRAMInitStruct->FSMC_WriteBurst = FSMC_WriteBurst_Disable;                  /* 禁用写突发 */
    
    /* 读/写时序配置（默认值较大，确保兼容性） */
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime = 0xF;    /* 地址建立时间 */
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime = 0xF;     /* 地址保持时间 */
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime = 0xFF;      /* 数据建立时间 */
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration = 0xF; /* 总线转换周期 */
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision = 0xF;         /* CLK分频系数 */
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency = 0xF;         /* 数据延迟 */
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode = FSMC_AccessMode_A; /* 访问模式A */
    
    /* 写时序配置（仅当扩展模式使能时有效） */
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime = 0xF;        /* 写地址建立时间 */
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime = 0xF;         /* 写地址保持时间 */
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime = 0xFF;          /* 写数据建立时间 */
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_BusTurnAroundDuration = 0xF;   /* 写总线转换周期 */
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode = FSMC_AccessMode_A; /* 写访问模式A */
}

/*********************************************************************
 * 函数名称       : FSMC_NANDStructInit
 * 功能描述       : 使用默认值填充FSMC_NANDInitStruct结构体的每个成员
 * 输入参数       : FSMC_NANDInitStruct - 指向要初始化的FSMC_NANDInitTypeDef结构体的指针
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 该函数用于快速初始化结构体，提供一组安全的默认配置
 */
void FSMC_NANDStructInit(FSMC_NANDInitTypeDef *FSMC_NANDInitStruct)
{
    /* Bank选择 */
    FSMC_NANDInitStruct->FSMC_Bank = FSMC_Bank2_NAND;
    
    /* 基本配置 */
    FSMC_NANDInitStruct->FSMC_Waitfeature = FSMC_Waitfeature_Disable;                   /* 禁用等待特性 */
    FSMC_NANDInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;                /* 8位数据宽度 */
    FSMC_NANDInitStruct->FSMC_ECC = FSMC_ECC_Disable;                                   /* 禁用ECC */
    FSMC_NANDInitStruct->FSMC_ECCPageSize = FSMC_ECCPageSize_256Bytes;                  /* ECC页大小256字节 */
    FSMC_NANDInitStruct->FSMC_TCLRSetupTime = 0x0;                                      /* CLE到RE延迟 */
    FSMC_NANDInitStruct->FSMC_TARSetupTime = 0x0;                                       /* ALE到RE延迟 */
    
    /* 通用存储空间时序配置（默认值较大，确保兼容性） */
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime = 0xFC;           /* 建立时间 */
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;       /* 等待时间 */
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;       /* 保持时间 */
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;        /* 高阻时间 */
    
    /* 属性存储空间时序配置 */
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime = 0xFC;        /* 建立时间 */
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;    /* 等待时间 */
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;    /* 保持时间 */
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;     /* 高阻时间 */
}

/*********************************************************************
 * 函数名称       : FSMC_NORSRAMCmd
 * 功能描述       : 使能或禁用指定的NOR/SRAM存储器块
 * 输入参数       : FSMC_Bank - 指定的FSMC存储器块
 *                  可选值：
 *                  FSMC_Bank1_NORSRAM1 - Bank1 NOR/SRAM1
 *                  FSMC_Bank1_NORSRAM2 - Bank1 NOR/SRAM2
 *                  FSMC_Bank1_NORSRAM3 - Bank1 NOR/SRAM3
 *                  FSMC_Bank1_NORSRAM4 - Bank1 NOR/SRAM4
 *                  NewState - 新状态：ENABLE（使能）或 DISABLE（禁用）
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 通过设置/清除BCR寄存器的MBKEN位来控制存储器块的使能状态
 */
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        FSMC_Bank1->BTCR[FSMC_Bank] |= BCR_MBKEN_Set;     /* 设置MBKEN位，使能存储器块 */
    }
    else
    {
        FSMC_Bank1->BTCR[FSMC_Bank] &= BCR_MBKEN_Reset;   /* 清除MBKEN位，禁用存储器块 */
    }
}

/*********************************************************************
 * 函数名称       : FSMC_NANDCmd
 * 功能描述       : 使能或禁用指定的NAND存储器块
 * 输入参数       : FSMC_Bank - 指定的FSMC NAND存储器块
 *                  可选值：FSMC_Bank2_NAND - Bank2 NAND
 *                  NewState - 新状态：ENABLE（使能）或 DISABLE（禁用）
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 通过设置/清除PCR寄存器的PBKEN位来控制NAND存储器块的使能状态
 */
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 |= PCR_PBKEN_Set;      /* 设置PBKEN位，使能NAND存储器块 */
        }
    }
    else
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 &= PCR_PBKEN_Reset;    /* 清除PBKEN位，禁用NAND存储器块 */
        }
    }
}

/*********************************************************************
 * 函数名称       : FSMC_NANDECCCmd
 * 功能描述       : 使能或禁用FSMC NAND的ECC（错误检查和纠正）功能
 * 输入参数       : FSMC_Bank - 指定的FSMC NAND存储器块
 *                  可选值：FSMC_Bank2_NAND - Bank2 NAND
 *                  NewState - 新状态：ENABLE（使能）或 DISABLE（禁用）
 * 输出参数       : 无
 * 返回值         : 无
 * 注意事项       : 1. ECC用于检测和纠正NAND闪存中的位错误
 *                  2. 通过设置/清除PCR寄存器的ECCEN位控制ECC功能
 */
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 |= PCR_ECCEN_Set;      /* 设置ECCEN位，使能ECC功能 */
        }
    }
    else
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 &= PCR_ECCEN_Reset;    /* 清除ECCEN位，禁用ECC功能 */
        }
    }
}

/*********************************************************************
 * 函数名称       : FSMC_GetECC
 * 功能描述       : 读取错误纠正码（ECC）寄存器的值
 * 输入参数       : FSMC_Bank - 指定的FSMC NAND存储器块
 *                  可选值：FSMC_Bank2_NAND - Bank2 NAND
 * 输出参数       : 无
 * 返回值         : eccval - ECC寄存器值（32位）
 * 注意事项       : 1. ECC值在每次NAND页读/写操作后更新
 *                  2. 该值用于验证数据完整性
 */
uint32_t FSMC_GetECC(uint32_t FSMC_Bank)
{
    uint32_t eccval = 0x00000000;   /* 初始化返回值 */

    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        eccval = FSMC_Bank2->ECCR2;  /* 读取Bank2的ECC寄存器值 */
    }

    return (eccval);
}

/*********************************************************************
 * 函数名称       : FSMC_GetFlagStatus
 * 功能描述       : 检查指定的FSMC标志位是否置位
 * 输入参数       : FSMC_Bank - 指定的FSMC存储器块
 *                  可选值：FSMC_Bank2_NAND - Bank2 NAND
 *                  FSMC_FLAG - 要检查的标志位
 *                  可选值：FSMC_FLAG_FEMPT - FIFO空标志
 * 输出参数       : 无
 * 返回值         : FlagStatus - 标志位状态：SET（置位）或RESET（复位）
 * 注意事项       : 主要用于检查NAND控制器的FIFO状态
 */
FlagStatus FSMC_GetFlagStatus(uint32_t FSMC_Bank, uint32_t FSMC_FLAG)
{
    FlagStatus bitstatus = RESET;   /* 默认返回复位状态 */
    uint32_t   tmpsr = 0x00000000;  /* 临时存储状态寄存器值 */

    /* 根据Bank选择读取对应的状态寄存器 */
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        tmpsr = FSMC_Bank2->SR2;
    }

    /* 检查指定的标志位是否置位 */
    if((tmpsr & FSMC_FLAG) != (uint16_t)RESET)
    {
        bitstatus = SET;    /* 标志位置位 */
    }
    else
    {
        bitstatus = RESET;  /* 标志位复位 */
    }

    return bitstatus;
}
