/********************************** (C) COPYRIGHT *******************************
* 文件名称          : ch32v30x_usart.c
* 作者              : WCH
* 版本              : V1.0.1
* 日期              : 2025/04/12
* 描述              : 本文件提供了所有USART固件函数
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意：本软件（修改或未修改）及二进制文件用于
*       南京沁恒微电子制造的微控制器。
*******************************************************************************/
#include "ch32v30x_usart.h"
#include "ch32v30x_rcc.h"

/* USART私有定义 */
#define CTLR1_UE_Set              ((uint16_t)0x2000) /* USART使能掩码 */
#define CTLR1_UE_Reset            ((uint16_t)0xDFFF) /* USART失能掩码 */

#define CTLR1_WAKE_Mask           ((uint16_t)0xF7FF) /* USART唤醒方式掩码 */

#define CTLR1_RWU_Set             ((uint16_t)0x0002) /* USART静默模式使能掩码 */
#define CTLR1_RWU_Reset           ((uint16_t)0xFFFD) /* USART静默模式失能掩码 */
#define CTLR1_SBK_Set             ((uint16_t)0x0001) /* USART发送中断字符掩码 */
#define CTLR1_CLEAR_Mask          ((uint16_t)0xE9F3) /* USART CTLR1寄存器掩码（用于清除相关位） */
#define CTLR2_Address_Mask        ((uint16_t)0xFFF0) /* USART地址掩码 */

#define CTLR2_LINEN_Set           ((uint16_t)0x4000) /* USART LIN使能掩码 */
#define CTLR2_LINEN_Reset         ((uint16_t)0xBFFF) /* USART LIN失能掩码 */

#define CTLR2_LBDL_Mask           ((uint16_t)0xFFDF) /* USART LIN中断检测掩码 */
#define CTLR2_STOP_CLEAR_Mask     ((uint16_t)0xCFFF) /* USART CTLR2停止位掩码 */
#define CTLR2_CLOCK_CLEAR_Mask    ((uint16_t)0xF0FF) /* USART CTLR2时钟掩码 */

#define CTLR3_SCEN_Set            ((uint16_t)0x0020) /* USART智能卡模式使能掩码 */
#define CTLR3_SCEN_Reset          ((uint16_t)0xFFDF) /* USART智能卡模式失能掩码 */

#define CTLR3_NACK_Set            ((uint16_t)0x0010) /* USART智能卡NACK使能掩码 */
#define CTLR3_NACK_Reset          ((uint16_t)0xFFEF) /* USART智能卡NACK失能掩码 */

#define CTLR3_HDSEL_Set           ((uint16_t)0x0008) /* USART半双工使能掩码 */
#define CTLR3_HDSEL_Reset         ((uint16_t)0xFFF7) /* USART半双工失能掩码 */

#define CTLR3_IRLP_Mask           ((uint16_t)0xFFFB) /* USART IrDA低功耗模式掩码 */
#define CTLR3_CLEAR_Mask          ((uint16_t)0xFCFF) /* USART CTLR3寄存器掩码（用于清除相关位） */

#define CTLR3_IREN_Set            ((uint16_t)0x0002) /* USART IrDA使能掩码 */
#define CTLR3_IREN_Reset          ((uint16_t)0xFFFD) /* USART IrDA失能掩码 */
#define GPR_LSB_Mask              ((uint16_t)0x00FF) /* 保护时间寄存器低字节掩码 */
#define GPR_MSB_Mask              ((uint16_t)0xFF00) /* 保护时间寄存器高字节掩码 */
#define IT_Mask                   ((uint16_t)0x001F) /* USART中断掩码（用于提取中断位位置） */

/*********************************************************************
 * @函数名      USART_DeInit
 *
 * @描述        将USARTx外设寄存器复位为默认值
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *
 * @返回值      无
 */
void USART_DeInit(USART_TypeDef *USARTx)
{
    if(USARTx == USART1)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, ENABLE);  /* 使能USART1复位 */
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_USART1, DISABLE); /* 释放USART1复位 */
    }
    else if(USARTx == USART2)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, ENABLE);  /* 使能USART2复位 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2, DISABLE); /* 释放USART2复位 */
    }
    else if(USARTx == USART3)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, ENABLE);  /* 使能USART3复位 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3, DISABLE); /* 释放USART3复位 */
    }
    else if(USARTx == UART4)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, ENABLE);   /* 使能UART4复位 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART4, DISABLE);  /* 释放UART4复位 */
    }
    else if(USARTx == UART5)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5, ENABLE);   /* 使能UART5复位 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART5, DISABLE);  /* 释放UART5复位 */
    }
    else if(USARTx == UART6)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART6, ENABLE);   /* 使能UART6复位 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART6, DISABLE);  /* 释放UART6复位 */
    }
    else if(USARTx == UART7)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, ENABLE);   /* 使能UART7复位 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART7, DISABLE);  /* 释放UART7复位 */
    }
    else if(USARTx == UART8)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, ENABLE);   /* 使能UART8复位 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART8, DISABLE);  /* 释放UART8复位 */
    }
}

/*********************************************************************
 * @函数名      USART_Init
 *
 * @描述        根据USART_InitStruct中的指定参数初始化USARTx外设
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_InitStruct - 指向USART_InitTypeDef结构体的指针，
 *                                 包含指定USART外设的配置信息
 *
 * @返回值      无
 */
void USART_Init(USART_TypeDef *USARTx, USART_InitTypeDef *USART_InitStruct)
{
    uint32_t          tmpreg = 0x00, apbclock = 0x00;
    uint32_t          integerdivider = 0x00;   /* 整数分频器 */
    uint32_t          fractionaldivider = 0x00;/* 小数分频器 */
    uint32_t          usartxbase = 0;          /* USART基地址 */
    RCC_ClocksTypeDef RCC_ClocksStatus;        /* 用于存储时钟频率 */

    /* 如果使用了硬件流控，这里可以添加相关初始化代码（当前为空） */
    if(USART_InitStruct->USART_HardwareFlowControl != USART_HardwareFlowControl_None)
    {
    }

    usartxbase = (uint32_t)USARTx;  /* 获取USART外设基地址 */
    
    /* 配置停止位 */
    tmpreg = USARTx->CTLR2;                        /* 读取CTLR2寄存器当前值 */
    tmpreg &= CTLR2_STOP_CLEAR_Mask;               /* 清除停止位 */
    tmpreg |= (uint32_t)USART_InitStruct->USART_StopBits; /* 设置新的停止位 */
    USARTx->CTLR2 = (uint16_t)tmpreg;              /* 写回CTLR2寄存器 */

    /* 配置字长、奇偶校验和模式（收发使能） */
    tmpreg = USARTx->CTLR1;                        /* 读取CTLR1寄存器当前值 */
    tmpreg &= CTLR1_CLEAR_Mask;                    /* 清除相关位 */
    tmpreg |= (uint32_t)USART_InitStruct->USART_WordLength | USART_InitStruct->USART_Parity |
              USART_InitStruct->USART_Mode;        /* 设置字长、奇偶校验和模式 */
    USARTx->CTLR1 = (uint16_t)tmpreg;              /* 写回CTLR1寄存器 */

    /* 配置硬件流控 */
    tmpreg = USARTx->CTLR3;                        /* 读取CTLR3寄存器当前值 */
    tmpreg &= CTLR3_CLEAR_Mask;                    /* 清除相关位 */
    tmpreg |= USART_InitStruct->USART_HardwareFlowControl; /* 设置硬件流控 */
    USARTx->CTLR3 = (uint16_t)tmpreg;              /* 写回CTLR3寄存器 */

    /* 获取系统时钟频率 */
    RCC_GetClocksFreq(&RCC_ClocksStatus);

    /* 确定USART挂载的APB总线时钟频率 */
    if(usartxbase == USART1_BASE)
    {
        apbclock = RCC_ClocksStatus.PCLK2_Frequency; /* USART1挂载在APB2总线 */
    }
    else
    {
        apbclock = RCC_ClocksStatus.PCLK1_Frequency; /* 其他USART挂载在APB1总线 */
    }

    /* 计算波特率分频值：
       integerdivider = (25 * apbclock) / (4 * baudrate)
       这个计算公式是CH32V30x特有的波特率计算方法 */
    integerdivider = ((25 * apbclock) / (4 * (USART_InitStruct->USART_BaudRate)));
    
    /* 整数部分：先除以100，然后左移4位（因为BRR寄存器高12位是整数部分） */
    tmpreg = (integerdivider / 100) << 4;
    
    /* 小数部分：integerdivider - 100*(整数部分) */
    fractionaldivider = integerdivider - (100 * (tmpreg >> 4));
    
    /* 小数部分计算：((小数部分 * 16) + 50) / 100，并取低4位 */
    tmpreg |= ((((fractionaldivider * 16) + 50) / 100)) & ((uint8_t)0x0F);
    
    /* 写入波特率寄存器 */
    USARTx->BRR = (uint16_t)tmpreg;
}

/*********************************************************************
 * @函数名      USART_StructInit
 *
 * @描述        用默认值填充USART_InitStruct的每个成员
 *
 * @参数        USART_InitStruct: 指向要初始化的USART_InitTypeDef结构体的指针
 *
 * @返回值      无
 */
void USART_StructInit(USART_InitTypeDef *USART_InitStruct)
{
    /* 设置默认值：波特率9600，8位数据位，1位停止位，无奇偶校验，收发使能，无硬件流控 */
    USART_InitStruct->USART_BaudRate = 9600;
    USART_InitStruct->USART_WordLength = USART_WordLength_8b;
    USART_InitStruct->USART_StopBits = USART_StopBits_1;
    USART_InitStruct->USART_Parity = USART_Parity_No;
    USART_InitStruct->USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct->USART_HardwareFlowControl = USART_HardwareFlowControl_None;
}

/*********************************************************************
 * @函数名      USART_ClockInit
 *
 * @描述        根据USART_ClockInitStruct中的指定参数初始化USARTx时钟
 *
 * @参数        USARTx - x可以是1、2、3选择USART外设
 *              USART_ClockInitStruct - 指向USART_ClockInitTypeDef结构体的指针，
 *                                      包含指定USART外设的时钟配置信息
 *
 * @返回值      无
 */
void USART_ClockInit(USART_TypeDef *USARTx, USART_ClockInitTypeDef *USART_ClockInitStruct)
{
    uint32_t tmpreg = 0x00;

    /* 配置时钟相关参数（时钟使能、时钟极性、时钟相位和最后位时钟脉冲） */
    tmpreg = USARTx->CTLR2;                        /* 读取CTLR2寄存器当前值 */
    tmpreg &= CTLR2_CLOCK_CLEAR_Mask;              /* 清除时钟相关位 */
    tmpreg |= (uint32_t)USART_ClockInitStruct->USART_Clock | USART_ClockInitStruct->USART_CPOL |
              USART_ClockInitStruct->USART_CPHA | USART_ClockInitStruct->USART_LastBit;
    USARTx->CTLR2 = (uint16_t)tmpreg;              /* 写回CTLR2寄存器 */
}

/*********************************************************************
 * @函数名      USART_ClockStructInit
 *
 * @描述        用默认值填充USART_ClockStructInit的每个成员
 *
 * @参数        USART_ClockInitStruct - 指向要初始化的USART_ClockInitTypeDef结构体的指针
 *
 * @返回值      无
 */
void USART_ClockStructInit(USART_ClockInitTypeDef *USART_ClockInitStruct)
{
    /* 设置默认值：时钟禁用、低电平、第一个边沿采样、最后位时钟脉冲禁用 */
    USART_ClockInitStruct->USART_Clock = USART_Clock_Disable;
    USART_ClockInitStruct->USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStruct->USART_CPHA = USART_CPHA_1Edge;
    USART_ClockInitStruct->USART_LastBit = USART_LastBit_Disable;
}

/*********************************************************************
 * @函数名      USART_Cmd
 *
 * @描述        使能或失能指定的USART外设
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              NewState: 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_Cmd(USART_TypeDef *USARTx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR1 |= CTLR1_UE_Set;    /* 设置UE位使能USART */
    }
    else
    {
        USARTx->CTLR1 &= CTLR1_UE_Reset;  /* 清除UE位失能USART */
    }
}

/*********************************************************************
 * @函数名      USART_ITConfig
 *
 * @描述        使能或失能指定的USART中断
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_IT - 指定要使能或失能的USART中断源
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_ITConfig(USART_TypeDef *USARTx, uint16_t USART_IT, FunctionalState NewState)
{
    uint32_t usartreg = 0x00, itpos = 0x00, itmask = 0x00;
    uint32_t usartxbase = 0x00;

    usartxbase = (uint32_t)USARTx;                 /* 获取USART基地址 */
    
    /* USART_IT的高3位决定中断使能寄存器（CTLR1、CTLR2、CTLR3） */
    usartreg = (((uint8_t)USART_IT) >> 0x05);
    
    /* 获取中断位在寄存器中的位置（低5位） */
    itpos = USART_IT & IT_Mask;
    
    /* 生成对应位的掩码 */
    itmask = (((uint32_t)0x01) << itpos);

    /* 根据中断源确定要操作的中断使能寄存器地址偏移 */
    if(usartreg == 0x01)        /* 对应CTLR1寄存器 */
    {
        usartxbase += 0x0C;     /* CTLR1寄存器偏移地址为0x0C */
    }
    else if(usartreg == 0x02)   /* 对应CTLR2寄存器 */
    {
        usartxbase += 0x10;     /* CTLR2寄存器偏移地址为0x10 */
    }
    else                        /* 对应CTLR3寄存器 */
    {
        usartxbase += 0x14;     /* CTLR3寄存器偏移地址为0x14 */
    }

    /* 设置或清除中断使能位 */
    if(NewState != DISABLE)
    {
        *(__IO uint32_t *)usartxbase |= itmask;   /* 使能中断 */
    }
    else
    {
        *(__IO uint32_t *)usartxbase &= ~itmask;  /* 失能中断 */
    }
}

/*********************************************************************
 * @函数名      USART_DMACmd
 *
 * @描述        使能或失能USART DMA接口
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_DMAReq - 指定DMA请求类型
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_DMACmd(USART_TypeDef *USARTx, uint16_t USART_DMAReq, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR3 |= USART_DMAReq;           /* 使能DMA请求 */
    }
    else
    {
        USARTx->CTLR3 &= (uint16_t)~USART_DMAReq; /* 失能DMA请求 */
    }
}

/*********************************************************************
 * @函数名      USART_SetAddress
 *
 * @描述        设置USART节点的地址（用于多处理器通信）
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_Address - 表示USART节点的地址
 *
 * @返回值      无
 */
void USART_SetAddress(USART_TypeDef *USARTx, uint8_t USART_Address)
{
    /* 清除地址位，然后设置新地址 */
    USARTx->CTLR2 &= CTLR2_Address_Mask;  /* 清除地址位 */
    USARTx->CTLR2 |= USART_Address;       /* 设置新地址 */
}

/*********************************************************************
 * @函数名      USART_WakeUpConfig
 *
 * @描述        选择USART唤醒方式
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_WakeUp - 指定USART唤醒方式
 *
 * @返回值      无
 */
void USART_WakeUpConfig(USART_TypeDef *USARTx, uint16_t USART_WakeUp)
{
    /* 清除唤醒方式位，然后设置新的唤醒方式 */
    USARTx->CTLR1 &= CTLR1_WAKE_Mask;  /* 清除唤醒方式位 */
    USARTx->CTLR1 |= USART_WakeUp;     /* 设置新的唤醒方式 */
}

/*********************************************************************
 * @函数名      USART_ReceiverWakeUpCmd
 *
 * @描述        决定USART是否进入静默模式（用于多处理器通信）
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_ReceiverWakeUpCmd(USART_TypeDef *USARTx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR1 |= CTLR1_RWU_Set;    /* 使能静默模式 */
    }
    else
    {
        USARTx->CTLR1 &= CTLR1_RWU_Reset;  /* 失能静默模式 */
    }
}

/*********************************************************************
 * @函数名      USART_LINBreakDetectLengthConfig
 *
 * @描述        设置USART LIN中断检测长度
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_LINBreakDetectLength - 指定LIN中断检测长度
 *
 * @返回值      无
 */
void USART_LINBreakDetectLengthConfig(USART_TypeDef *USARTx, uint16_t USART_LINBreakDetectLength)
{
    /* 清除LIN中断检测长度位，然后设置新的长度 */
    USARTx->CTLR2 &= CTLR2_LBDL_Mask;                 /* 清除LIN中断检测长度位 */
    USARTx->CTLR2 |= USART_LINBreakDetectLength;      /* 设置新的LIN中断检测长度 */
}

/*********************************************************************
 * @函数名      USART_LINCmd
 *
 * @描述        使能或失能USART LIN模式
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_LINCmd(USART_TypeDef *USARTx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR2 |= CTLR2_LINEN_Set;    /* 使能LIN模式 */
    }
    else
    {
        USARTx->CTLR2 &= CTLR2_LINEN_Reset;  /* 失能LIN模式 */
    }
}

/*********************************************************************
 * @函数名      USART_SendData
 *
 * @描述        通过USARTx外设发送单个数据
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              Data - 要发送的数据（只使用低9位）
 *
 * @返回值      无
 */
void USART_SendData(USART_TypeDef *USARTx, uint16_t Data)
{
    /* 将数据写入数据寄存器（只取低9位） */
    USARTx->DATAR = (Data & (uint16_t)0x01FF);
}

/*********************************************************************
 * @函数名      USART_ReceiveData
 *
 * @描述        返回USARTx外设最近接收到的数据
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *
 * @返回值      接收到的数据（只返回低9位）
 */
uint16_t USART_ReceiveData(USART_TypeDef *USARTx)
{
    /* 读取数据寄存器并返回低9位数据 */
    return (uint16_t)(USARTx->DATAR & (uint16_t)0x01FF);
}

/*********************************************************************
 * @函数名      USART_SendBreak
 *
 * @描述        发送中断字符（用于LIN通信）
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *
 * @返回值      无
 */
void USART_SendBreak(USART_TypeDef *USARTx)
{
    /* 设置SBK位发送中断字符 */
    USARTx->CTLR1 |= CTLR1_SBK_Set;
}

/*********************************************************************
 * @函数名      USART_SetGuardTime
 *
 * @描述        设置指定的USART保护时间（用于智能卡模式）
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_GuardTime - 指定保护时间
 *
 * @返回值      无
 */
void USART_SetGuardTime(USART_TypeDef *USARTx, uint8_t USART_GuardTime)
{
    /* 清除保护时间高字节，然后设置新的保护时间（高字节） */
    USARTx->GPR &= GPR_LSB_Mask;                          /* 清除高字节 */
    USARTx->GPR |= (uint16_t)((uint16_t)USART_GuardTime << 0x08); /* 设置高字节 */
}

/*********************************************************************
 * @函数名      USART_SetPrescaler
 *
 * @描述        设置系统时钟预分频器（用于IrDA模式）
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_Prescaler - 指定预分频器时钟
 *
 * @返回值      无
 */
void USART_SetPrescaler(USART_TypeDef *USARTx, uint8_t USART_Prescaler)
{
    /* 清除预分频器低字节，然后设置新的预分频器（低字节） */
    USARTx->GPR &= GPR_MSB_Mask;     /* 清除低字节 */
    USARTx->GPR |= USART_Prescaler;  /* 设置低字节 */
}

/*********************************************************************
 * @函数名      USART_SmartCardCmd
 *
 * @描述        使能或失能USART智能卡模式
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_SmartCardCmd(USART_TypeDef *USARTx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR3 |= CTLR3_SCEN_Set;    /* 使能智能卡模式 */
    }
    else
    {
        USARTx->CTLR3 &= CTLR3_SCEN_Reset;  /* 失能智能卡模式 */
    }
}

/*********************************************************************
 * @函数名      USART_SmartCardNACKCmd
 *
 * @描述        使能或失能NACK传输（用于智能卡模式）
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_SmartCardNACKCmd(USART_TypeDef *USARTx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR3 |= CTLR3_NACK_Set;    /* 使能NACK传输 */
    }
    else
    {
        USARTx->CTLR3 &= CTLR3_NACK_Reset;  /* 失能NACK传输 */
    }
}

/*********************************************************************
 * @函数名      USART_HalfDuplexCmd
 *
 * @描述        使能或失能USART半双工通信
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_HalfDuplexCmd(USART_TypeDef *USARTx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR3 |= CTLR3_HDSEL_Set;    /* 使能半双工模式 */
    }
    else
    {
        USARTx->CTLR3 &= CTLR3_HDSEL_Reset;  /* 失能半双工模式 */
    }
}

/*********************************************************************
 * @函数名      USART_IrDAConfig
 *
 * @描述        配置USART的IrDA接口
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_IrDAMode - 指定IrDA模式
 *
 * @返回值      无
 */
void USART_IrDAConfig(USART_TypeDef *USARTx, uint16_t USART_IrDAMode)
{
    /* 清除IrDA低功耗模式位，然后设置新的IrDA模式 */
    USARTx->CTLR3 &= CTLR3_IRLP_Mask;     /* 清除IrDA低功耗模式位 */
    USARTx->CTLR3 |= USART_IrDAMode;      /* 设置新的IrDA模式 */
}

/*********************************************************************
 * @函数名      USART_IrDACmd
 *
 * @描述        使能或失能USART的IrDA接口
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              NewState - 使能(ENABLE)或失能(DISABLE)
 *
 * @返回值      无
 */
void USART_IrDACmd(USART_TypeDef *USARTx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        USARTx->CTLR3 |= CTLR3_IREN_Set;    /* 使能IrDA模式 */
    }
    else
    {
        USARTx->CTLR3 &= CTLR3_IREN_Reset;  /* 失能IrDA模式 */
    }
}

/*********************************************************************
 * @函数名      USART_GetFlagStatus
 *
 * @描述        检查指定的USART标志是否置位
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_FLAG - 指定要检查的标志
 *
 * @返回值      标志状态：置位(SET)或复位(RESET)
 */
FlagStatus USART_GetFlagStatus(USART_TypeDef *USARTx, uint16_t USART_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* 检查指定标志位是否置位 */
    if((USARTx->STATR & USART_FLAG) != (uint16_t)RESET)
    {
        bitstatus = SET;    /* 标志置位 */
    }
    else
    {
        bitstatus = RESET;  /* 标志复位 */
    }
    return bitstatus;
}

/*********************************************************************
 * @函数名      USART_ClearFlag
 *
 * @描述        清除USARTx的挂起标志
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_FLAG - 指定要清除的标志
 *
 * @返回值      无
 */
void USART_ClearFlag(USART_TypeDef *USARTx, uint16_t USART_FLAG)
{
    /* 通过向STATR寄存器写入标志位的反码来清除标志 */
    USARTx->STATR = (uint16_t)~USART_FLAG;
}

/*********************************************************************
 * @函数名      USART_GetITStatus
 *
 * @描述        检查指定的USART中断是否发生
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_IT - 指定要检查的USART中断源
 *
 * @返回值      中断状态：置位(SET)或复位(RESET)
 */
ITStatus USART_GetITStatus(USART_TypeDef *USARTx, uint16_t USART_IT)
{
    uint32_t bitpos = 0x00, itmask = 0x00, usartreg = 0x00;
    ITStatus bitstatus = RESET;

    /* 获取中断使能寄存器位置和中断位掩码 */
    usartreg = (((uint8_t)USART_IT) >> 0x05);      /* 高3位决定中断使能寄存器 */
    itmask = USART_IT & IT_Mask;                   /* 低5位是中断位位置 */
    itmask = (uint32_t)0x01 << itmask;             /* 生成中断使能位掩码 */

    /* 检查中断是否使能 */
    if(usartreg == 0x01)        /* CTLR1寄存器 */
    {
        itmask &= USARTx->CTLR1;
    }
    else if(usartreg == 0x02)   /* CTLR2寄存器 */
    {
        itmask &= USARTx->CTLR2;
    }
    else                        /* CTLR3寄存器 */
    {
        itmask &= USARTx->CTLR3;
    }

    /* 获取状态寄存器中的中断标志位位置 */
    bitpos = USART_IT >> 0x08;                     /* USART_IT的高8位决定状态位位置 */
    bitpos = (uint32_t)0x01 << bitpos;             /* 生成状态位掩码 */
    bitpos &= USARTx->STATR;                       /* 检查状态位是否置位 */

    /* 如果中断使能且中断标志置位，则中断发生 */
    if((itmask != (uint16_t)RESET) && (bitpos != (uint16_t)RESET))
    {
        bitstatus = SET;    /* 中断发生 */
    }
    else
    {
        bitstatus = RESET;  /* 中断未发生 */
    }

    return bitstatus;
}

/*********************************************************************
 * @函数名      USART_ClearITPendingBit
 *
 * @描述        清除USARTx的中断挂起位
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              USART_IT - 指定要清除的中断挂起位
 *
 * @返回值      无
 */
void USART_ClearITPendingBit(USART_TypeDef *USARTx, uint16_t USART_IT)
{
    uint16_t bitpos = 0x00, itmask = 0x00;

    /* 获取状态寄存器中的中断标志位位置并生成掩码 */
    bitpos = USART_IT >> 0x08;                     /* USART_IT的高8位决定状态位位置 */
    itmask = ((uint16_t)0x01 << (uint16_t)bitpos); /* 生成状态位掩码 */
    
    /* 通过向STATR寄存器写入掩码的反码来清除中断挂起位 */
    USARTx->STATR = (uint16_t)~itmask;
}
