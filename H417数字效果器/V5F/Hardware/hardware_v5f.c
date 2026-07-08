#include "hardware_v5f.h"
#include "config.h"
#include "ch32h417_gpio.h"

#define I2SCFGR_I2SE_Set ((uint16_t)0x0400)

void I2S2_Init(void);
void I2S3_Init(void);
void DMA_Tx_Init(void);
void DMA_Rx_Init(void);
void DMA_NVIC_Init(void);
void CPU_GPIO_Init(void);

void Hardware_V5f(void)
{
    printf("V5F Hardware Configuring...\r\n");

    CPU_GPIO_Init();

    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE); 没用
    SPI_I2S_DeInit(SPI2);
    SPI_I2S_DeInit(SPI3);
    I2S2_Init();
    I2S3_Init();

    DMA_Tx_Init();
    DMA_Rx_Init();

    DMA_MuxChannelConfig(DMA_MuxChannel9, 68);  // DMA2_CH1 I2S3_Rx
    DMA_MuxChannelConfig(DMA_MuxChannel5, 65);  // DMA1_CH5 I2S2_Tx

    DMA_NVIC_Init();

    CPU_FENCE();
    SPI2->CTLR2 |= SPI_I2S_DMAReq_Tx;
    SPI3->CTLR2 |= SPI_I2S_DMAReq_Rx;
    CPU_FENCE();

    while (!((GPIOA->INDR & GPIO_Pin_15) != (uint32_t)Bit_RESET));
    while ((GPIOA->INDR & GPIO_Pin_15) != (uint32_t)Bit_RESET);

    CPU_FENCE();

    SPI2->I2SCFGR |= I2SCFGR_I2SE_Set;

    CPU_FENCE();

    // volatile uint8_t k = 401;

    // while (k-- != 0)
    // {
    //     __asm volatile("nop" ::: "memory");
    // }
    
    // 延迟一个周期再启用接收,避免APB总线冲突导致延时过长又对不上时钟
    // 上面改进旧版,采用轮询来延迟一个周期再收

    while (!((GPIOA->INDR & GPIO_Pin_15) != (uint32_t)Bit_RESET));
    while ((GPIOA->INDR & GPIO_Pin_15) != (uint32_t)Bit_RESET);

    CPU_FENCE();

    SPI3->I2SCFGR |= I2SCFGR_I2SE_Set;

    CPU_FENCE();
    printf("V5F Hardware Configuration complete.\r\n");
}

void I2S2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef I2S_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI2, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);

    // WS PB12(AF5)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // CK PB13(AF5)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // SD PB15(AF5)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF5);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI2, &I2S_InitStructure);
}

void I2S3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef I2S_InitStructure = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB1PeriphClockCmd(RCC_HB1Periph_SPI3, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);

    // WS PA15(AF6)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF6);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // CK PA14(AF1)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource14, GPIO_AF1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // SD PA13(AF1)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource13, GPIO_AF1);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_SlaveRx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_32b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI3, &I2S_InitStructure);
}

void DMA_Tx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel5);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)I2S_TX_DMA_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = I2S_BUFFER_DMA_CNT;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
}

void DMA_Rx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA2, ENABLE);

    DMA_DeInit(DMA2_Channel1);

    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI3->DATAR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)I2S_RX_DMA_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = I2S_BUFFER_DMA_CNT;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel1, &DMA_InitStructure);

    DMA_ITConfig(DMA2_Channel1, DMA_IT_HT | DMA_IT_TC | DMA_IT_TE, ENABLE);
    DMA_ClearITPendingBit(DMA2, DMA2_IT_GL1);
}

void DMA_NVIC_Init(void)
{
    NVIC_SetAllocateIRQ(DMA2_Channel1_IRQn, Core_ID_V5F);
    NVIC_SetPriority(DMA2_Channel1_IRQn, 0x00);
    NVIC_EnableIRQ(DMA2_Channel1_IRQn);

    DMA_ClearITPendingBit(DMA1, DMA1_IT_GL5);
    DMA_ClearITPendingBit(DMA2, DMA2_IT_GL1);

    DMA_Cmd(DMA1_Channel5, ENABLE);  // Tx
    DMA_Cmd(DMA2_Channel1, ENABLE);  // Rx
}

void CPU_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}