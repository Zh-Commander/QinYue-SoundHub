/********************************** (C) COPYRIGHT *******************************
 * �ļ���          : sdio.c
 * ����             : WCH
 * �汾            : V1.0.1
 * ����               : 2025/01/09
 * ����        : ���ļ�����SDIO����������
 *********************************************************************************
 * ��Ȩ���� (c) 2021 �Ͼ��ߺ�΢���ӹɷ����޹�˾��
 * ע�⣺���������޸Ļ�δ�޸ģ�����������ļ�����
 *       �Ͼ��ߺ�΢���������΢��������
 *******************************************************************************/
#include "sdio.h"
#include "string.h"


SDIO_InitTypeDef SDIO_InitStructure;         /* SDIO��ʼ���ṹ�� */
SDIO_CmdInitTypeDef SDIO_CmdInitStructure;   /* SDIO�����ʼ���ṹ�� */
SDIO_DataInitTypeDef SDIO_DataInitStructure; /* SDIO���ݳ�ʼ���ṹ�� */

/* �������� */
SD_Error CmdError(void);
SD_Error CmdResp7Error(void);
SD_Error CmdResp1Error(u8 cmd);
SD_Error CmdResp3Error(void);
SD_Error CmdResp2Error(void);
SD_Error CmdResp6Error(u8 cmd, u16 *prca);
SD_Error SDEnWideBus(u8 enx);
SD_Error IsCardProgramming(u8 *pstatus);
SD_Error FindSCR(u16 rca, u32 *pscr);
u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes);


static u8 CardType = SDIO_STD_CAPACITY_SD_CARD_V1_1;  /* �����ͣ�Ĭ��Ϊ��׼����SD��V1.1 */
static u32 CSD_Tab[4], CID_Tab[4], RCA = 0;           /* CSD�Ĵ������ݣ�CID�Ĵ������ݣ���Կ���ַ */
static u8 DeviceMode = SD_DMA_MODE;                   /* �豸ģʽ��Ĭ��ΪDMAģʽ */
static u8 StopCondition = 0;                          /* ֹͣ������־ */
volatile SD_Error TransferError = SD_OK;              /* ��������־ */
volatile u8 TransferEnd = 0;                          /* ���������־ */
SD_CardInfo SDCardInfo;                               /* SD����Ϣ�ṹ�� */

__attribute__((aligned(4))) u8 SDIO_DATA_BUFFER[512]; /* 512�ֽڵ�SDIO���ݻ�������4�ֽڶ��� */

/*********************************************************************
 * @fn      SD_Init
 *
 * @brief   ��ʼ��SDIO��
 *
 * @return  errorstatus: �������
 */
SD_Error SD_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;                                         /* NVIC�ж����ýṹ�� */
    GPIO_InitTypeDef GPIO_InitStructure;                                         /* GPIO��ʼ���ṹ�� */

    u8 clkdiv = 0;                                                               /* ʱ�ӷ�Ƶϵ�� */
    SD_Error errorstatus = SD_OK;                                                /* ����״̬����ʼ��Ϊ�ɹ� */

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); /* ʹ��GPIOC��GPIODʱ�� */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO | RCC_AHBPeriph_DMA2, ENABLE);      /* ʹ��SDIO��DMA2ʱ�� */

    /* ����SDIO�����ߺ�ʱ�������� */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12; /* PC8~PC12: D0,D1,D2,D3,CLK */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                                                  /* ����������� */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                                /* 50MHz�ٶ� */
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;         /* PD2: CMD */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   /* ����������� */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /* 50MHz�ٶ� */
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    SDIO_DeInit(); /* ��λSDIO�Ĵ��� */

    /* ����SDIO�ж� */
    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;           /* SDIO�ж�ͨ�� */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; /* 抢占优先级为0 */
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        /* �����ȼ�Ϊ0 */
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           /* ʹ���ж�ͨ�� */
    NVIC_Init(&NVIC_InitStructure);

    errorstatus = SD_PowerON(); /* SD���ϵ� */

    if (errorstatus == SD_OK)
    {
        errorstatus = SD_InitializeCards(); /* ��ʼ��SD�� */
    }

    if (errorstatus == SD_OK)
    {
        errorstatus = SD_GetCardInfo(&SDCardInfo); /* ��ȡSD����Ϣ */
    }

    if (errorstatus == SD_OK)
    {
        errorstatus = SD_SelectDeselect((u32)(SDCardInfo.RCA << 16)); /* ѡ��SD�� */
    }

    if (errorstatus == SD_OK)
    {
        errorstatus = SD_EnableWideBusOperation(1); /* ʹ�ܿ�����ģʽ��4λ�� */
    }

    if ((errorstatus == SD_OK) || (SDIO_MULTIMEDIA_CARD == CardType))
    {
        /* ���ݿ���������ʱ�ӷ�Ƶϵ�� */
        if (SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1 || SDCardInfo.CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0)
        {
            clkdiv = SDIO_TRANSFER_CLK_DIV + 6; /* ��׼����SD��ʹ�ýϵ��ʱ����降低时钟频率减少EMI干扰 */
        }
        else
        {
            clkdiv = SDIO_TRANSFER_CLK_DIV + 1;      /* ����������ʹ�ýϵ��ʱ����降低时钟频率减少EMI干扰 */
        }
        SDIO_Clock_Set(clkdiv);                      /* ����SDIOʱ�� */

        errorstatus = SD_SetDeviceMode(SD_DMA_MODE); /* �����豸ΪDMAģʽ */
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       DIO_Clock_Set
 *
 * @brief   ����SDIOʱ�ӡ�
 *
 * @param  clkdiv - ʱ�ӷ�Ƶϵ��
 *
 * @return  SD_Error -  �������
 */
void SDIO_Clock_Set(u8 clkdiv)
{
    u32 tmpreg = SDIO->CLKCR; /* ��ȡ��ǰCLKCR�Ĵ��� */

    tmpreg &= 0XFFFFFF00;     /* �����8λ��ʱ�ӷ�Ƶλ�� */
    tmpreg |= clkdiv;         /* �����µķ�Ƶϵ�� */
    SDIO->CLKCR = tmpreg;     /* д�ؼĴ��� */
}

/*********************************************************************
 * @fn      SD_PowerON
 *
 * @brief   SD���ϵ硣
 *
 * @return  SD_Error - �������
 */
SD_Error SD_PowerON(void)
{
    u8 i = 0;
    SD_Error errorstatus = SD_OK;                  /* ����״̬ */
    u32 response = 0, count = 0, validvoltage = 0; /* ��Ӧֵ�����Դ�������Ч��ѹ��־ */
    u32 SDType = SD_STD_CAPACITY;                  /* SD�����ͣ�Ĭ��Ϊ��׼���� */

    /* ����SDIO��ʼ������ */
    SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;                           /* ��ʼ��ʱ�ӷ�Ƶ */
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;                      /* ʱ�������ز��� */
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;                 /* ��ֹʱ����· */
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;           /* ��ֹʱ��ʡ�� */
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;                              /* 1λ���߿��� */
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable; /* ��ֹӲ�������� */
    SDIO_Init(&SDIO_InitStructure);                                                 /* ��ʼ��SDIO */

    SDIO_SetPowerState(SDIO_PowerState_ON);                                         /* ��SDIO��Դ */

    SDIO_ClockCmd(ENABLE);                                                          /* ʹ��SDIOʱ�� */

    /* ����CMD0��GO_IDLE_STATE��ʹSD���������״̬ */
    for (i = 0; i < 74; i++)
    {
        SDIO_CmdInitStructure.SDIO_Argument = 0x0;                  /* ����Ϊ0 */
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE; /* CMD0 */
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;     /* ����Ӧ */
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;             /* �޵ȴ� */
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;         /* ʹ������״̬�� */
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdError(); /* ���������� */
        if (errorstatus == SD_OK)
        {
            break; /* �ɹ�������ѭ�� */
        }
    }
    if (errorstatus)
    {
        return errorstatus; /* ���ʧ���򷵻ش��� */
    }

    /* ����CMD8��SEND_IF_COND�����SD���汾��֧��CMD8����SD��V2.0 */
    SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;    /* ���ģʽ���� */
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;   /* CMD8 */
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; /* ����Ӧ��R7�� */
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;            /* �޵ȴ� */
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;        /* ʹ������״̬�� */
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp7Error(); /* ���R7��Ӧ���� */
    if (errorstatus == SD_OK)
    {
        CardType = SDIO_STD_CAPACITY_SD_CARD_V2_0; /* ��⵽SD��V2.0 */
        SDType = SD_HIGH_CAPACITY;                 /* ��������Ϊ������ */
    }
    /* ����CMD55��APP_CMD����֪SD����һ����Ӧ���ض����� */
    SDIO_CmdInitStructure.SDIO_Argument = 0x00;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

    if (errorstatus == SD_OK) /* ���CMD55�ɹ�������SD�� */
    {
        /* ����ACMD41��SD_APP_OP_COND����ʼ��SD�������ȴ������� */
        while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
        {
            /* �ȷ���CMD55 */
            SDIO_CmdInitStructure.SDIO_Argument = 0x00;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp1Error(SD_CMD_APP_CMD);
            if (errorstatus != SD_OK)
            {
                return errorstatus;
            }

            /* ����ACMD41 */
            SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType; /* ��ѹ���ںͿ����� */
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;         /* ACMD41 */
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;           /* ����Ӧ��R3�� */
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp3Error(); /* ���R3��Ӧ */

            if (errorstatus != SD_OK)
            {
                return errorstatus;
            }
            response = SDIO->RESP1;                           /* ��ȡ��Ӧ */
            validvoltage = (((response >> 31) == 1) ? 1 : 0); /* ����31λ��������λ�� */
            count++;                                          /* ���Դ�����1 */
        }
        if (count >= SD_MAX_VOLT_TRIAL)                       /* ��������Դ��� */
        {
            errorstatus = SD_INVALID_VOLTRANGE;               /* ��Ч��ѹ��Χ���� */
            return errorstatus;
        }
        if (response &= SD_HIGH_CAPACITY)          /* ����Ƿ�֧�ָ����� */
        {
            CardType = SDIO_HIGH_CAPACITY_SD_CARD; /* ����Ϊ������SD�� */
        }
    }
    else /* �������SD��������MMC�� */
    {
        /* ����CMD1��SEND_OP_COND����ʼ��MMC�� */
        while ((!validvoltage) && (count < SD_MAX_VOLT_TRIAL))
        {
            SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_MMC; /* MMC��ѹ���� */
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_OP_COND;   /* CMD1 */
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;   /* ����Ӧ��R3�� */
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp3Error();
            if (errorstatus != SD_OK)
            {
                return errorstatus;
            }
            response = SDIO->RESP1;
            validvoltage = (((response >> 31) == 1) ? 1 : 0); /* ����31λ��������λ�� */
            count++;
        }
        if (count >= SD_MAX_VOLT_TRIAL)
        {
            errorstatus = SD_INVALID_VOLTRANGE;
            return errorstatus;
        }
        CardType = SDIO_MULTIMEDIA_CARD; /* ����ΪMMC�� */
    }
    return (errorstatus);
}

/*********************************************************************
 * @fn      SD_PowerOFF
 *
 * @brief   SD���ϵ硣
 *
 * @return  SD_Error - �������
 */
SD_Error SD_PowerOFF(void)
{
    SDIO_SetPowerState(SDIO_PowerState_OFF); /* �ر�SDIO��Դ */
    return SD_OK;
}

/*********************************************************************
 * @fn      SD_InitializeCards
 *
 * @brief   ��ʼ��SD����
 *
 * @return  SD_Error - �������
 */
SD_Error SD_InitializeCards(void)
{
    SD_Error errorstatus = SD_OK;
    u16 rca = 0x01;                              /* ��Կ���ַ����ʼ��Ϊ0x01 */
    if (SDIO_GetPowerState() == 0)               /* ���SDIO��Դ״̬ */
    {
        return SD_REQUEST_NOT_APPLICABLE;        /* ��Դδ�򿪣��������� */
    }
    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType) /* �������SDIO�� */
    {
        /* ����CMD2��ALL_SEND_CID����ȡCID */
        SDIO_CmdInitStructure.SDIO_Argument = 0x0;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long; /* ����Ӧ��R2�� */
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp2Error(); /* ���R2��Ӧ */
        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
        /* ����CID�Ĵ���ֵ */
        CID_Tab[0] = SDIO->RESP1;
        CID_Tab[1] = SDIO->RESP2;
        CID_Tab[2] = SDIO->RESP3;
        CID_Tab[3] = SDIO->RESP4;
    }
    /* ����SD����������׼�����͸���������SDIO��Ͽ� */
    if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_SECURE_DIGITAL_IO_COMBO_CARD == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
        /* ����CMD3��SET_REL_ADDR��������Ե�ַ */
        SDIO_CmdInitStructure.SDIO_Argument = 0x00;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; /* ����Ӧ��R6�� */
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp6Error(SD_CMD_SET_REL_ADDR, &rca); /* ���R6��Ӧ����ȡRCA */
        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
    }
    if (SDIO_MULTIMEDIA_CARD == CardType) /* �����MMC�� */
    {
        /* ����CMD3��SET_REL_ADDR��������Ե�ַ��MMC���Ĳ���ΪRCA����16λ */
        SDIO_CmdInitStructure.SDIO_Argument = (u32)(rca << 16);
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  // r6
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp2Error(); /* MMC��ʹ��R2��Ӧ */
        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
    }
    if (SDIO_SECURE_DIGITAL_IO_CARD != CardType) /* �������SDIO������SD����MMC���� */
    {
        RCA = rca;                               /* ����RCA */
        /* ����CMD9��SEND_CSD����ȡCSD */
        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long; /* ����Ӧ��R2�� */
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp2Error();
        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
        /* ����CSD�Ĵ���ֵ */
        CSD_Tab[0] = SDIO->RESP1;
        CSD_Tab[1] = SDIO->RESP2;
        CSD_Tab[2] = SDIO->RESP3;
        CSD_Tab[3] = SDIO->RESP4;
    }
    return SD_OK;
}

/*********************************************************************
 * @fn      SD_GetCardInfo
 *
 * @brief   ��ȡSD����Ϣ��
 *
 * @return  SD_Error - �������
 */
SD_Error SD_GetCardInfo(SD_CardInfo *cardinfo)
{
    SD_Error errorstatus = SD_OK;
    u8 tmp = 0;                        /* ��ʱ������������ȡ�ֶ� */
    cardinfo->CardType = (u8)CardType; /* ������ */
    cardinfo->RCA = (u16)RCA;          /* ��Կ���ַ */
    /* ����CSD�Ĵ��� */
    tmp = (u8)((CSD_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_csd.CSDStruct = (tmp & 0xC0) >> 6;        /* CSD�ṹ */
    cardinfo->SD_csd.SysSpecVersion = (tmp & 0x3C) >> 2;   /* ϵͳ�淶�汾 */
    cardinfo->SD_csd.Reserved1 = tmp & 0x03;               /* ����λ */
    tmp = (u8)((CSD_Tab[0] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.TAAC = tmp;                           /* ���ݷ���ʱ�� */
    tmp = (u8)((CSD_Tab[0] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.NSAC = tmp;                           /* ���뼶����ʱ�� */
    tmp = (u8)(CSD_Tab[0] & 0x000000FF);
    cardinfo->SD_csd.MaxBusClkFrec = tmp;                  /* �������ʱ��Ƶ�� */
    tmp = (u8)((CSD_Tab[1] & 0xFF000000) >> 24);
    cardinfo->SD_csd.CardComdClasses = tmp << 4;           /* �������࣬��4λ */
    tmp = (u8)((CSD_Tab[1] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.CardComdClasses |= (tmp & 0xF0) >> 4; /* �������࣬��4λ */
    cardinfo->SD_csd.RdBlockLen = tmp & 0x0F;              /* ���鳤�� */
    tmp = (u8)((CSD_Tab[1] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.PartBlockRead = (tmp & 0x80) >> 7;    /* ���ֿ�� */
    cardinfo->SD_csd.WrBlockMisalign = (tmp & 0x40) >> 6;  /* д�鲻���� */
    cardinfo->SD_csd.RdBlockMisalign = (tmp & 0x20) >> 5;  /* ���鲻���� */
    cardinfo->SD_csd.DSRImpl = (tmp & 0x10) >> 4;          /* DSRʵ�� */
    cardinfo->SD_csd.Reserved2 = 0;
    /* ��׼����SD����MMC�� */
    if ((CardType == SDIO_STD_CAPACITY_SD_CARD_V1_1) || (CardType == SDIO_STD_CAPACITY_SD_CARD_V2_0) || (SDIO_MULTIMEDIA_CARD == CardType))
    {
        cardinfo->SD_csd.DeviceSize = (tmp & 0x03) << 10;        /* �豸��С��λ[73:62] */
        tmp = (u8)(CSD_Tab[1] & 0x000000FF);
        cardinfo->SD_csd.DeviceSize |= (tmp) << 2;               /* �豸��С��λ[61:56] */
        tmp = (u8)((CSD_Tab[2] & 0xFF000000) >> 24);
        cardinfo->SD_csd.DeviceSize |= (tmp & 0xC0) >> 6;        /* �豸��С��λ[55:54] */
        cardinfo->SD_csd.MaxRdCurrentVDDMin = (tmp & 0x38) >> 3; /* ������������СVDD */
        cardinfo->SD_csd.MaxRdCurrentVDDMax = (tmp & 0x07);      /* �������������VDD */
        tmp = (u8)((CSD_Tab[2] & 0x00FF0000) >> 16);
        cardinfo->SD_csd.MaxWrCurrentVDDMin = (tmp & 0xE0) >> 5; /* ���д��������СVDD */
        cardinfo->SD_csd.MaxWrCurrentVDDMax = (tmp & 0x1C) >> 2; /* ���д���������VDD */
        cardinfo->SD_csd.DeviceSizeMul = (tmp & 0x03) << 1;      /* �豸��С������λ[49:47] */
        tmp = (u8)((CSD_Tab[2] & 0x0000FF00) >> 8);
        cardinfo->SD_csd.DeviceSizeMul |= (tmp & 0x80) >> 7;     /* �豸��С������λ[46] */
        /* ���㿨��������λ���ֽڣ� */
        cardinfo->CardCapacity = (cardinfo->SD_csd.DeviceSize + 1);
        cardinfo->CardCapacity *= (1 << (cardinfo->SD_csd.DeviceSizeMul + 2));
        cardinfo->CardBlockSize = 1 << (cardinfo->SD_csd.RdBlockLen); /* ���С */
        cardinfo->CardCapacity *= cardinfo->CardBlockSize;            /* ������ = ���� * ���С */
    }
    /* ������SD�� */
    else if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        tmp = (u8)(CSD_Tab[1] & 0x000000FF);
        cardinfo->SD_csd.DeviceSize = (tmp & 0x3F) << 16; /* �豸��С��λ[69:48] */
        tmp = (u8)((CSD_Tab[2] & 0xFF000000) >> 24);
        cardinfo->SD_csd.DeviceSize |= (tmp << 8);
        tmp = (u8)((CSD_Tab[2] & 0x00FF0000) >> 16);
        cardinfo->SD_csd.DeviceSize |= (tmp);
        tmp = (u8)((CSD_Tab[2] & 0x0000FF00) >> 8);
        /* ������SD���������㹫ʽ: ���� = (C_SIZE+1) * 512K�ֽ� */
        cardinfo->CardCapacity = (long long)(cardinfo->SD_csd.DeviceSize + 1) * 512 * 1024;
        cardinfo->CardBlockSize = 512; /* ������SD���̶����СΪ512�ֽ� */
    }
    /* ��������CSD�Ĵ��� */
    cardinfo->SD_csd.EraseGrSize = (tmp & 0x40) >> 6;         /* �������С */
    cardinfo->SD_csd.EraseGrMul = (tmp & 0x3F) << 1;          /* ��������� */
    tmp = (u8)(CSD_Tab[2] & 0x000000FF);
    cardinfo->SD_csd.EraseGrMul |= (tmp & 0x80) >> 7;         /* �����������λ[45] */
    cardinfo->SD_csd.WrProtectGrSize = (tmp & 0x7F);          /* д�������С */
    tmp = (u8)((CSD_Tab[3] & 0xFF000000) >> 24);
    cardinfo->SD_csd.WrProtectGrEnable = (tmp & 0x80) >> 7;   /* д������ʹ�� */
    cardinfo->SD_csd.ManDeflECC = (tmp & 0x60) >> 5;          /* ������Ĭ��ECC */
    cardinfo->SD_csd.WrSpeedFact = (tmp & 0x1C) >> 2;         /* д�ٶ����� */
    cardinfo->SD_csd.MaxWrBlockLen = (tmp & 0x03) << 2;       /* ���д�鳤�� */
    tmp = (u8)((CSD_Tab[3] & 0x00FF0000) >> 16);
    cardinfo->SD_csd.MaxWrBlockLen |= (tmp & 0xC0) >> 6;      /* ���д�鳤�ȣ�λ[13:12] */
    cardinfo->SD_csd.WriteBlockPaPartial = (tmp & 0x20) >> 5; /* д�鲿������ */
    cardinfo->SD_csd.Reserved3 = 0;
    cardinfo->SD_csd.ContentProtectAppli = (tmp & 0x01);      /* ���ݱ���Ӧ�� */
    tmp = (u8)((CSD_Tab[3] & 0x0000FF00) >> 8);
    cardinfo->SD_csd.FileFormatGrouop = (tmp & 0x80) >> 7;    /* �ļ���ʽ�� */
    cardinfo->SD_csd.CopyFlag = (tmp & 0x40) >> 6;            /* ���Ʊ�־ */
    cardinfo->SD_csd.PermWrProtect = (tmp & 0x20) >> 5;       /* ����д���� */
    cardinfo->SD_csd.TempWrProtect = (tmp & 0x10) >> 4;       /* ��ʱд���� */
    cardinfo->SD_csd.FileFormat = (tmp & 0x0C) >> 2;          /* �ļ���ʽ */
    cardinfo->SD_csd.ECC = (tmp & 0x03);                      /* ECC���� */
    tmp = (u8)(CSD_Tab[3] & 0x000000FF);
    cardinfo->SD_csd.CSD_CRC = (tmp & 0xFE) >> 1;             /* CSD CRC */
    cardinfo->SD_csd.Reserved4 = 1;
    /* ����CID�Ĵ��� */
    tmp = (u8)((CID_Tab[0] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ManufacturerID = tmp;   /* ������ID */
    tmp = (u8)((CID_Tab[0] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.OEM_AppliID = tmp << 8; /* OEM/Ӧ��ID */
    tmp = (u8)((CID_Tab[0] & 0x000000FF00) >> 8);
    cardinfo->SD_cid.OEM_AppliID |= tmp;
    tmp = (u8)(CID_Tab[0] & 0x000000FF);
    cardinfo->SD_cid.ProdName1 = tmp << 24; /* ��Ʒ���Ƶ�1���� */
    tmp = (u8)((CID_Tab[1] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdName1 |= tmp << 16;
    tmp = (u8)((CID_Tab[1] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.ProdName1 |= tmp << 8;
    tmp = (u8)((CID_Tab[1] & 0x0000FF00) >> 8);
    cardinfo->SD_cid.ProdName1 |= tmp;
    tmp = (u8)(CID_Tab[1] & 0x000000FF);
    cardinfo->SD_cid.ProdName2 = tmp;    /* ��Ʒ���Ƶ�2���� */
    tmp = (u8)((CID_Tab[2] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdRev = tmp;      /* ��Ʒ�汾 */
    tmp = (u8)((CID_Tab[2] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.ProdSN = tmp << 24; /* ��Ʒ���к� */
    tmp = (u8)((CID_Tab[2] & 0x0000FF00) >> 8);
    cardinfo->SD_cid.ProdSN |= tmp << 16;
    tmp = (u8)(CID_Tab[2] & 0x000000FF);
    cardinfo->SD_cid.ProdSN |= tmp << 8;
    tmp = (u8)((CID_Tab[3] & 0xFF000000) >> 24);
    cardinfo->SD_cid.ProdSN |= tmp;
    tmp = (u8)((CID_Tab[3] & 0x00FF0000) >> 16);
    cardinfo->SD_cid.Reserved1 |= (tmp & 0xF0) >> 4;   /* ����λ */
    cardinfo->SD_cid.ManufactDate = (tmp & 0x0F) << 8; /* �������� */
    tmp = (u8)((CID_Tab[3] & 0x0000FF00) >> 8);
    cardinfo->SD_cid.ManufactDate |= tmp;
    tmp = (u8)(CID_Tab[3] & 0x000000FF);
    cardinfo->SD_cid.CID_CRC = (tmp & 0xFE) >> 1; /* CID CRC */
    cardinfo->SD_cid.Reserved2 = 1;
    return errorstatus;
}

/*********************************************************************
 * @fn       SD_EnableWideBusOperation
 *
 * @brief   ʹ��SDIO�����߲�����
 *
 * @param  wmode: 0-1λ 1-4λ 2-8λ
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_EnableWideBusOperation(u32 wmode)
{
    SD_Error errorstatus = SD_OK;
    if (SDIO_MULTIMEDIA_CARD == CardType) /* MMC����֧�ֿ����� */
    {
        return SD_UNSUPPORTED_FEATURE;
    }
    else if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
    {
        if (wmode >= 2) /* SD��ֻ֧��1λ��4λģʽ */
        {
            return SD_UNSUPPORTED_FEATURE;
        }
        else
        {
            errorstatus = SDEnWideBus(wmode); /* ʹ�ܿ����� */
            if (SD_OK == errorstatus)
            {
                /* ����SDIO���߿��� */
                SDIO->CLKCR &= ~(3 << 11);       /* ���λ[12:11] */
                SDIO->CLKCR |= (u16)wmode << 11; /* �������߿��� */
                SDIO->CLKCR |= 0 << 14;          /* ��·ʱ�ӷ�Ƶ����λ14�� */
            }
        }
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       SD_SetDeviceMode
 *
 * @brief   ����SD������ģʽ��
 *
 * @param  Mode - ����ģʽ��SD_DMA_MODE��SD_POLLING_MODE��
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_SetDeviceMode(u32 Mode)
{
    SD_Error errorstatus = SD_OK;
    if ((Mode == SD_DMA_MODE) || (Mode == SD_POLLING_MODE)) /* ���ģʽ�Ƿ�Ϸ� */
    {
        DeviceMode = Mode;                                  /* �����豸ģʽ */
    }
    else
    {
        errorstatus = SD_INVALID_PARAMETER; /* ������Ч */
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       SD_SelectDeselect
 *
 * @brief   ѡ���ȡ��ѡ��SD����
 *
 * @param  addr - RCA��ַ������16λ��
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_SelectDeselect(u32 addr)
{
    SDIO_CmdInitStructure.SDIO_Argument = addr;                  /* ����ΪRCA����16λ */
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD; /* CMD7 */
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;   /* ����Ӧ��R1�� */
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    return CmdResp1Error(SD_CMD_SEL_DESEL_CARD); /* ���R1��Ӧ */
}

/*********************************************************************
 * @fn       SD_ReadBlock
 *
 * @brief   ��ȡһ���顣(DMAģʽ��ʹ��DMA����)
 *
 * @param   *buf - ���ݻ�����
 *                 addr - ���ַ���ֽڵ�ַ����ַ��ȡ���ڿ����ͣ�
 *                 blksize - ���С���ֽڣ�
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_ReadBlock(u8 *buf, long long addr, u16 blksize)
{
    SD_Error errorstatus = SD_OK;
    u8 power;                              /* ���С���ݴΣ���������SDIO���ݿ��С�Ĵ����� */
    u32 count = 0, *tempbuff = (u32 *)buf; /* ��ʱָ�룬����32λ���� */
    u32 timeout = SDIO_DATATIMEOUT;        /* ��ʱ������ */
    if (NULL == buf)                       /* ��黺����ָ�� */
    {
        return SD_INVALID_PARAMETER;
    }
    SDIO->DCTRL = 0x0;                          /* ��λ���ݿ��ƼĴ��� */
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) /* ������SD�� */
    {
        blksize = 512;                          /* �̶����СΪ512�ֽ� */
        addr >>= 9;                             /* ���ֽڵ�ַת��Ϊ���ַ������512�� */
    }

    /* ����SDIO����ͨ����������Ϊ��Ч���ã����������ã� */
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_1b;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard; /* ���򣺵�����д�� */
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    if (SDIO->RESP1 & SD_CARD_LOCKED) /* ��鿨�Ƿ����� */
    {
        return SD_LOCK_UNLOCK_FAILED;
    }

    /* ���ÿ鳤�� */
    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0)) /* �����С�Ƿ�Ϊ2�������ڷ�Χ�� */
    {
        power = convert_from_bytes_to_power_of_two(blksize);                    /* ������С���ݴ� */

        SDIO_CmdInitStructure.SDIO_Argument = blksize;                          /* ����Ϊ���С */
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;              /* CMD16 */
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;              /* ����Ӧ��R1�� */
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN); /* ���R1��Ӧ */

        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
    }
    else
    {
        return SD_INVALID_PARAMETER; /* ���С��Ч */
    }
    /* ����SDIO����ͨ�����ڶ����� */
    SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4; /* ���ݿ��С��λ[3:0]�� */
    SDIO_DataInitStructure.SDIO_DataLength = blksize;       /* ���ݳ��ȣ��ֽڣ� */
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO; /* ���򣺵�SDIO������ */
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /* ���Ͷ����������CMD17�� */
    SDIO_CmdInitStructure.SDIO_Argument = addr;                     /* ���ַ */
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK; /* CMD17 */
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;      /* ����Ӧ��R1�� */
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK); /* ���R1��Ӧ */
    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }
    /* ��ѯģʽ */
    if (DeviceMode == SD_POLLING_MODE)
    {
        /* �ȴ����ݴ�����ɻ������ */
        while (!(SDIO->STA & ((1 << 5) | (1 << 1) | (1 << 3) | (1 << 10) | (1 << 9)))) /* �ȴ����ݿ������CRCʧ�ܡ���ʱ������FIFO����������FIFO���� */
        {
            if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)                       /* �������FIFO������������8���֣� */
            {
                /* һ�ζ�ȡ8���֣�32�ֽڣ� */
                for (count = 0; count < 8; count++)
                {
                    *(tempbuff + count) = SDIO->FIFO;
                }
                tempbuff += 8;      /* ������ָ���ƶ�8���� */
                timeout = 0X7FFFFF; /* ���ó�ʱ������ */
            }
            else
            {
                if (timeout == 0) /* ��ʱ */
                {
                    return SD_DATA_TIMEOUT;
                }
                timeout--;
            }
        }
        /* �������־ */
        if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
            return SD_DATA_TIMEOUT;
        }
        else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
            return SD_DATA_CRC_FAIL;
        }
        else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
            return SD_RX_OVERRUN;
        }
        else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);
            return SD_START_BIT_ERR;
        }
        /* ��ȡFIFO��ʣ������� */
        while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
        {
            *tempbuff = SDIO_ReadData();
            tempbuff++;
        }
        SDIO_ClearFlag(SDIO_STATIC_FLAGS); /* ������о�̬��־ */
    }
    /* DMAģʽ */
    else if (DeviceMode == SD_DMA_MODE)
    {
        SD_DMA_Config((u32 *)buf, blksize, DMA_DIR_PeripheralSRC); /* ����DMA��������ΪԴ�� */
        TransferError = SD_OK;                                     /* ���ô�������־ */
        StopCondition = 0;                                         /* ���������ֹͣ���� */
        TransferEnd = 0;                                           /* ���ô��������־ */
        /* ʹ��SDIO�жϣ�����CRCʧ�ܡ����ݳ�ʱ�����ݿ����������FIFO���硢��ʼλ���� */
        SDIO->MASK |= (1 << 1) | (1 << 3) | (1 << 8) | (1 << 5) | (1 << 9);
        SDIO_DMACmd(ENABLE); /* ʹ��SDIO DMA���� */
        /* �ȴ�DMA������ɻ�ʱ */
        while (((DMA2->INTFR & 0X2000) == RESET) && (TransferEnd == 0) && (TransferError == SD_OK) && timeout)
        {
            timeout--;
        }
        if (timeout == 0)
        {
            return SD_DATA_TIMEOUT;
        }
        if (TransferError != SD_OK) /* ����д������ */
        {
            errorstatus = TransferError;
        }
    }
    return errorstatus;
}

__attribute__((aligned(4))) u32 *tempbuff; /* ��ʱ������ָ�루4�ֽڶ��룩 */

/*********************************************************************
 * @fn       SD_ReadMultiBlocks
 *
 * @brief   ��ȡ����顣(DMAģʽ��ʹ��DMA����)
 *
 * @param   *buf - ���ݻ�����
 *                 addr - ��ʼ���ַ
 *                 blksize - ���С���ֽڣ�
 *                 nblks - ������
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_ReadMultiBlocks(u8 *buf, long long addr, u16 blksize, u32 nblks)
{
    SD_Error errorstatus = SD_OK;
    u8 power;
    u32 count = 0;
    u32 timeout = SDIO_DATATIMEOUT;
    tempbuff = (u32 *)buf; /* ��ʼ����ʱ������ָ�� */

    SDIO->DCTRL = 0x0;
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) /* ������SD�� */
    {
        blksize = 512;
        addr >>= 9;
    }

    /* ��ʼ�������ã���Ч�� */
    SDIO_DataInitStructure.SDIO_DataBlockSize = 0;
    ;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    if (SDIO->RESP1 & SD_CARD_LOCKED)
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0))
    {
        power = convert_from_bytes_to_power_of_two(blksize);

        /* ���ÿ鳤�ȣ�CMD16�� */
        SDIO_CmdInitStructure.SDIO_Argument = blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);
        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
    }
    else
    {
        return SD_INVALID_PARAMETER;
    }
    if (nblks > 1)                                /* ���� */
    {
        if (nblks * blksize > SD_MAX_DATA_LENGTH) /* ��������ݳ����Ƿ񳬳����� */
        {
            return SD_INVALID_PARAMETER;
        }
        /* ��������ͨ�����ڶ��� */
        SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4;
        ;
        SDIO_DataInitStructure.SDIO_DataLength = nblks * blksize;
        SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
        SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO; /* ���򣺵�SDIO������ */
        SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
        SDIO_DataConfig(&SDIO_DataInitStructure);

        /* ���Ͷ���������CMD18�� */
        SDIO_CmdInitStructure.SDIO_Argument = addr;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK; /* CMD18 */
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_READ_MULT_BLOCK);
        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
        /* ��ѯģʽ */
        if (DeviceMode == SD_POLLING_MODE)
        {
            while (!(SDIO->STA & ((1 << 5) | (1 << 1) | (1 << 3) | (1 << 8) | (1 << 9)))) /* �ȴ����ݿ������CRCʧ�ܡ���ʱ�����ݽ�������ʼλ���� */
            {
                if (SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)
                {
                    for (count = 0; count < 8; count++)
                    {
                        *(tempbuff + count) = SDIO->FIFO;
                    }
                    tempbuff += 8;
                    timeout = 0X7FFFFF;
                }
                else
                {
                    if (timeout == 0)
                    {
                        return SD_DATA_TIMEOUT;
                    }
                    timeout--;
                }
            }
            /* ������ */
            if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
                return SD_DATA_TIMEOUT;
            }
            else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
                return SD_DATA_CRC_FAIL;
            }
            else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
                return SD_RX_OVERRUN;
            }
            else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_STBITERR);
                return SD_START_BIT_ERR;
            }

            /* ��ȡFIFO��ʣ������� */
            while (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)
            {
                *tempbuff = SDIO_ReadData();
                tempbuff++;
            }
            /* ������ݽ�����������ɣ�������ֹͣ�������CMD12�� */
            if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
            {
                if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
                {
                    SDIO_CmdInitStructure.SDIO_Argument = 0;
                    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION; /* CMD12 */
                    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                    SDIO_SendCommand(&SDIO_CmdInitStructure);

                    errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
                    if (errorstatus != SD_OK)
                    {
                        return errorstatus;
                    }
                }
            }
            SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        }
        /* DMAģʽ */
        else if (DeviceMode == SD_DMA_MODE)
        {
            SD_DMA_Config((u32 *)buf, nblks * blksize, DMA_DIR_PeripheralSRC); /* ����DMA */
            TransferError = SD_OK;
            StopCondition = 1;                                                 /* ������Ҫֹͣ���� */
            TransferEnd = 0;
            /* ʹ��SDIO�ж� */
            SDIO->MASK |= (1 << 1) | (1 << 3) | (1 << 8) | (1 << 5) | (1 << 9);
            SDIO->DCTRL |= 1 << 3; /* ʹ��DPSM������״̬���� */
            /* �ȴ�DMA������� */
            while (((DMA2->INTFR & 0X2000) == RESET) && timeout)
            {
                timeout--;
            }
            if (timeout == 0)
            {
                return SD_DATA_TIMEOUT;
            }
            /* �ȴ�������������жϴ����� */
            while ((TransferEnd == 0) && (TransferError == SD_OK));
            if (TransferError != SD_OK)
            {
                errorstatus = TransferError;
            }
        }
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       SD_WriteBlock
 *
 * @brief   д��һ���顣(DMAģʽ��ʹ��DMA����)
 *
 * @param   *buf - ���ݻ�����
 *                 addr - ���ַ
 *                 blksize - ���С���ֽڣ�
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_WriteBlock(u8 *buf, long long addr, u16 blksize)
{
    SD_Error errorstatus = SD_OK;
    u8 power = 0, cardstate = 0;                  /* ���С�ݴΣ���״̬ */
    u32 timeout = 0, bytestransferred = 0;        /* ��ʱ���������Ѵ����ֽ��� */
    u32 cardstatus = 0, count = 0, restwords = 0; /* ��״̬��ѭ����������ʣ������ */
    u32 tlen = blksize;                           /* �ܳ��� */
    u32 *tempbuff = (u32 *)buf;                   /* ��ʱ������ָ�� */

    if (buf == NULL)                              /* ��黺����ָ�� */
    {
        return SD_INVALID_PARAMETER;
    }

    SDIO->DCTRL = 0x0; /* ��λ���ݿ��ƼĴ��� */

    /* ��ʼ�������ã���Ч�� */
    SDIO_DataInitStructure.SDIO_DataBlockSize = 0;
    ;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard; /* ���򣺵�����д�� */
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);


    if (SDIO->RESP1 & SD_CARD_LOCKED) /* ��鿨�Ƿ����� */
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD) /* ������SD�� */
    {
        blksize = 512;
        addr >>= 9;
    }
    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0))
    {
        power = convert_from_bytes_to_power_of_two(blksize);

        /* ���ÿ鳤�ȣ�CMD16�� */
        SDIO_CmdInitStructure.SDIO_Argument = blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
    }
    else
    {
        return SD_INVALID_PARAMETER;
    }

    /* ����CMD13��SEND_STATUS����ȡ��״̬����鿨�Ƿ�׼���� */
    SDIO_CmdInitStructure.SDIO_Argument = (u32)RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);

    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }
    cardstatus = SDIO->RESP1; /* ��ȡ��״̬ */
    timeout = SD_DATATIMEOUT;
    /* �ȴ���׼���ã�״̬λ[8]Ϊ0��ʾæ�� */
    while (((cardstatus & 0x00000100) == 0) && (timeout > 0))
    {
        timeout--;

        /* �ٴη���CMD13��ȡ״̬ */
        SDIO_CmdInitStructure.SDIO_Argument = (u32)RCA << 16;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);
        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
        cardstatus = SDIO->RESP1;
    }
    if (timeout == 0) /* ��ʱ */
    {
        return SD_ERROR;
    }

    /* ����д���������CMD24�� */
    SDIO_CmdInitStructure.SDIO_Argument = addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK; /* CMD24 */
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);
    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }
    StopCondition = 0; /* ����д����ֹͣ���� */

    /* ��������ͨ������д���� */
    SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4;
    ;
    SDIO_DataInitStructure.SDIO_DataLength = blksize;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard; /* ���򣺵�����д�� */
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    timeout = SDIO_DATATIMEOUT;
    /* ��ѯģʽ */
    if (DeviceMode == SD_POLLING_MODE)
    {
        /* �ȴ����ݴ�����ɻ������ */
        while (!(SDIO->STA & ((1 << 10) | (1 << 4) | (1 << 1) | (1 << 3) | (1 << 9)))) /* �ȴ�����FIFO�ա����ݿ������CRCʧ�ܡ���ʱ����ʼλ���� */
        {
            if (SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)                       /* �������FIFO��գ����ٿ���д��8���֣� */
            {
                /* ���ʣ���������ڰ�FIFO��С��32�ֽڣ� */
                if ((tlen - bytestransferred) < SD_HALFFIFOBYTES)
                {
                    /* ����ʣ������������ȡ���� */
                    restwords = ((tlen - bytestransferred) % 4 == 0) ? ((tlen - bytestransferred) / 4) : ((tlen - bytestransferred) / 4 + 1);

                    /* д��ʣ����� */
                    for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
                    {
                        SDIO_WriteData(*tempbuff);
                    }
                }
                else /* ʣ�����ݴ��ڵ��ڰ�FIFO��С */
                {
                    /* һ��д��8���֣�32�ֽڣ� */
                    for (count = 0; count < 8; count++)
                    {
                        SDIO_WriteData(*(tempbuff + count));
                    }
                    tempbuff += 8;
                    bytestransferred += 32;
                }
                timeout = 0X3FFFFFFF; /* ���ó�ʱ������ */
            }
            else
            {
                if (timeout == 0)
                {
                    return SD_DATA_TIMEOUT;
                }
                timeout--;
            }
        }
        /* �������־ */
        if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
            return SD_DATA_TIMEOUT;
        }
        else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
            return SD_DATA_CRC_FAIL;
        }
        else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
            return SD_TX_UNDERRUN;
        }
        else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
        {
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);
            return SD_START_BIT_ERR;
        }

        SDIO->ICR = 0X5FF; /* ��������жϱ�־ */
    }
    /* DMAģʽ */
    else if (DeviceMode == SD_DMA_MODE)
    {
        SD_DMA_Config((u32 *)buf, blksize, DMA_DIR_PeripheralDST); /* ����DMA��������ΪĿ�꣩ */
        TransferError = SD_OK;
        StopCondition = 0;
        TransferEnd = 0;
        /* ʹ��SDIO�жϣ�CRCʧ�ܡ���ʱ�����ݿ����������FIFO�ա���ʼλ���� */
        SDIO->MASK |= (1 << 1) | (1 << 3) | (1 << 8) | (1 << 4) | (1 << 9);
        SDIO->DCTRL |= 1 << 3; /* ʹ��DPSM */
        /* �ȴ�DMA������� */
        while (((DMA2->INTFR & 0X2000) == RESET) && timeout)
        {
            timeout--;
        }
        if (timeout == 0)
        {
            SD_Init(); /* ��ʱ�����³�ʼ��SD�� */
            return SD_DATA_TIMEOUT;
        }
        timeout = SDIO_DATATIMEOUT;
        /* �ȴ�������������жϴ����� */
        while ((TransferEnd == 0) && (TransferError == SD_OK) && timeout)
        {
            timeout--;
        }
        if (timeout == 0)
        {
            return SD_DATA_TIMEOUT;
        }
        if (TransferError != SD_OK)
        {
            return TransferError;
        }
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS); /* ������о�̬��־ */
    /* ��鿨�Ƿ����ڱ�̣�д�룩 */
    errorstatus = IsCardProgramming(&cardstate);
    while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING))) /* �ȴ������� */
    {
        errorstatus = IsCardProgramming(&cardstate);
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       SD_WriteMultiBlocks
 *
 * @brief   д�����顣(DMAģʽ��ʹ��DMA����)
 *
 * @param   *buf - ���ݻ�����
 *                 addr - ��ʼ���ַ
 *                 blksize - ���С���ֽڣ�
 *                 nblks - ������
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_WriteMultiBlocks(u8 *buf, long long addr, u16 blksize, u32 nblks)
{
    SD_Error errorstatus = SD_OK;
    u8 power = 0, cardstate = 0;
    u32 timeout = 0, bytestransferred = 0;
    u32 count = 0, restwords = 0;
    u32 tlen = nblks * blksize; /* �ܳ��� */
    u32 *tempbuff = (u32 *)buf;
    if (buf == NULL)
    {
        return SD_INVALID_PARAMETER;
    }
    SDIO->DCTRL = 0x0;

    /* ��ʼ�������ã���Ч�� */
    SDIO_DataInitStructure.SDIO_DataBlockSize = 0;
    SDIO_DataInitStructure.SDIO_DataLength = 0;
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    if (SDIO->RESP1 & SD_CARD_LOCKED)
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    if (CardType == SDIO_HIGH_CAPACITY_SD_CARD)
    {
        blksize = 512;
        addr >>= 9;
    }
    if ((blksize > 0) && (blksize <= 2048) && ((blksize & (blksize - 1)) == 0))
    {
        power = convert_from_bytes_to_power_of_two(blksize);

        /* ���ÿ鳤�ȣ�CMD16�� */
        SDIO_CmdInitStructure.SDIO_Argument = blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }
    }
    else
    {
        return SD_INVALID_PARAMETER;
    }
    if (nblks > 1) /* ���д */
    {
        if (nblks * blksize > SD_MAX_DATA_LENGTH)
        {
            return SD_INVALID_PARAMETER;
        }
        /* ����SD������MMC������Ҫ�ȷ���CMD55��APP_CMD����ACMD23��SET_BLOCK_COUNT�����ÿ��� */
        if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
        {
            /* ����CMD55 */
            SDIO_CmdInitStructure.SDIO_Argument = (u32)RCA << 16;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

            if (errorstatus != SD_OK)
            {
                return errorstatus;
            }

            /* ����ACMD23 */
            SDIO_CmdInitStructure.SDIO_Argument = nblks;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT; /* ACMD23 */
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus = CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);

            if (errorstatus != SD_OK)
            {
                return errorstatus;
            }
        }

        /* ����д��������CMD25�� */
        SDIO_CmdInitStructure.SDIO_Argument = addr;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK; /* CMD25 */
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);

        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }

        /* ��������ͨ�����ڶ��д */
        SDIO_DataInitStructure.SDIO_DataBlockSize = power << 4;
        ;
        SDIO_DataInitStructure.SDIO_DataLength = nblks * blksize;
        SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
        SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToCard;
        SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
        SDIO_DataConfig(&SDIO_DataInitStructure);

        /* ��ѯģʽ */
        if (DeviceMode == SD_POLLING_MODE)
        {
            timeout = SDIO_DATATIMEOUT;

            /* �ȴ����ݴ�����ɻ������ */
            while (!(SDIO->STA & ((1 << 4) | (1 << 1) | (1 << 8) | (1 << 3) | (1 << 9)))) /* �ȴ����ݿ������CRCʧ�ܡ����ݽ�������ʱ����ʼλ���� */
            {
                if (SDIO->STA & (1 << 14))                                                /* ����FIFO��ձ�־��λ14�� */
                {
                    /* ���ʣ���������ڰ�FIFO��С��32�ֽڣ� */
                    if ((tlen - bytestransferred) < SD_HALFFIFOBYTES)
                    {
                        restwords = ((tlen - bytestransferred) % 4 == 0) ? ((tlen - bytestransferred) / 4) : ((tlen - bytestransferred) / 4 + 1);
                        for (count = 0; count < restwords; count++, tempbuff++, bytestransferred += 4)
                        {
                            SDIO_WriteData(*tempbuff);
                        }
                    }
                    else
                    {
                        for (count = 0; count < SD_HALFFIFO; count++)
                        {
                            SDIO_WriteData(*(tempbuff + count));
                        }
                        tempbuff += SD_HALFFIFO;
                        bytestransferred += SD_HALFFIFOBYTES;
                    }
                    timeout = 0X3FFFFFFF;
                }
                else
                {
                    if (timeout == 0)
                    {
                        return SD_DATA_TIMEOUT;
                    }
                    timeout--;
                }
            }
            /* �������־ */
            if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
                return SD_DATA_TIMEOUT;
            }
            else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
                return SD_DATA_CRC_FAIL;
            }
            else if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
                return SD_TX_UNDERRUN;
            }
            else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
            {
                SDIO_ClearFlag(SDIO_FLAG_STBITERR);
                return SD_START_BIT_ERR;
            }
            /* ������ݽ��������д��ɣ�������ֹͣ�������CMD12�� */
            if (SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)
            {
                if ((SDIO_STD_CAPACITY_SD_CARD_V1_1 == CardType) || (SDIO_STD_CAPACITY_SD_CARD_V2_0 == CardType) || (SDIO_HIGH_CAPACITY_SD_CARD == CardType))
                {
                    SDIO_CmdInitStructure.SDIO_Argument = 0;
                    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION; /* CMD12 */
                    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                    SDIO_SendCommand(&SDIO_CmdInitStructure);

                    errorstatus = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
                    if (errorstatus != SD_OK)
                    {
                        return errorstatus;
                    }
                }
            }
            SDIO_ClearFlag(SDIO_STATIC_FLAGS);
        }
        /* DMAģʽ */
        else if (DeviceMode == SD_DMA_MODE)
        {
            SD_DMA_Config((u32 *)buf, nblks * blksize, DMA_DIR_PeripheralDST);
            TransferError = SD_OK;
            StopCondition = 1; /* ���д��Ҫֹͣ���� */
            TransferEnd = 0;
            SDIO->MASK |= (1 << 1) | (1 << 3) | (1 << 8) | (1 << 4) | (1 << 9);
            SDIO->DCTRL |= 1 << 3; /* ʹ��DPSM */
            timeout = SDIO_DATATIMEOUT;
            /* �ȴ�DMA������� */
            while (((DMA2->INTFR & 0X2000) == RESET) && timeout)
            {
                timeout--;
            }
            if (timeout == 0)
            {
                SD_Init(); /* ��ʱ�����³�ʼ��SD�� */
                return SD_DATA_TIMEOUT;
            }
            timeout = SDIO_DATATIMEOUT;
            /* �ȴ�������������жϴ����� */
            while ((TransferEnd == 0) && (TransferError == SD_OK) && timeout)
            {
                timeout--;
            }
            if (timeout == 0)
            {
                return SD_DATA_TIMEOUT;
            }
            if (TransferError != SD_OK)
            {
                return TransferError;
            }
        }
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    /* ��鿨�Ƿ����ڱ�̣�д�룩 */
    errorstatus = IsCardProgramming(&cardstate);
    while ((errorstatus == SD_OK) && ((cardstate == SD_CARD_PROGRAMMING) || (cardstate == SD_CARD_RECEIVING)))
    {
        errorstatus = IsCardProgramming(&cardstate);
    }
    return errorstatus;
}

void SDIO_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      SDIO_IRQHandler
 *
 * @brief   �˺�������SDIO�쳣��
 *
 * @return  ��
 */
void SDIO_IRQHandler(void)
{
    SD_ProcessIRQSrc(); /* ����SDIO�ж�Դ�������� */
}

/*********************************************************************
 * @fn      SD_ProcessIRQSrc
 *
 * @brief   IRQ����������
 *
 * @return   SD_Error -  �������
 */
SD_Error SD_ProcessIRQSrc(void)
{
    /* ���ݿ�����жϣ���鴫����ɣ� */
    if (SDIO->STA & (1 << 8))
    {
        if (StopCondition == 1) /* �����Ҫֹͣ��������鴫�䣩 */
        {
            /* ����ֹͣ�������CMD12�� */
            SDIO_CmdInitStructure.SDIO_Argument = 0;
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            TransferError = CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
        }
        else /* ���鴫�� */
        {
            TransferError = SD_OK;
        }
        SDIO->ICR |= 1 << 8; /* ������ݿ�����жϱ�־ */
        /* ������������ж� */
        SDIO->MASK &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9));
        TransferEnd = 1; /* ���ô��������־ */
        return (TransferError);
    }
    /* ����CRCʧ���ж� */
    if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        SDIO->MASK &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9));
        TransferError = SD_DATA_CRC_FAIL;
        return (SD_DATA_CRC_FAIL);
    }
    /* ���ݳ�ʱ�ж� */
    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        SDIO->MASK &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9));
        TransferError = SD_DATA_TIMEOUT;
        return (SD_DATA_TIMEOUT);
    }
    /* ����FIFO�����ж� */
    if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        SDIO->MASK &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9));
        TransferError = SD_RX_OVERRUN;
        return (SD_RX_OVERRUN);
    }
    /* ����FIFO�����ж� */
    if (SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);
        SDIO->MASK &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9));
        TransferError = SD_TX_UNDERRUN;
        return (SD_TX_UNDERRUN);
    }
    /* ��ʼλ�����ж� */
    if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        SDIO->MASK &= ~((1 << 1) | (1 << 3) | (1 << 8) | (1 << 14) | (1 << 15) | (1 << 4) | (1 << 5) | (1 << 9));
        TransferError = SD_START_BIT_ERR;
        return (SD_START_BIT_ERR);
    }
    return (SD_OK);
}

/*********************************************************************
 * @fn      CmdError
 *
 * @brief   ��������飨����Ӧ�����CMD0����
 *
 * @return   SD_Error -  �������
 */
SD_Error CmdError(void)
{
    SD_Error errorstatus = SD_OK;
    u32 timeout = SDIO_CMD0TIMEOUT;
    /* �ȴ��������ɣ�CMDSENT��־�� */
    while (timeout--)
    {
        if (SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)
        {
            break;
        }
    }
    if (timeout == 0)
    {
        return SD_CMD_RSP_TIMEOUT;     /* ������Ӧ��ʱ */
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS); /* ������о�̬��־ */
    return errorstatus;
}

/*********************************************************************
 * @fn      CmdResp7Error
 *
 * @brief   R7��Ӧ�����飨����CMD8����
 *
 * @return   SD_Error -  �������
 */
SD_Error CmdResp7Error(void)
{
    SD_Error errorstatus = SD_OK;
    u32 status = 0;
    u32 timeout = SDIO_CMD0TIMEOUT;
    /* �ȴ�������Ӧ��CMDREND����CRCʧ�ܣ�CCRCFAIL����ʱ��CTIMEOUT�� */
    while (timeout--)
    {
        status = SDIO->STA;
        if (status & ((1 << 0) | (1 << 2) | (1 << 6))) /* λ0: CCRCFAIL, λ2: CTIMEOUT, λ6: CMDREND */
        {
            break;
        }
    }
    if ((timeout == 0) || (status & (1 << 2))) /* ��ʱ��CTIMEOUT��־��λ */
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); /* �����ʱ��־ */
        return errorstatus;
    }
    if ((status) & (1 << 6)) /* ������Ӧ��ɣ�CMDREND�� */
    {
        errorstatus = SD_OK;
        SDIO_ClearFlag(SDIO_FLAG_CMDREND); /* ���������Ӧ��ɱ�־ */
    }
    return errorstatus;
}

/*********************************************************************
 * @fn      CmdResp1Error
 *
 * @brief   R1��Ӧ�����飨���ڴ���������
 *
 * @return   SD_Error -  �������
 */
SD_Error CmdResp1Error(u8 cmd)
{
    u32 status;
    /* �ȴ�������Ӧ��CMDREND����CRCʧ�ܣ�CCRCFAIL����ʱ��CTIMEOUT�� */
    while (1)
    {
        status = SDIO->STA;
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;
        }
    }
    /* ��鳬ʱ */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    /* ���CRCʧ�� */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_CMD_CRC_FAIL;
    }
    /* ������������Ƿ�ƥ�� */
    if (SDIO->RESPCMD != cmd)
    {
        return SD_ILLEGAL_CMD;                         /* �Ƿ����� */
    }
    SDIO->ICR = 0X5FF;                                 /* ��������жϱ�־ */
    return (SD_Error)(SDIO->RESP1 & SD_OCR_ERRORBITS); /* ������Ӧ�еĴ���λ */
}

/*********************************************************************
 * @fn      CmdResp3Error
 *
 * @brief   R3��Ӧ�����飨����ACMD41��CMD1����
 *
 * @return   SD_Error -  �������
 */
SD_Error CmdResp3Error(void)
{
    u32 status;
    /* �ȴ�������Ӧ��CMDREND����CRCʧ�ܣ�CCRCFAIL����ʱ��CTIMEOUT�� */
    while (1)
    {
        status = SDIO->STA;
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;
        }
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS); /* R3��Ӧ�����CRC���������� */
    return SD_OK;
}

/*********************************************************************
 * @fn      CmdResp2Error
 *
 * @brief   R2��Ӧ�����飨����CMD2��CMD9����
 *
 * @return   SD_Error -  �������
 */
SD_Error CmdResp2Error(void)
{
    SD_Error errorstatus = SD_OK;
    u32 status = 0;
    u32 timeout = SDIO_CMD0TIMEOUT;
    while (timeout--)
    {
        status = SDIO->STA;
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;
        }
    }
    if ((timeout == 0) || (status & (1 << 2)))
    {
        errorstatus = SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return errorstatus;
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        errorstatus = SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);
    return errorstatus;
}

/*********************************************************************
 * @fn       CmdResp6Error
 *
 * @brief   R6��Ӧ�����飨����CMD3����
 *
 * @param   cmd - ���͵���������
 *                 *prca - ָ��RCA��ָ�루���ڴ洢���ص���Ե�ַ��
 *
 * @return  SD_Error -  �������
 */
SD_Error CmdResp6Error(u8 cmd, u16 *prca)
{
    SD_Error errorstatus = SD_OK;
    u32 status = 0;
    u32 rspr1; /* ��Ӧ1�Ĵ���ֵ */
    while (1)
    {
        status = SDIO->STA;
        if (status & ((1 << 0) | (1 << 2) | (1 << 6)))
        {
            break;
        }
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_CMD_CRC_FAIL;
    }
    if (SDIO->RESPCMD != cmd)
    {
        return SD_ILLEGAL_CMD;
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    rspr1 = SDIO->RESP1; /* ��ȡ��Ӧ */
    /* ������λ��ͨ��δ֪���󡢷Ƿ����COM CRCʧ�� */
    if (SD_ALLZERO == (rspr1 & (SD_R6_GENERAL_UNKNOWN_ERROR | SD_R6_ILLEGAL_CMD | SD_R6_COM_CRC_FAILED)))
    {
        *prca = (u16)(rspr1 >> 16); /* ��ȡRCA��λ[31:16]�� */
        return errorstatus;
    }
    if (rspr1 & SD_R6_GENERAL_UNKNOWN_ERROR)
    {
        return SD_GENERAL_UNKNOWN_ERROR;
    }
    if (rspr1 & SD_R6_ILLEGAL_CMD)
    {
        return SD_ILLEGAL_CMD;
    }
    if (rspr1 & SD_R6_COM_CRC_FAILED)
    {
        return SD_COM_CRC_FAILED;
    }
    return errorstatus;
}

/*********************************************************************
 * @fn       SDEnWideBus
 *
 * @brief   ʹ�ܿ����ߡ�
 *
 * @param   enx - 0-���� 1-ʹ�ܣ�4λģʽ��
 *
 * @return  SD_Error -  �������
 */
SD_Error SDEnWideBus(u8 enx)
{
    SD_Error errorstatus = SD_OK;
    u32 scr[2] = {0, 0}; /* SCR�Ĵ��� */
    u8 arg = 0X00;
    if (enx)
    {
        arg = 0X02; /* 4λģʽ */
    }
    else
    {
        arg = 0X00;                   /* 1λģʽ */
    }
    if (SDIO->RESP1 & SD_CARD_LOCKED) /* ��鿨�Ƿ����� */
    {
        return SD_LOCK_UNLOCK_FAILED;
    }
    /* ��ȡSCR�Ĵ����Լ���Ƿ�֧�ֿ����� */
    errorstatus = FindSCR(RCA, scr);
    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }
    /* ���SCR�еĿ�����֧��λ��λ1�� */
    if ((scr[1] & SD_WIDE_BUS_SUPPORT) != SD_ALLZERO)
    {
        /* ����CMD55��APP_CMD�� */
        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

        if (errorstatus != SD_OK)
        {
            return errorstatus;
        }

        /* ����ACMD6��APP_SD_SET_BUSWIDTH���������߿��� */
        SDIO_CmdInitStructure.SDIO_Argument = arg;                        /* 1λ��4λ */
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH; /* ACMD6 */
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus = CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);
        return errorstatus;
    }
    else
    {
        return SD_REQUEST_NOT_APPLICABLE; /* ����֧�ֿ����� */
    }
}

/*********************************************************************
 * @fn       IsCardProgramming
 *
 * @brief   ���SD�����״̬���Ƿ�����д�룩��
 *
 * @param   pcardstatus - ��ǰ״̬�������
 *
 * @return  SD_Error -  �������
 */
SD_Error IsCardProgramming(u8 *pstatus)
{
    vu32 respR1 = 0, status = 0;

    /* ����CMD13��SEND_STATUS����ȡ��״̬ */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    status = SDIO->STA;

    /* �ȴ�������Ӧ */
    while (!(status & ((1 << 0) | (1 << 6) | (1 << 2)))) /* CCRCFAIL, CMDREND, CTIMEOUT */
    {
        status = SDIO->STA;
    }

    /* ������ */
    if (SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);
        return SD_CMD_CRC_FAIL;
    }
    if (SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);
        return SD_CMD_RSP_TIMEOUT;
    }
    if (SDIO->RESPCMD != SD_CMD_SEND_STATUS)
    {
        return SD_ILLEGAL_CMD;
    }

    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    respR1 = SDIO->RESP1;                        /* ��ȡ��Ӧ */

    *pstatus = (u8)((respR1 >> 9) & 0x0000000F); /* ��ȡ״̬λ[12:9] */
    return SD_OK;
}

/*********************************************************************
 * @fn       SD_SendStatus
 *
 * @brief   ��ȡSD��״̬��
 *
 * @param   pcardstatus - ��ǰ״̬�������
 *
 * @return  SD_Error -  �������
 */
SD_Error SD_SendStatus(uint32_t *pcardstatus)
{
    SD_Error errorstatus = SD_OK;
    if (pcardstatus == NULL)
    {
        errorstatus = SD_INVALID_PARAMETER;
        return errorstatus;
    }

    /* ����CMD13��SEND_STATUS�� */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SEND_STATUS);
    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }
    *pcardstatus = SDIO->RESP1; /* ��ȡ��״̬ */
    return errorstatus;
}

/*********************************************************************
 * @fn      SD_GetState
 *
 * @brief   ��ȡSD��״̬��
 *
 * @return   SD_Error -  �������
 */
SDCardState SD_GetState(void)
{
    u32 resp1 = 0;
    if (SD_SendStatus(&resp1) != SD_OK) /* ��ȡ��״̬ */
    {
        return SD_CARD_ERROR;           /* ���� */
    }
    else
    {
        /* ��ȡ״̬λ[12:9] */
        return (SDCardState)((resp1 >> 9) & 0x0F);
    }
}

/*********************************************************************
 * @fn      FindSCR
 *
 * @brief   ��ȡSCR�Ĵ�����
 *
 * @return   SD_Error -  �������
 */
SD_Error FindSCR(u16 rca, u32 *pscr)
{
    u32 index = 0;
    SD_Error errorstatus = SD_OK;
    u32 tempscr[2] = {0, 0}; /* ��ʱ�洢SCRֵ��64λ�� */

    /* ���ÿ鳤��Ϊ8�ֽڣ�SCR�Ĵ�����СΪ8�ֽڣ� */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)8;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }

    /* ����CMD55��APP_CMD�� */
    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)rca << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_APP_CMD);

    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }

    /* ��������ͨ����8�ֽڣ������� */
    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength = 8;
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b; /* 8�ֽڿ��С */
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO; /* �� */
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    /* ����ACMD51��SD_APP_SEND_SCR����ȡSCR */
    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR; /* ACMD51 */
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;    // r1
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus = CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);

    if (errorstatus != SD_OK)
    {
        return errorstatus;
    }

    /* ��ѯ��ʽ��ȡSCR���� */
    while (!(SDIO->STA & (SDIO_FLAG_RXOVERR | SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_DBCKEND | SDIO_FLAG_STBITERR)))
    {
        if (SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET) /* ����FIFO�����ݿ��� */
        {
            *(tempscr + index) = SDIO_ReadData();
            index++;
            if (index >= 2) /* ��ȡ2���֣�8�ֽڣ� */
            {
                break;
            }
        }
    }
    /* ������ */
    if (SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);
        return SD_DATA_TIMEOUT;
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);
        return SD_DATA_CRC_FAIL;
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);
        return SD_RX_OVERRUN;
    }
    else if (SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);
        return SD_START_BIT_ERR;
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);

    /* ת���ֽ�˳�򣨴��תС�ˣ� */
    *(pscr + 1) = ((tempscr[0] & SD_0TO7BITS) << 24) | ((tempscr[0] & SD_8TO15BITS) << 8) | ((tempscr[0] & SD_16TO23BITS) >> 8) | ((tempscr[0] & SD_24TO31BITS) >> 24);
    *(pscr) = ((tempscr[1] & SD_0TO7BITS) << 24) | ((tempscr[1] & SD_8TO15BITS) << 8) | ((tempscr[1] & SD_16TO23BITS) >> 8) | ((tempscr[1] & SD_24TO31BITS) >> 24);
    return errorstatus;
}

/*********************************************************************
 * @fn       convert_from_bytes_to_power_of_two
 *
 * @brief   ���ֽ���ת��Ϊ2����ָ������������SDIO���ݿ��С�Ĵ�������
 *
 * @param   NumberOfBytes - �ֽ�����������2���ݣ�
 *
 * @return  SD_Error -  �������
 */
u8 convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
    u8 count = 0;
    while (NumberOfBytes != 1)
    {
        NumberOfBytes >>= 1; /* ����һλ */
        count++;             /* �������� */
    }
    return count;            /* ����ָ�� */
}

/*********************************************************************
 * @fn      SD_DMA_Config
 *
 * @brief   ����DMA��
 *
 * @param   mbuf - �ڴ滺������ַ
 *             bufsize - ���ݴ�С���ֽڣ�
 *             DMA_DIR - DMA����������ΪԴ��Ŀ�꣩
 *
 * @return  SD_Error -  �������
 */
void SD_DMA_Config(u32 *mbuf, u32 bufsize, u32 DMA_DIR)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE); /* ʹ��DMA2ʱ�� */

    DMA_DeInit(DMA2_Channel4);                         /* ��λDMA2ͨ��4 */
    DMA_Cmd(DMA2_Channel4, DISABLE);

    /* ����DMA */
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SDIO->FIFO;            /* �����ַ��SDIO FIFO */
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)mbuf;                       /* �ڴ��ַ */
    DMA_InitStructure.DMA_DIR = DMA_DIR;                                    /* ���䷽�� */
    DMA_InitStructure.DMA_BufferSize = bufsize / 4;                         /* ������������Ϊ��λ�� */
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        /* �����ַ������ */
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 /* �ڴ��ַ���� */
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; /* �������ݿ��ȣ��� */
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;         /* �ڴ����ݿ��ȣ��� */
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           /* ����ģʽ����ѭ���� */
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                     /* �����ȼ� */
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                            /* ��ֹ�ڴ浽�ڴ�ģʽ */
    DMA_Init(DMA2_Channel4, &DMA_InitStructure);

    DMA_Cmd(DMA2_Channel4, ENABLE); /* ʹ��DMAͨ�� */
}

/*********************************************************************
 * @fn      SD_ReadDisk
 *
 * @brief   ��ȡSD�����������𣩡�
 *
 * @param   buf - ���ݻ�����
 *            sector - ������ַ��LBA��
 *            cnt - ��������
 *
 * @return  SD_Error -  �������
 */
u8 SD_ReadDisk(u8 *buf, u32 sector, u8 cnt)
{
    u8 sta = SD_OK;
    long long lsector = sector;
    u8 n;
    lsector <<= 9; /* ������ַת��Ϊ�ֽڵ�ַ������512�� */
    /* �����������ַ����4�ֽڶ��룬��ʹ���м仺������SDIO_DATA_BUFFER�� */
    if ((u32)buf % 4 != 0)
    {
        for (n = 0; n < cnt; n++)
        {
            sta = SD_ReadBlock(SDIO_DATA_BUFFER, lsector + 512 * n, 512);
            memcpy(buf, SDIO_DATA_BUFFER, 512);
            buf += 512;
        }
    }
    else /* ��������ַ4�ֽڶ��� */
    {
        if (cnt == 1)
        {
            sta = SD_ReadBlock(buf, lsector, 512); /* �������� */
        }
        else
        {
            sta = SD_ReadMultiBlocks(buf, lsector, 512, cnt); /* �������� */
        }
    }
    return sta;
}

/*********************************************************************
 * @fn      SD_WriteDisk
 *
 * @brief   д��SD�����������𣩡�
 *
 * @param   buf - ���ݻ�����
 *            sector - ������ַ��LBA��
 *            cnt - ��������
 *
 * @return  SD_Error -  �������
 */
u8 SD_WriteDisk(u8 *buf, u32 sector, u8 cnt)
{
    u8 sta = SD_OK;
    u8 n;
    long long lsector = sector;
    lsector <<= 9; /* ������ַת��Ϊ�ֽڵ�ַ */
    /* �����������ַ����4�ֽڶ��룬��ʹ���м仺���� */
    if ((u32)buf % 4 != 0)
    {
        for (n = 0; n < cnt; n++)
        {
            memcpy(SDIO_DATA_BUFFER, buf, 512);
            sta = SD_WriteBlock(SDIO_DATA_BUFFER, lsector + 512 * n, 512);
            buf += 512;
        }
    }
    else /* ��������ַ4�ֽڶ��� */
    {
        if (cnt == 1)
        {
            sta = SD_WriteBlock(buf, lsector, 512); /* ������д */
        }
        else
        {
            sta = SD_WriteMultiBlocks(buf, lsector, 512, cnt); /* ������д */
        }
    }
    return sta;
}