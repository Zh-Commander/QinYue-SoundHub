#include "ft6336_touch.h"
#include "ili9341.h"
#include "FreeRTOS.h"
#include "task.h"

/* 调度器运行后使用 vTaskDelay 替代 Delay_Ms，避免禁用 SysTick */
#define FT6336_DelayMs(ms)           vTaskDelay(pdMS_TO_TICKS(ms))

#define FT6336_I2C                   I2C1
#define FT6336_I2C_ADDR              0x70
#define FT6336_I2C_TIMEOUT           4000U
#define FT6336_I2C_YIELD_INTERVAL    64U
#define FT6336_I2C_RX_DMA_CHANNEL    DMA1_Channel7
#define FT6336_I2C_RX_DMA_TC_FLAG    DMA1_FLAG_TC7
#define FT6336_I2C_RX_DMA_GL_FLAG    DMA1_FLAG_GL7

#define FT6336_TOUCH_PORT            GPIOD
#define FT6336_INT_PIN               GPIO_Pin_3
#define FT6336_RST_PIN               GPIO_Pin_4
#define FT6336_I2C_PORT              GPIOB
#define FT6336_I2C_SCL_PIN           GPIO_Pin_6
#define FT6336_I2C_SDA_PIN           GPIO_Pin_7

#define FT_REG_NUM_FINGER            0x02
#define FT_TP1_REG                   0x03
#define FT_ID_G_CIPHER_MID           0x9F
#define FT_ID_G_CIPHER_LOW           0xA0
#define FT_ID_G_CIPHER_HIGH          0xA3
#define FT_ID_G_FOCALTECH_ID         0xA8
#define FT6336_RAW_W                 240U
#define FT6336_RAW_H                 320U

static volatile uint32_t g_ft6336_read_fail_count = 0;
static volatile uint32_t g_ft6336_recover_count = 0;
static volatile uint32_t g_ft6336_busy_timeout_count = 0;
static volatile uint32_t g_ft6336_event_timeout_count = 0;

static bool FT6336_WaitBusyFree(void)
{
    uint32_t timeout = FT6336_I2C_TIMEOUT;
    uint32_t yield_count = 0U;

    while(I2C_GetFlagStatus(FT6336_I2C, I2C_FLAG_BUSY) != RESET)
    {
        if(timeout-- == 0U)
        {
            g_ft6336_busy_timeout_count++;
            return false;
        }

        yield_count++;
        if(yield_count >= FT6336_I2C_YIELD_INTERVAL)
        {
            yield_count = 0U;
            taskYIELD();
        }
    }

    return true;
}

static bool FT6336_WaitEvent(uint32_t event)
{
    uint32_t timeout = FT6336_I2C_TIMEOUT;
    uint32_t yield_count = 0U;

    while(I2C_CheckEvent(FT6336_I2C, event) != READY)
    {
        if(timeout-- == 0U)
        {
            g_ft6336_event_timeout_count++;
            return false;
        }

        yield_count++;
        if(yield_count >= FT6336_I2C_YIELD_INTERVAL)
        {
            yield_count = 0U;
            taskYIELD();
        }
    }

    return true;
}

static bool FT6336_WaitDmaRxDone(void)
{
    uint32_t timeout = FT6336_I2C_TIMEOUT;
    uint32_t yield_count = 0U;

    while(DMA_GetFlagStatus(FT6336_I2C_RX_DMA_TC_FLAG) == RESET)
    {
        if(timeout-- == 0U)
        {
            g_ft6336_event_timeout_count++;
            return false;
        }

        yield_count++;
        if(yield_count >= FT6336_I2C_YIELD_INTERVAL)
        {
            yield_count = 0U;
            taskYIELD();
        }
    }

    return true;
}

static void FT6336_PrepareDmaRx(uint8_t *buf, uint8_t len)
{
    DMA_InitTypeDef dma_init = {0};

    DMA_Cmd(FT6336_I2C_RX_DMA_CHANNEL, DISABLE);
    DMA_DeInit(FT6336_I2C_RX_DMA_CHANNEL);

    dma_init.DMA_PeripheralBaseAddr = (uint32_t)&FT6336_I2C->DATAR;
    dma_init.DMA_MemoryBaseAddr = (uint32_t)buf;
    dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma_init.DMA_BufferSize = len;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma_init.DMA_Mode = DMA_Mode_Normal;
    dma_init.DMA_Priority = DMA_Priority_High;
    dma_init.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(FT6336_I2C_RX_DMA_CHANNEL, &dma_init);
    DMA_ClearFlag(FT6336_I2C_RX_DMA_GL_FLAG);
}

static void FT6336_I2C_Init(void)
{
    GPIO_InitTypeDef gpio_init = {0};
    I2C_InitTypeDef i2c_init = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    gpio_init.GPIO_Pin = FT6336_I2C_SCL_PIN | FT6336_I2C_SDA_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);

    gpio_init.GPIO_Pin = FT6336_RST_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FT6336_TOUCH_PORT, &gpio_init);

    gpio_init.GPIO_Pin = FT6336_INT_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(FT6336_TOUCH_PORT, &gpio_init);

    I2C_DeInit(FT6336_I2C);
    /* 触摸读取提速到 400k，降低单次 I2C 传输占用 */
    i2c_init.I2C_ClockSpeed = 400000;
    i2c_init.I2C_Mode = I2C_Mode_I2C;
    i2c_init.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_init.I2C_OwnAddress1 = 0x00;
    i2c_init.I2C_Ack = I2C_Ack_Enable;
    i2c_init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(FT6336_I2C, &i2c_init);
    I2C_Cmd(FT6336_I2C, ENABLE);
    I2C_DMACmd(FT6336_I2C, DISABLE);
    I2C_DMALastTransferCmd(FT6336_I2C, DISABLE);
}

static void FT6336_I2C_BusRecover(void)
{
    GPIO_InitTypeDef gpio_init = {0};
    uint8_t pulse;

    DMA_Cmd(FT6336_I2C_RX_DMA_CHANNEL, DISABLE);
    DMA_ClearFlag(FT6336_I2C_RX_DMA_GL_FLAG);
    I2C_DMACmd(FT6336_I2C, DISABLE);
    I2C_DMALastTransferCmd(FT6336_I2C, DISABLE);

    I2C_Cmd(FT6336_I2C, DISABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    gpio_init.GPIO_Pin = FT6336_I2C_SCL_PIN | FT6336_I2C_SDA_PIN;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(FT6336_I2C_PORT, &gpio_init);

    GPIO_SetBits(FT6336_I2C_PORT, FT6336_I2C_SCL_PIN | FT6336_I2C_SDA_PIN);
    FT6336_DelayMs(1);

    for(pulse = 0U; pulse < 9U; pulse++)
    {
        if(GPIO_ReadInputDataBit(FT6336_I2C_PORT, FT6336_I2C_SDA_PIN) == Bit_SET)
        {
            break;
        }

        GPIO_SetBits(FT6336_I2C_PORT, FT6336_I2C_SCL_PIN);
        FT6336_DelayMs(1);
        GPIO_ResetBits(FT6336_I2C_PORT, FT6336_I2C_SCL_PIN);
        FT6336_DelayMs(1);
    }

    GPIO_ResetBits(FT6336_I2C_PORT, FT6336_I2C_SDA_PIN);
    FT6336_DelayMs(1);
    GPIO_SetBits(FT6336_I2C_PORT, FT6336_I2C_SCL_PIN);
    FT6336_DelayMs(1);
    GPIO_SetBits(FT6336_I2C_PORT, FT6336_I2C_SDA_PIN);
    FT6336_DelayMs(1);
}

static void FT6336_I2C_Recover(void)
{
    g_ft6336_recover_count++;
    FT6336_I2C_BusRecover();
    I2C_GenerateSTOP(FT6336_I2C, ENABLE);
    I2C_AcknowledgeConfig(FT6336_I2C, ENABLE);
    I2C_Cmd(FT6336_I2C, DISABLE);
    I2C_SoftwareResetCmd(FT6336_I2C, ENABLE);
    I2C_SoftwareResetCmd(FT6336_I2C, DISABLE);
    FT6336_I2C_Init();
}

static void FT6336_Reset(void)
{
    GPIO_WriteBit(FT6336_TOUCH_PORT, FT6336_RST_PIN, Bit_RESET);
    FT6336_DelayMs(10);
    GPIO_WriteBit(FT6336_TOUCH_PORT, FT6336_RST_PIN, Bit_SET);
    FT6336_DelayMs(200);
}

static bool FT6336_ReadRegsCpu(uint8_t reg, uint8_t *buf, uint8_t len)
{
    if((buf == NULL) || (len == 0U))
    {
        return false;
    }

    if(!FT6336_WaitBusyFree())
    {
        g_ft6336_read_fail_count++;
        FT6336_I2C_Recover();
        return false;
    }

    I2C_GenerateSTART(FT6336_I2C, ENABLE);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    {
        goto fail;
    }

    I2C_Send7bitAddress(FT6336_I2C, FT6336_I2C_ADDR, I2C_Direction_Transmitter);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        goto fail;
    }

    I2C_SendData(FT6336_I2C, reg);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        goto fail;
    }

    I2C_GenerateSTART(FT6336_I2C, ENABLE);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    {
        goto fail;
    }

    I2C_Send7bitAddress(FT6336_I2C, FT6336_I2C_ADDR, I2C_Direction_Receiver);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        goto fail;
    }

    while(len > 0U)
    {
        uint32_t timeout = FT6336_I2C_TIMEOUT;

        if(len == 1U)
        {
            I2C_AcknowledgeConfig(FT6336_I2C, DISABLE);
            I2C_GenerateSTOP(FT6336_I2C, ENABLE);
        }

        while(I2C_CheckEvent(FT6336_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) != READY)
        {
            if(timeout-- == 0U)
            {
                g_ft6336_event_timeout_count++;
                I2C_AcknowledgeConfig(FT6336_I2C, ENABLE);
                goto fail;
            }

            if((timeout % FT6336_I2C_YIELD_INTERVAL) == 0U)
            {
                taskYIELD();
            }
        }

        *buf++ = I2C_ReceiveData(FT6336_I2C);
        len--;
    }

    I2C_AcknowledgeConfig(FT6336_I2C, ENABLE);
    return true;

fail:
    g_ft6336_read_fail_count++;
    I2C_GenerateSTOP(FT6336_I2C, ENABLE);
    I2C_AcknowledgeConfig(FT6336_I2C, ENABLE);
    FT6336_I2C_Recover();
    return false;
}

static bool FT6336_ReadTouchPacketDma(uint8_t *buf, uint8_t len)
{
    if((buf == NULL) || (len == 0U))
    {
        return false;
    }

    if(!FT6336_WaitBusyFree())
    {
        g_ft6336_read_fail_count++;
        FT6336_I2C_Recover();
        return false;
    }

    I2C_GenerateSTART(FT6336_I2C, ENABLE);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    {
        goto fail;
    }

    I2C_Send7bitAddress(FT6336_I2C, FT6336_I2C_ADDR, I2C_Direction_Transmitter);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        goto fail;
    }

    I2C_SendData(FT6336_I2C, FT_REG_NUM_FINGER);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        goto fail;
    }

    I2C_GenerateSTART(FT6336_I2C, ENABLE);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_MODE_SELECT))
    {
        goto fail;
    }

    I2C_Send7bitAddress(FT6336_I2C, FT6336_I2C_ADDR, I2C_Direction_Receiver);
    if(!FT6336_WaitEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        goto fail;
    }

    FT6336_PrepareDmaRx(buf, len);
    I2C_AcknowledgeConfig(FT6336_I2C, ENABLE);
    I2C_DMACmd(FT6336_I2C, ENABLE);
    I2C_DMALastTransferCmd(FT6336_I2C, ENABLE);
    DMA_Cmd(FT6336_I2C_RX_DMA_CHANNEL, ENABLE);

    if(!FT6336_WaitDmaRxDone())
    {
        goto fail;
    }

    I2C_GenerateSTOP(FT6336_I2C, ENABLE);
    DMA_Cmd(FT6336_I2C_RX_DMA_CHANNEL, DISABLE);
    DMA_ClearFlag(FT6336_I2C_RX_DMA_GL_FLAG);
    I2C_DMACmd(FT6336_I2C, DISABLE);
    I2C_DMALastTransferCmd(FT6336_I2C, DISABLE);
    I2C_AcknowledgeConfig(FT6336_I2C, ENABLE);
    return true;

fail:
    g_ft6336_read_fail_count++;
    DMA_Cmd(FT6336_I2C_RX_DMA_CHANNEL, DISABLE);
    DMA_ClearFlag(FT6336_I2C_RX_DMA_GL_FLAG);
    I2C_DMACmd(FT6336_I2C, DISABLE);
    I2C_DMALastTransferCmd(FT6336_I2C, DISABLE);
    I2C_GenerateSTOP(FT6336_I2C, ENABLE);
    I2C_AcknowledgeConfig(FT6336_I2C, ENABLE);
    FT6336_I2C_Recover();
    return false;
}

bool FT6336_Touch_Init(void)
{
    uint8_t id = 0;
    uint8_t cipher[2] = {0};

    FT6336_I2C_Init();
    FT6336_Reset();

    if(!FT6336_ReadRegsCpu(FT_ID_G_FOCALTECH_ID, &id, 1U))
    {
        return false;
    }

    if(id != 0x11U)
    {
        return false;
    }

    if(!FT6336_ReadRegsCpu(FT_ID_G_CIPHER_MID, cipher, 2U))
    {
        return false;
    }

    if(cipher[0] != 0x26U)
    {
        return false;
    }

    if((cipher[1] != 0x00U) && (cipher[1] != 0x01U) && (cipher[1] != 0x02U))
    {
        return false;
    }

    if(!FT6336_ReadRegsCpu(FT_ID_G_CIPHER_HIGH, &id, 1U))
    {
        return false;
    }

    return (id == 0x64U);
}

bool FT6336_Touch_HasPoint(void)
{
    uint8_t finger_count = 0;

    if(!FT6336_ReadRegsCpu(FT_REG_NUM_FINGER, &finger_count, 1U))
    {
        return false;
    }

    finger_count &= 0x0FU;
    return (finger_count != 0U) && (finger_count <= 2U);
}

bool FT6336_Touch_IntActive(void)
{
    return (GPIO_ReadInputDataBit(FT6336_TOUCH_PORT, FT6336_INT_PIN) == Bit_RESET);
}

bool FT6336_Touch_ReadState(uint16_t *x, uint16_t *y, bool *pressed)
{
    uint8_t point_buf[5] = {0};
    uint8_t finger_count;
    uint16_t raw_x = 0;
    uint16_t raw_y = 0;
    uint16_t point_x = 0;
    uint16_t point_y = 0;

    if((x == NULL) || (y == NULL) || (pressed == NULL))
    {
        return false;
    }

    if(!FT6336_ReadTouchPacketDma(point_buf, (uint8_t)sizeof(point_buf)))
    {
        return false;
    }

    finger_count = point_buf[0] & 0x0FU;
    if((finger_count == 0U) || (finger_count > 2U))
    {
        *pressed = false;
        return true;
    }

    raw_x = (uint16_t)(((uint16_t)(point_buf[1] & 0x0FU) << 8) | point_buf[2]);
    raw_y = (uint16_t)(((uint16_t)(point_buf[3] & 0x0FU) << 8) | point_buf[4]);

#if (LCD_W > LCD_H)
    point_x = (raw_y >= FT6336_RAW_H) ? 0U : (uint16_t)(FT6336_RAW_H - 1U - raw_y);
    point_y = raw_x;
#else
    point_x = raw_x;
    point_y = raw_y;
#endif

    if(point_x >= LCD_W)
    {
        point_x = LCD_W - 1U;
    }

    if(point_y >= LCD_H)
    {
        point_y = LCD_H - 1U;
    }

    *x = point_x;
    *y = point_y;
    *pressed = true;
    return true;
}

bool FT6336_Touch_ReadPoint(uint16_t *x, uint16_t *y)
{
    bool pressed = false;

    if(!FT6336_Touch_ReadState(x, y, &pressed))
    {
        return false;
    }

    return pressed;
}

uint32_t FT6336_GetReadFailCount(void)
{
    return g_ft6336_read_fail_count;
}

uint32_t FT6336_GetRecoverCount(void)
{
    return g_ft6336_recover_count;
}

uint32_t FT6336_GetBusyTimeoutCount(void)
{
    return g_ft6336_busy_timeout_count;
}

uint32_t FT6336_GetEventTimeoutCount(void)
{
    return g_ft6336_event_timeout_count;
}
