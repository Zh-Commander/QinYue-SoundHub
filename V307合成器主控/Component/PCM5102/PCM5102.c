/*
 *@Note
   I2S master transmitter slave receiver DMA routine:
      This routine demonstrates that I2S3 acts as a slave to receive,
      and I2S2 acts as a master to transmit.
 SPI2-I2S2:
     WS -- PB12
     CK -- PB13
     SD -- PB15
     MCK-- PC6

 SPI3-I2S3:
     WS -- PA15
     CK -- PB3
     SD -- PB5
     MCK-- PC7
*/

#include "debug.h"
#include "string.h"
#include "PCM5102.h"
#include "..\\SD\\FATFS\\ff.h"
#include "FreeRTOS.h"
#include "task.h"

/* Global Variable */

#define AUDIO_BUFFER_SIZE  2048
#define AUDIO_HALF_SAMPLE_COUNT (AUDIO_BUFFER_SIZE / 2)

static u16 audioBuffer[AUDIO_BUFFER_SIZE * 2];
static int16_t audioReadBuffer[AUDIO_HALF_SAMPLE_COUNT];

void PCM5102_Pack16To24(u16 *dst, const int16_t *src, u16 sampleCount)
{
    u16 i;

    for(i = 0; i < sampleCount; i++)
    {
        uint32_t packed = (uint32_t)(((int32_t)src[i]) << 16);

        dst[i * 2] = (u16)(packed >> 16);
        dst[i * 2 + 1] = (u16)(packed & 0xFFFF);
    }
}


/*********************************************************************
 * @fn      PlayAudioFromSDCard
 *
 * @brief   Read audio data from SD card file and play via I2S+DMA
 *
 * @param   filename - Path to audio file on SD card (e.g., "1:/audio.pcm")
 *
 * @return  none
 */
void PlayAudioFromSDCard(const char *filename)
{
    FIL audioFile;
    FRESULT res;
    UINT bytesRead;
    u32 dataRemaining;
    u32 chunkSize;
    u32 fileSize;

    res = f_open(&audioFile, filename, FA_READ);
    if(res != FR_OK)
    {
        Debug_Printf("Error: Cannot open file %s\r\n", filename);
        return;
    }

    fileSize = f_size(&audioFile);
    dataRemaining = fileSize / sizeof(int16_t);

    //Debug_Printf("Playing: %s, %d bytes\r\n", filename, fileSize);

    SPI_I2S_DeInit(SPI2);
    I2S2_Init();

    memset(audioBuffer, 0, sizeof(audioBuffer));
    memset(audioReadBuffer, 0, sizeof(audioReadBuffer));

    /* 不使用循环模式，首次初始化 */
    DMA_Tx_Init(DMA1_Channel5, (u32)&SPI2->DATAR, (u32)audioBuffer, AUDIO_BUFFER_SIZE);
    DMA_Cmd(DMA1_Channel5, ENABLE);

    /* 读取第一个半缓冲区 */
    chunkSize = (dataRemaining > AUDIO_HALF_SAMPLE_COUNT) ? AUDIO_HALF_SAMPLE_COUNT : dataRemaining;
    f_read(&audioFile, (u8*)audioReadBuffer, chunkSize * sizeof(int16_t), &bytesRead);
    dataRemaining -= (bytesRead / sizeof(int16_t));
    PCM5102_Pack16To24(audioBuffer, audioReadBuffer, AUDIO_HALF_SAMPLE_COUNT);

    while(dataRemaining > 0)
    {
        /* 预读取：在 DMA 传输时，提前读取下一个数据块 */
        if(dataRemaining > 0)
        {
            chunkSize = (dataRemaining > AUDIO_HALF_SAMPLE_COUNT) ? AUDIO_HALF_SAMPLE_COUNT : dataRemaining;
            memset(audioReadBuffer, 0, sizeof(audioReadBuffer));
            f_read(&audioFile, (u8*)audioReadBuffer, chunkSize * sizeof(int16_t), &bytesRead);
            dataRemaining -= (bytesRead / sizeof(int16_t));
            PCM5102_Pack16To24(audioBuffer + AUDIO_BUFFER_SIZE, audioReadBuffer, AUDIO_HALF_SAMPLE_COUNT);
        }

        /* 等待前半部分传输完成 */
        while(!DMA_GetFlagStatus(DMA1_FLAG_TC5));
        DMA_ClearFlag(DMA1_FLAG_TC5);

        /* 快速重新配置 DMA，播放后半部分（数据已经准备好） */
        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA1_Channel5->MADDR = (u32)(audioBuffer + AUDIO_BUFFER_SIZE);
        DMA_SetCurrDataCounter(DMA1_Channel5, AUDIO_BUFFER_SIZE);
        DMA_Cmd(DMA1_Channel5, ENABLE);

        /* 预读取：在 DMA 传输时，提前读取下一个数据块 */
        if(dataRemaining > 0)
        {
            chunkSize = (dataRemaining > AUDIO_HALF_SAMPLE_COUNT) ? AUDIO_HALF_SAMPLE_COUNT : dataRemaining;
            memset(audioReadBuffer, 0, sizeof(audioReadBuffer));
            f_read(&audioFile, (u8*)audioReadBuffer, chunkSize * sizeof(int16_t), &bytesRead);
            dataRemaining -= (bytesRead / sizeof(int16_t));
            PCM5102_Pack16To24(audioBuffer, audioReadBuffer, AUDIO_HALF_SAMPLE_COUNT);
        }

        /* 等待后半部分传输完成 */
        while(!DMA_GetFlagStatus(DMA1_FLAG_TC5));
        DMA_ClearFlag(DMA1_FLAG_TC5);

        /* 快速重新配置 DMA，播放前半部分（数据已经准备好） */
        if(dataRemaining > 0 || bytesRead > 0)
        {
            DMA_Cmd(DMA1_Channel5, DISABLE);
            DMA1_Channel5->MADDR = (u32)audioBuffer;
            DMA_SetCurrDataCounter(DMA1_Channel5, AUDIO_BUFFER_SIZE);
            DMA_Cmd(DMA1_Channel5, ENABLE);
        }
    }

    /* 等待最后一次传输完成 */
    while(!DMA_GetFlagStatus(DMA1_FLAG_TC5));
    DMA_ClearFlag(DMA1_FLAG_TC5);

    memset(audioBuffer, 0, sizeof(audioBuffer));

    DMA_Cmd(DMA1_Channel5, DISABLE);
    I2S_Cmd(SPI2, DISABLE);
    f_close(&audioFile);

    Debug_Printf("Done\r\n");
}


/*********************************************************************
 * @fn      I2S2_Init
 *
 * @brief   Initialize I2S2
 *
 * @return  none
 */
void I2S2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    I2S_InitTypeDef  I2S_InitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_Standard = I2S_Standard_Phillips;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_24b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI2, &I2S_InitStructure);

    // Use standard I2S clock configuration, let the library automatically calculate the prescaler value closest to 48kHz

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    I2S_Cmd(SPI2, ENABLE);
}

/*********************************************************************
 * @fn      DMA_Tx_Init
 *
 * @brief   Initializes the DMAy Channelx configuration.
 *
 * @param   DMA_CHx - x can be 1 to 7.
 *          ppadr - Peripheral base address.
 *          memadr - Memory base address.
 *          bufsize - DMA channel buffer size.
 *
 * @return  none
 */
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize)
{
    DMA_InitTypeDef DMA_InitStructure={0};

    RCC_AHBPeriphClockCmd( RCC_AHBPeriph_DMA1, ENABLE );

    DMA_DeInit(DMA_CHx);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ppadr;
    DMA_InitStructure.DMA_MemoryBaseAddr = memadr;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = bufsize;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  // 改为 Normal 模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DMA_CHx, &DMA_InitStructure );
}
