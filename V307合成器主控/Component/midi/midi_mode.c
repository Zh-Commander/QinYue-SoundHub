#include "midi_mode.h"
#include "midi.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "ch32v30x_dma.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_misc.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_usart.h"
#include "..\\dynamic\\dynamic_audio.h"
#include "..\\loop\\loop.h"
#include <string.h>
#include <stdio.h>

#define MIDI_MODE_UART7_TASK_PRIO         9
#define MIDI_MODE_UART7_TASK_STACK        512
#define MIDI_MODE_UART7_TIMEOUT_MS        200
#define MIDI_MODE_SWITCH_SETTLE_MS        40
#define MIDI_MODE_TASK_STOP_WAIT_MS       1000
#define MIDI_MODE_UART7_CHANNEL           MIDI_CHANNEL_1
#define MIDI_MODE_DRUM_CHANNEL            MIDI_CHANNEL_10
#define MIDI_MODE_BASE_NOTE               60U
#define MIDI_MODE_PHYSICAL_KEY_VELOCITY   90U
#define MIDI_MODE_DRUM_KEY_TASK_PRIO      8
#define MIDI_MODE_DRUM_KEY_TASK_STACK     256
#define MIDI_MODE_DRUM_KEY_SCAN_MS        5U
#define MIDI_MODE_DRUM_KEY_DEBOUNCE_COUNT 3U

static TaskHandle_t g_midi_uart7_task_handle = NULL;
static TaskHandle_t g_midi_drum_key_task_handle = NULL;
static volatile uint8_t g_midi_uart7_running = 0U;
static volatile uint32_t g_midi_uart7_generation = 0U;
static volatile app_mode_t g_app_mode = APP_MODE_SWITCHING;
static SemaphoreHandle_t g_midi_send_mutex = NULL;

static uint8_t midi_mode_task_is_active(uint32_t task_generation)
{
    return (uint8_t)((g_midi_uart7_running != 0U) && (g_midi_uart7_generation == task_generation));
}
static char g_app_mode_error[64] = "";
static volatile uint8_t g_uart7_initialized = 0U;
static uint8_t g_uart7_rx_buffer[UART7_FRAME_SIZE];
static volatile uint8_t g_uart7_dma_buffer[UART7_FRAME_SIZE * 2U];
static uint32_t g_uart7_timeout_resync_count = 0U;

static const uint32_t g_uart7_crc32_table[256] = {
    0x00000000,0x77073096,0xEE0E612C,0x990951BA,0x076DC419,0x706AF48F,0xE963A535,0x9E6495A3,
    0x0EDB8832,0x79DCB8A4,0xE0D5E91E,0x97D2D988,0x09B64C2B,0x7EB17CBD,0xE7B82D07,0x90BF1D91,
    0x1DB71064,0x6AB020F2,0xF3B97148,0x84BE41DE,0x1ADAD47D,0x6DDDE4EB,0xF4D4B551,0x83D385C7,
    0x136C9856,0x646BA8C0,0xFD62F97A,0x8A65C9EC,0x14015C4F,0x63066CD9,0xFA0F3D63,0x8D080DF5,
    0x3B6E20C8,0x4C69105E,0xD56041E4,0xA2677172,0x3C03E4D1,0x4B04D447,0xD20D85FD,0xA50AB56B,
    0x35B5A8FA,0x42B2986C,0xDBBBC9D6,0xACBCF940,0x32D86CE3,0x45DF5C75,0xDCD60DCF,0xABD13D59,
    0x26D930AC,0x51DE003A,0xC8D75180,0xBFD06116,0x21B4F4B5,0x56B3C423,0xCFBA9599,0xB8BDA50F,
    0x2802B89E,0x5F058808,0xC60CD9B2,0xB10BE924,0x2F6F7C87,0x58684C11,0xC1611DAB,0xB6662D3D,
    0x76DC4190,0x01DB7106,0x98D220BC,0xEFD5102A,0x71B18589,0x06B6B51F,0x9FBFE4A5,0xE8B8D433,
    0x7807C9A2,0x0F00F934,0x9609A88E,0xE10E9818,0x7F6A0DBB,0x086D3D2D,0x91646C97,0xE6635C01,
    0x6B6B51F4,0x1C6C6162,0x856530D8,0xF262004E,0x6C0695ED,0x1B01A57B,0x8208F4C1,0xF50FC457,
    0x65B0D9C6,0x12B7E950,0x8BBEB8EA,0xFCB9887C,0x62DD1DDF,0x15DA2D49,0x8CD37CF3,0xFBD44C65,
    0x4DB26158,0x3AB551CE,0xA3BC0074,0xD4BB30E2,0x4ADFA541,0x3DD895D7,0xA4D1C46D,0xD3D6F4FB,
    0x4369E96A,0x346ED9FC,0xAD678846,0xDA60B8D0,0x44042D73,0x33031DE5,0xAA0A4C5F,0xDD0D7CC9,
    0x5005713C,0x270241AA,0xBE0B1010,0xC90C2086,0x5768B525,0x206F85B3,0xB966D409,0xCE61E49F,
    0x5EDEF90E,0x29D9C998,0xB0D09822,0xC7D7A8B4,0x59B33D17,0x2EB40D81,0xB7BD5C3B,0xC0BA6CAD,
    0xEDB88320,0x9ABFB3B6,0x03B6E20C,0x74B1D29A,0xEAD54739,0x9DD277AF,0x04DB2615,0x73DC1683,
    0xE3630B12,0x94643B84,0x0D6D6A3E,0x7A6A5AA8,0xE40ECF0B,0x9309FF9D,0x0A00AE27,0x7D079EB1,
    0xF00F9344,0x8708A3D2,0x1E01F268,0x6906C2FE,0xF762575D,0x806567CB,0x196C3671,0x6E6B06E7,
    0xFED41B76,0x89D32BE0,0x10DA7A5A,0x67DD4ACC,0xF9B9DF6F,0x8EBEEFF9,0x17B7BE43,0x60B08ED5,
    0xD6D6A3E8,0xA1D1937E,0x38D8C2C4,0x4FDFF252,0xD1BB67F1,0xA6BC5767,0x3FB506DD,0x48B2364B,
    0xD80D2BDA,0xAF0A1B4C,0x36034AF6,0x41047A60,0xDF60EFC3,0xA867DF55,0x316E8EEF,0x4669BE79,
    0xCB61B38C,0xBC66831A,0x256FD2A0,0x5268E236,0xCC0C7795,0xBB0B4703,0x220216B9,0x5505262F,
    0xC5BA3BBE,0xB2BD0B28,0x2BB45A92,0x5CB36A04,0xC2D7FFA7,0xB5D0CF31,0x2CD99E8B,0x5BDEAE1D,
    0x9B64C2B0,0xEC63F226,0x756AA39C,0x026D930A,0x9C0906A9,0xEB0E363F,0x72076785,0x05005713,
    0x95BF4A82,0xE2B87A14,0x7BB12BAE,0x0CB61B38,0x92D28E9B,0xE5D5BE0D,0x7CDCEFB7,0x0BDBDF21,
    0x86D3D2D4,0xF1D4E242,0x68DDB3F8,0x1FDA836E,0x81BE16CD,0xF6B9265B,0x6FB077E1,0x18B74777,
    0x88085AE6,0xFF0F6A70,0x66063BCA,0x11010B5C,0x8F659EFF,0xF862AE69,0x616BFFD3,0x166CCF45,
    0xA00AE278,0xD70DD2EE,0x4E048354,0x3903B3C2,0xA7672661,0xD06016F7,0x4969474D,0x3E6E77DB,
    0xAED16A4A,0xD9D65ADC,0x40DF0B66,0x37D83BF0,0xA9BCAE53,0xDEBB9EC5,0x47B2CF7F,0x30B5FFE9,
    0xBDBDF21C,0xCABAC28A,0x53B39330,0x24B4A3A6,0xBAD03605,0xCDD70693,0x54DE5729,0x23D967BF,
    0xB3667A2E,0xC4614AB8,0x5D681B02,0x2A6F2B94,0xB40BBE37,0xC30C8EA1,0x5A05DF1B,0x2D02EF8D
};

static int app_mode_start_free_play(void);
static void app_mode_stop_free_play(void);
static int app_mode_start_midi_keyboard(void);
static int app_mode_stop_midi_keyboard(void);
static void midi_mode_send_note_event_midi_velocity(uint8_t key_index, uint8_t midi_velocity, uint8_t pressed);

static void app_mode_set_error(const char *text)
{
    if (text == NULL) {
        g_app_mode_error[0] = '\0';
        return;
    }

    (void)snprintf(g_app_mode_error, sizeof(g_app_mode_error), "%s", text);
}

const char *app_mode_get_name(app_mode_t mode)
{
    switch (mode) {
    case APP_MODE_FREE_PLAY:
        return "FREE PLAY";
    case APP_MODE_MIDI_KEYBOARD:
        return "MIDI KEYBOARD";
    case APP_MODE_SWITCHING:
    default:
        return "SWITCHING";
    }
}

app_mode_t app_mode_get(void)
{
    return g_app_mode;
}

const char *app_mode_get_last_error(void)
{
    return g_app_mode_error;
}

uint8_t app_mode_is_loop_enabled(void)
{
    return (g_app_mode == APP_MODE_FREE_PLAY) ? 1U : 0U;
}

uint8_t app_mode_is_midi_uart_running(void)
{
    return g_midi_uart7_running;
}

static uint8_t midi_mode_key_to_note(uint8_t key_index)
{
    static const uint8_t drum_notes[DYNAMIC_DRUM_KEY_COUNT] = {36U, 38U, 42U, 46U, 45U, 49U};

    if ((key_index >= DYNAMIC_DRUM_KEY_BASE) &&
        (key_index < (DYNAMIC_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT))) {
        return drum_notes[key_index - DYNAMIC_DRUM_KEY_BASE];
    }

    return (uint8_t)(MIDI_MODE_BASE_NOTE + key_index - 1U);
}

static uint8_t midi_mode_key_to_channel(uint8_t key_index)
{
    if ((key_index >= DYNAMIC_DRUM_KEY_BASE) &&
        (key_index < (DYNAMIC_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT))) {
        return MIDI_MODE_DRUM_CHANNEL;
    }

    return MIDI_MODE_UART7_CHANNEL;
}

static uint8_t midi_mode_is_valid_key(uint8_t key_index)
{
    if ((key_index >= 1U) && (key_index <= DYNAMIC_LIVE_KEY_COUNT)) {
        return 1U;
    }

    return (uint8_t)((key_index >= DYNAMIC_DRUM_KEY_BASE) &&
                     (key_index < (DYNAMIC_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT)));
}

static uint8_t midi_mode_uart7_lane_to_physical_key(uint8_t lane_index)
{
    static const uint8_t lane_to_key_map[12] = {2U, 4U, 8U, 11U, 1U, 5U, 10U, 9U, 3U, 6U, 12U, 7U};

    if ((lane_index == 0U) || (lane_index > DYNAMIC_LIVE_KEY_COUNT)) {
        return lane_index;
    }

    if (lane_index <= 12U) {
        return lane_to_key_map[lane_index - 1U];
    }

    return lane_index;
}

static uint16_t midi_mode_remap_uart7_status_bits(uint16_t raw_status_bits)
{
    uint16_t mapped_status_bits = 0U;
    uint8_t lane_index;

    for (lane_index = 1U; lane_index <= DYNAMIC_LIVE_KEY_COUNT; lane_index++) {
        uint16_t lane_mask = (uint16_t)(1U << (lane_index - 1U));
        uint8_t physical_key = midi_mode_uart7_lane_to_physical_key(lane_index);

        if ((raw_status_bits & lane_mask) != 0U) {
            mapped_status_bits |= (uint16_t)(1U << (physical_key - 1U));
        }
    }

    return mapped_status_bits;
}

static void midi_mode_remap_uart7_force_buffer(uint8_t *dest_force_buffer, const uint8_t *src_force_buffer, uint8_t dest_size)
{
    uint8_t lane_index;

    if ((dest_force_buffer == NULL) || (src_force_buffer == NULL) || (dest_size < DYNAMIC_LIVE_KEY_COUNT)) {
        return;
    }

    memset(dest_force_buffer, 0, dest_size);
    for (lane_index = 1U; lane_index <= DYNAMIC_LIVE_KEY_COUNT; lane_index++) {
        uint8_t physical_key = midi_mode_uart7_lane_to_physical_key(lane_index);
        dest_force_buffer[physical_key - 1U] = src_force_buffer[lane_index - 1U];
    }
}

static uint32_t midi_mode_crc32_calculate(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFFUL;

    while (len-- != 0U) {
        crc = (crc >> 8) ^ g_uart7_crc32_table[(crc ^ *data++) & 0xFFU];
    }

    return crc ^ 0xFFFFFFFFUL;
}

static uint32_t midi_mode_read_u32_le(const uint8_t *data)
{
    return ((uint32_t)data[0]) |
           ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) |
           ((uint32_t)data[3] << 24);
}

static void midi_mode_uart7_clear_rx_errors(void)
{
    if ((UART7->STATR & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)) != 0U) {
        volatile uint32_t temp = UART7->STATR;
        temp = UART7->DATAR;
        (void)temp;
    }
}

static void midi_mode_uart7_init(void)
{
    GPIO_InitTypeDef gpio_init = {0};
    USART_InitTypeDef usart_init = {0};
    NVIC_InitTypeDef nvic = {0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    gpio_init.GPIO_Pin = GPIO_Pin_2;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &gpio_init);

    gpio_init.GPIO_Pin = GPIO_Pin_3;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &gpio_init);

    usart_init.USART_BaudRate = 1843200;
    usart_init.USART_WordLength = USART_WordLength_8b;
    usart_init.USART_StopBits = USART_StopBits_1;
    usart_init.USART_Parity = USART_Parity_No;
    usart_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART7, &usart_init);

    nvic.NVIC_IRQChannel = UART7_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 1;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    USART_Cmd(UART7, ENABLE);
    g_uart7_initialized = 1U;
}

static void midi_mode_uart7_dma_irq_init(void)
{
    NVIC_InitTypeDef nvic = {0};

    nvic.NVIC_IRQChannel = DMA2_Channel9_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 1;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    DMA_ClearITPendingBit(DMA2_IT_TC9);
    DMA_ClearFlag(DMA2_FLAG_GL9);
    DMA_ITConfig(DMA2_Channel9, DMA_IT_TC, ENABLE);
}

static void midi_mode_uart7_dma_rx_init(void)
{
    DMA_InitTypeDef dma_init = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(DMA2_Channel9, DISABLE);
    midi_mode_uart7_clear_rx_errors();

    DMA_DeInit(DMA2_Channel9);
    dma_init.DMA_PeripheralBaseAddr = (uint32_t)&UART7->DATAR;
    dma_init.DMA_MemoryBaseAddr = (uint32_t)g_uart7_dma_buffer;
    dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma_init.DMA_BufferSize = (uint16_t)sizeof(g_uart7_dma_buffer);
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    dma_init.DMA_Mode = DMA_Mode_Normal;
    dma_init.DMA_Priority = DMA_Priority_VeryHigh;
    dma_init.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel9, &dma_init);

    memset(g_uart7_rx_buffer, 0, sizeof(g_uart7_rx_buffer));
    memset((void *)g_uart7_dma_buffer, 0, sizeof(g_uart7_dma_buffer));
    DMA_ClearFlag(DMA2_FLAG_GL9);
}

static void midi_mode_uart7_restart_dma_rx(void)
{
    USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(DMA2_Channel9, DISABLE);
    midi_mode_uart7_clear_rx_errors();

    while (USART_GetFlagStatus(UART7, USART_FLAG_RXNE) == SET) {
        (void)USART_ReceiveData(UART7);
    }

    DMA_ClearFlag(DMA2_FLAG_GL9);
    DMA_SetCurrDataCounter(DMA2_Channel9, (uint16_t)sizeof(g_uart7_dma_buffer));
    memset((void *)g_uart7_dma_buffer, 0, sizeof(g_uart7_dma_buffer));
    DMA_Cmd(DMA2_Channel9, ENABLE);
    USART_DMACmd(UART7, USART_DMAReq_Rx, ENABLE);
}

static int midi_mode_uart7_wait_dma_rx_complete(TickType_t timeout_ticks)
{
    uint32_t notify_value = 0U;

    if (xTaskNotifyWait(0U, UART7_NOTIFY_ALL, &notify_value, timeout_ticks) != pdTRUE) {
        return -1;
    }

    return ((notify_value & UART7_NOTIFY_DMA_RX_DONE) != 0U) ? 0 : -1;
}

static int midi_mode_uart7_validate_frame(const uint8_t *frame)
{
    uint32_t crc_expected;
    uint32_t crc_actual;

    if ((frame[0] != UART7_FRAME_HEAD_H) || (frame[1] != UART7_FRAME_HEAD_L)) {
        return -1;
    }

    crc_expected = midi_mode_read_u32_le(frame + UART7_HEAD_SIZE + UART7_STATUS_SIZE + UART7_FORCE_SIZE);
    crc_actual = midi_mode_crc32_calculate(frame + UART7_HEAD_SIZE, UART7_STATUS_SIZE + UART7_FORCE_SIZE);
    return (crc_expected == crc_actual) ? 0 : -1;
}

static int midi_mode_uart7_extract_latest_valid_frame(uint8_t *frame)
{
    int offset;

    for (offset = (int)sizeof(g_uart7_dma_buffer) - UART7_FRAME_SIZE; offset >= 0; offset--) {
        if (midi_mode_uart7_validate_frame((const uint8_t *)&g_uart7_dma_buffer[offset]) == 0) {
            memcpy(frame, (const void *)&g_uart7_dma_buffer[offset], UART7_FRAME_SIZE);
            return 0;
        }
    }

    return -1;
}

static void midi_mode_send_note_event(uint8_t key_index, uint8_t velocity, uint8_t pressed)
{
    midi_mode_send_note_event_midi_velocity(key_index, midi_map_velocity_127(velocity), pressed);
}

static void midi_mode_send_note_event_midi_velocity(uint8_t key_index, uint8_t midi_velocity, uint8_t pressed)
{
    uint8_t note;
    uint8_t channel;
    int result;

    if (midi_mode_is_valid_key(key_index) == 0U) {
        return;
    }

    if (midi_velocity == 0U) {
        midi_velocity = 1U;
    }

    note = midi_mode_key_to_note(key_index);
    channel = midi_mode_key_to_channel(key_index);
    if (g_midi_send_mutex != NULL) {
        if (xSemaphoreTake(g_midi_send_mutex, pdMS_TO_TICKS(20)) != pdTRUE) {
            app_mode_set_error("MIDI send busy");
            return;
        }
    }

    if (pressed != 0U) {
        result = midi_send_note_on(channel, note, midi_velocity);
        if (result != 0) {
            app_mode_set_error("MIDI send failed");
        }
    } else {
        result = midi_send_note_off(channel, note);
        if (result != 0) {
            app_mode_set_error("MIDI send failed");
        }
    }

    if (g_midi_send_mutex != NULL) {
        xSemaphoreGive(g_midi_send_mutex);
    }
}

static void midi_mode_release_all_notes(void)
{
    uint8_t key_index;

    if (g_midi_send_mutex != NULL) {
        if (xSemaphoreTake(g_midi_send_mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
            return;
        }
    }

    for (key_index = 1U; key_index <= DYNAMIC_LIVE_KEY_COUNT; key_index++) {
        (void)midi_send_note_off(MIDI_MODE_UART7_CHANNEL, midi_mode_key_to_note(key_index));
    }

    for (key_index = 0U; key_index < DYNAMIC_DRUM_KEY_COUNT; key_index++) {
        (void)midi_send_note_off(MIDI_MODE_DRUM_CHANNEL,
                                 midi_mode_key_to_note((uint8_t)(DYNAMIC_DRUM_KEY_BASE + key_index)));
    }

    (void)midi_send_all_notes_off(MIDI_MODE_UART7_CHANNEL);
    (void)midi_send_all_notes_off(MIDI_MODE_DRUM_CHANNEL);

    if (g_midi_send_mutex != NULL) {
        xSemaphoreGive(g_midi_send_mutex);
    }
}

static void midi_mode_drum_key_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    gpio_init.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &gpio_init);
}

static void midi_mode_drum_key_task(void *pvParameters)
{
    uint32_t task_generation = (uint32_t)pvParameters;
    static const uint16_t drum_gpio_pins[DYNAMIC_DRUM_KEY_COUNT] = {
        GPIO_Pin_12,
        GPIO_Pin_11,
        GPIO_Pin_10,
        GPIO_Pin_9,
        GPIO_Pin_7,
        GPIO_Pin_8
    };
    uint8_t stable_state[DYNAMIC_DRUM_KEY_COUNT] = {0};
    uint8_t sample_state[DYNAMIC_DRUM_KEY_COUNT] = {0};
    uint8_t debounce_count[DYNAMIC_DRUM_KEY_COUNT] = {0};
    uint8_t initialized[DYNAMIC_DRUM_KEY_COUNT] = {0};
    uint8_t i;

    midi_mode_drum_key_gpio_init();
    Debug_Printf("[MIDI] drum key task entered\r\n");

    while (midi_mode_task_is_active(task_generation) != 0U) {
        for (i = 0U; i < DYNAMIC_DRUM_KEY_COUNT; i++) {
            uint8_t pressed = (GPIO_ReadInputDataBit(GPIOE, drum_gpio_pins[i]) == Bit_RESET) ? 1U : 0U;

            if (initialized[i] == 0U) {
                initialized[i] = 1U;
                sample_state[i] = pressed;
                stable_state[i] = pressed;
                debounce_count[i] = 0U;
                continue;
            }

            if (pressed != sample_state[i]) {
                sample_state[i] = pressed;
                debounce_count[i] = 0U;
                continue;
            }

            if (stable_state[i] == pressed) {
                debounce_count[i] = 0U;
                continue;
            }

            if (debounce_count[i] < MIDI_MODE_DRUM_KEY_DEBOUNCE_COUNT) {
                debounce_count[i]++;
            }

            if (debounce_count[i] < MIDI_MODE_DRUM_KEY_DEBOUNCE_COUNT) {
                continue;
            }

            stable_state[i] = pressed;
            debounce_count[i] = 0U;
            midi_mode_send_note_event_midi_velocity((uint8_t)(DYNAMIC_DRUM_KEY_BASE + i),
                                                    MIDI_MODE_PHYSICAL_KEY_VELOCITY,
                                                    pressed);

            if (pressed != 0U) {
                loop_on_live_note_on((uint8_t)(DYNAMIC_DRUM_KEY_BASE + i), MIDI_MODE_PHYSICAL_KEY_VELOCITY);
            } else {
                loop_on_live_note_off((uint8_t)(DYNAMIC_DRUM_KEY_BASE + i));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(MIDI_MODE_DRUM_KEY_SCAN_MS));
    }

    for (i = 0U; i < DYNAMIC_DRUM_KEY_COUNT; i++) {
        if (stable_state[i] != 0U) {
            midi_mode_send_note_event_midi_velocity((uint8_t)(DYNAMIC_DRUM_KEY_BASE + i),
                                                    MIDI_MODE_PHYSICAL_KEY_VELOCITY,
                                                    0U);
            loop_on_live_note_off((uint8_t)(DYNAMIC_DRUM_KEY_BASE + i));
        }
    }

    if (g_midi_uart7_generation == task_generation) {
        g_midi_drum_key_task_handle = NULL;
    }

    vTaskDelete(NULL);
}

static void midi_mode_uart7_task(void *pvParameters)
{
    uint32_t task_generation = (uint32_t)pvParameters;
    dynamic_key_mask_t previous_live_mask = 0U;
    uint8_t force_buffer[DYNAMIC_LIVE_KEY_COUNT] = {0};
    xTaskNotifyStateClear(NULL);
    midi_mode_uart7_init();
    midi_mode_uart7_dma_rx_init();
    midi_mode_uart7_dma_irq_init();
    midi_mode_uart7_restart_dma_rx();

    while (midi_mode_task_is_active(task_generation) != 0U) {
        dynamic_key_mask_t current_live_mask;
        uint16_t status_bits;
        uint8_t i;

        if (midi_mode_uart7_wait_dma_rx_complete(pdMS_TO_TICKS(MIDI_MODE_UART7_TIMEOUT_MS)) != 0) {
            if (midi_mode_task_is_active(task_generation) == 0U) {
                break;
            }
            g_uart7_timeout_resync_count++;
            midi_mode_uart7_restart_dma_rx();
            continue;
        }

        if (midi_mode_task_is_active(task_generation) == 0U) {
            break;
        }

        if (midi_mode_uart7_extract_latest_valid_frame(g_uart7_rx_buffer) != 0) {
            if (midi_mode_task_is_active(task_generation) == 0U) {
                break;
            }
            g_uart7_timeout_resync_count++;
            midi_mode_uart7_restart_dma_rx();
            continue;
        }

        g_uart7_timeout_resync_count = 0U;
        status_bits = (uint16_t)g_uart7_rx_buffer[UART7_HEAD_SIZE] |
                      ((uint16_t)g_uart7_rx_buffer[UART7_HEAD_SIZE + 1] << 8);
        midi_mode_remap_uart7_force_buffer(force_buffer,
                                           &g_uart7_rx_buffer[UART7_HEAD_SIZE + UART7_STATUS_SIZE],
                                           (uint8_t)sizeof(force_buffer));

        current_live_mask = (dynamic_key_mask_t)midi_mode_remap_uart7_status_bits(status_bits);
        if (midi_mode_task_is_active(task_generation) == 0U) {
            break;
        }
        midi_mode_uart7_restart_dma_rx();

        for (i = 0U; i < DYNAMIC_LIVE_KEY_COUNT; i++) {
            dynamic_key_mask_t mask = ((dynamic_key_mask_t)1U << i);
            if ((previous_live_mask & mask) == (current_live_mask & mask)) {
                continue;
            }

            midi_mode_send_note_event((uint8_t)(i + 1U), force_buffer[i], (current_live_mask & mask) ? 1U : 0U);
        }

        previous_live_mask = current_live_mask;
    }

    midi_mode_release_all_notes();
    if (g_midi_uart7_generation == task_generation) {
        DMA_Cmd(DMA2_Channel9, DISABLE);
        USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
        g_midi_uart7_task_handle = NULL;
    }
    vTaskDelete(NULL);
}

static int midi_mode_start_task(void)
{
    BaseType_t task_res;

    if (g_midi_uart7_running != 0U) {
        return 0;
    }

    g_midi_uart7_generation++;
    if (g_midi_uart7_generation == 0U) {
        g_midi_uart7_generation = 1U;
    }
    g_midi_uart7_running = 1U;
    task_res = xTaskCreate((TaskFunction_t)midi_mode_uart7_task,
                           (const char *)"midi_uart7",
                           (uint16_t)MIDI_MODE_UART7_TASK_STACK,
                           (void *)g_midi_uart7_generation,
                           (UBaseType_t)MIDI_MODE_UART7_TASK_PRIO,
                           (TaskHandle_t *)&g_midi_uart7_task_handle);
    if (task_res != pdPASS) {
        g_midi_uart7_running = 0U;
        app_mode_set_error("MIDI task create failed");
        return -1;
    }

    task_res = xTaskCreate((TaskFunction_t)midi_mode_drum_key_task,
                           (const char *)"midi_drum",
                           (uint16_t)MIDI_MODE_DRUM_KEY_TASK_STACK,
                           (void *)g_midi_uart7_generation,
                           (UBaseType_t)MIDI_MODE_DRUM_KEY_TASK_PRIO,
                           (TaskHandle_t *)&g_midi_drum_key_task_handle);
    if (task_res != pdPASS) {
        uint16_t wait;

        g_midi_uart7_running = 0U;
        if (g_midi_uart7_task_handle != NULL) {
            xTaskNotify(g_midi_uart7_task_handle, UART7_NOTIFY_RX_BYTE, eSetBits);
        }
        for (wait = 0U; wait < MIDI_MODE_TASK_STOP_WAIT_MS; wait++) {
            if (g_midi_uart7_task_handle == NULL) {
                break;
            }
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        app_mode_set_error("MIDI drum task create failed");
        return -1;
    }

    return 0;
}

static int midi_mode_stop_task(void)
{
    uint32_t stop_generation = g_midi_uart7_generation;
    uint8_t stopped = 0U;

    g_midi_uart7_running = 0U;
    if (g_midi_uart7_task_handle != NULL) {
        xTaskNotify(g_midi_uart7_task_handle, UART7_NOTIFY_RX_BYTE, eSetBits);
    }
    if (g_midi_drum_key_task_handle != NULL) {
        xTaskNotify(g_midi_drum_key_task_handle, UART7_NOTIFY_RX_BYTE, eSetBits);
    }

    USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(DMA2_Channel9, DISABLE);
    USART_ITConfig(UART7, USART_IT_RXNE, DISABLE);

    for (int wait = 0; wait < MIDI_MODE_TASK_STOP_WAIT_MS; wait++) {
        if ((g_midi_uart7_generation != stop_generation) ||
            ((g_midi_uart7_task_handle == NULL) && (g_midi_drum_key_task_handle == NULL))) {
            stopped = 1U;
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    g_uart7_initialized = 0U;
    vTaskDelay(pdMS_TO_TICKS(MIDI_MODE_SWITCH_SETTLE_MS));
    return (stopped != 0U) ? 0 : -1;
}

static int app_mode_start_free_play(void)
{
    if (dynamic_audio_init() != 0) {
        app_mode_set_error("Audio init failed");
        return -1;
    }

    if (loop_init() != 0) {
        dynamic_audio_deinit();
        app_mode_set_error("Loop init failed");
        return -1;
    }

    app_mode_set_error("");
    g_app_mode = APP_MODE_FREE_PLAY;
    return 0;
}

static void app_mode_stop_free_play(void)
{
    if (loop_is_running() != 0U) {
        loop_ui_state_t ui_state = {0};
        loop_get_ui_state(&ui_state);
        if (ui_state.recording != 0U) {
            (void)loop_ui_stop_record();
        }
        if (ui_state.playing != 0U) {
            (void)loop_ui_stop_playback();
        }
    }

    dynamic_audio_deinit();
    vTaskDelay(pdMS_TO_TICKS(MIDI_MODE_SWITCH_SETTLE_MS));
    loop_deinit();
    vTaskDelay(pdMS_TO_TICKS(MIDI_MODE_SWITCH_SETTLE_MS));
}

static int app_mode_start_midi_keyboard(void)
{
    if (midi_init() != 0) {
        app_mode_set_error("MIDI init failed");
        return -1;
    }

    if (g_midi_send_mutex == NULL) {
        g_midi_send_mutex = xSemaphoreCreateMutex();
        if (g_midi_send_mutex == NULL) {
            midi_deinit();
            app_mode_set_error("MIDI mutex create failed");
            return -1;
        }
    }

    (void)midi_send_program_change(MIDI_MODE_UART7_CHANNEL, MIDI_PROGRAM_ACOUSTIC_PIANO);
    if (midi_mode_start_task() != 0) {
        if (g_midi_send_mutex != NULL) {
            vSemaphoreDelete(g_midi_send_mutex);
            g_midi_send_mutex = NULL;
        }
        midi_deinit();
        return -1;
    }

    app_mode_set_error("");
    g_app_mode = APP_MODE_MIDI_KEYBOARD;
    return 0;
}

static int app_mode_stop_midi_keyboard(void)
{
    int stop_result;

    stop_result = midi_mode_stop_task();

    if (stop_result == 0) {
        midi_mode_release_all_notes();
        if (g_midi_send_mutex != NULL) {
            vSemaphoreDelete(g_midi_send_mutex);
            g_midi_send_mutex = NULL;
        }
        midi_deinit();
    } else {
        app_mode_set_error("MIDI stop late");
    }

    return 0;
}

int app_mode_init(void)
{
    g_app_mode = APP_MODE_SWITCHING;
    app_mode_set_error("");
    return app_mode_start_free_play();
}

int app_mode_switch(app_mode_t target_mode)
{
    app_mode_t previous_mode;
    int result;

    if ((target_mode != APP_MODE_FREE_PLAY) && (target_mode != APP_MODE_MIDI_KEYBOARD)) {
        app_mode_set_error("Invalid mode");
        return -1;
    }

    previous_mode = g_app_mode;
    if (previous_mode == target_mode) {
        return 0;
    }

    if (previous_mode == APP_MODE_SWITCHING) {
        app_mode_set_error("Mode busy");
        return -1;
    }

    g_app_mode = APP_MODE_SWITCHING;
    app_mode_set_error("");

    if (previous_mode == APP_MODE_FREE_PLAY) {
        app_mode_stop_free_play();
    } else if (previous_mode == APP_MODE_MIDI_KEYBOARD) {
        if (app_mode_stop_midi_keyboard() != 0) {
            g_app_mode = previous_mode;
            return -1;
        }
    }

    if (target_mode == APP_MODE_FREE_PLAY) {
        result = app_mode_start_free_play();
    } else {
        result = app_mode_start_midi_keyboard();
    }

    if (result != 0) {
        if (previous_mode == APP_MODE_FREE_PLAY) {
            (void)app_mode_start_free_play();
        } else if (previous_mode == APP_MODE_MIDI_KEYBOARD) {
            (void)app_mode_start_midi_keyboard();
        }
    }

    return result;
}

void app_mode_get_status(app_mode_status_t *status)
{
    midi_runtime_state_t midi_state = {0};

    if (status == NULL) {
        return;
    }

    memset(status, 0, sizeof(*status));
    midi_get_runtime_state(&midi_state);
    status->current_mode = g_app_mode;
    status->midi_ready = midi_state.ready;
    status->switching = (g_app_mode == APP_MODE_SWITCHING) ? 1U : 0U;
    status->last_note = midi_state.last_note;
    status->last_velocity = midi_state.last_velocity;
    status->note_on_count = midi_state.note_on_count;
    status->note_off_count = midi_state.note_off_count;
    status->send_error_count = midi_state.send_error_count;
    (void)snprintf(status->last_error, sizeof(status->last_error), "%s", g_app_mode_error);
}

void MidiMode_Uart7RxIsr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t status = UART7->STATR;

    if ((g_midi_uart7_running == 0U) || (g_midi_uart7_task_handle == NULL)) {
        return;
    }

    if ((status & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)) != 0U) {
        volatile uint32_t temp = UART7->DATAR;
        (void)temp;
    }

    if ((status & USART_FLAG_RXNE) != 0U) {
        xTaskNotifyFromISR(g_midi_uart7_task_handle,
                           UART7_NOTIFY_RX_BYTE,
                           eSetBits,
                           &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void MidiMode_Uart7DmaRxIsr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if ((g_midi_uart7_running == 0U) || (g_midi_uart7_task_handle == NULL)) {
        DMA_ClearITPendingBit(DMA2_IT_TC9);
        DMA_ClearFlag(DMA2_FLAG_GL9);
        return;
    }

    DMA_ClearITPendingBit(DMA2_IT_TC9);
    DMA_ClearFlag(DMA2_FLAG_GL9);
    xTaskNotifyFromISR(g_midi_uart7_task_handle,
                       UART7_NOTIFY_DMA_RX_DONE,
                       eSetBits,
                       &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
