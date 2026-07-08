/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_flash.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/05/24
* Description        : 本文件包含了FLASH固件库的所有函数原型。  
*                      This file contains all the functions prototypes for the FLASH  
*                      firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32V30x_FLASH_H
#define __CH32V30x_FLASH_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* FLASH状态枚举定义 */
typedef enum
{ 
  FLASH_BUSY = 1,                       /**< FLASH忙状态 */
  FLASH_ERROR_PG,                       /**< FLASH编程错误 */
  FLASH_ERROR_WRP,                      /**< FLASH写保护错误 */
  FLASH_COMPLETE,                       /**< FLASH操作完成 */
  FLASH_TIMEOUT,                        /**< FLASH操作超时 */
  FLASH_OP_RANGE_ERROR = 0xFD,          /**< FLASH操作范围错误 */
  FLASH_ALIGN_ERROR = 0xFE,             /**< FLASH地址对齐错误 */
  FLASH_ADR_RANGE_ERROR = 0xFF,         /**< FLASH地址范围错误 */
}FLASH_Status;

/* 写保护扇区定义 - 每个扇区4KB */
#define FLASH_WRProt_Sectors0        ((uint32_t)0x00000001) /**< 扇区0写保护 */
#define FLASH_WRProt_Sectors1        ((uint32_t)0x00000002) /**< 扇区1写保护 */
#define FLASH_WRProt_Sectors2        ((uint32_t)0x00000004) /**< 扇区2写保护 */
#define FLASH_WRProt_Sectors3        ((uint32_t)0x00000008) /**< 扇区3写保护 */
#define FLASH_WRProt_Sectors4        ((uint32_t)0x00000010) /**< 扇区4写保护 */
#define FLASH_WRProt_Sectors5        ((uint32_t)0x00000020) /**< 扇区5写保护 */
#define FLASH_WRProt_Sectors6        ((uint32_t)0x00000040) /**< 扇区6写保护 */
#define FLASH_WRProt_Sectors7        ((uint32_t)0x00000080) /**< 扇区7写保护 */
#define FLASH_WRProt_Sectors8        ((uint32_t)0x00000100) /**< 扇区8写保护 */
#define FLASH_WRProt_Sectors9        ((uint32_t)0x00000200) /**< 扇区9写保护 */
#define FLASH_WRProt_Sectors10       ((uint32_t)0x00000400) /**< 扇区10写保护 */
#define FLASH_WRProt_Sectors11       ((uint32_t)0x00000800) /**< 扇区11写保护 */
#define FLASH_WRProt_Sectors12       ((uint32_t)0x00001000) /**< 扇区12写保护 */
#define FLASH_WRProt_Sectors13       ((uint32_t)0x00002000) /**< 扇区13写保护 */
#define FLASH_WRProt_Sectors14       ((uint32_t)0x00004000) /**< 扇区14写保护 */
#define FLASH_WRProt_Sectors15       ((uint32_t)0x00008000) /**< 扇区15写保护 */
#define FLASH_WRProt_Sectors16       ((uint32_t)0x00010000) /**< 扇区16写保护 */
#define FLASH_WRProt_Sectors17       ((uint32_t)0x00020000) /**< 扇区17写保护 */
#define FLASH_WRProt_Sectors18       ((uint32_t)0x00040000) /**< 扇区18写保护 */
#define FLASH_WRProt_Sectors19       ((uint32_t)0x00080000) /**< 扇区19写保护 */
#define FLASH_WRProt_Sectors20       ((uint32_t)0x00100000) /**< 扇区20写保护 */
#define FLASH_WRProt_Sectors21       ((uint32_t)0x00200000) /**< 扇区21写保护 */
#define FLASH_WRProt_Sectors22       ((uint32_t)0x00400000) /**< 扇区22写保护 */
#define FLASH_WRProt_Sectors23       ((uint32_t)0x00800000) /**< 扇区23写保护 */
#define FLASH_WRProt_Sectors24       ((uint32_t)0x01000000) /**< 扇区24写保护 */
#define FLASH_WRProt_Sectors25       ((uint32_t)0x02000000) /**< 扇区25写保护 */
#define FLASH_WRProt_Sectors26       ((uint32_t)0x04000000) /**< 扇区26写保护 */
#define FLASH_WRProt_Sectors27       ((uint32_t)0x08000000) /**< 扇区27写保护 */
#define FLASH_WRProt_Sectors28       ((uint32_t)0x10000000) /**< 扇区28写保护 */
#define FLASH_WRProt_Sectors29       ((uint32_t)0x20000000) /**< 扇区29写保护 */
#define FLASH_WRProt_Sectors30       ((uint32_t)0x40000000) /**< 扇区30写保护 */
#define FLASH_WRProt_Sectors31to127  ((uint32_t)0x80000000) /**< 扇区31到127写保护 */

#define FLASH_WRProt_AllSectors      ((uint32_t)0xFFFFFFFF) /**< 所有扇区写保护 */

/* 独立看门狗选项字节配置 */
#define OB_IWDG_SW                     ((uint16_t)0x0001)  /**< 独立看门狗由软件控制 */
#define OB_IWDG_HW                     ((uint16_t)0x0000)  /**< 独立看门狗由硬件控制 */

/* STOP模式复位选项字节配置 */
#define OB_STOP_NoRST                  ((uint16_t)0x0002) /**< 进入STOP模式时不产生复位 */
#define OB_STOP_RST                    ((uint16_t)0x0000) /**< 进入STOP模式时产生复位 */

/* STANDBY模式复位选项字节配置 */
#define OB_STDBY_NoRST                 ((uint16_t)0x0004) /**< 进入STANDBY模式时不产生复位 */
#define OB_STDBY_RST                   ((uint16_t)0x0000) /**< 进入STANDBY模式时产生复位 */

/* FLASH中断源定义 */	
#define FLASH_IT_ERROR                 ((uint32_t)0x00000400)  /**< FLASH错误中断 */
#define FLASH_IT_EOP                   ((uint32_t)0x00001000)  /**< FLASH操作完成中断 */
#define FLASH_IT_BANK1_ERROR           FLASH_IT_ERROR          /**< FLASH BANK1错误中断 */
#define FLASH_IT_BANK1_EOP             FLASH_IT_EOP            /**< FLASH BANK1操作完成中断 */

/* FLASH状态标志定义 */	
#define FLASH_FLAG_BSY                 ((uint32_t)0x00000001)  /**< FLASH忙标志位 */
#define FLASH_FLAG_EOP                 ((uint32_t)0x00000020)  /**< FLASH操作完成标志位 */
#define FLASH_FLAG_WRPRTERR            ((uint32_t)0x00000010)  /**< FLASH写保护错误标志位 */
#define FLASH_FLAG_OPTERR              ((uint32_t)0x80000001)  /**< FLASH选项字节错误标志位 */

#define FLASH_FLAG_BANK1_BSY                 FLASH_FLAG_BSY       /**< FLASH BANK1忙标志位*/
#define FLASH_FLAG_BANK1_EOP                 FLASH_FLAG_EOP       /**< FLASH BANK1操作完成标志位 */
#define FLASH_FLAG_BANK1_WRPRTERR            FLASH_FLAG_WRPRTERR  /**< FLASH BANK1写保护错误标志位 */

/* FLASH访问时钟配置 */
#define FLASH_Access_SYSTEM_HALF      ((uint32_t)0x00000000)   /**< FLASH访问时钟 = 系统时钟/2 */
#define FLASH_Access_SYSTEM           ((uint32_t)0x02000000)   /**< FLASH访问时钟 = 系统时钟 */
 
/* 适用于所有设备的功能函数声明 */
void FLASH_Unlock(void);                                                                 /**< 解锁FLASH编程擦除控制器 */
void FLASH_Lock(void);                                                                   /**< 锁定FLASH编程擦除控制器 */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);                                     /**< 擦除指定页 */
FLASH_Status FLASH_EraseAllPages(void);                                                  /**< 擦除所有页 */
FLASH_Status FLASH_EraseOptionBytes(void);                                               /**< 擦除选项字节 */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data);                         /**< 编程一个字(32位)数据 */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data);                     /**< 编程半个字(16位)数据 */
FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data);                /**< 编程选项字节数据 */
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Sectors);                        /**< 使能写保护 */
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState);                          /**< 配置读保护 */
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STOP, uint16_t OB_STDBY); /**< 配置用户选项字节 */
uint32_t FLASH_GetUserOptionByte(void);                                                  /**< 获取用户选项字节值 */
uint32_t FLASH_GetWriteProtectionOptionByte(void);                                       /**< 获取写保护选项字节值 */
FlagStatus FLASH_GetReadOutProtectionStatus(void);                                       /**< 获取读保护状态 */
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);                        /**< 配置FLASH中断 */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG);                                     /**< 获取FLASH标志位状态 */
void FLASH_ClearFlag(uint32_t FLASH_FLAG);                                               /**< 清除FLASH标志位 */
FLASH_Status FLASH_GetStatus(void);                                                      /**< 获取FLASH状态 */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);                               /**< 等待上一次操作完成 */
void FLASH_Unlock_Fast(void);                                                            /**< 快速解锁FLASH编程擦除控制器 */
void FLASH_Lock_Fast(void);                                                              /**< 快速锁定FLASH编程擦除控制器 */
void FLASH_ErasePage_Fast(uint32_t Page_Address);                                        /**< 快速擦除指定页 */
void FLASH_EraseBlock_32K_Fast(uint32_t Block_Address);                                  /**< 快速擦除32K块 */
void FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t* pbuf);                      /**< 快速编程一页数据 */
void FLASH_Access_Clock_Cfg(uint32_t FLASH_Access_CLK);                                  /**< 配置FLASH访问时钟 */
void FLASH_Enhance_Mode(FunctionalState NewState);                                       /**< 配置增强模式 */

/* 新功能函数声明（适用于所有设备） */
void FLASH_UnlockBank1(void);                                                            /**< 解锁FLASH BANK1 */
void FLASH_LockBank1(void);                                                              /**< 锁定FLASH BANK1 */
FLASH_Status FLASH_EraseAllBank1Pages(void);                                             /**< 擦除BANK1所有页 */
FLASH_Status FLASH_GetBank1Status(void);                                                 /**< 获取BANK1状态 */
FLASH_Status FLASH_WaitForLastBank1Operation(uint32_t Timeout);                          /**< 等待BANK1上一次操作完成 */
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);                       /**< ROM区域擦除 */
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);       /**< ROM区域写入数据 */

#ifdef __cplusplus
}
#endif

#endif