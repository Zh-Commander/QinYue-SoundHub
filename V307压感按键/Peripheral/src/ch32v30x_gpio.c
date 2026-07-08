/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_gpio.c
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2024/05/06
* 描述            : 本文件提供了所有GPIO固件函数
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意：本软件（修改或未修改）及二进制文件仅用于南京沁恒微电子生产的微控制器。
*******************************************************************************/
#include "ch32v30x_gpio.h"
#include "ch32v30x_rcc.h"

/* 位掩码定义 */
#define ECR_PORTPINCONFIG_MASK    ((uint16_t)0xFF80)  /* 事件控制寄存器端口引脚配置掩码 */
#define LSB_MASK                  ((uint16_t)0xFFFF)  /* 低16位掩码 */
#define DBGAFR_POSITION_MASK      ((uint32_t)0x000F0000)  /* 调试AFR位置掩码 */
#define DBGAFR_SWJCFG_MASK        ((uint32_t)0xF0FFFFFF)  /* 调试AFR SWJ配置掩码 */
#define DBGAFR_LOCATION_MASK      ((uint32_t)0x00200000)  /* 调试AFR位置掩码 */
#define DBGAFR_NUMBITS_MASK       ((uint32_t)0x00100000)  /* 调试AFR位数掩码 */

/*********************************************************************
 * @fn      GPIO_DeInit
 *
 * @brief   将GPIOx外设寄存器初始化为默认复位值
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 *
 * @return  无
 */
void GPIO_DeInit(GPIO_TypeDef *GPIOx)
{
    // 根据不同的GPIO端口执行复位操作
    if(GPIOx == GPIOA)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA复位
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE); // 禁用GPIOA复位
    }
    else if(GPIOx == GPIOB)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOB, DISABLE);
    }
    else if(GPIOx == GPIOC)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOC, DISABLE);
    }
    else if(GPIOx == GPIOD)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOD, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOD, DISABLE);
    }
    else if(GPIOx == GPIOE)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOE, DISABLE);
    }
}

/*********************************************************************
 * @fn      GPIO_AFIODeInit
 *
 * @brief   将复用功能（重映射、事件控制和EXTI配置）寄存器初始化为默认复位值
 *
 * @return  无
 */
void GPIO_AFIODeInit(void)
{
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, ENABLE);  // 使能AFIO复位
    RCC_APB2PeriphResetCmd(RCC_APB2Periph_AFIO, DISABLE); // 禁用AFIO复位
}

/*********************************************************************
 * @fn      GPIO_Init
 *
 * @brief   根据GPIO_InitStruct中的指定参数初始化GPIOx外设
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_InitStruct - 指向GPIO_InitTypeDef结构体的指针，该结构体包含指定GPIO外设的配置信息
 *
 * @return  无
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
{
    uint32_t currentmode = 0x00, currentpin = 0x00, pinpos = 0x00, pos = 0x00;
    uint32_t tmpreg = 0x00, pinmask = 0x00;

    // 获取模式位（低4位）
    currentmode = ((uint32_t)GPIO_InitStruct->GPIO_Mode) & ((uint32_t)0x0F);

    // 如果模式包含速度位（第4位为1），则将速度加入模式配置
    if((((uint32_t)GPIO_InitStruct->GPIO_Mode) & ((uint32_t)0x10)) != 0x00)
    {
        currentmode |= (uint32_t)GPIO_InitStruct->GPIO_Speed;
    }

    // 配置低8位引脚（引脚0-7）
    if(((uint32_t)GPIO_InitStruct->GPIO_Pin & ((uint32_t)0x00FF)) != 0x00)
    {
        tmpreg = GPIOx->CFGLR;  // 获取当前配置低寄存器值

        // 遍历低8位引脚
        for(pinpos = 0x00; pinpos < 0x08; pinpos++)
        {
            pos = ((uint32_t)0x01) << pinpos;  // 计算引脚位置掩码
            currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;  // 检查当前引脚是否被选中

            // 如果当前引脚在要配置的引脚列表中
            if(currentpin == pos)
            {
                pos = pinpos << 2;  // 每个引脚配置占4位
                pinmask = ((uint32_t)0x0F) << pos;  // 创建引脚配置掩码
                tmpreg &= ~pinmask;  // 清除原配置
                tmpreg |= (currentmode << pos);  // 写入新配置

                // 根据模式配置上拉/下拉
                if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPD)  // 下拉输入
                {
                    GPIOx->BCR = (((uint32_t)0x01) << pinpos);  // 清除位（设置下拉）
                }
                else
                {
                    if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPU)  // 上拉输入
                    {
                        GPIOx->BSHR = (((uint32_t)0x01) << pinpos);  // 设置位（设置上拉）
                    }
                }
            }
        }
        GPIOx->CFGLR = tmpreg;  // 写回配置低寄存器
    }

    // 配置高8位引脚（引脚8-15）
    if(GPIO_InitStruct->GPIO_Pin > 0x00FF)
    {
        tmpreg = GPIOx->CFGHR;  // 获取当前配置高寄存器值

        // 遍历高8位引脚
        for(pinpos = 0x00; pinpos < 0x08; pinpos++)
        {
            pos = (((uint32_t)0x01) << (pinpos + 0x08));  // 计算引脚位置掩码（引脚8-15）
            currentpin = ((GPIO_InitStruct->GPIO_Pin) & pos);  // 检查当前引脚是否被选中

            // 如果当前引脚在要配置的引脚列表中
            if(currentpin == pos)
            {
                pos = pinpos << 2;  // 每个引脚配置占4位
                pinmask = ((uint32_t)0x0F) << pos;  // 创建引脚配置掩码
                tmpreg &= ~pinmask;  // 清除原配置
                tmpreg |= (currentmode << pos);  // 写入新配置

                // 根据模式配置上拉/下拉
                if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPD)  // 下拉输入
                {
                    GPIOx->BCR = (((uint32_t)0x01) << (pinpos + 0x08));  // 清除位（设置下拉）
                }

                if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPU)  // 上拉输入
                {
                    GPIOx->BSHR = (((uint32_t)0x01) << (pinpos + 0x08));  // 设置位（设置上拉）
                }
            }
        }
        GPIOx->CFGHR = tmpreg;  // 写回配置高寄存器
    }
}

/*********************************************************************
 * @fn      GPIO_StructInit
 *
 * @brief   将GPIO_InitStruct的每个成员初始化为默认值
 *
 * @param   GPIO_InitStruct - 指向要初始化的GPIO_InitTypeDef结构体的指针
 *
 * @return  无
 */
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct)
{
    GPIO_InitStruct->GPIO_Pin = GPIO_Pin_All;  // 所有引脚
    GPIO_InitStruct->GPIO_Speed = GPIO_Speed_2MHz;  // 2MHz速度
    GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入模式
}

/*********************************************************************
 * @fn      GPIO_ReadInputDataBit
 *
 * @brief   读取指定GPIO输入数据端口位的值
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_Pin - 指定要读取的端口位，可以是GPIO_Pin_x（x为0-15）
 *
 * @return  输入端口引脚的值（Bit_SET或Bit_RESET）
 */
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint8_t bitstatus = 0x00;

    // 检查指定引脚是否被置位
    if((GPIOx->INDR & GPIO_Pin) != (uint32_t)Bit_RESET)
    {
        bitstatus = (uint8_t)Bit_SET;  // 引脚为高电平
    }
    else
    {
        bitstatus = (uint8_t)Bit_RESET;  // 引脚为低电平
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      GPIO_ReadInputData
 *
 * @brief   读取指定GPIO输入数据端口的值
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 *
 * @return  输入端口的值（16位）
 */
uint16_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx)
{
    return ((uint16_t)GPIOx->INDR);
}

/*********************************************************************
 * @fn      GPIO_ReadOutputDataBit
 *
 * @brief   读取指定GPIO输出数据端口位的值
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_Pin - 指定要读取的端口位，可以是GPIO_Pin_x（x为0-15）
 *
 * @return  输出端口引脚的值（Bit_SET或Bit_RESET）
 */
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint8_t bitstatus = 0x00;

    // 检查指定引脚是否被置位
    if((GPIOx->OUTDR & GPIO_Pin) != (uint32_t)Bit_RESET)
    {
        bitstatus = (uint8_t)Bit_SET;  // 引脚输出高电平
    }
    else
    {
        bitstatus = (uint8_t)Bit_RESET;  // 引脚输出低电平
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      GPIO_ReadOutputData
 *
 * @brief   读取指定GPIO输出数据端口的值
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 *
 * @return  GPIO输出端口的值（16位）
 */
uint16_t GPIO_ReadOutputData(GPIO_TypeDef *GPIOx)
{
    return ((uint16_t)GPIOx->OUTDR);
}

/*********************************************************************
 * @fn      GPIO_SetBits
 *
 * @brief   设置选定的数据端口位（置1）
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_Pin - 指定要设置的端口位，可以是GPIO_Pin_x（x为0-15）的任意组合
 *
 * @return  无
 */
void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BSHR = GPIO_Pin;  // 位设置寄存器
}

/*********************************************************************
 * @fn      GPIO_ResetBits
 *
 * @brief   清除选定的数据端口位（置0）
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_Pin - 指定要清除的端口位，可以是GPIO_Pin_x（x为0-15）的任意组合
 *
 * @return  无
 */
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BCR = GPIO_Pin;  // 位清除寄存器
}

/*********************************************************************
 * @fn      GPIO_WriteBit
 *
 * @brief   设置或清除选定的数据端口位
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_Pin - 指定要写入的端口位，可以是GPIO_Pin_x（x为0-15）
 * @param   BitVal - 指定要写入选定位的值：
 *                   Bit_RESET - 清除端口引脚（置0）
 *                   Bit_SET - 设置端口引脚（置1）
 *
 * @return  无
 */
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
    if(BitVal != Bit_RESET)  // 如果值不是复位（即要置1）
    {
        GPIOx->BSHR = GPIO_Pin;  // 设置位
    }
    else  // 如果要置0
    {
        GPIOx->BCR = GPIO_Pin;  // 清除位
    }
}

/*********************************************************************
 * @fn      GPIO_ToggleBit
 *
 * @brief   翻转选定的数据端口位（取反）
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_Pin - 指定要翻转的端口位，可以是GPIO_Pin_x（x为0-15）
 *
 * @return  无
 */
void GPIO_ToggleBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    // 读取当前输出位的值并取反，然后写入
    GPIO_WriteBit(GPIOx, GPIO_Pin, !GPIO_ReadOutputDataBit(GPIOx, GPIO_Pin));
}

/*********************************************************************
 * @fn      GPIO_Write
 *
 * @brief   向指定GPIO数据端口写入数据
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   PortVal - 指定要写入端口输出数据寄存器的值
 *
 * @return  无
 */
void GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t PortVal)
{
    GPIOx->OUTDR = PortVal;
}

/*********************************************************************
 * @fn      GPIO_PinLockConfig
 *
 * @brief   锁定GPIO引脚配置寄存器
 *
 * @param   GPIOx - 可以是GPIOA到GPIOG，用于选择GPIO外设
 * @param   GPIO_Pin - 指定要锁定的端口位，可以是GPIO_Pin_x（x为0-15）的任意组合
 *
 * @return  无
 */
void GPIO_PinLockConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint32_t tmp = 0x00010000;

    tmp |= GPIO_Pin;  // 设置LCKK位（第16位）并包含要锁定的引脚
    GPIOx->LCKR = tmp;  // 第一次写入
    GPIOx->LCKR = GPIO_Pin;  // 第二次写入
    GPIOx->LCKR = tmp;  // 第三次写入
    tmp = GPIOx->LCKR;  // 读取LCKR寄存器
    tmp = GPIOx->LCKR;  // 再次读取（用于验证锁定）
}

/*********************************************************************
 * @fn      GPIO_EventOutputConfig
 *
 * @brief   选择用作事件输出的GPIO引脚
 *
 * @param   GPIO_PortSource - 选择用作事件输出源的GPIO端口
 *                            可以是GPIO_PortSourceGPIOx（x为A-E）
 * @param   GPIO_PinSource - 指定事件输出的引脚
 *                           可以是GPIO_PinSourcex（x为0-15）
 *
 * @return  无
 */
void GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)
{
    uint32_t tmpreg = 0x00;

    tmpreg = AFIO->ECR;  // 获取事件控制寄存器当前值
    tmpreg &= ECR_PORTPINCONFIG_MASK;  // 清除端口和引脚配置位
    tmpreg |= (uint32_t)GPIO_PortSource << 0x04;  // 设置端口源
    tmpreg |= GPIO_PinSource;  // 设置引脚源
    AFIO->ECR = tmpreg;  // 写回事件控制寄存器
}

/*********************************************************************
 * @fn      GPIO_EventOutputCmd
 *
 * @brief   使能或禁用事件输出
 *
 * @param   NewState - ENABLE或DISABLE
 *
 * @return  无
 */
void GPIO_EventOutputCmd(FunctionalState NewState)
{
    if(NewState)  // 如果新状态为使能
    {
        AFIO->ECR |= (1 << 7);  // 设置EVOE位（第7位）使能事件输出
    }
    else  // 如果新状态为禁用
    {
        AFIO->ECR &= ~(1 << 7);  // 清除EVOE位（第7位）禁用事件输出
    }
}

/*********************************************************************
 * @fn      GPIO_PinRemapConfig
 *
 * @brief   更改指定引脚的映射
 *
 * @param   GPIO_Remap - 选择要重映射的引脚，具体选项见函数说明
 * @param   NewState - ENABLE或DISABLE
 *
 * @return  无
 */
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState)
{
    uint32_t tmp = 0x00, tmp1 = 0x00, tmpreg = 0x00, tmpmask = 0x00;

    // 根据最高位判断使用哪个重映射寄存器
    if((GPIO_Remap & 0x80000000) == 0x80000000)  // 最高位为1，使用PCFR2
    {
        tmpreg = AFIO->PCFR2;
    }
    else  // 最高位为0，使用PCFR1
    {
        tmpreg = AFIO->PCFR1;
    }

    tmpmask = (GPIO_Remap & DBGAFR_POSITION_MASK) >> 0x10;  // 获取调试AFR位置
    tmp = GPIO_Remap & LSB_MASK;  // 获取低16位

    /* 清除位 */
    if((GPIO_Remap & 0x80000000) == 0x80000000)  // PCFR2寄存器
    {
        if((GPIO_Remap & (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) == (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK))  // [31:16] 2位
        {
            tmp1 = ((uint32_t)0x03) << (tmpmask + 0x10);
            tmpreg &= ~tmp1;
        }
        else if((GPIO_Remap & DBGAFR_NUMBITS_MASK) == DBGAFR_NUMBITS_MASK)  // [15:0] 2位
        {
            tmp1 = ((uint32_t)0x03) << tmpmask;
            tmpreg &= ~tmp1;
        }
        else  // [31:0] 1位
        {
            tmpreg &= ~(tmp << (((GPIO_Remap & 0x7FFFFFFF)>> 0x15) * 0x10));
        }
    }
    else  // PCFR1寄存器
    {
        if((GPIO_Remap & (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) == (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK))  // [26:24] 3位 SWD_JTAG
        {
            tmpreg &= DBGAFR_SWJCFG_MASK;  // 清除SWJ配置位
            AFIO->PCFR1 &= DBGAFR_SWJCFG_MASK;
        }
        else if((GPIO_Remap & DBGAFR_NUMBITS_MASK) == DBGAFR_NUMBITS_MASK)  // [15:0] 2位
        {
            tmp1 = ((uint32_t)0x03) << tmpmask;
            tmpreg &= ~tmp1;
            tmpreg |= ~DBGAFR_SWJCFG_MASK;
        }
        else  // [31:0] 1位
        {
            tmpreg &= ~(tmp << ((GPIO_Remap >> 0x15) * 0x10));
            tmpreg |= ~DBGAFR_SWJCFG_MASK;
        }
    }

    /* 设置位 */
    if(NewState != DISABLE)  // 如果新状态为使能
    {
        tmpreg |= (tmp << (((GPIO_Remap & 0x7FFFFFFF)>> 0x15) * 0x10));  // 设置重映射位
    }

    // 根据寄存器选择写回相应的寄存器
    if((GPIO_Remap & 0x80000000) == 0x80000000)
    {
        AFIO->PCFR2 = tmpreg;
    }
    else
    {
        AFIO->PCFR1 = tmpreg;
    }
}

/*********************************************************************
 * @fn      GPIO_EXTILineConfig
 *
 * @brief   选择用作EXTI线的GPIO引脚
 *
 * @param   GPIO_PortSource - 选择用作EXTI线源的GPIO端口
 *                            可以是GPIO_PortSourceGPIOx（x为A-G）
 * @param   GPIO_PinSource - 指定要配置的EXTI线
 *                           可以是GPIO_PinSourcex（x为0-15）
 *
 * @return  无
 */
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)
{
    uint32_t tmp = 0x00;

    // 计算要清除的位掩码
    tmp = ((uint32_t)0x0F) << (0x04 * (GPIO_PinSource & (uint8_t)0x03));
    // 清除对应EXTI配置寄存器的位
    AFIO->EXTICR[GPIO_PinSource >> 0x02] &= ~tmp;
    // 设置新的EXTI配置
    AFIO->EXTICR[GPIO_PinSource >> 0x02] |= (((uint32_t)GPIO_PortSource) << (0x04 * (GPIO_PinSource & (uint8_t)0x03)));
}

/*********************************************************************
 * @fn      GPIO_ETH_MediaInterfaceConfig
 *
 * @brief   选择以太网媒体接口
 *
 * @param   GPIO_ETH_MediaInterface - 指定媒体接口模式：
 *                                     GPIO_ETH_MediaInterface_MII - MII模式
 *                                     GPIO_ETH_MediaInterface_RMII - RMII模式
 *
 * @return  无
 */
void GPIO_ETH_MediaInterfaceConfig(uint32_t GPIO_ETH_MediaInterface)
{
    if(GPIO_ETH_MediaInterface)  // 如果选择RMII模式
    {
        AFIO->PCFR1 |= (1 << 23);  // 设置第23位为1，选择RMII模式
    }
    else  // 如果选择MII模式
    {
        AFIO->PCFR1 &= ~(1 << 23);  // 清除第23位，选择MII模式
    }
}
/*********************************************************************
 * @fn      GPIO_IPD_Unused
 *
 * @brief   将未使用的GPIO配置为输入下拉（或上拉，根据具体芯片型号）
 *
 * @param   无
 *
 * @return  无
 */
void GPIO_IPD_Unused(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    uint32_t chip = 0;
    
    // 使能所有GPIO端口的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC|\
                           RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
    
    // 读取芯片ID以确定芯片型号
    chip =  *( uint32_t * )0x1FFFF704 & (~0x000000F0);
    
    // 根据芯片型号配置未使用的引脚
    switch(chip)
    {
#ifdef CH32V30x_D8  // D8系列芯片
        case 0x30330504:     // CH32V303CBT6
        {
            // 配置GPIOC的引脚0-12为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            
            // 配置GPIOD的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x30320504:     // CH32V303RBT6
        {
            // 配置GPIOD的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7|GPIO_Pin_8\
                                          |GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11|GPIO_Pin_12\
                                          |GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x30310504:     // CH32V303RCT6
        {
            // 配置GPIOD的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7|GPIO_Pin_8\
                                          |GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11|GPIO_Pin_12\
                                          |GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x30300504:     // CH32V303VCT6
        {        
            break;  // 该型号不需要特殊配置
        }
#elif defined  (CH32V30x_D8C)  // D8C系列芯片
        case 0x30520508:     // CH32V305FBP6
        {
            // 配置GPIOA的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2\
                                         |GPIO_Pin_3|GPIO_Pin_4\
                                         |GPIO_Pin_6|GPIO_Pin_7\
                                         |GPIO_Pin_10|GPIO_Pin_11\
                                         |GPIO_Pin_12|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            
            // 配置GPIOB的引脚2为下拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            
            // 配置GPIOB的其他引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                         |GPIO_Pin_3\
                                         |GPIO_Pin_4|GPIO_Pin_5\
                                         |GPIO_Pin_8|GPIO_Pin_9;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            
            // 配置GPIOC的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                         |GPIO_Pin_2|GPIO_Pin_3\
                                         |GPIO_Pin_4|GPIO_Pin_5\
                                         |GPIO_Pin_10|GPIO_Pin_11\
                                         |GPIO_Pin_12|GPIO_Pin_13\
                                         |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            
            // 配置GPIOD的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                         |GPIO_Pin_2|GPIO_Pin_3\
                                         |GPIO_Pin_4|GPIO_Pin_5\
                                         |GPIO_Pin_6|GPIO_Pin_7\
                                         |GPIO_Pin_8|GPIO_Pin_9\
                                         |GPIO_Pin_10|GPIO_Pin_11\
                                         |GPIO_Pin_12|GPIO_Pin_13\
                                         |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                         |GPIO_Pin_2|GPIO_Pin_3\
                                         |GPIO_Pin_4|GPIO_Pin_5\
                                         |GPIO_Pin_6|GPIO_Pin_7\
                                         |GPIO_Pin_8|GPIO_Pin_9\
                                         |GPIO_Pin_10|GPIO_Pin_11\
                                         |GPIO_Pin_12|GPIO_Pin_13\
                                         |GPIO_Pin_14|GPIO_Pin_15;

            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x305C0508:     // CH32V305CCT6
        {
            // 配置GPIOC的引脚7-15为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8\
                                         |GPIO_Pin_9|GPIO_Pin_10\
                                         |GPIO_Pin_11|GPIO_Pin_12\
                                         |GPIO_Pin_13|GPIO_Pin_14\
                                         |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            
            // 配置GPIOD的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                         |GPIO_Pin_2|GPIO_Pin_3\
                                         |GPIO_Pin_4|GPIO_Pin_5\
                                         |GPIO_Pin_6|GPIO_Pin_7\
                                         |GPIO_Pin_8|GPIO_Pin_9\
                                         |GPIO_Pin_10|GPIO_Pin_11\
                                         |GPIO_Pin_12|GPIO_Pin_13\
                                         |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                         |GPIO_Pin_2|GPIO_Pin_3\
                                         |GPIO_Pin_4|GPIO_Pin_5\
                                         |GPIO_Pin_6|GPIO_Pin_7\
                                         |GPIO_Pin_8|GPIO_Pin_9\
                                         |GPIO_Pin_10|GPIO_Pin_11\
                                         |GPIO_Pin_12|GPIO_Pin_13\
                                         |GPIO_Pin_14|GPIO_Pin_15;

            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x30500508:     // CH32V305RBT6
        {
            // 配置GPIOD的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7|GPIO_Pin_8\
                                          |GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11|GPIO_Pin_12\
                                          |GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x30710508:     // CH32V307RCT6
        {
            // 配置GPIOD的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7|GPIO_Pin_8\
                                          |GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11|GPIO_Pin_12\
                                          |GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的所有引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x30730508:     // CH32V307WCU6
        {
            // 配置GPIOD的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7|GPIO_Pin_10\
                                          |GPIO_Pin_11|GPIO_Pin_12\
                                          |GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7|GPIO_Pin_8\
                                          |GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11|GPIO_Pin_12\
                                          |GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x3173B508:     // CH32V317WCU6
        {
            // 配置GPIOC的引脚6-9为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7\
                                          |GPIO_Pin_8|GPIO_Pin_9;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            
            // 配置GPIOD的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的引脚1-6为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x3175B508:     // CH32V317TCU6
        {
            // 配置GPIOA的引脚9-10为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            
            // 配置GPIOB的引脚9-11为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            
            // 配置GPIOC的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7|GPIO_Pin_8\
                                          |GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11|GPIO_Pin_12\
                                          |GPIO_Pin_13;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            
            // 配置GPIOD的特定引脚为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9\
                                          |GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15\
                                          |GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6\
                                          |GPIO_Pin_7;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOD, &GPIO_InitStructure);
            
            // 配置GPIOE的引脚0-6为上拉输入
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2\
                                          |GPIO_Pin_3|GPIO_Pin_4\
                                          |GPIO_Pin_5|GPIO_Pin_6;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOE, &GPIO_InitStructure);
            break;
        }
        case 0x30700508:     // CH32V307VCT6
        {        
            break;  // 该型号不需要特殊配置
        }
        case 0x3170B508:     // CH32V317VCT6
        {        
            break;  // 该型号不需要特殊配置
        }
#endif
        default:
        {
            break;  // 默认情况不执行任何操作
        }
    }
}