/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_flash.c
* 作者              : WCH
* 版本              : V1.0.1
* 日期              : 2025/04/14
* 描述              : 本文件提供了所有FLASH固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或不修改）和二进制代码用于南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/

#include "ch32v30x_flash.h"

/* Flash 控制寄存器位定义 */
#define CR_PG_Set                  ((uint32_t)0x00000001)  /* 编程(Program)使能位，置1允许编程操作 */
#define CR_PG_Reset                ((uint32_t)0xFFFFFFFE)  /* 编程(Program)禁止位，清零编程使能位 */
#define CR_PER_Set                 ((uint32_t)0x00000002)  /* 页擦除(Page Erase)使能位，置1允许页擦除 */
#define CR_PER_Reset               ((uint32_t)0xFFFFFFFD)  /* 页擦除禁止位，清零页擦除使能位 */
#define CR_MER_Set                 ((uint32_t)0x00000004)  /* 批量擦除(Mass Erase)使能位，置1允许全片擦除 */
#define CR_MER_Reset               ((uint32_t)0xFFFFFFFB)  /* 批量擦除禁止位，清零批量擦除使能位 */
#define CR_OPTPG_Set               ((uint32_t)0x00000010)  /* 选项字节编程使能位，置1允许编程选项字节 */
#define CR_OPTPG_Reset             ((uint32_t)0xFFFFFFEF)  /* 选项字节编程禁止位，清零选项字节编程使能位 */
#define CR_OPTER_Set               ((uint32_t)0x00000020)  /* 选项字节擦除使能位，置1允许擦除选项字节 */
#define CR_OPTER_Reset             ((uint32_t)0xFFFFFFDF)  /* 选项字节擦除禁止位，清零选项字节擦除使能位 */
#define CR_STRT_Set                ((uint32_t)0x00000040)  /* 启动操作位，置1触发擦除或编程操作 */
#define CR_LOCK_Set                ((uint32_t)0x00000080)  /* 锁定FPEC位，置1锁定Flash控制器，防止意外写操作 */
#define CR_FLOCK_Set               ((uint32_t)0x00008000)  /* 快速编程模式锁定位，置1锁定快速编程模式 */
#define CR_PAGE_PG                 ((uint32_t)0x00010000)  /* 页编程模式使能位，用于快速页编程 */
#define CR_PAGE_ER                 ((uint32_t)0x00020000)  /* 页擦除模式使能位，用于快速页擦除（256字节页） */
#define CR_BER32                   ((uint32_t)0x00040000)  /* 32KB块擦除使能位，用于快速块擦除 */
#define CR_PG_STRT                 ((uint32_t)0x00200000)  /* 快速页编程启动位，触发页编程操作 */

/* FLASH 状态寄存器位定义 */
#define SR_BSY                     ((uint32_t)0x00000001)  /* 忙标志位，1表示Flash正在执行操作 */
#define SR_WR_BSY                  ((uint32_t)0x00000002)  /* 写忙标志位，1表示Flash内部写操作正在进行 */
#define SR_WRPRTERR                ((uint32_t)0x00000010)  /* 写保护错误标志位，1表示试图写入被保护的页 */
#define SR_EOP                     ((uint32_t)0x00000020)  /* 操作结束标志位，1表示操作成功完成 */

/* FLASH 掩码定义 */
#define RDPRT_Mask                 ((uint32_t)0x00000002)  /* 读保护状态掩码，用于从OBR寄存器提取读保护位 */
#define WRP0_Mask                  ((uint32_t)0x000000FF)  /* 写保护区域0掩码（低8位） */
#define WRP1_Mask                  ((uint32_t)0x0000FF00)  /* 写保护区域1掩码（次低8位） */
#define WRP2_Mask                  ((uint32_t)0x00FF0000)  /* 写保护区域2掩码（次高8位） */
#define WRP3_Mask                  ((uint32_t)0xFF000000)  /* 写保护区域3掩码（高8位） */
#define OB_USER_BFB2               ((uint16_t)0x0008)      /* 选项字节用户位：启动Bank2标志（用于双Bank芯片） */

/* FLASH 密钥定义 */
#define RDP_Key                    ((uint16_t)0x00A5)      /* 读保护解除密钥，写入RDP选项字节可解除读保护 */
#define FLASH_KEY1                 ((uint32_t)0x45670123)  /* FPEC解锁第一个密钥 */
#define FLASH_KEY2                 ((uint32_t)0xCDEF89AB)  /* FPEC解锁第二个密钥 */

/* FLASH BANK地址范围 */
#define FLASH_BANK1_END_ADDRESS    ((uint32_t)0x807FFFF)   /* Bank1结束地址（假设芯片容量为512KB，地址范围0x08000000~0x0807FFFF） */

/* 超时定义（用于等待操作完成） */
#define EraseTimeout               ((uint32_t)0x00130000)  /* 擦除操作最大等待循环次数 */
#define ProgramTimeout             ((uint32_t)0x00005000)  /* 编程操作最大等待循环次数 */

/* Flash 有效编程地址范围 */
#define ValidAddrStart             (FLASH_BASE)            /* Flash起始地址（通常为0x08000000） */
#define ValidAddrEnd               (FLASH_BASE + 0x78000)  /* Flash有效结束地址（假设512KB，但留出部分空间？实际根据芯片调整） */

/* Flash 页/块大小定义 */
#define Size_256B                  0x100                   /* 256字节（页大小，用于快速模式） */
#define Size_4KB                   0x1000                  /* 4KB（标准页大小） */
#define Size_32KB                  0x8000                  /* 32KB（块大小） */

/* 全片擦除后的延迟宏，通过空操作循环实现简单延时，确保操作完成 */
#define FLASH_EraseAll_Delay(t)    ({for(uint32_t i = 0; i<t;i++){asm("nop");}})

/*********************************************************************
 * @fn      FLASH_Unlock
 *
 * @brief   解锁FLASH程序擦除控制器（FPEC）。
 *
 * @return  无
 */
void FLASH_Unlock(void)
{
    /* 向密钥寄存器写入两个密钥以解锁Bank1的FPEC */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/*********************************************************************
 * @fn      FLASH_UnlockBank1
 *
 * @brief   解锁FLASH Bank1的程序擦除控制器（与FLASH_Unlock功能相同）。
 *
 * @return  无
 */
void FLASH_UnlockBank1(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/*********************************************************************
 * @fn      FLASH_Lock
 *
 * @brief   锁定FLASH程序擦除控制器。
 *
 * @return  无
 */
void FLASH_Lock(void)
{
    /* 设置控制寄存器的LOCK位，锁定FPEC */
    FLASH->CTLR |= CR_LOCK_Set;
}

/*********************************************************************
 * @fn      FLASH_LockBank1
 *
 * @brief   锁定FLASH Bank1的程序擦除控制器。
 *
 * @return  无
 */
void FLASH_LockBank1(void)
{
    FLASH->CTLR |= CR_LOCK_Set;
}

/*********************************************************************
 * @fn      FLASH_ErasePage
 *
 * @brief   擦除指定的FLASH页（页大小为4KB）。
 *
 * @param   Page_Address - 要擦除的页地址。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
    FLASH_Status status = FLASH_COMPLETE;

    /* 等待上次操作完成 */
    status = FLASH_WaitForLastOperation(EraseTimeout);

    if(status == FLASH_COMPLETE)
    {
        /* 设置页擦除使能位 */
        FLASH->CTLR |= CR_PER_Set;
        /* 设置要擦除的页地址 */
        FLASH->ADDR = Page_Address;
        /* 启动擦除操作 */
        FLASH->CTLR |= CR_STRT_Set;

        /* 等待擦除完成 */
        status = FLASH_WaitForLastOperation(EraseTimeout);

        /* 清除页擦除使能位 */
        FLASH->CTLR &= CR_PER_Reset;
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_EraseAllPages
 *
 * @brief   擦除所有FLASH页（全片擦除）。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_EraseAllPages(void)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_WaitForLastOperation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    {
        /* 设置批量擦除使能位 */
        FLASH->CTLR |= CR_MER_Set;
        /* 启动擦除操作 */
        FLASH->CTLR |= CR_STRT_Set;

        /* 等待擦除完成 */
        status = FLASH_WaitForLastOperation(EraseTimeout);

        /* 清除批量擦除使能位 */
        FLASH->CTLR &= CR_MER_Reset;
    }
    /* 增加延时，确保内部操作完全结束 */
    FLASH_EraseAll_Delay(300000);
    return status;
}

/*********************************************************************
 * @fn      FLASH_EraseAllBank1Pages
 *
 * @brief   擦除所有Bank1的FLASH页。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_EraseAllBank1Pages(void)
{
    FLASH_Status status = FLASH_COMPLETE;
    status = FLASH_WaitForLastBank1Operation(EraseTimeout);

    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_MER_Set;
        FLASH->CTLR |= CR_STRT_Set;

        status = FLASH_WaitForLastBank1Operation(EraseTimeout);

        FLASH->CTLR &= CR_MER_Reset;
    }
    FLASH_EraseAll_Delay(300000);
    return status;
}

/*********************************************************************
 * @fn      FLASH_EraseOptionBytes
 *
 * @brief   擦除FLASH选项字节。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_EraseOptionBytes(void)
{
    uint16_t     rdptmp = RDP_Key;
    uint32_t     Address = 0x1FFFF800;  /* 选项字节在系统存储区中的基地址 */
    __IO uint8_t i;

    FLASH_Status status = FLASH_COMPLETE;
    /* 如果当前已使能读保护，则RDP应设为0x00，否则保持RDP_Key(0xA5)以解除保护 */
    if(FLASH_GetReadOutProtectionStatus() != RESET)
    {
        rdptmp = 0x00;
    }
    status = FLASH_WaitForLastOperation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    {
        /* 解锁选项字节区域 */
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        /* 设置选项字节擦除使能位 */
        FLASH->CTLR |= CR_OPTER_Set;
        /* 启动擦除操作 */
        FLASH->CTLR |= CR_STRT_Set;
        status = FLASH_WaitForLastOperation(EraseTimeout);

        if(status == FLASH_COMPLETE)
        {
            /* 清除选项字节擦除使能位 */
            FLASH->CTLR &= CR_OPTER_Reset;
            /* 设置选项字节编程使能位 */
            FLASH->CTLR |= CR_OPTPG_Set;
            /* 写入RDP选项字节（读保护配置） */
            OB->RDPR = (uint16_t)rdptmp;
            status = FLASH_WaitForLastOperation(ProgramTimeout);

            if(status != FLASH_TIMEOUT)
            {
                FLASH->CTLR &= CR_OPTPG_Reset;
            }
        }
        else
        {
            if(status != FLASH_TIMEOUT)
            {
                FLASH->CTLR &= CR_OPTPG_Reset;
            }
        }

        /* 将剩余的选项字节区域全部写为0xFF（默认值） */
        FLASH->CTLR |= CR_OPTPG_Set;

        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Address + 2 * i) = 0x00FF;  /* 选项字节每个单元为16位，低8位数据，高8位为反码 */
            while(FLASH->STATR & SR_BSY)             /* 等待编程完成 */
                ;
        }

        FLASH->CTLR &= ~CR_OPTPG_Set;                /* 关闭选项字节编程 */
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_ProgramWord
 *
 * @brief   在指定地址编程一个32位字。
 *
 * @param   Address - 要编程的地址。
 * @param   Data - 要编程的数据。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
    FLASH_Status  status = FLASH_COMPLETE;
    __IO uint32_t tmp = 0;

    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        /* 设置编程使能位 */
        FLASH->CTLR |= CR_PG_Set;

        /* 先写入低16位 */
        *(__IO uint16_t *)Address = (uint16_t)Data;
        status = FLASH_WaitForLastOperation(ProgramTimeout);

        if(status == FLASH_COMPLETE)
        {
            /* 再写入高16位（地址偏移2字节） */
            tmp = Address + 2;
            *(__IO uint16_t *)tmp = Data >> 16;
            status = FLASH_WaitForLastOperation(ProgramTimeout);
            FLASH->CTLR &= CR_PG_Reset;  /* 清除编程使能位 */
        }
        else
        {
            FLASH->CTLR &= CR_PG_Reset;
        }
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_ProgramHalfWord
 *
 * @brief   在指定地址编程一个16位半字。
 *
 * @param   Address - 要编程的地址。
 * @param   Data - 要编程的数据。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_PG_Set;               /* 使能编程 */
        *(__IO uint16_t *)Address = Data;       /* 写入半字 */
        status = FLASH_WaitForLastOperation(ProgramTimeout);
        FLASH->CTLR &= CR_PG_Reset;              /* 关闭编程 */
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_ProgramOptionByteData
 *
 * @brief   在指定的选项字节数据地址编程一个字节。
 *
 * @param   Address - 要编程的选项字节地址（位于0x1FFFF800~0x1FFFF80F范围内）。
 * @param   Data - 要编程的数据（8位）。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;  /* 选项字节基地址 */
    __IO uint8_t i;
    uint16_t     pbuf[8];             /* 用于缓存所有选项字节的数组（每个16位） */

    status = FLASH_WaitForLastOperation(ProgramTimeout);
    if(status == FLASH_COMPLETE)
    {
        /* 解锁选项字节 */
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        /* 读取当前所有选项字节到缓存 */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* 擦除选项字节区域 */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* 修改要编程的选项字节：格式为低8位数据，高8位为数据的反码 */
        pbuf[((Address - 0x1FFFF800) / 2)] = ((((uint16_t) ~(Data)) << 8) | ((uint16_t)Data));

        /* 使能选项字节编程 */
        FLASH->CTLR |= CR_OPTPG_Set;

        /* 将缓存中的值写回选项字节区域 */
        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }

        /* 关闭选项字节编程 */
        FLASH->CTLR &= ~CR_OPTPG_Set;
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_EnableWriteProtection
 *
 * @brief   使能指定扇区的写保护。
 *
 * @param   FLASH_Sectors - 要写保护的扇区位掩码（32位，每1位对应一个扇区组）。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Sectors)
{
    uint16_t     WRP0_Data = 0xFFFF, WRP1_Data = 0xFFFF, WRP2_Data = 0xFFFF, WRP3_Data = 0xFFFF;
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;
    __IO uint8_t i;
    uint16_t     pbuf[8];

    /* 输入参数FLASH_Sectors中，某位为1表示对应扇区需要写保护。
       但选项字节中写保护位是反码（0表示保护，1表示不保护），所以取反 */
    FLASH_Sectors = (uint32_t)(~FLASH_Sectors);
    /* 分别提取四个字节到WRP0~WRP3 */
    WRP0_Data = (uint16_t)(FLASH_Sectors & WRP0_Mask);
    WRP1_Data = (uint16_t)((FLASH_Sectors & WRP1_Mask) >> 8);
    WRP2_Data = (uint16_t)((FLASH_Sectors & WRP2_Mask) >> 16);
    WRP3_Data = (uint16_t)((FLASH_Sectors & WRP3_Mask) >> 24);

    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        /* 解锁选项字节 */
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        /* 读取当前选项字节到缓存 */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* 擦除选项字节 */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* 更新写保护数据到缓存（选项字节中WRP0~WRP3位于pbuf[4]~pbuf[7]） */
        pbuf[4] = WRP0_Data;
        pbuf[5] = WRP1_Data;
        pbuf[6] = WRP2_Data;
        pbuf[7] = WRP3_Data;

        /* 使能选项字节编程并写入 */
        FLASH->CTLR |= CR_OPTPG_Set;
        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }
        FLASH->CTLR &= ~CR_OPTPG_Set;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_ReadOutProtection
 *
 * @brief   使能或禁用读保护。
 *
 * @param   NewState - 读保护的新状态（ENABLE 或 DISABLE）。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;
    __IO uint8_t i;
    uint16_t     pbuf[8];

    status = FLASH_WaitForLastOperation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    {
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        /* 读出现有选项字节 */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* 擦除选项字节 */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* 设置RDP选项字节值：
           若禁用读保护，RDP应设为0x5AA5（解除保护）；
           若使能读保护，RDP应设为0x00FF（保护使能） */
        if(NewState == DISABLE)
            pbuf[0] = 0x5AA5;
        else
            pbuf[0] = 0x00FF;

        /* 编程选项字节 */
        FLASH->CTLR |= CR_OPTPG_Set;
        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }
        FLASH->CTLR &= ~CR_OPTPG_Set;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_UserOptionByteConfig
 *
 * @brief   配置FLASH用户选项字节（IWDG_SW / RST_STOP / RST_STDBY）。
 *
 * @param   OB_IWDG - IWDG模式选择：
 *            OB_IWDG_SW - 软件IWDG
 *            OB_IWDG_HW - 硬件IWDG
 * @param   OB_STOP - 进入STOP模式时是否产生复位：
 *            OB_STOP_NoRST - 不复位
 *            OB_STOP_RST - 复位
 * @param   OB_STDBY - 进入待机模式时是否产生复位：
 *            OB_STDBY_NoRST - 不复位
 *            OB_STDBY_RST - 复位
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STOP, uint16_t OB_STDBY)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;
    __IO uint8_t i;
    uint16_t     pbuf[8];
    uint16_t     temp;

    FLASH->OBKEYR = FLASH_KEY1;
    FLASH->OBKEYR = FLASH_KEY2;
    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        /* 读取当前选项字节 */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* 保留原用户字节中的其他位（低3位用于IWDG/STOP/STDBY，其余位不变） */
        temp=pbuf[1]&(~0x7);

        /* 擦除选项字节 */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* 组合新的用户字节（pbuf[1]对应USER选项） */
        pbuf[1] = OB_IWDG | (uint16_t)(OB_STOP | (uint16_t)(OB_STDBY | ((uint16_t)temp)));

        /* 编程选项字节 */
        FLASH->CTLR |= CR_OPTPG_Set;
        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }
        FLASH->CTLR &= ~CR_OPTPG_Set;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_GetUserOptionByte
 *
 * @brief   获取FLASH用户选项字节的值。
 *
 * @return  FLASH用户选项字节的值（IWDG_SW位0，RST_STOP位1，RST_STDBY位2）。
 */
uint32_t FLASH_GetUserOptionByte(void)
{
    /* OBR寄存器位9:2为用户选项字节，右移2位后得到低3位有效值 */
    return (uint32_t)(FLASH->OBR >> 2);
}

/*********************************************************************
 * @fn      FLASH_GetWriteProtectionOptionByte
 *
 * @brief   获取FLASH写保护选项字节寄存器的值。
 *
 * @return  FLASH写保护选项字节寄存器（WPR）的值。
 */
uint32_t FLASH_GetWriteProtectionOptionByte(void)
{
    return (uint32_t)(FLASH->WPR);
}

/*********************************************************************
 * @fn      FLASH_GetReadOutProtectionStatus
 *
 * @brief   检查FLASH读保护状态是否已设置。
 *
 * @return  读保护状态（SET 或 RESET）。
 */
FlagStatus FLASH_GetReadOutProtectionStatus(void)
{
    FlagStatus readoutstatus = RESET;
    /* OBR寄存器位1为读保护标志 */
    if((FLASH->OBR & RDPRT_Mask) != (uint32_t)RESET)
    {
        readoutstatus = SET;
    }
    else
    {
        readoutstatus = RESET;
    }
    return readoutstatus;
}

/*********************************************************************
 * @fn      FLASH_ITConfig
 *
 * @brief   使能或禁用指定的FLASH中断。
 *
 * @param   FLASH_IT - 要配置的中断源：
 *            FLASH_IT_ERROR - FLASH错误中断
 *            FLASH_IT_EOP   - FLASH操作结束中断
 * @param   NewState - 中断的新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        FLASH->CTLR |= FLASH_IT;       /* 使能中断 */
    }
    else
    {
        FLASH->CTLR &= ~(uint32_t)FLASH_IT; /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      FLASH_GetFlagStatus
 *
 * @brief   检查指定的FLASH标志位是否置位。
 *
 * @param   FLASH_FLAG - 要检查的标志：
 *            FLASH_FLAG_BSY       - 忙标志
 *            FLASH_FLAG_WRPRTERR  - 写保护错误标志
 *            FLASH_FLAG_EOP       - 操作结束标志
 *            FLASH_FLAG_OPTERR    - 选项字节错误标志（位于OBR寄存器）
 *
 * @return  标志的新状态（SET 或 RESET）。
 */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG)
{
    FlagStatus bitstatus = RESET;

    if(FLASH_FLAG == FLASH_FLAG_OPTERR)
    {
        /* 选项字节错误标志位于OBR寄存器的位0 */
        if((FLASH->OBR & (1 << 0)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else
    {
        /* 其他标志位于STATR寄存器 */
        if((FLASH->STATR & FLASH_FLAG) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      FLASH_ClearFlag
 *
 * @brief   清除FLASH的挂起标志。
 *
 * @param   FLASH_FLAG - 要清除的标志（可以是FLASH_FLAG_WRPRTERR或FLASH_FLAG_EOP）。
 *
 * @return  无
 */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
    /* 向STATR寄存器的对应位写1以清除标志 */
    FLASH->STATR = FLASH_FLAG;
}

/*********************************************************************
 * @fn      FLASH_GetStatus
 *
 * @brief   返回FLASH当前状态。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE。
 */
FLASH_Status FLASH_GetStatus(void)
{
    FLASH_Status flashstatus = FLASH_COMPLETE;

    if((FLASH->STATR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY)
    {
        flashstatus = FLASH_BUSY;            /* 忙 */
    }
    else
    {
        if((FLASH->STATR & FLASH_FLAG_WRPRTERR) != 0)
        {
            flashstatus = FLASH_ERROR_WRP;   /* 写保护错误 */
        }
        else
        {
            flashstatus = FLASH_COMPLETE;    /* 操作完成且无错误 */
        }
    }
    return flashstatus;
}

/*********************************************************************
 * @fn      FLASH_GetBank1Status
 *
 * @brief   返回FLASH Bank1的当前状态。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE。
 */
FLASH_Status FLASH_GetBank1Status(void)
{
    FLASH_Status flashstatus = FLASH_COMPLETE;

    if((FLASH->STATR & FLASH_FLAG_BANK1_BSY) == FLASH_FLAG_BSY)
    {
        flashstatus = FLASH_BUSY;
    }
    else
    {
        if((FLASH->STATR & FLASH_FLAG_BANK1_WRPRTERR) != 0)
        {
            flashstatus = FLASH_ERROR_WRP;
        }
        else
        {
            flashstatus = FLASH_COMPLETE;
        }
    }
    return flashstatus;
}

/*********************************************************************
 * @fn      FLASH_WaitForLastOperation
 *
 * @brief   等待上次Flash操作完成，或超时发生。
 *
 * @param   Timeout - 最大等待循环次数。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_GetBank1Status();
    while((status == FLASH_BUSY) && (Timeout != 0x00))
    {
        status = FLASH_GetBank1Status();
        Timeout--;
    }
    if(Timeout == 0x00)
    {
        status = FLASH_TIMEOUT;   /* 超时 */
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_WaitForLastBank1Operation
 *
 * @brief   等待Bank1上的Flash操作完成，或超时发生。
 *
 * @param   Timeout - 最大等待循环次数。
 *
 * @return  FLASH状态：可能为FLASH_BUSY, FLASH_ERROR_PG, FLASH_ERROR_WRP,
 *          FLASH_COMPLETE 或 FLASH_TIMEOUT。
 */
FLASH_Status FLASH_WaitForLastBank1Operation(uint32_t Timeout)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_GetBank1Status();
    while((status == FLASH_FLAG_BANK1_BSY) && (Timeout != 0x00))
    {
        status = FLASH_GetBank1Status();
        Timeout--;
    }
    if(Timeout == 0x00)
    {
        status = FLASH_TIMEOUT;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_Unlock_Fast
 *
 * @brief   解锁快速编程擦除模式。
 *
 * @return  无
 */
void FLASH_Unlock_Fast(void)
{
    /* 先解锁FPEC */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    /* 再解锁快速编程模式（向MODEKEYR写入密钥） */
    FLASH->MODEKEYR = FLASH_KEY1;
    FLASH->MODEKEYR = FLASH_KEY2;
}

/*********************************************************************
 * @fn      FLASH_Lock_Fast
 *
 * @brief   锁定快速编程擦除模式。
 *
 * @return  无
 */
void FLASH_Lock_Fast(void)
{
    /* 设置控制寄存器的FLOCK位锁定快速模式 */
    FLASH->CTLR |= CR_FLOCK_Set;
}

/*********************************************************************
 * @fn      FLASH_ErasePage_Fast
 *
 * @brief   快速模式擦除指定的FLASH页（页大小256字节）。
 *
 * @param   Page_Address - 要擦除的页地址（自动对齐到256字节边界）。
 *
 * @return  无
 */
void FLASH_ErasePage_Fast(uint32_t Page_Address)
{
    Page_Address &= 0xFFFFFF00;   /* 确保地址为256字节对齐 */

    FLASH->CTLR |= CR_PAGE_ER;     /* 使能页擦除（256字节） */
    FLASH->ADDR = Page_Address;    /* 设置擦除地址 */
    FLASH->CTLR |= CR_STRT_Set;    /* 启动擦除 */
    while(FLASH->STATR & SR_BSY)   /* 等待操作完成 */
        ;
    FLASH->CTLR &= ~CR_PAGE_ER;    /* 关闭页擦除模式 */
}

/*********************************************************************
 * @fn      FLASH_EraseBlock_32K_Fast
 *
 * @brief   快速模式擦除指定的FLASH块（块大小32KB）。
 *
 * @param   Block_Address - 要擦除的块地址（自动对齐到32KB边界）。
 *
 * @return  无
 */
void FLASH_EraseBlock_32K_Fast(uint32_t Block_Address)
{
    Block_Address &= 0xFFFF8000;   /* 对齐到32KB边界 */

    FLASH->CTLR |= CR_BER32;       /* 使能32KB块擦除 */
    FLASH->ADDR = Block_Address;
    FLASH->CTLR |= CR_STRT_Set;
    while(FLASH->STATR & SR_BSY)
        ;
    FLASH->CTLR &= ~CR_BER32;      /* 关闭块擦除模式 */
}

/*********************************************************************
 * @fn      FLASH_ProgramPage_Fast
 *
 * @brief   快速模式编程一个FLASH页（页大小256字节）。
 *
 * @param   Page_Address - 要编程的页地址（自动对齐到256字节边界）。
 * @param   pbuf - 指向要写入的数据缓冲区（至少256字节，即64个32位字）。
 *
 * @return  无
 */
void FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t *pbuf)
{
    uint8_t size = 64;   /* 256字节 = 64个32位字 */

    Page_Address &= 0xFFFFFF00;   /* 地址对齐 */

    FLASH->CTLR |= CR_PAGE_PG;    /* 使能页编程模式 */
    while(FLASH->STATR & SR_BSY)  /* 等待上一次操作完成（如有） */
        ;
    while(FLASH->STATR & SR_WR_BSY) /* 等待写缓冲区空闲 */
        ;

    /* 循环写入64个字（256字节） */
    while(size)
    {
        *(uint32_t *)Page_Address = *(uint32_t *)pbuf;  /* 写入一个字 */
        Page_Address += 4;
        pbuf += 1;
        size -= 1;
        while(FLASH->STATR & SR_WR_BSY)  /* 等待当前字写入完成，避免溢出 */
            ;
    }

    /* 触发页编程操作（将缓冲区数据实际写入Flash） */
    FLASH->CTLR |= CR_PG_STRT;
    while(FLASH->STATR & SR_BSY)
        ;
    FLASH->CTLR &= ~CR_PAGE_PG;   /* 关闭页编程模式 */
}

/*********************************************************************
 * @fn      FLASH_Access_Clock_Cfg
 *
 * @brief   配置FLASH访问时钟（需要先解锁FPEC）。
 *
 * @param   FLASH_Access_CLK - 访问时钟选择：
 *            FLASH_Access_SYSTEM_HALF - 系统时钟/2
 *            FLASH_Access_SYSTEM      - 系统时钟
 *
 * @return  无
 */
void FLASH_Access_Clock_Cfg(uint32_t FLASH_Access_CLK)
{
    FLASH->CTLR &= ~(1 << 25);    /* 清零ACCR位（位25） */
    FLASH->CTLR |= FLASH_Access_CLK; /* 设置访问时钟 */
}

/*********************************************************************
 * @fn      FLASH_Enhance_Mode
 *
 * @brief   使能或禁用FLASH增强模式（提高读取性能）。
 *
 * @param   NewState - 新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void FLASH_Enhance_Mode(FunctionalState NewState)
{
    if(NewState)
    {
        FLASH->CTLR |= (1 << 24);    /* 使能增强模式（位24） */
    }
    else
    {
        FLASH->CTLR &= ~(1 << 24);   /* 禁用增强模式 */
        FLASH->CTLR |= (1 << 22);    /* 可能需要设置预取缓冲？根据手册，同时需要置位位22 */
    }
}

/*********************************************************************
 * @fn      ROM_ERASE
 *
 * @brief   内部静态函数，按指定大小擦除连续的Flash区域。
 *
 * @param   StartAddr - 起始地址（已按擦除大小对齐）。
 * @param   Cnt - 要擦除的块数。
 * @param   Erase_Size - 擦除大小（Size_32KB, Size_4KB, Size_256B）。
 *
 * @return  无
 */
static void ROM_ERASE(uint32_t StartAddr, uint32_t Cnt, uint32_t Erase_Size)
{
    do{
        /* 根据擦除大小设置相应的控制位 */
        if(Erase_Size == Size_32KB)
        {
            FLASH->CTLR |= CR_BER32;        /* 32KB块擦除 */
        }
        else if(Erase_Size == Size_4KB)
        {
            FLASH->CTLR |= CR_PER_Set;      /* 4KB页擦除 */
        }
        else if(Erase_Size == Size_256B)
        {
            FLASH->CTLR |= CR_PAGE_ER;       /* 256字节页擦除（快速模式） */
        }

        FLASH->ADDR = StartAddr;            /* 设置地址 */
        FLASH->CTLR |= CR_STRT_Set;         /* 启动擦除 */
        while(FLASH->STATR & SR_BSY)        /* 等待完成 */
            ;

        /* 清除对应的控制位，并更新起始地址 */
        if(Erase_Size == Size_32KB)
        {
            FLASH->CTLR &= ~CR_BER32;
            StartAddr += Size_32KB;
        }
        else if(Erase_Size == Size_4KB)
        {
            FLASH->CTLR &= ~CR_PER_Set;
            StartAddr += Size_4KB;
        }
        else if(Erase_Size == Size_256B)
        {
            FLASH->CTLR &= ~CR_PAGE_ER;
            StartAddr += Size_256B;
        }
    }while(--Cnt);
}

/*********************************************************************
 * @fn      FLASH_ROM_ERASE
 *
 * @brief   擦除指定区域（支持混合大小擦除，自动选择最优擦除方式）。
 *
 * @param   StartAddr - 起始地址（必须256字节对齐）。
 * @param   Length - 要擦除的长度（必须是256的倍数，且不为0）。
 *
 * @return  FLASH状态：
 *            FLASH_ADR_RANGE_ERROR - 地址超出范围
 *            FLASH_ALIGN_ERROR     - 地址或长度未对齐
 *            FLASH_OP_RANGE_ERROR  - 操作超出有效范围
 *            FLASH_COMPLETE        - 成功
 */
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length)
{
    uint32_t Addr0 = 0, Addr1 = 0, Length0 = 0, Length1 = 0;

    FLASH_Status status = FLASH_COMPLETE;

    /* 检查地址范围 */
    if((StartAddr < ValidAddrStart) || (StartAddr >= ValidAddrEnd))
    {
        return FLASH_ADR_RANGE_ERROR;
    }

    if((StartAddr + Length) > ValidAddrEnd)
    {
        return FLASH_OP_RANGE_ERROR;
    }

    /* 检查对齐：起始地址和长度必须是256字节的整数倍 */
    if((StartAddr & (Size_256B-1)) || (Length & (Size_256B-1)) || (Length == 0))
    {
        return FLASH_ALIGN_ERROR;
    }

    /* 解锁FPEC和快速模式 */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    FLASH->MODEKEYR = FLASH_KEY1;
    FLASH->MODEKEYR = FLASH_KEY2;

    Addr0 = StartAddr;

    /* 将待擦除区域分割为三个部分：可能不完整的32KB块、完整的32KB块、不完整的32KB块（尾部），
       然后同样处理4KB块和256B页，确保擦除覆盖整个区域且无重叠。
       此算法通过逐步分解剩余部分来处理。 */

    if(Length >= Size_32KB)
    {
        Length0 = Size_32KB - (Addr0 & (Size_32KB - 1)); /* 第一个不完整32KB块的长度 */
        Addr1 = StartAddr + Length0;                      /* 剩余部分的起始地址 */
        Length1 = Length - Length0;                        /* 剩余部分的总长度 */
    }
    else if(Length >= Size_4KB)
    {
        Length0 = Size_4KB - (Addr0 & (Size_4KB - 1));    /* 第一个不完整4KB块的长度 */
        Addr1 = StartAddr + Length0;
        Length1 = Length - Length0;
    }
    else if(Length >= Size_256B)
    {
        Length0 = Length;                                  /* 只有256B页 */
    }

    /* 处理32KB块（头部和主体部分） */
    if(Length0 >= Size_32KB)  // 头部可能有未对齐部分
    {
        Length = Length0;
        if(Addr0 & (Size_32KB - 1))
        {
            Length0 = Size_32KB - (Addr0 & (Size_32KB - 1));  /* 第一个不完整块的大小 */
        }
        else
        {
            Length0 = 0;  /* 如果已对齐，则头部无需要单独处理，直接擦除整个块 */
        }

        /* 擦除中间完整的32KB块（数量 = (Length - Length0) / 32KB） */
        ROM_ERASE((Addr0 + Length0), ((Length - Length0) >> 15), Size_32KB);
    }

    if(Length1 >= Size_32KB)  // 尾部可能有未对齐部分
    {
        StartAddr = Addr1;
        Length = Length1;

        if((Addr1 + Length1) & (Size_32KB - 1))
        {
            Addr1 = ((StartAddr + Length1) & (~(Size_32KB - 1))); /* 尾部未对齐块的起始地址（对齐后） */
            Length1 = (StartAddr + Length1) & (Size_32KB - 1);     /* 尾部未对齐块的长度 */
        }
        else
        {
            Length1 = 0;  /* 如果尾部正好对齐，则无残留 */
        }

        ROM_ERASE(StartAddr, ((Length - Length1) >> 15), Size_32KB);
    }

    /* 处理4KB块（剩余部分） */
    if(Length0 >= Size_4KB) // 头部剩余未对齐部分
    {
        Length = Length0;
        if(Addr0 & (Size_4KB - 1))
        {
            Length0 = Size_4KB - (Addr0 & (Size_4KB - 1));
        }
        else
        {
            Length0 = 0;
        }

        ROM_ERASE((Addr0 + Length0), ((Length - Length0) >> 12), Size_4KB);
    }

    if(Length1 >= Size_4KB) // 尾部剩余未对齐部分
    {
        StartAddr = Addr1;
        Length = Length1;

        if((Addr1 + Length1) & (Size_4KB - 1))
        {
            Addr1 = ((StartAddr + Length1) & (~(Size_4KB - 1)));
            Length1 = (StartAddr + Length1) & (Size_4KB - 1);
        }
        else
        {
            Length1 = 0;
        }

        ROM_ERASE(StartAddr, ((Length - Length1) >> 12), Size_4KB);
    }

    /* 最后处理256B页（最小的擦除单位） */
    if(Length0) // 头部剩余256B页
    {
        ROM_ERASE(Addr0, (Length0 >> 8), Size_256B);
    }

    if(Length1) // 尾部剩余256B页
    {
        ROM_ERASE(Addr1, (Length1 >> 8), Size_256B);
    }

    /* 锁定快速模式和FPEC */
    FLASH->CTLR |= CR_FLOCK_Set;
    FLASH->CTLR |= CR_LOCK_Set;

    return status;
}

/*********************************************************************
 * @fn      FLASH_ROM_WRITE
 *
 * @brief   向指定区域写入数据（使用快速页编程，每页256字节）。
 *
 * @param   StartAddr - 起始地址（必须256字节对齐）。
 * @param   pbuf - 指向数据缓冲区的指针（按32位字组织）。
 * @param   Length - 要写入的字节数（必须是256的倍数）。
 *
 * @return  FLASH状态：
 *            FLASH_ADR_RANGE_ERROR - 地址超出范围
 *            FLASH_ALIGN_ERROR     - 地址或长度未对齐
 *            FLASH_OP_RANGE_ERROR  - 操作超出有效范围
 *            FLASH_COMPLETE        - 成功
 */
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length)
{
    uint32_t i;
    uint8_t size;

    FLASH_Status status = FLASH_COMPLETE;

    /* 参数检查 */
    if((StartAddr < ValidAddrStart) || (StartAddr >= ValidAddrEnd))
    {
        return FLASH_ADR_RANGE_ERROR;
    }

    if((StartAddr + Length) > ValidAddrEnd)
    {
        return FLASH_OP_RANGE_ERROR;
    }

    if((StartAddr & (Size_256B-1)) || (Length & (Size_256B-1)) || (Length == 0))
    {
        return FLASH_ALIGN_ERROR;
    }

    i = Length >> 8;   /* 需要编程的页数（每页256字节） */

    /* 解锁FPEC和快速模式 */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    FLASH->MODEKEYR = FLASH_KEY1;
    FLASH->MODEKEYR = FLASH_KEY2;

    do{
        FLASH->CTLR |= CR_PAGE_PG;        /* 使能页编程模式 */
        while(FLASH->STATR & SR_BSY)      /* 等待就绪 */
            ;
        while(FLASH->STATR & SR_WR_BSY)
            ;
        size = 64;                         /* 每页256字节 = 64个字 */
        while(size)
        {
            *(uint32_t *)StartAddr = *(uint32_t *)pbuf;  /* 写入一个字 */
            StartAddr += 4;
            pbuf += 1;
            size -= 1;
            while(FLASH->STATR & SR_WR_BSY)  /* 等待字写入完成 */
                ;
        }

        FLASH->CTLR |= CR_PG_STRT;          /* 触发页编程 */
        while(FLASH->STATR & SR_BSY)        /* 等待页编程完成 */
            ;
        FLASH->CTLR &= ~CR_PAGE_PG;         /* 关闭页编程模式 */
    }while(--i);                             /* 循环所有页 */

    /* 锁定 */
    FLASH->CTLR |= CR_FLOCK_Set;
    FLASH->CTLR |= CR_LOCK_Set;

    return status;
}