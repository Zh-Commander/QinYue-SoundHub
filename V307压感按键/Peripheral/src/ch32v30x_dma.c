/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_dma.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 本文件提供了所有DMA固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或不修改）和二进制代码用于南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/

#include "ch32v30x_dma.h"
#include "ch32v30x_rcc.h"

/* DMA1 各通道中断挂起位掩码 */
#define DMA1_Channel1_IT_Mask     ((uint32_t)(DMA_GIF1 | DMA_TCIF1 | DMA_HTIF1 | DMA_TEIF1))  /* 通道1全局、传输完成、半传输、传输错误中断掩码 */
#define DMA1_Channel2_IT_Mask     ((uint32_t)(DMA_GIF2 | DMA_TCIF2 | DMA_HTIF2 | DMA_TEIF2))  /* 通道2全局、传输完成、半传输、传输错误中断掩码 */
#define DMA1_Channel3_IT_Mask     ((uint32_t)(DMA_GIF3 | DMA_TCIF3 | DMA_HTIF3 | DMA_TEIF3))  /* 通道3全局、传输完成、半传输、传输错误中断掩码 */
#define DMA1_Channel4_IT_Mask     ((uint32_t)(DMA_GIF4 | DMA_TCIF4 | DMA_HTIF4 | DMA_TEIF4))  /* 通道4全局、传输完成、半传输、传输错误中断掩码 */
#define DMA1_Channel5_IT_Mask     ((uint32_t)(DMA_GIF5 | DMA_TCIF5 | DMA_HTIF5 | DMA_TEIF5))  /* 通道5全局、传输完成、半传输、传输错误中断掩码 */
#define DMA1_Channel6_IT_Mask     ((uint32_t)(DMA_GIF6 | DMA_TCIF6 | DMA_HTIF6 | DMA_TEIF6))  /* 通道6全局、传输完成、半传输、传输错误中断掩码 */
#define DMA1_Channel7_IT_Mask     ((uint32_t)(DMA_GIF7 | DMA_TCIF7 | DMA_HTIF7 | DMA_TEIF7))  /* 通道7全局、传输完成、半传输、传输错误中断掩码 */

/* DMA2 各通道中断挂起位掩码（通道1~7对应DMA2基本部分，通道8~11对应DMA2扩展部分） */
#define DMA2_Channel1_IT_Mask     ((uint32_t)(DMA_GIF1 | DMA_TCIF1 | DMA_HTIF1 | DMA_TEIF1))
#define DMA2_Channel2_IT_Mask     ((uint32_t)(DMA_GIF2 | DMA_TCIF2 | DMA_HTIF2 | DMA_TEIF2))
#define DMA2_Channel3_IT_Mask     ((uint32_t)(DMA_GIF3 | DMA_TCIF3 | DMA_HTIF3 | DMA_TEIF3))
#define DMA2_Channel4_IT_Mask     ((uint32_t)(DMA_GIF4 | DMA_TCIF4 | DMA_HTIF4 | DMA_TEIF4))
#define DMA2_Channel5_IT_Mask     ((uint32_t)(DMA_GIF5 | DMA_TCIF5 | DMA_HTIF5 | DMA_TEIF5))
#define DMA2_Channel6_IT_Mask     ((uint32_t)(DMA_GIF6 | DMA_TCIF6 | DMA_HTIF6 | DMA_TEIF6))
#define DMA2_Channel7_IT_Mask     ((uint32_t)(DMA_GIF7 | DMA_TCIF7 | DMA_HTIF7 | DMA_TEIF7))
#define DMA2_Channel8_IT_Mask     ((uint32_t)(DMA_GIF8 | DMA_TCIF8 | DMA_HTIF8 | DMA_TEIF8))
#define DMA2_Channel9_IT_Mask     ((uint32_t)(DMA_GIF9 | DMA_TCIF9 | DMA_HTIF9 | DMA_TEIF9))
#define DMA2_Channel10_IT_Mask    ((uint32_t)(DMA_GIF10 | DMA_TCIF10 | DMA_HTIF10 | DMA_TEIF10))
#define DMA2_Channel11_IT_Mask    ((uint32_t)(DMA_GIF11 | DMA_TCIF11 | DMA_HTIF11 | DMA_TEIF11))

/* DMA2 标志掩码 */
#define FLAG_Mask                 ((uint32_t)0x10000000)  /* 用于区分DMA2基本部分的标志位 */
#define DMA2_EXTEN_FLAG_Mask      ((uint32_t)0x20000000)  /* 用于区分DMA2扩展部分的标志位 */

/* DMA 寄存器掩码 */
#define CFGR_CLEAR_Mask           ((uint32_t)0xFFFF800F)  /* 清除配置寄存器中除方向、模式、增量、数据大小、优先级、存储器到存储器位以外的其他位 */

/*********************************************************************
 * @fn      DMA_DeInit
 *
 * @brief   将DMAy通道x寄存器复位为默认值。
 *
 * @param   DMAy_Channelx - 选择DMA通道，y可为1或2，x范围：DMA1为1~7，DMA2为1~11。
 *
 * @return  无
 */
void DMA_DeInit(DMA_Channel_TypeDef *DMAy_Channelx)
{
    /* 先禁止通道（清零使能位），然后清除配置寄存器、计数器、外设地址、存储器地址 */
    DMAy_Channelx->CFGR &= (uint16_t)(~DMA_CFGR1_EN);
    DMAy_Channelx->CFGR = 0;
    DMAy_Channelx->CNTR = 0;
    DMAy_Channelx->PADDR = 0;
    DMAy_Channelx->MADDR = 0;

    /* 清除对应通道的中断标志位（写1清除） */
    if(DMAy_Channelx == DMA1_Channel1)
    {
        DMA1->INTFCR |= DMA1_Channel1_IT_Mask;
    }
    else if(DMAy_Channelx == DMA1_Channel2)
    {
        DMA1->INTFCR |= DMA1_Channel2_IT_Mask;
    }
    else if(DMAy_Channelx == DMA1_Channel3)
    {
        DMA1->INTFCR |= DMA1_Channel3_IT_Mask;
    }
    else if(DMAy_Channelx == DMA1_Channel4)
    {
        DMA1->INTFCR |= DMA1_Channel4_IT_Mask;
    }
    else if(DMAy_Channelx == DMA1_Channel5)
    {
        DMA1->INTFCR |= DMA1_Channel5_IT_Mask;
    }
    else if(DMAy_Channelx == DMA1_Channel6)
    {
        DMA1->INTFCR |= DMA1_Channel6_IT_Mask;
    }
    else if(DMAy_Channelx == DMA1_Channel7)
    {
        DMA1->INTFCR |= DMA1_Channel7_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel1)
    {
        DMA2->INTFCR |= DMA2_Channel1_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel2)
    {
        DMA2->INTFCR |= DMA2_Channel2_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel3)
    {
        DMA2->INTFCR |= DMA2_Channel3_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel4)
    {
        DMA2->INTFCR |= DMA2_Channel4_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel5)
    {
        DMA2->INTFCR |= DMA2_Channel5_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel6)
    {
        DMA2->INTFCR |= DMA2_Channel6_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel7)
    {
        DMA2->INTFCR |= DMA2_Channel7_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel8)
    {
        DMA2_EXTEN->INTFCR |= DMA2_Channel8_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel9)
    {
        DMA2_EXTEN->INTFCR |= DMA2_Channel9_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel10)
    {
        DMA2_EXTEN->INTFCR |= DMA2_Channel10_IT_Mask;
    }
    else if(DMAy_Channelx == DMA2_Channel11)
    {
        DMA2_EXTEN->INTFCR |= DMA2_Channel11_IT_Mask;
    }
}

/*********************************************************************
 * @fn      DMA_Init
 *
 * @brief   根据DMA_InitStruct中指定的参数初始化DMAy通道x。
 *
 * @param   DMAy_Channelx - 选择DMA通道，y可为1或2，x范围：DMA1为1~7，DMA2为1~11。
 * @param   DMA_InitStruct - 指向DMA_InitTypeDef结构体的指针，包含指定通道的配置信息。
 *
 * @return  无
 */
void DMA_Init(DMA_Channel_TypeDef *DMAy_Channelx, DMA_InitTypeDef *DMA_InitStruct)
{
    uint32_t tmpreg = 0;

    /* 读取当前配置寄存器值，清除待配置的位，然后设置新值 */
    tmpreg = DMAy_Channelx->CFGR;
    tmpreg &= CFGR_CLEAR_Mask;
    tmpreg |= DMA_InitStruct->DMA_DIR | DMA_InitStruct->DMA_Mode |
              DMA_InitStruct->DMA_PeripheralInc | DMA_InitStruct->DMA_MemoryInc |
              DMA_InitStruct->DMA_PeripheralDataSize | DMA_InitStruct->DMA_MemoryDataSize |
              DMA_InitStruct->DMA_Priority | DMA_InitStruct->DMA_M2M;

    DMAy_Channelx->CFGR = tmpreg;          /* 写入配置寄存器 */
    DMAy_Channelx->CNTR = DMA_InitStruct->DMA_BufferSize;   /* 设置传输数据量 */
    DMAy_Channelx->PADDR = DMA_InitStruct->DMA_PeripheralBaseAddr; /* 设置外设基地址 */
    DMAy_Channelx->MADDR = DMA_InitStruct->DMA_MemoryBaseAddr;     /* 设置存储器基地址 */
}

/*********************************************************************
 * @fn      DMA_StructInit
 *
 * @brief   将DMA_InitStruct结构体每个成员初始化为默认值。
 *
 * @param   DMA_InitStruct - 指向DMA_InitTypeDef结构体的指针，将被初始化。
 *
 * @return  无
 */
void DMA_StructInit(DMA_InitTypeDef *DMA_InitStruct)
{
    DMA_InitStruct->DMA_PeripheralBaseAddr = 0;          /* 外设地址默认0 */
    DMA_InitStruct->DMA_MemoryBaseAddr = 0;              /* 存储器地址默认0 */
    DMA_InitStruct->DMA_DIR = DMA_DIR_PeripheralSRC;     /* 默认方向：外设作为源（外设->存储器） */
    DMA_InitStruct->DMA_BufferSize = 0;                  /* 缓冲区大小默认0 */
    DMA_InitStruct->DMA_PeripheralInc = DMA_PeripheralInc_Disable; /* 外设地址增量禁止 */
    DMA_InitStruct->DMA_MemoryInc = DMA_MemoryInc_Disable;         /* 存储器地址增量禁止 */
    DMA_InitStruct->DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; /* 外设数据宽度：字节 */
    DMA_InitStruct->DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;       /* 存储器数据宽度：字节 */
    DMA_InitStruct->DMA_Mode = DMA_Mode_Normal;           /* 模式：正常模式（非循环） */
    DMA_InitStruct->DMA_Priority = DMA_Priority_Low;      /* 优先级：低 */
    DMA_InitStruct->DMA_M2M = DMA_M2M_Disable;            /* 存储器到存储器模式禁止 */
}

/*********************************************************************
 * @fn      DMA_Cmd
 *
 * @brief   使能或禁用指定的DMAy通道x。
 *
 * @param   DMAy_Channelx - 选择DMA通道。
 * @param   NewState - 新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void DMA_Cmd(DMA_Channel_TypeDef *DMAy_Channelx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMAy_Channelx->CFGR |= DMA_CFGR1_EN;   /* 置位使能位 */
    }
    else
    {
        DMAy_Channelx->CFGR &= (uint16_t)(~DMA_CFGR1_EN); /* 清零使能位 */
    }
}

/*********************************************************************
 * @fn      DMA_ITConfig
 *
 * @brief   使能或禁用指定的DMAy通道x中断。
 *
 * @param   DMAy_Channelx - 选择DMA通道。
 * @param   DMA_IT - 要配置的中断源，可以是DMA_IT_TC、DMA_IT_HT、DMA_IT_TE的组合。
 * @param   NewState - 新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void DMA_ITConfig(DMA_Channel_TypeDef *DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMAy_Channelx->CFGR |= DMA_IT;   /* 使能中断 */
    }
    else
    {
        DMAy_Channelx->CFGR &= ~DMA_IT;  /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      DMA_SetCurrDataCounter
 *
 * @brief   设置当前DMAy通道x传输的数据单元个数。
 *
 * @param   DMAy_Channelx - 选择DMA通道。
 * @param   DataNumber - 当前传输的数据单元个数（取值范围0~65535）。
 *
 * @return  无
 */
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx, uint16_t DataNumber)
{
    DMAy_Channelx->CNTR = DataNumber;  /* 写入计数器寄存器 */
}

/*********************************************************************
 * @fn      DMA_GetCurrDataCounter
 *
 * @brief   返回当前DMAy通道x传输中剩余的数据单元个数。
 *
 * @param   DMAy_Channelx - 选择DMA通道。
 *
 * @return  剩余数据单元个数。
 */
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef *DMAy_Channelx)
{
    return ((uint16_t)(DMAy_Channelx->CNTR));  /* 读取计数器寄存器 */
}

/*********************************************************************
 * @fn      DMA_GetFlagStatus
 *
 * @brief   检查指定的DMAy标志是否置位。
 *
 * @param   DMAy_FLAG - 要检查的标志，如DMA1_FLAG_GL1等（具体见头文件定义）。
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG)
{
    FlagStatus bitstatus = RESET;
    uint32_t   tmpreg = 0;

    /* 根据标志的最高位判断属于哪个DMA的哪个部分 */
    if((DMAy_FLAG & FLAG_Mask) == FLAG_Mask)          /* 属于DMA2基本部分 */
    {
        tmpreg = DMA2->INTFR;
    }
    else if((DMAy_FLAG & DMA2_EXTEN_FLAG_Mask) == DMA2_EXTEN_FLAG_Mask) /* 属于DMA2扩展部分 */
    {
        tmpreg = DMA2_EXTEN->INTFR;
    }
    else                                              /* 属于DMA1 */
    {
        tmpreg = DMA1->INTFR;
    }

    if((tmpreg & DMAy_FLAG) != (uint32_t)RESET)       /* 检查对应位是否置1 */
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
 * @fn      DMA_ClearFlag
 *
 * @brief   清除指定的DMAy挂起标志。
 *
 * @param   DMAy_FLAG - 要清除的标志。
 *
 * @return  无
 */
void DMA_ClearFlag(uint32_t DMAy_FLAG)
{
    /* 根据标志的最高位选择对应的中断标志清除寄存器，写入标志值（写1清除） */
    if((DMAy_FLAG & FLAG_Mask) == FLAG_Mask)
    {
        DMA2->INTFCR = DMAy_FLAG;
    }
    else if((DMAy_FLAG & DMA2_EXTEN_FLAG_Mask) == DMA2_EXTEN_FLAG_Mask)
    {
        DMA2_EXTEN->INTFCR = DMAy_FLAG;
    }
    else
    {
        DMA1->INTFCR = DMAy_FLAG;
    }
}

/*********************************************************************
 * @fn      DMA_GetITStatus
 *
 * @brief   检查指定的DMAy中断是否发生。
 *
 * @param   DMAy_IT - 要检查的中断源，如DMA1_IT_GL1等。
 *
 * @return  ITStatus - SET 或 RESET。
 */
ITStatus DMA_GetITStatus(uint32_t DMAy_IT)
{
    ITStatus bitstatus = RESET;
    uint32_t tmpreg = 0;

    /* 与获取标志状态相同，判断所属DMA部分 */
    if((DMAy_IT & FLAG_Mask) == FLAG_Mask)
    {
        tmpreg = DMA2->INTFR;
    }
    else if((DMAy_IT & DMA2_EXTEN_FLAG_Mask) == DMA2_EXTEN_FLAG_Mask)
    {
        tmpreg = DMA2_EXTEN->INTFR;
    }
    else
    {
        tmpreg = DMA1->INTFR;
    }

    if((tmpreg & DMAy_IT) != (uint32_t)RESET)
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
 * @fn      DMA_ClearITPendingBit
 *
 * @brief   清除指定的DMAy中断挂起位。
 *
 * @param   DMAy_IT - 要清除的中断源。
 *
 * @return  无
 */
void DMA_ClearITPendingBit(uint32_t DMAy_IT)
{
    /* 与清除标志相同，写入对应中断标志清除寄存器 */
    if((DMAy_IT & FLAG_Mask) == FLAG_Mask)
    {
        DMA2->INTFCR = DMAy_IT;
    }
    else if((DMAy_IT & DMA2_EXTEN_FLAG_Mask) == DMA2_EXTEN_FLAG_Mask)
    {
        DMA2_EXTEN->INTFCR = DMAy_IT;
    }
    else
    {
        DMA1->INTFCR = DMAy_IT;
    }
}