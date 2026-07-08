/********************************** (C) COPYRIGHT *******************************
* 文件名          : ch32v30x_tim.c
* 作者            : WCH
* 版本            : V1.0.1
* 日期            : 2025/07/04
* 描述            : 此文件提供了所有TIM固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意: 本软件（修改或未修改）及二进制代码用于南京沁恒微电子生产的微控制器。
*******************************************************************************/

#include "ch32v30x_tim.h"
#include "ch32v30x_rcc.h"

/* TIM寄存器位掩码定义 */
#define SMCFGR_ETR_Mask    ((uint16_t)0x00FF) // SMCFGR寄存器中ETR相关位的掩码（低8位）
#define CHCTLR_Offset      ((uint16_t)0x0018) // 从TIM基地址到CHCTLR1寄存器的偏移量
#define CCER_CCE_Set       ((uint16_t)0x0001) // CCER寄存器中CCxE位（使能位）的置位值
#define CCER_CCNE_Set      ((uint16_t)0x0004) // CCER寄存器中CCxNE位（互补通道使能位）的置位值

/* 静态函数声明 */
static void TI1_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI2_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI3_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);
static void TI4_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter);

/*********************************************************************
 * @函数名 TIM_DeInit
 *
 * @描述   将TIMx外设寄存器恢复为其默认复位值。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *
 * @返回值 无
 */
void TIM_DeInit(TIM_TypeDef *TIMx)
{
    if(TIMx == TIM1)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM1, DISABLE);
    }
    else if(TIMx == TIM8)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM8, DISABLE);
    }
    else if(TIMx == TIM9)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM9, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM9, DISABLE);
    }
    else if(TIMx == TIM10)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM10, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_TIM10, DISABLE);
    }
    else if(TIMx == TIM2)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM2, DISABLE);
    }
    else if(TIMx == TIM3)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM3, DISABLE);
    }
    else if(TIMx == TIM4)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM4, DISABLE);
    }
    else if(TIMx == TIM5)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM5, DISABLE);
    }
    else if(TIMx == TIM6)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM6, DISABLE);
    }
    else if(TIMx == TIM7)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_TIM7, DISABLE);
    }
}

/*********************************************************************
 * @函数名 TIM_TimeBaseInit
 *
 * @描述   根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx时基单元外设。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_TimeBaseInitStruct - 指向TIM_TimeBaseInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct)
{
    uint16_t tmpcr1 = 0;
    uint32_t temper1 = 0;
    temper1 = ((*( uint32_t * )0x1FFFF704) & 0x000000F0) >> 4; // 读取系统时钟配置寄存器中的时钟源信息
    tmpcr1 = TIMx->CTLR1;

    // 如果系统时钟源为PLL，并且TIMx是高级定时器，则设置CTLR1寄存器的位13（可能用于时钟分频或对齐模式）
    if(((temper1>=4)&&(temper1<=8))&&((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10)))
    {
        tmpcr1 |= 1<<13;
    }

    // 对于支持计数模式的定时器，配置计数模式
    if((TIMx == TIM1) || (TIMx == TIM2) || (TIMx == TIM3) || (TIMx == TIM4) ||
       (TIMx == TIM5) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_DIR | TIM_CMS))); // 清零方向和中央对齐模式位
        tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_CounterMode; // 设置计数模式
    }

    // 对于支持时钟分频的定时器，配置时钟分频
    if((TIMx != TIM6) && (TIMx != TIM7))
    {
        tmpcr1 &= (uint16_t)(~((uint16_t)TIM_CTLR1_CKD)); // 清零时钟分频位
        tmpcr1 |= (uint32_t)TIM_TimeBaseInitStruct->TIM_ClockDivision; // 设置时钟分频
    }

    TIMx->CTLR1 = tmpcr1; // 写入CTLR1寄存器
    TIMx->ATRLR = TIM_TimeBaseInitStruct->TIM_Period; // 设置自动重装载值
    TIMx->PSC = TIM_TimeBaseInitStruct->TIM_Prescaler; // 设置预分频器

    // 对于高级定时器，设置重复计数器
    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        TIMx->RPTCR = TIM_TimeBaseInitStruct->TIM_RepetitionCounter;
    }

    // 立即重载预分频器
    TIMx->SWEVGR = TIM_PSCReloadMode_Immediate;
}

/*********************************************************************
 * @函数名 TIM_OC1Init
 *
 * @描述   根据TIM_OCInitStruct中指定的参数初始化TIMx通道1。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCInitStruct - 指向TIM_OCInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_OC1Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    // 禁用通道1输出
    TIMx->CCER &= (uint16_t)(~(uint16_t)TIM_CC1E);
    tmpccer = TIMx->CCER; // 备份CCER寄存器
    tmpcr2 = TIMx->CTLR2; // 备份CTLR2寄存器
    tmpccmrx = TIMx->CHCTLR1; // 备份CHCTLR1寄存器

    // 清零通道1的输出比较模式和输入捕获选择位
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC1M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC1S));
    tmpccmrx |= TIM_OCInitStruct->TIM_OCMode; // 设置输出比较模式

    // 配置通道1输出极性
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC1P));
    tmpccer |= TIM_OCInitStruct->TIM_OCPolarity;

    // 配置通道1输出使能
    tmpccer |= TIM_OCInitStruct->TIM_OutputState;

    // 如果是高级定时器，还需要配置互补通道和空闲状态
    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        // 配置互补通道输出极性
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC1NP));
        tmpccer |= TIM_OCInitStruct->TIM_OCNPolarity;

        // 配置互补通道输出使能
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC1NE));
        tmpccer |= TIM_OCInitStruct->TIM_OutputNState;

        // 清零输出空闲状态位
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS1));
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS1N));

        // 设置输出空闲状态
        tmpcr2 |= TIM_OCInitStruct->TIM_OCIdleState;
        tmpcr2 |= TIM_OCInitStruct->TIM_OCNIdleState;
    }

    // 将配置写入寄存器
    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR1 = tmpccmrx;
    TIMx->CH1CVR = TIM_OCInitStruct->TIM_Pulse; // 设置比较值
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC2Init
 *
 * @描述   根据TIM_OCInitStruct中指定的参数初始化TIMx通道2。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCInitStruct - 指向TIM_OCInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_OC2Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    // 禁用通道2输出
    TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CC2E));
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CTLR2;
    tmpccmrx = TIMx->CHCTLR1;

    // 清零通道2的输出比较模式和输入捕获选择位
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC2M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC2S));
    tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8); // 设置输出比较模式（通道2在寄存器的高8位）

    // 配置通道2输出极性
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC2P));
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 4); // 通道2极性位在CCER寄存器的位4

    // 配置通道2输出使能
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 4);

    // 如果是高级定时器，还需要配置互补通道和空闲状态
    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        // 配置互补通道输出极性
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC2NP));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 4);

        // 配置互补通道输出使能
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC2NE));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 4);

        // 清零输出空闲状态位
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS2));
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS2N));

        // 设置输出空闲状态
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 2); // 通道2空闲状态在位2
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 2);
    }

    // 将配置写入寄存器
    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR1 = tmpccmrx;
    TIMx->CH2CVR = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC3Init
 *
 * @描述   根据TIM_OCInitStruct中指定的参数初始化TIMx通道3。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCInitStruct - 指向TIM_OCInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_OC3Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    // 禁用通道3输出
    TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CC3E));
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CTLR2;
    tmpccmrx = TIMx->CHCTLR2; // 通道3和4的配置在CHCTLR2寄存器

    // 清零通道3的输出比较模式和输入捕获选择位
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC3M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC3S));
    tmpccmrx |= TIM_OCInitStruct->TIM_OCMode; // 设置输出比较模式

    // 配置通道3输出极性
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC3P));
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 8); // 通道3极性位在CCER寄存器的位8

    // 配置通道3输出使能
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 8);

    // 如果是高级定时器，还需要配置互补通道和空闲状态
    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        // 配置互补通道输出极性
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC3NP));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCNPolarity << 8);

        // 配置互补通道输出使能
        tmpccer &= (uint16_t)(~((uint16_t)TIM_CC3NE));
        tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputNState << 8);

        // 清零输出空闲状态位
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS3));
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS3N));

        // 设置输出空闲状态
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 4); // 通道3空闲状态在位4
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCNIdleState << 4);
    }

    // 将配置写入寄存器
    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR2 = tmpccmrx;
    TIMx->CH3CVR = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC4Init
 *
 * @描述   根据TIM_OCInitStruct中指定的参数初始化TIMx通道4。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCInitStruct - 指向TIM_OCInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_OC4Init(TIM_TypeDef *TIMx, TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    uint16_t tmpccmrx = 0, tmpccer = 0, tmpcr2 = 0;

    // 禁用通道4输出
    TIMx->CCER &= (uint16_t)(~((uint16_t)TIM_CC4E));
    tmpccer = TIMx->CCER;
    tmpcr2 = TIMx->CTLR2;
    tmpccmrx = TIMx->CHCTLR2;

    // 清零通道4的输出比较模式和输入捕获选择位
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_OC4M));
    tmpccmrx &= (uint16_t)(~((uint16_t)TIM_CC4S));
    tmpccmrx |= (uint16_t)(TIM_OCInitStruct->TIM_OCMode << 8); // 设置输出比较模式（通道4在寄存器的高8位）

    // 配置通道4输出极性
    tmpccer &= (uint16_t)(~((uint16_t)TIM_CC4P));
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OCPolarity << 12); // 通道4极性位在CCER寄存器的位12

    // 配置通道4输出使能
    tmpccer |= (uint16_t)(TIM_OCInitStruct->TIM_OutputState << 12);

    // 如果是高级定时器，还需要配置空闲状态（通道4没有互补通道）
    if((TIMx == TIM1) || (TIMx == TIM8) || (TIMx == TIM9) || (TIMx == TIM10))
    {
        // 清零输出空闲状态位
        tmpcr2 &= (uint16_t)(~((uint16_t)TIM_OIS4));

        // 设置输出空闲状态
        tmpcr2 |= (uint16_t)(TIM_OCInitStruct->TIM_OCIdleState << 6); // 通道4空闲状态在位6
    }

    // 将配置写入寄存器
    TIMx->CTLR2 = tmpcr2;
    TIMx->CHCTLR2 = tmpccmrx;
    TIMx->CH4CVR = TIM_OCInitStruct->TIM_Pulse;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_ICInit
 *
 * @描述   根据TIM_ICInitStruct中指定的参数初始化TIM外设（输入捕获模式）。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICInitStruct - 指向TIM_ICInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_ICInit(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct)
{
    // 根据选择的通道调用相应的配置函数
    if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
    {
        TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler); // 设置输入捕获预分频器
    }
    else if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_2)
    {
        TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
    else if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_3)
    {
        TI3_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC3Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
    else
    {
        TI4_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity,
                   TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC4Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
}

/*********************************************************************
 * @函数名 TIM_PWMIConfig
 *
 * @描述   根据TIM_ICInitStruct中指定的参数配置TIM外设以测量外部PWM信号。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICInitStruct - 指向TIM_ICInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_PWMIConfig(TIM_TypeDef *TIMx, TIM_ICInitTypeDef *TIM_ICInitStruct)
{
    uint16_t icoppositepolarity = TIM_ICPolarity_Rising; // 相反极性
    uint16_t icoppositeselection = TIM_ICSelection_DirectTI; // 相反输入选择

    // 根据输入的极性设置相反的极性
    if(TIM_ICInitStruct->TIM_ICPolarity == TIM_ICPolarity_Rising)
    {
        icoppositepolarity = TIM_ICPolarity_Falling;
    }
    else
    {
        icoppositepolarity = TIM_ICPolarity_Rising;
    }

    // 根据输入的输入选择设置相反的输入选择
    if(TIM_ICInitStruct->TIM_ICSelection == TIM_ICSelection_DirectTI)
    {
        icoppositeselection = TIM_ICSelection_IndirectTI;
    }
    else
    {
        icoppositeselection = TIM_ICSelection_DirectTI;
    }

    // 配置两个通道，一个用于测量PWM的高电平，另一个用于测量低电平
    if(TIM_ICInitStruct->TIM_Channel == TIM_Channel_1)
    {
        // 配置通道1
        TI1_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity, TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);

        // 配置通道2为相反设置
        TI2_Config(TIMx, icoppositepolarity, icoppositeselection, TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
    else // 如果选择的是通道2
    {
        TI2_Config(TIMx, TIM_ICInitStruct->TIM_ICPolarity, TIM_ICInitStruct->TIM_ICSelection,
                   TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC2Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);

        TI1_Config(TIMx, icoppositepolarity, icoppositeselection, TIM_ICInitStruct->TIM_ICFilter);
        TIM_SetIC1Prescaler(TIMx, TIM_ICInitStruct->TIM_ICPrescaler);
    }
}

/*********************************************************************
 * @函数名 TIM_BDTRConfig
 *
 * @描述   配置断路功能、死区时间、锁定级别、OSSI、OSSR状态和AOE（自动输出使能）。
 *
 * @参数   TIMx - x可以是（1、8、9、10），用于选择TIM外设（高级定时器）。
 *          TIM_BDTRInitStruct - 指向TIM_BDTRInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_BDTRConfig(TIM_TypeDef *TIMx, TIM_BDTRInitTypeDef *TIM_BDTRInitStruct)
{
    // 将各配置项组合写入BDTR寄存器
    TIMx->BDTR = (uint32_t)TIM_BDTRInitStruct->TIM_OSSRState | TIM_BDTRInitStruct->TIM_OSSIState |
                 TIM_BDTRInitStruct->TIM_LOCKLevel | TIM_BDTRInitStruct->TIM_DeadTime |
                 TIM_BDTRInitStruct->TIM_Break | TIM_BDTRInitStruct->TIM_BreakPolarity |
                 TIM_BDTRInitStruct->TIM_AutomaticOutput;
}

/*********************************************************************
 * @函数名 TIM_TimeBaseStructInit
 *
 * @描述   将TIM_TimeBaseInitStruct的每个成员填充为其默认值。
 *
 * @参数   TIM_TimeBaseInitStruct - 指向TIM_TimeBaseInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_TimeBaseStructInit(TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct)
{
    TIM_TimeBaseInitStruct->TIM_Period = 0xFFFF; // 自动重装载值默认为最大值
    TIM_TimeBaseInitStruct->TIM_Prescaler = 0x0000; // 预分频器默认为0
    TIM_TimeBaseInitStruct->TIM_ClockDivision = TIM_CKD_DIV1; // 时钟分频默认为1分频
    TIM_TimeBaseInitStruct->TIM_CounterMode = TIM_CounterMode_Up; // 计数模式默认为向上计数
    TIM_TimeBaseInitStruct->TIM_RepetitionCounter = 0x0000; // 重复计数器默认为0
}

/*********************************************************************
 * @函数名 TIM_OCStructInit
 *
 * @描述   将TIM_OCInitStruct的每个成员填充为其默认值。
 *
 * @参数   TIM_OCInitStruct - 指向TIM_OCInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_OCStructInit(TIM_OCInitTypeDef *TIM_OCInitStruct)
{
    TIM_OCInitStruct->TIM_OCMode = TIM_OCMode_Timing; // 输出比较模式默认为定时模式
    TIM_OCInitStruct->TIM_OutputState = TIM_OutputState_Disable; // 输出使能默认为禁用
    TIM_OCInitStruct->TIM_OutputNState = TIM_OutputNState_Disable; // 互补输出使能默认为禁用
    TIM_OCInitStruct->TIM_Pulse = 0x0000; // 比较值默认为0
    TIM_OCInitStruct->TIM_OCPolarity = TIM_OCPolarity_High; // 输出极性默认为高电平有效
    TIM_OCInitStruct->TIM_OCNPolarity = TIM_OCNPolarity_High; // 互补输出极性默认为高电平有效
    TIM_OCInitStruct->TIM_OCIdleState = TIM_OCIdleState_Reset; // 空闲状态默认为复位状态
    TIM_OCInitStruct->TIM_OCNIdleState = TIM_OCNIdleState_Reset; // 互补通道空闲状态默认为复位状态
}

/*********************************************************************
 * @函数名 TIM_ICStructInit
 *
 * @描述   将TIM_ICInitStruct的每个成员填充为其默认值。
 *
 * @参数   TIM_ICInitStruct - 指向TIM_ICInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_ICStructInit(TIM_ICInitTypeDef *TIM_ICInitStruct)
{
    TIM_ICInitStruct->TIM_Channel = TIM_Channel_1; // 通道默认为通道1
    TIM_ICInitStruct->TIM_ICPolarity = TIM_ICPolarity_Rising; // 输入极性默认为上升沿
    TIM_ICInitStruct->TIM_ICSelection = TIM_ICSelection_DirectTI; // 输入选择默认为直接连接到TIx
    TIM_ICInitStruct->TIM_ICPrescaler = TIM_ICPSC_DIV1; // 输入捕获预分频默认为1分频
    TIM_ICInitStruct->TIM_ICFilter = 0x00; // 输入滤波器默认为0（无滤波）
}

/*********************************************************************
 * @函数名 TIM_BDTRStructInit
 *
 * @描述   将TIM_BDTRInitStruct的每个成员填充为其默认值。
 *
 * @参数   TIM_BDTRInitStruct - 指向TIM_BDTRInitTypeDef结构体的指针。
 *
 * @返回值 无
 */
void TIM_BDTRStructInit(TIM_BDTRInitTypeDef *TIM_BDTRInitStruct)
{
    TIM_BDTRInitStruct->TIM_OSSRState = TIM_OSSRState_Disable; // OSSR状态默认为禁用
    TIM_BDTRInitStruct->TIM_OSSIState = TIM_OSSIState_Disable; // OSSI状态默认为禁用
    TIM_BDTRInitStruct->TIM_LOCKLevel = TIM_LOCKLevel_OFF; // 锁定级别默认为关
    TIM_BDTRInitStruct->TIM_DeadTime = 0x00; // 死区时间默认为0
    TIM_BDTRInitStruct->TIM_Break = TIM_Break_Disable; // 断路功能默认为禁用
    TIM_BDTRInitStruct->TIM_BreakPolarity = TIM_BreakPolarity_Low; // 断路极性默认为低电平
    TIM_BDTRInitStruct->TIM_AutomaticOutput = TIM_AutomaticOutput_Disable; // 自动输出使能默认为禁用
}

/*********************************************************************
 * @函数名 TIM_Cmd
 *
 * @描述   使能或禁用指定的TIM外设。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR1 |= TIM_CEN; // 使能计数器
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t)(~((uint16_t)TIM_CEN)); // 禁用计数器
    }
}

        /*********************************************************************
 * @函数名 TIM_CtrlPWMOutputs
 *
 * @描述   使能或禁用TIM外设主输出（高级定时器的PWM输出）。
 *
 * @参数   TIMx - x可以是1/8/9/10，用于选择TIM外设（高级定时器）。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_CtrlPWMOutputs(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->BDTR |= TIM_MOE; // 使能主输出
    }
    else
    {
        TIMx->BDTR &= (uint16_t)(~((uint16_t)TIM_MOE)); // 禁用主输出
    }
}

/*********************************************************************
 * @函数名 TIM_ITConfig
 *
 * @描述   使能或禁用指定的TIM中断。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_IT - 指定要启用或禁用的TIM中断源。
 *            TIM_IT_Update - TIM更新中断源。
 *            TIM_IT_CC1 - TIM捕获比较1中断源。
 *            TIM_IT_CC2 - TIM捕获比较2中断源。
 *            TIM_IT_CC3 - TIM捕获比较3中断源。
 *            TIM_IT_CC4 - TIM捕获比较4中断源。
 *            TIM_IT_COM - TIM换向中断源。
 *            TIM_IT_Trigger - TIM触发中断源。
 *            TIM_IT_Break - TIM断路中断源。
 *            TIM6/TIM7只有TIM_IT_Update。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_ITConfig(TIM_TypeDef *TIMx, uint16_t TIM_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->DMAINTENR |= TIM_IT; // 使能指定的中断
    }
    else
    {
        TIMx->DMAINTENR &= (uint16_t)~TIM_IT; // 禁用指定的中断
    }
}

/*********************************************************************
 * @函数名 TIM_GenerateEvent
 *
 * @描述   配置TIMx事件由软件生成。
 *
 * @参数   TIMx - x可以是1到10，用于生成事件。
 *          TIM_EventSource - 事件源。
 *            TIM_EventSource_Update - 更新事件。
 *            TIM_EventSource_CC1 - TIM捕获比较1事件。
 *            TIM_EventSource_CC2 - TIM捕获比较2事件。
 *            TIM_EventSource_CC3 - TIM捕获比较3事件。
 *            TIM_EventSource_CC4 - TIM捕获比较4事件。
 *            TIM_EventSource_COM - TIM换向事件。
 *            TIM_EventSource_Trigger - TIM触发事件。
 *            TIM_EventSource_Break - TIM断路事件。
 *            TIM6/TIM7只有TIM_EventSource_Update。
 *
 * @返回值 无
 */
void TIM_GenerateEvent(TIM_TypeDef *TIMx, uint16_t TIM_EventSource)
{
    TIMx->SWEVGR = TIM_EventSource; // 通过软件生成事件
}

/*********************************************************************
 * @函数名 TIM_DMAConfig
 *
 * @描述   配置TIMx的DMA接口。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_DMABase - DMA基地址。
 *            TIM_DMABase_CR - 控制寄存器1。
 *            TIM_DMABase_CR2 - 控制寄存器2。
 *            TIM_DMABase_SMCR - 从模式控制寄存器。
 *            TIM_DMABase_DIER - DMA/中断使能寄存器。
 *            TIM1_DMABase_SR - 状态寄存器。
 *            TIM_DMABase_EGR - 事件生成寄存器。
 *            TIM_DMABase_CCMR1 - 捕获/比较模式寄存器1。
 *            TIM_DMABase_CCMR2 - 捕获/比较模式寄存器2。
 *            TIM_DMABase_CCER - 捕获/比较使能寄存器。
 *            TIM_DMABase_CNT - 计数器寄存器。
 *            TIM_DMABase_PSC - 预分频器寄存器。
 *            TIM_DMABase_CCR1 - 捕获/比较寄存器1。
 *            TIM_DMABase_CCR2 - 捕获/比较寄存器2。
 *            TIM_DMABase_CCR3 - 捕获/比较寄存器3。
 *            TIM_DMABase_CCR4 - 捕获/比较寄存器4。
 *            TIM_DMABase_BDTR - 断路和死区寄存器。
 *            TIM_DMABase_DCR - DMA控制寄存器。
 *          TIM_DMABurstLength - DMA突发传输长度。
 *            TIM_DMABurstLength_1Transfer - 1次传输。
 *            TIM_DMABurstLength_18Transfers - 18次传输。
 *
 * @返回值 无
 */
void TIM_DMAConfig(TIM_TypeDef *TIMx, uint16_t TIM_DMABase, uint16_t TIM_DMABurstLength)
{
    TIMx->DMACFGR = TIM_DMABase | TIM_DMABurstLength; // 配置DMA寄存器
}

/*********************************************************************
 * @函数名 TIM_DMACmd
 *
 * @描述   使能或禁用TIMx的DMA请求。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_DMASource - 指定DMA请求源。
 *            TIM_DMA_Update - TIM更新DMA源。
 *            TIM_DMA_CC1 - TIM捕获比较1 DMA源。
 *            TIM_DMA_CC2 - TIM捕获比较2 DMA源。
 *            TIM_DMA_CC3 - TIM捕获比较3 DMA源。
 *            TIM_DMA_CC4 - TIM捕获比较4 DMA源。
 *            TIM_DMA_COM - TIM换向DMA源。
 *            TIM_DMA_Trigger - TIM触发DMA源。
 *            TIM6/TIM7只有TIM_DMA_Update。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_DMACmd(TIM_TypeDef *TIMx, uint16_t TIM_DMASource, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->DMAINTENR |= TIM_DMASource; // 使能DMA请求
    }
    else
    {
        TIMx->DMAINTENR &= (uint16_t)~TIM_DMASource; // 禁用DMA请求
    }
}

/*********************************************************************
 * @函数名 TIM_InternalClockConfig
 *
 * @描述   配置TIMx内部时钟。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *
 * @返回值 无
 */
void TIM_InternalClockConfig(TIM_TypeDef *TIMx)
{
    TIMx->SMCFGR &= (uint16_t)(~((uint16_t)TIM_SMS)); // 清零从模式选择位，选择内部时钟
}

/*********************************************************************
 * @函数名 TIM_ITRxExternalClockConfig
 *
 * @描述   将TIMx内部触发器配置为外部时钟。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_InputTriggerSource - 触发源。
 *            TIM_TS_ITR0 - 内部触发器0。
 *            TIM_TS_ITR1 - 内部触发器1。
 *            TIM_TS_ITR2 - 内部触发器2。
 *            TIM_TS_ITR3 - 内部触发器3。
 *
 * @返回值 无
 */
void TIM_ITRxExternalClockConfig(TIM_TypeDef *TIMx, uint16_t TIM_InputTriggerSource)
{
    TIM_SelectInputTrigger(TIMx, TIM_InputTriggerSource); // 选择输入触发源
    TIMx->SMCFGR |= TIM_SlaveMode_External1; // 设置为外部时钟模式1
}

/*********************************************************************
 * @函数名 TIM_TIxExternalClockConfig
 *
 * @描述   将TIMx触发器配置为外部时钟。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_TIxExternalCLKSource - 触发源。
 *            TIM_TIxExternalCLK1Source_TI1ED - TI1边沿检测器。
 *            TIM_TIxExternalCLK1Source_TI1 - 滤波后的定时器输入1。
 *            TIM_TIxExternalCLK1Source_TI2 - 滤波后的定时器输入2。
 *          TIM_ICPolarity - 指定TIx极性。
 *             TIM_ICPolarity_Rising - 上升沿。
 *             TIM_ICPolarity_Falling - 下降沿。
 *          ICFilter - 指定滤波器值。
 *             此参数必须是0x0到0xF之间的值。
 *
 * @返回值 无
 */
void TIM_TIxExternalClockConfig(TIM_TypeDef *TIMx, uint16_t TIM_TIxExternalCLKSource,
                                uint16_t TIM_ICPolarity, uint16_t ICFilter)
{
    // 根据触发源配置相应的输入通道
    if(TIM_TIxExternalCLKSource == TIM_TIxExternalCLK1Source_TI2)
    {
        TI2_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);
    }
    else
    {
        TI1_Config(TIMx, TIM_ICPolarity, TIM_ICSelection_DirectTI, ICFilter);
    }

    TIM_SelectInputTrigger(TIMx, TIM_TIxExternalCLKSource); // 选择输入触发源
    TIMx->SMCFGR |= TIM_SlaveMode_External1; // 设置为外部时钟模式1
}

/*********************************************************************
 * @函数名 TIM_ETRClockMode1Config
 *
 * @描述   配置外部时钟模式1。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ExtTRGPrescaler - 外部触发器预分频器。
 *            TIM_ExtTRGPSC_OFF - ETRP预分频器关闭。
 *            TIM_ExtTRGPSC_DIV2 - ETRP频率除以2。
 *            TIM_ExtTRGPSC_DIV4 - ETRP频率除以4。
 *            TIM_ExtTRGPSC_DIV8 - ETRP频率除以8。
 *          TIM_ExtTRGPolarity - 外部触发器极性。
 *            TIM_ExtTRGPolarity_Inverted - 低电平有效或下降沿有效。
 *            TIM_ExtTRGPolarity_NonInverted - 高电平有效或上升沿有效。
 *          ExtTRGFilter - 外部触发器滤波器。
 *             此参数必须是0x0到0xF之间的值。
 *
 * @返回值 无
 */
void TIM_ETRClockMode1Config(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                             uint16_t ExtTRGFilter)
{
    uint16_t tmpsmcr = 0;

    TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter); // 配置ETR
    tmpsmcr = TIMx->SMCFGR;
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMS)); // 清零从模式选择位
    tmpsmcr |= TIM_SlaveMode_External1; // 设置为外部时钟模式1
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_TS)); // 清零触发选择位
    tmpsmcr |= TIM_TS_ETRF; // 选择ETR作为触发源
    TIMx->SMCFGR = tmpsmcr;
}

/*********************************************************************
 * @函数名 TIM_ETRClockMode2Config
 *
 * @描述   配置外部时钟模式2。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ExtTRGPrescaler - 外部触发器预分频器。
 *            TIM_ExtTRGPSC_OFF - ETRP预分频器关闭。
 *            TIM_ExtTRGPSC_DIV2 - ETRP频率除以2。
 *            TIM_ExtTRGPSC_DIV4 - ETRP频率除以4。
 *            TIM_ExtTRGPSC_DIV8 - ETRP频率除以8。
 *          TIM_ExtTRGPolarity - 外部触发器极性。
 *            TIM_ExtTRGPolarity_Inverted - 低电平有效或下降沿有效。
 *            TIM_ExtTRGPolarity_NonInverted - 高电平有效或上升沿有效。
 *          ExtTRGFilter - 外部触发器滤波器。
 *             此参数必须是0x0到0xF之间的值。
 *
 * @返回值 无
 */
void TIM_ETRClockMode2Config(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler,
                             uint16_t TIM_ExtTRGPolarity, uint16_t ExtTRGFilter)
{
    TIM_ETRConfig(TIMx, TIM_ExtTRGPrescaler, TIM_ExtTRGPolarity, ExtTRGFilter); // 配置ETR
    TIMx->SMCFGR |= TIM_ECE; // 使能外部时钟模式2
}

/*********************************************************************
 * @函数名 TIM_ETRConfig
 *
 * @描述   配置TIMx外部触发器（ETR）。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ExtTRGPrescaler - 外部触发器预分频器。
 *            TIM_ExtTRGPSC_OFF - ETRP预分频器关闭。
 *            TIM_ExtTRGPSC_DIV2 - ETRP频率除以2。
 *            TIM_ExtTRGPSC_DIV4 - ETRP频率除以4。
 *            TIM_ExtTRGPSC_DIV8 - ETRP频率除以8。
 *          TIM_ExtTRGPolarity - 外部触发器极性。
 *            TIM_ExtTRGPolarity_Inverted - 低电平有效或下降沿有效。
 *            TIM_ExtTRGPolarity_NonInverted - 高电平有效或上升沿有效。
 *          ExtTRGFilter - 外部触发器滤波器。
 *             此参数必须是0x0到0xF之间的值。
 *
 * @返回值 无
 */
void TIM_ETRConfig(TIM_TypeDef *TIMx, uint16_t TIM_ExtTRGPrescaler, uint16_t TIM_ExtTRGPolarity,
                   uint16_t ExtTRGFilter)
{
    uint16_t tmpsmcr = 0;

    tmpsmcr = TIMx->SMCFGR;
    tmpsmcr &= SMCFGR_ETR_Mask; // 保留ETR相关位以外的其他位
    tmpsmcr |= (uint16_t)(TIM_ExtTRGPrescaler | (uint16_t)(TIM_ExtTRGPolarity | (uint16_t)(ExtTRGFilter << (uint16_t)8)));
    TIMx->SMCFGR = tmpsmcr;
}

/*********************************************************************
 * @函数名 TIM_PrescalerConfig
 *
 * @描述   配置TIMx预分频器。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          Prescaler - 指定预分频器寄存器值。
 *          TIM_PSCReloadMode - 指定TIM预分频器重载模式。
 *            TIM_PSCReloadMode_Update - 预分频器在更新事件时加载。
 *            TIM_PSCReloadMode_Immediate - 预分频器立即加载。
 *
 * @返回值 无
 */
void TIM_PrescalerConfig(TIM_TypeDef *TIMx, uint16_t Prescaler, uint16_t TIM_PSCReloadMode)
{
    TIMx->PSC = Prescaler; // 设置预分频器值
    TIMx->SWEVGR = TIM_PSCReloadMode; // 设置预分频器重载模式
}

/*********************************************************************
 * @函数名 TIM_CounterModeConfig
 *
 * @描述   指定要使用的TIMx计数器模式。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_CounterMode - 指定要使用的计数器模式。
 *            TIM_CounterMode_Up - TIM向上计数模式。
 *            TIM_CounterMode_Down - TIM向下计数模式。
 *            TIM_CounterMode_CenterAligned1 - TIM中央对齐模式1。
 *            TIM_CounterMode_CenterAligned2 - TIM中央对齐模式2。
 *            TIM_CounterMode_CenterAligned3 - TIM中央对齐模式3。
 *
 * @返回值 无
 */
void TIM_CounterModeConfig(TIM_TypeDef *TIMx, uint16_t TIM_CounterMode)
{
    uint16_t tmpcr1 = 0;

    tmpcr1 = TIMx->CTLR1;
    tmpcr1 &= (uint16_t)(~((uint16_t)(TIM_DIR | TIM_CMS))); // 清零方向和中央对齐模式位
    tmpcr1 |= TIM_CounterMode; // 设置计数器模式
    TIMx->CTLR1 = tmpcr1;
}

/*********************************************************************
 * @函数名 TIM_SelectInputTrigger
 *
 * @描述   选择输入触发源。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_InputTriggerSource - 输入触发源。
 *            TIM_TS_ITR0 - 内部触发器0。
 *            TIM_TS_ITR1 - 内部触发器1。
 *            TIM_TS_ITR2 - 内部触发器2。
 *            TIM_TS_ITR3 - 内部触发器3。
 *            TIM_TS_TI1F_ED - TI1边沿检测器。
 *            TIM_TS_TI1FP1 - 滤波后的定时器输入1。
 *            TIM_TS_TI2FP2 - 滤波后的定时器输入2。
 *            TIM_TS_ETRF - 外部触发器输入。
 *
 * @返回值 无
 */
void TIM_SelectInputTrigger(TIM_TypeDef *TIMx, uint16_t TIM_InputTriggerSource)
{
    uint16_t tmpsmcr = 0;

    tmpsmcr = TIMx->SMCFGR;
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_TS)); // 清零触发选择位
    tmpsmcr |= TIM_InputTriggerSource; // 设置输入触发源
    TIMx->SMCFGR = tmpsmcr;
}

/*********************************************************************
 * @函数名 TIM_EncoderInterfaceConfig
 *
 * @描述   配置TIMx编码器接口。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_EncoderMode - 指定TIMx编码器模式。
 *            TIM_EncoderMode_TI1 - 计数器根据TI2FP2电平在TI1FP1边沿计数。
 *            TIM_EncoderMode_TI2 - 计数器根据TI1FP1电平在TI2FP2边沿计数。
 *            TIM_EncoderMode_TI12 - 计数器在TI1FP1和TI2FP2两个边沿计数。
 *          TIM_IC1Polarity - 指定IC1极性。
 *            TIM_ICPolarity_Falling - IC下降沿。
 *            TTIM_ICPolarity_Rising - IC上升沿。
 *          TIM_IC2Polarity - 指定IC2极性。
 *            TIM_ICPolarity_Falling - IC下降沿。
 *            TIM_ICPolarity_Rising - IC上升沿。
 *
 * @返回值 无
 */
void TIM_EncoderInterfaceConfig(TIM_TypeDef *TIMx, uint16_t TIM_EncoderMode,
                                uint16_t TIM_IC1Polarity, uint16_t TIM_IC2Polarity)
{
    uint16_t tmpsmcr = 0;
    uint16_t tmpccmr1 = 0;
    uint16_t tmpccer = 0;

    tmpsmcr = TIMx->SMCFGR;
    tmpccmr1 = TIMx->CHCTLR1;
    tmpccer = TIMx->CCER;
    
    tmpsmcr &= (uint16_t)(~((uint16_t)TIM_SMS)); // 清零从模式选择位
    tmpsmcr |= TIM_EncoderMode; // 设置编码器模式
    
    tmpccmr1 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC1S)) & (uint16_t)(~((uint16_t)TIM_CC2S))); // 清零CC1S和CC2S位
    tmpccmr1 |= TIM_CC1S_0 | TIM_CC2S_0; // 设置通道1和2为输入模式
    
    tmpccer &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC1P)) & ((uint16_t) ~((uint16_t)TIM_CC2P))); // 清零CC1P和CC2P位
    tmpccer |= (uint16_t)(TIM_IC1Polarity | (uint16_t)(TIM_IC2Polarity << (uint16_t)4)); // 设置通道1和2的极性
    
    TIMx->SMCFGR = tmpsmcr;
    TIMx->CHCTLR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_ForcedOC1Config
 *
 * @描述   强制TIMx输出1波形为有效或无效电平。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ForcedAction - 指定要设置到输出波形的强制动作。
 *            TIM_ForcedAction_Active - 强制OC1REF为有效电平。
 *            TIM_ForcedAction_InActive - 强制OC1REF为无效电平。
 *
 * @返回值 无
 */
void TIM_ForcedOC1Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1M); // 清零输出比较模式位
    tmpccmr1 |= TIM_ForcedAction; // 设置强制输出模式
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_ForcedOC2Config
 *
 * @描述   强制TIMx输出2波形为有效或无效电平。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ForcedAction - 指定要设置到输出波形的强制动作。
 *            TIM_ForcedAction_Active - 强制OC2REF为有效电平。
 *            TIM_ForcedAction_InActive - 强制OC2REF为无效电平。
 *
 * @返回值 无
 */
void TIM_ForcedOC2Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2M); // 清零输出比较模式位
    tmpccmr1 |= (uint16_t)(TIM_ForcedAction << 8); // 设置强制输出模式（通道2在寄存器的高8位）
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_ForcedOC3Config
 *
 * @描述   强制TIMx输出3波形为有效或无效电平。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ForcedAction - 指定要设置到输出波形的强制动作。
 *            TIM_ForcedAction_Active - 强制OC3REF为有效电平。
 *            TIM_ForcedAction_InActive - 强制OC3REF为无效电平。
 *
 * @返回值 无
 */
void TIM_ForcedOC3Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3M); // 清零输出比较模式位
    tmpccmr2 |= TIM_ForcedAction; // 设置强制输出模式
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_ForcedOC4Config
 *
 * @描述   强制TIMx输出4波形为有效或无效电平。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ForcedAction - 指定要设置到输出波形的强制动作。
 *            TIM_ForcedAction_Active - 强制OC4REF为有效电平。
 *            TIM_ForcedAction_InActive - 强制OC4REF为无效电平。
 *
 * @返回值 无
 */
void TIM_ForcedOC4Config(TIM_TypeDef *TIMx, uint16_t TIM_ForcedAction)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4M); // 清零输出比较模式位
    tmpccmr2 |= (uint16_t)(TIM_ForcedAction << 8); // 设置强制输出模式（通道4在寄存器的高8位）
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_ARRPreloadConfig
 *
 * @描述   使能或禁用TIMx外设ARR上的预加载寄存器。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_ARRPreloadConfig(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR1 |= TIM_ARPE; // 使能ARR预加载
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_ARPE); // 禁用ARR预加载
    }
}

/*********************************************************************
 * @函数名 TIM_SelectCOM
 *
 * @描述   选择TIM外设换向事件。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_SelectCOM(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_CCUS; // 使能换向事件
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_CCUS); // 禁用换向事件
    }
}

/*********************************************************************
 * @函数名 TIM_SelectCCDMA
 *
 * @描述   选择TIMx外设捕获比较DMA源。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_SelectCCDMA(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_CCDS; // 使能捕获比较DMA源选择
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_CCDS); // 禁用捕获比较DMA源选择
    }
}

/*********************************************************************
 * @函数名 TIM_CCPreloadControl
 *
 * @描述   设置或复位TIM外设捕获比较预加载控制位。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_CCPreloadControl(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_CCPC; // 使能捕获比较预加载控制
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_CCPC); // 禁用捕获比较预加载控制
    }
}

/*********************************************************************
 * @函数名 TIM_OC1PreloadConfig
 *
 * @描述   使能或禁用TIMx外设CCR1上的预加载寄存器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPreload - TIMx外设预加载寄存器的新状态。
 *            TIM_OCPreload_Enable - 使能。
 *            TIM_OCPreload_Disable - 禁用。
 *
 * @返回值 无
 */
void TIM_OC1PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1PE); // 清零通道1预加载使能位
    tmpccmr1 |= TIM_OCPreload; // 设置预加载使能状态
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_OC2PreloadConfig
 *
 * @描述   使能或禁用TIMx外设CCR2上的预加载寄存器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPreload - TIMx外设预加载寄存器的新状态。
 *            TIM_OCPreload_Enable - 使能。
 *            TIM_OCPreload_Disable - 禁用。
 *
 * @返回值 无
 */
void TIM_OC2PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2PE); // 清零通道2预加载使能位
    tmpccmr1 |= (uint16_t)(TIM_OCPreload << 8); // 设置预加载使能状态（通道2在寄存器的高8位）
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_OC3PreloadConfig
 *
 * @描述   使能或禁用TIMx外设CCR3上的预加载寄存器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPreload - TIMx外设预加载寄存器的新状态。
 *            TIM_OCPreload_Enable - 使能。
 *            TIM_OCPreload_Disable - 禁用。
 *
 * @返回值 无
 */
void TIM_OC3PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3PE); // 清零通道3预加载使能位
    tmpccmr2 |= TIM_OCPreload; // 设置预加载使能状态
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_OC4PreloadConfig
 *
 * @描述   使能或禁用TIMx外设CCR4上的预加载寄存器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPreload - TIMx外设预加载寄存器的新状态。
 *            TIM_OCPreload_Enable - 使能。
 *            TIM_OCPreload_Disable - 禁用。
 *
 * @返回值 无
 */
void TIM_OC4PreloadConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPreload)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4PE); // 清零通道4预加载使能位
    tmpccmr2 |= (uint16_t)(TIM_OCPreload << 8); // 设置预加载使能状态（通道4在寄存器的高8位）
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_OC1FastConfig
 *
 * @描述   配置TIMx输出比较1快速特性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCFast - 输出比较快速使能位的新状态。
 *            TIM_OCFast_Enable - TIM输出比较快速使能。
 *            TIM_OCFast_Disable - TIM输出比较快速禁用。
 *
 * @返回值 无
 */
void TIM_OC1FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1FE); // 清零通道1快速使能位
    tmpccmr1 |= TIM_OCFast; // 设置快速使能状态
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_OC2FastConfig
 *
 * @描述   配置TIMx输出比较2快速特性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCFast - 输出比较快速使能位的新状态。
 *            TIM_OCFast_Enable - TIM输出比较快速使能。
 *            TIM_OCFast_Disable - TIM输出比较快速禁用。
 *
 * @返回值 无
 */
void TIM_OC2FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2FE); // 清零通道2快速使能位
    tmpccmr1 |= (uint16_t)(TIM_OCFast << 8); // 设置快速使能状态（通道2在寄存器的高8位）
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_OC3FastConfig
 *
 * @描述   配置TIMx输出比较3快速特性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCFast - 输出比较快速使能位的新状态。
 *            TIM_OCFast_Enable - TIM输出比较快速使能。
 *            TIM_OCFast_Disable - TIM输出比较快速禁用。
 *
 * @返回值 无
 */
void TIM_OC3FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3FE); // 清零通道3快速使能位
    tmpccmr2 |= TIM_OCFast; // 设置快速使能状态
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_OC4FastConfig
 *
 * @描述   配置TIMx输出比较4快速特性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCFast - 输出比较快速使能位的新状态。
 *            TIM_OCFast_Enable - TIM输出比较快速使能。
 *            TIM_OCFast_Disable - TIM输出比较快速禁用。
 *
 * @返回值 无
 */
void TIM_OC4FastConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCFast)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4FE); // 清零通道4快速使能位
    tmpccmr2 |= (uint16_t)(TIM_OCFast << 8); // 设置快速使能状态（通道4在寄存器的高8位）
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_ClearOC1Ref
 *
 * @描述   在外部事件上清除或保护OCREF1信号。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCClear - 输出比较清除使能位的新状态。
 *            TIM_OCClear_Enable - TIM输出清除使能。
 *            TIM_OCClear_Disable - TIM输出清除禁用。
 *
 * @返回值 无
 */
void TIM_ClearOC1Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC1CE); // 清零通道1输出清除使能位
    tmpccmr1 |= TIM_OCClear; // 设置输出清除使能状态
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_ClearOC2Ref
 *
 * @描述   在外部事件上清除或保护OCREF2信号。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCClear - 输出比较清除使能位的新状态。
 *            TIM_OCClear_Enable - TIM输出清除使能。
 *            TIM_OCClear_Disable - TIM输出清除禁用。
 *
 * @返回值 无
 */
void TIM_ClearOC2Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr1 = 0;

    tmpccmr1 = TIMx->CHCTLR1;
    tmpccmr1 &= (uint16_t) ~((uint16_t)TIM_OC2CE); // 清零通道2输出清除使能位
    tmpccmr1 |= (uint16_t)(TIM_OCClear << 8); // 设置输出清除使能状态（通道2在寄存器的高8位）
    TIMx->CHCTLR1 = tmpccmr1;
}

/*********************************************************************
 * @函数名 TIM_ClearOC3Ref
 *
 * @描述   在外部事件上清除或保护OCREF3信号。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCClear - 输出比较清除使能位的新状态。
 *            TIM_OCClear_Enable - TIM输出清除使能。
 *            TIM_OCClear_Disable - TIM输出清除禁用。
 *
 * @返回值 无
 */
void TIM_ClearOC3Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC3CE); // 清零通道3输出清除使能位
    tmpccmr2 |= TIM_OCClear; // 设置输出清除使能状态
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_ClearOC4Ref
 *
 * @描述   在外部事件上清除或保护OCREF4信号。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCClear - 输出比较清除使能位的新状态。
 *            TIM_OCClear_Enable - TIM输出清除使能。
 *            TIM_OCClear_Disable - TIM输出清除禁用。
 *
 * @返回值 无
 */
void TIM_ClearOC4Ref(TIM_TypeDef *TIMx, uint16_t TIM_OCClear)
{
    uint16_t tmpccmr2 = 0;

    tmpccmr2 = TIMx->CHCTLR2;
    tmpccmr2 &= (uint16_t) ~((uint16_t)TIM_OC4CE); // 清零通道4输出清除使能位
    tmpccmr2 |= (uint16_t)(TIM_OCClear << 8); // 设置输出清除使能状态（通道4在寄存器的高8位）
    TIMx->CHCTLR2 = tmpccmr2;
}

/*********************************************************************
 * @函数名 TIM_OC1PolarityConfig
 *
 * @描述   配置TIMx通道1极性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPolarity - 指定OC1极性。
 *            TIM_OCPolarity_High - 输出比较高电平有效。
 *            TIM_OCPolarity_Low - 输出比较低电平有效。
 *
 * @返回值 无
 */
void TIM_OC1PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC1P); // 清零通道1极性位
    tmpccer |= TIM_OCPolarity; // 设置通道1极性
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC1NPolarityConfig
 *
 * @描述   配置TIMx通道1互补输出极性。
 *
 * @参数   TIMx - x可以是（1、8、9、10），用于选择TIM外设（高级定时器）。
 *          TIM_OCNPolarity - 指定OC1N极性。
 *            TIM_OCNPolarity_High - 输出比较高电平有效。
 *            TIM_OCNPolarity_Low - 输出比较低电平有效。
 *
 * @返回值 无
 */
void TIM_OC1NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC1NP); // 清零通道1互补输出极性位
    tmpccer |= TIM_OCNPolarity; // 设置通道1互补输出极性
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC2PolarityConfig
 *
 * @描述   配置TIMx通道2极性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPolarity - 指定OC2极性。
 *            TIM_OCPolarity_High - 输出比较高电平有效。
 *            TIM_OCPolarity_Low - 输出比较低电平有效。
 *
 * @返回值 无
 */
void TIM_OC2PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC2P); // 清零通道2极性位
    tmpccer |= (uint16_t)(TIM_OCPolarity << 4); // 设置通道2极性（位4）
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC2NPolarityConfig
 *
 * @描述   配置TIMx通道2互补输出极性。
 *
 * @参数   TIMx - x可以是（1、8、9、10），用于选择TIM外设（高级定时器）。
 *          TIM_OCNPolarity - 指定OC2N极性。
 *            TIM_OCNPolarity_High - 输出比较高电平有效。
 *            TIM_OCNPolarity_Low - 输出比较低电平有效。
 *
 * @返回值 无
 */
void TIM_OC2NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC2NP); // 清零通道2互补输出极性位
    tmpccer |= (uint16_t)(TIM_OCNPolarity << 4); // 设置通道2互补输出极性（位4）
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC3PolarityConfig
 *
 * @描述   配置TIMx通道3极性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPolarity - 指定OC3极性。
 *            TIM_OCPolarity_High - 输出比较高电平有效。
 *            TIM_OCPolarity_Low - 输出比较低电平有效。
 *
 * @返回值 无
 */
void TIM_OC3PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC3P); // 清零通道3极性位
    tmpccer |= (uint16_t)(TIM_OCPolarity << 8); // 设置通道3极性（位8）
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC3NPolarityConfig
 *
 * @描述   配置TIMx通道3互补输出极性。
 *
 * @参数   TIMx - x可以是（1、8、9、10），用于选择TIM外设（高级定时器）。
 *          TIM_OCNPolarity - 指定OC3N极性。
 *            TIM_OCNPolarity_High - 输出比较高电平有效。
 *            TIM_OCNPolarity_Low - 输出比较低电平有效。
 *
 * @返回值 无
 */
void TIM_OC3NPolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCNPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC3NP); // 清零通道3互补输出极性位
    tmpccer |= (uint16_t)(TIM_OCNPolarity << 8); // 设置通道3互补输出极性（位8）
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_OC4PolarityConfig
 *
 * @描述   配置TIMx通道4极性。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_OCPolarity - 指定OC4极性。
 *            TIM_OCPolarity_High - 输出比较高电平有效。
 *            TIM_OCPolarity_Low - 输出比较低电平有效。
 *
 * @返回值 无
 */
void TIM_OC4PolarityConfig(TIM_TypeDef *TIMx, uint16_t TIM_OCPolarity)
{
    uint16_t tmpccer = 0;

    tmpccer = TIMx->CCER;
    tmpccer &= (uint16_t) ~((uint16_t)TIM_CC4P); // 清零通道4极性位
    tmpccer |= (uint16_t)(TIM_OCPolarity << 12); // 设置通道4极性（位12）
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TIM_CCxCmd
 *
 * @描述   使能或禁用TIM捕获比较通道x。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_Channel - 指定TIM通道。
 *            TIM_Channel_1 - TIM通道1。
 *            TIM_Channel_2 - TIM通道2。
 *            TIM_Channel_3 - TIM通道3。
 *            TIM_Channel_4 - TIM通道4。
 *          TIM_CCx - 指定TIM通道CCxE位的新状态。
 *            TIM_CCx_Enable - 使能。
 *            TIM_CCx_Disable - 禁用。
 *
 * @返回值 无
 */
void TIM_CCxCmd(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_CCx)
{
    uint16_t tmp = 0;

    tmp = CCER_CCE_Set << TIM_Channel; // 计算要操作的位掩码
    TIMx->CCER &= (uint16_t)~tmp; // 清零对应的CCxE位
    TIMx->CCER |= (uint16_t)(TIM_CCx << TIM_Channel); // 设置CCxE位的新状态
}

/*********************************************************************
 * @函数名 TIM_CCxNCmd
 *
 * @描述   使能或禁用TIM捕获比较通道xN（互补通道）。
 *
 * @参数   TIMx - x可以是（1、8、9、10），用于选择TIM外设（高级定时器）。
 *          TIM_Channel - 指定TIM通道。
 *            TIM_Channel_1 - TIM通道1。
 *            TIM_Channel_2 - TIM通道2。
 *            TIM_Channel_3 - TIM通道3。
 *          TIM_CCxN - 指定TIM通道CCxNE位的新状态。
 *            TIM_CCxN_Enable - 使能。
 *            TIM_CCxN_Disable - 禁用。
 *
 * @返回值 无
 */
void TIM_CCxNCmd(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_CCxN)
{
    uint16_t tmp = 0;

    tmp = CCER_CCNE_Set << TIM_Channel; // 计算要操作的位掩码
    TIMx->CCER &= (uint16_t)~tmp; // 清零对应的CCxNE位
    TIMx->CCER |= (uint16_t)(TIM_CCxN << TIM_Channel); // 设置CCxNE位的新状态
}

/*********************************************************************
 * @函数名 TIM_SelectOCxM
 *
 * @描述   选择TIM输出比较模式。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_Channel - 指定TIM通道。
 *            TIM_Channel_1 - TIM通道1。
 *            TIM_Channel_2 - TIM通道2。
 *            TIM_Channel_3 - TIM通道3。
 *            TIM_Channel_4 - TIM通道4。
 *          TIM_OCMode - 指定TIM输出比较模式。
 *            TIM_OCMode_Timing - 定时模式。
 *            TIM_OCMode_Active - 激活模式。
 *            TIM_OCMode_Toggle - 翻转模式。
 *            TIM_OCMode_PWM1 - PWM模式1。
 *            TIM_OCMode_PWM2 - PWM模式2。
 *            TIM_ForcedAction_Active - 强制激活模式。
 *            TIM_ForcedAction_InActive - 强制非激活模式。
 *
 * @返回值 无
 */
void TIM_SelectOCxM(TIM_TypeDef *TIMx, uint16_t TIM_Channel, uint16_t TIM_OCMode)
{
    uint32_t tmp = 0;
    uint16_t tmp1 = 0;

    tmp = (uint32_t)TIMx; // 获取TIMx基地址
    tmp += CHCTLR_Offset; // 加上偏移量得到CHCTLR1地址
    tmp1 = CCER_CCE_Set << (uint16_t)TIM_Channel; // 计算CCxE位掩码
    TIMx->CCER &= (uint16_t)~tmp1; // 禁用通道输出

    // 根据通道选择操作对应的寄存器
    if((TIM_Channel == TIM_Channel_1) || (TIM_Channel == TIM_Channel_3))
    {
        tmp += (TIM_Channel >> 1); // 计算正确的寄存器地址
        *(__IO uint32_t *)tmp &= (uint32_t) ~((uint32_t)TIM_OC1M); // 清零输出比较模式位
        *(__IO uint32_t *)tmp |= TIM_OCMode; // 设置输出比较模式
    }
    else
    {
        tmp += (uint16_t)(TIM_Channel - (uint16_t)4) >> (uint16_t)1; // 计算正确的寄存器地址
        *(__IO uint32_t *)tmp &= (uint32_t) ~((uint32_t)TIM_OC2M); // 清零输出比较模式位
        *(__IO uint32_t *)tmp |= (uint16_t)(TIM_OCMode << 8); // 设置输出比较模式（通道2或4在高8位）
    }
}

/*********************************************************************
 * @函数名 TIM_UpdateDisableConfig
 *
 * @描述   使能或禁用TIMx更新事件。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_UpdateDisableConfig(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR1 |= TIM_UDIS; // 使能更新事件禁用
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_UDIS); // 禁用更新事件禁用
    }
}

/*********************************************************************
 * @函数名 TIM_UpdateRequestConfig
 *
 * @描述   配置TIMx更新请求中断源。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_UpdateSource - 指定更新源。
 *            TIM_UpdateSource_Regular - 常规更新源。
 *            TIM_UpdateSource_Global - 全局更新源。
 *
 * @返回值 无
 */
void TIM_UpdateRequestConfig(TIM_TypeDef *TIMx, uint16_t TIM_UpdateSource)
{
    if(TIM_UpdateSource != TIM_UpdateSource_Global)
    {
        TIMx->CTLR1 |= TIM_URS; // 设置仅由软件或硬件触发产生更新中断
    }
    else
    {
        TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_URS); // 设置所有事件都能产生更新中断
    }
}

/*********************************************************************
 * @函数名 TIM_SelectHallSensor
 *
 * @描述   使能或禁用TIMx的霍尔传感器接口。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          NewState - 使能或禁用。
 *
 * @返回值 无
 */
void TIM_SelectHallSensor(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        TIMx->CTLR2 |= TIM_TI1S; // 使能TI1输入连接到三个定时器输入（用于霍尔传感器）
    }
    else
    {
        TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_TI1S); // 禁用TI1输入连接
    }
}

/*********************************************************************
 * @函数名 TIM_SelectOnePulseMode
 *
 * @描述   选择TIMx的单脉冲模式。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_OPMode - 指定要使用的OPM模式。
 *            TIM_OPMode_Single - 单脉冲模式。
 *            TIM_OPMode_Repetitive - 重复脉冲模式。
 *
 * @返回值 无
 */
void TIM_SelectOnePulseMode(TIM_TypeDef *TIMx, uint16_t TIM_OPMode)
{
    TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_OPM); // 清零单脉冲模式位
    TIMx->CTLR1 |= TIM_OPMode; // 设置单脉冲模式
}

/*********************************************************************
 * @函数名 TIM_SelectOutputTrigger
 *
 * @描述   选择TIMx触发输出模式。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_TRGOSource - 指定触发输出源。
 *            TIM_TRGOSource_Reset - TIM_EGR寄存器中的UG位用作触发输出（TRGO）。
 *            TIM_TRGOSource_Enable - 计数器使能CEN用作触发输出（TRGO）。
 *            TIM_TRGOSource_Update - 更新事件被选为触发输出（TRGO）。
 *            TIM_TRGOSource_OC1 - 当CC1IF标志要设置时，触发输出发送正脉冲（TRGO）。
 *            TIM_TRGOSource_OC1Ref - OC1REF信号用作触发输出（TRGO）。
 *            TIM_TRGOSource_OC2Ref - OC2REF信号用作触发输出（TRGO）。
 *            TIM_TRGOSource_OC3Ref - OC3REF信号用作触发输出（TRGO）。
 *            TIM_TRGOSource_OC4Ref - OC4REF信号用作触发输出（TRGO）。
 *            TIM6/TIM7只有TIM_TRGOSource_Reset/TIM_TRGOSource_Enable/TIM_TRGOSource_Update。
 *
 * @返回值 无
 */
void TIM_SelectOutputTrigger(TIM_TypeDef *TIMx, uint16_t TIM_TRGOSource)
{
    TIMx->CTLR2 &= (uint16_t) ~((uint16_t)TIM_MMS); // 清零主模式选择位
    TIMx->CTLR2 |= TIM_TRGOSource; // 设置触发输出源
}

/*********************************************************************
 * @函数名 TIM_SelectSlaveMode
 *
 * @描述   选择TIMx从模式。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_SlaveMode - 指定定时器从模式。
 *            TIM_SlaveMode_Reset - 选定触发信号（TRGI）的上升沿重新初始化计数器。
 *            TIM_SlaveMode_Gated - 当触发信号（TRGI）为高电平时，计数器时钟使能。
 *            TIM_SlaveMode_Trigger - 计数器在触发TRGI的上升沿开始。
 *            TIM_SlaveMode_External1 - 选定触发（TRGI）的上升沿时钟计数器。
 *
 * @返回值 无
 */
void TIM_SelectSlaveMode(TIM_TypeDef *TIMx, uint16_t TIM_SlaveMode)
{
    TIMx->SMCFGR &= (uint16_t) ~((uint16_t)TIM_SMS); // 清零从模式选择位
    TIMx->SMCFGR |= TIM_SlaveMode; // 设置从模式
}

/*********************************************************************
 * @函数名 TIM_SelectMasterSlaveMode
 *
 * @描述   设置或复位TIMx主/从模式。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_MasterSlaveMode - 指定定时器主从模式。
 *            TIM_MasterSlaveMode_Enable - 当前定时器与其从定时器之间的同步（通过TRGO）。
 *            TIM_MasterSlaveMode_Disable - 无动作。
 *
 * @返回值 无
 */
void TIM_SelectMasterSlaveMode(TIM_TypeDef *TIMx, uint16_t TIM_MasterSlaveMode)
{
    TIMx->SMCFGR &= (uint16_t) ~((uint16_t)TIM_MSM); // 清零主从模式位
    TIMx->SMCFGR |= TIM_MasterSlaveMode; // 设置主从模式
}

/*********************************************************************
 * @函数名 TIM_SetCounter
 *
 * @描述   设置TIMx计数器寄存器值。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          Counter - 指定计数器寄存器的新值。
 *
 * @返回值 无
 */
void TIM_SetCounter(TIM_TypeDef *TIMx, uint16_t Counter)
{
    TIMx->CNT = Counter; // 设置计数器值
}

/*********************************************************************
 * @函数名 TIM_SetAutoreload
 *
 * @描述   设置TIMx自动重装载寄存器值。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          Autoreload - 指定自动重装载寄存器的新值。
 *
 * @返回值 无
 */
void TIM_SetAutoreload(TIM_TypeDef *TIMx, uint16_t Autoreload)
{
    TIMx->ATRLR = Autoreload; // 设置自动重装载值
}

/*********************************************************************
 * @函数名 TIM_SetCompare1
 *
 * @描述   设置TIMx捕获比较1寄存器值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          Compare1 - 指定捕获比较1寄存器的新值。
 *
 * @返回值 无
 */
void TIM_SetCompare1(TIM_TypeDef *TIMx, uint16_t Compare1)
{
    TIMx->CH1CVR = Compare1; // 设置通道1比较值
}

/*********************************************************************
 * @函数名 TIM_SetCompare2
 *
 * @描述   设置TIMx捕获比较2寄存器值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          Compare2 - 指定捕获比较2寄存器的新值。
 *
 * @返回值 无
 */
void TIM_SetCompare2(TIM_TypeDef *TIMx, uint16_t Compare2)
{
    TIMx->CH2CVR = Compare2; // 设置通道2比较值
}

/*********************************************************************
 * @函数名 TIM_SetCompare3
 *
 * @描述   设置TIMx捕获比较3寄存器值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          Compare3 - 指定捕获比较3寄存器的新值。
 *
 * @返回值 无
 */
void TIM_SetCompare3(TIM_TypeDef *TIMx, uint16_t Compare3)
{
    TIMx->CH3CVR = Compare3; // 设置通道3比较值
}

/*********************************************************************
 * @函数名 TIM_SetCompare4
 *
 * @描述   设置TIMx捕获比较4寄存器值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          Compare4 - 指定捕获比较4寄存器的新值。
 *
 * @返回值 无
 */
void TIM_SetCompare4(TIM_TypeDef *TIMx, uint16_t Compare4)
{
    TIMx->CH4CVR = Compare4; // 设置通道4比较值
}

/*********************************************************************
 * @函数名 TIM_SetIC1Prescaler
 *
 * @描述   设置TIMx输入捕获1预分频器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPSC - 指定输入捕获1预分频器的新值。
 *            TIM_ICPSC_DIV1 - 无预分频。
 *            TIM_ICPSC_DIV2 - 每2个事件捕获一次。
 *            TIM_ICPSC_DIV4 - 每4个事件捕获一次。
 *            TIM_ICPSC_DIV8 - 每8个事件捕获一次。
 *
 * @返回值 无
 */
void TIM_SetIC1Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR1 &= (uint16_t) ~((uint16_t)TIM_IC1PSC); // 清零输入捕获1预分频位
    TIMx->CHCTLR1 |= TIM_ICPSC; // 设置输入捕获1预分频
}

/*********************************************************************
 * @函数名 TIM_SetIC2Prescaler
 *
 * @描述   设置TIMx输入捕获2预分频器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPSC - 指定输入捕获2预分频器的新值。
 *            TIM_ICPSC_DIV1 - 无预分频。
 *            TIM_ICPSC_DIV2 - 每2个事件捕获一次。
 *            TIM_ICPSC_DIV4 - 每4个事件捕获一次。
 *            TIM_ICPSC_DIV8 - 每8个事件捕获一次。
 *
 * @返回值 无
 */
void TIM_SetIC2Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR1 &= (uint16_t) ~((uint16_t)TIM_IC2PSC); // 清零输入捕获2预分频位
    TIMx->CHCTLR1 |= (uint16_t)(TIM_ICPSC << 8); // 设置输入捕获2预分频（位8）
}

/*********************************************************************
 * @函数名 TIM_SetIC3Prescaler
 *
 * @描述   设置TIMx输入捕获3预分频器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPSC - 指定输入捕获3预分频器的新值。
 *            TIM_ICPSC_DIV1 - 无预分频。
 *            TIM_ICPSC_DIV2 - 每2个事件捕获一次。
 *            TIM_ICPSC_DIV4 - 每4个事件捕获一次。
 *            TIM_ICPSC_DIV8 - 每8个事件捕获一次。
 *
 * @返回值 无
 */
void TIM_SetIC3Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR2 &= (uint16_t) ~((uint16_t)TIM_IC3PSC); // 清零输入捕获3预分频位
    TIMx->CHCTLR2 |= TIM_ICPSC; // 设置输入捕获3预分频
}

/*********************************************************************
 * @函数名 TIM_SetIC4Prescaler
 *
 * @描述   设置TIMx输入捕获4预分频器。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPSC - 指定输入捕获4预分频器的新值。
 *            TIM_ICPSC_DIV1 - 无预分频。
 *            TIM_ICPSC_DIV2 - 每2个事件捕获一次。
 *            TIM_ICPSC_DIV4 - 每4个事件捕获一次。
 *            TIM_ICPSC_DIV8 - 每8个事件捕获一次。
 *
 * @返回值 无
 */
void TIM_SetIC4Prescaler(TIM_TypeDef *TIMx, uint16_t TIM_ICPSC)
{
    TIMx->CHCTLR2 &= (uint16_t) ~((uint16_t)TIM_IC4PSC); // 清零输入捕获4预分频位
    TIMx->CHCTLR2 |= (uint16_t)(TIM_ICPSC << 8); // 设置输入捕获4预分频（位8）
}

/*********************************************************************
 * @函数名 TIM_SetClockDivision
 *
 * @描述   设置TIMx时钟分频值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_CKD - 指定时钟分频值。
 *            TIM_CKD_DIV1 - TDTS = Tck_tim。
 *            TIM_CKD_DIV2 - TDTS = 2*Tck_tim。
 *            TIM_CKD_DIV4 - TDTS = 4*Tck_tim。
 *
 * @返回值 无
 */
void TIM_SetClockDivision(TIM_TypeDef *TIMx, uint16_t TIM_CKD)
{
    TIMx->CTLR1 &= (uint16_t) ~((uint16_t)TIM_CTLR1_CKD); // 清零时钟分频位
    TIMx->CTLR1 |= TIM_CKD; // 设置时钟分频
}

/*********************************************************************
 * @函数名 TIM_GetCapture1
 *
 * @描述   获取TIMx输入捕获1值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *
 * @返回值 TIMx->CH1CVR - 捕获比较1寄存器值。
 */
uint16_t TIM_GetCapture1(TIM_TypeDef *TIMx)
{
    return TIMx->CH1CVR;
}

/*********************************************************************
 * @函数名 TIM_GetCapture2
 *
 * @描述   获取TIMx输入捕获2值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *
 * @返回值 TIMx->CH2CVR - 捕获比较2寄存器值。
 */
uint16_t TIM_GetCapture2(TIM_TypeDef *TIMx)
{
    return TIMx->CH2CVR;
}

/*********************************************************************
 * @函数名 TIM_GetCapture3
 *
 * @描述   获取TIMx输入捕获3值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *
 * @返回值 TIMx->CH3CVR - 捕获比较3寄存器值。
 */
uint16_t TIM_GetCapture3(TIM_TypeDef *TIMx)
{
    return TIMx->CH3CVR;
}

/*********************************************************************
 * @函数名 TIM_GetCapture4
 *
 * @描述   获取TIMx输入捕获4值。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *
 * @返回值 TIMx->CH4CVR - 捕获比较4寄存器值。
 */
uint16_t TIM_GetCapture4(TIM_TypeDef *TIMx)
{
    return TIMx->CH4CVR;
}

/*********************************************************************
 * @函数名 TIM_GetCounter
 *
 * @描述   获取TIMx计数器值。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *
 * @返回值 TIMx->CNT - 计数器寄存器值。
 */
uint16_t TIM_GetCounter(TIM_TypeDef *TIMx)
{
    return TIMx->CNT;
}

/*********************************************************************
 * @函数名 TIM_GetPrescaler
 *
 * @描述   获取TIMx预分频器值。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *
 * @返回值 TIMx->PSC - 预分频器寄存器值。
 */
uint16_t TIM_GetPrescaler(TIM_TypeDef *TIMx)
{
    return TIMx->PSC;
}

/*********************************************************************
 * @函数名 TIM_GetFlagStatus
 *
 * @描述   检查指定的TIM标志是否设置。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_FLAG - 指定要检查的标志。
 *            TIM_FLAG_Update - TIM更新标志。
 *            TIM_FLAG_CC1 - TIM捕获比较1标志。
 *            TIM_FLAG_CC2 - TIM捕获比较2标志。
 *            TIM_FLAG_CC3 - TIM捕获比较3标志。
 *            TIM_FLAG_CC4 - TIM捕获比较4标志。
 *            TIM_FLAG_COM - TIM换向标志。
 *            TIM_FLAG_Trigger - TIM触发标志。
 *            TIM_FLAG_Break - TIM断路标志。
 *            TIM_FLAG_CC1OF - TIM捕获比较1过捕获标志。
 *            TIM_FLAG_CC2OF - TIM捕获比较2过捕获标志。
 *            TIM_FLAG_CC3OF - TIM捕获比较3过捕获标志。
 *            TIM_FLAG_CC4OF - TIM捕获比较4过捕获标志。
 *            TIM6/TIM7只有TIM_FLAG_Update。
 *
 * @返回值 标志状态（SET或RESET）。
 */
FlagStatus TIM_GetFlagStatus(TIM_TypeDef *TIMx, uint16_t TIM_FLAG)
{
    ITStatus bitstatus = RESET;

    if((TIMx->INTFR & TIM_FLAG) != (uint16_t)RESET)
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
 * @函数名 TIM_ClearFlag
 *
 * @描述   清除TIMx的待处理标志。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_FLAG - 指定要清除的标志。
 *            TIM_FLAG_Update - TIM更新标志。
 *            TIM_FLAG_CC1 - TIM捕获比较1标志。
 *            TIM_FLAG_CC2 - TIM捕获比较2标志。
 *            TIM_FLAG_CC3 - TIM捕获比较3标志。
 *            TIM_FLAG_CC4 - TIM捕获比较4标志。
 *            TIM_FLAG_COM - TIM换向标志。
 *            TIM_FLAG_Trigger - TIM触发标志。
 *            TIM_FLAG_Break - TIM断路标志。
 *            TIM_FLAG_CC1OF - TIM捕获比较1过捕获标志。
 *            TIM_FLAG_CC2OF - TIM捕获比较2过捕获标志。
 *            TIM_FLAG_CC3OF - TIM捕获比较3过捕获标志。
 *            TIM_FLAG_CC4OF - TIM捕获比较4过捕获标志。
 *            TIM6/TIM7只有TIM_FLAG_Update。
 *
 * @返回值 无
 */
void TIM_ClearFlag(TIM_TypeDef *TIMx, uint16_t TIM_FLAG)
{
    TIMx->INTFR = (uint16_t)~TIM_FLAG; // 通过写入0清除标志位
}

/*********************************************************************
 * @函数名 TIM_GetITStatus
 *
 * @描述   检查TIM中断是否发生。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_IT - 指定要检查的TIM中断源。
 *            TIM_IT_Update - TIM更新中断源。
 *            TIM_IT_CC1 - TIM捕获比较1中断源。
 *            TIM_IT_CC2 - TIM捕获比较2中断源。
 *            TIM_IT_CC3 - TIM捕获比较3中断源。
 *            TIM_IT_CC4 - TIM捕获比较4中断源。
 *            TIM_IT_COM - TIM换向中断源。
 *            TIM_IT_Trigger - TIM触发中断源。
 *            TIM_IT_Break - TIM断路中断源。
 *            TIM6/TIM7只有TIM_IT_Update。
 *
 * @返回值 中断状态（SET或RESET）。
 */
ITStatus TIM_GetITStatus(TIM_TypeDef *TIMx, uint16_t TIM_IT)
{
    ITStatus bitstatus = RESET;
    uint16_t itstatus = 0x0, itenable = 0x0;

    itstatus = TIMx->INTFR & TIM_IT; // 检查中断标志位

    itenable = TIMx->DMAINTENR & TIM_IT; // 检查中断使能位
    if((itstatus != (uint16_t)RESET) && (itenable != (uint16_t)RESET))
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
 * @函数名 TIM_ClearITPendingBit
 *
 * @描述   清除TIMx的中断待处理位。
 *
 * @参数   TIMx - x可以是1到10，用于选择TIM外设。
 *          TIM_IT - 指定要清除的中断源。
 *            TIM_IT_Update - TIM更新中断源。
 *            TIM_IT_CC1 - TIM捕获比较1中断源。
 *            TIM_IT_CC2 - TIM捕获比较2中断源。
 *            TIM_IT_CC3 - TIM捕获比较3中断源。
 *            TIM_IT_CC4 - TIM捕获比较4中断源。
 *            TIM_IT_COM - TIM换向中断源。
 *            TIM_IT_Trigger - TIM触发中断源。
 *            TIM_IT_Break - TIM断路中断源。
 *            TIM6/TIM7只有TIM_IT_Update。
 *
 * @返回值 无
 */
void TIM_ClearITPendingBit(TIM_TypeDef *TIMx, uint16_t TIM_IT)
{
    TIMx->INTFR = (uint16_t)~TIM_IT; // 通过写入0清除中断标志位
}

/*********************************************************************
 * @函数名 TI1_Config
 *
 * @描述   将TI1配置为输入。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPolarity - 输入极性。
 *             TIM_ICPolarity_Rising - 上升沿。
 *             TIM_ICPolarity_Falling - 下降沿。
 *          TIM_ICSelection - 指定要使用的输入。
 *             TIM_ICSelection_DirectTI - TIM输入1连接到IC1。
 *             TIM_ICSelection_IndirectTI - TIM输入1连接到IC2。
 *             TIM_ICSelection_TRC - TIM输入1连接到TRC。
 *          TIM_ICFilter - 指定输入捕获滤波器。
 *             此参数必须是0x00到0x0F之间的值。
 *
 * @返回值 无
 */
static void TI1_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr1 = 0, tmpccer = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC1E); // 禁用通道1
    tmpccmr1 = TIMx->CHCTLR1; // 备份CHCTLR1寄存器
    tmpccer = TIMx->CCER; // 备份CCER寄存器
    
    // 配置输入捕获模式和滤波器
    tmpccmr1 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC1S)) & ((uint16_t) ~((uint16_t)TIM_IC1F)));
    tmpccmr1 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

    // 配置输入极性
    tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC1P));
    tmpccer |= (uint16_t)(TIM_ICPolarity | (uint16_t)TIM_CC1E); // 使能通道1
 
    TIMx->CHCTLR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TI2_Config
 *
 * @描述   将TI2配置为输入。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPolarity - 输入极性。
 *             TIM_ICPolarity_Rising - 上升沿。
 *             TIM_ICPolarity_Falling - 下降沿。
 *          TIM_ICSelection - 指定要使用的输入。
 *             TIM_ICSelection_DirectTI - TIM输入2连接到IC2。
 *             TIM_ICSelection_IndirectTI - TIM输入2连接到IC1。
 *             TIM_ICSelection_TRC - TIM输入2连接到TRC。
 *          TIM_ICFilter - 指定输入捕获滤波器。
 *             此参数必须是0x00到0x0F之间的值。
 *
 * @返回值 无
 */
static void TI2_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr1 = 0, tmpccer = 0, tmp = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC2E); // 禁用通道2
    tmpccmr1 = TIMx->CHCTLR1; // 备份CHCTLR1寄存器
    tmpccer = TIMx->CCER; // 备份CCER寄存器
    tmp = (uint16_t)(TIM_ICPolarity << 4); // 计算极性位偏移
    
    // 配置输入捕获模式和滤波器
    tmpccmr1 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC2S)) & ((uint16_t) ~((uint16_t)TIM_IC2F)));
    tmpccmr1 |= (uint16_t)(TIM_ICFilter << 12); // 设置滤波器
    tmpccmr1 |= (uint16_t)(TIM_ICSelection << 8); // 设置输入选择

    // 配置输入极性
    tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC2P));
    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC2E); // 使能通道2

    TIMx->CHCTLR1 = tmpccmr1;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TI3_Config
 *
 * @描述   将TI3配置为输入。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPolarity - 输入极性。
 *             TIM_ICPolarity_Rising - 上升沿。
 *             TIM_ICPolarity_Falling - 下降沿。
 *          TIM_ICSelection - 指定要使用的输入。
 *             TIM_ICSelection_DirectTI - TIM输入3连接到IC3。
 *             TIM_ICSelection_IndirectTI - TIM输入3连接到IC4。
 *             TIM_ICSelection_TRC - TIM输入3连接到TRC。
 *          TIM_ICFilter - 指定输入捕获滤波器。
 *             此参数必须是0x00到0x0F之间的值。
 *
 * @返回值 无
 */
static void TI3_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC3E); // 禁用通道3
    tmpccmr2 = TIMx->CHCTLR2; // 备份CHCTLR2寄存器
    tmpccer = TIMx->CCER; // 备份CCER寄存器
    tmp = (uint16_t)(TIM_ICPolarity << 8); // 计算极性位偏移
    
    // 配置输入捕获模式和滤波器
    tmpccmr2 &= (uint16_t)(((uint16_t) ~((uint16_t)TIM_CC3S)) & ((uint16_t) ~((uint16_t)TIM_IC3F)));
    tmpccmr2 |= (uint16_t)(TIM_ICSelection | (uint16_t)(TIM_ICFilter << (uint16_t)4));

    // 配置输入极性
    tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC3P));
    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC3E); // 使能通道3

    TIMx->CHCTLR2 = tmpccmr2;
    TIMx->CCER = tmpccer;
}

/*********************************************************************
 * @函数名 TI4_Config
 *
 * @描述   将TI4配置为输入。
 *
 * @参数   TIMx - x可以是（1、2、3、4、5、8、9、10），用于选择TIM外设。
 *          TIM_ICPolarity - 输入极性。
 *             TIM_ICPolarity_Rising - 上升沿。
 *             TIM_ICPolarity_Falling - 下降沿。
 *          TIM_ICSelection - 指定要使用的输入。
 *             TIM_ICSelection_DirectTI - TIM输入4连接到IC4。
 *             TIM_ICSelection_IndirectTI - TIM输入4连接到IC3。
 *             TIM_ICSelection_TRC - TIM输入4连接到TRC。
 *          TIM_ICFilter - 指定输入捕获滤波器。
 *             此参数必须是0x00到0x0F之间的值。
 *
 * @返回值 无
 */
static void TI4_Config(TIM_TypeDef *TIMx, uint16_t TIM_ICPolarity, uint16_t TIM_ICSelection,
                       uint16_t TIM_ICFilter)
{
    uint16_t tmpccmr2 = 0, tmpccer = 0, tmp = 0;

    TIMx->CCER &= (uint16_t) ~((uint16_t)TIM_CC4E); // 禁用通道4
    tmpccmr2 = TIMx->CHCTLR2; // 备份CHCTLR2寄存器
    tmpccer = TIMx->CCER; // 备份CCER寄存器
    tmp = (uint16_t)(TIM_ICPolarity << 12); // 计算极性位偏移
    
    // 配置输入捕获模式和滤波器
    tmpccmr2 &= (uint16_t)((uint16_t)(~(uint16_t)TIM_CC4S) & ((uint16_t) ~((uint16_t)TIM_IC4F)));
    tmpccmr2 |= (uint16_t)(TIM_ICSelection << 8); // 设置输入选择
    tmpccmr2 |= (uint16_t)(TIM_ICFilter << 12); // 设置滤波器

    // 配置输入极性
    tmpccer &= (uint16_t) ~((uint16_t)(TIM_CC4P));
    tmpccer |= (uint16_t)(tmp | (uint16_t)TIM_CC4E); // 使能通道4

    TIMx->CHCTLR2 = tmpccmr2;
    TIMx->CCER = tmpccer;
}
