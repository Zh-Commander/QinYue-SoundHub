#include "adc_volume.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ch32v30x_adc.h"
#include "ch32v30x_dma.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_misc.h"
#include "ch32v30x_rcc.h"
#include <string.h>

#define ADC_VOLUME_CHANNEL_COUNT         4U
#define ADC_VOLUME_DMA_BUFFER_SIZE       ADC_VOLUME_CHANNEL_COUNT
#define ADC_VOLUME_SAMPLE_PERIOD_MS      10U
#define ADC_VOLUME_DEBUG_PRINT_PERIOD_MS 200U
#define ADC_VOLUME_DEBUG_PRINT_DIV       (ADC_VOLUME_DEBUG_PRINT_PERIOD_MS / ADC_VOLUME_SAMPLE_PERIOD_MS)
#define ADC_VOLUME_FILTER_SHIFT          3U
#define ADC_VOLUME_DEFAULT_Q8            256U
#define ADC_VOLUME_TASK_STOP_WAIT_MS     300U

typedef struct {
    uint16_t dma_buffer[ADC_VOLUME_DMA_BUFFER_SIZE];
    uint16_t filtered_raw[ADC_VOLUME_CHANNEL_COUNT];
    volatile uint16_t volume_q8;
    volatile uint8_t running;
    TaskHandle_t task_handle;
} adc_volume_runtime_t;

static adc_volume_runtime_t g_adc_volume = {
    .volume_q8 = ADC_VOLUME_DEFAULT_Q8
};
static volatile uint32_t g_adc_volume_generation = 0U;

static uint8_t adc_volume_task_is_active(uint32_t task_generation)
{
    return (uint8_t)((g_adc_volume.running != 0U) && (g_adc_volume_generation == task_generation));
}

static void adc_volume_gpio_init(void);
static void adc_volume_dma_init(void);
static void adc_volume_adc_init(void);
static void adc_volume_busy_delay_us(uint32_t us);
static uint16_t adc_volume_average_raw(void);
static uint16_t adc_volume_raw_to_q8(uint16_t raw_value);
static void adc_volume_task(void *pvParameters);

static void adc_volume_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;

    gpio_init.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_Init(GPIOB, &gpio_init);

    gpio_init.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Init(GPIOC, &gpio_init);
}

static void adc_volume_dma_init(void)
{
    DMA_InitTypeDef dma_init = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    DMA_DeInit(DMA1_Channel1);

    dma_init.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->RDATAR;
    dma_init.DMA_MemoryBaseAddr = (uint32_t)g_adc_volume.dma_buffer;
    dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma_init.DMA_BufferSize = ADC_VOLUME_DMA_BUFFER_SIZE;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_init.DMA_Mode = DMA_Mode_Circular;
    dma_init.DMA_Priority = DMA_Priority_High;
    dma_init.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &dma_init);
    DMA_Cmd(DMA1_Channel1, ENABLE);
}

static void adc_volume_busy_delay_us(uint32_t us)
{
    while (us-- > 0U) {
        for (volatile uint32_t i = 0U; i < (SystemCoreClock / 8000000U); i++) {
            __NOP();
        }
    }
}

static void adc_volume_adc_init(void)
{
    ADC_InitTypeDef adc_init = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    ADC_DeInit(ADC1);

    adc_init.ADC_Mode = ADC_Mode_Independent;
    adc_init.ADC_ScanConvMode = ENABLE;
    adc_init.ADC_ContinuousConvMode = ENABLE;
    adc_init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;
    adc_init.ADC_NbrOfChannel = ADC_VOLUME_CHANNEL_COUNT;
    adc_init.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    adc_init.ADC_Pga = ADC_Pga_1;
    ADC_Init(ADC1, &adc_init);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 2, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_239Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_239Cycles5);

    ADC_DMACmd(ADC1, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_BufferCmd(ADC1, DISABLE);

    adc_volume_busy_delay_us(10U);
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)) {
    }
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)) {
    }

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

static uint16_t adc_volume_average_raw(void)
{
    uint32_t sum = 0U;
    uint8_t i;

    for (i = 0U; i < ADC_VOLUME_CHANNEL_COUNT; i++) {
        uint16_t sample = g_adc_volume.dma_buffer[i];
        uint16_t filtered = g_adc_volume.filtered_raw[i];

        if (filtered == 0U) {
            filtered = sample;
        } else {
            filtered = (uint16_t)((((uint32_t)filtered * ((1UL << ADC_VOLUME_FILTER_SHIFT) - 1UL)) + sample) >> ADC_VOLUME_FILTER_SHIFT);
        }

        g_adc_volume.filtered_raw[i] = filtered;
        sum += filtered;
    }

    return (uint16_t)(sum / ADC_VOLUME_CHANNEL_COUNT);
}

static uint16_t adc_volume_raw_to_q8(uint16_t raw_value)
{
    uint32_t scaled;

    if (raw_value >= 4095U) {
        return 256U;
    }

    scaled = ((uint32_t)raw_value * 256U + 2047U) / 4095U;
    return (uint16_t)scaled;
}

static void adc_volume_task(void *pvParameters)
{
    uint32_t task_generation = (uint32_t)pvParameters;
    uint16_t debug_divider = 0U;

    Debug_Printf("[ADC] volume task started\r\n");

    while (adc_volume_task_is_active(task_generation) != 0U) {
        uint16_t average_raw = adc_volume_average_raw();
        uint16_t volume_q8 = adc_volume_raw_to_q8(average_raw);

        g_adc_volume.volume_q8 = volume_q8;

        debug_divider++;
        if (debug_divider >= ADC_VOLUME_DEBUG_PRINT_DIV) {
            debug_divider = 0U;
            Debug_Printf("[ADC] PB0:%u PB1:%u PC4:%u PC5:%u AVG:%u VOL_Q8:%u\r\n",
                         g_adc_volume.filtered_raw[0],
                         g_adc_volume.filtered_raw[1],
                         g_adc_volume.filtered_raw[2],
                         g_adc_volume.filtered_raw[3],
                         average_raw,
                         volume_q8);
        }

        vTaskDelay(pdMS_TO_TICKS(ADC_VOLUME_SAMPLE_PERIOD_MS));
    }

    if (g_adc_volume_generation == task_generation) {
        ADC_SoftwareStartConvCmd(ADC1, DISABLE);
        ADC_Cmd(ADC1, DISABLE);
        ADC_DMACmd(ADC1, DISABLE);
        DMA_Cmd(DMA1_Channel1, DISABLE);
        g_adc_volume.task_handle = NULL;
    }
    vTaskDelete(NULL);
}

int adc_volume_init(void)
{
    BaseType_t task_res;

    if (g_adc_volume.running != 0U) {
        return 0;
    }

    memset(g_adc_volume.dma_buffer, 0, sizeof(g_adc_volume.dma_buffer));
    memset(g_adc_volume.filtered_raw, 0, sizeof(g_adc_volume.filtered_raw));
    g_adc_volume_generation++;
    if (g_adc_volume_generation == 0U) {
        g_adc_volume_generation = 1U;
    }
    g_adc_volume.volume_q8 = ADC_VOLUME_DEFAULT_Q8;
    g_adc_volume.running = 1U;

    adc_volume_gpio_init();
    adc_volume_dma_init();
    adc_volume_adc_init();

    task_res = xTaskCreate((TaskFunction_t)adc_volume_task,
                           (const char *)"adc_volume",
                           (uint16_t)ADC_VOLUME_TASK_STACK,
                           (void *)g_adc_volume_generation,
                           (UBaseType_t)ADC_VOLUME_TASK_PRIO,
                           (TaskHandle_t *)&g_adc_volume.task_handle);
    if (task_res != pdPASS) {
        g_adc_volume.running = 0U;
        ADC_SoftwareStartConvCmd(ADC1, DISABLE);
        ADC_Cmd(ADC1, DISABLE);
        ADC_DMACmd(ADC1, DISABLE);
        DMA_Cmd(DMA1_Channel1, DISABLE);
        Debug_Printf("[ADC] create task failed\r\n");
        return -1;
    }

    Debug_Printf("[ADC] PB0/PB1/PC4/PC5 volume control initialized\r\n");
    return 0;
}

void adc_volume_deinit(void)
{
    uint32_t stop_generation;

    if (g_adc_volume.running == 0U) {
        return;
    }

    stop_generation = g_adc_volume_generation;
    g_adc_volume.running = 0U;

    for (int wait = 0; wait < ADC_VOLUME_TASK_STOP_WAIT_MS; wait++) {
        if ((g_adc_volume_generation != stop_generation) || (g_adc_volume.task_handle == NULL)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    g_adc_volume.volume_q8 = ADC_VOLUME_DEFAULT_Q8;
}

uint8_t adc_volume_is_running(void)
{
    return g_adc_volume.running;
}

uint16_t adc_volume_get_q8(void)
{
    return g_adc_volume.volume_q8;
}

uint16_t adc_volume_get_raw(uint8_t channel_index)
{
    if (channel_index >= ADC_VOLUME_CHANNEL_COUNT) {
        return 0U;
    }

    return g_adc_volume.filtered_raw[channel_index];
}
