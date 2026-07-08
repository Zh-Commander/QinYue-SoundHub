/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_can.c
* 作者              : WCH
* 版本              : V1.0.1
* 日期              : 2025/04/06
* 描述              : 该文件提供了所有 CAN 固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司
* 注意: 本软件（修改或未修改）和二进制文件可用于南京沁恒微电子股份有限公司生产的微控制器。
*******************************************************************************/

#include "ch32v30x_can.h"
#include "ch32v30x_rcc.h"

/* 以下为 CAN 寄存器位定义 */

/* CAN 控制寄存器 (CTLR) 的调试冻结位 */
#define CTLR_DBF            ((uint32_t)0x00010000)

/* CAN 邮箱发送请求位 (用于 TXMIR 寄存器) */
#define TMIDxR_TXRQ         ((uint32_t)0x00000001)

/* CAN 过滤器控制寄存器 (FCTLR) 的过滤器初始化位 */
#define FCTLR_FINIT         ((uint32_t)0x00000001)

/* 等待 INAK 位（初始化应答）的超时计数值 */
#define INAK_TIMEOUT        ((uint32_t)0x0000FFFF)
/* 等待 SLAK 位（睡眠应答）的超时计数值 */
#define SLAK_TIMEOUT        ((uint32_t)0x0000FFFF)

/* 标志位掩码，用于区分不同寄存器组的标志 */
#define CAN_FLAGS_TSTATR    ((uint32_t)0x08000000)  /* 发送状态寄存器标志 */
#define CAN_FLAGS_RFIFO1    ((uint32_t)0x04000000)  /* 接收 FIFO1 寄存器标志 */
#define CAN_FLAGS_RFIFO0    ((uint32_t)0x02000000)  /* 接收 FIFO0 寄存器标志 */
#define CAN_FLAGS_STATR     ((uint32_t)0x01000000)  /* 状态寄存器标志 */
#define CAN_FLAGS_ERRSR     ((uint32_t)0x00F00000)  /* 错误状态寄存器标志 */

/* 发送邮箱编号定义 */
#define CAN_TXMAILBOX_0     ((uint8_t)0x00)  /* 邮箱 0 */
#define CAN_TXMAILBOX_1     ((uint8_t)0x01)  /* 邮箱 1 */
#define CAN_TXMAILBOX_2     ((uint8_t)0x02)  /* 邮箱 2 */

/* 模式掩码，用于提取 STATR 寄存器中的模式位 (INAK 和 SLAK) */
#define CAN_MODE_MASK       ((uint32_t)0x00000003)

/* 静态函数声明：检查指定中断位是否置位 */
static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit);

/*********************************************************************
 * @fn      CAN_DeInit
 *
 * @brief   将 CAN 外设寄存器复位到默认值。
 *
 * @param   CANx - 可以是 CAN1 或 CAN2，选择要操作的 CAN 外设。
 *
 * @return  无
 */
void CAN_DeInit(CAN_TypeDef *CANx)
{
    if(CANx == CAN1)
    {
        /* 复位 CAN1 外设 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);
    }
    else
    {
        /* 复位 CAN2 外设 */
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN2, DISABLE);
    }
}

/*********************************************************************
 * @fn      CAN_Init
 *
 * @brief   根据 CAN_InitStruct 中的参数初始化 CAN 外设。
 *
 * @param   CANx - 可以是 CAN1，选择要初始化的 CAN 外设。
 *          CAN_InitStruct - 指向 CAN_InitTypeDef 结构体的指针，
 *                           包含 CAN 的配置信息。
 *
 * @return  InitStatus - CAN 初始化状态。
 *             CAN_InitStatus_Failed  : 初始化失败
 *             CAN_InitStatus_Success : 初始化成功
 */
uint8_t CAN_Init(CAN_TypeDef *CANx, CAN_InitTypeDef *CAN_InitStruct)
{
    uint8_t  InitStatus = CAN_InitStatus_Failed;
    uint32_t wait_ack = 0x00000000;
    uint32_t chipid = DBGMCU_GetCHIPID();               /* 读取芯片 ID */
    uint32_t chippackid = (chipid >> 4) & 0xf;          /* 提取封装版本 */
    /* 针对 CH32V30x 某些封装版本的特殊处理 */
    if(chippackid >= 4 && chippackid <= 7)
    {
        if(CAN1 == CANx)
        {
            /* 对 CAN1 进行特殊复位操作（通过 APB2 外设时钟控制寄存器） */
            (*(__IO uint32_t *)(0x40021010)) |= 0x2000000;
            (*(__IO uint32_t *)(0x40021010)) &= ~(0x2000000);
        }else if(CAN2 == CANx)
        {
            (*(__IO uint32_t *)(0x40021010)) |= 0x4000000;
            (*(__IO uint32_t *)(0x40021010)) &= ~(0x4000000);
        }
        
        /* 进入初始化模式 */
        CANx->CTLR &= ~0x2;       /* 清除睡眠位 */
        CANx->CTLR |= 0x1;        /* 设置初始化请求位 */

        /* 等待 INAK 位置位，确认进入初始化模式 */
        while(!(CANx->STATR & 0x1) && (wait_ack != 0x0000FFFF))
        {
            wait_ack++;
        }

        if((CANx->STATR & 0x1))
        {
            /* 成功进入初始化模式后，根据系统时钟自动配置波特率寄存器 */
            /* BTIMR 高位置 0xC1100000 是预设值，后部分根据时钟计算位时序 */
            CANx->BTIMR = ( uint32_t)0xC1100000| \
                                    ((uint32_t)SystemCoreClock/(((((*(__IO uint32_t *)(0x40021004)) >> 8) & 0x7) < 0x4) ? 1 : (uint32_t)0x2<<(((*(__IO uint32_t *)(0x40021004)) >> 8) & 0x3))/4000000 - 1);
        }
        else
        {
            return CAN_InitStatus_Failed;
        }
        /* 退出初始化模式 */
        CANx->CTLR &= ~0x1;
        wait_ack = 0;
        /* 等待 INAK 位清零，确认退出初始化模式 */
        while((CANx->STATR & 0x1) && (wait_ack != 0x0000FFFF))
        {
            wait_ack++;
        }

        if((CANx->STATR & 0x1)){
            return CAN_InitStatus_Failed;
        }

        /* 以下是对 CAN 过滤器相关寄存器的特殊初始化操作 */
        (*(__IO uint32_t *)(0x4000660C)) |= 0x3;        /* 使能过滤器时钟? 具体地址对应 CAN 过滤器控制 */
        (*(__IO uint32_t *)(0x40006640)) = 0x0;         /* 清除过滤器寄存器 */
        (*(__IO uint32_t *)(0x40006644)) = 0x0;
        (*(__IO uint32_t *)(0x40006648)) = 0x0;
        (*(__IO uint32_t *)(0x4000664C)) = 0x0;
        (*(__IO uint32_t *)(0x4000661C)) |= 0x3;        /* 其他过滤器配置 */
        (*(__IO uint32_t *)(0x40006600)) &= ~0x1;       /* 禁用过滤器 */
        CAN_SlaveStartBank(1);                           /* 设置从过滤器起始组 */
        if(CAN1 == CANx)
        {
            /* 对 CAN1 进行额外初始化，等待并设置某些寄存器 */
            (*(__IO uint32_t *)(0x40006580)) |= 0x3;
            while(!((*(__IO uint32_t *)(0x4000640C)) & 0x3));
            (*(__IO uint32_t *)(0x4000640C)) = 0x38;
        }else if (CAN2 == CANx)
        {
            (*(__IO uint32_t *)(0x40006980)) |= 0x3;
            while(!((*(__IO uint32_t *)(0x4000680C)) & 0x3));
            (*(__IO uint32_t *)(0x4000680C)) = 0x38;
        }
        
        if(CAN1 == CANx)
        {
            /* 再次复位 CAN1 */
            (*(__IO uint32_t *)(0x40021010)) |= 0x2000000;
            (*(__IO uint32_t *)(0x40021010)) &= ~(0x2000000);
        }else if(CAN2 == CANx)
        {
            (*(__IO uint32_t *)(0x40021010)) |= 0x4000000;
            (*(__IO uint32_t *)(0x40021010)) &= ~(0x4000000);
        }

        /* 重新启用过滤器并设置 */
        (*(__IO uint32_t *)(0x40006600)) |= 0x1;
        (*(__IO uint32_t *)(0x4000660C)) |= 0x3;
        (*(__IO uint32_t *)(0x4000661C)) |= 0x3;
        (*(__IO uint32_t *)(0x40006600)) &= ~0x1;
        CAN_SlaveStartBank(1);
        wait_ack = 0;
    }

    /* 标准初始化流程：进入初始化模式 */
    CANx->CTLR &= (~(uint32_t)CAN_CTLR_SLEEP);   /* 清除睡眠位 */
    CANx->CTLR |= CAN_CTLR_INRQ;                 /* 设置初始化请求位 */

    /* 等待 INAK 位置位，确认进入初始化模式 */
    while(((CANx->STATR & CAN_STATR_INAK) != CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT))
    {
        wait_ack++;
    }

    if((CANx->STATR & CAN_STATR_INAK) != CAN_STATR_INAK)
    {
        InitStatus = CAN_InitStatus_Failed;      /* 超时未进入初始化模式 */
    }
    else
    {
        /* 配置 CAN 控制寄存器中的各项功能 */
        if(CAN_InitStruct->CAN_TTCM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_TTCM;          /* 使能时间触发通信模式 */
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_TTCM;
        }

        if(CAN_InitStruct->CAN_ABOM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_ABOM;          /* 使能自动离线管理 */
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_ABOM;
        }

        if(CAN_InitStruct->CAN_AWUM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_AWUM;          /* 使能自动唤醒模式 */
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_AWUM;
        }

        if(CAN_InitStruct->CAN_NART == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_NART;          /* 禁止自动重传 */
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_NART;
        }

        if(CAN_InitStruct->CAN_RFLM == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_RFLM;          /* 使能接收 FIFO 锁定模式 */
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_RFLM;
        }

        if(CAN_InitStruct->CAN_TXFP == ENABLE)
        {
            CANx->CTLR |= CAN_CTLR_TXFP;          /* 使能发送 FIFO 优先级 */
        }
        else
        {
            CANx->CTLR &= ~(uint32_t)CAN_CTLR_TXFP;
        }

        /* 配置位时序寄存器：模式、SJW、BS1、BS2、预分频器 */
        CANx->BTIMR = (uint32_t)((uint32_t)CAN_InitStruct->CAN_Mode << 30) |   /* 模式位位于 bit30-31 */
                      ((uint32_t)CAN_InitStruct->CAN_SJW << 24) |              /* SJW 位于 bit24-25 */
                      ((uint32_t)CAN_InitStruct->CAN_BS1 << 16) |              /* BS1 位于 bit16-19 */
                      ((uint32_t)CAN_InitStruct->CAN_BS2 << 20) |              /* BS2 位于 bit20-22 */
                      ((uint32_t)CAN_InitStruct->CAN_Prescaler - 1);           /* 预分频器位于 bit0-9 */

        /* 清除初始化请求位，退出初始化模式 */
        CANx->CTLR &= ~(uint32_t)CAN_CTLR_INRQ;
        wait_ack = 0;

        /* 等待 INAK 位清零，确认已进入正常模式 */
        while(((CANx->STATR & CAN_STATR_INAK) == CAN_STATR_INAK) && (wait_ack != INAK_TIMEOUT))
        {
            wait_ack++;
        }

        if((CANx->STATR & CAN_STATR_INAK) == CAN_STATR_INAK)
        {
            InitStatus = CAN_InitStatus_Failed;   /* 退出初始化模式失败 */
        }
        else
        {
            InitStatus = CAN_InitStatus_Success;  /* 成功进入正常模式 */
        }
    }

    return InitStatus;
}

/*********************************************************************
 * @fn      CAN_FilterInit
 *
 * @brief   根据 CAN_FilterInitStruct 中的参数初始化 CAN 过滤器。
 *
 * @param   CAN_FilterInitStruct - 指向 CAN_FilterInitTypeDef 结构体的指针，
 *                                  包含过滤器的配置信息。
 *
 * @return  无
 */
void CAN_FilterInit(CAN_FilterInitTypeDef *CAN_FilterInitStruct)
{
    uint32_t filter_number_bit_pos = 0;

    /* 根据过滤器编号生成对应的位掩码 (0~27) */
    filter_number_bit_pos = ((uint32_t)1) << CAN_FilterInitStruct->CAN_FilterNumber;

    /* 进入过滤器初始化模式 */
    CAN1->FCTLR |= FCTLR_FINIT;

    /* 禁用指定的过滤器（从过滤器有效寄存器 FWR 中清除对应位） */
    CAN1->FWR &= ~(uint32_t)filter_number_bit_pos;

    /* 配置过滤器尺度（16位或32位） */
    if(CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_16bit)
    {
        /* 清除 FSCFGR 中对应位，选择 16 位模式 */
        CAN1->FSCFGR &= ~(uint32_t)filter_number_bit_pos;

        /* 对于 16 位模式，FR1 寄存器存放 ID 低16位和掩码/ID 低16位 */
        /* FR1: 高16位为屏蔽/标识符低16位，低16位为标识符低16位 */
        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);

        /* FR2 寄存器存放 ID 高16位和掩码/ID 高16位 */
        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh);
    }

    if(CAN_FilterInitStruct->CAN_FilterScale == CAN_FilterScale_32bit)
    {
        /* 设置 FSCFGR 中对应位，选择 32 位模式 */
        CAN1->FSCFGR |= filter_number_bit_pos;

        /* 32 位模式下，FR1 存放完整的标识符（ID） */
        /* FR1: 高16位为 ID 高16位，低16位为 ID 低16位 */
        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR1 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdHigh) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterIdLow);

        /* FR2 存放完整的屏蔽码或标识符列表（取决于模式） */
        CAN1->sFilterRegister[CAN_FilterInitStruct->CAN_FilterNumber].FR2 =
            ((0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdHigh) << 16) |
            (0x0000FFFF & (uint32_t)CAN_FilterInitStruct->CAN_FilterMaskIdLow);
    }

    /* 配置过滤器模式：标识符掩码模式 或 标识符列表模式 */
    if(CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdMask)
    {
        /* 掩码模式：FMCFGR 对应位清零 */
        CAN1->FMCFGR &= ~(uint32_t)filter_number_bit_pos;
    }
    else /* 列表模式 */
    {
        CAN1->FMCFGR |= (uint32_t)filter_number_bit_pos;
    }

    /* 配置过滤器关联的 FIFO (FIFO0 或 FIFO1) */
    if(CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO0)
    {
        /* 关联 FIFO0：FAFIFOR 对应位清零 */
        CAN1->FAFIFOR &= ~(uint32_t)filter_number_bit_pos;
    }

    if(CAN_FilterInitStruct->CAN_FilterFIFOAssignment == CAN_Filter_FIFO1)
    {
        /* 关联 FIFO1：FAFIFOR 对应位置位 */
        CAN1->FAFIFOR |= (uint32_t)filter_number_bit_pos;
    }

    /* 激活过滤器（如果要求使能） */
    if(CAN_FilterInitStruct->CAN_FilterActivation == ENABLE)
    {
        CAN1->FWR |= filter_number_bit_pos;   /* 设置 FWR 对应位，使能过滤器 */
    }

    /* 退出过滤器初始化模式 */
    CAN1->FCTLR &= ~FCTLR_FINIT;
}

/*********************************************************************
 * @fn      CAN_StructInit
 *
 * @brief   将 CAN_InitStruct 中的每个成员初始化为默认值。
 *
 * @param   CAN_InitStruct - 指向 CAN_InitTypeDef 结构体的指针，将被初始化。
 *
 * @return  无
 */
void CAN_StructInit(CAN_InitTypeDef *CAN_InitStruct)
{
    /* 全部设置为 DISABLE 或默认值 */
    CAN_InitStruct->CAN_TTCM = DISABLE;           /* 时间触发通信模式关闭 */
    CAN_InitStruct->CAN_ABOM = DISABLE;           /* 自动离线管理关闭 */
    CAN_InitStruct->CAN_AWUM = DISABLE;           /* 自动唤醒模式关闭 */
    CAN_InitStruct->CAN_NART = DISABLE;           /* 非自动重传关闭（即允许自动重传） */
    CAN_InitStruct->CAN_RFLM = DISABLE;           /* 接收 FIFO 锁定模式关闭 */
    CAN_InitStruct->CAN_TXFP = DISABLE;           /* 发送 FIFO 优先级关闭（按标识符优先级） */
    CAN_InitStruct->CAN_Mode = CAN_Mode_Normal;   /* 正常工作模式 */
    CAN_InitStruct->CAN_SJW = CAN_SJW_1tq;        /* 重新同步跳跃宽度 1 个时间单元 */
    CAN_InitStruct->CAN_BS1 = CAN_BS1_4tq;        /* 位段1 为 4 个时间单元 */
    CAN_InitStruct->CAN_BS2 = CAN_BS2_3tq;        /* 位段2 为 3 个时间单元 */
    CAN_InitStruct->CAN_Prescaler = 1;             /* 预分频器为 1 */
}

/*********************************************************************
 * @fn      CAN_SlaveStartBank
 *
 * @brief   该函数仅适用于 CH32 互联型器件。设置从过滤器起始组。
 *
 * @param   CAN_BankNumber - 从过滤器起始组号，范围 1..27。
 *
 * @return  无
 */
void CAN_SlaveStartBank(uint8_t CAN_BankNumber)
{
    /* 进入过滤器初始化模式 */
    CAN1->FCTLR |= FCTLR_FINIT;
    /* 清除原来的起始组设置位（bit8-15 以及低4位保留） */
    CAN1->FCTLR &= (uint32_t)0xFFFFC0F1;
    /* 设置新的起始组号，左移8位存入 FCTLR 寄存器 */
    CAN1->FCTLR |= (uint32_t)(CAN_BankNumber) << 8;
    /* 退出过滤器初始化模式 */
    CAN1->FCTLR &= ~FCTLR_FINIT;
}

/*********************************************************************
 * @fn      CAN_DBGFreeze
 *
 * @brief   使能或禁用 CAN 的调试冻结功能。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void CAN_DBGFreeze(CAN_TypeDef *CANx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        CANx->CTLR |= CTLR_DBF;      /* 使能调试冻结：在调试时停止 CAN */
    }
    else
    {
        CANx->CTLR &= ~CTLR_DBF;     /* 禁用调试冻结 */
    }
}

/*********************************************************************
 * @fn      CAN_TTComModeCmd
 *
 * @brief   使能或禁用 CAN 的时间触发通信模式。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          NewState - ENABLE 或 DISABLE。
 *          注意：必须将 DLC 设置为 8，以便在 CAN 总线上发送时间戳（2字节）。
 *
 * @return  无
 */
void CAN_TTComModeCmd(CAN_TypeDef *CANx, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 使能时间触发通信模式 */
        CANx->CTLR |= CAN_CTLR_TTCM;

        /* 为每个发送邮箱设置时间戳发送请求位 TGT */
        CANx->sTxMailBox[0].TXMDTR |= ((uint32_t)CAN_TXMDT0R_TGT);
        CANx->sTxMailBox[1].TXMDTR |= ((uint32_t)CAN_TXMDT1R_TGT);
        CANx->sTxMailBox[2].TXMDTR |= ((uint32_t)CAN_TXMDT2R_TGT);
    }
    else
    {
        /* 禁用时间触发通信模式 */
        CANx->CTLR &= (uint32_t)(~(uint32_t)CAN_CTLR_TTCM);

        /* 清除每个邮箱的时间戳发送请求位 */
        CANx->sTxMailBox[0].TXMDTR &= ((uint32_t)~CAN_TXMDT0R_TGT);
        CANx->sTxMailBox[1].TXMDTR &= ((uint32_t)~CAN_TXMDT1R_TGT);
        CANx->sTxMailBox[2].TXMDTR &= ((uint32_t)~CAN_TXMDT2R_TGT);
    }
}

/*********************************************************************
 * @fn      CAN_Transmit
 *
 * @brief   启动一个消息的发送。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          TxMessage - 指向 CanTxMsg 结构体的指针，包含 CAN ID、DLC 和数据。
 *
 * @return  transmit_mailbox - 用于发送的邮箱号，或 CAN_TxStatus_NoMailBox（无空邮箱）。
 */
uint8_t CAN_Transmit(CAN_TypeDef *CANx, CanTxMsg *TxMessage)
{
    uint8_t transmit_mailbox = 0;

    /* 检查哪个发送邮箱为空（TME 位指示空） */
    if((CANx->TSTATR & CAN_TSTATR_TME0) == CAN_TSTATR_TME0)
    {
        transmit_mailbox = 0;   /* 邮箱 0 为空 */
    }
    else if((CANx->TSTATR & CAN_TSTATR_TME1) == CAN_TSTATR_TME1)
    {
        transmit_mailbox = 1;   /* 邮箱 1 为空 */
    }
    else if((CANx->TSTATR & CAN_TSTATR_TME2) == CAN_TSTATR_TME2)
    {
        transmit_mailbox = 2;   /* 邮箱 2 为空 */
    }
    else
    {
        transmit_mailbox = CAN_TxStatus_NoMailBox;  /* 无空邮箱 */
    }

    if(transmit_mailbox != CAN_TxStatus_NoMailBox)
    {
        /* 清除之前可能存在的发送请求位 */
        CANx->sTxMailBox[transmit_mailbox].TXMIR &= TMIDxR_TXRQ;

        /* 配置标识符寄存器 TXMIR */
        if(TxMessage->IDE == CAN_Id_Standard)
        {
            /* 标准标识符：ID 位于 bit21-31，RTR 位于 bit20 */
            CANx->sTxMailBox[transmit_mailbox].TXMIR |= ((TxMessage->StdId << 21) |
                                                         TxMessage->RTR);
        }
        else
        {
            /* 扩展标识符：ID 位于 bit3-31，IDE 位于 bit2，RTR 位于 bit1 */
            CANx->sTxMailBox[transmit_mailbox].TXMIR |= ((TxMessage->ExtId << 3) |
                                                         TxMessage->IDE |
                                                         TxMessage->RTR);
        }

        /* 配置数据长度码 TXMDTR */
        TxMessage->DLC &= (uint8_t)0x0000000F;          /* 确保 DLC 只取低4位 */
        CANx->sTxMailBox[transmit_mailbox].TXMDTR &= (uint32_t)0xFFFFFFF0; /* 清零 DLC 位 */
        CANx->sTxMailBox[transmit_mailbox].TXMDTR |= TxMessage->DLC;       /* 设置 DLC */

        /* 写入数据到邮箱寄存器 */
        /* TXMDLR 存放数据字节 0-3（低字节为 Data0） */
        CANx->sTxMailBox[transmit_mailbox].TXMDLR = (((uint32_t)TxMessage->Data[3] << 24) |
                                                     ((uint32_t)TxMessage->Data[2] << 16) |
                                                     ((uint32_t)TxMessage->Data[1] << 8) |
                                                     ((uint32_t)TxMessage->Data[0]));
        /* TXMDHR 存放数据字节 4-7 */
        CANx->sTxMailBox[transmit_mailbox].TXMDHR = (((uint32_t)TxMessage->Data[7] << 24) |
                                                     ((uint32_t)TxMessage->Data[6] << 16) |
                                                     ((uint32_t)TxMessage->Data[5] << 8) |
                                                     ((uint32_t)TxMessage->Data[4]));

        /* 设置发送请求位 TXRQ，启动发送 */
        CANx->sTxMailBox[transmit_mailbox].TXMIR |= TMIDxR_TXRQ;
    }

    return transmit_mailbox;  /* 返回实际使用的邮箱号或 NoMailBox */
}

/*********************************************************************
 * @fn      CAN_TransmitStatus
 *
 * @brief   检查一个消息的发送状态。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          TransmitMailbox - 用于发送的邮箱号。
 *
 * @return  状态：
 *            CAN_TxStatus_Ok      : 发送成功
 *            CAN_TxStatus_Failed  : 发送失败
 *            CAN_TxStatus_Pending : 发送中
 */
uint8_t CAN_TransmitStatus(CAN_TypeDef *CANx, uint8_t TransmitMailbox)
{
    uint32_t state = 0;

    /* 根据邮箱号读取相关的状态位：RQCP（发送完成）、TXOK（发送成功）、TME（邮箱空） */
    switch(TransmitMailbox)
    {
        case(CAN_TXMAILBOX_0):
            state = CANx->TSTATR & (CAN_TSTATR_RQCP0 | CAN_TSTATR_TXOK0 | CAN_TSTATR_TME0);
            break;

        case(CAN_TXMAILBOX_1):
            state = CANx->TSTATR & (CAN_TSTATR_RQCP1 | CAN_TSTATR_TXOK1 | CAN_TSTATR_TME1);
            break;

        case(CAN_TXMAILBOX_2):
            state = CANx->TSTATR & (CAN_TSTATR_RQCP2 | CAN_TSTATR_TXOK2 | CAN_TSTATR_TME2);
            break;

        default:
            state = CAN_TxStatus_Failed;
            break;
    }

    /* 解析状态 */
    switch(state)
    {
        case(0x0):
            state = CAN_TxStatus_Pending;   /* 没有完成位，发送仍在进行中 */
            break;

        case(CAN_TSTATR_RQCP0 | CAN_TSTATR_TME0):
            state = CAN_TxStatus_Failed;    /* 发送完成但 TXOK 未置位，发送失败 */
            break;

        case(CAN_TSTATR_RQCP1 | CAN_TSTATR_TME1):
            state = CAN_TxStatus_Failed;
            break;

        case(CAN_TSTATR_RQCP2 | CAN_TSTATR_TME2):
            state = CAN_TxStatus_Failed;
            break;

        case(CAN_TSTATR_RQCP0 | CAN_TSTATR_TXOK0 | CAN_TSTATR_TME0):
            state = CAN_TxStatus_Ok;         /* 发送完成且成功 */
            break;

        case(CAN_TSTATR_RQCP1 | CAN_TSTATR_TXOK1 | CAN_TSTATR_TME1):
            state = CAN_TxStatus_Ok;
            break;

        case(CAN_TSTATR_RQCP2 | CAN_TSTATR_TXOK2 | CAN_TSTATR_TME2):
            state = CAN_TxStatus_Ok;
            break;

        default:
            state = CAN_TxStatus_Failed;     /* 其他组合视为失败 */
            break;
    }

    return (uint8_t)state;
}

/*********************************************************************
 * @fn      CAN_CancelTransmit
 *
 * @brief   取消一个发送请求。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          Mailbox - 邮箱号，可选：
 *            CAN_TXMAILBOX_0
 *            CAN_TXMAILBOX_1
 *            CAN_TXMAILBOX_2
 *
 * @return  无
 */
void CAN_CancelTransmit(CAN_TypeDef *CANx, uint8_t Mailbox)
{
    switch(Mailbox)
    {
        case(CAN_TXMAILBOX_0):
            CANx->TSTATR |= CAN_TSTATR_ABRQ0;   /* 设置邮箱0的中止请求位 */
            break;

        case(CAN_TXMAILBOX_1):
            CANx->TSTATR |= CAN_TSTATR_ABRQ1;   /* 设置邮箱1的中止请求位 */
            break;

        case(CAN_TXMAILBOX_2):
            CANx->TSTATR |= CAN_TSTATR_ABRQ2;   /* 设置邮箱2的中止请求位 */
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      CAN_Receive
 *
 * @brief   接收一个消息。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          FIFONumber - 接收 FIFO 号，可选：
 *            CAN_FIFO0
 *            CAN_FIFO1
 *          RxMessage - 指向 CanRxMsg 结构体的指针，用于存放接收到的消息。
 *
 * @return  无
 */
void CAN_Receive(CAN_TypeDef *CANx, uint8_t FIFONumber, CanRxMsg *RxMessage)
{
    /* 读取标识符寄存器 RXMIR 中的 IDE 位（bit2），判断是标准帧还是扩展帧 */
    RxMessage->IDE = (uint8_t)0x04 & CANx->sFIFOMailBox[FIFONumber].RXMIR;

    if(RxMessage->IDE == CAN_Id_Standard)
    {
        /* 标准标识符：提取 bit21-31 作为 StdId */
        RxMessage->StdId = (uint32_t)0x000007FF & (CANx->sFIFOMailBox[FIFONumber].RXMIR >> 21);
    }
    else
    {
        /* 扩展标识符：提取 bit3-31 作为 ExtId */
        RxMessage->ExtId = (uint32_t)0x1FFFFFFF & (CANx->sFIFOMailBox[FIFONumber].RXMIR >> 3);
    }

    /* 读取 RTR 位（bit1） */
    RxMessage->RTR = (uint8_t)0x02 & CANx->sFIFOMailBox[FIFONumber].RXMIR;
    /* 读取 DLC（低4位） */
    RxMessage->DLC = (uint8_t)0x0F & CANx->sFIFOMailBox[FIFONumber].RXMDTR;
    /* 读取 FMI（过滤器匹配索引，位于 RXMDTR 的 bit8-15） */
    RxMessage->FMI = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDTR >> 8);

    /* 读取数据字节 0-3（RXMDLR 寄存器） */
    RxMessage->Data[0] = (uint8_t)0xFF & CANx->sFIFOMailBox[FIFONumber].RXMDLR;
    RxMessage->Data[1] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDLR >> 8);
    RxMessage->Data[2] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDLR >> 16);
    RxMessage->Data[3] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDLR >> 24);
    /* 读取数据字节 4-7（RXMDHR 寄存器） */
    RxMessage->Data[4] = (uint8_t)0xFF & CANx->sFIFOMailBox[FIFONumber].RXMDHR;
    RxMessage->Data[5] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDHR >> 8);
    RxMessage->Data[6] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDHR >> 16);
    RxMessage->Data[7] = (uint8_t)0xFF & (CANx->sFIFOMailBox[FIFONumber].RXMDHR >> 24);

    /* 释放 FIFO（出栈），以便下一个消息可以使用该邮箱 */
    if(FIFONumber == CAN_FIFO0)
    {
        CANx->RFIFO0 |= CAN_RFIFO0_RFOM0;   /* 设置释放 FIFO0 输出邮箱位 */
    }
    else
    {
        CANx->RFIFO1 |= CAN_RFIFO1_RFOM1;   /* 设置释放 FIFO1 输出邮箱位 */
    }
}

/*********************************************************************
 * @fn      CAN_FIFORelease
 *
 * @brief   释放指定的 FIFO（出栈）。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          FIFONumber - 接收 FIFO 号，可选：
 *            CAN_FIFO0
 *            CAN_FIFO1
 *
 * @return  无
 */
void CAN_FIFORelease(CAN_TypeDef *CANx, uint8_t FIFONumber)
{
    if(FIFONumber == CAN_FIFO0)
    {
        CANx->RFIFO0 |= CAN_RFIFO0_RFOM0;   /* 释放 FIFO0 */
    }
    else
    {
        CANx->RFIFO1 |= CAN_RFIFO1_RFOM1;   /* 释放 FIFO1 */
    }
}

/*********************************************************************
 * @fn      CAN_MessagePending
 *
 * @brief   返回指定 FIFO 中挂起的消息数量。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          FIFONumber - 接收 FIFO 号，可选：
 *            CAN_FIFO0
 *            CAN_FIFO1
 *
 * @return  消息数量（0-3）。
 */
uint8_t CAN_MessagePending(CAN_TypeDef *CANx, uint8_t FIFONumber)
{
    uint8_t message_pending = 0;

    if(FIFONumber == CAN_FIFO0)
    {
        /* RFIFO0 的低2位指示 FIFO0 中的消息数 */
        message_pending = (uint8_t)(CANx->RFIFO0 & (uint32_t)0x03);
    }
    else if(FIFONumber == CAN_FIFO1)
    {
        /* RFIFO1 的低2位指示 FIFO1 中的消息数 */
        message_pending = (uint8_t)(CANx->RFIFO1 & (uint32_t)0x03);
    }
    else
    {
        message_pending = 0;
    }

    return message_pending;
}

/*********************************************************************
 * @fn      CAN_OperatingModeRequest
 *
 * @brief   请求切换 CAN 的工作模式。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          CAN_OperatingMode - 请求的模式：
 *            CAN_OperatingMode_Initialization
 *            CAN_OperatingMode_Normal
 *            CAN_OperatingMode_Sleep
 *
 * @return  状态：
 *          CAN_ModeStatus_Failed  - 进入指定模式失败
 *          CAN_ModeStatus_Success - 成功进入指定模式
 */
uint8_t CAN_OperatingModeRequest(CAN_TypeDef *CANx, uint8_t CAN_OperatingMode)
{
    uint8_t  status = CAN_ModeStatus_Failed;
    uint32_t timeout = INAK_TIMEOUT;

    if(CAN_OperatingMode == CAN_OperatingMode_Initialization)
    {
        /* 进入初始化模式：清除睡眠位，设置初始化请求位 */
        CANx->CTLR = (uint32_t)((CANx->CTLR & (uint32_t)(~(uint32_t)CAN_CTLR_SLEEP)) | CAN_CTLR_INRQ);

        /* 等待 INAK 位置位 */
        while(((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_INAK) && (timeout != 0))
        {
            timeout--;
        }
        if((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_INAK)
        {
            status = CAN_ModeStatus_Failed;
        }
        else
        {
            status = CAN_ModeStatus_Success;
        }
    }
    else if(CAN_OperatingMode == CAN_OperatingMode_Normal)
    {
        /* 进入正常模式：清除睡眠位和初始化请求位 */
        CANx->CTLR &= (uint32_t)(~(CAN_CTLR_SLEEP | CAN_CTLR_INRQ));

        /* 等待 INAK 和 SLAK 位都清零（即模式位为 0） */
        while(((CANx->STATR & CAN_MODE_MASK) != 0) && (timeout != 0))
        {
            timeout--;
        }
        if((CANx->STATR & CAN_MODE_MASK) != 0)
        {
            status = CAN_ModeStatus_Failed;
        }
        else
        {
            status = CAN_ModeStatus_Success;
        }
    }
    else if(CAN_OperatingMode == CAN_OperatingMode_Sleep)
    {
        /* 进入睡眠模式：清除初始化请求位，设置睡眠位 */
        CANx->CTLR = (uint32_t)((CANx->CTLR & (uint32_t)(~(uint32_t)CAN_CTLR_INRQ)) | CAN_CTLR_SLEEP);

        /* 等待 SLAK 位置位 */
        while(((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_SLAK) && (timeout != 0))
        {
            timeout--;
        }
        if((CANx->STATR & CAN_MODE_MASK) != CAN_STATR_SLAK)
        {
            status = CAN_ModeStatus_Failed;
        }
        else
        {
            status = CAN_ModeStatus_Success;
        }
    }
    else
    {
        status = CAN_ModeStatus_Failed;
    }

    return (uint8_t)status;
}

/*********************************************************************
 * @fn      CAN_Sleep
 *
 * @brief   使 CAN 进入低功耗模式（睡眠）。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *
 * @return  睡眠状态：
 *            CAN_Sleep_Ok     - 成功进入睡眠
 *            CAN_Sleep_Failed - 进入睡眠失败
 */
uint8_t CAN_Sleep(CAN_TypeDef *CANx)
{
    uint8_t sleepstatus = CAN_Sleep_Failed;

    /* 清除初始化请求位，设置睡眠位 */
    CANx->CTLR = (((CANx->CTLR) & (uint32_t)(~(uint32_t)CAN_CTLR_INRQ)) | CAN_CTLR_SLEEP);

    /* 检查是否确实进入了睡眠模式（SLAK 置位，INAK 清零） */
    if((CANx->STATR & (CAN_STATR_SLAK | CAN_STATR_INAK)) == CAN_STATR_SLAK)
    {
        sleepstatus = CAN_Sleep_Ok;
    }

    return (uint8_t)sleepstatus;
}

/*********************************************************************
 * @fn      CAN_WakeUp
 *
 * @brief   唤醒 CAN（从睡眠模式退出）。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *
 * @return  唤醒状态：
 *            CAN_WakeUp_Ok     - 成功唤醒
 *            CAN_WakeUp_Failed - 唤醒失败
 */
uint8_t CAN_WakeUp(CAN_TypeDef *CANx)
{
    uint32_t wait_slak = SLAK_TIMEOUT;
    uint8_t  wakeupstatus = CAN_WakeUp_Failed;

    /* 清除睡眠位，尝试唤醒 */
    CANx->CTLR &= ~(uint32_t)CAN_CTLR_SLEEP;

    /* 等待 SLAK 位清零，表示已退出睡眠 */
    while(((CANx->STATR & CAN_STATR_SLAK) == CAN_STATR_SLAK) && (wait_slak != 0x00))
    {
        wait_slak--;
    }
    if((CANx->STATR & CAN_STATR_SLAK) != CAN_STATR_SLAK)
    {
        wakeupstatus = CAN_WakeUp_Ok;
    }

    return (uint8_t)wakeupstatus;
}

/*********************************************************************
 * @fn      CAN_GetLastErrorCode
 *
 * @brief   获取 CAN 最后一次错误代码（LEC）。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *
 * @return  错误代码：
 *            CAN_ErrorCode_NoErr           - 无错误
 *            CAN_ErrorCode_StuffErr        - 位填充错误
 *            CAN_ErrorCode_FormErr          - 格式错误
 *            CAN_ErrorCode_ACKErr           - 应答错误
 *            CAN_ErrorCode_BitRecessiveErr  - 隐性位错误
 *            CAN_ErrorCode_BitDominantErr   - 显性位错误
 *            CAN_ErrorCode_CRCErr           - CRC 错误
 *            CAN_ErrorCode_SoftwareSetErr   - 软件设置的错误
 */
uint8_t CAN_GetLastErrorCode(CAN_TypeDef *CANx)
{
    uint8_t errorcode = 0;

    /* LEC 位于 ERRSR 寄存器的低3位 */
    errorcode = (((uint8_t)CANx->ERRSR) & (uint8_t)CAN_ERRSR_LEC);

    return errorcode;
}

/*********************************************************************
 * @fn      CAN_GetReceiveErrorCounter
 *
 * @brief   获取 CAN 接收错误计数器（REC）的值。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *         注意：接收时出错，计数器加1或加8；成功接收减1或复位到120（若大于128）。
 *               当计数器超过127，CAN 控制器进入错误被动状态。
 * @return  接收错误计数器值。
 */
uint8_t CAN_GetReceiveErrorCounter(CAN_TypeDef *CANx)
{
    uint8_t counter = 0;

    /* REC 位于 ERRSR 寄存器的 bit24-30 */
    counter = (uint8_t)((CANx->ERRSR & CAN_ERRSR_REC) >> 24);

    return counter;
}

/*********************************************************************
 * @fn      CAN_GetLSBTransmitErrorCounter
 *
 * @brief   获取 9 位 CAN 发送错误计数器（TEC）的低8位。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *
 * @return  发送错误计数器低8位。
 */
uint8_t CAN_GetLSBTransmitErrorCounter(CAN_TypeDef *CANx)
{
    uint8_t counter = 0;

    /* TEC 的低8位位于 ERRSR 寄存器的 bit16-23 */
    counter = (uint8_t)((CANx->ERRSR & CAN_ERRSR_TEC) >> 16);

    return counter;
}

/*********************************************************************
 * @fn      CAN_ITConfig
 *
 * @brief   使能或禁用指定的 CAN 中断。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          CAN_IT - 指定要配置的中断源，可以是以下值的组合：
 *            CAN_IT_TME, CAN_IT_FMP0, CAN_IT_FF0, CAN_IT_FOV0,
 *            CAN_IT_FMP1, CAN_IT_FF1, CAN_IT_FOV1,
 *            CAN_IT_EWG, CAN_IT_EPV, CAN_IT_LEC,
 *            CAN_IT_ERR, CAN_IT_WKU, CAN_IT_SLK.
 *          NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void CAN_ITConfig(CAN_TypeDef *CANx, uint32_t CAN_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        CANx->INTENR |= CAN_IT;    /* 使能中断 */
    }
    else
    {
        CANx->INTENR &= ~CAN_IT;   /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      CAN_GetFlagStatus
 *
 * @brief   检查指定的 CAN 标志位是否被置位。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          CAN_FLAG - 指定要检查的标志，可以是以下之一：
 *            CAN_FLAG_EWG, CAN_FLAG_EPV, CAN_FLAG_BOF,
 *            CAN_FLAG_RQCP0, CAN_FLAG_RQCP1, CAN_FLAG_RQCP2,
 *            CAN_FLAG_FMP1, CAN_FLAG_FF1, CAN_FLAG_FOV1,
 *            CAN_FLAG_FMP0, CAN_FLAG_FF0, CAN_FLAG_FOV0,
 *            CAN_FLAG_WKU, CAN_FLAG_SLAK, CAN_FLAG_LEC.
 *
 * @return  FlagStatus - SET 或 RESET。
 */
FlagStatus CAN_GetFlagStatus(CAN_TypeDef *CANx, uint32_t CAN_FLAG)
{
    FlagStatus bitstatus = RESET;

    /* 根据标志的高位掩码确定属于哪个寄存器，然后读取相应位 */
    if((CAN_FLAG & CAN_FLAGS_ERRSR) != (uint32_t)RESET)
    {
        if((CANx->ERRSR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else if((CAN_FLAG & CAN_FLAGS_STATR) != (uint32_t)RESET)
    {
        if((CANx->STATR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else if((CAN_FLAG & CAN_FLAGS_TSTATR) != (uint32_t)RESET)
    {
        if((CANx->TSTATR & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else if((CAN_FLAG & CAN_FLAGS_RFIFO0) != (uint32_t)RESET)
    {
        if((CANx->RFIFO0 & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else /* 默认为 RFIFO1 相关标志 */
    {
        if((uint32_t)(CANx->RFIFO1 & (CAN_FLAG & 0x000FFFFF)) != (uint32_t)RESET)
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
 * @fn      CAN_ClearFlag
 *
 * @brief   清除 CAN 的挂起标志。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          CAN_FLAG - 指定要清除的标志，可以是以下之一：
 *            CAN_FLAG_RQCP0, CAN_FLAG_RQCP1, CAN_FLAG_RQCP2,
 *            CAN_FLAG_FF1, CAN_FLAG_FOV1,
 *            CAN_FLAG_FF0, CAN_FLAG_FOV0,
 *            CAN_FLAG_WKU, CAN_FLAG_SLAK, CAN_FLAG_LEC.
 *
 * @return  无
 */
void CAN_ClearFlag(CAN_TypeDef *CANx, uint32_t CAN_FLAG)
{
    uint32_t flagtmp = 0;

    if(CAN_FLAG == CAN_FLAG_LEC)
    {
        /* LEC 标志通过写 0 到 ERRSR 来清除 */
        CANx->ERRSR = (uint32_t)RESET;
    }
    else
    {
        flagtmp = CAN_FLAG & 0x000FFFFF;   /* 提取低位标志位 */

        if((CAN_FLAG & CAN_FLAGS_RFIFO0) != (uint32_t)RESET)
        {
            CANx->RFIFO0 = (uint32_t)(flagtmp);   /* 写1清除 RFIFO0 中的标志 */
        }
        else if((CAN_FLAG & CAN_FLAGS_RFIFO1) != (uint32_t)RESET)
        {
            CANx->RFIFO1 = (uint32_t)(flagtmp);   /* 写1清除 RFIFO1 中的标志 */
        }
        else if((CAN_FLAG & CAN_FLAGS_TSTATR) != (uint32_t)RESET)
        {
            CANx->TSTATR = (uint32_t)(flagtmp);   /* 写1清除 TSTATR 中的标志 */
        }
        else /* 其他标志在 STATR 寄存器中 */
        {
            CANx->STATR = (uint32_t)(flagtmp);    /* 写1清除 STATR 中的标志 */
        }
    }
}

/*********************************************************************
 * @fn      CAN_GetITStatus
 *
 * @brief   检查指定的 CAN 中断是否发生。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          CAN_IT - 指定要检查的中断源，可以是以下之一：
 *            CAN_IT_TME, CAN_IT_FMP0, CAN_IT_FF0, CAN_IT_FOV0,
 *            CAN_IT_FMP1, CAN_IT_FF1, CAN_IT_FOV1,
 *            CAN_IT_WKU, CAN_IT_SLK, CAN_IT_EWG, CAN_IT_EPV,
 *            CAN_IT_BOF, CAN_IT_LEC, CAN_IT_ERR.
 *
 * @return  ITStatus - SET 或 RESET。
 */
ITStatus CAN_GetITStatus(CAN_TypeDef *CANx, uint32_t CAN_IT)
{
    ITStatus itstatus = RESET;

    /* 首先检查中断使能寄存器中对应的位是否使能 */
    if((CANx->INTENR & CAN_IT) != RESET)
    {
        /* 根据中断源读取相应的状态寄存器位 */
        switch(CAN_IT)
        {
            case CAN_IT_TME:
                /* 发送邮箱空中断：检查 RQCP0|RQCP1|RQCP2 是否有完成事件 */
                itstatus = CheckITStatus(CANx->TSTATR, CAN_TSTATR_RQCP0 | CAN_TSTATR_RQCP1 | CAN_TSTATR_RQCP2);
                break;

            case CAN_IT_FMP0:
                /* FIFO0 消息挂号中断：检查 FMP0 位（消息数量） */
                itstatus = CheckITStatus(CANx->RFIFO0, CAN_RFIFO0_FMP0);
                break;

            case CAN_IT_FF0:
                /* FIFO0 满中断：检查 FULL0 位 */
                itstatus = CheckITStatus(CANx->RFIFO0, CAN_RFIFO0_FULL0);
                break;

            case CAN_IT_FOV0:
                /* FIFO0 溢出中断：检查 FOVR0 位 */
                itstatus = CheckITStatus(CANx->RFIFO0, CAN_RFIFO0_FOVR0);
                break;

            case CAN_IT_FMP1:
                /* FIFO1 消息挂号中断：检查 FMP1 位 */
                itstatus = CheckITStatus(CANx->RFIFO1, CAN_RFIFO1_FMP1);
                break;

            case CAN_IT_FF1:
                /* FIFO1 满中断：检查 FULL1 位 */
                itstatus = CheckITStatus(CANx->RFIFO1, CAN_RFIFO1_FULL1);
                break;

            case CAN_IT_FOV1:
                /* FIFO1 溢出中断：检查 FOVR1 位 */
                itstatus = CheckITStatus(CANx->RFIFO1, CAN_RFIFO1_FOVR1);
                break;

            case CAN_IT_WKU:
                /* 唤醒中断：检查 WKUI 位 */
                itstatus = CheckITStatus(CANx->STATR, CAN_STATR_WKUI);
                break;

            case CAN_IT_SLK:
                /* 睡眠中断：检查 SLAKI 位 */
                itstatus = CheckITStatus(CANx->STATR, CAN_STATR_SLAKI);
                break;

            case CAN_IT_EWG:
                /* 错误警告中断：检查 EWGF 位 */
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_EWGF);
                break;

            case CAN_IT_EPV:
                /* 错误被动中断：检查 EPVF 位 */
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_EPVF);
                break;

            case CAN_IT_BOF:
                /* 离线中断：检查 BOFF 位 */
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_BOFF);
                break;

            case CAN_IT_LEC:
                /* 最后一次错误码中断：检查 LEC 位是否非零 */
                itstatus = CheckITStatus(CANx->ERRSR, CAN_ERRSR_LEC);
                break;

            case CAN_IT_ERR:
                /* 错误中断（汇总）：检查 ERRI 位 */
                itstatus = CheckITStatus(CANx->STATR, CAN_STATR_ERRI);
                break;

            default:
                itstatus = RESET;
                break;
        }
    }
    else
    {
        itstatus = RESET;   /* 中断未使能 */
    }

    return itstatus;
}

/*********************************************************************
 * @fn      CAN_ClearITPendingBit
 *
 * @brief   清除 CAN 的中断挂起位。
 *
 * @param   CANx - 可以是 CAN1，选择要操作的 CAN 外设。
 *          CAN_IT - 指定要清除的中断挂起位，可以是以下之一：
 *            CAN_IT_TME, CAN_IT_FF0, CAN_IT_FOV0,
 *            CAN_IT_FF1, CAN_IT_FOV1,
 *            CAN_IT_WKU, CAN_IT_SLK,
 *            CAN_IT_EWG, CAN_IT_EPV, CAN_IT_BOF, CAN_IT_LEC, CAN_IT_ERR.
 *
 * @return  无
 */
void CAN_ClearITPendingBit(CAN_TypeDef *CANx, uint32_t CAN_IT)
{
    switch(CAN_IT)
    {
        case CAN_IT_TME:
            /* 清除发送完成中断标志（写1清除） */
            CANx->TSTATR = CAN_TSTATR_RQCP0 | CAN_TSTATR_RQCP1 | CAN_TSTATR_RQCP2;
            break;

        case CAN_IT_FF0:
            /* 清除 FIFO0 满中断标志 */
            CANx->RFIFO0 = CAN_RFIFO0_FULL0;
            break;

        case CAN_IT_FOV0:
            /* 清除 FIFO0 溢出中断标志 */
            CANx->RFIFO0 = CAN_RFIFO0_FOVR0;
            break;

        case CAN_IT_FF1:
            /* 清除 FIFO1 满中断标志 */
            CANx->RFIFO1 = CAN_RFIFO1_FULL1;
            break;

        case CAN_IT_FOV1:
            /* 清除 FIFO1 溢出中断标志 */
            CANx->RFIFO1 = CAN_RFIFO1_FOVR1;
            break;

        case CAN_IT_WKU:
            /* 清除唤醒中断标志 */
            CANx->STATR = CAN_STATR_WKUI;
            break;

        case CAN_IT_SLK:
            /* 清除睡眠中断标志 */
            CANx->STATR = CAN_STATR_SLAKI;
            break;

        case CAN_IT_EWG:
            /* 清除错误警告中断（通过清除 ERRI 和 ERRSR） */
            CANx->STATR = CAN_STATR_ERRI;   /* 清除 ERRI 位 */
            break;

        case CAN_IT_EPV:
            CANx->STATR = CAN_STATR_ERRI;
            break;

        case CAN_IT_BOF:
            CANx->STATR = CAN_STATR_ERRI;
            break;

        case CAN_IT_LEC:
            /* 清除 LEC 错误码（写0到 ERRSR）并清除 ERRI */
            CANx->ERRSR = RESET;
            CANx->STATR = CAN_STATR_ERRI;
            break;

        case CAN_IT_ERR:
            /* 清除所有错误中断（清除 ERRSR 和 ERRI） */
            CANx->ERRSR = RESET;
            CANx->STATR = CAN_STATR_ERRI;
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      CheckITStatus
 *
 * @brief   检查 CAN 寄存器中的特定位是否置位。
 *
 * @param   CAN_Reg - 要检查的寄存器值。
 *          It_Bit - 要检查的位掩码。
 *
 * @return  ITStatus - SET 或 RESET。
 */
static ITStatus CheckITStatus(uint32_t CAN_Reg, uint32_t It_Bit)
{
    ITStatus pendingbitstatus = RESET;

    if((CAN_Reg & It_Bit) != (uint32_t)RESET)
    {
        pendingbitstatus = SET;   /* 位已置位 */
    }
    else
    {
        pendingbitstatus = RESET; /* 位未置位 */
    }

    return pendingbitstatus;
}