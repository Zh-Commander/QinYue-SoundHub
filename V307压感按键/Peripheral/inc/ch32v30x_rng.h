/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_rng.h
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2021/06/06
* 描述            : 该文件包含了随机数生成器(RNG)固件库的所有函数原型
*********************************************************************************
* 版权所有 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及其二进制文件仅用于南京沁恒微电子制造的微控制器
*******************************************************************************/
#ifndef __CH32V30x_RNG_H
#define __CH32V30x_RNG_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "ch32v30x.h"

 /* RNG 标志位定义 */
#define RNG_FLAG_DRDY               ((uint8_t)0x0001) /* 数据准备就绪标志 */
#define RNG_FLAG_CECS               ((uint8_t)0x0002) /* 时钟错误当前状态标志 */
#define RNG_FLAG_SECS               ((uint8_t)0x0004) /* 种子错误当前状态标志 */

/* RNG 中断定义 */
#define RNG_IT_CEI                  ((uint8_t)0x20) /* 时钟错误中断 */
#define RNG_IT_SEI                  ((uint8_t)0x40) /* 种子错误中断 */


void RNG_Cmd(FunctionalState NewState); /* 使能或失能RNG外设 */
uint32_t RNG_GetRandomNumber(void); /* 获取随机数 */
void RNG_ITConfig(FunctionalState NewState); /* 使能或失能RNG中断 */
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG); /* 获取RNG标志位状态 */
void RNG_ClearFlag(uint8_t RNG_FLAG); /* 清除RNG标志位 */
ITStatus RNG_GetITStatus(uint8_t RNG_IT); /* 获取RNG中断状态 */
void RNG_ClearITPendingBit(uint8_t RNG_IT); /* 清除RNG中断挂起位 */

#ifdef __cplusplus
}
#endif

#endif
