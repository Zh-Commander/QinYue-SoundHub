/**
 * @file    dynamic_audio.c
 * @brief   动态多通道混音播放模块实现
 */

//=============================================================================
// HEADER INCLUDES
//=============================================================================

#include "dynamic_audio.h"
#include "debug.h"
#include "..\\adc\\adc_volume.h"
#include "..\\loop\\loop.h"
#include <string.h>
#include <stdio.h>

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* FatFS */
#include "..\\SD\\FATFS\\ff.h"
#include "..\\SD\\fs_lock.h"

/* Peripheral */
#include "..\\PCM5102\\PCM5102.h"

/* CH32V307 Peripheral */
#include "ch32v30x_spi.h"
#include "ch32v30x_dma.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_usart.h"
#include "ch32v30x_misc.h"

//=============================================================================
// CONFIGURATION
//=============================================================================

/** 任务优先级 (FreeRTOS数字越大优先级越高) */
#define TASK_PRIO_AUDIO_MIX     DYNAMIC_TASK_PRIO_AUDIO_MIX       /**< 混音任务优先级  */

/** 任务栈大小 */
#define TASK_STACK_AUDIO_MIX    768     /**< 混音任务栈深度(单位: words) */

/** 混音阶段单次文件读取块大小 (samples)
 *  音频任务已高于 UART7/UI，实时路径优先减少 f_read 次数与上下文切换。
 */
#define DYNAMIC_MIX_READ_CHUNK_SAMPLES  DYNAMIC_HALF_BUFFER_SIZE
#define DYNAMIC_TAIL_FADE_SAMPLES       128U
#define DYNAMIC_RELEASE_CLOSE_GRACE_MS  (((DYNAMIC_HALF_BUFFER_SIZE * 1000U) + DYNAMIC_SAMPLE_RATE - 1U) / DYNAMIC_SAMPLE_RATE)

/** 机械鼓按键任务配置 */
#define DYNAMIC_TASK_PRIO_DRUM_KEY      8
#define DYNAMIC_TASK_STACK_DRUM_KEY     256
#define DYNAMIC_DRUM_SCAN_PERIOD_MS     5U
#define DYNAMIC_DRUM_DEBOUNCE_COUNT     3U
#define DYNAMIC_DRUM_TRIGGER_VELOCITY   90U
#define DYNAMIC_DRUM_RECORD_VELOCITY    90U
#define DYNAMIC_TASK_STOP_WAIT_MS       300U

//=============================================================================
// TYPE DEFINITIONS
//=============================================================================

/**
 * @brief 播放通道结构体
 */
typedef struct {
    uint8_t key_index;                              /**< 虚拟键索引 (1-38) */
    FIL file;                                       /**< FatFS 文件对象 */
    playback_state_t state;                         /**< 播放状态 */
    uint32_t file_size;                             /**< 文件总字节数 */
    uint32_t played_bytes;                          /**< 已播放字节数 */
    int16_t read_buffer[DYNAMIC_READ_BUFFER_SIZE];  /**< 读取缓冲区 */
    TickType_t release_tick;                        /**< 释放时刻 */
    uint8_t need_restart;                           /**< 需要重新开始 */
    uint16_t volume_q8;                             /**< 当前通道音量系数(Q8, 0-256) */
} playback_channel_t;

/**
 * @brief 混音器结构体
 */
typedef struct {
    playback_channel_t channels[DYNAMIC_MAX_CHANNELS];  /**< 播放通道数组 */
    int16_t mix_buffer[DYNAMIC_AUDIO_BUFFER_SIZE];      /**< 混音输出缓冲区(16bit采样) */
    uint16_t dma_buffer[DYNAMIC_DMA_BUFFER_SIZE];       /**< I2S DMA发送缓冲区(16bit采样直发) */
    SemaphoreHandle_t mutex;                            /**< 互斥锁 */
    volatile uint8_t running;                           /**< 运行标志 */
    dynamic_key_mask_t key_states;                       /**< 合并后的按键状态位图 */
    volatile dynamic_key_mask_t target_key_states;       /**< 合并后的目标按键状态位图 */
    dynamic_key_mask_t live_key_states;                  /**< 实时输入按键状态位图 */
    dynamic_key_mask_t seq_key_states;                   /**< 回放输入按键状态位图 */
    dynamic_key_mask_t drum_key_states;                  /**< 机械鼓输入按键状态位图 */
    uint8_t live_force_buffer[DYNAMIC_LIVE_KEY_COUNT];   /**< 实时输入力度缓冲区 */
    uint8_t seq_force_buffer[DYNAMIC_SEQ_KEY_COUNT];     /**< 回放输入力度缓冲区 */
    uint8_t drum_force_buffer[DYNAMIC_DRUM_KEY_COUNT];   /**< 机械鼓输入力度缓冲区 */
    uint16_t release_delay_ms;                           /**< 延音时间 */
    uint16_t master_volume_q8;                           /**< 总音量系数(Q8, 0-256) */
    uint8_t seq_drum_force_buffer[DYNAMIC_DRUM_KEY_COUNT];
} dynamic_mixer_t;

//=============================================================================
// STATIC VARIABLES
//=============================================================================

/** 混音器实例 */
static dynamic_mixer_t mixer = {0};

/** 任务句柄 */
static TaskHandle_t audio_mix_task_handle = NULL;
static TaskHandle_t uart7_slave_task_handle = NULL;
static TaskHandle_t drum_key_task_handle = NULL;
static volatile uint32_t dynamic_runtime_generation = 0U;

static uint8_t dynamic_task_is_active(uint32_t task_generation)
{
    return (uint8_t)((mixer.running != 0U) && (dynamic_runtime_generation == task_generation));
}

void DynamicAudio_AudioDmaTxIsr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    DMA_ClearITPendingBit(DMA1_IT_TC5);
    DMA_ClearFlag(DMA1_FLAG_GL5);

    if (audio_mix_task_handle != NULL) {
        xTaskNotifyFromISR(audio_mix_task_handle,
                           AUDIO_NOTIFY_DMA_TX_DONE,
                           eSetBits,
                           &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void DynamicAudio_Uart7DmaRxIsr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    DMA_ClearITPendingBit(DMA2_IT_TC9);
    DMA_ClearFlag(DMA2_FLAG_GL9);

    if (uart7_slave_task_handle != NULL) {
        xTaskNotifyFromISR(uart7_slave_task_handle,
                           UART7_NOTIFY_DMA_RX_DONE,
                           eSetBits,
                           &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void DynamicAudio_Uart7RxIsr(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t status = UART7->STATR;

    if ((status & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)) != 0U) {
        volatile uint32_t temp = UART7->DATAR;
        (void)temp;
    }

    if ((status & USART_FLAG_RXNE) != 0U) {
        if (uart7_slave_task_handle != NULL) {
            xTaskNotifyFromISR(uart7_slave_task_handle,
                               UART7_NOTIFY_RX_BYTE,
                               eSetBits,
                               &xHigherPriorityTaskWoken);
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/** UART7副芯片接收缓冲区 */
static uint8_t uart7_rx_buffer[UART7_FRAME_SIZE];
static volatile uint8_t uart7_initialized = 0;

/** UART7 DMA批量接收缓冲区
 *  至少保留 2 帧，便于从任意帧边界切入时仍能在缓冲区内找到 1 帧完整有效帧。
 */
#define UART7_DMA_FRAME_BATCH_SIZE   (UART7_FRAME_SIZE * 2)
static volatile uint8_t uart7_dma_buffer[UART7_DMA_FRAME_BATCH_SIZE];

/** 超时自动执行时间 (ms) */
#define UART7_DMA_RX_TIMEOUT_MS      200
#define UART7_STARTUP_SYNC_TIMEOUT_MS 120
#define UART7_PRINT_INTERVAL_MS      50
#define UART7_ENABLE_PERIODIC_STATUS_LOG 1
#define UART7_TIMEOUT_RESYNC_COUNT   3
#define UART7_IDLE_CONFIRM_COUNT     2
#define UART7_IDLE_QUERY_MAX_RETRIES 10
#define UART7_IDLE_QUERY_TIMEOUT_MS  20
#define UART7_IDLE_QUERY_INTERVAL_MS 5

//=============================================================================
// PRIVATE FUNCTIONS DECLARATION
//=============================================================================

/* Utility */
static int find_slot_by_key(uint8_t key_index);
static int find_free_slot(void);
static uint8_t dynamic_is_valid_virtual_key(uint8_t key_index);
static uint8_t dynamic_is_live_drum_virtual_key(uint8_t key_index);
static uint8_t dynamic_is_seq_drum_virtual_key(uint8_t key_index);
static uint8_t dynamic_virtual_key_to_file_index(uint8_t key_index);
static uint8_t dynamic_uart7_lane_to_physical_key(uint8_t lane_index);
static uint16_t dynamic_remap_uart7_status_bits(uint16_t raw_status_bits);
static void dynamic_remap_uart7_force_buffer(uint8_t *dest_force_buffer, const uint8_t *src_force_buffer, uint8_t dest_size);
static dynamic_key_mask_t dynamic_virtual_key_to_mask(uint8_t key_index);
static void refresh_target_key_states(void);
static uint16_t get_key_volume_q8(uint8_t key_index);
static void update_channel_volume(int slot, uint8_t key_index);

static void dynamic_notify_audio_task(uint32_t bits);

/* File Operations */
static int open_audio_file(int slot, uint8_t key_index);
static void close_audio_file(int slot);
static void update_target_key_states(dynamic_key_mask_t new_target_states);

static void sync_key_states(void);
static void check_release_timeout(void);

/* Audio Mixing */
static void mix_all_channels(void);
static void copy_mix_to_dma_buffer(uint16_t *dst, const int16_t *src, uint16_t sample_count);

/* DMA Output */
static void i2s_init_for_dynamic(void);
static void audio_dma_tx_irq_init(void);
static int audio_wait_dma_tx_complete(TickType_t timeout_ticks);

/* UART Init */
static void uart7_slave_init(void);
static void uart7_clear_rx_errors(void);
static void uart7_reset_dma_parser(void);
static void uart7_restart_dma_rx(void);
static void uart7_dma_rx_init(void);
static void uart7_dma_irq_init(void);
static int uart7_wait_dma_rx_complete(TickType_t timeout_ticks);
static int uart7_wait_rx_byte(uint8_t *byte, TickType_t timeout_ticks);
static int uart7_try_lock_scan_stream(TickType_t timeout_ticks);
static int uart7_wait_idle_state(uint32_t task_generation);
static int uart7_sync_scan_stream(uint32_t task_generation);
static int uart7_validate_frame(const uint8_t *frame);
static int uart7_extract_latest_valid_frame(uint8_t *frame);
static uint32_t uart7_crc32_calculate(const uint8_t *data, uint32_t len);
static uint32_t uart7_read_u32_le(const uint8_t *data);

/* Drum key */
static void drum_key_gpio_init(void);
static void drum_key_release_all(void);
static void drum_key_scan_task(void *pvParameters);

/* FreeRTOS Tasks */
static void uart7_slave_recv_task(void *pvParameters);
static void audio_mix_task(void *pvParameters);

static const uint32_t uart7_crc32_table[256] = {
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

static uint32_t uart7_crc32_calculate(const uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;

    while (len--) {
        crc = (crc >> 8) ^ uart7_crc32_table[(crc ^ *data++) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

static uint32_t uart7_read_u32_le(const uint8_t *data)
{
    return ((uint32_t)data[0]) |
           ((uint32_t)data[1] << 8) |
           ((uint32_t)data[2] << 16) |
           ((uint32_t)data[3] << 24);
}

/**
 * @brief 根据按键索引查找播放槽位
 * @param key_index 虚拟键索引 (1-38)
 * @return 槽位索引，未找到返回 -1
 */
static int find_slot_by_key(uint8_t key_index)
{
    for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
        /* Releasing voices must keep ringing; a retrigger should open a new slot. */
        if (mixer.channels[i].state == PLAYBACK_PLAYING &&
            mixer.channels[i].key_index == key_index) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief 查找空闲播放槽位
 * @return 空闲槽位索引，无空闲返回 -1
 */
static int find_free_slot(void)
{
    for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
        if (mixer.channels[i].state == PLAYBACK_IDLE) {
            return i;
        }
    }
    return -1;
}

static uint8_t dynamic_is_valid_virtual_key(uint8_t key_index)
{
    return (key_index >= DYNAMIC_LIVE_KEY_BASE && key_index < (DYNAMIC_LIVE_KEY_BASE + DYNAMIC_TOTAL_KEYS));
}

static uint8_t dynamic_is_drum_virtual_key(uint8_t key_index)
{
    return (uint8_t)((key_index >= DYNAMIC_DRUM_KEY_BASE) &&
                     (key_index < (DYNAMIC_SEQ_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT)));
}

static uint8_t dynamic_is_live_drum_virtual_key(uint8_t key_index)
{
    return (uint8_t)((key_index >= DYNAMIC_DRUM_KEY_BASE) &&
                     (key_index < (DYNAMIC_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT)));
}

static uint8_t dynamic_is_seq_drum_virtual_key(uint8_t key_index)
{
    return (uint8_t)((key_index >= DYNAMIC_SEQ_DRUM_KEY_BASE) &&
                     (key_index < (DYNAMIC_SEQ_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT)));
}

static uint8_t dynamic_virtual_key_to_file_index(uint8_t key_index)
{
    if (dynamic_is_live_drum_virtual_key(key_index)) {
        return (uint8_t)(key_index - DYNAMIC_DRUM_KEY_BASE + 1U);
    }

    if (dynamic_is_seq_drum_virtual_key(key_index)) {
        return (uint8_t)(key_index - DYNAMIC_SEQ_DRUM_KEY_BASE + 1U);
    }

    return (uint8_t)(((key_index - DYNAMIC_LIVE_KEY_BASE) % DYNAMIC_LIVE_KEY_COUNT) + 1U);
}

static uint8_t dynamic_uart7_lane_to_physical_key(uint8_t lane_index)
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

static uint16_t dynamic_remap_uart7_status_bits(uint16_t raw_status_bits)
{
    uint16_t mapped_status_bits = 0U;
    uint8_t lane_index;

    for (lane_index = 1U; lane_index <= DYNAMIC_LIVE_KEY_COUNT; lane_index++) {
        uint16_t lane_mask = (uint16_t)(1U << (lane_index - 1U));
        uint8_t physical_key = dynamic_uart7_lane_to_physical_key(lane_index);

        if ((raw_status_bits & lane_mask) != 0U) {
            mapped_status_bits |= (uint16_t)(1U << (physical_key - 1U));
        }
    }

    return mapped_status_bits;
}

static void dynamic_remap_uart7_force_buffer(uint8_t *dest_force_buffer, const uint8_t *src_force_buffer, uint8_t dest_size)
{
    uint8_t lane_index;

    if ((dest_force_buffer == NULL) || (src_force_buffer == NULL) || (dest_size < DYNAMIC_LIVE_KEY_COUNT)) {
        return;
    }

    memset(dest_force_buffer, 0, dest_size);
    for (lane_index = 1U; lane_index <= DYNAMIC_LIVE_KEY_COUNT; lane_index++) {
        uint8_t physical_key = dynamic_uart7_lane_to_physical_key(lane_index);
        dest_force_buffer[physical_key - 1U] = src_force_buffer[lane_index - 1U];
    }
}

static int build_audio_filepath(char *filepath, size_t filepath_size, uint8_t key_index)
{
    static const char *const piano_note_files[12] = {
        "1:/timbre/piano/C5.pcm",
        "1:/timbre/piano/C#5.pcm",
        "1:/timbre/piano/D5.pcm",
        "1:/timbre/piano/D#5.pcm",
        "1:/timbre/piano/E5.pcm",
        "1:/timbre/piano/F5.pcm",
        "1:/timbre/piano/F#5.pcm",
        "1:/timbre/piano/G5.pcm",
        "1:/timbre/piano/G#5.pcm",
        "1:/timbre/piano/A5.pcm",
        "1:/timbre/piano/A#5.pcm",
        "1:/timbre/piano/B5.pcm"
    };
    uint8_t file_index;

    if ((filepath == NULL) || (filepath_size == 0U) || !dynamic_is_valid_virtual_key(key_index)) {
        return -1;
    }

    file_index = dynamic_virtual_key_to_file_index(key_index);
    if (dynamic_is_drum_virtual_key(key_index)) {
        snprintf(filepath, filepath_size, "1:/timbre/drum/%u.pcm", file_index);
    } else if (file_index <= 12U) {
        snprintf(filepath, filepath_size, "%s", piano_note_files[file_index - 1U]);
    } else {
        snprintf(filepath, filepath_size, "%s%s%d%s",
                 DYNAMIC_SD_PATH, DYNAMIC_FILE_PREFIX, file_index, DYNAMIC_FILE_EXT);
    }

    return 0;
}

static dynamic_key_mask_t dynamic_virtual_key_to_mask(uint8_t key_index)
{
    return ((dynamic_key_mask_t)1U << (key_index - DYNAMIC_LIVE_KEY_BASE));
}

static void dynamic_notify_audio_task(uint32_t bits)
{
    if (audio_mix_task_handle != NULL) {
        xTaskNotify(audio_mix_task_handle, bits, eSetBits);
    }
}

static void refresh_target_key_states(void)
{
    update_target_key_states(mixer.live_key_states | mixer.seq_key_states | mixer.drum_key_states);
}

static uint16_t get_key_volume_q8(uint8_t key_index)
{
    uint8_t velocity = 255;

    if (!dynamic_is_valid_virtual_key(key_index)) {
        return 256;
    }

    if (key_index >= DYNAMIC_LIVE_KEY_BASE && key_index < (DYNAMIC_LIVE_KEY_BASE + DYNAMIC_LIVE_KEY_COUNT)) {
        uint8_t live_idx = (uint8_t)(key_index - DYNAMIC_LIVE_KEY_BASE);
        velocity = mixer.live_force_buffer[live_idx];
        if ((velocity == 0U) && ((mixer.live_key_states & dynamic_virtual_key_to_mask(key_index)) != 0U)) {
            velocity = 1U;
        }
    } else if (key_index >= DYNAMIC_SEQ_KEY_BASE && key_index < (DYNAMIC_SEQ_KEY_BASE + DYNAMIC_SEQ_KEY_COUNT)) {
        uint8_t seq_idx = (uint8_t)(key_index - DYNAMIC_SEQ_KEY_BASE);
        velocity = mixer.seq_force_buffer[seq_idx];
        if (velocity == 0) {
            velocity = 255;
        }
    } else if (dynamic_is_live_drum_virtual_key(key_index)) {
        uint8_t drum_idx = (uint8_t)(key_index - DYNAMIC_DRUM_KEY_BASE);
        velocity = mixer.drum_force_buffer[drum_idx];
        if (velocity == 0U) {
            velocity = DYNAMIC_DRUM_TRIGGER_VELOCITY;
        }
    } else if (dynamic_is_seq_drum_virtual_key(key_index)) {
        uint8_t drum_idx = (uint8_t)(key_index - DYNAMIC_SEQ_DRUM_KEY_BASE);
        velocity = mixer.seq_drum_force_buffer[drum_idx];
        if (velocity == 0U) {
            velocity = DYNAMIC_DRUM_RECORD_VELOCITY;
        }
    }

    return (uint16_t)(((uint32_t)velocity * 256U + 127U) / 255U);
}

static void update_channel_volume(int slot, uint8_t key_index)
{
    mixer.channels[slot].volume_q8 = get_key_volume_q8(key_index);
}

void dynamic_set_master_volume_q8(uint16_t volume_q8)
{
    if (volume_q8 > 256U) {
        volume_q8 = 256U;
    }

    mixer.master_volume_q8 = volume_q8;
}

uint16_t dynamic_get_master_volume_q8(void)
{
    return mixer.master_volume_q8;
}

//=============================================================================
// PRIVATE FUNCTIONS: File Operations
//=============================================================================

/**
 * @brief 打开音频文件
 * @param slot 槽位索引
 * @param key_index 虚拟键索引 (1-38)
 * @return 0 成功, -1 失败
 */
static int open_audio_file(int slot, uint8_t key_index)
{
    char filepath[32];
    FRESULT res;

    if (build_audio_filepath(filepath, sizeof(filepath), key_index) != 0) {
        return -1;
    }

    if (fs_lock_take(pdMS_TO_TICKS(200)) != pdTRUE) {
        return -1;
    }

    /* 打开文件 */
    res = f_open(&mixer.channels[slot].file, filepath, FA_READ);
    fs_lock_give();
    if (res != FR_OK) {
        return -1;
    }

    /* 获取文件大小 */
    mixer.channels[slot].file_size = f_size(&mixer.channels[slot].file);
    mixer.channels[slot].played_bytes = 0;
    mixer.channels[slot].key_index = key_index;
    mixer.channels[slot].state = PLAYBACK_PLAYING;
    mixer.channels[slot].need_restart = 0;
    mixer.channels[slot].release_tick = 0;
    update_channel_volume(slot, key_index);

    return 0;
}

/**
 * @brief 关闭音频文件
 * @param slot 槽位索引
 */
/* Close an active channel and release its FatFS handle. */
static void close_audio_file(int slot)
{
    if (mixer.channels[slot].state != PLAYBACK_IDLE) {
        if (fs_lock_take(pdMS_TO_TICKS(200)) != pdTRUE) {
            return;
        }

        f_close(&mixer.channels[slot].file);
        fs_lock_give();
        mixer.channels[slot].state = PLAYBACK_IDLE;
        mixer.channels[slot].played_bytes = 0;
        mixer.channels[slot].file_size = 0;
        mixer.channels[slot].key_index = 0;
        mixer.channels[slot].need_restart = 0;
        mixer.channels[slot].release_tick = 0;
        mixer.channels[slot].volume_q8 = 0;
    }
}

//=============================================================================
// PRIVATE FUNCTIONS: Playback Pool Management
//=============================================================================

static void update_target_key_states(dynamic_key_mask_t new_target_states)
{
    dynamic_key_mask_t previous_states = mixer.target_key_states;
    dynamic_key_mask_t released_bits = previous_states & (~new_target_states);

    mixer.target_key_states = new_target_states;

    if (released_bits == 0) {
        dynamic_notify_audio_task(AUDIO_NOTIFY_WORK_PENDING);
        return;
    }

    for (int i = DYNAMIC_LIVE_KEY_BASE; i < (DYNAMIC_LIVE_KEY_BASE + DYNAMIC_TOTAL_KEYS); i++) {
        dynamic_key_mask_t mask = dynamic_virtual_key_to_mask((uint8_t)i);
        int slot;

        if ((released_bits & mask) == 0) {
            continue;
        }

        mixer.key_states &= ~mask;
        slot = find_slot_by_key((uint8_t)i);

        if (slot >= 0 && mixer.channels[slot].state != PLAYBACK_IDLE) {
            mixer.channels[slot].state = PLAYBACK_RELEASING;
            mixer.channels[slot].release_tick = xTaskGetTickCount();
        }
    }

    dynamic_notify_audio_task(AUDIO_NOTIFY_WORK_PENDING);
}

static void sync_key_states(void)
{
    dynamic_key_mask_t target_states = mixer.target_key_states;
    dynamic_key_mask_t current_states = mixer.key_states;
    dynamic_key_mask_t changed_bits = current_states ^ target_states;

    for (int i = DYNAMIC_LIVE_KEY_BASE; i < (DYNAMIC_LIVE_KEY_BASE + DYNAMIC_TOTAL_KEYS); i++) {
        dynamic_key_mask_t mask = dynamic_virtual_key_to_mask((uint8_t)i);
        int slot;

        if ((changed_bits & mask) == 0) {
            continue;
        }

        slot = find_slot_by_key((uint8_t)i);

        if (target_states & mask) {
            mixer.key_states |= mask;

            if (slot >= 0) {
                update_channel_volume(slot, (uint8_t)i);
                mixer.channels[slot].need_restart = 1;
                mixer.channels[slot].release_tick = 0;
                mixer.channels[slot].state = PLAYBACK_PLAYING;
            } else {
                slot = find_free_slot();
                if (slot >= 0 && open_audio_file(slot, (uint8_t)i) == 0) {
                    mixer.key_states |= mask;
                } else {
                    mixer.key_states &= ~mask;
                }
            }
        } else {
            mixer.key_states &= ~mask;

            if (slot >= 0 && mixer.channels[slot].state != PLAYBACK_IDLE) {
                mixer.channels[slot].state = PLAYBACK_RELEASING;
                mixer.channels[slot].release_tick = xTaskGetTickCount();
            }
        }
    }
}

static void check_release_timeout(void)
{
    TickType_t current_tick = xTaskGetTickCount();

    for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
        if (mixer.channels[i].state == PLAYBACK_RELEASING) {
            /* 计算已释放时间 */
            TickType_t elapsed = current_tick - mixer.channels[i].release_tick;
            uint32_t elapsed_ms = (elapsed * 1000) / configTICK_RATE_HZ;

            if (elapsed_ms >= (mixer.release_delay_ms + DYNAMIC_RELEASE_CLOSE_GRACE_MS)) {
                close_audio_file(i);
            }
        }
    }
}

//=============================================================================
// PRIVATE FUNCTIONS: Audio Mixing
//=============================================================================

/**
 * @brief 混合所有活跃通道
 * @note 真正的多通道混音：累加所有通道数据
 */
static void mix_all_channels(void)
{
    UINT bytes_read;
    FRESULT res;

    /* 清空混音缓冲区 */
    memset(mixer.mix_buffer, 0, sizeof(mixer.mix_buffer));

    /* 遍历所有通道 */
    for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {

        if (mixer.channels[i].state == PLAYBACK_IDLE) {
            continue;
        }

        /* 检查是否需要重新开始 */
        if (mixer.channels[i].need_restart) {
            if (fs_lock_take(pdMS_TO_TICKS(200)) == pdTRUE) {
                f_lseek(&mixer.channels[i].file, 0);
                fs_lock_give();
            }
            mixer.channels[i].played_bytes = 0;
            mixer.channels[i].need_restart = 0;
            mixer.channels[i].state = PLAYBACK_PLAYING;
            mixer.channels[i].release_tick = 0;
        }

        if (mixer.channels[i].state == PLAYBACK_IDLE) {
            continue;
        }

        /* 检查文件是否播放完毕 */
        if (mixer.channels[i].played_bytes >= mixer.channels[i].file_size) {
            close_audio_file(i);
            continue;
        }

        /* 分块读取一个半缓冲区，缩短单次 f_read() 阻塞窗口 */
        uint32_t bytes_remaining = mixer.channels[i].file_size - mixer.channels[i].played_bytes;
        uint32_t total_bytes_to_read = DYNAMIC_HALF_BUFFER_SIZE * sizeof(int16_t);
        uint32_t total_bytes_read = 0;
        uint8_t close_after_mix = 0U;

        if (bytes_remaining <= total_bytes_to_read) {
            total_bytes_to_read = bytes_remaining;
            close_after_mix = 1U;
        }

        if (mixer.channels[i].state == PLAYBACK_RELEASING) {
            TickType_t elapsed = xTaskGetTickCount() - mixer.channels[i].release_tick;
            uint32_t elapsed_ms = (elapsed * 1000U) / configTICK_RATE_HZ;

            if ((elapsed_ms + DYNAMIC_RELEASE_CLOSE_GRACE_MS) >= mixer.release_delay_ms) {
                close_after_mix = 1U;
            }
        }

        while (total_bytes_read < total_bytes_to_read) {
            uint32_t chunk_bytes = total_bytes_to_read - total_bytes_read;
            uint32_t max_chunk_bytes = DYNAMIC_MIX_READ_CHUNK_SAMPLES * sizeof(int16_t);

            if (chunk_bytes > max_chunk_bytes) {
                chunk_bytes = max_chunk_bytes;
            }

            if (mixer.channels[i].state == PLAYBACK_IDLE) {
                break;
            }

            if (fs_lock_take(pdMS_TO_TICKS(200)) != pdTRUE) {
                close_audio_file(i);
                break;
            }

            res = f_read(&mixer.channels[i].file,
                         ((uint8_t *)mixer.channels[i].read_buffer) + total_bytes_read,
                         chunk_bytes,
                         &bytes_read);
            fs_lock_give();

            if (res != FR_OK || bytes_read == 0) {
                close_audio_file(i);
                break;
            }

            total_bytes_read += bytes_read;
            mixer.channels[i].played_bytes += bytes_read;
        }

        if (mixer.channels[i].state == PLAYBACK_IDLE || total_bytes_read == 0) {
            continue;
        }

        /* 混音：累加到mix_buffer */
        int samples_read = total_bytes_read / sizeof(int16_t);
        uint16_t channel_volume_q8 = mixer.channels[i].volume_q8;
        int fade_start = samples_read;
        int fade_len = 0;

        if ((close_after_mix != 0U) && (samples_read > 0)) {
            fade_len = (samples_read > (int)DYNAMIC_TAIL_FADE_SAMPLES) ? (int)DYNAMIC_TAIL_FADE_SAMPLES : samples_read;
            fade_start = samples_read - fade_len;
        }

        for (int j = 0; j < samples_read && j < DYNAMIC_HALF_BUFFER_SIZE; j++) {
            int32_t sample = (int32_t)mixer.channels[i].read_buffer[j];
            int32_t scaled_sample = (sample * (int32_t)channel_volume_q8) >> 8;
            if ((fade_len > 0) && (j >= fade_start)) {
                int32_t fade_pos = (int32_t)(samples_read - j - 1);
                if (fade_pos < 0) {
                    fade_pos = 0;
                }
                scaled_sample = (scaled_sample * fade_pos) / fade_len;
            }
            scaled_sample = (scaled_sample * (int32_t)mixer.master_volume_q8) >> 8;
            int32_t sum = (int32_t)mixer.mix_buffer[j] + scaled_sample;
            /* 饱和限幅 */
            if (sum > 32767) sum = 32767;
            else if (sum < -32768) sum = -32768;
            mixer.mix_buffer[j] = (int16_t)sum;
        }

        if (close_after_mix != 0U) {
            close_audio_file(i);
        }
    }
}

static void copy_mix_to_dma_buffer(uint16_t *dst, const int16_t *src, uint16_t sample_count)
{
    uint16_t i;

    if ((dst == NULL) || (src == NULL)) {
        return;
    }

    for (i = 0; i < sample_count; i++) {
        dst[i] = (uint16_t)src[i];
    }
}

//=============================================================================
// PRIVATE FUNCTIONS: DMA Output
//=============================================================================

/**
 * @brief 初始化I2S用于动态播放
 */
static void i2s_init_for_dynamic(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2S_InitTypeDef I2S_InitStructure = {0};

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
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
    I2S_InitStructure.I2S_AudioFreq = I2S_AudioFreq_48k;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_Init(SPI2, &I2S_InitStructure);

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
    I2S_Cmd(SPI2, ENABLE);
}

static void audio_dma_tx_irq_init(void)
{
    NVIC_InitTypeDef nvic = {0};

    nvic.NVIC_IRQChannel = DMA1_Channel5_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    DMA_ClearITPendingBit(DMA1_IT_TC5);
    DMA_ClearFlag(DMA1_FLAG_GL5);
    DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
}

static int audio_wait_dma_tx_complete(TickType_t timeout_ticks)
{
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t remaining_ticks = timeout_ticks;
    uint32_t notify_value = 0;

    for (;;) {
        TickType_t elapsed_ticks;

        if (xTaskNotifyWait(0, AUDIO_NOTIFY_ALL, &notify_value, remaining_ticks) != pdTRUE) {
            return -1;
        }

        if ((notify_value & AUDIO_NOTIFY_DMA_TX_DONE) != 0U) {
            return 0;
        }

        elapsed_ticks = xTaskGetTickCount() - start_tick;
        if (elapsed_ticks >= timeout_ticks) {
            return -1;
        }

        remaining_ticks = timeout_ticks - elapsed_ticks;
    }
}

//=============================================================================
// PRIVATE FUNCTIONS: UART Parser
//=============================================================================
// PRIVATE FUNCTIONS: UART
//=============================================================================

/**
 * @brief 初始化UART7用于接收副芯片数据
 * @note  使用PC2-TX, PC3-RX，波特率1843200
 */
static void uart7_slave_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    /* 使能时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* 配置TX引脚(PC2)为复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置RX引脚(PC3)为浮空输入 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* 配置UART7 - 波特率1.8432Mbps */
    USART_InitStructure.USART_BaudRate = 1843200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(UART7, &USART_InitStructure);

    {
        NVIC_InitTypeDef nvic = {0};
        nvic.NVIC_IRQChannel = UART7_IRQn;
        nvic.NVIC_IRQChannelPreemptionPriority = 1;
        nvic.NVIC_IRQChannelSubPriority = 1;
        nvic.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&nvic);
    }

    /* 使能UART7 */
    USART_Cmd(UART7, ENABLE);

    uart7_initialized = 1;

    Debug_Printf("UART7 initialized (1843200bps, PC2-TX, PC3-RX) for slave chip communication\r\n");
}

static void uart7_clear_rx_errors(void)
{
    if (UART7->STATR & (USART_FLAG_ORE | USART_FLAG_NE | USART_FLAG_FE | USART_FLAG_PE)) {
        volatile uint32_t temp = UART7->STATR;
        temp = UART7->DATAR;
        (void)temp;
    }
}

static void uart7_reset_dma_parser(void)
{
    memset(uart7_rx_buffer, 0, sizeof(uart7_rx_buffer));
    memset((void *)uart7_dma_buffer, 0, sizeof(uart7_dma_buffer));
}

static void uart7_restart_dma_rx(void)
{
    USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(DMA2_Channel9, DISABLE);
    uart7_clear_rx_errors();

    while(USART_GetFlagStatus(UART7, USART_FLAG_RXNE) == SET)
    {
        (void)USART_ReceiveData(UART7);
    }

    DMA_ClearFlag(DMA2_FLAG_GL9);
    DMA_SetCurrDataCounter(DMA2_Channel9, UART7_DMA_FRAME_BATCH_SIZE);
    memset((void *)uart7_dma_buffer, 0, sizeof(uart7_dma_buffer));

    DMA_Cmd(DMA2_Channel9, ENABLE);
    USART_DMACmd(UART7, USART_DMAReq_Rx, ENABLE);
    uart7_clear_rx_errors();
}

/**
 * @brief 初始化UART7 DMA接收
 * @note  使用DMA2_Channel9接收UART7数据
 */
static void uart7_dma_rx_init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    /* 使能DMA2时钟 */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);

    /* 先关闭DMA请求，避免配置过程中残留数据进入 */
    USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
    DMA_Cmd(DMA2_Channel9, DISABLE);
    uart7_clear_rx_errors();

    /* 配置DMA2_Channel9用于UART7接收 */
    DMA_DeInit(DMA2_Channel9);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(UART7->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)uart7_dma_buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = UART7_DMA_FRAME_BATCH_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA2_Channel9, &DMA_InitStructure);

    uart7_reset_dma_parser();
    DMA_ClearFlag(DMA2_FLAG_GL9);
}

static void uart7_dma_irq_init(void)
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

static int uart7_wait_dma_rx_complete(TickType_t timeout_ticks)
{
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t remaining_ticks = timeout_ticks;
    uint32_t notify_value = 0;

    for (;;) {
        TickType_t elapsed_ticks;

        if (xTaskNotifyWait(0, UART7_NOTIFY_ALL, &notify_value, remaining_ticks) != pdTRUE) {
            return -1;
        }

        if ((notify_value & UART7_NOTIFY_DMA_RX_DONE) != 0U) {
            return 0;
        }

        elapsed_ticks = xTaskGetTickCount() - start_tick;
        if (elapsed_ticks >= timeout_ticks) {
            return -1;
        }

        remaining_ticks = timeout_ticks - elapsed_ticks;
    }
}

static int uart7_wait_rx_byte(uint8_t *byte, TickType_t timeout_ticks)
{
    TickType_t start_tick = xTaskGetTickCount();
    TickType_t remaining_ticks = timeout_ticks;
    uint32_t notify_value = 0;
    int result = -1;

    if (byte == NULL) {
        return -1;
    }

    xTaskNotifyStateClear(NULL);
    USART_ITConfig(UART7, USART_IT_RXNE, ENABLE);

    for (;;) {
        TickType_t elapsed_ticks;

        uart7_clear_rx_errors();

        if (USART_GetFlagStatus(UART7, USART_FLAG_RXNE) != RESET) {
            *byte = (uint8_t)USART_ReceiveData(UART7);
            result = 0;
            break;
        }

        if (remaining_ticks == 0) {
            break;
        }

        if (xTaskNotifyWait(0,
                            UART7_NOTIFY_ALL,
                            &notify_value,
                            remaining_ticks) != pdTRUE) {
            break;
        }

        if ((notify_value & UART7_NOTIFY_RX_BYTE) != 0U &&
            USART_GetFlagStatus(UART7, USART_FLAG_RXNE) != RESET) {
            *byte = (uint8_t)USART_ReceiveData(UART7);
            result = 0;
            break;
        }

        elapsed_ticks = xTaskGetTickCount() - start_tick;
        if (elapsed_ticks >= timeout_ticks) {
            break;
        }

        remaining_ticks = timeout_ticks - elapsed_ticks;
    }

    USART_ITConfig(UART7, USART_IT_RXNE, DISABLE);
    return result;
}

static int uart7_try_lock_scan_stream(TickType_t timeout_ticks)
{
    uart7_restart_dma_rx();

    if (uart7_wait_dma_rx_complete(timeout_ticks) != 0) {
        return -1;
    }

    if (uart7_extract_latest_valid_frame(uart7_rx_buffer) != 0) {
        return -1;
    }

    Debug_Printf("Slave chip scan stream locked\r\n");
    uart7_restart_dma_rx();
    return 0;
}

static int uart7_wait_idle_state(uint32_t task_generation)
{
    uint8_t cmd;
    int idle_confirmed = 0;
    int attempt = 0;

    Debug_Printf("Checking slave chip status...\r\n");

    while (attempt < UART7_IDLE_QUERY_MAX_RETRIES && idle_confirmed < UART7_IDLE_CONFIRM_COUNT) {
        if (dynamic_task_is_active(task_generation) == 0U) {
            return -1;
        }

        attempt++;
        uart7_clear_rx_errors();
        USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
        DMA_Cmd(DMA2_Channel9, DISABLE);

        while (USART_GetFlagStatus(UART7, USART_FLAG_RXNE) == SET) {
            (void)USART_ReceiveData(UART7);
        }

        USART_SendData(UART7, 0x00);
        while (USART_GetFlagStatus(UART7, USART_FLAG_TXE) == RESET);
        while (USART_GetFlagStatus(UART7, USART_FLAG_TC) == RESET);

        if (uart7_wait_rx_byte(&cmd, pdMS_TO_TICKS(UART7_IDLE_QUERY_TIMEOUT_MS)) != 0) {
            Debug_Printf("UART7 idle query timeout (%d/%d)\r\n",
                         attempt,
                         UART7_IDLE_QUERY_MAX_RETRIES);
            idle_confirmed = 0;
            vTaskDelay(pdMS_TO_TICKS(UART7_IDLE_QUERY_INTERVAL_MS));
            continue;
        }

        if (cmd == 0x00) {
            idle_confirmed++;
            Debug_Printf("UART7 idle query ack=0x00 (%d/%d)\r\n",
                         idle_confirmed,
                         UART7_IDLE_CONFIRM_COUNT);
        } else {
            Debug_Printf("UART7 idle query got 0x%02X, reset confirm\r\n", cmd);
            idle_confirmed = 0;
        }

        if (idle_confirmed < UART7_IDLE_CONFIRM_COUNT) {
            vTaskDelay(pdMS_TO_TICKS(UART7_IDLE_QUERY_INTERVAL_MS));
        }
    }

    if (idle_confirmed >= UART7_IDLE_CONFIRM_COUNT) {
        Debug_Printf("Slave chip is in IDLE state\r\n");
        return 0;
    }

    Debug_Printf("UART7 idle query failed after %d tries, continue with scan sync\r\n",
                 UART7_IDLE_QUERY_MAX_RETRIES);
    return -1;
}

static int uart7_sync_scan_stream(uint32_t task_generation)
{
    int attempt;

    Debug_Printf("Locking slave scan stream...\r\n");

    if (uart7_try_lock_scan_stream(pdMS_TO_TICKS(UART7_STARTUP_SYNC_TIMEOUT_MS)) == 0) {
        return 0;
    }

    Debug_Printf("No valid scan frame yet, sending scan mode command...\r\n");

    for (attempt = 0; attempt < 3; attempt++) {
        if (dynamic_task_is_active(task_generation) == 0U) {
            return -1;
        }

        USART_SendData(UART7, UART7_CMD_ENTER_SCAN);
        while (USART_GetFlagStatus(UART7, USART_FLAG_TXE) == RESET);
        while (USART_GetFlagStatus(UART7, USART_FLAG_TC) == RESET);

        if (uart7_try_lock_scan_stream(pdMS_TO_TICKS(UART7_STARTUP_SYNC_TIMEOUT_MS)) == 0) {
            return 0;
        }

        Debug_Printf("UART7 scan sync attempt %d failed\r\n", attempt + 1);
        USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
        DMA_Cmd(DMA2_Channel9, DISABLE);
        uart7_clear_rx_errors();
        while (USART_GetFlagStatus(UART7, USART_FLAG_RXNE) == SET) {
            (void)USART_ReceiveData(UART7);
        }
    }

    Debug_Printf("Slave chip scan stream lock timeout\r\n");
    uart7_restart_dma_rx();
    return -1;
}

static int uart7_validate_frame(const uint8_t *frame)
{
    uint32_t crc_expected;
    uint32_t crc_actual;

    if (frame[0] != UART7_FRAME_HEAD_H || frame[1] != UART7_FRAME_HEAD_L) {
        return -1;
    }

    crc_expected = uart7_read_u32_le(frame + UART7_HEAD_SIZE + UART7_STATUS_SIZE + UART7_FORCE_SIZE);
    crc_actual = uart7_crc32_calculate(frame + UART7_HEAD_SIZE,
                                       UART7_STATUS_SIZE + UART7_FORCE_SIZE);

    if (crc_actual != crc_expected) {
        return -1;
    }

    return 0;
}

static int uart7_extract_latest_valid_frame(uint8_t *frame)
{
    int offset;

    for (offset = UART7_DMA_FRAME_BATCH_SIZE - UART7_FRAME_SIZE; offset >= 0; offset--) {
        if (uart7_validate_frame((const uint8_t *)&uart7_dma_buffer[offset]) == 0) {
            memcpy(frame, (const void *)&uart7_dma_buffer[offset], UART7_FRAME_SIZE);
            return 0;
        }
    }

    return -1;
}

int uart7_handshake(void)
{
    /* 如果UART7未初始化，先初始化 */
    if(!uart7_initialized)
    {
        uart7_slave_init();
    }

    /* 添加延时让UART7稳定 */
    vTaskDelay(pdMS_TO_TICKS(100));

    Debug_Printf("Sending scan mode command to slave chip...\r\n");

    /*
     * 副芯片收到0x01后不会返回0x00确认，而是直接进入扫描并开始发送数据流。
     * 因此这里只负责发送命令，不等待应答字节。
     */
    USART_SendData(UART7, UART7_CMD_ENTER_SCAN);
    while(USART_GetFlagStatus(UART7, USART_FLAG_TXE) == RESET);
    while(USART_GetFlagStatus(UART7, USART_FLAG_TC) == RESET);

    Debug_Printf("Slave chip scan mode command sent\r\n");
    return 0;
}

//=============================================================================
// PRIVATE FUNCTIONS: FreeRTOS Tasks
//=============================================================================

/**
 * @brief UART7接收副芯片数据任务
 * @note  接收副芯片20字节标准帧，按 0xABCD + 2B状态 + 12B力度 + CRC32 解析
 */
static void uart7_slave_recv_task(void *pvParameters)
{
    uint32_t task_generation = (uint32_t)pvParameters;

    Debug_Printf("[Dynamic] uart7 task entered\r\n");
    uint16_t status_bits;
    TickType_t last_print_tick;
    uint8_t timeout_resync_count;

    xTaskNotifyStateClear(NULL);

    /* 初始化UART7 */
    uart7_slave_init();

    Debug_Printf("Initializing DMA receiver...\r\n");
    uart7_dma_rx_init();
    uart7_dma_irq_init();

    /* 优先直接锁定扫描流；若副芯片仍在IDLE，再补发查询/进入扫描命令 */
    if (uart7_sync_scan_stream(task_generation) != 0) {
        uart7_wait_idle_state(task_generation);
        uart7_sync_scan_stream(task_generation);
    }
    Debug_Printf("Receiving slave chip standard frames via DMA normal mode...\r\n");

    last_print_tick = xTaskGetTickCount();
    timeout_resync_count = 0;

    while (dynamic_task_is_active(task_generation) != 0U)
    {
        uint16_t current_status_bits;
        uint8_t current_force_buffer[UART7_FORCE_SIZE];
        dynamic_key_mask_t current_live_mask;
        dynamic_key_mask_t previous_live_mask;

        if (dynamic_task_is_active(task_generation) == 0U) {
            break;
        }

        if (uart7_wait_dma_rx_complete(pdMS_TO_TICKS(UART7_DMA_RX_TIMEOUT_MS)) != 0)
        {
            if (dynamic_task_is_active(task_generation) == 0U) {
                break;
            }

            timeout_resync_count++;

            if (timeout_resync_count >= UART7_TIMEOUT_RESYNC_COUNT)
            {
                mixer.live_key_states = 0;
                refresh_target_key_states();
                memset(mixer.live_force_buffer, 0, sizeof(mixer.live_force_buffer));

                for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
                    if (mixer.channels[i].state == PLAYBACK_PLAYING) {
                        update_channel_volume(i, mixer.channels[i].key_index);
                    }
                }

#if UART7_ENABLE_PERIODIC_STATUS_LOG
                if ((xTaskGetTickCount() - last_print_tick) >= pdMS_TO_TICKS(UART7_PRINT_INTERVAL_MS))
                {
                    Debug_Printf("Status: 0x0000 | Force: 0 0 0 0 0 0 0 0 0 0 0 0\r\n");
                    last_print_tick = xTaskGetTickCount();
                }
#endif
                timeout_resync_count = 0;
                if (uart7_sync_scan_stream(task_generation) != 0) {
                    uart7_wait_idle_state(task_generation);
                    uart7_sync_scan_stream(task_generation);
                }
            }
            else
            {
                if (dynamic_task_is_active(task_generation) == 0U) {
                    break;
                }
                uart7_restart_dma_rx();
            }
            continue;
        }

        if (dynamic_task_is_active(task_generation) == 0U) {
            break;
        }

        if (uart7_extract_latest_valid_frame(uart7_rx_buffer) != 0)
        {
            if (dynamic_task_is_active(task_generation) == 0U) {
                break;
            }

            timeout_resync_count++;

            if (timeout_resync_count >= UART7_TIMEOUT_RESYNC_COUNT)
            {
                mixer.live_key_states = 0;
                refresh_target_key_states();
                memset(mixer.live_force_buffer, 0, sizeof(mixer.live_force_buffer));

                for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
                    if (mixer.channels[i].state == PLAYBACK_PLAYING) {
                        update_channel_volume(i, mixer.channels[i].key_index);
                    }
                }

                timeout_resync_count = 0;
                if (uart7_sync_scan_stream(task_generation) != 0) {
                    uart7_wait_idle_state(task_generation);
                    uart7_sync_scan_stream(task_generation);
                }
            }
            else
            {
                if (dynamic_task_is_active(task_generation) == 0U) {
                    break;
                }
                uart7_restart_dma_rx();
            }
            continue;
        }

        timeout_resync_count = 0;
        current_status_bits = (uint16_t)uart7_rx_buffer[UART7_HEAD_SIZE] |
                              ((uint16_t)uart7_rx_buffer[UART7_HEAD_SIZE + 1] << 8);
        memcpy(current_force_buffer,
               &uart7_rx_buffer[UART7_HEAD_SIZE + UART7_STATUS_SIZE],
               UART7_FORCE_SIZE);

        if (dynamic_task_is_active(task_generation) == 0U) {
            break;
        }
        uart7_restart_dma_rx();

        previous_live_mask = mixer.live_key_states;
        current_live_mask = (dynamic_key_mask_t)dynamic_remap_uart7_status_bits(current_status_bits);
        status_bits = (uint16_t)current_live_mask;
        dynamic_remap_uart7_force_buffer(mixer.live_force_buffer, current_force_buffer, (uint8_t)sizeof(mixer.live_force_buffer));

        for (int i = 0; i < DYNAMIC_LIVE_KEY_COUNT; i++) {
            dynamic_key_mask_t mask = dynamic_virtual_key_to_mask((uint8_t)(DYNAMIC_LIVE_KEY_BASE + i));

            if ((previous_live_mask & mask) == (current_live_mask & mask)) {
                continue;
            }

            if (current_live_mask & mask) {
                loop_on_live_note_on((uint8_t)(DYNAMIC_LIVE_KEY_BASE + i), mixer.live_force_buffer[i]);
            } else {
                loop_on_live_note_off((uint8_t)(DYNAMIC_LIVE_KEY_BASE + i));
            }
        }

        mixer.live_key_states = current_live_mask;
        refresh_target_key_states();

        for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
            if (mixer.channels[i].state == PLAYBACK_PLAYING) {
                update_channel_volume(i, mixer.channels[i].key_index);
            }
        }

#if UART7_ENABLE_PERIODIC_STATUS_LOG
        if ((xTaskGetTickCount() - last_print_tick) >= pdMS_TO_TICKS(UART7_PRINT_INTERVAL_MS))
        {
            char line[128];
            int len;
            len = snprintf(line, sizeof(line), "Status: 0x%04X | Force: %d %d %d %d %d %d %d %d %d %d %d %d\r\n",
                           status_bits,
                           mixer.live_force_buffer[0], mixer.live_force_buffer[1], mixer.live_force_buffer[2], mixer.live_force_buffer[3],
                           mixer.live_force_buffer[4], mixer.live_force_buffer[5], mixer.live_force_buffer[6], mixer.live_force_buffer[7],
                           mixer.live_force_buffer[8], mixer.live_force_buffer[9], mixer.live_force_buffer[10], mixer.live_force_buffer[11]);
            if (len > 0)
            {
                Debug_Printf("%s", line);
            }
            last_print_tick = xTaskGetTickCount();
        }
#endif
    }

    if (dynamic_runtime_generation == task_generation) {
        DMA_Cmd(DMA2_Channel9, DISABLE);
        USART_DMACmd(UART7, USART_DMAReq_Rx, DISABLE);
        USART_ITConfig(UART7, USART_IT_RXNE, DISABLE);
        uart7_slave_task_handle = NULL;
    }

    vTaskDelete(NULL);
}
/**
 * @brief 混音播放任务
 * @note 嵌套循环模式：有活跃通道时进入播放循环
 */
static void drum_key_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    gpio_init.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOE, &gpio_init);
}

static void drum_key_release_all(void)
{
    mixer.drum_key_states = 0U;
    memset(mixer.drum_force_buffer, 0, sizeof(mixer.drum_force_buffer));

    if (mixer.running != 0U) {
        refresh_target_key_states();
    }
}

static void drum_key_scan_task(void *pvParameters)
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
    uint8_t initial_sync_done = 0U;
    uint8_t i;
    drum_key_gpio_init();
    Debug_Printf("[Dynamic] drum key task entered\r\n");

    while (dynamic_task_is_active(task_generation) != 0U) {
        uint8_t has_change = 0U;

        if (dynamic_task_is_active(task_generation) == 0U) {
            break;
        }

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

            if (debounce_count[i] < DYNAMIC_DRUM_DEBOUNCE_COUNT) {
                debounce_count[i]++;
            }

            if (debounce_count[i] < DYNAMIC_DRUM_DEBOUNCE_COUNT) {
                continue;
            }

            stable_state[i] = pressed;
            debounce_count[i] = 0U;
            has_change = 1U;

            if (pressed != 0U) {
                mixer.drum_force_buffer[i] = DYNAMIC_DRUM_TRIGGER_VELOCITY;
                loop_on_live_note_on((uint8_t)(DYNAMIC_DRUM_KEY_BASE + i), DYNAMIC_DRUM_RECORD_VELOCITY);
            } else {
                mixer.drum_force_buffer[i] = 0U;
                loop_on_live_note_off((uint8_t)(DYNAMIC_DRUM_KEY_BASE + i));
            }
        }

        if (has_change != 0U || initial_sync_done == 0U) {
            dynamic_key_mask_t drum_mask = 0U;

            if (dynamic_task_is_active(task_generation) == 0U) {
                break;
            }

            for (i = 0U; i < DYNAMIC_DRUM_KEY_COUNT; i++) {
                if (stable_state[i] != 0U) {
                    uint8_t key_index = (uint8_t)(DYNAMIC_DRUM_KEY_BASE + i);
                    drum_mask |= dynamic_virtual_key_to_mask(key_index);
                    mixer.drum_force_buffer[i] = DYNAMIC_DRUM_TRIGGER_VELOCITY;
                } else {
                    mixer.drum_force_buffer[i] = 0U;
                }
            }

            if (dynamic_task_is_active(task_generation) == 0U) {
                break;
            }

            mixer.drum_key_states = drum_mask;
            refresh_target_key_states();
            initial_sync_done = 1U;

            for (i = 0U; i < DYNAMIC_MAX_CHANNELS; i++) {
                if (mixer.channels[i].state == PLAYBACK_PLAYING) {
                    update_channel_volume(i, mixer.channels[i].key_index);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(DYNAMIC_DRUM_SCAN_PERIOD_MS));
    }

    if (dynamic_runtime_generation == task_generation) {
        drum_key_release_all();
        drum_key_task_handle = NULL;
    }
    vTaskDelete(NULL);
}

static void audio_mix_task(void *pvParameters)
{
    uint32_t task_generation = (uint32_t)pvParameters;

    Debug_Printf("[Dynamic] audio task entered\r\n");
    uint16_t dma_half_size = DYNAMIC_DMA_HALF_BUFFER_SIZE;
    uint8_t fill_half = 1U;
    uint16_t *fill_ptr = NULL;
    uint8_t dma_streaming = 0U;

    xTaskNotifyStateClear(NULL);
    audio_dma_tx_irq_init();

    SPI_I2S_DeInit(SPI2);
    i2s_init_for_dynamic();
    memset(mixer.dma_buffer, 0, sizeof(mixer.dma_buffer));
    memset(mixer.mix_buffer, 0, sizeof(mixer.mix_buffer));

    while (dynamic_task_is_active(task_generation) != 0U) {
        sync_key_states();
        check_release_timeout();

        if (dynamic_get_active_count() <= 0) {
            if (dma_streaming != 0U) {
                DMA_Cmd(DMA1_Channel5, DISABLE);
                dma_streaming = 0U;
                fill_half = 1U;
            }

            if (xTaskNotifyWait(0, AUDIO_NOTIFY_ALL, NULL, pdMS_TO_TICKS(10)) == pdTRUE) {
                xTaskNotifyStateClear(NULL);
            }
            continue;
        }

        if (dma_streaming == 0U) {
            dynamic_set_master_volume_q8(adc_volume_get_q8());
            mix_all_channels();
            copy_mix_to_dma_buffer(mixer.dma_buffer, mixer.mix_buffer, DYNAMIC_HALF_BUFFER_SIZE);
            xTaskNotifyStateClear(NULL);
            DMA_Tx_Init(DMA1_Channel5, (uint32_t)&SPI2->DATAR, (uint32_t)mixer.dma_buffer, dma_half_size);
            DMA_ClearITPendingBit(DMA1_IT_TC5);
            DMA_ClearFlag(DMA1_FLAG_GL5);
            DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
            DMA_Cmd(DMA1_Channel5, ENABLE);
            dma_streaming = 1U;
            fill_half = 1U;
            continue;
        }

        dynamic_set_master_volume_q8(adc_volume_get_q8());
        mix_all_channels();
        fill_ptr = mixer.dma_buffer + (fill_half * DYNAMIC_DMA_HALF_BUFFER_SIZE);
        copy_mix_to_dma_buffer(fill_ptr, mixer.mix_buffer, DYNAMIC_HALF_BUFFER_SIZE);

        if (audio_wait_dma_tx_complete(pdMS_TO_TICKS(100)) != 0) {
            DMA_Cmd(DMA1_Channel5, DISABLE);
            dma_streaming = 0U;
            fill_half = 1U;
            continue;
        }

        DMA_Cmd(DMA1_Channel5, DISABLE);
        DMA1_Channel5->MADDR = (uint32_t)fill_ptr;
        DMA_SetCurrDataCounter(DMA1_Channel5, dma_half_size);
        DMA_ClearITPendingBit(DMA1_IT_TC5);
        DMA_ClearFlag(DMA1_FLAG_GL5);
        DMA_Cmd(DMA1_Channel5, ENABLE);

        fill_half ^= 1U;
    }

    if (dynamic_runtime_generation == task_generation) {
        DMA_Cmd(DMA1_Channel5, DISABLE);
        I2S_Cmd(SPI2, DISABLE);
        audio_mix_task_handle = NULL;
    }

    vTaskDelete(NULL);
}


//=============================================================================
// PUBLIC API IMPLEMENTATION: Initialization
//=============================================================================

int dynamic_audio_init(void)
{
    BaseType_t task_res;

    if (mixer.running != 0U) {
        return 0;
    }

    Debug_Printf("[Dynamic] Initializing...\r\n");

    /* 初始化混音器 */
    memset(&mixer, 0, sizeof(mixer));
    dynamic_runtime_generation++;
    if (dynamic_runtime_generation == 0U) {
        dynamic_runtime_generation = 1U;
    }
    mixer.running = 1;
    mixer.release_delay_ms = DYNAMIC_RELEASE_DELAY_MS;
    mixer.master_volume_q8 = 256U;
    mixer.key_states = 0;
    mixer.live_key_states = 0;
    mixer.seq_key_states = 0;
    mixer.drum_key_states = 0;
    memset(mixer.live_force_buffer, 0, sizeof(mixer.live_force_buffer));
    memset(mixer.seq_force_buffer, 0, sizeof(mixer.seq_force_buffer));
    memset(mixer.drum_force_buffer, 0, sizeof(mixer.drum_force_buffer));
    memset(mixer.seq_drum_force_buffer, 0, sizeof(mixer.seq_drum_force_buffer));
    memset(uart7_rx_buffer, 0, sizeof(uart7_rx_buffer));

    /* 创建互斥锁 */
    mixer.mutex = xSemaphoreCreateMutex();
    if (mixer.mutex == NULL) {
        Debug_Printf("[Dynamic] Create mixer mutex failed\r\n");
        mixer.running = 0;
        return -1;
    }

    if (adc_volume_init() != 0) {
        Debug_Printf("[Dynamic] adc volume init failed\r\n");
        mixer.running = 0;
        vSemaphoreDelete(mixer.mutex);
        mixer.mutex = NULL;
        return -1;
    }

    /* 创建UART7副芯片接收任务 */
    task_res = xTaskCreate((TaskFunction_t)uart7_slave_recv_task,
                           (const char *)"uart7_slave",
                           (uint16_t)UART7_RECV_STK_SIZE,
                           (void *)dynamic_runtime_generation,
                           (UBaseType_t)UART7_RECV_TASK_PRIO,
                           (TaskHandle_t *)&uart7_slave_task_handle);
    if (task_res != pdPASS) {
        Debug_Printf("[Dynamic] Create uart7 task failed\r\n");
        mixer.running = 0;
        adc_volume_deinit();
        drum_key_task_handle = NULL;
        vSemaphoreDelete(mixer.mutex);
        mixer.mutex = NULL;
        return -1;
    }

    /* 创建机械鼓按键任务 */
    task_res = xTaskCreate((TaskFunction_t)drum_key_scan_task,
                           (const char *)"drum_key",
                           (uint16_t)DYNAMIC_TASK_STACK_DRUM_KEY,
                           (void *)dynamic_runtime_generation,
                           (UBaseType_t)DYNAMIC_TASK_PRIO_DRUM_KEY,
                           (TaskHandle_t *)&drum_key_task_handle);
    if (task_res != pdPASS) {
        Debug_Printf("[Dynamic] Create drum key task failed\r\n");
        mixer.running = 0;
        drum_key_task_handle = NULL;
        if (uart7_slave_task_handle != NULL) {
            xTaskNotify(uart7_slave_task_handle, UART7_NOTIFY_RX_BYTE, eSetBits);
        }
        adc_volume_deinit();
        vSemaphoreDelete(mixer.mutex);
        mixer.mutex = NULL;
        return -1;
    }

    /* 创建混音任务 */
    task_res = xTaskCreate((TaskFunction_t)audio_mix_task,
                           (const char *)"audio_mix",
                           (uint16_t)TASK_STACK_AUDIO_MIX,
                           (void *)dynamic_runtime_generation,
                           (UBaseType_t)TASK_PRIO_AUDIO_MIX,
                           (TaskHandle_t *)&audio_mix_task_handle);
    if (task_res != pdPASS) {
        Debug_Printf("[Dynamic] Create audio task failed\r\n");
        mixer.running = 0;
        if (uart7_slave_task_handle != NULL) {
            xTaskNotify(uart7_slave_task_handle, UART7_NOTIFY_RX_BYTE, eSetBits);
        }
        drum_key_release_all();
        adc_volume_deinit();
        vSemaphoreDelete(mixer.mutex);
        mixer.mutex = NULL;
        return -1;
    }

    Debug_Printf("Dynamic audio system initialized\r\n");
    Debug_Printf("UART7: Slave chip standard frame (0xABCD + 2B status + 12B force + CRC32)\r\n");
    Debug_Printf("Audio files: piano C5~B5 + input13.pcm~input16.pcm + drum1~drum6.pcm\r\n");
    return 0;
}

void dynamic_audio_deinit(void)
{
    uint32_t stop_generation;

    if (mixer.running == 0U && mixer.mutex == NULL) {
        return;
    }

    /* 停止任务 */
    stop_generation = dynamic_runtime_generation;
    mixer.running = 0;
    dynamic_notify_audio_task(AUDIO_NOTIFY_ALL);
    if (uart7_slave_task_handle != NULL) {
        xTaskNotify(uart7_slave_task_handle, UART7_NOTIFY_RX_BYTE, eSetBits);
    }

    for (int wait = 0; wait < DYNAMIC_TASK_STOP_WAIT_MS; wait++) {
        if ((dynamic_runtime_generation != stop_generation) ||
            (audio_mix_task_handle == NULL && uart7_slave_task_handle == NULL && drum_key_task_handle == NULL)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    adc_volume_deinit();
    mixer.live_key_states = 0;
    memset(mixer.live_force_buffer, 0, sizeof(mixer.live_force_buffer));
    mixer.drum_key_states = 0U;
    memset(mixer.drum_force_buffer, 0, sizeof(mixer.drum_force_buffer));
    dynamic_seq_release_all();
    refresh_target_key_states();

    for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
        close_audio_file(i);
    }

    uart7_initialized = 0;

    /* 删除互斥锁 */
    if (mixer.mutex != NULL) {
        vSemaphoreDelete(mixer.mutex);
        mixer.mutex = NULL;
    }
}

//=============================================================================
// PUBLIC API IMPLEMENTATION: Key Control
//=============================================================================

void dynamic_live_note_on(uint8_t key_index, uint8_t velocity)
{
    if (key_index < 1 || key_index > DYNAMIC_LIVE_KEY_COUNT) {
        return;
    }

    mixer.live_force_buffer[key_index - 1] = velocity;
    mixer.live_key_states |= dynamic_virtual_key_to_mask(key_index);
    refresh_target_key_states();
    loop_on_live_note_on(key_index, velocity);
}


void dynamic_live_note_off(uint8_t key_index)
{
    if (key_index < 1 || key_index > DYNAMIC_LIVE_KEY_COUNT) {
        return;
    }

    mixer.live_key_states &= ~dynamic_virtual_key_to_mask(key_index);
    refresh_target_key_states();
    loop_on_live_note_off(key_index);
}


void dynamic_seq_note_on(uint8_t key_index, uint8_t velocity)
{
    uint8_t virtual_key_index;

    if ((key_index < 1U) ||
        ((key_index > DYNAMIC_SEQ_KEY_COUNT) && !dynamic_is_live_drum_virtual_key(key_index))) {
        return;
    }

    if (dynamic_is_live_drum_virtual_key(key_index)) {
        uint8_t drum_idx = (uint8_t)(key_index - DYNAMIC_DRUM_KEY_BASE);
        virtual_key_index = (uint8_t)(DYNAMIC_SEQ_DRUM_KEY_BASE + drum_idx);
        mixer.seq_drum_force_buffer[drum_idx] = velocity;
    } else {
        virtual_key_index = (uint8_t)(DYNAMIC_SEQ_KEY_BASE + key_index - 1);
        mixer.seq_force_buffer[key_index - 1U] = velocity;
    }

    mixer.seq_key_states |= dynamic_virtual_key_to_mask(virtual_key_index);
    refresh_target_key_states();
}


void dynamic_seq_note_off(uint8_t key_index)
{
    uint8_t virtual_key_index;

    if ((key_index < 1U) ||
        ((key_index > DYNAMIC_SEQ_KEY_COUNT) && !dynamic_is_live_drum_virtual_key(key_index))) {
        return;
    }

    if (dynamic_is_live_drum_virtual_key(key_index)) {
        uint8_t drum_idx = (uint8_t)(key_index - DYNAMIC_DRUM_KEY_BASE);
        virtual_key_index = (uint8_t)(DYNAMIC_SEQ_DRUM_KEY_BASE + drum_idx);
        mixer.seq_drum_force_buffer[drum_idx] = 0U;
    } else {
        virtual_key_index = (uint8_t)(DYNAMIC_SEQ_KEY_BASE + key_index - 1);
    }

    mixer.seq_key_states &= ~dynamic_virtual_key_to_mask(virtual_key_index);
    refresh_target_key_states();
}


void dynamic_seq_release_all(void)
{

    /* 仅释放按键状态，保留最后一次力度，避免延音阶段被回退为默认满音量 */
    mixer.seq_key_states = 0;
    memset(mixer.seq_force_buffer, 0, sizeof(mixer.seq_force_buffer));
    memset(mixer.seq_drum_force_buffer, 0, sizeof(mixer.seq_drum_force_buffer));
    refresh_target_key_states();
}


//=============================================================================
// PUBLIC API IMPLEMENTATION: Status Query
//=============================================================================

int dynamic_get_active_count(void)
{
    int count = 0;

    for (int i = 0; i < DYNAMIC_MAX_CHANNELS; i++) {
        if (mixer.channels[i].state != PLAYBACK_IDLE) {
            count++;
        }
    }

    return count;
}

dynamic_key_mask_t dynamic_get_key_states(void)
{
    return mixer.key_states;
}

//=============================================================================
// PUBLIC API IMPLEMENTATION: Configuration
//=============================================================================

void dynamic_set_release_delay(uint16_t delay_ms)
{
    mixer.release_delay_ms = delay_ms;
    Debug_Printf("[Dynamic] Release delay set to %d ms\r\n", delay_ms);
}

uint16_t dynamic_get_release_delay(void)
{
    return mixer.release_delay_ms;
}

uint8_t dynamic_audio_is_running(void)
{
    return mixer.running;
}
