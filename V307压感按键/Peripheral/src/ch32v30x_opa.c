/********************************** (C) COPYRIGHT  *******************************
* 文件名          : ch32v30x_opa.c
* 作者             : WCH
* 版本            : V1.0.0
* 日期               : 2021/06/06
* 描述        : 该文件提供了所有运算放大器(OPA)固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或未修改）及二进制文件仅供南京沁恒微电子制造的微控制器使用。
*******************************************************************************/
#include "ch32v30x_opa.h"

/* 寄存器掩码，用于清除或设置特定位 */
#define OPA_MASK         ((uint32_t)0x000F)
/* OPA总数量，每个OPA的配置位占用4位 */
#define OPA_Total_NUM    4

/*********************************************************************
 * @函数名      OPA_DeInit
 *
 * @描述   将OPA外设寄存器重置为默认复位值。
 *
 * @返回  无
 */
void OPA_DeInit(void)
{
    OPA->CR = 0;  /* 将控制寄存器清零 */
}

/*********************************************************************
 * @函数名      OPA_Init
 *
 * @描述   根据OPA_InitStruct中指定的参数初始化OPA外设。
 *
 * @参数   OPA_InitStruct - 指向OPA_InitTypeDef结构体的指针
 *
 * @返回  无
 */
void OPA_Init(OPA_InitTypeDef *OPA_InitStruct)
{
    uint32_t tmp = 0;
    tmp = OPA->CR;  /* 读取当前控制寄存器值 */
    /* 清除对应OPA的配置位 */
    tmp &= ~(OPA_MASK << (OPA_InitStruct->OPA_NUM * OPA_Total_NUM));
    /* 设置新的配置值（包括正端输入选择、负端输入选择和模式）并左移到对应OPA位置 */
    tmp |= (((OPA_InitStruct->PSEL << OPA_PSEL_OFFSET) | (OPA_InitStruct->NSEL << OPA_NSEL_OFFSET) | (OPA_InitStruct->Mode << OPA_MODE_OFFSET)) << (OPA_InitStruct->OPA_NUM * OPA_Total_NUM));
    OPA->CR = tmp;  /* 写回控制寄存器 */
}

/*********************************************************************
 * @函数名      OPA_StructInit
 *
 * @描述   使用其复位值填充每个OPA_StructInit成员。
 *
 * @参数   OPA_StructInit - 指向OPA_InitTypeDef结构体的指针
 *
 * @返回  无
 */
void OPA_StructInit(OPA_InitTypeDef *OPA_InitStruct)
{
    OPA_InitStruct->Mode = OUT_IO_OUT1;  /* 默认模式：输出到外部引脚OUT1 */
    OPA_InitStruct->PSEL = CHP0;        /* 默认正端输入选择：通道0 */
    OPA_InitStruct->NSEL = CHN0;        /* 默认负端输入选择：通道0 */
    OPA_InitStruct->OPA_NUM = OPA1;     /* 默认OPA编号：OPA1 */
}

/*********************************************************************
 * @函数名      OPA_Cmd
 *
 * @描述   使能或失能指定的OPA外设。
 *
 * @参数   OPA_NUM - 选择OPA编号
 *            NewState - 使能（ENABLE）或失能（DISABLE）。
 *
 * @返回  无
 */
void OPA_Cmd(OPA_Num_TypeDef OPA_NUM, FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        /* 设置对应OPA的使能位（每个OPA的使能位在配置位的最低有效位） */
        OPA->CR |= (1 << (OPA_NUM * OPA_Total_NUM));
    }
    else
    {
        /* 清除对应OPA的使能位 */
        OPA->CR &= ~(1 << (OPA_NUM * OPA_Total_NUM));
    }
}