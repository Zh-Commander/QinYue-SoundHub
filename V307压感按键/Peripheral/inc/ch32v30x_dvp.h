/********************************** (C) COPYRIGHT *******************************
* 文件名          : ch32v30x_dvp.h
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2021/06/06
* 描述            : 该文件包含DVP（数字视频端口）固件库的所有函数原型。
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或未修改）及二进制代码仅用于
*       南京沁恒微电子制造的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_DVP_H
#define __CH32V30x_DVP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* DVP 数据模式 */
typedef enum
{
 Video_Mode = 0,   /* 视频模式 */
 JPEG_Mode,        /* JPEG模式 */
}DVP_Data_ModeTypeDef;

/* DVP DMA 控制 */
typedef enum
{
 DVP_DMA_Disable = 0,  /* 禁用DMA */
 DVP_DMA_Enable,       /* 启用DMA */
}DVP_DMATypeDef;

/* DVP 标志位和FIFO复位控制 */
typedef enum
{
 DVP_FLAG_FIFO_RESET_Disable = 0,  /* 禁用标志位和FIFO复位 */
 DVP_FLAG_FIFO_RESET_Enable,       /* 启用标志位和FIFO复位 */
}DVP_FLAG_FIFO_RESETTypeDef;

/* DVP 接收复位控制 */
typedef enum
{
 DVP_RX_RESET_Disable = 0,  /* 禁用接收复位 */
 DVP_RX_RESET_Enable,       /* 启用接收复位 */
}DVP_RX_RESETTypeDef;

/* 函数声明部分 */

/**
  * @brief  配置DVP中断
  * @param  s : 中断状态（使能/禁用）
  * @param  i : 中断类型
  * @note   该函数用于使能或禁用特定DVP中断
  */
void DVP_INTCfg( uint8_t s,  uint8_t i );

/**
  * @brief  设置DVP数据模式
  * @param  s : 模式选择
  * @param  i : 数据模式类型（视频模式或JPEG模式）
  * @note   该函数用于配置DVP工作在视频模式或JPEG模式
  */
void DVP_Mode( uint8_t s,  DVP_Data_ModeTypeDef i);

/**
  * @brief  配置DVP基本参数
  * @param  s : DMA控制（使能/禁用）
  * @param  i : 标志位和FIFO复位控制
  * @param  j : 接收复位控制
  * @note   该函数用于配置DVP的DMA、标志位/FIFO复位和接收复位功能
  */
void DVP_Cfg( DVP_DMATypeDef s,  DVP_FLAG_FIFO_RESETTypeDef i, DVP_RX_RESETTypeDef j);

#ifdef __cplusplus
}
#endif

#endif
