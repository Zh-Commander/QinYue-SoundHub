#include "ili9341.h"
#include "sgl_core.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"

#define ILI9341_SGL_LOG                0
#if ILI9341_SGL_LOG
#define ILI9341_LOG(...) Debug_Printf(__VA_ARGS__)
#else
#define ILI9341_LOG(...) ((void)0)
#endif

/* LCD 初始化 SPI 发送超时检测宏：发送失败时释放 CS 并返回 */
#define ILI9341_CHECK(cmd) do { if((cmd) == 0) { ILI9341_CS_Set(Bit_SET); ILI9341_LOG("[SGL] LCD init SPI timeout\r\n"); return; } } while(0)

/* 调度器运行后使用 vTaskDelay 替代 Delay_Ms，避免禁用 SysTick */
#define ILI9341_DelayMs(ms) vTaskDelay(pdMS_TO_TICKS(ms))

#define LCD_SPI                     SPI1
#define LCD_DMA_CHANNEL             DMA1_Channel3
#define LCD_DMA_TC_FLAG             DMA1_FLAG_TC3
#define LCD_DMA_GL_FLAG             DMA1_FLAG_GL3
#define LCD_DMA_TC_IT               DMA1_IT_TC3
#define LCD_NOTIFY_DMA_TX_DONE      (1UL << 0)
#define LCD_NOTIFY_ALL              (LCD_NOTIFY_DMA_TX_DONE)

#define LCD_CS_PORT                 GPIOA
#define LCD_CS_PIN                  GPIO_Pin_4
#define LCD_SCK_PIN                 GPIO_Pin_5
#define LCD_MISO_PIN                GPIO_Pin_6
#define LCD_MOSI_PIN                GPIO_Pin_7

#define LCD_CTRL_PORT               GPIOD
#define LCD_DC_PIN                  GPIO_Pin_0
#define LCD_RST_PIN                 GPIO_Pin_1
#define LCD_BL_PIN                  GPIO_Pin_2
#define LCD_SPI_WAIT_TIMEOUT        1000000U
#define LCD_SPI_BAUD_PRESCALER      SPI_BaudRatePrescaler_8
#define LCD_DMA_STAGE_BUFFER_COUNT  2U
#define LCD_DMA_STAGE_MAX_BYTES     (LCD_W * 16U * sizeof(uint16_t))

static volatile uint32_t g_lcd_flush_start_count = 0;
static volatile uint32_t g_lcd_flush_done_count = 0;
static volatile uint32_t g_lcd_flush_busy_timeout_count = 0;
static volatile uint32_t g_lcd_flush_tx_wait_timeout_count = 0;
static volatile uint32_t g_lcd_flush_idle_wait_timeout_count = 0;
static volatile uint32_t g_lcd_flush_len_mismatch_count = 0;
static volatile uint32_t g_lcd_flush_src_changed_count = 0;
static volatile uint32_t g_lcd_flush_stage_wait_count = 0;
static volatile uint8_t g_lcd_dma_active = 0U;
static volatile uint8_t g_lcd_dma_stage_busy[LCD_DMA_STAGE_BUFFER_COUNT] = {0U, 0U};
static volatile uint8_t g_lcd_dma_active_stage = 0U;
static TaskHandle_t g_lcd_flush_task_handle = NULL;
static const uint8_t *g_lcd_active_flush_ptr = NULL;
static uint16_t g_lcd_active_flush_len = 0;
static uint32_t g_lcd_active_flush_sig = 0;
static uint8_t g_lcd_dma_stage_buf[LCD_DMA_STAGE_BUFFER_COUNT][LCD_DMA_STAGE_MAX_BYTES] __attribute__((aligned(4)));
static void ILI9341_ClearSpiRxResidual(void);
static void ILI9341_ClearActiveFlushState(void);
static void ILI9341_ReleaseActiveStageBuffer(void);
static void ILI9341_SwapColor16ToStageBuffer(uint8_t *dst, const void *src, uint16_t len);
static int ILI9341_WaitStageBuffer(uint8_t *stage_index);
static void ILI9341_StartDmaFlush(uint8_t stage_index, uint16_t len);
static int ILI9341_WaitLcdDmaDone(TickType_t timeout_ticks);
static int ILI9341_FinalizeDmaFlush(void);


static void ILI9341_CS_Set(BitAction state)
{
    GPIO_WriteBit(LCD_CS_PORT, LCD_CS_PIN, state);
}

static void ILI9341_DC_Set(BitAction state)
{
    GPIO_WriteBit(LCD_CTRL_PORT, LCD_DC_PIN, state);
}

static void ILI9341_RST_Set(BitAction state)
{
    GPIO_WriteBit(LCD_CTRL_PORT, LCD_RST_PIN, state);
}



static uint32_t ILI9341_CalcSampleSignature(const void *data, uint16_t len)
{
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t sig = 2166136261UL;
    uint16_t i;
    uint16_t sample_count;

    if((bytes == NULL) || (len == 0U))
    {
        return 0U;
    }

    sample_count = (len < 32U) ? len : 32U;
    for(i = 0U; i < sample_count; i++)
    {
        sig ^= bytes[i];
        sig *= 16777619UL;
    }

    if(len > sample_count)
    {
        for(i = (uint16_t)(len - sample_count); i < len; i++)
        {
            sig ^= bytes[i];
            sig *= 16777619UL;
        }
    }

    return sig;
}

static void ILI9341_ResetTransferState(void)
{
    DMA_Cmd(LCD_DMA_CHANNEL, DISABLE);
    SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, DISABLE);
    DMA_ClearFlag(LCD_DMA_GL_FLAG);
    DMA_ClearITPendingBit(LCD_DMA_TC_IT);
    ILI9341_ClearSpiRxResidual();
    ILI9341_ReleaseActiveStageBuffer();
    ILI9341_DC_Set(Bit_SET);
    ILI9341_CS_Set(Bit_SET);
}

static int ILI9341_WaitSpiTxEmpty(void)
{
    uint32_t timeout = LCD_SPI_WAIT_TIMEOUT;

    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if(timeout-- == 0U)
        {
            g_lcd_flush_tx_wait_timeout_count++;
            return 0;
        }
    }

    return 1;
}

static int ILI9341_WaitSpiIdle(void)
{
    uint32_t timeout = LCD_SPI_WAIT_TIMEOUT;

    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_BSY) == SET)
    {
        if(timeout-- == 0U)
        {
            g_lcd_flush_idle_wait_timeout_count++;
            return 0;
        }
    }

    return 1;
}


static void ILI9341_ClearActiveFlushState(void)
{
    g_lcd_active_flush_ptr = NULL;
    g_lcd_active_flush_len = 0U;
    g_lcd_active_flush_sig = 0U;
    g_lcd_flush_task_handle = NULL;
}

static void ILI9341_ReleaseActiveStageBuffer(void)
{
    if(g_lcd_dma_active != 0U)
    {
        g_lcd_dma_stage_busy[g_lcd_dma_active_stage] = 0U;
    }
    ILI9341_ClearActiveFlushState();
    g_lcd_dma_active = 0U;
}

static void ILI9341_SwapColor16ToStageBuffer(uint8_t *dst, const void *src, uint16_t len)
{
    const uint16_t *src16;
    uint16_t *dst16;
    uint16_t pixel_count;
    uint16_t i;

    if((dst == NULL) || (src == NULL) || (len < sizeof(uint16_t)))
    {
        return;
    }

    src16 = (const uint16_t *)src;
    dst16 = (uint16_t *)dst;
    pixel_count = (uint16_t)(len / sizeof(uint16_t));
    for(i = 0U; i < pixel_count; i++)
    {
        uint16_t color = src16[i];
        dst16[i] = (uint16_t)((color << 8) | (color >> 8));
    }
}

static int ILI9341_WaitStageBuffer(uint8_t *stage_index)
{
    TickType_t start_tick;

    if(stage_index == NULL)
    {
        return 0;
    }

    start_tick = xTaskGetTickCount();
    while(1)
    {
        uint8_t i;

        taskENTER_CRITICAL();
        for(i = 0U; i < LCD_DMA_STAGE_BUFFER_COUNT; i++)
        {
            if(g_lcd_dma_stage_busy[i] == 0U)
            {
                g_lcd_dma_stage_busy[i] = 1U;
                taskEXIT_CRITICAL();
                *stage_index = i;
                return 1;
            }
        }
        taskEXIT_CRITICAL();

        g_lcd_flush_stage_wait_count++;

        if(xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
        {
            vTaskDelay(1);
        }
        else if((uint32_t)(xTaskGetTickCount() - start_tick) > pdMS_TO_TICKS(20))
        {
            return 0;
        }
    }
}

static void ILI9341_StartDmaFlush(uint8_t stage_index, uint16_t len)
{
    g_lcd_dma_active_stage = stage_index;
    g_lcd_dma_active = 1U;
    DMA_Cmd(LCD_DMA_CHANNEL, DISABLE);
    DMA_ClearFlag(LCD_DMA_GL_FLAG);
    DMA_ClearITPendingBit(LCD_DMA_TC_IT);
    LCD_DMA_CHANNEL->MADDR = (uint32_t)g_lcd_dma_stage_buf[stage_index];
    DMA_SetCurrDataCounter(LCD_DMA_CHANNEL, len);
    SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, ENABLE);
    DMA_Cmd(LCD_DMA_CHANNEL, ENABLE);
}

static int ILI9341_WaitLcdDmaDone(TickType_t timeout_ticks)
{
    TickType_t start_tick;
    TickType_t remaining_ticks;
    uint32_t notify_value = 0;

    if(g_lcd_flush_task_handle == NULL)
    {
        return 0;
    }

    if(xTaskGetSchedulerState() != taskSCHEDULER_RUNNING)
    {
        return 0;
    }

    start_tick = xTaskGetTickCount();
    remaining_ticks = timeout_ticks;
    for(;;)
    {
        TickType_t elapsed_ticks;

        if(xTaskNotifyWait(0, LCD_NOTIFY_ALL, &notify_value, remaining_ticks) != pdTRUE)
        {
            return 0;
        }

        if((notify_value & LCD_NOTIFY_DMA_TX_DONE) != 0U)
        {
            return 1;
        }

        elapsed_ticks = xTaskGetTickCount() - start_tick;
        if(elapsed_ticks >= timeout_ticks)
        {
            return 0;
        }

        remaining_ticks = timeout_ticks - elapsed_ticks;
    }
}

static int ILI9341_FinalizeDmaFlush(void)
{
    int tx_empty_ok;
    int spi_idle_ok;

    DMA_Cmd(LCD_DMA_CHANNEL, DISABLE);
    DMA_ClearFlag(LCD_DMA_GL_FLAG);
    DMA_ClearITPendingBit(LCD_DMA_TC_IT);
    SPI_I2S_DMACmd(LCD_SPI, SPI_I2S_DMAReq_Tx, DISABLE);

    tx_empty_ok = ILI9341_WaitSpiTxEmpty();
    spi_idle_ok = ILI9341_WaitSpiIdle();
    ILI9341_ClearSpiRxResidual();
    if((tx_empty_ok == 0) || (spi_idle_ok == 0))
    {
        ILI9341_ResetTransferState();
        return 0;
    }

    if((g_lcd_active_flush_ptr != NULL) && (g_lcd_active_flush_len != 0U))
    {
        uint32_t done_sig = ILI9341_CalcSampleSignature(g_lcd_active_flush_ptr, g_lcd_active_flush_len);
        if(done_sig != g_lcd_active_flush_sig)
        {
            g_lcd_flush_src_changed_count++;
            ILI9341_LOG("[SGL] flush src changed ptr=0x%08lx len=%u start=0x%08lx done=0x%08lx count=%lu\r\n",
                         (unsigned long)g_lcd_active_flush_ptr,
                         (unsigned)g_lcd_active_flush_len,
                         (unsigned long)g_lcd_active_flush_sig,
                         (unsigned long)done_sig,
                         (unsigned long)g_lcd_flush_src_changed_count);
        }
    }

    ILI9341_DC_Set(Bit_SET);
    ILI9341_CS_Set(Bit_SET);
    ILI9341_ReleaseActiveStageBuffer();
    g_lcd_flush_done_count++;
    return 1;
}


static void ILI9341_ClearSpiRxResidual(void)
{
    if(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) != RESET)
    {
        (void)SPI_I2S_ReceiveData(LCD_SPI);
        (void)SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_OVR);
    }
}

static int ILI9341_WriteByte(uint8_t data)
{
    uint32_t timeout = LCD_SPI_WAIT_TIMEOUT;

    while(SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET)
    {
        if(timeout-- == 0U)
        {
            return 0;
        }
    }

    SPI_I2S_SendData(LCD_SPI, data);

    return ILI9341_WaitSpiIdle();
}

static int ILI9341_WriteCommand(uint8_t cmd)
{
    ILI9341_DC_Set(Bit_RESET);
    return ILI9341_WriteByte(cmd);
}

static int ILI9341_WriteData8(uint8_t data)
{
    ILI9341_DC_Set(Bit_SET);
    return ILI9341_WriteByte(data);
}

static int ILI9341_WriteData16(uint16_t data)
{
    ILI9341_DC_Set(Bit_SET);
    if(ILI9341_WriteByte((uint8_t)(data >> 8)) == 0)
    {
        return 0;
    }
    return ILI9341_WriteByte((uint8_t)data);
}

static int ILI9341_SetAddressWindow(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2)
{
    if(ILI9341_WriteCommand(0x2A) == 0)
    {
        return 0;
    }
    if(ILI9341_WriteData16(x1) == 0)
    {
        return 0;
    }
    if(ILI9341_WriteData16(x2) == 0)
    {
        return 0;
    }

    if(ILI9341_WriteCommand(0x2B) == 0)
    {
        return 0;
    }
    if(ILI9341_WriteData16(y1) == 0)
    {
        return 0;
    }
    if(ILI9341_WriteData16(y2) == 0)
    {
        return 0;
    }

    return ILI9341_WriteCommand(0x2C);
}

static void ILI9341_ResetPanel(void)
{
    ILI9341_RST_Set(Bit_SET);
    ILI9341_DelayMs(20);
    ILI9341_RST_Set(Bit_RESET);
    ILI9341_DelayMs(20);
    ILI9341_RST_Set(Bit_SET);
    ILI9341_DelayMs(120);
}

static void ILI9341_GPIO_Init(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_SPI1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    gpio_init.GPIO_Pin = LCD_CS_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_CS_PORT, &gpio_init);

    gpio_init.GPIO_Pin = LCD_DC_PIN | LCD_RST_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LCD_CTRL_PORT, &gpio_init);

    /* PD2 复用为 SDIO_CMD，此处不再初始化为 GPIO 输出，避免与 SDIO 冲突 */

    gpio_init.GPIO_Pin = LCD_SCK_PIN | LCD_MOSI_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);

    gpio_init.GPIO_Pin = LCD_MISO_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio_init);

    ILI9341_CS_Set(Bit_SET);
    ILI9341_DC_Set(Bit_SET);
    ILI9341_RST_Set(Bit_SET);
    /* 背光引脚 PD2 与 SDIO_CMD 复用，不再由 GPIO 控制，保持硬件默认电平 */
}

static void ILI9341_SPI_DMA_Init(void)
{
    SPI_InitTypeDef spi_init = {0};
    DMA_InitTypeDef dma_init = {0};
    NVIC_InitTypeDef nvic_init = {0};

    SPI_I2S_DeInit(LCD_SPI);
    spi_init.SPI_Direction = SPI_Direction_1Line_Tx;
    spi_init.SPI_Mode = SPI_Mode_Master;
    spi_init.SPI_DataSize = SPI_DataSize_8b;
    spi_init.SPI_CPOL = SPI_CPOL_High;
    spi_init.SPI_CPHA = SPI_CPHA_2Edge;
    spi_init.SPI_NSS = SPI_NSS_Soft;
    spi_init.SPI_BaudRatePrescaler = LCD_SPI_BAUD_PRESCALER;
    spi_init.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_init.SPI_CRCPolynomial = 7;
    SPI_Init(LCD_SPI, &spi_init);
    SPI_Cmd(LCD_SPI, ENABLE);

    DMA_DeInit(LCD_DMA_CHANNEL);
    dma_init.DMA_PeripheralBaseAddr = (uint32_t)&LCD_SPI->DATAR;
    dma_init.DMA_MemoryBaseAddr = 0;
    dma_init.DMA_DIR = DMA_DIR_PeripheralDST;
    dma_init.DMA_BufferSize = 0;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma_init.DMA_Mode = DMA_Mode_Normal;
    dma_init.DMA_Priority = DMA_Priority_High;
    dma_init.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(LCD_DMA_CHANNEL, &dma_init);
    DMA_ITConfig(LCD_DMA_CHANNEL, DMA_IT_TC, ENABLE);

    nvic_init.NVIC_IRQChannel = DMA1_Channel3_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 1;
    nvic_init.NVIC_IRQChannelSubPriority = 0;
    nvic_init.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic_init);
}

void ILI9341_Init(void)
{
    ILI9341_GPIO_Init();
    ILI9341_SPI_DMA_Init();
    ILI9341_ResetPanel();

    ILI9341_CS_Set(Bit_RESET);

    ILI9341_CHECK(ILI9341_WriteCommand(0xCF));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));
    ILI9341_CHECK(ILI9341_WriteData8(0xC1));
    ILI9341_CHECK(ILI9341_WriteData8(0x30));

    ILI9341_CHECK(ILI9341_WriteCommand(0xED));
    ILI9341_CHECK(ILI9341_WriteData8(0x64));
    ILI9341_CHECK(ILI9341_WriteData8(0x03));
    ILI9341_CHECK(ILI9341_WriteData8(0x12));
    ILI9341_CHECK(ILI9341_WriteData8(0x81));

    ILI9341_CHECK(ILI9341_WriteCommand(0xE8));
    ILI9341_CHECK(ILI9341_WriteData8(0x85));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));
    ILI9341_CHECK(ILI9341_WriteData8(0x78));

    ILI9341_CHECK(ILI9341_WriteCommand(0xCB));
    ILI9341_CHECK(ILI9341_WriteData8(0x39));
    ILI9341_CHECK(ILI9341_WriteData8(0x2C));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));
    ILI9341_CHECK(ILI9341_WriteData8(0x34));
    ILI9341_CHECK(ILI9341_WriteData8(0x02));

    ILI9341_CHECK(ILI9341_WriteCommand(0xF7));
    ILI9341_CHECK(ILI9341_WriteData8(0x20));

    ILI9341_CHECK(ILI9341_WriteCommand(0xEA));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));

    ILI9341_CHECK(ILI9341_WriteCommand(0xC0));
    ILI9341_CHECK(ILI9341_WriteData8(0x13));

    ILI9341_CHECK(ILI9341_WriteCommand(0xC1));
    ILI9341_CHECK(ILI9341_WriteData8(0x13));

    ILI9341_CHECK(ILI9341_WriteCommand(0xC5));
    ILI9341_CHECK(ILI9341_WriteData8(0x22));
    ILI9341_CHECK(ILI9341_WriteData8(0x35));

    ILI9341_CHECK(ILI9341_WriteCommand(0xC7));
    ILI9341_CHECK(ILI9341_WriteData8(0xBD));

    ILI9341_CHECK(ILI9341_WriteCommand(0x21));

    ILI9341_CHECK(ILI9341_WriteCommand(0x36));
    ILI9341_CHECK(ILI9341_WriteData8(0xA8));

    ILI9341_CHECK(ILI9341_WriteCommand(0xB6));
    ILI9341_CHECK(ILI9341_WriteData8(0x0A));
    ILI9341_CHECK(ILI9341_WriteData8(0xA2));

    ILI9341_CHECK(ILI9341_WriteCommand(0x3A));
    ILI9341_CHECK(ILI9341_WriteData8(0x55));

    ILI9341_CHECK(ILI9341_WriteCommand(0xF6));
    ILI9341_CHECK(ILI9341_WriteData8(0x01));
    ILI9341_CHECK(ILI9341_WriteData8(0x30));

    ILI9341_CHECK(ILI9341_WriteCommand(0xB1));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));
    ILI9341_CHECK(ILI9341_WriteData8(0x1B));

    ILI9341_CHECK(ILI9341_WriteCommand(0xF2));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));

    ILI9341_CHECK(ILI9341_WriteCommand(0x26));
    ILI9341_CHECK(ILI9341_WriteData8(0x01));

    ILI9341_CHECK(ILI9341_WriteCommand(0xE0));
    ILI9341_CHECK(ILI9341_WriteData8(0x0F));
    ILI9341_CHECK(ILI9341_WriteData8(0x35));
    ILI9341_CHECK(ILI9341_WriteData8(0x31));
    ILI9341_CHECK(ILI9341_WriteData8(0x0B));
    ILI9341_CHECK(ILI9341_WriteData8(0x0E));
    ILI9341_CHECK(ILI9341_WriteData8(0x06));
    ILI9341_CHECK(ILI9341_WriteData8(0x49));
    ILI9341_CHECK(ILI9341_WriteData8(0xA7));
    ILI9341_CHECK(ILI9341_WriteData8(0x33));
    ILI9341_CHECK(ILI9341_WriteData8(0x07));
    ILI9341_CHECK(ILI9341_WriteData8(0x0F));
    ILI9341_CHECK(ILI9341_WriteData8(0x03));
    ILI9341_CHECK(ILI9341_WriteData8(0x0C));
    ILI9341_CHECK(ILI9341_WriteData8(0x0A));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));

    ILI9341_CHECK(ILI9341_WriteCommand(0xE1));
    ILI9341_CHECK(ILI9341_WriteData8(0x00));
    ILI9341_CHECK(ILI9341_WriteData8(0x0A));
    ILI9341_CHECK(ILI9341_WriteData8(0x0F));
    ILI9341_CHECK(ILI9341_WriteData8(0x04));
    ILI9341_CHECK(ILI9341_WriteData8(0x11));
    ILI9341_CHECK(ILI9341_WriteData8(0x08));
    ILI9341_CHECK(ILI9341_WriteData8(0x36));
    ILI9341_CHECK(ILI9341_WriteData8(0x58));
    ILI9341_CHECK(ILI9341_WriteData8(0x4D));
    ILI9341_CHECK(ILI9341_WriteData8(0x07));
    ILI9341_CHECK(ILI9341_WriteData8(0x10));
    ILI9341_CHECK(ILI9341_WriteData8(0x0C));
    ILI9341_CHECK(ILI9341_WriteData8(0x32));
    ILI9341_CHECK(ILI9341_WriteData8(0x34));
    ILI9341_CHECK(ILI9341_WriteData8(0x0F));

    ILI9341_CHECK(ILI9341_WriteCommand(0x11));
    ILI9341_DelayMs(120);
    ILI9341_CHECK(ILI9341_WriteCommand(0x29));

    ILI9341_SetAddressWindow(0, LCD_W - 1, 0, LCD_H - 1);
    ILI9341_CS_Set(Bit_SET);
    /* 背光由硬件保持，不再操作 PD2 */
}

void ILI9341_FlushArea(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const void *data, uint16_t len)
{
    uint16_t width;
    uint16_t height;
    uint16_t expected_len;
    uint8_t stage_index;

    if((data == NULL) || (len == 0U))
    {
        sgl_fbdev_flush_ready();
        return;
    }

    width = (uint16_t)(x2 - x1 + 1U);
    height = (uint16_t)(y2 - y1 + 1U);
    expected_len = (uint16_t)(width * height * sizeof(uint16_t));
    if(len != expected_len)
    {
        g_lcd_flush_len_mismatch_count++;
        ILI9341_LOG("[SGL] flush mismatch x:%u-%u y:%u-%u w:%u h:%u len:%u exp:%u\r\n",
                     (unsigned)x1,
                     (unsigned)x2,
                     (unsigned)y1,
                     (unsigned)y2,
                     (unsigned)width,
                     (unsigned)height,
                     (unsigned)len,
                     (unsigned)expected_len);
    }

    if(len > LCD_DMA_STAGE_MAX_BYTES)
    {
        ILI9341_LOG("[SGL] flush len overflow len:%u max:%u\r\n",
                     (unsigned)len,
                     (unsigned)LCD_DMA_STAGE_MAX_BYTES);
        sgl_fbdev_flush_ready();
        return;
    }

    g_lcd_flush_start_count++;

    if(ILI9341_WaitStageBuffer(&stage_index) == 0)
    {
        g_lcd_flush_busy_timeout_count++;
        sgl_fbdev_flush_ready();
        return;
    }

    if(ILI9341_WaitSpiIdle() == 0)
    {
        g_lcd_flush_busy_timeout_count++;
        g_lcd_dma_stage_busy[stage_index] = 0U;
        ILI9341_ResetTransferState();
        sgl_fbdev_flush_ready();
        return;
    }

    ILI9341_SwapColor16ToStageBuffer(g_lcd_dma_stage_buf[stage_index], data, len);
    g_lcd_active_flush_ptr = (const uint8_t *)data;
    g_lcd_active_flush_len = len;
    g_lcd_active_flush_sig = ILI9341_CalcSampleSignature(data, len);
    g_lcd_flush_task_handle = xTaskGetCurrentTaskHandle();
    (void)xTaskNotifyStateClear(g_lcd_flush_task_handle);

    ILI9341_CS_Set(Bit_RESET);
    if(ILI9341_SetAddressWindow(x1, x2, y1, y2) == 0)
    {
        g_lcd_dma_stage_busy[stage_index] = 0U;
        ILI9341_ResetTransferState();
        sgl_fbdev_flush_ready();
        return;
    }
    ILI9341_DC_Set(Bit_SET);

    ILI9341_StartDmaFlush(stage_index, len);

    if(ILI9341_WaitLcdDmaDone(pdMS_TO_TICKS(20)) == 0)
    {
        g_lcd_flush_busy_timeout_count++;
        ILI9341_ResetTransferState();
        sgl_fbdev_flush_ready();
        return;
    }

    if(ILI9341_FinalizeDmaFlush() == 0)
    {
        sgl_fbdev_flush_ready();
        return;
    }

    sgl_fbdev_flush_ready();
}

void ILI9341_FlushCompleteISR(void)
{
    if(DMA_GetITStatus(LCD_DMA_TC_IT) != RESET)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        DMA_ClearFlag(LCD_DMA_GL_FLAG);
        DMA_ClearITPendingBit(LCD_DMA_TC_IT);

        if(g_lcd_flush_task_handle != NULL)
        {
            xTaskNotifyFromISR(g_lcd_flush_task_handle,
                               LCD_NOTIFY_DMA_TX_DONE,
                               eSetBits,
                               &xHigherPriorityTaskWoken);
        }

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

uint32_t ILI9341_GetFlushStartCount(void)
{
    return g_lcd_flush_start_count;
}

uint32_t ILI9341_GetFlushDoneCount(void)
{
    return g_lcd_flush_done_count;
}

uint32_t ILI9341_GetFlushBusyTimeoutCount(void)
{
    return g_lcd_flush_busy_timeout_count;
}

uint32_t ILI9341_GetFlushTxWaitTimeoutCount(void)
{
    return g_lcd_flush_tx_wait_timeout_count;
}

uint32_t ILI9341_GetFlushIdleWaitTimeoutCount(void)
{
    return g_lcd_flush_idle_wait_timeout_count;
}

uint32_t ILI9341_GetFlushLenMismatchCount(void)
{
    return g_lcd_flush_len_mismatch_count;
}

uint32_t ILI9341_GetFlushStageWaitCount(void)
{
    return g_lcd_flush_stage_wait_count;
}
