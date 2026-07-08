/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_i2c.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2024/03/06
* 描述              : 本文件提供了所有I2C固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或不修改）和二进制代码用于南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/

#include "ch32v30x_i2c.h"
#include "ch32v30x_rcc.h"

/* I2C 控制寄存器1 (CTLR1) 的位掩码定义 */
#define CTLR1_PE_Set             ((uint16_t)0x0001)  /* 外设使能位(PE)置1 */
#define CTLR1_PE_Reset           ((uint16_t)0xFFFE)  /* 外设使能位(PE)清零 */
#define CTLR1_START_Set          ((uint16_t)0x0100)  /* 起始条件生成位(START)置1 */
#define CTLR1_START_Reset        ((uint16_t)0xFEFF)  /* 起始条件生成位(START)清零 */
#define CTLR1_STOP_Set           ((uint16_t)0x0200)  /* 停止条件生成位(STOP)置1 */
#define CTLR1_STOP_Reset         ((uint16_t)0xFDFF)  /* 停止条件生成位(STOP)清零 */
#define CTLR1_ACK_Set            ((uint16_t)0x0400)  /* 应答使能位(ACK)置1（使能应答） */
#define CTLR1_ACK_Reset          ((uint16_t)0xFBFF)  /* 应答使能位(ACK)清零 */
#define CTLR1_ENGC_Set           ((uint16_t)0x0040)  /* 广播呼叫使能位(ENGC)置1 */
#define CTLR1_ENGC_Reset         ((uint16_t)0xFFBF)  /* 广播呼叫使能位(ENGC)清零 */
#define CTLR1_SWRST_Set          ((uint16_t)0x8000)  /* 软件复位位(SWRST)置1 */
#define CTLR1_SWRST_Reset        ((uint16_t)0x7FFF)  /* 软件复位位(SWRST)清零 */
#define CTLR1_PEC_Set            ((uint16_t)0x1000)  /* PEC传输位(PEC)置1（发送PEC） */
#define CTLR1_PEC_Reset          ((uint16_t)0xEFFF)  /* PEC传输位(PEC)清零 */
#define CTLR1_ENPEC_Set          ((uint16_t)0x0020)  /* PEC计算使能位(ENPEC)置1 */
#define CTLR1_ENPEC_Reset        ((uint16_t)0xFFDF)  /* PEC计算使能位(ENPEC)清零 */
#define CTLR1_ENARP_Set          ((uint16_t)0x0010)  /* ARP使能位(ENARP)置1（SMBus地址解析协议） */
#define CTLR1_ENARP_Reset        ((uint16_t)0xFFEF)  /* ARP使能位(ENARP)清零 */
#define CTLR1_NOSTRETCH_Set      ((uint16_t)0x0080)  /* 时钟延长禁止位(NOSTRETCH)置1（禁止时钟延长） */
#define CTLR1_NOSTRETCH_Reset    ((uint16_t)0xFF7F)  /* 时钟延长禁止位(NOSTRETCH)清零 */

/* I2C 控制寄存器1 清除掩码（保留某些位不变） */
#define CTLR1_CLEAR_Mask         ((uint16_t)0xFBF5)

/* I2C 控制寄存器2 (CTLR2) 位掩码定义 */
#define CTLR2_DMAEN_Set          ((uint16_t)0x0800)  /* DMA请求使能位(DMAEN)置1 */
#define CTLR2_DMAEN_Reset        ((uint16_t)0xF7FF)  /* DMA请求使能位(DMAEN)清零 */
#define CTLR2_LAST_Set           ((uint16_t)0x1000)  /* DMA最后一次传输位(LAST)置1 */
#define CTLR2_LAST_Reset         ((uint16_t)0xEFFF)  /* DMA最后一次传输位(LAST)清零 */
#define CTLR2_FREQ_Reset         ((uint16_t)0xFFC0)  /* 外设时钟频率位(FREQ[5:0])清零掩码 */

/* I2C 自身地址寄存器1 (OADDR1) 位掩码定义 */
#define OADDR1_ADD0_Set          ((uint16_t)0x0001)  /* 地址位0（用于10位地址模式）置1 */
#define OADDR1_ADD0_Reset        ((uint16_t)0xFFFE)  /* 地址位0清零 */

/* I2C 自身地址寄存器2 (OADDR2) 位掩码定义 */
#define OADDR2_ENDUAL_Set        ((uint16_t)0x0001)  /* 双地址模式使能位(ENDUAL)置1 */
#define OADDR2_ENDUAL_Reset      ((uint16_t)0xFFFE)  /* 双地址模式使能位(ENDUAL)清零 */
#define OADDR2_ADD2_Reset        ((uint16_t)0xFF01)  /* 自身地址2位[7:1]清零掩码（保留位0作为ENDUAL） */

/* I2C 时钟配置寄存器 (CKCFGR) 位掩码定义 */
#define CKCFGR_FS_Set            ((uint16_t)0x8000)  /* I2C模式选择位(F/S)置1（快速模式） */
#define CKCFGR_CCR_Set           ((uint16_t)0x0FFF)  /* 时钟控制位(CCR[11:0])掩码 */

/* I2C 标志位掩码（用于事件组合） */
#define FLAG_Mask                ((uint32_t)0x00FFFFFF)  /* 只取低24位，组合STAR1和STAR2的低16位及STAR2的高16位？实际用低24位 */

/* I2C 中断使能掩码（用于检查中断是否使能） */
#define ITEN_Mask                ((uint32_t)0x07000000)  /* 高8位中的位[26:24]用于表示中断使能位 */

/*********************************************************************
 * @fn      I2C_DeInit
 *
 * @brief   将I2Cx外设寄存器复位为默认值。
 *
 * @param   I2Cx - 选择I2C外设，可以是I2C1或I2C2。
 *
 * @return  无
 */
void I2C_DeInit(I2C_TypeDef *I2Cx)
{
    if(I2Cx == I2C1)
    {
        /* 通过RCC外设复位I2C1 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
    }
    else
    {
        /* 通过RCC外设复位I2C2 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
    }
}

/*********************************************************************
 * @fn      I2C_Init
 *
 * @brief   根据I2C_InitStruct中的参数初始化I2Cx外设。
 *
 * @param   I2Cx - 选择I2C外设，可以是I2C1或I2C2。
 * @param   I2C_InitStruct - 指向I2C_InitTypeDef结构体的指针，包含I2C的配置信息。
 *
 * @return  无
 */
void I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct)
{
    uint16_t tmpreg = 0, freqrange = 0;
    uint16_t result = 0x04;  /* 用于存储计算出的CCR值，初始最小值 */
    uint32_t pclk1 = 8000000;  /* 默认PCLK1频率，后面会被实际值覆盖 */

    RCC_ClocksTypeDef rcc_clocks;

    /* 步骤1: 配置I2C外设时钟频率（FREQ[5:0]） */
    tmpreg = I2Cx->CTLR2;
    tmpreg &= CTLR2_FREQ_Reset;  /* 清除原来的FREQ位 */
    RCC_GetClocksFreq(&rcc_clocks);
    pclk1 = rcc_clocks.PCLK1_Frequency;  /* 获取APB1时钟频率（I2C挂载在APB1上） */
    freqrange = (uint16_t)(pclk1 / 1000000);  /* 计算MHz为单位的值（需要2~50MHz） */
    if(freqrange >= 60)  /* 限制最大值，I2C规范要求最高50MHz，这里设为60保险 */
    {
        freqrange = 60;
    }
    tmpreg |= freqrange;  /* 写入新的FREQ值 */
    I2Cx->CTLR2 = tmpreg;

    /* 步骤2: 先禁止I2C外设，因为配置时钟寄存器时需要PE=0（某些芯片要求） */
    I2Cx->CTLR1 &= CTLR1_PE_Reset;
    tmpreg = 0;

    /* 步骤3: 根据时钟速度计算CCR值并配置时钟控制寄存器CKCFGR */
    if(I2C_InitStruct->I2C_ClockSpeed <= 100000)  /* 标准模式 (<=100kHz) */
    {
        /* 标准模式公式: CCR = PCLK1 / (2 * ClockSpeed) */
        result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed << 1));

        if(result < 0x04)  /* CCR最小值不能小于4 */
        {
            result = 0x04;
        }

        tmpreg |= result;  /* 将CCR值放入低12位 */
        /* 标准模式下的上升时间寄存器值 = FREQ + 1（参考手册） */
        I2Cx->RTR = freqrange + 1;
    }
    else  /* 快速模式 (>100kHz) */
    {
        /* 根据占空比选择不同的公式 */
        if(I2C_InitStruct->I2C_DutyCycle == I2C_DutyCycle_2)
        {
            /* 占空比 Tlow/Thigh = 2 时，CCR = PCLK1 / (3 * ClockSpeed) */
            result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 3));
        }
        else  /* I2C_DutyCycle_16_9，占空比16/9 */
        {
            /* 占空比16/9时，CCR = PCLK1 / (25 * ClockSpeed) */
            result = (uint16_t)(pclk1 / (I2C_InitStruct->I2C_ClockSpeed * 25));
            result |= I2C_DutyCycle_16_9;  /* 设置DUTY位（位15）？注意：DUTY位在CKCFGR中是位14？标准库中I2C_DutyCycle_16_9宏可能已经包含了正确的位位置，这里直接OR即可 */
        }

        if((result & CKCFGR_CCR_Set) == 0)  /* 确保CCR值不为0（至少为1） */
        {
            result |= (uint16_t)0x0001;
        }

        /* 设置快速模式位F/S（位15），并将计算出的CCR值写入低12位 */
        tmpreg |= (uint16_t)(result | CKCFGR_FS_Set);
        /* 快速模式下的上升时间计算公式：FREQ * 300 / 1000 + 1（单位ns，参考手册） */
        I2Cx->RTR = (uint16_t)(((freqrange * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
    }

    /* 写入时钟配置寄存器 */
    I2Cx->CKCFGR = tmpreg;
    /* 使能I2C外设（PE=1） */
    I2Cx->CTLR1 |= CTLR1_PE_Set;

    /* 步骤4: 配置I2C模式（I2C或SMBus）和应答使能 */
    tmpreg = I2Cx->CTLR1;
    tmpreg &= CTLR1_CLEAR_Mask;  /* 清除SMBUS、ACK、ENGC、NOSTRETCH、ENPEC、ENARP等位 */
    /* 设置模式和应答位 */
    tmpreg |= (uint16_t)((uint32_t)I2C_InitStruct->I2C_Mode | I2C_InitStruct->I2C_Ack);
    I2Cx->CTLR1 = tmpreg;

    /* 步骤5: 配置自身地址1和地址模式（7位或10位） */
    I2Cx->OADDR1 = (I2C_InitStruct->I2C_AcknowledgedAddress | I2C_InitStruct->I2C_OwnAddress1);
}

/*********************************************************************
 * @fn      I2C_StructInit
 *
 * @brief   将I2C_InitStruct结构体每个成员初始化为默认值。
 *
 * @param   I2C_InitStruct - 指向I2C_InitTypeDef结构体的指针，将被初始化。
 *
 * @return  无
 */
void I2C_StructInit(I2C_InitTypeDef *I2C_InitStruct)
{
    I2C_InitStruct->I2C_ClockSpeed = 5000;               /* 默认时钟速度5kHz */
    I2C_InitStruct->I2C_Mode = I2C_Mode_I2C;             /* 默认I2C模式（非SMBus） */
    I2C_InitStruct->I2C_DutyCycle = I2C_DutyCycle_2;     /* 默认占空比2（快速模式时使用） */
    I2C_InitStruct->I2C_OwnAddress1 = 0;                 /* 默认自身地址0 */
    I2C_InitStruct->I2C_Ack = I2C_Ack_Disable;           /* 默认禁止应答 */
    I2C_InitStruct->I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; /* 默认7位地址模式 */
}

/*********************************************************************
 * @fn      I2C_Cmd
 *
 * @brief   使能或禁用指定的I2C外设。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_PE_Set;      /* 置位PE，使能I2C */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_PE_Reset;    /* 清零PE，禁用I2C */
    }
}

/*********************************************************************
 * @fn      I2C_DMACmd
 *
 * @brief   使能或禁用指定I2C的DMA请求。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void I2C_DMACmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR2 |= CTLR2_DMAEN_Set;   /* 使能DMA */
    }
    else
    {
        I2Cx->CTLR2 &= CTLR2_DMAEN_Reset; /* 禁止DMA */
    }
}

/*********************************************************************
 * @fn      I2C_DMALastTransferCmd
 *
 * @brief   指定下一次DMA传输是否为最后一次（用于产生NACK和STOP）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（最后一次）或 DISABLE。
 *
 * @return  无
 */
void I2C_DMALastTransferCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR2 |= CTLR2_LAST_Set;    /* 置位LAST，表示下次DMA传输是最后一次 */
    }
    else
    {
        I2Cx->CTLR2 &= CTLR2_LAST_Reset;  /* 清零LAST */
    }
}

/*********************************************************************
 * @fn      I2C_GenerateSTART
 *
 * @brief   产生I2C起始条件。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（产生起始条件）或 DISABLE（取消）。
 *
 * @return  无
 */
void I2C_GenerateSTART(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_START_Set;   /* 置位START，发送起始条件 */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_START_Reset; /* 清零START */
    }
}

/*********************************************************************
 * @fn      I2C_GenerateSTOP
 *
 * @brief   产生I2C停止条件。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（产生停止条件）或 DISABLE。
 *
 * @return  无
 */
void I2C_GenerateSTOP(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_STOP_Set;    /* 置位STOP，发送停止条件 */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_STOP_Reset;  /* 清零STOP */
    }
}

/*********************************************************************
 * @fn      I2C_AcknowledgeConfig
 *
 * @brief   使能或禁用I2C的应答功能（ACK）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（使能应答）或 DISABLE（禁止应答）。
 *
 * @return  无
 */
void I2C_AcknowledgeConfig(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ACK_Set;     /* 置位ACK，使能应答 */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ACK_Reset;   /* 清零ACK，禁止应答 */
    }
}

/*********************************************************************
 * @fn      I2C_OwnAddress2Config
 *
 * @brief   配置I2C的自身地址2（仅7位地址）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   Address - 指定的7位自身地址2（高7位有效，忽略最低位）。
 *
 * @return  无
 */
void I2C_OwnAddress2Config(I2C_TypeDef *I2Cx, uint8_t Address)
{
    uint16_t tmpreg = 0;

    tmpreg = I2Cx->OADDR2;
    tmpreg &= OADDR2_ADD2_Reset;          /* 清除原来的地址位[7:1]（保留ENDUAL位） */
    tmpreg |= (uint16_t)((uint16_t)Address & (uint16_t)0x00FE); /* 写入新的地址（只取高7位） */
    I2Cx->OADDR2 = tmpreg;
}

/*********************************************************************
 * @fn      I2C_DualAddressCmd
 *
 * @brief   使能或禁用I2C的双地址模式。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（使能双地址）或 DISABLE。
 *
 * @return  无
 */
void I2C_DualAddressCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->OADDR2 |= OADDR2_ENDUAL_Set;  /* 置位ENDUAL，使能双地址 */
    }
    else
    {
        I2Cx->OADDR2 &= OADDR2_ENDUAL_Reset; /* 清零ENDUAL */
    }
}

/*********************************************************************
 * @fn      I2C_GeneralCallCmd
 *
 * @brief   使能或禁用I2C的广播呼叫功能。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（使能广播呼叫）或 DISABLE。
 *
 * @return  无
 */
void I2C_GeneralCallCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ENGC_Set;     /* 置位ENGC，使能广播呼叫 */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ENGC_Reset;   /* 清零ENGC */
    }
}

/*********************************************************************
 * @fn      I2C_ITConfig
 *
 * @brief   使能或禁用指定的I2C中断。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_IT - 要配置的中断源，可以是以下值的组合：
 *            I2C_IT_BUF - 缓冲区中断（发送缓冲空/接收缓冲非空）
 *            I2C_IT_EVT - 事件中断（起始、地址、停止、传输完成等）
 *            I2C_IT_ERR - 错误中断（总线错误、仲裁丢失等）
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void I2C_ITConfig(I2C_TypeDef *I2Cx, uint16_t I2C_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR2 |= I2C_IT;             /* 使能中断 */
    }
    else
    {
        I2Cx->CTLR2 &= (uint16_t)~I2C_IT;  /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      I2C_SendData
 *
 * @brief   通过I2Cx外设发送一个字节数据。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   Data - 要发送的字节。
 *
 * @return  无
 */
void I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Data)
{
    I2Cx->DATAR = Data;  /* 写入数据寄存器 */
}

/*********************************************************************
 * @fn      I2C_ReceiveData
 *
 * @brief   返回I2Cx最近接收到的数据。
 *
 * @param   I2Cx - 选择I2C外设。
 *
 * @return  接收到的数据（8位）。
 */
uint8_t I2C_ReceiveData(I2C_TypeDef *I2Cx)
{
    return (uint8_t)I2Cx->DATAR;  /* 读取数据寄存器 */
}

/*********************************************************************
 * @fn      I2C_Send7bitAddress
 *
 * @brief   发送7位地址字节以选择从设备。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   Address - 要发送的从设备地址（7位，不需左移，函数会自动处理）。
 * @param   I2C_Direction - 数据传输方向：
 *            I2C_Direction_Transmitter - 发送模式（主机写）
 *            I2C_Direction_Receiver    - 接收模式（主机读）
 *
 * @return  无
 */
void I2C_Send7bitAddress(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t I2C_Direction)
{
    if(I2C_Direction != I2C_Direction_Transmitter)  /* 如果是接收模式 */
    {
        Address |= OADDR1_ADD0_Set;  /* 将地址最低位置1，表示读操作 */
    }
    else
    {
        Address &= OADDR1_ADD0_Reset; /* 最低位清零，表示写操作 */
    }

    I2Cx->DATAR = Address;  /* 将组合后的地址字节写入数据寄存器 */
}

/*********************************************************************
 * @fn      I2C_ReadRegister
 *
 * @brief   读取指定的I2C寄存器并返回其值。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_Register - 要读取的寄存器，可取以下值：
 *            I2C_Register_CTLR1
 *            I2C_Register_CTLR2
 *            I2C_Register_OADDR1
 *            I2C_Register_OADDR2
 *            I2C_Register_DATAR
 *            I2C_Register_STAR1
 *            I2C_Register_STAR2
 *            I2C_Register_CKCFGR
 *            I2C_Register_RTR
 *
 * @return  读取到的寄存器值（16位）。
 */
uint16_t I2C_ReadRegister(I2C_TypeDef *I2Cx, uint8_t I2C_Register)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)I2Cx;          /* 获取I2C基地址 */
    tmp += I2C_Register;           /* 加上寄存器偏移 */

    return (*(__IO uint16_t *)tmp); /* 读取该地址的16位值 */
}

/*********************************************************************
 * @fn      I2C_SoftwareResetCmd
 *
 * @brief   使能或禁用I2C软件复位。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（复位）或 DISABLE（释放复位）。
 *
 * @return  无
 */
void I2C_SoftwareResetCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_SWRST_Set;   /* 置位SWRST，复位I2C状态 */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_SWRST_Reset; /* 清零SWRST */
    }
}

/*********************************************************************
 * @fn      I2C_NACKPositionConfig
 *
 * @brief   选择主机接收模式下NACK的位置（用于控制何时发送NACK）。
 *          此函数与I2C_PECPositionConfig配置的是同一个位（POS），但用于普通I2C模式。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_NACKPosition - NACK位置：
 *            I2C_NACKPosition_Next    - 下一个字节是最后一个接收字节（NACK将在下一个字节后发送）
 *            I2C_NACKPosition_Current - 当前字节是最后一个接收字节（在当前字节后立即发送NACK）
 *
 * @return  无
 */
void I2C_NACKPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_NACKPosition)
{
    if(I2C_NACKPosition == I2C_NACKPosition_Next)
    {
        I2Cx->CTLR1 |= I2C_NACKPosition_Next;   /* 设置POS位，NACK位置为下一个 */
    }
    else
    {
        I2Cx->CTLR1 &= I2C_NACKPosition_Current; /* 清除POS位，NACK位置为当前 */
    }
}

/*********************************************************************
 * @fn      I2C_SMBusAlertConfig
 *
 * @brief   驱动SMBusAlert引脚为高或低（用于SMBus模式）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_SMBusAlert - 指定SMBAlert引脚电平：
 *            I2C_SMBusAlert_Low  - 引脚拉低
 *            I2C_SMBusAlert_High - 引脚拉高
 *
 * @return  无
 */
void I2C_SMBusAlertConfig(I2C_TypeDef *I2Cx, uint16_t I2C_SMBusAlert)
{
    if(I2C_SMBusAlert == I2C_SMBusAlert_Low)
    {
        I2Cx->CTLR1 |= I2C_SMBusAlert_Low;   /* 置位SMBALERT位，输出低电平 */
    }
    else
    {
        I2Cx->CTLR1 &= I2C_SMBusAlert_High;  /* 清零SMBALERT位，输出高电平 */
    }
}

/*********************************************************************
 * @fn      I2C_TransmitPEC
 *
 * @brief   使能或禁用PEC传输（发送PEC字节）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（使能PEC传输）或 DISABLE。
 *
 * @return  无
 */
void I2C_TransmitPEC(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_PEC_Set;       /* 置位PEC，使能PEC传输 */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_PEC_Reset;     /* 清零PEC */
    }
}

/*********************************************************************
 * @fn      I2C_PECPositionConfig
 *
 * @brief   选择PEC字节的位置（用于SMBus模式）。
 *          此函数与I2C_NACKPositionConfig配置的是同一个位（POS）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_PECPosition - PEC位置：
 *            I2C_PECPosition_Next    - 下一个字节是PEC
 *            I2C_PECPosition_Current - 当前字节是PEC
 *
 * @return  无
 */
void I2C_PECPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_PECPosition)
{
    if(I2C_PECPosition == I2C_PECPosition_Next)
    {
        I2Cx->CTLR1 |= I2C_PECPosition_Next;   /* 设置POS位 */
    }
    else
    {
        I2Cx->CTLR1 &= I2C_PECPosition_Current; /* 清除POS位 */
    }
}

/*********************************************************************
 * @fn      I2C_CalculatePEC
 *
 * @brief   使能或禁用PEC值计算（对传输的字节进行CRC计算）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（使能PEC计算）或 DISABLE。
 *
 * @return  无
 */
void I2C_CalculatePEC(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ENPEC_Set;      /* 置位ENPEC，使能PEC计算 */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ENPEC_Reset;    /* 清零ENPEC */
    }
}

/*********************************************************************
 * @fn      I2C_GetPEC
 *
 * @brief   返回计算出的PEC值。
 *
 * @param   I2Cx - 选择I2C外设。
 *
 * @return  PEC值（8位）。
 */
uint8_t I2C_GetPEC(I2C_TypeDef *I2Cx)
{
    return ((I2Cx->STAR2) >> 8);  /* STAR2寄存器的高8位存放PEC值 */
}

/*********************************************************************
 * @fn      I2C_ARPCmd
 *
 * @brief   使能或禁用SMBus的地址解析协议（ARP）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void I2C_ARPCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_ENARP_Set;      /* 置位ENARP，使能ARP */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_ENARP_Reset;    /* 清零ENARP */
    }
}

/*********************************************************************
 * @fn      I2C_StretchClockCmd
 *
 * @brief   使能或禁用I2C时钟延长功能（当从设备需要更多时间处理数据时）。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   NewState - ENABLE（使能时钟延长）或 DISABLE（禁止时钟延长）。
 *          注意：参数含义与函数名相反：NewState == DISABLE 时，禁止时钟延长（即允许延长？）
 *                实际代码：DISABLE -> 置位NOSTRETCH，禁止时钟延长。所以调用时要注意。
 *
 * @return  无
 */
void I2C_StretchClockCmd(I2C_TypeDef *I2Cx, FunctionalState NewState)
{
    if(NewState == DISABLE)
    {
        I2Cx->CTLR1 |= CTLR1_NOSTRETCH_Set;   /* 禁止时钟延长（NOSTRETCH=1） */
    }
    else
    {
        I2Cx->CTLR1 &= CTLR1_NOSTRETCH_Reset; /* 使能时钟延长（NOSTRETCH=0） */
    }
}

/*********************************************************************
 * @fn      I2C_FastModeDutyCycleConfig
 *
 * @brief   配置快速模式的占空比。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_DutyCycle - 占空比：
 *            I2C_DutyCycle_2    - Tlow/Thigh = 2
 *            I2C_DutyCycle_16_9 - Tlow/Thigh = 16/9
 *
 * @return  无
 */
void I2C_FastModeDutyCycleConfig(I2C_TypeDef *I2Cx, uint16_t I2C_DutyCycle)
{
    if(I2C_DutyCycle != I2C_DutyCycle_16_9)
    {
        I2Cx->CKCFGR &= I2C_DutyCycle_2;      /* 清除DUTY位，选择占空比2 */
    }
    else
    {
        I2Cx->CKCFGR |= I2C_DutyCycle_16_9;   /* 置位DUTY位，选择占空比16/9 */
    }
}

/*********************************************************************
 * @fn      I2C_CheckEvent
 *
 * @brief   检查最后一次I2C事件是否等于参数中指定的事件。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_EVENT - 要检查的事件组合（由STAR1和STAR2位组合而成）。
 *
 * @return  ErrorStatus - READY（事件发生）或 NoREADY（未发生）。
 */
ErrorStatus I2C_CheckEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
    uint32_t    lastevent = 0;
    uint32_t    flag1 = 0, flag2 = 0;
    ErrorStatus status = NoREADY;

    flag1 = I2Cx->STAR1;                /* 读取状态寄存器1 */
    flag2 = I2Cx->STAR2;                /* 读取状态寄存器2 */
    flag2 = flag2 << 16;                 /* 将STAR2左移16位，以便与STAR1组合成32位 */

    lastevent = (flag1 | flag2) & FLAG_Mask; /* 组合并掩码低24位（事件相关位） */

    if((lastevent & I2C_EVENT) == I2C_EVENT) /* 检查所需事件位是否全部置位 */
    {
        status = READY;
    }
    else
    {
        status = NoREADY;
    }

    return status;
}

/*********************************************************************
 * @fn      I2C_GetLastEvent
 *
 * @brief   返回最后一次I2C事件的值。
 *
 * @param   I2Cx - 选择I2C外设。
 *
 * @return  最后一次事件值（32位，低24位有效）。
 */
uint32_t I2C_GetLastEvent(I2C_TypeDef *I2Cx)
{
    uint32_t lastevent = 0;
    uint32_t flag1 = 0, flag2 = 0;

    flag1 = I2Cx->STAR1;
    flag2 = I2Cx->STAR2;
    flag2 = flag2 << 16;
    lastevent = (flag1 | flag2) & FLAG_Mask;

    return lastevent;
}

/*********************************************************************
 * @fn      I2C_GetFlagStatus
 *
 * @brief   检查指定的I2C标志是否置位。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_FLAG - 要检查的标志，可以是STAR1或STAR2中的位。
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG)
{
    FlagStatus    bitstatus = RESET;
    __IO uint32_t i2creg = 0, i2cxbase = 0;

    i2cxbase = (uint32_t)I2Cx;               /* 获取I2C基地址 */
    i2creg = I2C_FLAG >> 28;                  /* 高4位用于选择寄存器：1->STAR2（偏移0x18），0->STAR1（偏移0x14） */
    I2C_FLAG &= FLAG_Mask;                     /* 清除高8位，只保留低24位（但实际标志位在低16位，保留低16位即可） */

    if(i2creg != 0)                            /* 如果标志属于STAR2（高16位） */
    {
        i2cxbase += 0x14;                      /* STAR1地址偏移？注意：代码写的是0x14，但STAR1是0x14，STAR2是0x18。
                                                   这里逻辑：i2creg不为0时，地址加0x14？可能错误？但根据标准库，应该是：
                                                   if(i2creg != 0) i2cxbase += 0x18; else i2cxbase += 0x14; 
                                                   但这里代码是先判断i2creg，如果非0则加0x14，后面又有else分支加0x18。
                                                   可能是作者意图：i2creg为1时表示STAR2，加0x18。但代码写反了。
                                                   我们按原样注释，但指出可能有问题。 */
    }
    else
    {
        I2C_FLAG = (uint32_t)(I2C_FLAG >> 16);  /* 如果标志来自STAR1，则需要右移16位（因为传入的I2C_FLAG可能是低16位值） */
        i2cxbase += 0x18;                        /* STAR2地址？逻辑混乱，但原样保留 */
    }

    if(((*(__IO uint32_t *)i2cxbase) & I2C_FLAG) != (uint32_t)RESET)
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
 * @fn      I2C_ClearFlag
 *
 * @brief   清除I2C的挂起标志。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_FLAG - 要清除的标志（必须是可清除的，如错误标志）。
 *
 * @return  无
 */
void I2C_ClearFlag(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG)
{
    uint32_t flagpos = 0;

    flagpos = I2C_FLAG & FLAG_Mask;          /* 获取标志位位置 */
    I2Cx->STAR1 = (uint16_t)~flagpos;        /* 向STAR1的对应位写1（但此处取反后写入，实际上是清除？标准库中清除标志是通过写1到对应位，但这里写的是取反值，可能不正确？但通常这些标志通过读操作清除，这里可能是针对某些可写清除的标志。但根据注释，此函数用于清除可写清除的标志（如错误标志）。*/
}

/*********************************************************************
 * @fn      I2C_GetITStatus
 *
 * @brief   检查指定的I2C中断是否发生。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_IT - 要检查的中断源，可以是带中断使能位的标志组合。
 *
 * @return  ITStatus - SET 或 RESET。
 */
ITStatus I2C_GetITStatus(I2C_TypeDef *I2Cx, uint32_t I2C_IT)
{
    ITStatus bitstatus = RESET;
    uint32_t enablestatus = 0;

    enablestatus = (uint32_t)(((I2C_IT & ITEN_Mask) >> 16) & (I2Cx->CTLR2)); /* 提取中断使能位并与CTLR2中对应位比较 */
    I2C_IT &= FLAG_Mask;                                                     /* 提取标志位 */

    if(((I2Cx->STAR1 & I2C_IT) != (uint32_t)RESET) && enablestatus)          /* 标志置位且中断使能 */
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
 * @fn      I2C_ClearITPendingBit
 *
 * @brief   清除I2C的中断挂起位。
 *
 * @param   I2Cx - 选择I2C外设。
 * @param   I2C_IT - 要清除的中断源（可清除的标志位）。
 *
 * @return  无
 */
void I2C_ClearITPendingBit(I2C_TypeDef *I2Cx, uint32_t I2C_IT)
{
    uint32_t flagpos = 0;

    flagpos = I2C_IT & FLAG_Mask;            /* 获取标志位位置 */
    I2Cx->STAR1 = (uint16_t)~flagpos;        /* 写入取反值以清除标志（同ClearFlag） */
}