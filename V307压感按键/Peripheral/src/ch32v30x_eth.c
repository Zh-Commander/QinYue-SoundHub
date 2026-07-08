/********************************** (C) COPYRIGHT  *******************************
* 文件名            : ch32v30x_eth.c
* 作者              : WCH
* 版本              : V1.0.0
* 日期              : 2021/06/06
* 描述              : 本文件提供了所有ETH（以太网）固件函数。
*********************************************************************************
* 版权 (c) 2021 南京沁恒微电子股份有限公司。
* 注意：本软件（修改或不修改）和二进制代码用于南京沁恒微电子股份有限公司制造的微控制器。
*******************************************************************************/

#include "ch32v30x_eth.h"
#include "ch32v30x_rcc.h"

/* 全局变量，用于追踪当前使用的发送和接收描述符 */
ETH_DMADESCTypeDef *DMATxDescToSet;   /* 指向当前要设置的发送描述符 */
ETH_DMADESCTypeDef *DMARxDescToGet;   /* 指向当前要获取的接收描述符 */
ETH_DMADESCTypeDef *DMAPTPTxDescToSet; /* 指向当前要设置的PTP发送描述符（用于时间戳） */
ETH_DMADESCTypeDef *DMAPTPRxDescToGet; /* 指向当前要获取的PTP接收描述符（用于时间戳） */

/*********************************************************************
 * @fn      ETH_DeInit
 *
 * @brief   重新初始化ETH硬件。
 *
 * @return  无
 */
#ifdef CH32V30x_D8C
void ETH_DeInit(void)
{
    /* 通过RCC复位ETH_MAC外设 */
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, ENABLE);  /* 使能复位 */
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, DISABLE); /* 释放复位 */
}
#endif

/*********************************************************************
 * @fn      ETH_StructInit
 *
 * @brief   将ETH_InitStruct结构体的每个成员初始化为默认值。
 *
 * @param   ETH_InitStruct - 指向ETH_InitTypeDef结构体的指针，该结构体将被初始化。
 *
 * @return  无
 */
void ETH_StructInit(ETH_InitTypeDef *ETH_InitStruct)
{
    /*------------------------   MAC 配置默认值 -----------------------------------*/
    ETH_InitStruct->ETH_AutoNegotiation = ETH_AutoNegotiation_Disable;      /* 禁止自动协商 */
    ETH_InitStruct->ETH_Watchdog = ETH_Watchdog_Enable;                     /* 使能看门狗 */
    ETH_InitStruct->ETH_Jabber = ETH_Jabber_Enable;                         /* 使能超长帧检测 */
    ETH_InitStruct->ETH_InterFrameGap = ETH_InterFrameGap_96Bit;            /* 帧间隔96位 */
    ETH_InitStruct->ETH_CarrierSense = ETH_CarrierSense_Enable;             /* 使能载波监听 */
    ETH_InitStruct->ETH_Speed = ETH_Speed_10M;                              /* 10M速度 */
    ETH_InitStruct->ETH_ReceiveOwn = ETH_ReceiveOwn_Enable;                 /* 使能接收自冲突 */
    ETH_InitStruct->ETH_LoopbackMode = ETH_LoopbackMode_Disable;            /* 禁止环回模式 */
    ETH_InitStruct->ETH_Mode = ETH_Mode_HalfDuplex;                         /* 半双工模式 */
    ETH_InitStruct->ETH_ChecksumOffload = ETH_ChecksumOffload_Disable;      /* 禁止校验和卸载 */
    ETH_InitStruct->ETH_RetryTransmission = ETH_RetryTransmission_Enable;   /* 使能重传 */
    ETH_InitStruct->ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable; /* 禁止自动剥离填充和CRC */
    ETH_InitStruct->ETH_BackOffLimit = ETH_BackOffLimit_10;                 /* 退避限制10 */
    ETH_InitStruct->ETH_DeferralCheck = ETH_DeferralCheck_Disable;          /* 禁止推迟检查 */
    ETH_InitStruct->ETH_ReceiveAll = ETH_ReceiveAll_Disable;                /* 禁止接收所有帧 */
    ETH_InitStruct->ETH_SourceAddrFilter = ETH_SourceAddrFilter_Disable;    /* 禁止源地址过滤 */
    ETH_InitStruct->ETH_PassControlFrames = ETH_PassControlFrames_BlockAll; /* 阻止所有控制帧 */
    ETH_InitStruct->ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Disable; /* 禁止接收广播帧 */
    ETH_InitStruct->ETH_DestinationAddrFilter = ETH_DestinationAddrFilter_Normal; /* 正常目标地址过滤 */
    ETH_InitStruct->ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;      /* 禁止混杂模式 */
    ETH_InitStruct->ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect; /* 完美过滤多播帧 */
    ETH_InitStruct->ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect; /* 完美过滤单播帧 */
    ETH_InitStruct->ETH_HashTableHigh = 0x0;                                /* 哈希表高32位 */
    ETH_InitStruct->ETH_HashTableLow = 0x0;                                 /* 哈希表低32位 */
    ETH_InitStruct->ETH_PauseTime = 0x0;                                    /* 暂停时间 */
    ETH_InitStruct->ETH_ZeroQuantaPause = ETH_ZeroQuantaPause_Disable;      /* 禁止零量子暂停 */
    ETH_InitStruct->ETH_PauseLowThreshold = ETH_PauseLowThreshold_Minus4;   /* 暂停低阈值-4 */
    ETH_InitStruct->ETH_UnicastPauseFrameDetect = ETH_UnicastPauseFrameDetect_Disable; /* 禁止单播暂停帧检测 */
    ETH_InitStruct->ETH_ReceiveFlowControl = ETH_ReceiveFlowControl_Disable; /* 禁止接收流控制 */
    ETH_InitStruct->ETH_TransmitFlowControl = ETH_TransmitFlowControl_Disable; /* 禁止发送流控制 */
    ETH_InitStruct->ETH_VLANTagComparison = ETH_VLANTagComparison_16Bit;     /* 16位VLAN比较 */
    ETH_InitStruct->ETH_VLANTagIdentifier = 0x0;                            /* VLAN标识符 */
    /*------------------------   DMA 配置默认值 -----------------------------------*/
    ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Disable; /* 禁止丢弃TCP/IP校验和错误帧 */
    ETH_InitStruct->ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable; /* 使能接收存储转发 */
    ETH_InitStruct->ETH_FlushReceivedFrame = ETH_FlushReceivedFrame_Enable;  /* 使能刷新接收帧 */
    ETH_InitStruct->ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable; /* 使能发送存储转发 */
    ETH_InitStruct->ETH_TransmitThresholdControl = ETH_TransmitThresholdControl_64Bytes; /* 发送阈值64字节 */
    ETH_InitStruct->ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable; /* 禁止转发错误帧 */
    ETH_InitStruct->ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable; /* 禁止转发过小好帧 */
    ETH_InitStruct->ETH_ReceiveThresholdControl = ETH_ReceiveThresholdControl_64Bytes; /* 接收阈值64字节 */
    ETH_InitStruct->ETH_SecondFrameOperate = ETH_SecondFrameOperate_Disable; /* 禁止第二帧操作 */
    ETH_InitStruct->ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable; /* 使能地址对齐突发 */
    ETH_InitStruct->ETH_FixedBurst = ETH_FixedBurst_Disable;                 /* 禁止固定突发 */
    ETH_InitStruct->ETH_RxDMABurstLength = ETH_RxDMABurstLength_1Beat;      /* Rx DMA突发长度1拍 */
    ETH_InitStruct->ETH_TxDMABurstLength = ETH_TxDMABurstLength_1Beat;      /* Tx DMA突发长度1拍 */
    ETH_InitStruct->ETH_DescriptorSkipLength = 0x0;                         /* 描述符跳过长 */
    ETH_InitStruct->ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_1_1; /* 轮询仲裁 Rx:Tx = 1:1 */
}

/*********************************************************************
 * @fn      ETH_Start
 *
 * @brief   使能ENET MAC和DMA的接收/发送。
 *
 * @return  无
 */
void ETH_Start(void)
{
    ETH_MACTransmissionCmd(ENABLE);   /* 使能MAC发送 */
    ETH_FlushTransmitFIFO();          /* 刷新发送FIFO */
    ETH_MACReceptionCmd(ENABLE);      /* 使能MAC接收 */
    ETH_DMATransmissionCmd(ENABLE);   /* 使能DMA发送 */
    ETH_DMAReceptionCmd(ENABLE);      /* 使能DMA接收 */
}

/*********************************************************************
 * @fn      ETH_HandleTxPkt
 *
 * @brief   发送一个数据包，从应用程序缓冲区ppkt指向的位置。
 *
 * @param   ppkt - 指向待发送的应用程序数据包缓冲区的指针。
 * @param   FrameLength - 发送数据包的大小（字节数）。
 *
 * @return  ETH_ERROR - 如果发送描述符被DMA占用（所有权属于DMA）。
 *          ETH_SUCCESS - 正确发送。
 */
uint32_t ETH_HandleTxPkt(uint8_t *ppkt, uint16_t FrameLength)
{
    uint32_t offset = 0;

    /* 检查当前发送描述符是否被DMA占用（OWN位），如果被占用则返回错误 */
    if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET)
    {
        return ETH_ERROR;
    }

    /* 将应用程序缓冲区的数据复制到发送描述符指向的缓冲区中 */
    for(offset = 0; offset < FrameLength; offset++)
    {
        (*(__IO uint8_t *)((DMATxDescToSet->Buffer1Addr) + offset)) = (*(ppkt + offset));
    }

    /* 设置发送描述符的控制缓冲区大小（即要发送的字节数） */
    DMATxDescToSet->ControlBufferSize = (FrameLength & ETH_DMATxDesc_TBS1);
    /* 设置描述符为帧的第一个段(FS)和最后一个段(LS) */
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;
    /* 将描述符所有权交给DMA（设置OWN位） */
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

    /* 如果DMA状态寄存器中的发送缓冲区不可用位(TBUS)被置位，清除它并轮询发送 */
    if((ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
    {
        ETH->DMASR = ETH_DMASR_TBUS;      /* 写1清除该位 */
        ETH->DMATPDR = 0;                 /* 触发发送轮询 */
    }

    /* 根据描述符链或环模式，更新DMATxDescToSet指向下一个描述符 */
    if((DMATxDescToSet->Status & ETH_DMATxDesc_TCH) != (uint32_t)RESET)  /* 链模式（TCH=1） */
    {
        DMATxDescToSet = (ETH_DMADESCTypeDef *)(DMATxDescToSet->Buffer2NextDescAddr);
    }
    else  /* 环模式 */
    {
        if((DMATxDescToSet->Status & ETH_DMATxDesc_TER) != (uint32_t)RESET)  /* 如果当前是环尾，则回到环首 */
        {
            DMATxDescToSet = (ETH_DMADESCTypeDef *)(ETH->DMATDLAR);          /* 从DMA发送描述符列表地址寄存器获取首地址 */
        }
        else  /* 否则跳到下一个描述符（按固定步长） */
        {
            /* 步长 = 每个描述符大小(16字节) + (描述符跳过长 << 2) */
            DMATxDescToSet = (ETH_DMADESCTypeDef *)((uint32_t)DMATxDescToSet + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));
        }
    }

    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_HandleRxPkt
 *
 * @brief   接收一个数据包并将其复制到ppkt指向的内存中。
 *
 * @param   ppkt - 指向应用程序接收缓冲区的指针。
 *
 * @return  ETH_ERROR - 如果接收过程中出现错误。
 *          framelength - 接收到的数据包大小（字节数），如果接收正确。
 */
uint32_t ETH_HandleRxPkt(uint8_t *ppkt)
{
    uint32_t offset = 0, framelength = 0;

    /* 检查当前接收描述符是否被DMA占用，如果被占用说明没有新数据，返回错误 */
    if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (uint32_t)RESET)
    {
        return ETH_ERROR;
    }

    /* 检查接收帧是否无错误(ES=0)，且是最后一帧(LS=1)和第一帧(FS=1) */
    if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) &&
       ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (uint32_t)RESET) &&
       ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (uint32_t)RESET))
    {
        /* 获取帧长度（状态寄存器中的FL字段），减去4个字节的CRC（CRC不复制到缓冲区） */
        framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARXDESC_FRAME_LENGTHSHIFT) - 4;

        /* 将接收描述符指向的缓冲区数据复制到应用程序缓冲区 */
        for(offset = 0; offset < framelength; offset++)
        {
            (*(ppkt + offset)) = (*(__IO uint8_t *)((DMARxDescToGet->Buffer1Addr) + offset));
        }
    }
    else
    {
        framelength = ETH_ERROR;  /* 接收帧有错误 */
    }

    /* 将描述符所有权归还给DMA，以便DMA可以再次使用它 */
    DMARxDescToGet->Status = ETH_DMARxDesc_OWN;

    /* 如果DMA状态寄存器中的接收缓冲区不可用位(RBUS)被置位，清除它并轮询接收 */
    if((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
    {
        ETH->DMASR = ETH_DMASR_RBUS;
        ETH->DMARPDR = 0;          /* 触发接收轮询 */
    }

    /* 根据描述符链或环模式，更新DMARxDescToGet指向下一个描述符 */
    if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RCH) != (uint32_t)RESET)  /* 链模式 */
    {
        DMARxDescToGet = (ETH_DMADESCTypeDef *)(DMARxDescToGet->Buffer2NextDescAddr);
    }
    else  /* 环模式 */
    {
        if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RER) != (uint32_t)RESET)  /* 如果是环尾 */
        {
            DMARxDescToGet = (ETH_DMADESCTypeDef *)(ETH->DMARDLAR);  /* 回到环首 */
        }
        else
        {
            /* 跳到下一个描述符 */
            DMARxDescToGet = (ETH_DMADESCTypeDef *)((uint32_t)DMARxDescToGet + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));
        }
    }

    return (framelength);
}

/*********************************************************************
 * @fn      ETH_GetRxPktSize
 *
 * @brief   获取接收到的数据包的大小。
 *
 * @return  framelength - 接收到的数据包大小（字节数）。
 */
uint32_t ETH_GetRxPktSize(void)
{
    uint32_t frameLength = 0;
    /* 检查当前描述符是否有效（无错误且是完整帧） */
    if(((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) == (uint32_t)RESET) &&
       ((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) &&
       ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (uint32_t)RESET) &&
       ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (uint32_t)RESET))
    {
        /* 获取帧长度 */
        frameLength = ETH_GetDMARxDescFrameLength(DMARxDescToGet);
    }

    return frameLength;
}

/*********************************************************************
 * @fn      ETH_DropRxPkt
 *
 * @brief   丢弃一个接收到的数据包（将描述符所有权归还给DMA）。
 *
 * @return  无
 */
void ETH_DropRxPkt(void)
{
    /* 将描述符所有权归还给DMA */
    DMARxDescToGet->Status = ETH_DMARxDesc_OWN;

    /* 更新到下一个描述符 */
    if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RCH) != (uint32_t)RESET)  /* 链模式 */
    {
        DMARxDescToGet = (ETH_DMADESCTypeDef *)(DMARxDescToGet->Buffer2NextDescAddr);
    }
    else  /* 环模式 */
    {
        if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RER) != (uint32_t)RESET)
        {
            DMARxDescToGet = (ETH_DMADESCTypeDef *)(ETH->DMARDLAR);
        }
        else
        {
            DMARxDescToGet = (ETH_DMADESCTypeDef *)((uint32_t)DMARxDescToGet + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));
        }
    }
}

/*********************************************************************
 * @fn      ETH_ReadPHYRegister
 *
 * @brief   读取PHY寄存器。
 *
 * @param   PHYAddress - PHY设备地址（0~31）。
 * @param   PHYReg - PHY寄存器地址（0~31）。
 *
 * @return  ETH_ERROR - 如果超时。
 *          MAC MIIDR寄存器的值 - 从所选PHY寄存器读取的数据。
 */
uint16_t ETH_ReadPHYRegister(uint16_t PHYAddress, uint16_t PHYReg)
{
    uint32_t      tmpreg = 0;
    __IO uint32_t timeout = 0;

    /* 配置MAC MII地址寄存器（MACMIIAR） */
    tmpreg = ETH->MACMIIAR;
    tmpreg &= ~MACMIIAR_CR_MASK;                        /* 清除时钟范围位 */
    tmpreg |= (((uint32_t)PHYAddress << 11) & ETH_MACMIIAR_PA);  /* 设置PHY地址 */
    tmpreg |= (((uint32_t)PHYReg << 6) & ETH_MACMIIAR_MR);       /* 设置PHY寄存器地址 */
    tmpreg &= ~ETH_MACMIIAR_MW;                           /* 设置为读操作（MW=0） */
    tmpreg |= ETH_MACMIIAR_MB;                             /* 置位忙标志，启动读操作 */
    ETH->MACMIIAR = tmpreg;

    /* 等待操作完成（忙标志清零）或超时 */
    do
    {
        timeout++;
        tmpreg = ETH->MACMIIAR;
    } while((tmpreg & ETH_MACMIIAR_MB) && (timeout < (uint32_t)PHY_READ_TO));  /* PHY_READ_TO定义在头文件中 */

    if(timeout == PHY_READ_TO)  /* 超时 */
    {
        return (uint16_t)ETH_ERROR;
    }

    /* 返回读出的数据 */
    return (uint16_t)(ETH->MACMIIDR);
}

/*********************************************************************
 * @fn      ETH_WritePHYRegister
 *
 * @brief   写入PHY寄存器。
 *
 * @param   PHYAddress - PHY设备地址（0~31）。
 * @param   PHYReg - PHY寄存器地址（0~31）。
 * @param   PHYValue - 要写入的值。
 *
 * @return  ETH_ERROR - 如果超时。
 *          ETH_SUCCESS - 正确写入。
 */
uint32_t ETH_WritePHYRegister(uint16_t PHYAddress, uint16_t PHYReg, uint16_t PHYValue)
{
    uint32_t      tmpreg = 0;
    __IO uint32_t timeout = 0;

    /* 配置MAC MII地址寄存器 */
    tmpreg = ETH->MACMIIAR;
    tmpreg &= ~MACMIIAR_CR_MASK;
    tmpreg |= (((uint32_t)PHYAddress << 11) & ETH_MACMIIAR_PA);
    tmpreg |= (((uint32_t)PHYReg << 6) & ETH_MACMIIAR_MR);
    tmpreg |= ETH_MACMIIAR_MW;        /* 设置为写操作（MW=1） */
    tmpreg |= ETH_MACMIIAR_MB;        /* 启动操作 */
    ETH->MACMIIDR = PHYValue;          /* 先写入数据寄存器 */
    ETH->MACMIIAR = tmpreg;

    /* 等待操作完成或超时 */
    do
    {
        timeout++;
        tmpreg = ETH->MACMIIAR;
    } while((tmpreg & ETH_MACMIIAR_MB) && (timeout < (uint32_t)PHY_WRITE_TO));

    if(timeout >= PHY_WRITE_TO)        /* 超时 */
    {
        return ETH_ERROR;
    }

    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_PHYLoopBackCmd
 *
 * @brief   使能或禁用PHY环回模式。
 *
 * @param   PHYAddress - PHY设备地址（0~31）。
 * @param   NewState - PHY环回模式的新状态（ENABLE 或 DISABLE）。
 *
 * @return  ETH_ERROR - 如果PHY配置失败。
 *          ETH_SUCCESS - PHY配置成功。
 */
uint32_t ETH_PHYLoopBackCmd(uint16_t PHYAddress, FunctionalState NewState)
{
    uint16_t tmpreg = 0;

    /* 读取PHY的基本控制寄存器（PHY_BCR，地址0） */
    tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_BCR);

    if(NewState != DISABLE)
    {
        tmpreg |= PHY_Loopback;      /* 设置环回位 */
    }
    else
    {
        tmpreg &= (uint16_t)(~(uint16_t)PHY_Loopback); /* 清除环回位 */
    }

    /* 将修改后的值写回 */
    if(ETH_WritePHYRegister(PHYAddress, PHY_BCR, tmpreg) != (uint32_t)RESET)
    {
        return ETH_SUCCESS;
    }
    else
    {
        return ETH_ERROR;
    }
}

/*********************************************************************
 * @fn      ETH_MACTransmissionCmd
 *
 * @brief   使能或禁用MAC发送。
 *
 * @param   NewState - MAC发送的新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void ETH_MACTransmissionCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACCR |= ETH_MACCR_TE;    /* 置位发送使能位 */
    }
    else
    {
        ETH->MACCR &= ~ETH_MACCR_TE;   /* 清零发送使能位 */
    }
}

/*********************************************************************
 * @fn      ETH_MACReceptionCmd
 *
 * @brief   使能或禁用MAC接收。
 *
 * @param   NewState - MAC接收的新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void ETH_MACReceptionCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACCR |= ETH_MACCR_RE;    /* 置位接收使能位 */
    }
    else
    {
        ETH->MACCR &= ~ETH_MACCR_RE;   /* 清零接收使能位 */
    }
}

/*********************************************************************
 * @fn      ETH_GetFlowControlBusyStatus
 *
 * @brief   获取流控制忙状态。
 *
 * @return  流控制忙状态位的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetFlowControlBusyStatus(void)
{
    FlagStatus bitstatus = RESET;

    if((ETH->MACFCR & ETH_MACFCR_FCBBPA) != (uint32_t)RESET)  /* 检查忙位 */
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_InitiatePauseControlFrame
 *
 * @brief   发起一个暂停控制帧（仅全双工模式）。
 *
 * @return  无
 */
void ETH_InitiatePauseControlFrame(void)
{
    /* 置位流控制寄存器中的FCBBPA位，发起暂停帧 */
    ETH->MACFCR |= ETH_MACFCR_FCBBPA;
}

/*********************************************************************
 * @fn      ETH_BackPressureActivationCmd
 *
 * @brief   使能或禁用MAC背压操作（仅半双工模式）。
 *
 * @param   NewState - MAC背压操作的新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void ETH_BackPressureActivationCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACFCR |= ETH_MACFCR_FCBBPA;    /* 使能背压 */
    }
    else
    {
        ETH->MACFCR &= ~ETH_MACFCR_FCBBPA;   /* 禁用背压 */
    }
}

/*********************************************************************
 * @fn      ETH_GetMACFlagStatus
 *
 * @brief   检查指定的以太网MAC标志是否置位。
 *
 * @param   ETH_MAC_FLAG - 要检查的标志（如ETH_MACSR_PHYSTS等）。
 *
 * @return  ETH_MAC_FLAG的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetMACFlagStatus(uint32_t ETH_MAC_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((ETH->MACSR & ETH_MAC_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_GetMACITStatus
 *
 * @brief   检查指定的以太网MAC中断是否发生。
 *
 * @param   ETH_MAC_IT - 要检查的中断源。
 *
 * @return  ETH_MAC_IT的新状态（SET 或 RESET）。
 */
ITStatus ETH_GetMACITStatus(uint32_t ETH_MAC_IT)
{
    FlagStatus bitstatus = RESET;

    if((ETH->MACSR & ETH_MAC_IT) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_MACITConfig
 *
 * @brief   使能或禁用指定的以太网MAC中断。
 *
 * @param   ETH_MAC_IT - 要配置的中断源。
 * @param   NewState - 新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void ETH_MACITConfig(uint32_t ETH_MAC_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACIMR &= (~(uint32_t)ETH_MAC_IT);  /* 清零中断屏蔽位以允许中断 */
    }
    else
    {
        ETH->MACIMR |= ETH_MAC_IT;                /* 置位中断屏蔽位以屏蔽中断 */
    }
}

/*********************************************************************
 * @fn      ETH_MACAddressConfig
 *
 * @brief   配置所选的MAC地址。
 *
 * @param   MacAddr - 要配置的MAC地址索引：
 *            ETH_MAC_Address0 - MAC地址0
 *            ETH_MAC_Address1 - MAC地址1
 *            ETH_MAC_Address2 - MAC地址2
 *            ETH_MAC_Address3 - MAC地址3
 * @param   Addr - 指向MAC地址缓冲区（6字节）的指针。
 *
 * @return  无
 */
void ETH_MACAddressConfig(uint32_t MacAddr, uint8_t *Addr)
{
    uint32_t tmpreg;

    /* 写入MAC地址高16位（字节5和4）到MAC地址高寄存器 */
    tmpreg = ((uint32_t)Addr[5] << 8) | (uint32_t)Addr[4];
    (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr)) = tmpreg;
    /* 写入MAC地址低32位（字节3、2、1、0）到MAC地址低寄存器 */
    tmpreg = ((uint32_t)Addr[3] << 24) | ((uint32_t)Addr[2] << 16) | ((uint32_t)Addr[1] << 8) | Addr[0];

    (*(__IO uint32_t *)(ETH_MAC_ADDR_LBASE + MacAddr)) = tmpreg;
}

/*********************************************************************
 * @fn      ETH_GetMACAddress
 *
 * @brief   获取所选的MAC地址。
 *
 * @param   MacAddr - 要获取的MAC地址索引（同ETH_MACAddressConfig）。
 * @param   Addr - 指向存放MAC地址的缓冲区（6字节）的指针。
 *
 * @return  无
 */
void ETH_GetMACAddress(uint32_t MacAddr, uint8_t *Addr)
{
    uint32_t tmpreg;

    /* 读取MAC地址高寄存器 */
    tmpreg = (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr));
    Addr[5] = ((tmpreg >> 8) & (uint8_t)0xFF);
    Addr[4] = (tmpreg & (uint8_t)0xFF);
    /* 读取MAC地址低寄存器 */
    tmpreg = (*(__IO uint32_t *)(ETH_MAC_ADDR_LBASE + MacAddr));
    Addr[3] = ((tmpreg >> 24) & (uint8_t)0xFF);
    Addr[2] = ((tmpreg >> 16) & (uint8_t)0xFF);
    Addr[1] = ((tmpreg >> 8) & (uint8_t)0xFF);
    Addr[0] = (tmpreg & (uint8_t)0xFF);
}

/*********************************************************************
 * @fn      ETH_MACAddressPerfectFilterCmd
 *
 * @brief   使能或禁用地址过滤模块对指定MAC地址的使用。
 *
 * @param   MacAddr - 要配置的MAC地址索引。
 * @param   NewState - 新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void ETH_MACAddressPerfectFilterCmd(uint32_t MacAddr, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        /* 置位地址使能位(AE) */
        (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr)) |= ETH_MACA1HR_AE;
    }
    else
    {
        /* 清零地址使能位 */
        (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr)) &= (~(uint32_t)ETH_MACA1HR_AE);
    }
}

/*********************************************************************
 * @fn      ETH_MACAddressFilterConfig
 *
 * @brief   设置指定以太网MAC地址的过滤类型（源地址或目标地址）。
 *
 * @param   MacAddr - 要配置的MAC地址索引。
 * @param   Filter - 过滤类型：
 *            ETH_MAC_AddressFilter_SA - 与接收帧的源地址(SA)比较
 *            ETH_MAC_AddressFilter_DA - 与接收帧的目标地址(DA)比较
 *
 * @return  无
 */
void ETH_MACAddressFilterConfig(uint32_t MacAddr, uint32_t Filter)
{
    if(Filter != ETH_MAC_AddressFilter_DA)  /* 如果是源地址过滤 */
    {
        (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr)) |= ETH_MACA1HR_SA;   /* 置位SA位 */
    }
    else  /* 目标地址过滤 */
    {
        (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr)) &= (~(uint32_t)ETH_MACA1HR_SA); /* 清零SA位 */
    }
}

/*********************************************************************
 * @fn      ETH_MACAddressMaskBytesFilterConfig
 *
 * @brief   设置指定以太网MAC地址的字节掩码（用于模糊匹配）。
 *
 * @param   MacAddr - 要配置的MAC地址索引（仅支持地址1~3）。
 * @param   MaskByte - 要屏蔽的字节（可组合）：
 *            ETH_MAC_AddressMask_Byte5 - 屏蔽字节5
 *            ETH_MAC_AddressMask_Byte4 - 屏蔽字节4
 *            ETH_MAC_AddressMask_Byte3 - 屏蔽字节3
 *            ETH_MAC_AddressMask_Byte2 - 屏蔽字节2
 *            ETH_MAC_AddressMask_Byte1 - 屏蔽字节1
 *
 * @return  无
 */
void ETH_MACAddressMaskBytesFilterConfig(uint32_t MacAddr, uint32_t MaskByte)
{
    /* 先清除原来的掩码位，然后设置新的掩码 */
    (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr)) &= (~(uint32_t)ETH_MACA1HR_MBC);
    (*(__IO uint32_t *)(ETH_MAC_ADDR_HBASE + MacAddr)) |= MaskByte;
}

/*********************************************************************
 * @fn      ETH_DMATxDescChainInit
 *
 * @brief   以链模式初始化DMA发送描述符。
 *
 * @param   DMATxDescTab - 指向第一个发送描述符列表的指针。
 * @param   TxBuff - 指向第一个发送缓冲区的指针（每个缓冲区大小固定为ETH_MAX_PACKET_SIZE）。
 * @param   TxBuffCount - 发送描述符的数量。
 *
 * @return  无
 */
void ETH_DMATxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount)
{
    uint32_t            i = 0;
    ETH_DMADESCTypeDef *DMATxDesc;

    DMATxDescToSet = DMATxDescTab;  /* 设置当前发送描述符指针 */

    for(i = 0; i < TxBuffCount; i++)
    {
        DMATxDesc = DMATxDescTab + i;
        DMATxDesc->Status = ETH_DMATxDesc_TCH | ETH_DMATxDesc_IC;  /* 初始状态：链模式使能，中断完成使能 */
        DMATxDesc->Buffer1Addr = (uint32_t)(&TxBuff[i * ETH_MAX_PACKET_SIZE]);  /* 设置缓冲区地址 */

        if(i < (TxBuffCount - 1))
        {
            DMATxDesc->Buffer2NextDescAddr = (uint32_t)(DMATxDescTab + i + 1);  /* 指向下一个描述符 */
        }
        else
        {
            DMATxDesc->Buffer2NextDescAddr = (uint32_t)DMATxDescTab;  /* 最后一个描述符指向第一个，形成链环 */
        }
    }

    /* 将第一个描述符地址写入DMA发送描述符列表地址寄存器 */
    ETH->DMATDLAR = (uint32_t)DMATxDescTab;
}

/*********************************************************************
 * @fn      ETH_DMATxDescRingInit
 *
 * @brief   以环模式初始化DMA发送描述符。
 *
 * @param   DMATxDescTab - 指向第一个发送描述符列表的指针。
 * @param   TxBuff1 - 指向第一个发送缓冲区1列表的指针。
 * @param   TxBuff2 - 指向第一个发送缓冲区2列表的指针。
 * @param   TxBuffCount - 发送描述符的数量。
 *
 * @return  无
 */
void ETH_DMATxDescRingInit(ETH_DMADESCTypeDef *DMATxDescTab, uint8_t *TxBuff1, uint8_t *TxBuff2, uint32_t TxBuffCount)
{
    uint32_t            i = 0;
    ETH_DMADESCTypeDef *DMATxDesc;

    DMATxDescToSet = DMATxDescTab;

    for(i = 0; i < TxBuffCount; i++)
    {
        DMATxDesc = DMATxDescTab + i;
        DMATxDesc->Buffer1Addr = (uint32_t)(&TxBuff1[i * ETH_MAX_PACKET_SIZE]);  /* 缓冲区1地址 */
        DMATxDesc->Buffer2NextDescAddr = (uint32_t)(&TxBuff2[i * ETH_MAX_PACKET_SIZE]); /* 缓冲区2地址（环模式下可能用作第二个缓冲区或下一个描述符？根据标准，这里可能错误，但照原样保留） */

        if(i == (TxBuffCount - 1))
        {
            DMATxDesc->Status = ETH_DMATxDesc_TER;  /* 最后一个描述符置位环尾标志TER */
        }
    }

    ETH->DMATDLAR = (uint32_t)DMATxDescTab;
}

/*********************************************************************
 * @fn      ETH_GetDMATxDescFlagStatus
 *
 * @brief   检查指定的以太网DMA发送描述符标志是否置位。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   ETH_DMATxDescFlag - 要检查的标志（如OWN、IC、LS等）。
 *
 * @return  ETH_DMATxDescFlag的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetDMATxDescFlagStatus(ETH_DMADESCTypeDef *DMATxDesc, uint32_t ETH_DMATxDescFlag)
{
    FlagStatus bitstatus = RESET;

    if((DMATxDesc->Status & ETH_DMATxDescFlag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_GetDMATxDescCollisionCount
 *
 * @brief   返回指定DMA发送描述符的碰撞计数。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 *
 * @return  碰撞计数值。
 */
uint32_t ETH_GetDMATxDescCollisionCount(ETH_DMADESCTypeDef *DMATxDesc)
{
    return ((DMATxDesc->Status & ETH_DMATxDesc_CC) >> ETH_DMATXDESC_COLLISION_COUNTSHIFT);
}

/*********************************************************************
 * @fn      ETH_SetDMATxDescOwnBit
 *
 * @brief   设置指定DMA发送描述符的OWN位（将所有权交给DMA）。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 *
 * @return  无
 */
void ETH_SetDMATxDescOwnBit(ETH_DMADESCTypeDef *DMATxDesc)
{
    DMATxDesc->Status |= ETH_DMATxDesc_OWN;
}

/*********************************************************************
 * @fn      ETH_DMATxDescTransmitITConfig
 *
 * @brief   使能或禁用指定DMA发送描述符的发送完成中断。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   NewState - 新状态（ENABLE 或 DISABLE）。
 *
 * @return  无
 */
void ETH_DMATxDescTransmitITConfig(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMATxDesc->Status |= ETH_DMATxDesc_IC;      /* 使能中断 */
    }
    else
    {
        DMATxDesc->Status &= (~(uint32_t)ETH_DMATxDesc_IC); /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      ETH_DMATxDescFrameSegmentConfig
 *
 * @brief   配置指定DMA发送描述符的帧段标志（第一个段或最后一个段）。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   DMATxDesc_FrameSegment - 要设置的段标志（ETH_DMATxDesc_FS 或 ETH_DMATxDesc_LS）。
 *
 * @return  无
 */
void ETH_DMATxDescFrameSegmentConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t DMATxDesc_FrameSegment)
{
    DMATxDesc->Status |= DMATxDesc_FrameSegment;  /* 置位对应的段标志 */
}

/*********************************************************************
 * @fn      ETH_DMATxDescChecksumInsertionConfig
 *
 * @brief   选择指定DMA发送描述符的校验和插入方式。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   DMATxDesc_Checksum - 校验和插入配置。
 *
 * @return  无
 */
void ETH_DMATxDescChecksumInsertionConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t DMATxDesc_Checksum)
{
    DMATxDesc->Status |= DMATxDesc_Checksum;  /* 设置校验和插入位 */
}

/*********************************************************************
 * @fn      ETH_DMATxDescCRCCmd
 *
 * @brief   使能或禁用DMA发送描述符的CRC插入。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   NewState - ENABLE 使能CRC插入（默认），DISABLE 禁用CRC插入。
 *
 * @return  无
 */
void ETH_DMATxDescCRCCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMATxDesc->Status &= (~(uint32_t)ETH_DMATxDesc_DC);  /* 清零DC位，使能CRC（默认） */
    }
    else
    {
        DMATxDesc->Status |= ETH_DMATxDesc_DC;               /* 置位DC位，禁用CRC */
    }
}

/*********************************************************************
 * @fn      ETH_DMATxDescEndOfRingCmd
 *
 * @brief   使能或禁用DMA发送描述符的环尾标志。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   NewState - ENABLE 将该描述符设为环尾，DISABLE 清除环尾标志。
 *
 * @return  无
 */
void ETH_DMATxDescEndOfRingCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMATxDesc->Status |= ETH_DMATxDesc_TER;    /* 置位TER */
    }
    else
    {
        DMATxDesc->Status &= (~(uint32_t)ETH_DMATxDesc_TER); /* 清零TER */
    }
}

/*********************************************************************
 * @fn      ETH_DMATxDescSecondAddressChainedCmd
 *
 * @brief   使能或禁用DMA发送描述符的第二个地址链模式（即Buffer2NextDescAddr作为下一个描述符指针）。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   NewState - ENABLE 使能链模式，DISABLE 禁用链模式。
 *
 * @return  无
 */
void ETH_DMATxDescSecondAddressChainedCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMATxDesc->Status |= ETH_DMATxDesc_TCH;    /* 置位TCH，使能链模式 */
    }
    else
    {
        DMATxDesc->Status &= (~(uint32_t)ETH_DMATxDesc_TCH); /* 清零TCH */
    }
}

/*********************************************************************
 * @fn      ETH_DMATxDescShortFramePaddingCmd
 *
 * @brief   使能或禁用DMA发送描述符的短帧填充（对于小于64字节的帧自动填充）。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   NewState - ENABLE 使能自动填充，DISABLE 禁用自动填充。
 *
 * @return  无
 */
void ETH_DMATxDescShortFramePaddingCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMATxDesc->Status &= (~(uint32_t)ETH_DMATxDesc_DP);  /* 清零DP位，使能填充 */
    }
    else
    {
        DMATxDesc->Status |= ETH_DMATxDesc_DP;               /* 置位DP位，禁用填充 */
    }
}

/*********************************************************************
 * @fn      ETH_DMATxDescTimeStampCmd
 *
 * @brief   使能或禁用DMA发送描述符的时间戳功能。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   NewState - ENABLE 使能时间戳，DISABLE 禁用时间戳。
 *
 * @return  无
 */
void ETH_DMATxDescTimeStampCmd(ETH_DMADESCTypeDef *DMATxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMATxDesc->Status |= ETH_DMATxDesc_TTSE;    /* 置位TTSE，使能时间戳 */
    }
    else
    {
        DMATxDesc->Status &= (~(uint32_t)ETH_DMATxDesc_TTSE); /* 清零TTSE */
    }
}

/*********************************************************************
 * @fn      ETH_DMATxDescBufferSizeConfig
 *
 * @brief   配置指定DMA发送描述符的缓冲区1和缓冲区2的大小。
 *
 * @param   DMATxDesc - 指向DMA发送描述符的指针。
 * @param   BufferSize1 - 缓冲区1的大小（字节数）。
 * @param   BufferSize2 - 缓冲区2的大小（字节数，如果不使用则填0）。
 *
 * @return  无
 */
void ETH_DMATxDescBufferSizeConfig(ETH_DMADESCTypeDef *DMATxDesc, uint32_t BufferSize1, uint32_t BufferSize2)
{
    /* 将缓冲区大小写入控制/缓冲区大小寄存器（低13位为缓冲区1大小，中间位为缓冲区2大小） */
    DMATxDesc->ControlBufferSize |= (BufferSize1 | (BufferSize2 << ETH_DMATXDESC_BUFFER2_SIZESHIFT));
}

/*********************************************************************
 * @fn      ETH_DMARxDescChainInit
 *
 * @brief   以链模式初始化DMA接收描述符。
 *
 * @param   DMARxDescTab - 指向第一个接收描述符列表的指针。
 * @param   RxBuff - 指向第一个接收缓冲区的指针。
 * @param   RxBuffCount - 接收描述符的数量。
 *
 * @return  无
 */
void ETH_DMARxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount)
{
    uint32_t            i = 0;
    ETH_DMADESCTypeDef *DMARxDesc;

    DMARxDescToGet = DMARxDescTab;  /* 设置当前接收描述符指针 */

    for(i = 0; i < RxBuffCount; i++)
    {
        DMARxDesc = DMARxDescTab + i;
        DMARxDesc->Status = ETH_DMARxDesc_OWN;                     /* 初始所有权给DMA */
        DMARxDesc->ControlBufferSize = ETH_DMARxDesc_RCH | (uint32_t)ETH_MAX_PACKET_SIZE; /* 使能链模式并设置缓冲区大小 */
        DMARxDesc->Buffer1Addr = (uint32_t)(&RxBuff[i * ETH_MAX_PACKET_SIZE]); /* 缓冲区地址 */

        if(i < (RxBuffCount - 1))
        {
            DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab + i + 1); /* 指向下一个描述符 */
        }
        else
        {
            DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab);         /* 最后一个指向第一个，形成环 */
        }
    }

    ETH->DMARDLAR = (uint32_t)DMARxDescTab;  /* 设置DMA接收描述符列表地址寄存器 */
}

/*********************************************************************
 * @fn      ETH_DMARxDescRingInit
 *
 * @brief   以环模式初始化DMA接收描述符。
 *
 * @param   DMARxDescTab - 指向第一个接收描述符列表的指针。
 * @param   RxBuff1 - 指向第一个接收缓冲区1列表的指针。
 * @param   RxBuff2 - 指向第一个接收缓冲区2列表的指针。
 * @param   RxBuffCount - 接收描述符的数量。
 *
 * @return  无
 */
void ETH_DMARxDescRingInit(ETH_DMADESCTypeDef *DMARxDescTab, uint8_t *RxBuff1, uint8_t *RxBuff2, uint32_t RxBuffCount)
{
    uint32_t            i = 0;
    ETH_DMADESCTypeDef *DMARxDesc;

    DMARxDescToGet = DMARxDescTab;

    for(i = 0; i < RxBuffCount; i++)
    {
        DMARxDesc = DMARxDescTab + i;
        DMARxDesc->Status = ETH_DMARxDesc_OWN;                         /* 所有权给DMA */
        DMARxDesc->ControlBufferSize = ETH_MAX_PACKET_SIZE;            /* 设置缓冲区大小 */
        DMARxDesc->Buffer1Addr = (uint32_t)(&RxBuff1[i * ETH_MAX_PACKET_SIZE]); /* 缓冲区1地址 */
        DMARxDesc->Buffer2NextDescAddr = (uint32_t)(&RxBuff2[i * ETH_MAX_PACKET_SIZE]); /* 缓冲区2地址（环模式下可能用作第二个缓冲区） */

        if(i == (RxBuffCount - 1))
        {
            DMARxDesc->ControlBufferSize |= ETH_DMARxDesc_RER;         /* 最后一个描述符置位环尾标志RER */
        }
    }

    ETH->DMARDLAR = (uint32_t)DMARxDescTab;
}

/*********************************************************************
 * @fn      ETH_GetDMARxDescFlagStatus
 *
 * @brief   检查指定的以太网DMA接收描述符标志是否置位。
 *
 * @param   DMARxDesc - 指向DMA接收描述符的指针。
 * @param   ETH_DMARxDescFlag - 要检查的标志（如OWN、ES、LS等）。
 *
 * @return  ETH_DMARxDescFlag的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetDMARxDescFlagStatus(ETH_DMADESCTypeDef *DMARxDesc, uint32_t ETH_DMARxDescFlag)
{
    FlagStatus bitstatus = RESET;

    if((DMARxDesc->Status & ETH_DMARxDescFlag) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_SetDMARxDescOwnBit
 *
 * @brief   设置指定DMA接收描述符的OWN位（将所有权交给DMA）。
 *
 * @param   DMARxDesc - 指向DMA接收描述符的指针。
 *
 * @return  无
 */
void ETH_SetDMARxDescOwnBit(ETH_DMADESCTypeDef *DMARxDesc)
{
    DMARxDesc->Status |= ETH_DMARxDesc_OWN;
}

/*********************************************************************
 * @fn      ETH_GetDMARxDescFrameLength
 *
 * @brief   返回指定DMA接收描述符的帧长度。
 *
 * @param   DMARxDesc - 指向DMA接收描述符的指针。
 *
 * @return  帧长度值。
 */
uint32_t ETH_GetDMARxDescFrameLength(ETH_DMADESCTypeDef *DMARxDesc)
{
    return ((DMARxDesc->Status & ETH_DMARxDesc_FL) >> ETH_DMARXDESC_FRAME_LENGTHSHIFT);
}

/*********************************************************************
 * @fn      ETH_DMARxDescReceiveITConfig
 *
 * @brief   使能或禁用指定DMA接收描述符的接收完成中断。
 *
 * @param   DMARxDesc - 指向DMA接收描述符的指针。
 * @param   NewState - ENABLE 使能中断，DISABLE 禁用中断。
 *
 * @return  无
 */
void ETH_DMARxDescReceiveITConfig(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMARxDesc->ControlBufferSize &= (~(uint32_t)ETH_DMARxDesc_DIC); /* 清零DIC位，使能中断 */
    }
    else
    {
        DMARxDesc->ControlBufferSize |= ETH_DMARxDesc_DIC;              /* 置位DIC位，禁用中断 */
    }
}

/*********************************************************************
 * @fn      ETH_DMARxDescEndOfRingCmd
 *
 * @brief   使能或禁用DMA接收描述符的环尾标志。
 *
 * @param   DMARxDesc - 指向DMA接收描述符的指针。
 * @param   NewState - ENABLE 将该描述符设为环尾，DISABLE 清除环尾标志。
 *
 * @return  无
 */
void ETH_DMARxDescEndOfRingCmd(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMARxDesc->ControlBufferSize |= ETH_DMARxDesc_RER;    /* 置位RER */
    }
    else
    {
        DMARxDesc->ControlBufferSize &= (~(uint32_t)ETH_DMARxDesc_RER); /* 清零RER */
    }
}

/*********************************************************************
 * @fn      ETH_DMARxDescSecondAddressChainedCmd
 *
 * @brief   使能或禁用DMA接收描述符的第二个地址链模式。
 *
 * @param   DMARxDesc - 指向DMA接收描述符的指针。
 * @param   NewState - ENABLE 使能链模式，DISABLE 禁用链模式。
 *
 * @return  无
 */
void ETH_DMARxDescSecondAddressChainedCmd(ETH_DMADESCTypeDef *DMARxDesc, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        DMARxDesc->ControlBufferSize |= ETH_DMARxDesc_RCH;    /* 置位RCH，使能链模式 */
    }
    else
    {
        DMARxDesc->ControlBufferSize &= (~(uint32_t)ETH_DMARxDesc_RCH); /* 清零RCH */
    }
}

/*********************************************************************
 * @fn      ETH_GetDMARxDescBufferSize
 *
 * @brief   返回指定DMA接收描述符的缓冲区大小（缓冲区1或缓冲区2）。
 *
 * @param   DMARxDesc - 指向DMA接收描述符的指针。
 * @param   DMARxDesc_Buffer - 指定要获取的缓冲区：
 *            ETH_DMARxDesc_Buffer1 - 获取缓冲区1大小
 *            ETH_DMARxDesc_Buffer2 - 获取缓冲区2大小
 *
 * @return  缓冲区大小值。
 */
uint32_t ETH_GetDMARxDescBufferSize(ETH_DMADESCTypeDef *DMARxDesc, uint32_t DMARxDesc_Buffer)
{
    if(DMARxDesc_Buffer != ETH_DMARxDesc_Buffer1)
    {
        /* 获取缓冲区2大小（右移16位后） */
        return ((DMARxDesc->ControlBufferSize & ETH_DMARxDesc_RBS2) >> ETH_DMARXDESC_BUFFER2_SIZESHIFT);
    }
    else
    {
        /* 获取缓冲区1大小（低13位） */
        return (DMARxDesc->ControlBufferSize & ETH_DMARxDesc_RBS1);
    }
}

/*********************************************************************
 * @fn      ETH_SoftwareReset
 *
 * @brief   复位所有MAC子系统内部寄存器和逻辑。
 *
 * @return  无
 */
void ETH_SoftwareReset(void)
{
    ETH->DMABMR |= ETH_DMABMR_SR;  /* 置位软件复位位 */
}

/*********************************************************************
 * @fn      ETH_GetSoftwareResetStatus
 *
 * @brief   检查以太网软件复位位是否置位。
 *
 * @return  软件复位位状态（SET 或 RESET）。
 */
FlagStatus ETH_GetSoftwareResetStatus(void)
{
    FlagStatus bitstatus = RESET;
    if((ETH->DMABMR & ETH_DMABMR_SR) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_GetlinkStaus
 *
 * @brief   检查内部10BASE-T PHY是否链接。
 *
 * @return  内部10BASE-T PHY链接状态（PHY_10BASE_T_LINKED 或 PHY_10BASE_T_NOT_LINKED）。
 */
FlagStatus ETH_GetlinkStaus(void)
{
    FlagStatus bitstatus = RESET;

    /* 检查DMASR寄存器的最高位（位31），该位指示内部PHY链接状态 */
    if((ETH->DMASR & 0x80000000) != (uint32_t)RESET)
    {
        bitstatus = PHY_10BASE_T_LINKED;
    }
    else
    {
        bitstatus = PHY_10BASE_T_NOT_LINKED;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_GetDMAFlagStatus
 *
 * @brief   检查指定的以太网DMA标志是否置位。
 *
 * @param   ETH_DMA_FLAG - 要检查的标志（如ETH_DMA_FLAG_NIS等）。
 *
 * @return  ETH_DMA_FLAG的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetDMAFlagStatus(uint32_t ETH_DMA_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((ETH->DMASR & ETH_DMA_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_DMAClearFlag
 *
 * @brief   清除指定的以太网DMA标志。
 *
 * @param   ETH_DMA_FLAG - 要清除的标志（写1清除）。
 *
 * @return  无
 */
void ETH_DMAClearFlag(uint32_t ETH_DMA_FLAG)
{
    ETH->DMASR = (uint32_t)ETH_DMA_FLAG;  /* 向对应位写1清除 */
}

/*********************************************************************
 * @fn      ETH_GetDMAITStatus
 *
 * @brief   检查指定的以太网DMA中断是否发生。
 *
 * @param   ETH_DMA_IT - 要检查的中断源。
 *
 * @return  ETH_DMA_IT的新状态（SET 或 RESET）。
 */
ITStatus ETH_GetDMAITStatus(uint32_t ETH_DMA_IT)
{
    ITStatus bitstatus = RESET;

    if((ETH->DMASR & ETH_DMA_IT) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_DMAClearITPendingBit
 *
 * @brief   清除以太网DMA的中断挂起位。
 *
 * @param   ETH_DMA_IT - 要清除的中断源。
 *
 * @return  无
 */
void ETH_DMAClearITPendingBit(uint32_t ETH_DMA_IT)
{
    ETH->DMASR = (uint32_t)ETH_DMA_IT;  /* 写1清除 */
}

/*********************************************************************
 * @fn      ETH_GetTransmitProcessState
 *
 * @brief   返回以太网DMA发送进程的状态。
 *
 * @return  DMA发送进程状态值（如ETH_DMA_TransmitProcess_Stopped等）。
 */
uint32_t ETH_GetTransmitProcessState(void)
{
    return ((uint32_t)(ETH->DMASR & ETH_DMASR_TS));  /* 读取TS位域 */
}

/*********************************************************************
 * @fn      ETH_GetReceiveProcessState
 *
 * @brief   返回以太网DMA接收进程的状态。
 *
 * @return  DMA接收进程状态值（如ETH_DMA_ReceiveProcess_Stopped等）。
 */
uint32_t ETH_GetReceiveProcessState(void)
{
    return ((uint32_t)(ETH->DMASR & ETH_DMASR_RS));  /* 读取RS位域 */
}

/*********************************************************************
 * @fn      ETH_FlushTransmitFIFO
 *
 * @brief   刷新以太网发送FIFO。
 *
 * @return  无
 */
void ETH_FlushTransmitFIFO(void)
{
    ETH->DMAOMR |= ETH_DMAOMR_FTF;  /* 置位刷新发送FIFO位 */
}

/*********************************************************************
 * @fn      ETH_GetFlushTransmitFIFOStatus
 *
 * @brief   检查以太网发送FIFO刷新位是否清除。
 *
 * @return  刷新发送FIFO位状态（SET 或 RESET）。
 */
FlagStatus ETH_GetFlushTransmitFIFOStatus(void)
{
    FlagStatus bitstatus = RESET;
    if((ETH->DMAOMR & ETH_DMAOMR_FTF) != (uint32_t)RESET)
    {
        bitstatus = SET;  /* 还在刷新中？实际上该位自清除，但检查是否仍在置位 */
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_DMATransmissionCmd
 *
 * @brief   使能或禁用DMA发送。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_DMATransmissionCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->DMAOMR |= ETH_DMAOMR_ST;   /* 置位发送使能位 */
    }
    else
    {
        ETH->DMAOMR &= ~ETH_DMAOMR_ST;  /* 清零发送使能位 */
    }
}

/*********************************************************************
 * @fn      ETH_DMAReceptionCmd
 *
 * @brief   使能或禁用DMA接收。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_DMAReceptionCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->DMAOMR |= ETH_DMAOMR_SR;   /* 置位接收使能位 */
    }
    else
    {
        ETH->DMAOMR &= ~ETH_DMAOMR_SR;  /* 清零接收使能位 */
    }
}

/*********************************************************************
 * @fn      ETH_DMAITConfig
 *
 * @brief   使能或禁用指定的以太网DMA中断。
 *
 * @param   ETH_DMA_IT - 要配置的中断源。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_DMAITConfig(uint32_t ETH_DMA_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->DMAIER |= ETH_DMA_IT;       /* 使能中断 */
    }
    else
    {
        ETH->DMAIER &= (~(uint32_t)ETH_DMA_IT); /* 禁用中断 */
    }
}

/*********************************************************************
 * @fn      ETH_GetDMAOverflowStatus
 *
 * @brief   检查指定的以太网DMA溢出标志是否置位。
 *
 * @param   ETH_DMA_Overflow - 要检查的溢出标志：
 *            ETH_DMA_Overflow_RxFIFOCounter - 接收FIFO溢出计数器溢出
 *            ETH_DMA_Overflow_MissedFrameCounter - 丢帧计数器溢出
 *
 * @return  ETH_DMA_Overflow的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetDMAOverflowStatus(uint32_t ETH_DMA_Overflow)
{
    FlagStatus bitstatus = RESET;

    if((ETH->DMAMFBOCR & ETH_DMA_Overflow) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_GetRxOverflowMissedFrameCounter
 *
 * @brief   获取接收溢出丢帧计数器值。
 *
 * @return  接收溢出丢帧计数器值。
 */
uint32_t ETH_GetRxOverflowMissedFrameCounter(void)
{
    return ((uint32_t)((ETH->DMAMFBOCR & ETH_DMAMFBOCR_MFA) >> ETH_DMA_RX_OVERFLOW_MISSEDFRAMES_COUNTERSHIFT));
}

/*********************************************************************
 * @fn      ETH_GetBufferUnavailableMissedFrameCounter
 *
 * @brief   获取缓冲区不可用丢帧计数器值。
 *
 * @return  缓冲区不可用丢帧计数器值。
 */
uint32_t ETH_GetBufferUnavailableMissedFrameCounter(void)
{
    return ((uint32_t)(ETH->DMAMFBOCR) & ETH_DMAMFBOCR_MFC);
}

/*********************************************************************
 * @fn      ETH_GetCurrentTxDescStartAddress
 *
 * @brief   获取当前发送描述符起始地址（DMACHTDR寄存器）。
 *
 * @return  当前发送描述符起始地址。
 */
uint32_t ETH_GetCurrentTxDescStartAddress(void)
{
    return ((uint32_t)(ETH->DMACHTDR));
}

/*********************************************************************
 * @fn      ETH_GetCurrentRxDescStartAddress
 *
 * @brief   获取当前接收描述符起始地址（DMACHRDR寄存器）。
 *
 * @return  当前接收描述符起始地址。
 */
uint32_t ETH_GetCurrentRxDescStartAddress(void)
{
    return ((uint32_t)(ETH->DMACHRDR));
}

/*********************************************************************
 * @fn      ETH_GetCurrentTxBufferAddress
 *
 * @brief   获取当前发送缓冲区地址（当前发送描述符的Buffer1Addr）。
 *
 * @return  当前发送缓冲区地址。
 */
uint32_t ETH_GetCurrentTxBufferAddress(void)
{
    return (DMATxDescToSet->Buffer1Addr);
}

/*********************************************************************
 * @fn      ETH_GetCurrentRxBufferAddress
 *
 * @brief   获取当前接收缓冲区地址（DMACHRBAR寄存器）。
 *
 * @return  当前接收缓冲区地址。
 */
uint32_t ETH_GetCurrentRxBufferAddress(void)
{
    return ((uint32_t)(ETH->DMACHRBAR));
}

/*********************************************************************
 * @fn      ETH_ResumeDMATransmission
 *
 * @brief   通过写入DMATPDR寄存器恢复DMA发送。
 *
 * @return  无
 */
void ETH_ResumeDMATransmission(void)
{
    ETH->DMATPDR = 0;  /* 写入任意值（通常为0）以恢复发送 */
}

/*********************************************************************
 * @fn      ETH_ResumeDMAReception
 *
 * @brief   通过写入DMARPDR寄存器恢复DMA接收。
 *
 * @return  无
 */
void ETH_ResumeDMAReception(void)
{
    ETH->DMARPDR = 0;  /* 写入任意值（通常为0）以恢复接收 */
}

/*********************************************************************
 * @fn      ETH_ResetWakeUpFrameFilterRegisterPointer
 *
 * @brief   复位唤醒帧过滤寄存器指针。
 *
 * @return  无
 */
void ETH_ResetWakeUpFrameFilterRegisterPointer(void)
{
    ETH->MACPMTCSR |= ETH_MACPMTCSR_WFFRPR;  /* 置位WFFRPR位，复位指针 */
}

/*********************************************************************
 * @fn      ETH_SetWakeUpFrameFilterRegister
 *
 * @brief   填充远程唤醒帧寄存器。
 *
 * @param   Buffer - 指向唤醒帧过滤寄存器缓冲区数据的指针（8个字，每个32位）。
 *
 * @return  无
 */
void ETH_SetWakeUpFrameFilterRegister(uint32_t *Buffer)
{
    uint32_t i = 0;

    for(i = 0; i < ETH_WAKEUP_REGISTER_LENGTH; i++)
    {
        ETH->MACRWUFFR = Buffer[i];  /* 依次写入8个唤醒帧过滤寄存器 */
    }
}

/*********************************************************************
 * @fn      ETH_GlobalUnicastWakeUpCmd
 *
 * @brief   使能或禁用全局单播唤醒（任何单播包唤醒）。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_GlobalUnicastWakeUpCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACPMTCSR |= ETH_MACPMTCSR_GU;   /* 置位GU位 */
    }
    else
    {
        ETH->MACPMTCSR &= ~ETH_MACPMTCSR_GU;  /* 清零GU位 */
    }
}

/*********************************************************************
 * @fn      ETH_GetPMTFlagStatus
 *
 * @brief   检查指定的以太网PMT标志是否置位。
 *
 * @param   ETH_PMT_FLAG - 要检查的标志（如ETH_PMT_FLAG_WUFF等）。
 *
 * @return  ETH_PMT_FLAG的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetPMTFlagStatus(uint32_t ETH_PMT_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((ETH->MACPMTCSR & ETH_PMT_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_WakeUpFrameDetectionCmd
 *
 * @brief   使能或禁用MAC唤醒帧检测。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_WakeUpFrameDetectionCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACPMTCSR |= ETH_MACPMTCSR_WFE;   /* 置位WFE位 */
    }
    else
    {
        ETH->MACPMTCSR &= ~ETH_MACPMTCSR_WFE;  /* 清零WFE位 */
    }
}

/*********************************************************************
 * @fn      ETH_MagicPacketDetectionCmd
 *
 * @brief   使能或禁用MAC魔术包检测。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_MagicPacketDetectionCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACPMTCSR |= ETH_MACPMTCSR_MPE;   /* 置位MPE位 */
    }
    else
    {
        ETH->MACPMTCSR &= ~ETH_MACPMTCSR_MPE;  /* 清零MPE位 */
    }
}

/*********************************************************************
 * @fn      ETH_PowerDownCmd
 *
 * @brief   使能或禁用MAC掉电模式。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_PowerDownCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MACPMTCSR |= ETH_MACPMTCSR_PD;    /* 置位PD位，进入掉电 */
    }
    else
    {
        ETH->MACPMTCSR &= ~ETH_MACPMTCSR_PD;   /* 清零PD位，退出掉电 */
    }
}

/*********************************************************************
 * @fn      ETH_MMCCounterFreezeCmd
 *
 * @brief   使能或禁用MMC计数器冻结。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_MMCCounterFreezeCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MMCCR |= ETH_MMCCR_MCF;    /* 置位MCF位，冻结计数器 */
    }
    else
    {
        ETH->MMCCR &= ~ETH_MMCCR_MCF;   /* 清零MCF位 */
    }
}

/*********************************************************************
 * @fn      ETH_MMCResetOnReadCmd
 *
 * @brief   使能或禁用MMC读后自动复位。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_MMCResetOnReadCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MMCCR |= ETH_MMCCR_ROR;    /* 置位ROR位，读后复位 */
    }
    else
    {
        ETH->MMCCR &= ~ETH_MMCCR_ROR;   /* 清零ROR位 */
    }
}

/*********************************************************************
 * @fn      ETH_MMCCounterRolloverCmd
 *
 * @brief   使能或禁用MMC计数器停止溢出（计数器满后是否回滚）。
 *
 * @param   NewState - ENABLE（使能停止溢出，即计数器满后停止）或 DISABLE（使能溢出回滚）。
 *
 * @return  无
 */
void ETH_MMCCounterRolloverCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->MMCCR &= ~ETH_MMCCR_CSR;   /* 清零CSR位，计数器溢出后停止 */
    }
    else
    {
        ETH->MMCCR |= ETH_MMCCR_CSR;    /* 置位CSR位，计数器溢出后回滚 */
    }
}

/*********************************************************************
 * @fn      ETH_MMCCountersReset
 *
 * @brief   复位MMC计数器。
 *
 * @return  无
 */
void ETH_MMCCountersReset(void)
{
    ETH->MMCCR |= ETH_MMCCR_CR;  /* 置位CR位，复位计数器 */
}

/*********************************************************************
 * @fn      ETH_MMCITConfig
 *
 * @brief   使能或禁用指定的以太网MMC中断。
 *
 * @param   ETH_MMC_IT - 要配置的MMC中断源。
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_MMCITConfig(uint32_t ETH_MMC_IT, FunctionalState NewState)
{
    /* 高28位为1表示接收中断，否则为发送中断（根据实际宏定义） */
    if((ETH_MMC_IT & (uint32_t)0x10000000) != (uint32_t)RESET)  /* 如果是接收中断（位28为1） */
    {
        ETH_MMC_IT &= 0xEFFFFFFF;  /* 清除位28，得到实际中断位 */

        if(NewState != DISABLE)
        {
            ETH->MMCRIMR &= (~(uint32_t)ETH_MMC_IT);  /* 清零屏蔽位，使能中断 */
        }
        else
        {
            ETH->MMCRIMR |= ETH_MMC_IT;                /* 置位屏蔽位，禁用中断 */
        }
    }
    else  /* 发送中断 */
    {
        if(NewState != DISABLE)
        {
            ETH->MMCTIMR &= (~(uint32_t)ETH_MMC_IT);  /* 清零屏蔽位，使能中断 */
        }
        else
        {
            ETH->MMCTIMR |= ETH_MMC_IT;                /* 置位屏蔽位，禁用中断 */
        }
    }
}

/*********************************************************************
 * @fn      ETH_GetMMCITStatus
 *
 * @brief   检查指定的以太网MMC中断是否发生。
 *
 * @param   ETH_MMC_IT - 要检查的MMC中断源。
 *
 * @return  ETH_MMC_IT的新状态（SET 或 RESET）。
 */
ITStatus ETH_GetMMCITStatus(uint32_t ETH_MMC_IT)
{
    ITStatus bitstatus = RESET;

    if((ETH_MMC_IT & (uint32_t)0x10000000) != (uint32_t)RESET)  /* 接收中断 */
    {
        /* 检查对应中断标志位，并且屏蔽位为0（中断使能） */
        if((((ETH->MMCRIR & ETH_MMC_IT) != (uint32_t)RESET)) && ((ETH->MMCRIMR & ETH_MMC_IT) != (uint32_t)RESET))
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else  /* 发送中断 */
    {
        if((((ETH->MMCTIR & ETH_MMC_IT) != (uint32_t)RESET)) && ((ETH->MMCTIMR & ETH_MMC_IT) != (uint32_t)RESET))
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
 * @fn      ETH_GetMMCRegister
 *
 * @brief   获取指定的以太网MMC寄存器值。
 *
 * @param   ETH_MMCReg - 要读取的MMC寄存器（如ETH_MMCCR、ETH_MMCRIR等）。
 *
 * @return  MMC寄存器值。
 */
uint32_t ETH_GetMMCRegister(uint32_t ETH_MMCReg)
{
    return (*(__IO uint32_t *)(ETH_MAC_BASE + ETH_MMCReg));
}

/*********************************************************************
 * @fn      ETH_EnablePTPTimeStampAddend
 *
 * @brief   用时间戳加数寄存器更新PTP模块（用于精调）。
 *
 * @return  无
 */
void ETH_EnablePTPTimeStampAddend(void)
{
    ETH->PTPTSCR |= ETH_PTPTSCR_TSARU;  /* 置位TSARU位，触发更新 */
}

/*********************************************************************
 * @fn      ETH_EnablePTPTimeStampInterruptTrigger
 *
 * @brief   使能PTP时间戳中断触发。
 *
 * @return  无
 */
void ETH_EnablePTPTimeStampInterruptTrigger(void)
{
    ETH->PTPTSCR |= ETH_PTPTSCR_TSITE;  /* 置位TSITE位，使能中断触发 */
}

/*********************************************************************
 * @fn      ETH_EnablePTPTimeStampUpdate
 *
 * @brief   用时间戳更新寄存器的值更新PTP系统时间。
 *
 * @return  无
 */
void ETH_EnablePTPTimeStampUpdate(void)
{
    ETH->PTPTSCR |= ETH_PTPTSCR_TSSTU;  /* 置位TSSTU位，触发更新 */
}

/*********************************************************************
 * @fn      ETH_InitializePTPTimeStamp
 *
 * @brief   初始化PTP时间戳。
 *
 * @return  无
 */
void ETH_InitializePTPTimeStamp(void)
{
    ETH->PTPTSCR |= ETH_PTPTSCR_TSSTI;  /* 置位TSSTI位，初始化 */
}

/*********************************************************************
 * @fn      ETH_PTPUpdateMethodConfig
 *
 * @brief   选择PTP更新方法（粗调或细调）。
 *
 * @param   UpdateMethod - 更新方法：
 *            ETH_PTP_CoarseUpdate - 粗调
 *            ETH_PTP_FineUpdate    - 细调
 *
 * @return  无
 */
void ETH_PTPUpdateMethodConfig(uint32_t UpdateMethod)
{
    if(UpdateMethod != ETH_PTP_CoarseUpdate)  /* 如果是细调 */
    {
        ETH->PTPTSCR |= ETH_PTPTSCR_TSFCU;    /* 置位TSFCU位，选择细调 */
    }
    else
    {
        ETH->PTPTSCR &= (~(uint32_t)ETH_PTPTSCR_TSFCU); /* 清零TSFCU位，选择粗调 */
    }
}

/*********************************************************************
 * @fn      ETH_PTPTimeStampCmd
 *
 * @brief   使能或禁用发送和接收帧的PTP时间戳。
 *
 * @param   NewState - ENABLE 或 DISABLE。
 *
 * @return  无
 */
void ETH_PTPTimeStampCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        ETH->PTPTSCR |= ETH_PTPTSCR_TSE;    /* 置位TSE位，使能时间戳 */
    }
    else
    {
        ETH->PTPTSCR &= (~(uint32_t)ETH_PTPTSCR_TSE); /* 清零TSE位，禁用时间戳 */
    }
}

/*********************************************************************
 * @fn      ETH_GetPTPFlagStatus
 *
 * @brief   检查指定的以太网PTP标志是否置位。
 *
 * @param   ETH_PTP_FLAG - 要检查的标志（如ETH_PTP_FLAG_TSS等）。
 *
 * @return  ETH_PTP_FLAG的新状态（SET 或 RESET）。
 */
FlagStatus ETH_GetPTPFlagStatus(uint32_t ETH_PTP_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((ETH->PTPTSCR & ETH_PTP_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      ETH_SetPTPSubSecondIncrement
 *
 * @brief   设置系统时间亚秒递增步长。
 *
 * @param   SubSecondValue - PTP亚秒递增寄存器值。
 *
 * @return  无
 */
void ETH_SetPTPSubSecondIncrement(uint32_t SubSecondValue)
{
    ETH->PTPSSIR = SubSecondValue;  /* 写入亚秒递增寄存器 */
}

/*********************************************************************
 * @fn      ETH_SetPTPTimeStampUpdate
 *
 * @brief   设置时间戳更新的符号和值。
 *
 * @param   Sign - 更新时间符号（正或负）。
 * @param   SecondValue - 更新的秒值。
 * @param   SubSecondValue - 更新的亚秒值。
 *
 * @return  无
 */
void ETH_SetPTPTimeStampUpdate(uint32_t Sign, uint32_t SecondValue, uint32_t SubSecondValue)
{
    ETH->PTPTSHUR = SecondValue;                /* 写入秒更新高值（实际是秒值） */
    ETH->PTPTSLUR = Sign | SubSecondValue;      /* 写入符号和亚秒值 */
}

/*********************************************************************
 * @fn      ETH_SetPTPTimeStampAddend
 *
 * @brief   设置时间戳加数寄存器的值。
 *
 * @param   Value - PTP时间戳加数寄存器值。
 *
 * @return  无
 */
void ETH_SetPTPTimeStampAddend(uint32_t Value)
{
    ETH->PTPTSAR = Value;  /* 写入加数寄存器 */
}

/*********************************************************************
 * @fn      ETH_SetPTPTargetTime
 *
 * @brief   设置目标时间寄存器的值。
 *
 * @param   HighValue - 目标时间高寄存器值。
 * @param   LowValue - 目标时间低寄存器值。
 *
 * @return  无
 */
void ETH_SetPTPTargetTime(uint32_t HighValue, uint32_t LowValue)
{
    ETH->PTPTTHR = HighValue;  /* 写入目标时间高寄存器 */
    ETH->PTPTTLR = LowValue;   /* 写入目标时间低寄存器 */
}

/*********************************************************************
 * @fn      ETH_GetPTPRegister
 *
 * @brief   获取指定的以太网PTP寄存器值。
 *
 * @param   ETH_PTPReg - 要读取的PTP寄存器（如ETH_PTPTSCR、ETH_PTPSSIR等）。
 *
 * @return  PTP寄存器值。
 */
uint32_t ETH_GetPTPRegister(uint32_t ETH_PTPReg)
{
    return (*(__IO uint32_t *)(ETH_MAC_BASE + ETH_PTPReg));
}

/*********************************************************************
 * @fn      ETH_DMAPTPTxDescChainInit
 *
 * @brief   以链模式初始化DMA发送描述符（带PTP时间戳）。
 *
 * @param   DMATxDescTab - 指向第一个发送描述符列表的指针。
 * @param   DMAPTPTxDescTab - 指向第一个PTP发送描述符列表的指针（用于存储时间戳）。
 * @param   TxBuff - 指向第一个发送缓冲区的指针。
 * @param   TxBuffCount - 发送描述符的数量。
 *
 * @return  无
 */
void ETH_DMAPTPTxDescChainInit(ETH_DMADESCTypeDef *DMATxDescTab, ETH_DMADESCTypeDef *DMAPTPTxDescTab,
                               uint8_t *TxBuff, uint32_t TxBuffCount)
{
    uint32_t            i = 0;
    ETH_DMADESCTypeDef *DMATxDesc;

    DMATxDescToSet = DMATxDescTab;
    DMAPTPTxDescToSet = DMAPTPTxDescTab;

    for(i = 0; i < TxBuffCount; i++)
    {
        DMATxDesc = DMATxDescTab + i;
        DMATxDesc->Status = ETH_DMATxDesc_TCH | ETH_DMATxDesc_TTSE; /* 链模式使能，时间戳使能 */
        DMATxDesc->Buffer1Addr = (uint32_t)(&TxBuff[i * ETH_MAX_PACKET_SIZE]);

        if(i < (TxBuffCount - 1))
        {
            DMATxDesc->Buffer2NextDescAddr = (uint32_t)(DMATxDescTab + i + 1);
        }
        else
        {
            DMATxDesc->Buffer2NextDescAddr = (uint32_t)DMATxDescTab;
        }

        /* 初始化对应的PTP描述符，缓冲区地址和下一个描述符地址与普通描述符相同 */
        (&DMAPTPTxDescTab[i])->Buffer1Addr = DMATxDesc->Buffer1Addr;
        (&DMAPTPTxDescTab[i])->Buffer2NextDescAddr = DMATxDesc->Buffer2NextDescAddr;
    }

    /* 最后一个PTP描述符的状态字段用于存储PTP描述符列表首地址（形成PTP链） */
    (&DMAPTPTxDescTab[i - 1])->Status = (uint32_t)DMAPTPTxDescTab;

    ETH->DMATDLAR = (uint32_t)DMATxDescTab;
}

/*********************************************************************
 * @fn      ETH_DMAPTPRxDescChainInit
 *
 * @brief   以链模式初始化DMA接收描述符（带PTP时间戳）。
 *
 * @param   DMARxDescTab - 指向第一个接收描述符列表的指针。
 * @param   DMAPTPRxDescTab - 指向第一个PTP接收描述符列表的指针（用于存储时间戳）。
 * @param   RxBuff - 指向第一个接收缓冲区的指针。
 * @param   RxBuffCount - 接收描述符的数量。
 *
 * @return  无
 */
void ETH_DMAPTPRxDescChainInit(ETH_DMADESCTypeDef *DMARxDescTab, ETH_DMADESCTypeDef *DMAPTPRxDescTab,
                               uint8_t *RxBuff, uint32_t RxBuffCount)
{
    uint32_t            i = 0;
    ETH_DMADESCTypeDef *DMARxDesc;

    DMARxDescToGet = DMARxDescTab;
    DMAPTPRxDescToGet = DMAPTPRxDescTab;

    for(i = 0; i < RxBuffCount; i++)
    {
        DMARxDesc = DMARxDescTab + i;
        DMARxDesc->Status = ETH_DMARxDesc_OWN;                     /* 所有权给DMA */
        DMARxDesc->ControlBufferSize = ETH_DMARxDesc_RCH | (uint32_t)ETH_MAX_PACKET_SIZE; /* 链模式使能 */
        DMARxDesc->Buffer1Addr = (uint32_t)(&RxBuff[i * ETH_MAX_PACKET_SIZE]);

        if(i < (RxBuffCount - 1))
        {
            DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab + i + 1);
        }
        else
        {
            DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab);
        }

        /* 初始化对应的PTP接收描述符 */
        (&DMAPTPRxDescTab[i])->Buffer1Addr = DMARxDesc->Buffer1Addr;
        (&DMAPTPRxDescTab[i])->Buffer2NextDescAddr = DMARxDesc->Buffer2NextDescAddr;
    }

    /* 最后一个PTP描述符的状态字段用于存储PTP描述符列表首地址 */
    (&DMAPTPRxDescTab[i - 1])->Status = (uint32_t)DMAPTPRxDescTab;
    ETH->DMARDLAR = (uint32_t)DMARxDescTab;
}

/*********************************************************************
 * @fn      ETH_HandlePTPTxPkt
 *
 * @brief   发送一个带时间戳的数据包，并将时间戳值存储到PTPTxTab中。
 *
 * @param   ppkt - 指向待发送的应用程序数据包缓冲区的指针。
 * @param   FrameLength - 发送数据包的大小（字节数）。
 * @param   PTPTxTab - 指向存储时间戳值的PTP发送表的指针（至少2个32位字：时间戳高和低）。
 *
 * @return  ETH_ERROR 或 ETH_SUCCESS。
 */
uint32_t ETH_HandlePTPTxPkt(uint8_t *ppkt, uint16_t FrameLength, uint32_t *PTPTxTab)
{
    uint32_t offset = 0, timeout = 0;

    /* 检查当前发送描述符是否被DMA占用 */
    if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (uint32_t)RESET)
    {
        return ETH_ERROR;
    }

    /* 复制数据到发送缓冲区（注意使用PTP描述符的Buffer1Addr，它与普通描述符相同） */
    for(offset = 0; offset < FrameLength; offset++)
    {
        (*(__IO uint8_t *)((DMAPTPTxDescToSet->Buffer1Addr) + offset)) = (*(ppkt + offset));
    }

    DMATxDescToSet->ControlBufferSize = (FrameLength & (uint32_t)0x1FFF); /* 设置帧长度 */
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;        /* 设置为完整帧 */
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;                          /* 所有权交给DMA */

    /* 如果DMA状态寄存器中的发送缓冲区不可用位被置位，清除并轮询 */
    if((ETH->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
    {
        ETH->DMASR = ETH_DMASR_TBUS;
        ETH->DMATPDR = 0;
    }

    /* 等待时间戳状态位TTSS置位（表示时间戳已捕获） */
    do
    {
        timeout++;
    } while(!(DMATxDescToSet->Status & ETH_DMATxDesc_TTSS) && (timeout < 0xFFFF));

    if(timeout == PHY_READ_TO)  /* 超时 */
    {
        return ETH_ERROR;
    }

    DMATxDescToSet->Status &= ~ETH_DMATxDesc_TTSS;  /* 清除时间戳状态位 */
    /* 存储时间戳值（假设时间戳存储在描述符的Buffer1Addr和Buffer2NextDescAddr？实际可能由硬件存储在其他位置，但这里按照代码原样存储这两个地址值，可能不正确，但保留原样） */
    *PTPTxTab++ = DMATxDescToSet->Buffer1Addr;
    *PTPTxTab = DMATxDescToSet->Buffer2NextDescAddr;

    /* 更新到下一个描述符 */
    if((DMATxDescToSet->Status & ETH_DMATxDesc_TCH) != (uint32_t)RESET)  /* 链模式 */
    {
        DMATxDescToSet = (ETH_DMADESCTypeDef *)(DMAPTPTxDescToSet->Buffer2NextDescAddr);
        if(DMAPTPTxDescToSet->Status != 0)
        {
            DMAPTPTxDescToSet = (ETH_DMADESCTypeDef *)(DMAPTPTxDescToSet->Status);
        }
        else
        {
            DMAPTPTxDescToSet++;
        }
    }
    else  /* 环模式 */
    {
        if((DMATxDescToSet->Status & ETH_DMATxDesc_TER) != (uint32_t)RESET)
        {
            DMATxDescToSet = (ETH_DMADESCTypeDef *)(ETH->DMATDLAR);
            DMAPTPTxDescToSet = (ETH_DMADESCTypeDef *)(ETH->DMATDLAR);
        }
        else
        {
            DMATxDescToSet = (ETH_DMADESCTypeDef *)((uint32_t)DMATxDescToSet + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));
            DMAPTPTxDescToSet = (ETH_DMADESCTypeDef *)((uint32_t)DMAPTPTxDescToSet + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));
        }
    }

    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_HandlePTPRxPkt
 *
 * @brief   接收一个带时间戳的数据包，并将时间戳值存储到PTPRxTab中。
 *
 * @param   ppkt - 指向应用程序接收缓冲区的指针。
 * @param   PTPRxTab - 指向存储时间戳值的PTP接收表的指针。
 *
 * @return  ETH_ERROR 或接收到的帧长度。
 */
uint32_t ETH_HandlePTPRxPkt(uint8_t *ppkt, uint32_t *PTPRxTab)
{
    uint32_t offset = 0, framelength = 0;

    /* 检查当前接收描述符是否被DMA占用 */
    if((DMARxDescToGet->Status & ETH_DMARxDesc_OWN) != (uint32_t)RESET)
    {
        return ETH_ERROR;
    }
    /* 检查帧是否正确且完整 */
    if(((DMARxDescToGet->Status & ETH_DMARxDesc_ES) == (uint32_t)RESET) &&
       ((DMARxDescToGet->Status & ETH_DMARxDesc_LS) != (uint32_t)RESET) &&
       ((DMARxDescToGet->Status & ETH_DMARxDesc_FS) != (uint32_t)RESET))
    {
        framelength = ((DMARxDescToGet->Status & ETH_DMARxDesc_FL) >> ETH_DMARXDESC_FRAME_LENGTHSHIFT) - 4; /* 获取帧长度减去CRC */

        /* 复制数据到应用程序缓冲区 */
        for(offset = 0; offset < framelength; offset++)
        {
            (*(ppkt + offset)) = (*(__IO uint8_t *)((DMAPTPRxDescToGet->Buffer1Addr) + offset));
        }
    }
    else
    {
        framelength = ETH_ERROR;
    }

    /* 如果DMA状态寄存器中的接收缓冲区不可用位被置位，清除并轮询 */
    if((ETH->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
    {
        ETH->DMASR = ETH_DMASR_RBUS;
        ETH->DMARPDR = 0;
    }

    /* 存储时间戳值（同样存储描述符地址作为时间戳？可能不正确，但保留原样） */
    *PTPRxTab++ = DMARxDescToGet->Buffer1Addr;
    *PTPRxTab = DMARxDescToGet->Buffer2NextDescAddr;
    DMARxDescToGet->Status |= ETH_DMARxDesc_OWN;  /* 归还所有权给DMA */

    /* 更新到下一个描述符 */
    if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RCH) != (uint32_t)RESET)  /* 链模式 */
    {
        DMARxDescToGet = (ETH_DMADESCTypeDef *)(DMAPTPRxDescToGet->Buffer2NextDescAddr);
        if(DMAPTPRxDescToGet->Status != 0)
        {
            DMAPTPRxDescToGet = (ETH_DMADESCTypeDef *)(DMAPTPRxDescToGet->Status);
        }
        else
        {
            DMAPTPRxDescToGet++;
        }
    }
    else  /* 环模式 */
    {
        if((DMARxDescToGet->ControlBufferSize & ETH_DMARxDesc_RER) != (uint32_t)RESET)
        {
            DMARxDescToGet = (ETH_DMADESCTypeDef *)(ETH->DMARDLAR);
        }
        else
        {
            DMARxDescToGet = (ETH_DMADESCTypeDef *)((uint32_t)DMARxDescToGet + 0x10 + ((ETH->DMABMR & ETH_DMABMR_DSL) >> 2));
        }
    }

    return (framelength);
}

/*********************************************************************
 * @fn      RGMII_TXC_Delay
 *
 * @brief   设置RGMII TXC时钟延迟。
 *
 * @param   clock_polarity - 时钟极性（1 翻转，0 不翻转）。
 * @param   delay_time - 延迟时间（0~7）。
 *
 * @return  无
 */
void RGMII_TXC_Delay(uint8_t clock_polarity, uint8_t delay_time)
{
    if(clock_polarity)
    {
        ETH->MACCR |= (uint32_t)(1 << 1);  /* 置位位1，可能用于极性控制 */
    }
    else
    {
        ETH->MACCR &= ~(uint32_t)(1 << 1); /* 清零位1 */
    }
    if(delay_time <= 7)
    {
        ETH->MACCR &= ~(uint32_t)(7 << 29);   /* 清除延迟位（位31-29） */
        ETH->MACCR |= (uint32_t)(delay_time << 29); /* 设置延迟值 */
    }
}