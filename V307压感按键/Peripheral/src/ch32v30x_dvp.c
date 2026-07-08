/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_dvp.c
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 此文件提供了所有DVP（数字视频端口）固件函数。
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司。
* 注意: 本软件（无论修改与否）及其二进制文件用于
*       南京沁恒微电子制造的微控制器。
*******************************************************************************/
#include "ch32v30x_dvp.h"

/*********************************************************************
 * @fn      DVP_INTCfg

 * @brief   DVP中断配置

 * @param   s - 中断使能控制
 *             使能 (ENABLE)
 *             禁用 (DISABLE)
 *          i - 中断类型（可以是以下位的组合）
 *             RB_DVP_IE_STP_FRM - 停止帧中断使能位
 *             RB_DVP_IE_FIFO_OV - FIFO溢出中断使能位
 *             RB_DVP_IE_FRM_DONE - 帧完成中断使能位
 *             RB_DVP_IE_ROW_DONE - 行完成中断使能位
 *             RB_DVP_IE_STR_FRM - 开始帧中断使能位

 * @return  无
 */
void DVP_INTCfg(uint8_t s, uint8_t i)
{
    if(s) /* 如果 s 为真 (ENABLE) */
    {
        DVP->IER |= i; /* 设置中断使能寄存器(IER)的相应位，开启指定中断 */
    }
    else /* 如果 s 为假 (DISABLE) */
    {
        DVP->IER &= ~i; /* 清除中断使能寄存器(IER)的相应位，关闭指定中断 */
    }
}

/*********************************************************************
 * @fn      DVP_Mode

 * @brief   配置DVP工作模式

 * @param   s - 数据位宽模式选择
 *             RB_DVP_D8_MOD  - 8位数据模式
 *             RB_DVP_D10_MOD - 10位数据模式
 *             RB_DVP_D12_MOD - 12位数据模式
 *          i - 编码模式选择（Video_Mode 或 JPEG_Mode）
 *             Video_Mode - 视频（原始数据）模式
 *             JPEG_Mode  - JPEG编码模式

 * @return  无
 */
void DVP_Mode(uint8_t s, DVP_Data_ModeTypeDef i)
{
    DVP->CR0 &= ~RB_DVP_MSK_DAT_MOD; /* 先清除控制寄存器0(CR0)中的数据模式位 */

    if(s) /* 如果s不为0，即指定了有效的位宽模式 */
    {
        DVP->CR0 |= s; /* 设置CR0寄存器中的对应位，应用选择的位宽模式 */
    }
    else /* 如果s为0，则清除所有位宽模式位 */
    {
        DVP->CR0 &= ~(3 << 4); /* 清除数据模式相关的位（第4、5位） */
    }

    if(i) /* 如果i为真 (JPEG_Mode) */
    {
        DVP->CR0 |= RB_DVP_JPEG; /* 设置CR0寄存器中的JPEG模式位，启用JPEG编码 */
    }
    else /* 如果i为假 (Video_Mode) */
    {
        DVP->CR0 &= ~RB_DVP_JPEG; /* 清除CR0寄存器中的JPEG模式位，启用原始视频模式 */
    }
}

/*********************************************************************
 * @fn      DVP_Cfg

 * @brief   DVP基本功能配置

 * @param   s - DMA功能使能控制
 *            DVP_DMA_Enable  - 启用DMA传输
 *            DVP_DMA_Disable - 禁用DMA传输
 *          i - FIFO及标志位复位控制
 *            DVP_FLAG_FIFO_RESET_Enable  - 启用FIFO和标志位清零
 *            DVP_FLAG_FIFO_RESET_Disable - 禁用FIFO和标志位清零
 *          j - 接收器复位控制
 *            DVP_RX_RESET_Enable  - 启用接收器复位
 *            DVP_RX_RESET_Disable - 禁用接收器复位

 * @return  无
 */
void DVP_Cfg(DVP_DMATypeDef s, DVP_FLAG_FIFO_RESETTypeDef i, DVP_RX_RESETTypeDef j)
{
    switch(s) /* 根据参数 s 配置DMA */
    {
        case DVP_DMA_Enable:
            DVP->CR1 |= RB_DVP_DMA_EN; /* 设置控制寄存器1(CR1)的DMA使能位 */
            break;
        case DVP_DMA_Disable:
            DVP->CR1 &= ~RB_DVP_DMA_EN; /* 清除控制寄存器1(CR1)的DMA使能位 */
            break;
        default:
            break; /* 默认情况，不执行操作 */
    }

    switch(i) /* 根据参数 i 配置FIFO及标志位复位 */
    {
        case DVP_RX_RESET_Enable: /* 注意：此处参数名与枚举值可能不匹配，保持原代码逻辑。此case对应“全部清零”功能 */
            DVP->CR1 |= RB_DVP_ALL_CLR; /* 设置CR1寄存器的“全部清零”位，复位FIFO和状态标志 */
            break;
        case DVP_RX_RESET_Disable:
            DVP->CR1 &= ~RB_DVP_ALL_CLR; /* 清除CR1寄存器的“全部清零”位 */
            break;
        default:
            break;
    }

    switch(j) /* 根据参数 j 配置接收器复位 */
    {
        case DVP_RX_RESET_Enable:
            DVP->CR1 |= RB_DVP_RCV_CLR; /* 设置CR1寄存器的接收器清零位，复位接收器 */
            break;
        case DVP_RX_RESET_Disable:
            DVP->CR1 &= ~RB_DVP_RCV_CLR; /* 清除CR1寄存器的接收器清零位 */
            break;
        default:
            break;
    }
}