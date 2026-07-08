/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_crc.c
* 作者            : WCH
* 版本            : V1.0.0
* 日期            : 2021/06/06
* 描述            : 该文件提供了所有CRC固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（无论是否被修改）及其二进制文件仅供南京沁恒微电子制造的单片机使用。
*******************************************************************************/
#include "ch32v30x_crc.h"

/*********************************************************************
 * @fn      CRC_ResetDR
 *
 * @brief   重置CRC数据寄存器（DR）。
 *
 * @return  无
 */
void CRC_ResetDR(void)
{
    /* 将CRC控制寄存器（CTLR）的复位位（CRC_CTLR_RESET）置位，以复位CRC数据寄存器 */
    CRC->CTLR = CRC_CTLR_RESET;
}

/*********************************************************************
 * @fn      CRC_CalcCRC
 *
 * @brief   计算给定数据字（32位）的32位CRC值。
 *
 * @param   Data - 要计算CRC的数据字（32位）。
 *
 * @return  32位CRC值。
 */
uint32_t CRC_CalcCRC(uint32_t Data)
{
    /* 将数据写入CRC数据寄存器（DATAR），硬件自动计算CRC */
    CRC->DATAR = Data;

    /* 读取CRC数据寄存器，返回计算得到的CRC值 */
    return (CRC->DATAR);
}

/*********************************************************************
 * @fn      CRC_CalcBlockCRC
 *
 * @brief   计算给定数据字（32位）缓冲区的32位CRC值。
 *
 * @param   pBuffer - 指向包含要计算数据的缓冲区的指针。
 *          BufferLength - 要计算的缓冲区长度（以32位字为单位）。
 *
 * @return  32位CRC值。
 */
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength)
{
    uint32_t index = 0;

    /* 遍历缓冲区中的每个数据字，依次写入CRC数据寄存器进行CRC计算 */
    for(index = 0; index < BufferLength; index++)
    {
        CRC->DATAR = pBuffer[index];
    }

    /* 返回最后一次计算得到的CRC值（即整个数据块的CRC值） */
    return (CRC->DATAR);
}

/*********************************************************************
 * @fn      CRC_GetCRC
 *
 * @brief   返回当前CRC值（即CRC数据寄存器中的值）。
 *
 * @return  32位CRC值。
 */
uint32_t CRC_GetCRC(void)
{
    /* 直接读取CRC数据寄存器（DATAR）并返回其值 */
    return (CRC->DATAR);
}

/*********************************************************************
 * @fn      CRC_SetIDRegister
 *
 * @brief   在独立数据（ID）寄存器中存储一个8位数据。
 *          该寄存器可用于存储用户自定义的标识符，不影响CRC计算。
 *
 * @param   IDValue - 要存储在ID寄存器中的8位值。
 *
 * @return  无
 */
void CRC_SetIDRegister(uint8_t IDValue)
{
    /* 将8位数据写入CRC独立数据寄存器（IDATAR） */
    CRC->IDATAR = IDValue;
}

/*********************************************************************
 * @fn      CRC_GetIDRegister
 *
 * @brief   返回存储在独立数据（ID）寄存器中的8位数据。
 *
 * @return  ID寄存器的8位值。
 */
uint8_t CRC_GetIDRegister(void)
{
    /* 读取CRC独立数据寄存器（IDATAR）并返回其值 */
    return (CRC->IDATAR);
}