/********************************** (C) COPYRIGHT *******************************
* 文件名            : ch32v30x_rcc.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2024/05/28
* 描述              : 本文件提供了所有RCC（复位与时钟控制）固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或不修改）和二进制代码用于南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/

#include "ch32v30x_rcc.h"

/* 以下宏定义用于在别名区域中定位RCC寄存器的位地址 */
#define RCC_OFFSET                 (RCC_BASE - PERIPH_BASE)  /* RCC寄存器相对于外设基地址的偏移 */

/* BDCTLR寄存器偏移（备份域控制寄存器） */
#define BDCTLR_OFFSET              (RCC_OFFSET + 0x20)

/* RCC寄存器位掩码（用于设置或清除特定位） */

/* CTLR（控制寄存器）位掩码 */
#define CTLR_HSEBYP_Reset          ((uint32_t)0xFFFBFFFF)  /* 清除HSE旁路位（位18）的掩码 */
#define CTLR_HSEBYP_Set            ((uint32_t)0x00040000)  /* 设置HSE旁路位的值 */
#define CTLR_HSEON_Reset           ((uint32_t)0xFFFEFFFF)  /* 清除HSE使能位（位16）的掩码 */
#define CTLR_HSEON_Set             ((uint32_t)0x00010000)  /* 设置HSE使能位的值 */
#define CTLR_HSITRIM_Mask          ((uint32_t)0xFFFFFF07)  /* HSI校准位（位7-3）的掩码（保留低3位） */

#define CFGR0_PLL_Mask             ((uint32_t)0xFFC0FFFF)  /* PLL配置掩码（清除PLL倍频和源选择位）用于非307芯片 */
#define CFGR0_PLL_Mask_1           ((uint32_t)0xFFC2FFFF)  /* 另一种PLL掩码（用于CH32V307） */

#define CFGR0_PLLMull_Mask         ((uint32_t)0x003C0000)  /* PLL倍频因子位（位19-18）掩码 */
#define CFGR0_PLLSRC_Mask          ((uint32_t)0x00010000)  /* PLL时钟源选择位（位16）掩码 */
#define CFGR0_PLLXTPRE_Mask        ((uint32_t)0x00020000)  /* HSE分频器（用于PLL输入）位（位17）掩码 */
#define CFGR0_SWS_Mask             ((uint32_t)0x0000000C)  /* 系统时钟切换状态位（位3-2）掩码 */
#define CFGR0_SW_Mask              ((uint32_t)0xFFFFFFFC)  /* 系统时钟切换位（位1-0）掩码（清除这两位） */
#define CFGR0_HPRE_Reset_Mask      ((uint32_t)0xFFFFFF0F)  /* AHB预分频器（位7-4）清除掩码 */
#define CFGR0_HPRE_Set_Mask        ((uint32_t)0x000000F0)  /* AHB预分频器设置掩码 */
#define CFGR0_PPRE1_Reset_Mask     ((uint32_t)0xFFFFF8FF)  /* APB1预分频器（位10-8）清除掩码 */
#define CFGR0_PPRE1_Set_Mask       ((uint32_t)0x00000700)  /* APB1预分频器设置掩码 */
#define CFGR0_PPRE2_Reset_Mask     ((uint32_t)0xFFFFC7FF)  /* APB2预分频器（位13-11）清除掩码 */
#define CFGR0_PPRE2_Set_Mask       ((uint32_t)0x00003800)  /* APB2预分频器设置掩码 */
#define CFGR0_ADCPRE_Reset_Mask    ((uint32_t)0xFFFF3FFF)  /* ADC预分频器（位15-14）清除掩码 */
#define CFGR0_ADCPRE_Set_Mask      ((uint32_t)0x0000C000)  /* ADC预分频器设置掩码 */

/* RSTSCKR（复位与时钟控制/状态寄存器）位掩码 */
#define RSTSCKR_RMVF_Set           ((uint32_t)0x01000000)  /* 清除复位标志位（位24）的值 */

/* CFGR2（配置寄存器2）位掩码 */
#define CFGR2_PREDIV1SRC           ((uint32_t)0x00010000)  /* PREDIV1时钟源选择位（位16） */
#define CFGR2_PREDIV1              ((uint32_t)0x0000000F)  /* PREDIV1分频因子位（位3-0） */
#define CFGR2_PREDIV2              ((uint32_t)0x000000F0)  /* PREDIV2分频因子位（位7-4） */
#define CFGR2_PLL2MUL              ((uint32_t)0x00000F00)  /* PLL2倍频因子位（位11-8） */
#define CFGR2_PLL3MUL              ((uint32_t)0x0000F000)  /* PLL3倍频因子位（位15-12） */

/* RCC标志位掩码（用于从标志位编号中提取位位置） */
#define FLAG_Mask                  ((uint8_t)0x1F)         /* 低5位，用于获取位索引（0-31） */

/* INTR（中断寄存器）字节2（位15-8）的基地址（用于使能中断） */
#define INTR_BYTE2_ADDRESS         ((uint32_t)0x40021009)

/* INTR寄存器字节3（位23-16）的基地址（用于清除中断挂起位） */
#define INTR_BYTE3_ADDRESS         ((uint32_t)0x4002100A)

/* CFGR0寄存器字节4（位31-24）的基地址（用于MCO配置） */
#define CFGR0_BYTE4_ADDRESS        ((uint32_t)0x40021007)

/* BDCTLR寄存器基地址（备份域控制寄存器） */
#define BDCTLR_ADDRESS             (PERIPH_BASE + BDCTLR_OFFSET)

/* 预分频表：用于根据AHB/APB预分频器设置计算实际分频值（右移位数） */
/* 索引对应预分频器编码值，值表示需要右移的位数 */
static __I uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

/* ADC预分频表：根据ADC预分频编码值得到实际分频系数 */
static __I uint8_t ADCPrescTable[4] = {2, 4, 6, 8};  /* 编码0->2分频，1->4分频，2->6分频，3->8分频 */

/*********************************************************************
 * @fn      RCC_DeInit
 *
 * @brief   将RCC时钟配置复位为默认复位状态。
 *          注意：如果HSE直接或通过PLL用作系统时钟，则不能停止HSE。
 *
 * @return  无
 */
void RCC_DeInit(void)
{
    /* 使能HSI振荡器（置位HSION位） */
    RCC->CTLR |= (uint32_t)0x00000001;

#ifdef CH32V30x_D8C
    /* 对于D8C型号（可能指大容量芯片），保留CFGR0的某些位，清除其他位 */
    RCC->CFGR0 &= (uint32_t)0xF8FF0000;
#else
    /* 对于其他型号，清除CFGR0的高16位中的部分位 */
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
#endif

    /* 清除HSEON、HSEBYP、CSSON等位，并关闭PLL */
    RCC->CTLR &= (uint32_t)0xFEF6FFFF;  /* 清除位17、位19等 */
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;  /* 清除HSEBYP位 */
    RCC->CFGR0 &= (uint32_t)0xFF80FFFF;  /* 清除PLL倍频、源选择等 */

#ifdef CH32V30x_D8C
    /* D8C型号：清除其他控制位，清除中断标志，复位CFGR2 */
    RCC->CTLR &= (uint32_t)0xEBFFFFFF;  /* 清除PLL2、PLL3等使能位 */
    RCC->INTR = 0x00FF0000;              /* 清除所有中断标志 */
    RCC->CFGR2 = 0x00000000;              /* 复位CFGR2 */
#else
    /* 非D8C型号：清除中断标志（CSS、PLLRDY等） */
    RCC->INTR = 0x009F0000;
#endif
}

/*********************************************************************
 * @fn      RCC_HSEConfig
 *
 * @brief   配置外部高速振荡器（HSE）。
 *
 * @param   RCC_HSE - 指定HSE的新状态，可以是以下值：
 *            RCC_HSE_OFF     - 关闭HSE振荡器
 *            RCC_HSE_ON      - 开启HSE振荡器
 *            RCC_HSE_Bypass  - HSE振荡器旁路，使用外部时钟输入
 *          注意：如果HSE直接或通过PLL用作系统时钟，则不能停止HSE。
 *
 * @return  无
 */
void RCC_HSEConfig(uint32_t RCC_HSE)
{
    /* 先清除HSEON和HSEBYP位 */
    RCC->CTLR &= CTLR_HSEON_Reset;   /* 清零位16（HSEON） */
    RCC->CTLR &= CTLR_HSEBYP_Reset;  /* 清零位18（HSEBYP） */

    switch(RCC_HSE)
    {
        case RCC_HSE_ON:
            RCC->CTLR |= CTLR_HSEON_Set;  /* 仅使能HSE */
            break;

        case RCC_HSE_Bypass:
            /* 同时设置HSEBYP和HSEON（旁路模式要求HSEON也使能） */
            RCC->CTLR |= CTLR_HSEBYP_Set | CTLR_HSEON_Set;
            break;

        default:  /* RCC_HSE_OFF 已经通过清除操作实现，无需额外动作 */
            break;
    }
}

/*********************************************************************
 * @fn      RCC_WaitForHSEStartUp
 *
 * @brief   等待HSE启动稳定。
 *
 * @return  READY   - HSE振荡器就绪稳定
 *          NoREADY - HSE尚未就绪（超时）
 */
ErrorStatus RCC_WaitForHSEStartUp(void)
{
    __IO uint32_t StartUpCounter = 0;  /* 启动等待计数器 */

    ErrorStatus status = NoREADY;
    FlagStatus  HSEStatus = RESET;

    do
    {
        HSEStatus = RCC_GetFlagStatus(RCC_FLAG_HSERDY);  /* 检查HSE就绪标志 */
        StartUpCounter++;
    } while((StartUpCounter != HSE_STARTUP_TIMEOUT) && (HSEStatus == RESET));

    if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)  /* 再次检查，确保标志确实置位 */
    {
        status = READY;
    }
    else
    {
        status = NoREADY;
    }

    return (status);
}

/*********************************************************************
 * @fn      RCC_AdjustHSICalibrationValue
 *
 * @brief   调整内部高速振荡器（HSI）的校准值。
 *
 * @param   HSICalibrationValue - 校准修调值，取值范围0~0x1F（5位）。
 *
 * @return  无
 */
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CTLR;
    tmpreg &= CTLR_HSITRIM_Mask;           /* 清除原来的校准位（位7-3） */
    tmpreg |= (uint32_t)HSICalibrationValue << 3;  /* 写入新的校准值（左移3位到位7-3） */
    RCC->CTLR = tmpreg;
}

/*********************************************************************
 * @fn      RCC_HSICmd
 *
 * @brief   使能或禁用内部高速振荡器（HSI）。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_HSICmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 0);   /* 置位HSION位（位0） */
    }
    else
    {
        RCC->CTLR &= ~(1 << 0);  /* 清零HSION位 */
    }
}

/*********************************************************************
 * @fn      RCC_PLLConfig
 *
 * @brief   配置PLL时钟源和倍频因子。
 *
 * @param   RCC_PLLSource - PLL输入时钟源：
 *            RCC_PLLSource_HSI_Div2 - HSI时钟2分频作为PLL输入
 *            RCC_PLLSource_PREDIV1  - PREDIV1时钟作为PLL输入
 * @param   RCC_PLLMul - PLL倍频因子，可以是 RCC_PLLMul_x，其中x表示倍频数。
 *            对于CH32V307，支持部分扩展倍频值（如x6.5等）。
 *            具体定义见头文件。
 *
 * @return  无
 */
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;

    /* 根据芯片型号选择不同的掩码清除PLL相关位 */
    if(((*(uint32_t *)0x1FFFF70C) & (1 << 14)) != (1 << 14))  /* 检查芯片特征标志位（位14） */
    { /* 对于非CH32V307型号（如CH32V103等） */
        tmpreg &= CFGR0_PLL_Mask;
    }
    else
    { /* 对于CH32V307型号 */
        tmpreg &= CFGR0_PLL_Mask_1;
    }

    tmpreg |= RCC_PLLSource | RCC_PLLMul;  /* 设置源和倍频因子 */
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PLLCmd
 *
 * @brief   使能或禁用PLL。
 *          注意：如果PLL用作系统时钟，则不能禁用它。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_PLLCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 24);   /* 置位PLLON位（位24） */
    }
    else
    {
        RCC->CTLR &= ~(1 << 24);  /* 清零PLLON位 */
    }
}

/*********************************************************************
 * @fn      RCC_SYSCLKConfig
 *
 * @brief   配置系统时钟源（SYSCLK）。
 *
 * @param   RCC_SYSCLKSource - 系统时钟源：
 *            RCC_SYSCLKSource_HSI    - HSI作为系统时钟
 *            RCC_SYSCLKSource_HSE    - HSE作为系统时钟
 *            RCC_SYSCLKSource_PLLCLK - PLL输出作为系统时钟
 *
 * @return  无
 */
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_SW_Mask;          /* 清除SW位（位1-0） */
    tmpreg |= RCC_SYSCLKSource;       /* 设置新的系统时钟源 */
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_GetSYSCLKSource
 *
 * @brief   返回当前用作系统时钟的时钟源。
 *
 * @return  0x00 - HSI作为系统时钟
 *          0x04 - HSE作为系统时钟
 *          0x08 - PLL作为系统时钟
 */
uint8_t RCC_GetSYSCLKSource(void)
{
    return ((uint8_t)(RCC->CFGR0 & CFGR0_SWS_Mask));  /* 读取SWS位（位3-2） */
}

/*********************************************************************
 * @fn      RCC_HCLKConfig
 *
 * @brief   配置AHB时钟（HCLK）分频。
 *
 * @param   RCC_SYSCLK - AHB时钟分频系数，由系统时钟（SYSCLK）分频得到。
 *            可取值为：RCC_SYSCLK_Div1, RCC_SYSCLK_Div2, ... RCC_SYSCLK_Div512。
 *
 * @return  无
 */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_HPRE_Reset_Mask;  /* 清除HPRE位（位7-4） */
    tmpreg |= RCC_SYSCLK;              /* 设置新的HPRE值 */
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PCLK1Config
 *
 * @brief   配置低速APB时钟（PCLK1）分频。
 *
 * @param   RCC_HCLK - APB1时钟分频系数，由AHB时钟（HCLK）分频得到。
 *            可取值为：RCC_HCLK_Div1, RCC_HCLK_Div2, ... RCC_HCLK_Div16。
 *
 * @return  无
 */
void RCC_PCLK1Config(uint32_t RCC_HCLK)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_PPRE1_Reset_Mask;  /* 清除PPRE1位（位10-8） */
    tmpreg |= RCC_HCLK;                  /* 设置新的PPRE1值 */
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PCLK2Config
 *
 * @brief   配置高速APB时钟（PCLK2）分频。
 *
 * @param   RCC_HCLK - APB2时钟分频系数，由AHB时钟（HCLK）分频得到。
 *            可取值为：RCC_HCLK_Div1, RCC_HCLK_Div2, ... RCC_HCLK_Div16。
 *
 * @return  无
 */
void RCC_PCLK2Config(uint32_t RCC_HCLK)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_PPRE2_Reset_Mask;  /* 清除PPRE2位（位13-11） */
    tmpreg |= RCC_HCLK << 3;            /* PPRE2位在CFGR0中的偏移是11，这里RCC_HCLK定义可能已经是偏移后的值？但根据标准库，RCC_HCLK_Div1等宏已经左移了8位？ 
                                           实际此函数写法可能是为了统一，RCC_HCLK参数应传入未偏移的值，然后左移3位（因为PPRE2起始位11相对于PPRE1起始位8多了3位）。 
                                           但更常见的是RCC_HCLK宏已经包含了正确的位偏移，所以这里直接使用RCC_HCLK即可。但源码中使用了 <<3，需要确认。
                                           我们按照代码原样注释。 */
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_ITConfig
 *
 * @brief   使能或禁用指定的RCC中断。
 *
 * @param   RCC_IT - 要配置的中断源，可以是以下值的组合：
 *            RCC_IT_LSIRDY - LSI就绪中断
 *            RCC_IT_LSERDY - LSE就绪中断
 *            RCC_IT_HSIRDY - HSI就绪中断
 *            RCC_IT_HSERDY - HSE就绪中断
 *            RCC_IT_PLLRDY - PLL就绪中断
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 向INTR寄存器的字节2（位15-8）写入相应的位以使能中断 */
        *(__IO uint8_t *)INTR_BYTE2_ADDRESS |= RCC_IT;
    }
    else
    {
        *(__IO uint8_t *)INTR_BYTE2_ADDRESS &= (uint8_t)~RCC_IT;
    }
}

/*********************************************************************
 * @fn      RCC_ADCCLKConfig
 *
 * @brief   配置ADC时钟（ADCCLK）分频。
 *
 * @param   RCC_PCLK2 - ADC时钟分频系数，由APB2时钟（PCLK2）分频得到。
 *            可取值为：RCC_PCLK2_Div2, RCC_PCLK2_Div4, RCC_PCLK2_Div6, RCC_PCLK2_Div8。
 *
 * @return  无
 */
void RCC_ADCCLKConfig(uint32_t RCC_PCLK2)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_ADCPRE_Reset_Mask;  /* 清除ADCPRE位（位15-14） */
    tmpreg |= RCC_PCLK2;                 /* 设置新的ADCPRE值 */
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_LSEConfig
 *
 * @brief   配置外部低速振荡器（LSE）。
 *
 * @param   RCC_LSE - 指定LSE的新状态，可以是以下值：
 *            RCC_LSE_OFF     - 关闭LSE
 *            RCC_LSE_ON      - 开启LSE
 *            RCC_LSE_Bypass  - 旁路LSE，使用外部时钟输入
 *
 * @return  无
 */
void RCC_LSEConfig(uint8_t RCC_LSE)
{
    /* 先写两次0（关闭LSE），确保清除之前的状态 */
    *(__IO uint8_t *)BDCTLR_ADDRESS = RCC_LSE_OFF;
    *(__IO uint8_t *)BDCTLR_ADDRESS = RCC_LSE_OFF;

    switch(RCC_LSE)
    {
        case RCC_LSE_ON:
            *(__IO uint8_t *)BDCTLR_ADDRESS = RCC_LSE_ON;  /* 仅使能LSE */
            break;

        case RCC_LSE_Bypass:
            /* 同时设置旁路和使能位（旁路模式下也必须使能） */
            *(__IO uint8_t *)BDCTLR_ADDRESS = RCC_LSE_Bypass | RCC_LSE_ON;
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      RCC_LSICmd
 *
 * @brief   使能或禁用内部低速振荡器（LSI）。
 *          注意：如果独立看门狗（IWDG）正在运行，则不能禁用LSI。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_LSICmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->RSTSCKR |= (1 << 0);   /* 置位LSION位（位0） */
    }
    else
    {
        RCC->RSTSCKR &= ~(1 << 0);  /* 清零LSION位 */
    }
}

/*********************************************************************
 * @fn      RCC_RTCCLKConfig
 *
 * @brief   一旦选择了RTC时钟，除非复位备份域，否则不能更改。
 *
 * @param   RCC_RTCCLKSource - RTC时钟源：
 *            RCC_RTCCLKSource_LSE        - LSE作为RTC时钟
 *            RCC_RTCCLKSource_LSI        - LSI作为RTC时钟
 *            RCC_RTCCLKSource_HSE_Div128 - HSE的128分频作为RTC时钟
 *
 * @return  无
 */
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource)
{
    /* 设置BDCTLR寄存器的相应位（位8-9和位0-2? 根据手册，RTCSEL[1:0]位于位8-9，以及RTCEN位于位15。
       这里直接使用RCC_RTCCLKSource宏，它应该包含了正确的位设置。 */
    RCC->BDCTLR |= RCC_RTCCLKSource;
}

/*********************************************************************
 * @fn      RCC_RTCCLKCmd
 *
 * @brief   必须在通过RCC_RTCCLKConfig选择了RTC时钟后才能调用此函数。
 *
 * @param   NewState - ENABLE 或 DISABLE（使能或禁用RTC时钟）。
 *
 * @return  无
 */
void RCC_RTCCLKCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->BDCTLR |= (1 << 15);   /* 置位RTCEN位（位15），使能RTC时钟 */
    }
    else
    {
        RCC->BDCTLR &= ~(1 << 15);  /* 清零RTCEN位 */
    }
}

/*********************************************************************
 * @fn      RCC_GetClocksFreq
 *
 * @brief   计算并返回各时钟频率（SYSCLK、HCLK、PCLK1、PCLK2、ADCCLK）。
 *          注意：当使用HSE分数倍频时，结果可能不完全精确。
 *
 * @param   RCC_Clocks - 指向RCC_ClocksTypeDef结构体的指针，用于存储计算结果。
 *
 * @return  无
 */
void RCC_GetClocksFreq(RCC_ClocksTypeDef *RCC_Clocks)
{
    uint32_t tmp = 0, pllmull = 0, pllsource = 0, presc = 0;
    uint8_t Pll_6_5 = 0;  /* 用于标记是否使用6.5倍频（CH32V307特有） */

#ifdef CH32V30x_D8C
    uint8_t Pll2mull = 0;  /* PLL2倍频因子（用于D8C型号） */
#endif

    tmp = RCC->CFGR0 & CFGR0_SWS_Mask;  /* 获取系统时钟切换状态 */

    switch(tmp)
    {
        case 0x00:  /* HSI作为系统时钟 */
            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
            break;

        case 0x04:  /* HSE作为系统时钟 */
            RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
            break;

        case 0x08:  /* PLL作为系统时钟 */
            pllmull = RCC->CFGR0 & CFGR0_PLLMull_Mask;   /* 获取PLL倍频因子编码 */
            pllsource = RCC->CFGR0 & CFGR0_PLLSRC_Mask;  /* 获取PLL输入源 */

            /* 将编码转换为实际倍频值（标准算法：编码右移18后+2） */
            pllmull = (pllmull >> 18) + 2;

            if(((*(uint32_t *)0x1FFFF70C) & (1 << 14)) != (1 << 14))
            { /* 对于非CH32V307型号 */
                if(pllmull == 17)  /* 编码15对应倍频18？根据手册，对于非307，倍频编码15表示18倍频 */
                    pllmull = 18;
            }
            else
            { /* 对于CH32V307，倍频编码有特殊含义 */
                if(pllmull == 2)    /* 编码0表示18倍频（特殊） */
                    pllmull = 18;
                if(pllmull == 15)   /* 编码13表示6.5倍频？代码中为13，但注释说*6.5 */
                {
                    pllmull = 13;   /* 实际倍频值为13，后面再除以2得到6.5 */
                    Pll_6_5 = 1;
                }
                if(pllmull == 16)   /* 编码14表示15倍频？ */
                    pllmull = 15;
                if(pllmull == 17)   /* 编码15表示16倍频？ */
                    pllmull = 16;
            }

            if(pllsource == 0x00)  /* PLL输入源为HSI/2 或 HSI（取决于预分频） */
            {
                if(EXTEN->EXTEN_CTR & EXTEN_PLL_HSI_PRE)  /* 如果设置了HSI不分频直接给PLL（某些芯片特性） */
                {
                    RCC_Clocks->SYSCLK_Frequency = (HSI_VALUE) * pllmull;
                }
                else
                {
                    RCC_Clocks->SYSCLK_Frequency = (HSI_VALUE >> 1) * pllmull;  /* HSI/2作为PLL输入 */
                }
            }
            else  /* PLL输入源为PREDIV1（通常是HSE或其分频） */
            {
#ifdef CH32V30x_D8
                /* 对于D8型号（非D8C），检查HSE是否二分频给PLL */
                if((RCC->CFGR0 & CFGR0_PLLXTPRE_Mask) != (uint32_t)RESET)
                {
                    RCC_Clocks->SYSCLK_Frequency = (HSE_VALUE >> 1) * pllmull;  /* HSE/2作为PLL输入 */
                }
                else
                {
                    RCC_Clocks->SYSCLK_Frequency = HSE_VALUE * pllmull;
                }
#else
                /* 对于支持PLL2、PLL3的高级型号，需考虑更多分频链 */
                if(RCC->CFGR2 & (1<<16)) /* 如果PREDIV1源选择为PLL2 */
                {
                    /* 先计算PLL2输出频率：HSE经过PREDIV2分频后，乘以PLL2倍频因子 */
                    RCC_Clocks->SYSCLK_Frequency = HSE_VALUE / (((RCC->CFGR2 & 0xF0)>>4) + 1);  /* PREDIV2分频 */

                    Pll2mull = (uint8_t)((RCC->CFGR2 & 0xF00)>>8);  /* PLL2倍频编码 */

                    /* 根据编码计算实际倍频值（特殊值处理） */
                    if(Pll2mull == 0) RCC_Clocks->SYSCLK_Frequency = (RCC_Clocks->SYSCLK_Frequency * 5)>>1;  /* 2.5倍 */
                    else if(Pll2mull == 1) RCC_Clocks->SYSCLK_Frequency = (RCC_Clocks->SYSCLK_Frequency * 25)>>1; /* 12.5倍 */
                    else if(Pll2mull == 15) RCC_Clocks->SYSCLK_Frequency = RCC_Clocks->SYSCLK_Frequency * 20; /* 20倍 */
                    else  RCC_Clocks->SYSCLK_Frequency = RCC_Clocks->SYSCLK_Frequency * (Pll2mull + 2);  /* 普通倍频 */

                    /* 再经过PREDIV1分频后作为PLL输入 */
                    RCC_Clocks->SYSCLK_Frequency = RCC_Clocks->SYSCLK_Frequency / ((RCC->CFGR2 & 0xF) + 1);  /* PREDIV1分频 */
                }
                else /* PREDIV1源为HSE */
                {
                    /* HSE直接经PREDIV1分频后作为PLL输入 */
                    RCC_Clocks->SYSCLK_Frequency = HSE_VALUE / ((RCC->CFGR2 & 0xF) + 1);  /* PREDIV1分频 */
                }

                /* 再乘以PLL倍频因子得到最终SYSCLK */
                RCC_Clocks->SYSCLK_Frequency = RCC_Clocks->SYSCLK_Frequency * pllmull;
#endif
            }

            if(Pll_6_5 == 1)  /* 如果是6.5倍频，则之前乘了13，现在除以2得到6.5 */
                RCC_Clocks->SYSCLK_Frequency = (RCC_Clocks->SYSCLK_Frequency / 2);

            break;

        default:
            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;  /* 默认用HSI */
            break;
    }

    /* 计算HCLK = SYSCLK / HPRE分频 */
    tmp = RCC->CFGR0 & CFGR0_HPRE_Set_Mask;  /* 获取HPRE编码 */
    tmp = tmp >> 4;
    presc = APBAHBPrescTable[tmp];           /* 查询表得到实际右移位次数 */
    RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc;

    /* 计算PCLK1 = HCLK / PPRE1分频 */
    tmp = RCC->CFGR0 & CFGR0_PPRE1_Set_Mask;
    tmp = tmp >> 8;
    presc = APBAHBPrescTable[tmp];
    RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency >> presc;

    /* 计算PCLK2 = HCLK / PPRE2分频 */
    tmp = RCC->CFGR0 & CFGR0_PPRE2_Set_Mask;
    tmp = tmp >> 11;
    presc = APBAHBPrescTable[tmp];
    RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency >> presc;

    /* 计算ADCCLK = PCLK2 / ADCPRE分频 */
    tmp = RCC->CFGR0 & CFGR0_ADCPRE_Set_Mask;
    tmp = tmp >> 14;
    presc = ADCPrescTable[tmp];              /* 查询表得到实际分频系数（2,4,6,8） */
    RCC_Clocks->ADCCLK_Frequency = RCC_Clocks->PCLK2_Frequency / presc;
}

/*********************************************************************
 * @fn      RCC_AHBPeriphClockCmd
 *
 * @brief   使能或禁用AHB总线外设的时钟。
 *
 * @param   RCC_AHBPeriph - 要控制的AHB外设时钟，可以是以下宏的组合：
 *            RCC_AHBPeriph_DMA1, RCC_AHBPeriph_DMA2, RCC_AHBPeriph_SRAM,
 *            RCC_AHBPeriph_CRC, RCC_AHBPeriph_FSMC, RCC_AHBPeriph_RNG,
 *            RCC_AHBPeriph_SDIO, RCC_AHBPeriph_USBHS, RCC_AHBPeriph_USBFS,
 *            RCC_AHBPeriph_DVP, RCC_AHBPeriph_ETH_MAC,
 *            RCC_AHBPeriph_ETH_MAC_Tx, RCC_AHBPeriph_ETH_MAC_Rx。
 *          注意：SRAM时钟只能在睡眠模式下禁用。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->AHBPCENR |= RCC_AHBPeriph;   /* 使能时钟 */
    }
    else
    {
        RCC->AHBPCENR &= ~RCC_AHBPeriph;  /* 禁用时钟 */
    }
}

/*********************************************************************
 * @fn      RCC_APB2PeriphClockCmd
 *
 * @brief   使能或禁用高速APB（APB2）总线外设的时钟。
 *
 * @param   RCC_APB2Periph - 要控制的APB2外设时钟，可以是以下宏的组合：
 *            RCC_APB2Periph_AFIO, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB,
 *            RCC_APB2Periph_GPIOC, RCC_APB2Periph_GPIOD, RCC_APB2Periph_GPIOE,
 *            RCC_APB2Periph_ADC1, RCC_APB2Periph_ADC2, RCC_APB2Periph_TIM1,
 *            RCC_APB2Periph_SPI1, RCC_APB2Periph_TIM8, RCC_APB2Periph_USART1,
 *            RCC_APB2Periph_TIM9, RCC_APB2Periph_TIM10。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->APB2PCENR |= RCC_APB2Periph;   /* 使能时钟 */
    }
    else
    {
        RCC->APB2PCENR &= ~RCC_APB2Periph;  /* 禁用时钟 */
    }
}

/*********************************************************************
 * @fn      RCC_APB1PeriphClockCmd
 *
 * @brief   使能或禁用低速APB（APB1）总线外设的时钟。
 *
 * @param   RCC_APB1Periph - 要控制的APB1外设时钟，可以是以下宏的组合：
 *            RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3, RCC_APB1Periph_TIM4,
 *            RCC_APB1Periph_TIM5, RCC_APB1Periph_TIM6, RCC_APB1Periph_TIM7,
 *            RCC_APB1Periph_UART6, RCC_APB1Periph_UART7, RCC_APB1Periph_UART8,
 *            RCC_APB1Periph_WWDG, RCC_APB1Periph_SPI2, RCC_APB1Periph_SPI3,
 *            RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4,
 *            RCC_APB1Periph_UART5, RCC_APB1Periph_I2C1, RCC_APB1Periph_I2C2,
 *            RCC_APB1Periph_USB, RCC_APB1Periph_CAN1, RCC_APB1Periph_BKP,
 *            RCC_APB1Periph_PWR, RCC_APB1Periph_DAC。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->APB1PCENR |= RCC_APB1Periph;   /* 使能时钟 */
    }
    else
    {
        RCC->APB1PCENR &= ~RCC_APB1Periph;  /* 禁用时钟 */
    }
}

/*********************************************************************
 * @fn      RCC_APB2PeriphResetCmd
 *
 * @brief   强制或释放高速APB（APB2）外设的复位。
 *
 * @param   RCC_APB2Periph - 要复位的外设，组合同RCC_APB2PeriphClockCmd。
 * @param   NewState - ENABLE（复位）或 DISABLE（释放复位）。
 *
 * @return  无
 */
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->APB2PRSTR |= RCC_APB2Periph;   /* 置位复位位 */
    }
    else
    {
        RCC->APB2PRSTR &= ~RCC_APB2Periph;  /* 清零复位位 */
    }
}

/*********************************************************************
 * @fn      RCC_APB1PeriphResetCmd
 *
 * @brief   强制或释放低速APB（APB1）外设的复位。
 *
 * @param   RCC_APB1Periph - 要复位的外设，组合同RCC_APB1PeriphClockCmd。
 * @param   NewState - ENABLE（复位）或 DISABLE（释放复位）。
 *
 * @return  无
 */
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->APB1PRSTR |= RCC_APB1Periph;   /* 置位复位位 */
    }
    else
    {
        RCC->APB1PRSTR &= ~RCC_APB1Periph;  /* 清零复位位 */
    }
}

/*********************************************************************
 * @fn      RCC_BackupResetCmd
 *
 * @brief   强制或释放备份域复位。
 *
 * @param   NewState - ENABLE（复位备份域）或 DISABLE（释放复位）。
 *
 * @return  无
 */
void RCC_BackupResetCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->BDCTLR |= (1 << 16);   /* 置位BDRST位（位16），复位备份域 */
    }
    else
    {
        RCC->BDCTLR &= ~(1 << 16);  /* 清零BDRST位 */
    }
}

/*********************************************************************
 * @fn      RCC_ClockSecuritySystemCmd
 *
 * @brief   使能或禁用时钟安全系统（CSS）。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_ClockSecuritySystemCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 19);   /* 置位CSSON位（位19） */
    }
    else
    {
        RCC->CTLR &= ~(1 << 19);  /* 清零CSSON位 */
    }
}

/*********************************************************************
 * @fn      RCC_MCOConfig
 *
 * @brief   选择在MCO引脚上输出的时钟源。
 *
 * @param   RCC_MCO - 要输出的时钟源，可以是以下值：
 *            RCC_MCO_NoClock       - 无时钟输出
 *            RCC_MCO_SYSCLK        - 系统时钟
 *            RCC_MCO_HSI           - HSI时钟
 *            RCC_MCO_HSE           - HSE时钟
 *            RCC_MCO_PLLCLK_Div2   - PLL时钟的2分频
 *            RCC_MCO_PLL2CLK       - PLL2时钟（仅部分芯片）
 *            RCC_MCO_PLL3CLK_Div2  - PLL3时钟的2分频
 *            RCC_MCO_XT1           - 外部晶振输入
 *            RCC_MCO_PLL3CLK       - PLL3时钟
 *
 * @return  无
 */
void RCC_MCOConfig(uint8_t RCC_MCO)
{
    /* 直接向CFGR0的字节4（位31-24）写入MCO配置值 */
    *(__IO uint8_t *)CFGR0_BYTE4_ADDRESS = RCC_MCO;
}

/*********************************************************************
 * @fn      RCC_GetFlagStatus
 *
 * @brief   检查指定的RCC标志位是否置位。
 *
 * @param   RCC_FLAG - 要检查的标志，可以是以下值：
 *            RCC_FLAG_HSIRDY, RCC_FLAG_HSERDY, RCC_FLAG_PLLRDY,
 *            RCC_FLAG_PLL2RDY, RCC_FLAG_PLL3RDY, RCC_FLAG_LSERDY,
 *            RCC_FLAG_LSIRDY, RCC_FLAG_PINRST, RCC_FLAG_PORRST,
 *            RCC_FLAG_SFTRST, RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST,
 *            RCC_FLAG_LPWRRST。
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
    uint32_t tmp = 0;
    uint32_t statusreg = 0;

    FlagStatus bitstatus = RESET;
    tmp = RCC_FLAG >> 5;  /* 高3位用于选择寄存器：1->CTLR, 2->BDCTLR, 其他->RSTSCKR */

    if(tmp == 1)
    {
        statusreg = RCC->CTLR;
    }
    else if(tmp == 2)
    {
        statusreg = RCC->BDCTLR;
    }
    else
    {
        statusreg = RCC->RSTSCKR;
    }

    tmp = RCC_FLAG & FLAG_Mask;  /* 低5位为寄存器中的位偏移 */

    if((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
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
 * @fn      RCC_ClearFlag
 *
 * @brief   清除RCC的复位标志。
 *          注意：可清除的标志包括：RCC_FLAG_PINRST, RCC_FLAG_PORRST,
 *          RCC_FLAG_SFTRST, RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST, RCC_FLAG_LPWRRST。
 *
 * @return  无
 */
void RCC_ClearFlag(void)
{
    RCC->RSTSCKR |= RSTSCKR_RMVF_Set;  /* 置位RMVF位（位24），清除所有复位标志 */
}

/*********************************************************************
 * @fn      RCC_GetITStatus
 *
 * @brief   检查指定的RCC中断是否发生。
 *
 * @param   RCC_IT - 要检查的中断源，可以是以下值：
 *            RCC_IT_LSIRDY, RCC_IT_LSERDY, RCC_IT_HSIRDY, RCC_IT_HSERDY,
 *            RCC_IT_PLLRDY, RCC_IT_PLL2RDY, RCC_IT_PLL3RDY, RCC_IT_CSS。
 *
 * @return  ITStatus - SET 或 RESET。
 */
ITStatus RCC_GetITStatus(uint8_t RCC_IT)
{
    ITStatus bitstatus = RESET;

    if((RCC->INTR & RCC_IT) != (uint32_t)RESET)  /* 检查INTR寄存器中对应位 */
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
 * @fn      RCC_ClearITPendingBit
 *
 * @brief   清除RCC的中断挂起位。
 *
 * @param   RCC_IT - 要清除的中断源，同RCC_GetITStatus中的参数。
 *
 * @return  无
 */
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
    /* 向INTR寄存器的字节3（位23-16）写入对应的位，以清除中断标志 */
    *(__IO uint8_t *)INTR_BYTE3_ADDRESS = RCC_IT;
}

/*********************************************************************
 * @fn      RCC_PREDIV1Config
 *
 * @brief   配置PREDIV1分频因子。
 *
 * @param   RCC_PREDIV1_Source - PREDIV1时钟源：
 *            RCC_PREDIV1_Source_HSE   - HSE作为PREDIV1输入
 *            RCC_PREDIV1_Source_PLL2  - PLL2作为PREDIV1输入
 * @param   RCC_PREDIV1_Div - PREDIV1分频系数，可取 RCC_PREDIV1_Divx，x=1~16。
 *          注意：此函数必须在PLL禁用时调用。
 *
 * @return  无
 */
void RCC_PREDIV1Config(uint32_t RCC_PREDIV1_Source, uint32_t RCC_PREDIV1_Div)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR2;
    tmpreg &= ~(CFGR2_PREDIV1 | CFGR2_PREDIV1SRC);  /* 清除PREDIV1分频位和源选择位 */
    tmpreg |= RCC_PREDIV1_Source | RCC_PREDIV1_Div; /* 设置新值 */
    RCC->CFGR2 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PREDIV2Config
 *
 * @brief   配置PREDIV2分频因子。
 *
 * @param   RCC_PREDIV2_Div - PREDIV2分频系数，可取 RCC_PREDIV2_Divx，x=1~16。
 *          注意：此函数必须在PLL2和PLL3都禁用时调用。
 *
 * @return  无
 */
void RCC_PREDIV2Config(uint32_t RCC_PREDIV2_Div)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR2;
    tmpreg &= ~CFGR2_PREDIV2;  /* 清除PREDIV2分频位（位7-4） */
    tmpreg |= RCC_PREDIV2_Div; /* 设置新值 */
    RCC->CFGR2 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PLL2Config
 *
 * @brief   配置PLL2倍频因子。
 *
 * @param   RCC_PLL2Mul - PLL2倍频因子，可取 RCC_PLL2Mul_x，x可以是4~16、2.5、12.5、20等。
 *          注意：此函数必须在PLL2禁用时调用。
 *
 * @return  无
 */
void RCC_PLL2Config(uint32_t RCC_PLL2Mul)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR2;
    tmpreg &= ~CFGR2_PLL2MUL;  /* 清除PLL2倍频位（位11-8） */
    tmpreg |= RCC_PLL2Mul;      /* 设置新值 */
    RCC->CFGR2 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PLL2Cmd
 *
 * @brief   使能或禁用PLL2。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *          注意：如果PLL2间接用作系统时钟（例如作为PLL的输入），则不能禁用它。
 *
 * @return  无
 */
void RCC_PLL2Cmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 26);   /* 置位PLL2ON位（位26） */
    }
    else
    {
        RCC->CTLR &= ~(1 << 26);  /* 清零PLL2ON位 */
    }
}

/*********************************************************************
 * @fn      RCC_PLL3Config
 *
 * @brief   配置PLL3倍频因子。
 *
 * @param   RCC_PLL3Mul - PLL3倍频因子，可取 RCC_PLL3Mul_x，范围同PLL2。
 *          注意：此函数必须在PLL3禁用时调用。
 *
 * @return  无
 */
void RCC_PLL3Config(uint32_t RCC_PLL3Mul)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR2;
    tmpreg &= ~CFGR2_PLL3MUL;  /* 清除PLL3倍频位（位15-12） */
    tmpreg |= RCC_PLL3Mul;      /* 设置新值 */
    RCC->CFGR2 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PLL3Cmd
 *
 * @brief   使能或禁用PLL3。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_PLL3Cmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 28);   /* 置位PLL3ON位（位28） */
    }
    else
    {
        RCC->CTLR &= ~(1 << 28);  /* 清零PLL3ON位 */
    }
}

/*********************************************************************
 * @fn      RCC_USBFSCLKConfig
 *
 * @brief   配置USB OTG FS时钟源。
 *
 * @param   RCC_USBFSCLKSource - USB FS时钟源，可取：
 *            RCC_USBFSCLKSource_PLLCLK_Div1 - PLL时钟1分频
 *            RCC_USBFSCLKSource_PLLCLK_Div2 - PLL时钟2分频
 *            RCC_USBFSCLKSource_PLLCLK_Div3 - PLL时钟3分频
 *
 * @return  无
 */
void RCC_USBFSCLKConfig(uint32_t RCC_USBFSCLKSource)
{
    RCC->CFGR0 &= ~(3 << 22);               /* 清除位23-22（USBFSPRE） */
    RCC->CFGR0 |= RCC_USBFSCLKSource << 22; /* 设置分频系数 */
}

/*********************************************************************
 * @fn      RCC_I2S2CLKConfig
 *
 * @brief   配置I2S2时钟源。
 *
 * @param   RCC_I2S2CLKSource - I2S2时钟源，可取：
 *            RCC_I2S2CLKSource_SYSCLK    - 系统时钟
 *            RCC_I2S2CLKSource_PLL3_VCO  - PLL3 VCO时钟
 *          注意：必须在使能I2S2 APB时钟之前调用。
 *
 * @return  无
 */
void RCC_I2S2CLKConfig(uint32_t RCC_I2S2CLKSource)
{
    RCC->CFGR2 &= ~(1 << 17);               /* 清除I2S2SRC位（位17） */
    RCC->CFGR2 |= RCC_I2S2CLKSource << 17;  /* 设置时钟源 */
}

/*********************************************************************
 * @fn      RCC_I2S3CLKConfig
 *
 * @brief   配置I2S3时钟源。
 *
 * @param   RCC_I2S3CLKSource - I2S3时钟源，同I2S2。
 *
 * @return  无
 */
void RCC_I2S3CLKConfig(uint32_t RCC_I2S3CLKSource)
{
    RCC->CFGR2 &= ~(1 << 18);               /* 清除I2S3SRC位（位18） */
    RCC->CFGR2 |= RCC_I2S3CLKSource << 18;  /* 设置时钟源 */
}

/*********************************************************************
 * @fn      RCC_AHBPeriphResetCmd
 *
 * @brief   强制或释放AHB外设的复位。
 *
 * @param   RCC_AHBPeriph - 要复位的AHB外设，可取：
 *            RCC_AHBPeriph_USBFS
 *            RCC_AHBPeriph_ETH_MAC
 * @param   NewState - ENABLE（复位）或 DISABLE（释放复位）。
 *
 * @return  无
 */
void RCC_AHBPeriphResetCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->AHBRSTR |= RCC_AHBPeriph;   /* 置位复位位 */
    }
    else
    {
        RCC->AHBRSTR &= ~RCC_AHBPeriph;  /* 清零复位位 */
    }
}

/*********************************************************************
 * @fn      RCC_ADCCLKADJcmd
 *
 * @brief   使能或禁用ADC时钟占空比调整。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_ADCCLKADJcmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->CFGR0 |= (1 << 31);   /* 置位ADCPREDIV位（位31），使能占空比调整 */
    }
    else
    {
        RCC->CFGR0 &= ~(1 << 31);  /* 清零该位 */
    }
}

/*********************************************************************
 * @fn      RCC_RNGCLKConfig
 *
 * @brief   配置随机数发生器（RNG）时钟源。
 *
 * @param   RCC_RNGCLKSource - RNG时钟源，可取：
 *            RCC_RNGCLKSource_SYSCLK    - 系统时钟
 *            RCC_RNGCLKSource_PLL3_VCO  - PLL3 VCO时钟
 *
 * @return  无
 */
void RCC_RNGCLKConfig(uint32_t RCC_RNGCLKSource)
{
    RCC->CFGR2 &= ~(1 << 19);               /* 清除RNGSRC位（位19） */
    RCC->CFGR2 |= RCC_RNGCLKSource << 19;   /* 设置时钟源 */
}

/*********************************************************************
 * @fn      RCC_ETH1GCLKConfig
 *
 * @brief   配置以太网1G时钟源。
 *
 * @param   RCC_ETH1GCLKSource - 以太网1G时钟源，可取：
 *            RCC_ETH1GCLKSource_PLL2_VCO   - PLL2 VCO时钟
 *            RCC_ETH1GCLKSource_PLL3_VCO   - PLL3 VCO时钟
 *            RCC_ETH1GCLKSource_PB1_IN     - GPIO PB1输入时钟
 *
 * @return  无
 */
void RCC_ETH1GCLKConfig(uint32_t RCC_ETH1GCLKSource)
{
    RCC->CFGR2 &= ~(3 << 20);                /* 清除ETH1GSRC位（位21-20） */
    RCC->CFGR2 |= RCC_ETH1GCLKSource << 20;  /* 设置时钟源 */
}

/*********************************************************************
 * @fn      RCC_ETH1G_125Mcmd
 *
 * @brief   使能或禁用以太网1G 125M时钟输出。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_ETH1G_125Mcmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->CFGR2 |= (1 << 22);   /* 置位ETH1G125MEN位（位22） */
    }
    else
    {
        RCC->CFGR2 &= ~(1 << 22);  /* 清零该位 */
    }
}

/*********************************************************************
 * @fn      RCC_USBHSConfig
 *
 * @brief   配置USB高速时钟分频。
 *
 * @param   RCC_USBHS - USB高速时钟分频系数，可取：
 *            RCC_USBPLL_Div1 ~ RCC_USBPLL_Div8。
 *
 * @return  无
 */
void RCC_USBHSConfig(uint32_t RCC_USBHS)
{
    RCC->CFGR2 &= ~(7 << 24);           /* 清除USBHSPRE位（位26-24） */
    RCC->CFGR2 |= RCC_USBHS << 24;      /* 设置分频值 */
}

/*********************************************************************
 * @fn      RCC_USBHSPLLCLKConfig
 *
 * @brief   配置USB高速PLL时钟源。
 *
 * @param   RCC_USBHSPLLCLKSource - USB高速PLL时钟源，可取：
 *            RCC_USBHSPLLCLKSource_HSE - HSE时钟
 *            RCC_USBHSPLLCLKSource_HSI - HSI时钟
 *
 * @return  无
 */
void RCC_USBHSPLLCLKConfig(uint32_t RCC_USBHSPLLCLKSource)
{
    RCC->CFGR2 &= ~(1 << 27);                /* 清除USBPLLSRC位（位27） */
    RCC->CFGR2 |= RCC_USBHSPLLCLKSource << 27; /* 设置时钟源 */
}

/*********************************************************************
 * @fn      RCC_USBHSPLLCKREFCLKConfig
 *
 * @brief   配置USB高速PLL参考时钟频率。
 *
 * @param   RCC_USBHSPLLCKREFCLKSource - 参考时钟频率选择，可取：
 *            RCC_USBHSPLLCKREFCLK_3M - 3MHz
 *            RCC_USBHSPLLCKREFCLK_4M - 4MHz
 *            RCC_USBHSPLLCKREFCLK_8M - 8MHz
 *            RCC_USBHSPLLCKREFCLK_5M - 5MHz
 *
 * @return  无
 */
void RCC_USBHSPLLCKREFCLKConfig(uint32_t RCC_USBHSPLLCKREFCLKSource)
{
    RCC->CFGR2 &= ~(3 << 28);                /* 清除USBPLLREF位（位29-28） */
    RCC->CFGR2 |= RCC_USBHSPLLCKREFCLKSource << 28; /* 设置参考时钟 */
}

/*********************************************************************
 * @fn      RCC_USBHSPHYPLLALIVEcmd
 *
 * @brief   使能或禁用USBHS PHY PLL保持活动状态（即使在睡眠模式）。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void RCC_USBHSPHYPLLALIVEcmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->CFGR2 |= (1 << 30);   /* 置位PHYPLLALIVE位（位30） */
    }
    else
    {
        RCC->CFGR2 &= ~(1 << 30);  /* 清零该位 */
    }
}

/*********************************************************************
 * @fn      RCC_USBCLK48MConfig
 *
 * @brief   配置USB 48MHz时钟源。
 *
 * @param   RCC_USBCLK48MSource - USB 48MHz时钟源，可取：
 *            RCC_USBCLK48MCLKSource_PLLCLK  - PLL时钟
 *            RCC_USBCLK48MCLKSource_USBPHY  - USB PHY时钟
 *
 * @return  无
 */
void RCC_USBCLK48MConfig(uint32_t RCC_USBCLK48MSource)
{
    RCC->CFGR2 &= ~(1 << 31);                /* 清除USBCLK48MSRC位（位31） */
    RCC->CFGR2 |= RCC_USBCLK48MSource << 31; /* 设置时钟源 */
}