/********************************** (C) COPYRIGHT *******************************
* 文件名           : ch32v30x_crc.h
* 作者             : WCH
* 版本             : V1.0.0
* 日期             : 2021/06/06
* 描述             : 本文件包含CRC固件库的所有函数原型。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）及二进制文件仅用于南京沁恒微电子生产的微控制器。
*******************************************************************************/
#ifndef __CH32V30x_CRC_H
#define __CH32V30x_CRC_H

#ifdef __cplusplus
 extern "C" {  // 如果是C++环境，使用C语言链接规范
#endif

#include "ch32v30x.h"  // 包含CH32V30x系列微控制器的通用头文件

/**
  * @brief  重置CRC数据寄存器为初始值（通常为0xFFFFFFFF）
  */
void CRC_ResetDR(void);

/**
  * @brief  计算单个32位数据的CRC值
  * @param  Data: 要计算CRC的32位数据
  * @retval 计算得到的32位CRC校验值
  */
uint32_t CRC_CalcCRC(uint32_t Data);

/**
  * @brief  计算一个数据块的CRC值
  * @param  pBuffer: 指向待计算数据块的指针（32位数组）
  * @param  BufferLength: 数据块的长度（32位数据的个数）
  * @retval 整个数据块计算得到的32位CRC校验值
  */
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength);

/**
  * @brief  获取当前的CRC值（直接从CRC数据寄存器读取）
  * @retval 当前CRC数据寄存器中的32位值
  */
uint32_t CRC_GetCRC(void);

/**
  * @brief  设置CRC独立数据寄存器（IDR）的值
  * @param  IDValue: 要写入IDR的8位值
  */
void CRC_SetIDRegister(uint8_t IDValue);

/**
  * @brief  获取CRC独立数据寄存器（IDR）的值
  * @retval 从IDR读取的8位值
  */
uint8_t CRC_GetIDRegister(void);

#ifdef __cplusplus
}
#endif

#endif /* __CH32V30x_CRC_H */
