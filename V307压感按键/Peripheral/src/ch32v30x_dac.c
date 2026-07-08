/********************************** (C) COPYRIGHT  *******************************
* 文件名           : ch32v30x_dac.c
* 作者             : WCH
* 版本             : V1.0.0
* 日期             : 2021/06/06
* 描述             : 该文件提供了所有DAC固件函数。
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及二进制文件用于南京沁恒微电子有限公司生产的微控制器。
*******************************************************************************/
#include "ch32v30x_dac.h"
#include "ch32v30x_rcc.h"

/* CTLR 寄存器掩码 */
#define CTLR_CLEAR_MASK    ((uint32_t)0x00000FFE)  /* 用于清除DAC控制寄存器的有效位 */

/* DAC 双通道软件触发掩码 */
#define DUAL_SWTR_SET      ((uint32_t)0x00000003)  /* 设置双通道软件触发位 */
#define DUAL_SWTR_RESET    ((uint32_t)0xFFFFFFFC)  /* 清除双通道软件触发位 */

/* DHR 寄存器偏移量（相对于DAC基地址） */
#define DHR12R1_OFFSET     ((uint32_t)0x00000008)  /* DAC通道1 12位右对齐数据保持寄存器偏移 */
#define DHR12R2_OFFSET     ((uint32_t)0x00000014)  /* DAC通道2 12位右对齐数据保持寄存器偏移 */
#define DHR12RD_OFFSET     ((uint32_t)0x00000020)  /* DAC双通道 12位右对齐数据保持寄存器偏移 */

/* DOR 寄存器偏移量（相对于DAC基地址） */
#define DOR_OFFSET         ((uint32_t)0x0000002C)  /* DAC数据输出寄存器偏移 */

/*********************************************************************
 * @fn      DAC_DeInit
 *
 * @brief   将DAC外设寄存器复位为默认值。
 *
 * @return  无
 */
void DAC_DeInit(void)
{
    /* 使能DAC外设复位 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_DAC, ENABLE);
    /* 禁用DAC外设复位 */
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_DAC, DISABLE);
}

/*********************************************************************
 * @fn      DAC_Init
 *
 * @brief   根据DAC_InitStruct中的指定参数初始化DAC外设。
 *
 * @param   DAC_Channel - 选择的DAC通道。
 *            DAC_Channel_1 - 选择DAC通道1
 *            DAC_Channel_2 - 选择DAC通道2
 *          DAC_InitStruct - 指向DAC_InitTypeDef结构的指针，包含初始化参数。
 *
 * @return  无
 */
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef *DAC_InitStruct)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0;

    /* 读取当前DAC控制寄存器值 */
    tmpreg1 = DAC->CTLR;
    /* 清除对应通道的配置位 */
    tmpreg1 &= ~(CTLR_CLEAR_MASK << DAC_Channel);
    /* 合并所有配置参数 */
    tmpreg2 = (DAC_InitStruct->DAC_Trigger | DAC_InitStruct->DAC_WaveGeneration |
               DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude | DAC_InitStruct->DAC_OutputBuffer);
    /* 将配置写入对应通道位 */
    tmpreg1 |= tmpreg2 << DAC_Channel;
    /* 更新DAC控制寄存器 */
    DAC->CTLR = tmpreg1;
}

/*********************************************************************
 * @fn      DAC_StructInit
 *
 * @brief   将DAC_InitStruct的每个成员填充为默认值。
 *
 * @param   DAC_InitStruct - 指向将被初始化的DAC_InitTypeDef结构的指针。
 *
 * @return  无
 */
void DAC_StructInit(DAC_InitTypeDef *DAC_InitStruct)
{
    /* 默认配置：无触发、无波形生成、LFSR无屏蔽位0、使能输出缓冲 */
    DAC_InitStruct->DAC_Trigger = DAC_Trigger_None;
    DAC_InitStruct->DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStruct->DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    DAC_InitStruct->DAC_OutputBuffer = DAC_OutputBuffer_Enable;
}

/*********************************************************************
 * @fn      DAC_Cmd
 *
 * @brief   使能或禁用指定的DAC通道。
 *
 * @param   DAC_Channel - 选择的DAC通道。
 *            DAC_Channel_1 - 选择DAC通道1
 *            DAC_Channel_2 - 选择DAC通道2
 *          NewState - DAC通道的新状态（ENABLE或DISABLE）。
 *
 * @return  无
 */
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 使能指定DAC通道 */
        DAC->CTLR |= (DAC_EN1 << DAC_Channel);
    }
    else
    {
        /* 禁用指定DAC通道 */
        DAC->CTLR &= ~(DAC_EN1 << DAC_Channel);
    }
}

/*********************************************************************
 * @fn      DAC_DMACmd
 *
 * @brief   使能或禁用指定DAC通道的DMA请求。
 *
 * @param   DAC_Channel - 选择的DAC通道。
 *            DAC_Channel_1 - 选择DAC通道1
 *            DAC_Channel_2 - 选择DAC通道2
 *          NewState - DAC通道的新状态（ENABLE或DISABLE）。
 *
 * @return  无
 */
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 使能指定DAC通道的DMA */
        DAC->CTLR |= (DAC_DMAEN1 << DAC_Channel);
    }
    else
    {
        /* 禁用指定DAC通道的DMA */
        DAC->CTLR &= ~(DAC_DMAEN1 << DAC_Channel);
    }
}

/*********************************************************************
 * @fn      DAC_SoftwareTriggerCmd
 *
 * @brief   使能或禁用所选DAC通道的软件触发。
 *
 * @param   DAC_Channel - 选择的DAC通道。
 *            DAC_Channel_1 - 选择DAC通道1
 *            DAC_Channel_2 - 选择DAC通道2
 *          NewState - DAC通道的新状态（ENABLE或DISABLE）。
 *
 * @return  无
 */
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 设置指定通道的软件触发位 */
        DAC->SWTR |= (uint32_t)DAC_SWTRIG1 << (DAC_Channel >> 4);
    }
    else
    {
        /* 清除指定通道的软件触发位 */
        DAC->SWTR &= ~((uint32_t)DAC_SWTRIG1 << (DAC_Channel >> 4));
    }
}

/*********************************************************************
 * @fn      DAC_DualSoftwareTriggerCmd
 *
 * @brief   使能或禁用两个DAC通道的软件触发。
 *
 * @param   NewState - DAC通道的新状态（ENABLE或DISABLE）。
 *
 * @return  无
 */
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 设置双通道软件触发位 */
        DAC->SWTR |= DUAL_SWTR_SET;
    }
    else
    {
        /* 清除双通道软件触发位 */
        DAC->SWTR &= DUAL_SWTR_RESET;
    }
}

/*********************************************************************
 * @fn      DAC_WaveGenerationCmd
 *
 * @brief   使能或禁用所选DAC通道的波形生成。
 *
 * @param   DAC_Channel - 选择的DAC通道。
 *            DAC_Channel_1 - 选择DAC通道1
 *            DAC_Channel_2 - 选择DAC通道2
 *          DAC_Wave - 指定要启用或禁用的波形类型。
 *            DAC_Wave_Noise - 噪声波生成
 *            DAC_Wave_Triangle - 三角波生成
 *          NewState - DAC通道的新状态（ENABLE或DISABLE）。
 *
 * @return  无
 */
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 使能指定波形生成 */
        DAC->CTLR |= DAC_Wave << DAC_Channel;
    }
    else
    {
        /* 禁用指定波形生成 */
        DAC->CTLR &= ~(DAC_Wave << DAC_Channel);
    }
}

/*********************************************************************
 * @fn      DAC_SetChannel1Data
 *
 * @brief   设置DAC通道1的指定数据保持寄存器值。
 *
 * @param   DAC_Align - 指定DAC通道1的数据对齐方式。
 *            DAC_Align_8b_R - 选择8位右对齐
 *            DAC_Align_12b_L - 选择12位左对齐
 *            DAC_Align_12b_R - 选择12位右对齐
 *          Data - 要加载到所选数据保持寄存器的数据。
 *
 * @return  无
 */
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data)
{
    __IO uint32_t tmp = 0;

    /* 计算目标寄存器的地址：DAC基地址 + DHR12R1偏移 + 对齐偏移 */
    tmp = (uint32_t)DAC_BASE;
    tmp += DHR12R1_OFFSET + DAC_Align;

    /* 写入数据到目标寄存器 */
    *(__IO uint32_t *)tmp = Data;
}

/*********************************************************************
 * @fn      DAC_SetChannel2Data
 *
 * @brief   设置DAC通道2的指定数据保持寄存器值。
 *
 * @param   DAC_Align - 指定DAC通道1的数据对齐方式。
 *            DAC_Align_8b_R - 选择8位右对齐
 *            DAC_Align_12b_L - 选择12位左对齐
 *            DAC_Align_12b_R - 选择12位右对齐
 *          Data - 要加载到所选数据保持寄存器的数据。
 *
 * @return  无
 */
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data)
{
    __IO uint32_t tmp = 0;

    /* 计算目标寄存器的地址：DAC基地址 + DHR12R2偏移 + 对齐偏移 */
    tmp = (uint32_t)DAC_BASE;
    tmp += DHR12R2_OFFSET + DAC_Align;

    /* 写入数据到目标寄存器 */
    *(__IO uint32_t *)tmp = Data;
}

/*********************************************************************
 * @fn      DAC_SetDualChannelData
 *
 * @brief   设置两个DAC通道的数据保持寄存器值。
 *
 * @param   DAC_Align - 指定DAC通道1的数据对齐方式。
 *            DAC_Align_8b_R - 选择8位右对齐
 *            DAC_Align_12b_L - 选择12位左对齐
 *            DAC_Align_12b_R - 选择12位右对齐
 *          Data2 - DAC通道2的数据。
 *          Data1 - DAC通道1的数据。
 *
 * @return  无
 */
void DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1)
{
    uint32_t data = 0, tmp = 0;

    /* 根据对齐方式合并两个通道的数据 */
    if(DAC_Align == DAC_Align_8b_R)
    {
        /* 8位右对齐：Data2在高8位，Data1在低8位 */
        data = ((uint32_t)Data2 << 8) | Data1;
    }
    else
    {
        /* 12位对齐：Data2在高12位，Data1在低12位 */
        data = ((uint32_t)Data2 << 16) | Data1;
    }

    /* 计算目标寄存器的地址：DAC基地址 + DHR12RD偏移 + 对齐偏移 */
    tmp = (uint32_t)DAC_BASE;
    tmp += DHR12RD_OFFSET + DAC_Align;

    /* 写入数据到目标寄存器 */
    *(__IO uint32_t *)tmp = data;
}

/*********************************************************************
 * @fn      DAC_GetDataOutputValue
 *
 * @brief   返回所选DAC通道的最后数据输出值。
 *
 * @param   DAC_Channel - 选择的DAC通道。
 *            DAC_Channel_1 - 选择DAC通道1
 *            DAC_Channel_2 - 选择DAC通道2
 *
 * @return  DAC数据输出寄存器的值。
 */
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel)
{
    __IO uint32_t tmp = 0;

    /* 计算目标寄存器的地址：DAC基地址 + DOR偏移 + 通道偏移（每通道4字节） */
    tmp = (uint32_t)DAC_BASE;
    tmp += DOR_OFFSET + ((uint32_t)DAC_Channel >> 2);

    /* 读取并返回数据输出寄存器的值 */
    return (uint16_t)(*(__IO uint32_t *)tmp);
}