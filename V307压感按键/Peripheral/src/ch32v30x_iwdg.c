/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_iwdg.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2024/03/06
* 功能描述          : 本文件提供了独立看门狗(IWDG)的所有固件函数
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意：本软件（修改或未修改）及二进制文件仅适用于
*       南京沁恒微电子生产的微控制器
*******************************************************************************/
#include "ch32v30x_iwdg.h"

/* 控制寄存器(CTLR)位掩码定义 */
#define CTLR_KEY_Reload    ((uint16_t)0xAAAA)  /* 喂狗密钥：向CTLR写入0xAAAA可重新加载计数器的值 */
#define CTLR_KEY_Enable    ((uint16_t)0xCCCC)  /* 启用密钥：向CTLR写入0xCCCC可启动看门狗 */

/*********************************************************************
 * @函数名      IWDG_WriteAccessCmd
 *
 * @功能描述    启用或禁用对IWDG_PSCR（预分频器）和IWDG_RLDR（重装载值）寄存器的写访问权限
 *
 * @参数        IWDG_WriteAccess - 指定对IWDG_PSCR和IWDG_RLDR寄存器的写访问权限
 *            IWDG_WriteAccess_Enable  - 启用写访问权限
 *            IWDG_WriteAccess_Disable - 禁用写访问权限
 *            注意：写访问权限仅能在禁用看门狗时修改，启用看门狗后自动锁定
 *
 * @返回值      无
 */
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
    /* 将访问控制命令写入控制寄存器 */
    IWDG->CTLR = IWDG_WriteAccess;
}

/*********************************************************************
 * @函数名      IWDG_SetPrescaler
 *
 * @功能描述    设置IWDG预分频器值，决定看门狗时钟的分频系数
 *
 * @参数        IWDG_Prescaler - 指定预分频器值，可选值：
 *             IWDG_Prescaler_4   - 4分频   (最长超时时间)
 *             IWDG_Prescaler_8   - 8分频
 *             IWDG_Prescaler_16  - 16分频
 *             IWDG_Prescaler_32  - 32分频
 *             IWDG_Prescaler_64  - 64分频
 *             IWDG_Prescaler_128 - 128分频
 *             IWDG_Prescaler_256 - 256分频 (最短超时时间)
 *
 * @返回值      无
 *
 * @注意        1. 该函数必须在IWDG启用前调用
 *             2. 预分频器值越小，看门狗计数越慢，超时时间越长
 *             3. 需先调用IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable)开启写权限
 */
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
    /* 将预分频器值写入预分频寄存器(PSCR) */
    IWDG->PSCR = IWDG_Prescaler;
}

/*********************************************************************
 * @函数名      IWDG_SetReload
 *
 * @功能描述    设置IWDG重装载值，决定看门狗的计数初值和超时时间
 *
 * @参数        Reload - 指定重装载值（12位有效）
 *            该参数必须在0到0x0FFF（4095）之间
 *            计算公式：超时时间 = (Reload + 1) * (预分频值 / 看门狗时钟频率)
 *
 * @返回值      无
 *
 * @注意        1. 该函数必须在IWDG启用前调用
 *             2. 需先调用IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable)开启写权限
 */
void IWDG_SetReload(uint16_t Reload)
{
    /* 将重装载值写入重装载寄存器(RLDR) */
    IWDG->RLDR = Reload;
}

/*********************************************************************
 * @函数名      IWDG_ReloadCounter
 *
 * @功能描述    喂狗操作：将重装载寄存器的值重新加载到计数器，防止看门狗复位
 *
 * @返回值      无
 *
 * @注意        1. 必须在看门狗超时前周期调用此函数
 *             2. 该操作称为"喂狗"或"刷新看门狗"
 */
void IWDG_ReloadCounter(void)
{
    /* 向控制寄存器写入0xAAAA密钥，触发计数器重装载 */
    IWDG->CTLR = CTLR_KEY_Reload;
}

/*********************************************************************
 * @函数名      IWDG_Enable
 *
 * @功能描述    启用独立看门狗，启用后写权限自动禁用
 *
 * @返回值      无
 *
 * @注意        1. 启用后无法停止看门狗（除非系统复位）
 *             2. 启用前必须正确配置预分频器和重装载值
 *             3. 启用后应定期调用IWDG_ReloadCounter()喂狗
 */
void IWDG_Enable(void)
{
    /* 向控制寄存器写入0xCCCC密钥，启动看门狗 */
    IWDG->CTLR = CTLR_KEY_Enable;
    
    /* 等待看门狗启动完成（检查RCC的看门狗状态位） */
    while((RCC->RSTSCKR & 0x2)==RESET);
}

/*********************************************************************
 * @函数名      IWDG_GetFlagStatus
 *
 * @功能描述    检查指定的IWDG状态标志位是否置位
 *
 * @参数        IWDG_FLAG - 指定要检查的标志位：
 *            IWDG_FLAG_PVU - 预分频器更新中标志，为1表示预分频器正在更新
 *            IWDG_FLAG_RVU - 重装载值更新中标志，为1表示重装载值正在更新
 *
 * @返回值      FlagStatus - 标志位状态：
 *            SET   - 标志位置位（正在更新）
 *            RESET - 标志位复位（更新完成）
 *
 * @注意        1. 在更新预分频器或重装载值时，相应标志位会自动置位
 *             2. 更新完成后，硬件自动清除标志位
 *             3. 软件可通过查询标志位判断更新是否完成
 */
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* 检查状态寄存器(STATR)中指定标志位是否置位 */
    if((IWDG->STATR & IWDG_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;      /* 标志位置位 */
    }
    else
    {
        bitstatus = RESET;    /* 标志位复位 */
    }

    return bitstatus;
}