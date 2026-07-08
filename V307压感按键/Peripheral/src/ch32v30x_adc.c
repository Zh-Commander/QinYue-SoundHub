/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_adc.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 该文件提供了所有 ADC 固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）和二进制文件可用于南京沁恒微电子股份有限公司生产的微控制器。
*******************************************************************************/

#include "ch32v30x_adc.h"
#include "ch32v30x_rcc.h"

/* 以下宏定义用于设置/清除 ADC 控制寄存器中的各种位 */

/* ADC DISCNUM (规则组不连续模式通道数) 掩码，用于清除 CTLR1 中相关位 */
#define CTLR1_DISCNUM_Reset              ((uint32_t)0xFFFF1FFF)

/* ADC DISCEN (规则组不连续模式使能) 置位和复位掩码 */
#define CTLR1_DISCEN_Set                 ((uint32_t)0x00000800)
#define CTLR1_DISCEN_Reset               ((uint32_t)0xFFFFF7FF)

/* ADC JAUTO (自动注入组转换) 置位和复位掩码 */
#define CTLR1_JAUTO_Set                  ((uint32_t)0x00000400)
#define CTLR1_JAUTO_Reset                ((uint32_t)0xFFFFFBFF)

/* ADC JDISCEN (注入组不连续模式使能) 置位和复位掩码 */
#define CTLR1_JDISCEN_Set                ((uint32_t)0x00001000)
#define CTLR1_JDISCEN_Reset              ((uint32_t)0xFFFFEFFF)

/* ADC AWDCH (模拟看门狗通道选择) 清除掩码，用于清除通道选择位 */
#define CTLR1_AWDCH_Reset                ((uint32_t)0xFFFFFFE0)

/* ADC 模拟看门狗模式清除掩码，用于清除模式选择位 */
#define CTLR1_AWDMode_Reset              ((uint32_t)0xFF3FFDFF)

/* CTLR1 寄存器清除掩码，用于在配置前保留某些位 */
#define CTLR1_CLEAR_Mask                 ((uint32_t)0xE0F0FEFF)

/* ADC ADON (ADC 使能) 置位和复位掩码 */
#define CTLR2_ADON_Set                   ((uint32_t)0x00000001)
#define CTLR2_ADON_Reset                 ((uint32_t)0xFFFFFFFE)

/* ADC DMA (DMA 请求使能) 置位和复位掩码 */
#define CTLR2_DMA_Set                    ((uint32_t)0x00000100)
#define CTLR2_DMA_Reset                  ((uint32_t)0xFFFFFEFF)

/* ADC RSTCAL (复位校准) 置位掩码 */
#define CTLR2_RSTCAL_Set                 ((uint32_t)0x00000008)

/* ADC CAL (校准) 置位掩码 */
#define CTLR2_CAL_Set                    ((uint32_t)0x00000004)

/* ADC SWSTART (规则组软件启动) 置位掩码 */
#define CTLR2_SWSTART_Set                ((uint32_t)0x00400000)

/* ADC EXTTRIG (规则组外部触发使能) 置位和复位掩码 */
#define CTLR2_EXTTRIG_Set                ((uint32_t)0x00100000)
#define CTLR2_EXTTRIG_Reset              ((uint32_t)0xFFEFFFFF)

/* ADC 软件启动组合掩码 (EXTTRIG + SWSTART) 用于软件触发模式 */
#define CTLR2_EXTTRIG_SWSTART_Set        ((uint32_t)0x00500000)
#define CTLR2_EXTTRIG_SWSTART_Reset      ((uint32_t)0xFFAFFFFF)

/* ADC JEXTSEL (注入组外部触发选择) 清除掩码 */
#define CTLR2_JEXTSEL_Reset              ((uint32_t)0xFFFF8FFF)

/* ADC JEXTTRIG (注入组外部触发使能) 置位和复位掩码 */
#define CTLR2_JEXTTRIG_Set               ((uint32_t)0x00008000)
#define CTLR2_JEXTTRIG_Reset             ((uint32_t)0xFFFF7FFF)

/* ADC JSWSTART (注入组软件启动) 置位掩码 */
#define CTLR2_JSWSTART_Set               ((uint32_t)0x00200000)

/* ADC 注入组软件启动组合掩码 (JEXTTRIG + JSWSTART) 用于软件触发模式 */
#define CTLR2_JEXTTRIG_JSWSTART_Set      ((uint32_t)0x00208000)
#define CTLR2_JEXTTRIG_JSWSTART_Reset    ((uint32_t)0xFFDF7FFF)

/* ADC TSVREFE (温度传感器和 Vref 使能) 置位和复位掩码 */
#define CTLR2_TSVREFE_Set                ((uint32_t)0x00800000)
#define CTLR2_TSVREFE_Reset              ((uint32_t)0xFF7FFFFF)

/* CTLR2 寄存器清除掩码，用于在配置前保留某些位 */
#define CTLR2_CLEAR_Mask                 ((uint32_t)0xFFF1F7FD)

/* ADC 规则序列寄存器 SQx 字段掩码 (每个 SQ 占5位) */
#define RSQR3_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR2_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR1_SQ_Set                     ((uint32_t)0x0000001F)

/* RSQR1 寄存器清除掩码，用于清除序列长度等位 */
#define RSQR1_CLEAR_Mask                 ((uint32_t)0xFF0FFFFF)

/* ADC 注入序列寄存器 JSQx 字段掩码 */
#define ISQR_JSQ_Set                     ((uint32_t)0x0000001F)

/* ADC JL (注入组序列长度) 字段置位和复位掩码 */
#define ISQR_JL_Set                      ((uint32_t)0x00300000)
#define ISQR_JL_Reset                    ((uint32_t)0xFFCFFFFF)

/* ADC 采样时间寄存器 SMPx 字段掩码 (每个通道采样时间占3位) */
#define SAMPTR1_SMP_Set                  ((uint32_t)0x00000007)
#define SAMPTR2_SMP_Set                  ((uint32_t)0x00000007)

/* ADC 注入数据寄存器偏移量 (相对于 ADC 基址) */
#define IDATAR_Offset                    ((uint8_t)0x28)

/* ADC1 规则数据寄存器地址 */
#define RDATAR_ADDRESS                   ((uint32_t)0x4001244C)

/*********************************************************************
 * @fn      ADC_DeInit
 *
 * @brief   将 ADCx 外设寄存器复位到默认值。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  无
 */
void ADC_DeInit(ADC_TypeDef *ADCx)
{
    if(ADCx == ADC1)
    {
        /* 复位 ADC1 外设 */
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1, DISABLE);
    }
    else if(ADCx == ADC2)
    {
        /* 复位 ADC2 外设 */
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2, DISABLE);
    }
}

/*********************************************************************
 * @fn      ADC_Init
 *
 * @brief   根据 ADC_InitStruct 中的参数初始化 ADCx 外设。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要初始化的 ADC 外设。
 *          ADC_InitStruct - 指向 ADC_InitTypeDef 结构体的指针，
 *                            包含指定 ADC 外设的配置信息。
 *
 * @return  无
 */
void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct)
{
    uint32_t tmpreg1 = 0;
    uint8_t  tmpreg2 = 0;

    /* 配置 CTLR1 寄存器 */
    tmpreg1 = ADCx->CTLR1;
    tmpreg1 &= CTLR1_CLEAR_Mask;  /* 清除需要配置的位域 */
    /* 设置 ADC 模式、输出缓冲、PGA、扫描模式 */
    tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_Mode | (uint32_t)ADC_InitStruct->ADC_OutputBuffer |
                          (uint32_t)ADC_InitStruct->ADC_Pga | ((uint32_t)ADC_InitStruct->ADC_ScanConvMode << 8));
    ADCx->CTLR1 = tmpreg1;

    /* 配置 CTLR2 寄存器 */
    tmpreg1 = ADCx->CTLR2;
    tmpreg1 &= CTLR2_CLEAR_Mask;  /* 清除需要配置的位域 */
    /* 设置数据对齐、外部触发、连续转换模式 */
    tmpreg1 |= (uint32_t)(ADC_InitStruct->ADC_DataAlign | ADC_InitStruct->ADC_ExternalTrigConv |
                          ((uint32_t)ADC_InitStruct->ADC_ContinuousConvMode << 1));
    ADCx->CTLR2 = tmpreg1;

    /* 配置规则序列寄存器 RSQR1（设置序列长度） */
    tmpreg1 = ADCx->RSQR1;
    tmpreg1 &= RSQR1_CLEAR_Mask;  /* 清除序列长度位 */
    tmpreg2 |= (uint8_t)(ADC_InitStruct->ADC_NbrOfChannel - (uint8_t)1); /* 序列长度 = 通道数-1 */
    tmpreg1 |= (uint32_t)tmpreg2 << 20;  /* 将长度写入 RSQR1 的 L[3:0] 位 */
    ADCx->RSQR1 = tmpreg1;
}

/*********************************************************************
 * @fn      ADC_StructInit
 *
 * @brief   将 ADC_InitStruct 中的每个成员初始化为默认值。
 *
 * @param   ADC_InitStruct - 指向 ADC_InitTypeDef 结构体的指针，
 *                            包含指定 ADC 外设的配置信息。
 *
 * @return  无
 */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
    /* 默认值：独立模式、单次转换、软件触发、右对齐、转换1个通道 */
    ADC_InitStruct->ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct->ADC_ScanConvMode = DISABLE;
    ADC_InitStruct->ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct->ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStruct->ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct->ADC_NbrOfChannel = 1;
}

/*********************************************************************
 * @fn      ADC_Cmd
 *
 * @brief   使能或禁用指定的 ADC 外设。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_Cmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_ADON_Set;   /* 设置 ADON 位，使能 ADC */
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_ADON_Reset; /* 清除 ADON 位，禁用 ADC */
    }
}

/*********************************************************************
 * @fn      ADC_DMACmd
 *
 * @brief   使能或禁用指定 ADC 的 DMA 请求。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_DMACmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_DMA_Set;    /* 使能 DMA 请求 */
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_DMA_Reset;  /* 禁用 DMA 请求 */
    }
}

/*********************************************************************
 * @fn      ADC_ITConfig
 *
 * @brief   使能或禁用指定的 ADC 中断。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_IT - 指定要配置的中断源，可以是以下值的组合：
 *            ADC_IT_EOC  - 转换结束中断
 *            ADC_IT_AWD  - 模拟看门狗中断
 *            ADC_IT_JEOC - 注入组转换结束中断
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_ITConfig(ADC_TypeDef *ADCx, uint16_t ADC_IT, FunctionalState NewState)
{
    uint8_t itmask = 0;

    itmask = (uint8_t)ADC_IT;  /* 取低8位作为中断使能位 */

    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= itmask;          /* 使能中断 */
    }
    else
    {
        ADCx->CTLR1 &= (~(uint32_t)itmask); /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      ADC_ResetCalibration
 *
 * @brief   复位选定的 ADC 校准寄存器。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  无
 */
void ADC_ResetCalibration(ADC_TypeDef *ADCx)
{
    ADCx->CTLR2 |= CTLR2_RSTCAL_Set;   /* 设置 RSTCAL 位，开始复位校准 */
}

/*********************************************************************
 * @fn      ADC_GetResetCalibrationStatus
 *
 * @brief   获取选定的 ADC 复位校准寄存器的状态。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus ADC_GetResetCalibrationStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_RSTCAL_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;   /* RSTCAL 仍为 1，复位校准进行中 */
    }
    else
    {
        bitstatus = RESET; /* RSTCAL 已清零，复位校准完成 */
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_StartCalibration
 *
 * @brief   启动选定的 ADC 校准过程。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  无
 */
void ADC_StartCalibration(ADC_TypeDef *ADCx)
{
    ADCx->CTLR2 |= CTLR2_CAL_Set;   /* 设置 CAL 位，开始校准 */
}

/*********************************************************************
 * @fn      ADC_GetCalibrationStatus
 *
 * @brief   获取选定的 ADC 校准状态。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus ADC_GetCalibrationStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_CAL_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;   /* CAL 为 1，校准进行中 */
    }
    else
    {
        bitstatus = RESET; /* CAL 已清零，校准完成 */
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_SoftwareStartConvCmd
 *
 * @brief   使能或禁用选定的 ADC 软件启动转换（用于规则组）。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_SoftwareStartConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 设置 EXTTRIG 和 SWSTART，允许软件触发规则组转换 */
        ADCx->CTLR2 |= CTLR2_EXTTRIG_SWSTART_Set;
    }
    else
    {
        /* 清除 EXTTRIG 和 SWSTART，禁用软件触发 */
        ADCx->CTLR2 &= CTLR2_EXTTRIG_SWSTART_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_GetSoftwareStartConvStatus
 *
 * @brief   获取选定的 ADC 软件启动转换状态（规则组）。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus ADC_GetSoftwareStartConvStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_SWSTART_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;   /* SWSTART 为 1，转换已启动 */
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_DiscModeChannelCountConfig
 *
 * @brief   配置选定 ADC 规则组的不连续模式通道数。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          Number - 指定不连续模式的规则通道计数值 (1-8)。
 *
 * @return  无
 */
void ADC_DiscModeChannelCountConfig(ADC_TypeDef *ADCx, uint8_t Number)
{
    uint32_t tmpreg1 = 0;
    uint32_t tmpreg2 = 0;

    tmpreg1 = ADCx->CTLR1;
    tmpreg1 &= CTLR1_DISCNUM_Reset;          /* 清除 DISCNUM[2:0] 位 */
    tmpreg2 = Number - 1;                    /* 硬件中 DISCNUM = Number-1 */
    tmpreg1 |= tmpreg2 << 13;                 /* 写入 DISCNUM 位 */
    ADCx->CTLR1 = tmpreg1;
}

/*********************************************************************
 * @fn      ADC_DiscModeCmd
 *
 * @brief   使能或禁用指定 ADC 规则组的不连续模式。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_DiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= CTLR1_DISCEN_Set;     /* 使能规则组不连续模式 */
    }
    else
    {
        ADCx->CTLR1 &= CTLR1_DISCEN_Reset;   /* 禁用规则组不连续模式 */
    }
}

/*********************************************************************
 * @fn      ADC_RegularChannelConfig
 *
 * @brief   配置选定 ADC 的规则组通道：设置其在序列中的排名和采样时间。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_Channel - 要配置的 ADC 通道，取值范围 ADC_Channel_0 ~ ADC_Channel_17。
 *          Rank - 规则组序列中的排名，取值范围 1 到 16。
 *          ADC_SampleTime - 通道的采样时间，如 ADC_SampleTime_1Cycles5 等。
 *
 * @return  无
 */
void ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0;

    /* 设置采样时间寄存器 SAMPTR1 (通道10-17) 或 SAMPTR2 (通道0-9) */
    if(ADC_Channel > ADC_Channel_9)
    {
        tmpreg1 = ADCx->SAMPTR1;
        tmpreg2 = SAMPTR1_SMP_Set << (3 * (ADC_Channel - 10));  /* 每个通道占3位，计算偏移 */
        tmpreg1 &= ~tmpreg2;                                     /* 清除该通道的采样时间位 */
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));
        tmpreg1 |= tmpreg2;                                      /* 设置新的采样时间 */
        ADCx->SAMPTR1 = tmpreg1;
    }
    else /* 通道0-9 */
    {
        tmpreg1 = ADCx->SAMPTR2;
        tmpreg2 = SAMPTR2_SMP_Set << (3 * ADC_Channel);
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
        tmpreg1 |= tmpreg2;
        ADCx->SAMPTR2 = tmpreg1;
    }

    /* 设置规则序列寄存器 RSQR1/2/3，根据排名将通道号写入相应位置 */
    if(Rank < 7)
    {
        tmpreg1 = ADCx->RSQR3;
        tmpreg2 = RSQR3_SQ_Set << (5 * (Rank - 1));   /* 每个 SQ 占5位 */
        tmpreg1 &= ~tmpreg2;                            /* 清除该排名对应的 SQ 位 */
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));
        tmpreg1 |= tmpreg2;                             /* 写入通道号 */
        ADCx->RSQR3 = tmpreg1;
    }
    else if(Rank < 13)
    {
        tmpreg1 = ADCx->RSQR2;
        tmpreg2 = RSQR2_SQ_Set << (5 * (Rank - 7));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR2 = tmpreg1;
    }
    else
    {
        tmpreg1 = ADCx->RSQR1;
        tmpreg2 = RSQR1_SQ_Set << (5 * (Rank - 13));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR1 = tmpreg1;
    }
}

/*********************************************************************
 * @fn      ADC_ExternalTrigConvCmd
 *
 * @brief   使能或禁用 ADCx 通过外部触发转换（规则组）。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_ExternalTrigConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_EXTTRIG_Set;     /* 使能规则组外部触发 */
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_EXTTRIG_Reset;   /* 禁用规则组外部触发 */
    }
}

/*********************************************************************
 * @fn      ADC_GetConversionValue
 *
 * @brief   返回指定 ADC 规则组的最新转换结果。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  转换结果数据。
 */
uint16_t ADC_GetConversionValue(ADC_TypeDef *ADCx)
{
    return (uint16_t)ADCx->RDATAR;   /* 读取规则数据寄存器 */
}

/*********************************************************************
 * @fn      ADC_GetDualModeConversionValue
 *
 * @brief   返回双 ADC 模式下 ADC1 和 ADC2 的组合转换结果。
 *
 * @return  32 位数据，高16位为 ADC2 结果，低16位为 ADC1 结果。
 */
uint32_t ADC_GetDualModeConversionValue(void)
{
    return (*(__IO uint32_t *)RDATAR_ADDRESS);   /* 读取双 ADC 数据寄存器 */
}

/*********************************************************************
 * @fn      ADC_AutoInjectedConvCmd
 *
 * @brief   使能或禁用选定 ADC 在规则组转换后自动启动注入组转换。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_AutoInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= CTLR1_JAUTO_Set;        /* 使能自动注入 */
    }
    else
    {
        ADCx->CTLR1 &= CTLR1_JAUTO_Reset;      /* 禁用自动注入 */
    }
}

/*********************************************************************
 * @fn      ADC_InjectedDiscModeCmd
 *
 * @brief   使能或禁用指定 ADC 注入组的不连续模式。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_InjectedDiscModeCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= CTLR1_JDISCEN_Set;      /* 使能注入组不连续模式 */
    }
    else
    {
        ADCx->CTLR1 &= CTLR1_JDISCEN_Reset;    /* 禁用注入组不连续模式 */
    }
}

/*********************************************************************
 * @fn      ADC_ExternalTrigInjectedConvConfig
 *
 * @brief   配置 ADCx 注入组转换的外部触发源。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_ExternalTrigInjecConv - 指定注入组的外部触发源，如
 *            ADC_ExternalTrigInjecConv_T1_TRGO 等。
 *
 * @return  无
 */
void ADC_ExternalTrigInjectedConvConfig(ADC_TypeDef *ADCx, uint32_t ADC_ExternalTrigInjecConv)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CTLR2;
    tmpreg &= CTLR2_JEXTSEL_Reset;               /* 清除 JEXTSEL[2:0] 位 */
    tmpreg |= ADC_ExternalTrigInjecConv;          /* 设置新的触发源 */
    ADCx->CTLR2 = tmpreg;
}

/*********************************************************************
 * @fn      ADC_ExternalTrigInjectedConvCmd
 *
 * @brief   使能或禁用 ADCx 注入组转换的外部触发。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_ExternalTrigInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR2 |= CTLR2_JEXTTRIG_Set;       /* 使能注入组外部触发 */
    }
    else
    {
        ADCx->CTLR2 &= CTLR2_JEXTTRIG_Reset;     /* 禁用注入组外部触发 */
    }
}

/*********************************************************************
 * @fn      ADC_SoftwareStartInjectedConvCmd
 *
 * @brief   使能或禁用选定 ADC 注入组转换的软件启动。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_SoftwareStartInjectedConvCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 设置 JEXTTRIG 和 JSWSTART，允许软件触发注入组转换 */
        ADCx->CTLR2 |= CTLR2_JEXTTRIG_JSWSTART_Set;
    }
    else
    {
        /* 清除 JEXTTRIG 和 JSWSTART，禁用软件触发 */
        ADCx->CTLR2 &= CTLR2_JEXTTRIG_JSWSTART_Reset;
    }
}

/*********************************************************************
 * @fn      ADC_GetSoftwareStartInjectedConvCmdStatus
 *
 * @brief   获取选定 ADC 注入组软件启动转换的状态。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus ADC_GetSoftwareStartInjectedConvCmdStatus(ADC_TypeDef *ADCx)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->CTLR2 & CTLR2_JSWSTART_Set) != (uint32_t)RESET)
    {
        bitstatus = SET;   /* JSWSTART 为 1，注入组转换已启动 */
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_InjectedChannelConfig
 *
 * @brief   配置选定 ADC 的注入组通道：设置其在序列中的排名和采样时间。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_Channel - 要配置的 ADC 通道。
 *          Rank - 注入组序列中的排名，取值范围 1 到 4。
 *          ADC_SampleTime - 通道的采样时间。
 *
 * @return  无
 */
void ADC_InjectedChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0, tmpreg3 = 0;

    /* 设置采样时间（同规则组） */
    if(ADC_Channel > ADC_Channel_9)
    {
        tmpreg1 = ADCx->SAMPTR1;
        tmpreg2 = SAMPTR1_SMP_Set << (3 * (ADC_Channel - 10));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));
        tmpreg1 |= tmpreg2;
        ADCx->SAMPTR1 = tmpreg1;
    }
    else
    {
        tmpreg1 = ADCx->SAMPTR2;
        tmpreg2 = SAMPTR2_SMP_Set << (3 * ADC_Channel);
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
        tmpreg1 |= tmpreg2;
        ADCx->SAMPTR2 = tmpreg1;
    }

    /* 设置注入序列寄存器 ISQR */
    tmpreg1 = ADCx->ISQR;
    tmpreg3 = (tmpreg1 & ISQR_JL_Set) >> 20;        /* 获取当前注入组序列长度 JL */
    /* 计算要写入的 JSQ 位偏移。注入序列寄存器中，JSQ 从最高位开始排列，与 JL 有关。
       公式: 偏移 = 5 * ( (Rank + 3) - (JL + 1) ) */
    tmpreg2 = ISQR_JSQ_Set << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
    tmpreg1 &= ~tmpreg2;                             /* 清除该 JSQ 位 */
    tmpreg2 = (uint32_t)ADC_Channel << (5 * (uint8_t)((Rank + 3) - (tmpreg3 + 1)));
    tmpreg1 |= tmpreg2;                              /* 写入通道号 */
    ADCx->ISQR = tmpreg1;
}

/*********************************************************************
 * @fn      ADC_InjectedSequencerLengthConfig
 *
 * @brief   配置注入组序列的长度。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          Length - 序列长度，取值范围 1 到 4。
 *
 * @return  无
 */
void ADC_InjectedSequencerLengthConfig(ADC_TypeDef *ADCx, uint8_t Length)
{
    uint32_t tmpreg1 = 0;
    uint32_t tmpreg2 = 0;

    tmpreg1 = ADCx->ISQR;
    tmpreg1 &= ISQR_JL_Reset;           /* 清除 JL 位 */
    tmpreg2 = Length - 1;                /* JL = 长度-1 */
    tmpreg1 |= tmpreg2 << 20;            /* 写入 JL 位 */
    ADCx->ISQR = tmpreg1;
}

/*********************************************************************
 * @fn      ADC_SetInjectedOffset
 *
 * @brief   设置注入通道的转换结果偏移值。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_InjectedChannel - 注入通道，可以是 ADC_InjectedChannel_1 到 _4。
 *          Offset - 偏移值（12位）。
 *
 * @return  无
 */
void ADC_SetInjectedOffset(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel, uint16_t Offset)
{
    __IO uint32_t tmp = 0;

    /* 计算注入偏移寄存器地址：基址 + 注入通道索引（1~4） */
    tmp = (uint32_t)ADCx;
    tmp += ADC_InjectedChannel;   /* 注意：通道索引值即对应寄存器的偏移，例如 0x04, 0x08, 0x0C, 0x10? 具体见头文件定义 */

    *(__IO uint32_t *)tmp = (uint32_t)Offset;   /* 写入偏移值 */
}

/*********************************************************************
 * @fn      ADC_GetInjectedConversionValue
 *
 * @brief   返回指定 ADC 注入通道的转换结果。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_InjectedChannel - 注入通道，可以是 ADC_InjectedChannel_1 到 _4。
 *
 * @return  转换结果数据。
 */
uint16_t ADC_GetInjectedConversionValue(ADC_TypeDef *ADCx, uint8_t ADC_InjectedChannel)
{
    __IO uint32_t tmp = 0;

    /* 计算注入数据寄存器地址：基址 + IDATAR_Offset + 通道索引 */
    tmp = (uint32_t)ADCx;
    tmp += ADC_InjectedChannel + IDATAR_Offset;

    return (uint16_t)(*(__IO uint32_t *)tmp);
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdogCmd
 *
 * @brief   使能或禁用模拟看门狗，可选择作用于单个/所有规则或注入通道。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_AnalogWatchdog - 模拟看门狗配置，如
 *            ADC_AnalogWatchdog_SingleRegEnable 等。
 *
 * @return  无
 */
void ADC_AnalogWatchdogCmd(ADC_TypeDef *ADCx, uint32_t ADC_AnalogWatchdog)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CTLR1;
    tmpreg &= CTLR1_AWDMode_Reset;   /* 清除原有的看门狗模式位 */
    tmpreg |= ADC_AnalogWatchdog;     /* 设置新的看门狗模式 */
    ADCx->CTLR1 = tmpreg;
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdogThresholdsConfig
 *
 * @brief   配置模拟看门狗的高阈值和低阈值。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          HighThreshold - 高阈值（12位）。
 *          LowThreshold - 低阈值（12位）。
 *
 * @return  无
 */
void ADC_AnalogWatchdogThresholdsConfig(ADC_TypeDef *ADCx, uint16_t HighThreshold,
                                        uint16_t LowThreshold)
{
    ADCx->WDHTR = HighThreshold;   /* 写高阈值寄存器 */
    ADCx->WDLTR = LowThreshold;    /* 写低阈值寄存器 */
}

/*********************************************************************
 * @fn      ADC_AnalogWatchdogSingleChannelConfig
 *
 * @brief   配置模拟看门狗所监视的单个通道。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_Channel - 要监视的 ADC 通道。
 *
 * @return  无
 */
void ADC_AnalogWatchdogSingleChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel)
{
    uint32_t tmpreg = 0;

    tmpreg = ADCx->CTLR1;
    tmpreg &= CTLR1_AWDCH_Reset;   /* 清除原有的通道选择位 */
    tmpreg |= ADC_Channel;          /* 设置新的通道 */
    ADCx->CTLR1 = tmpreg;
}

/*********************************************************************
 * @fn      ADC_TempSensorVrefintCmd
 *
 * @brief   使能或禁用温度传感器和内部参考电压通道。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_TempSensorVrefintCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADC1->CTLR2 |= CTLR2_TSVREFE_Set;   /* 使能温度传感器和 Vref */
    }
    else
    {
        ADC1->CTLR2 &= CTLR2_TSVREFE_Reset; /* 禁用 */
    }
}

/*********************************************************************
 * @fn      ADC_GetFlagStatus
 *
 * @brief   检查指定的 ADC 标志是否被置位。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_FLAG - 指定要检查的标志，可以是：
 *            ADC_FLAG_AWD   - 模拟看门狗标志
 *            ADC_FLAG_EOC   - 规则组转换结束标志
 *            ADC_FLAG_JEOC  - 注入组转换结束标志
 *            ADC_FLAG_JSTRT - 注入组转换开始标志
 *            ADC_FLAG_STRT  - 规则组转换开始标志
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus ADC_GetFlagStatus(ADC_TypeDef *ADCx, uint8_t ADC_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((ADCx->STATR & ADC_FLAG) != (uint8_t)RESET)
    {
        bitstatus = SET;   /* 标志置位 */
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_ClearFlag
 *
 * @brief   清除 ADCx 的挂起标志。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_FLAG - 指定要清除的标志，同上。
 *
 * @return  无
 */
void ADC_ClearFlag(ADC_TypeDef *ADCx, uint8_t ADC_FLAG)
{
    ADCx->STATR = ~(uint32_t)ADC_FLAG;   /* 写1清除？硬件中写相应位清零，这里取反写入？注意实际硬件可能是写1清除或写0清除，需参考手册 */
}

/*********************************************************************
 * @fn      ADC_GetITStatus
 *
 * @brief   检查指定的 ADC 中断是否发生。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_IT - 指定要检查的中断源，可以是：
 *            ADC_IT_EOC  - 规则组转换结束中断
 *            ADC_IT_AWD  - 模拟看门狗中断
 *            ADC_IT_JEOC - 注入组转换结束中断
 *
 * @return  ITStatus - SET 或 RESET。
 */
ITStatus ADC_GetITStatus(ADC_TypeDef *ADCx, uint16_t ADC_IT)
{
    ITStatus bitstatus = RESET;
    uint32_t itmask = 0, enablestatus = 0;

    itmask = ADC_IT >> 8;                       /* 高8位为状态寄存器中的标志位 */
    enablestatus = (ADCx->CTLR1 & (uint8_t)ADC_IT); /* 低8位为中断使能位 */

    if(((ADCx->STATR & itmask) != (uint32_t)RESET) && enablestatus)
    {
        bitstatus = SET;   /* 中断标志置位且中断使能 */
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ADC_ClearITPendingBit
 *
 * @brief   清除 ADCx 的中断挂起位。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          ADC_IT - 指定要清除的中断源，同上。
 *
 * @return  无
 */
void ADC_ClearITPendingBit(ADC_TypeDef *ADCx, uint16_t ADC_IT)
{
    uint8_t itmask = 0;

    itmask = (uint8_t)(ADC_IT >> 8);   /* 取高8位作为状态标志位 */
    ADCx->STATR = ~(uint32_t)itmask;   /* 清除该标志（写1清零？） */
}

/*********************************************************************
 * @fn      TempSensor_Volt_To_Temper
 *
 * @brief   将内部温度传感器的电压值转换为温度值。
 *
 * @param   Value - 电压值（单位 mV）。
 *
 * @return  温度值（单位：摄氏度，可能带有小数部分乘以10？）。
 */
s32 TempSensor_Volt_To_Temper(s32 Value)
{
    s32 Temper, Refer_Volt, Refer_Temper;
    s32 k = 43;   /* 温度系数，由芯片特性决定 */

    /* 从芯片闪存中读取出厂校准值：低16位为参考电压，高16位为参考温度 */
    Refer_Volt = (s32)((*(u32 *)0x1FFFF720) & 0x0000FFFF);
    Refer_Temper = (s32)(((*(u32 *)0x1FFFF720) >> 16) & 0x0000FFFF);

    /* 根据校准值计算温度：T = Tref - ((V - Vref)*10 + k/2) / k */
    Temper = Refer_Temper - ((Value - Refer_Volt) * 10 + (k >> 1)) / k;

    return Temper;
}

/*********************************************************************
 * @fn      ADC_BufferCmd
 *
 * @brief   使能或禁用 ADCx 的输出缓冲器。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ADC_BufferCmd(ADC_TypeDef *ADCx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ADCx->CTLR1 |= (1 << 26);   /* 设置 BUFEN 位（bit26），使能输出缓冲 */
    }
    else
    {
        ADCx->CTLR1 &= ~(1 << 26);  /* 清除 BUFEN 位，禁用输出缓冲 */
    }
}

/*********************************************************************
 * @fn      Get_CalibrationValue
 *
 * @brief   获取 ADCx 的校准值（用于偏移校正）。
 *
 * @param   ADCx - 可以是 ADC1 或 ADC2，选择要操作的 ADC 外设。
 *
 * @return  校准值，为 16 位有符号数。
 */
int16_t Get_CalibrationValue(ADC_TypeDef *ADCx)
{
    __IO uint8_t  i, j;
    uint16_t      buf[10];
    __IO uint16_t t;

    /* 进行10次校准，获取10个校准结果 */
    for(i = 0; i < 10; i++)
    {
        ADC_ResetCalibration(ADCx);
        while(ADC_GetResetCalibrationStatus(ADCx))
            ;   /* 等待复位校准完成 */
        ADC_StartCalibration(ADCx);
        while(ADC_GetCalibrationStatus(ADCx))
            ;   /* 等待校准完成 */
        buf[i] = ADCx->RDATAR;   /* 读取校准结果（校准值存在规则数据寄存器中） */
    }

    /* 对10个结果进行冒泡排序，取中间6个值的平均值 */
    for(i = 0; i < 10; i++)
    {
        for(j = 0; j < 9; j++)
        {
            if(buf[j] > buf[j + 1])
            {
                t = buf[j];
                buf[j] = buf[j + 1];
                buf[j + 1] = t;
            }
        }
    }

    /* 去掉前2个和后2个，取中间6个求和 */
    t = 0;
    for(i = 0; i < 6; i++)
    {
        t += buf[i + 2];
    }

    /* 计算平均值，并四舍五入 */
    t = (t / 6) + ((t % 6) / 3);

    /* 返回校准值，以 2048 为基准（假设理想值为2048） */
    return (int16_t)(2048 - (int16_t)t);
}