/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_dbgmcu.c
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2024/05/28
* 描述            : 该文件提供所有DBGMCU(调试MCU)固件功能函数
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及二进制代码用于南京沁恒微电子生产的微控制器
*******************************************************************************/

#include "ch32v30x_dbgmcu.h"  // 包含DBGMCU头文件

/* 定义掩码，用于提取设备ID的低16位 */
#define IDCODE_DEVID_MASK    ((uint32_t)0x0000FFFF)

/*********************************************************************
 * @函数名      DBGMCU_GetREVID
 *
 * @描述        获取设备修订标识符(芯片版本号)
 *
 * @返回值      设备修订标识符(低16位)
 */
uint32_t DBGMCU_GetREVID(void)
{
    /* 从固定地址0x1FFFF704读取芯片ID寄存器，并使用掩码提取低16位作为修订版本号 */
    return ((*(uint32_t *)0x1FFFF704) & IDCODE_DEVID_MASK);
}

/*********************************************************************
 * @函数名      DBGMCU_GetDEVID
 *
 * @描述        获取设备标识符(芯片型号ID)
 *
 * @返回值      设备标识符(高16位)
 */
uint32_t DBGMCU_GetDEVID(void)
{
    /* 从固定地址0x1FFFF704读取芯片ID寄存器，右移16位提取高16位作为设备ID */
    return ((*(uint32_t *)0x1FFFF704) >> 16);
}

/*********************************************************************
 * @函数名      __get_DEBUG_CR
 *
 * @描述        读取调试控制寄存器(DEBUG Control Register)
 *
 * @返回值      调试控制寄存器的当前值
 */
uint32_t __get_DEBUG_CR(void)
{
    uint32_t result;  // 用于存储读取结果的变量

    /* 使用RISC-V汇编指令csrr读取调试控制寄存器(地址0x7C0)到result变量 */
    __asm volatile("csrr %0,""0x7C0" : "=r"(result));
    return (result);
}

/*********************************************************************
 * @函数名      __set_DEBUG_CR
 *
 * @描述        设置调试控制寄存器(DEBUG Control Register)
 *
 * @参数        value  - 要设置的调试控制寄存器值
 *
 * @返回值      无
 */
void __set_DEBUG_CR(uint32_t value)
{
    /* 使用RISC-V汇编指令csrw将value值写入调试控制寄存器(地址0x7C0) */
    __asm volatile("csrw 0x7C0, %0" : : "r"(value));
}

/*********************************************************************
 * @函数名      DBGMCU_Config
 *
 * @描述        配置在调试模式下MCU的外设和低功耗模式行为
 *              当MCU处于调试暂停状态时，控制指定外设是否继续运行
 *
 * @参数        DBGMCU_Periph - 指定要配置的外设和低功耗模式，可以是以下值：
 *                DBGMCU_IWDG_STOP - 当内核暂停时，调试模式下停止独立看门狗
 *                DBGMCU_WWDG_STOP - 当内核暂停时，调试模式下停止窗口看门狗
 *                DBGMCU_TIM1_STOP - 当内核暂停时，调试模式下停止TIM1计数器
 *                DBGMCU_TIM2_STOP - 当内核暂停时，调试模式下停止TIM2计数器
 *          NewState - 使能或禁用该功能，取值为ENABLE或DISABLE
 *
 * @返回值      无
 */
void DBGMCU_Config(uint32_t DBGMCU_Periph, FunctionalState NewState)
{
    uint32_t val;  // 临时变量，用于存储当前调试控制寄存器的值

    if(NewState != DISABLE)
    {
        /* 使能功能：直接将配置值写入调试控制寄存器 */
        __set_DEBUG_CR(DBGMCU_Periph);
    }
    else
    {
        /* 禁用功能：
           1. 先读取当前调试控制寄存器的值
           2. 清除要禁用的位（通过按位取反后与运算）
           3. 将修改后的值写回寄存器 */
        val = __get_DEBUG_CR();                 // 获取当前值
        val &= ~(uint32_t)DBGMCU_Periph;        // 清除指定位
        __set_DEBUG_CR(val);                    // 写回修改后的值
    }
}

/*********************************************************************
 * @函数名      DBGMCU_GetCHIPID
 *
 * @描述        获取完整的芯片标识符
 *
 * @返回值      完整的芯片标识符（32位）
 *              芯片ID列表-
 *              CH32V303CBT6-0x303305x4
 *              CH32V303RBT6-0x303205x4
 *              CH32V303RCT6-0x303105x4
 *              CH32V303VCT6-0x303005x4
 *              CH32V305FBP6-0x305205x8
 *              CH32V305RBT6-0x305005x8
 *              CH32V305GBU6-0x305B05x8
 *              CH32V305CCT6-0x305C05x8
 *              CH32V307WCU6-0x307305x8
 *              CH32V307FBP6-0x307205x8
 *              CH32V307RCT6-0x307105x8
 *              CH32V307VCT6-0x307005x8
 *              CH32V317VCT6-0x3170B5X8
 *              CH32V317WCU6-0x3173B5X8
 *              CH32V317TCU6-0x3175B5X8
 */
uint32_t DBGMCU_GetCHIPID(void)
{
    /* 直接返回芯片ID寄存器(地址0x1FFFF704)的完整32位值 */
    return( *( uint32_t * )0x1FFFF704 );
}
