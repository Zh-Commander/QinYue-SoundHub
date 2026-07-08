/********************************** (C) COPYRIGHT *******************************
* 文件名          : ch32v30x_rtc.c
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2021/06/06
* 描述            : 本文件提供所有RTC固件功能函数
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及其二进制文件用于南京沁恒微电子制造的微控制器。
*******************************************************************************/

#include "ch32v30x_rtc.h"

/* RTC私有定义 */
#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF)  /* RTC低16位掩码，用于提取计数器/分频器的低16位值 */
#define PRLH_MSB_MASK    ((uint32_t)0x000F0000)  /* RTC预分频器高4位掩码，预分频器共20位，高4位在PSCRH寄存器中 */

/*********************************************************************
 * @fn      RTC_ITConfig
 *
 * @brief   使能或失能指定的RTC中断
 *
 * @param   RTC_IT - 指定要使能或失能的RTC中断源
 *            RTC_IT_OW  - 溢出中断
 *            RTC_IT_ALR - 闹钟中断
 *            RTC_IT_SEC - 秒中断
 * @param   NewState - 指定RTC中断的新状态（ENABLE或DISABLE）
 *
 * @return  无
 */
void RTC_ITConfig(uint16_t RTC_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RTC->CTLRH |= RTC_IT;  /* 使能中断: 置位CTLRH寄存器中的相应中断使能位 */
    }
    else
    {
        RTC->CTLRH &= (uint16_t)~RTC_IT;  /* 失能中断: 清零CTLRH寄存器中的相应中断使能位 */
    }
}

/*********************************************************************
 * @fn      RTC_EnterConfigMode
 *
 * @brief   进入RTC配置模式
 * @note    在修改RTC寄存器（如计数器、预分频器、闹钟值）前必须进入配置模式
 *
 * @return  无
 */
void RTC_EnterConfigMode(void)
{
    RTC->CTLRL |= RTC_CTLRL_CNF;  /* 设置CNF位，进入配置模式 */
}

/*********************************************************************
 * @fn      RTC_ExitConfigMode
 *
 * @brief   退出RTC配置模式
 * @note    配置完成后必须退出配置模式，RTC才会使用新的配置值
 *
 * @return  无
 */
void RTC_ExitConfigMode(void)
{
    RTC->CTLRL &= (uint16_t) ~((uint16_t)RTC_CTLRL_CNF);  /* 清零CNF位，退出配置模式 */
}

/*********************************************************************
 * @fn      RTC_GetCounter
 *
 * @brief   获取RTC计数器的当前值
 * @note    RTC计数器是一个32位寄存器，分为两个16位的CNTH和CNTL
 *          为防止读取过程中计数器变化导致数据不一致，采用连续读取并验证的方法
 *
 * @return  RTC计数器的32位值
 */
uint32_t RTC_GetCounter(void)
{
    uint16_t high1a = 0, high1b = 0, high2a = 0, high2b = 0;  /* 用于保存两次读取的高16位值 */
    uint16_t low1 = 0, low2 = 0;                              /* 用于保存两次读取的低16位值 */

    /* 连续读取高字节直到两次读取结果一致，确保读取稳定 */
    do{
        high1a = RTC->CNTH;
        high1b = RTC->CNTH;
    }while( high1a != high1b );

    /* 连续读取低字节直到两次读取结果一致，确保读取稳定 */
    do{
        low1 = RTC->CNTL;
        low2 = RTC->CNTL;
    }while( low1 != low2 );

    /* 再次读取高字节，检查是否在读取低字节期间高字节发生变化 */
    do{
        high2a = RTC->CNTH;
        high2b = RTC->CNTH;
    }while( high2a != high2b );

    /* 如果第一次和第二次读取的高字节不同，说明在读取过程中计数器发生了变化，需要重新读取低字节 */
    if(high1b != high2b)
    {
        do{
            low1 = RTC->CNTL;
            low2 = RTC->CNTL;
        }while( low1 != low2 );
    }

    /* 将高16位和低16位组合成32位计数值并返回 */
    return (((uint32_t)high2b << 16) | low2);
}

/*********************************************************************
 * @fn      RTC_SetCounter
 *
 * @brief   设置RTC计数器的值
 * @note    必须先进入配置模式才能写入计数器值
 *
 * @param   CounterValue - RTC计数器的新值（32位）
 *
 * @return  无
 */
void RTC_SetCounter(uint32_t CounterValue)
{
    RTC_EnterConfigMode();                     /* 进入配置模式 */
    RTC->CNTH = CounterValue >> 16;            /* 写入高16位 */
    RTC->CNTL = (CounterValue & RTC_LSB_MASK); /* 写入低16位（使用掩码确保只取低16位） */
    RTC_ExitConfigMode();                      /* 退出配置模式 */
}

/*********************************************************************
 * @fn      RTC_SetPrescaler
 *
 * @brief   设置RTC预分频器的值
 * @note    RTC预分频器共20位，用于对RTC时钟源进行分频，产生1Hz的时钟用于计数器递增
 *          必须先进入配置模式才能写入预分频器值
 *
 * @param   PrescalerValue - RTC预分频器的新值（20位有效，实际是32位变量）
 *
 * @return  无
 */
void RTC_SetPrescaler(uint32_t PrescalerValue)
{
    RTC_EnterConfigMode();                                    /* 进入配置模式 */
    RTC->PSCRH = (PrescalerValue & PRLH_MSB_MASK) >> 16;      /* 写入高4位（只取[19:16]位） */
    RTC->PSCRL = (PrescalerValue & RTC_LSB_MASK);             /* 写入低16位（[15:0]位） */
    RTC_ExitConfigMode();                                     /* 退出配置模式 */
}

/*********************************************************************
 * @fn      RTC_SetAlarm
 *
 * @brief   设置RTC闹钟值
 * @note    当计数器值达到闹钟值时，如果使能了闹钟中断，则会产生中断
 *          必须先进入配置模式才能写入闹钟值
 *
 * @param   AlarmValue - RTC闹钟的新值（32位）
 *
 * @return  无
 */
void RTC_SetAlarm(uint32_t AlarmValue)
{
    RTC_EnterConfigMode();                     /* 进入配置模式 */
    RTC->ALRMH = AlarmValue >> 16;             /* 写入闹钟值的高16位 */
    RTC->ALRML = (AlarmValue & RTC_LSB_MASK);  /* 写入闹钟值的低16位 */
    RTC_ExitConfigMode();                      /* 退出配置模式 */
}

/*********************************************************************
 * @fn      RTC_GetDivider
 *
 * @brief   获取RTC分频器的当前值
 * @note    RTC分频器是一个20位递减计数器，用于产生RTC时钟（通常为1Hz）
 *          读取方法与RTC_GetCounter类似，需要防止读取过程中值变化
 *
 * @return  RTC分频器的20位值（实际返回32位，但高12位为0）
 */
uint32_t RTC_GetDivider(void)
{
    uint16_t high1a = 0, high1b = 0, high2a = 0, high2b = 0;  /* 用于保存两次读取的高4位值（实际在DIVH的低4位） */
    uint16_t low1 = 0, low2 = 0;                              /* 用于保存两次读取的低16位值 */

    /* 连续读取高字节直到两次读取结果一致 */
    do{
        high1a = RTC->DIVH;
        high1b = RTC->DIVH;
    }while( high1a != high1b );

    /* 连续读取低字节直到两次读取结果一致 */
    do{
        low1 = RTC->DIVL;
        low2 = RTC->DIVL;
    }while( low1 != low2 );

    /* 再次读取高字节，检查是否在读取低字节期间高字节发生变化 */
    do{
        high2a = RTC->DIVH;
        high2b = RTC->DIVH;
    }while( high2a != high2b );

    /* 如果第一次和第二次读取的高字节不同，说明在读取过程中分频器发生了变化，需要重新读取低字节 */
    if(high1b != high2b)
    {
        do{
            low1 = RTC->DIVL;
            low2 = RTC->DIVL;
        }while( low1 != low2 );
    }

    /* 将高4位（实际只取低4位）和低16位组合成20位分频器值并返回 */
    return ((((uint32_t)high2b & (uint32_t)0x000F) << 16) | low2);
}

/*********************************************************************
 * @fn      RTC_WaitForLastTask
 *
 * @brief   等待上一次对RTC寄存器的写操作完成
 * @note    在对RTC寄存器进行任何写操作之前，必须调用此函数确保上一次写操作已完成
 *          通过检查RTOFF（RTC Operation OFF）标志位来判断是否可写
 *
 * @return  无
 */
void RTC_WaitForLastTask(void)
{
    /* 轮询RTOFF标志位，当该位为1时表示RTC寄存器可写（上一次操作已完成） */
    while((RTC->CTLRL & RTC_FLAG_RTOFF) == (uint16_t)RESET)
    {
    }
}

/*********************************************************************
 * @fn      RTC_WaitForSynchro
 *
 * @brief   等待RTC寄存器与APB时钟同步
 * @note    在APB复位或APB时钟停止后，进行任何读操作之前必须调用此函数
 *          通过清除并等待RSF（Registers Synchronized Flag）标志位来确保同步完成
 *
 * @return  无
 */
void RTC_WaitForSynchro(void)
{
    RTC->CTLRL &= (uint16_t)~RTC_FLAG_RSF;  /* 清除RSF标志位 */
    /* 等待RSF标志位被硬件置1，表示同步完成 */
    while((RTC->CTLRL & RTC_FLAG_RSF) == (uint16_t)RESET)
    {
    }
}

/*********************************************************************
 * @fn      RTC_GetFlagStatus
 *
 * @brief   检查指定的RTC标志位是否被置位
 *
 * @param   RTC_FLAG - 指定要检查的标志位
 *            RTC_FLAG_RTOFF - RTC操作关闭标志（指示RTC寄存器是否可写）
 *            RTC_FLAG_RSF   - 寄存器同步标志
 *            RTC_FLAG_OW    - 溢出标志
 *            RTC_FLAG_ALR   - 闹钟标志
 *            RTC_FLAG_SEC   - 秒标志
 *
 * @return  标志位的新状态（SET或RESET）
 */
FlagStatus RTC_GetFlagStatus(uint16_t RTC_FLAG)
{
    FlagStatus bitstatus = RESET;  /* 默认状态为RESET */
    
    /* 检查CTLRL寄存器中对应标志位是否被置位 */
    if((RTC->CTLRL & RTC_FLAG) != (uint16_t)RESET)
    {
        bitstatus = SET;  /* 标志位置位 */
    }
    else
    {
        bitstatus = RESET;  /* 标志位未置位 */
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      RTC_ClearFlag
 *
 * @brief   清除RTC的挂起标志位
 * @note    只能清除RSF、OW、ALR和SEC标志位，RTOFF标志位为只读，不能清除
 *
 * @param   RTC_FLAG - 指定要清除的标志位
 *            RTC_FLAG_RSF - 寄存器同步标志
 *            RTC_FLAG_OW  - 溢出标志
 *            RTC_FLAG_ALR - 闹钟标志
 *            RTC_FLAG_SEC - 秒标志
 *
 * @return  无
 */
void RTC_ClearFlag(uint16_t RTC_FLAG)
{
    /* 通过写0清除CTLRL寄存器中对应的标志位 */
    RTC->CTLRL &= (uint16_t)~RTC_FLAG;
}

/*********************************************************************
 * @fn      RTC_GetITStatus
 *
 * @brief   检查指定的RTC中断是否发生
 * @note    中断状态需要同时检查CTLRH（中断使能）和CTLRL（中断标志）寄存器
 *
 * @param   RTC_IT - 指定要检查的RTC中断源
 *            RTC_IT_OW  - 溢出中断
 *            RTC_IT_ALR - 闹钟中断
 *            RTC_IT_SEC - 秒中断
 *
 * @return  中断的新状态（SET或RESET）
 */
ITStatus RTC_GetITStatus(uint16_t RTC_IT)
{
    ITStatus bitstatus = RESET;  /* 默认状态为RESET */

    /* 先检查中断标志位是否置位（在CTLRL寄存器中） */
    bitstatus = (ITStatus)(RTC->CTLRL & RTC_IT);
    /* 再检查中断是否使能（在CTLRH寄存器中）且标志位置位，两者都满足才表示中断发生 */
    if(((RTC->CTLRH & RTC_IT) != (uint16_t)RESET) && (bitstatus != (uint16_t)RESET))
    {
        bitstatus = SET;  /* 中断发生 */
    }
    else
    {
        bitstatus = RESET;  /* 中断未发生 */
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      RTC_ClearITPendingBit
 *
 * @brief   清除RTC的中断挂起位
 * @note    实际上是通过清除中断标志位来实现的
 *
 * @param   RTC_IT - 指定要清除的中断挂起位
 *            RTC_IT_OW  - 溢出中断
 *            RTC_IT_ALR - 闹钟中断
 *            RTC_IT_SEC - 秒中断
 *
 * @return  无
 */
void RTC_ClearITPendingBit(uint16_t RTC_IT)
{
    /* 通过清除CTLRL寄存器中的中断标志位来清除中断挂起状态 */
    RTC->CTLRL &= (uint16_t)~RTC_IT;
}
