/**
 * @file    dynamic_audio.h
 * @brief   动态多通道混音播放模块
 * @details 支持48播放槽混音，16路实时输入 + 16路回放输入 + 6路机械鼓输入三来源键控，延音释放
 *
 * @note    前12键音频文件命名：1:/timbre/piano/C5.pcm ~ 1:/timbre/piano/B5.pcm，13~16 仍为 input13.pcm ~ input16.pcm，机械鼓键使用 1:/timbre/drum/1.pcm ~ 1:/timbre/drum/6.pcm
 */

#ifndef __DYNAMIC_AUDIO_H
#define __DYNAMIC_AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//=============================================================================
// CONFIGURATION
//=============================================================================

/** 最大混音通道数 */
#define DYNAMIC_MAX_CHANNELS        36

/** 实时输入键数量 */
#define DYNAMIC_LIVE_KEY_COUNT      16

/** 回放输入键数量 */
#define DYNAMIC_SEQ_KEY_COUNT       16

/** 机械鼓键数量 */
#define DYNAMIC_DRUM_KEY_COUNT      6

/** 总虚拟键数量 */
#define DYNAMIC_TOTAL_KEYS          (DYNAMIC_LIVE_KEY_COUNT + DYNAMIC_SEQ_KEY_COUNT + (DYNAMIC_DRUM_KEY_COUNT * 2))

/** 实时输入起始键索引 */
#define DYNAMIC_LIVE_KEY_BASE       1

/** 回放输入起始键索引 */
#define DYNAMIC_SEQ_KEY_BASE        (DYNAMIC_LIVE_KEY_BASE + DYNAMIC_LIVE_KEY_COUNT)

/** 机械鼓输入起始键索引 */
#define DYNAMIC_DRUM_KEY_BASE       (DYNAMIC_SEQ_KEY_BASE + DYNAMIC_SEQ_KEY_COUNT)

#define DYNAMIC_SEQ_DRUM_KEY_BASE   (DYNAMIC_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT)

/** 键状态位图类型 */
typedef uint64_t dynamic_key_mask_t;

/** 混音总缓冲区大小 (samples) - 与老工程一致 */
#define DYNAMIC_AUDIO_BUFFER_SIZE   2048

/** 混音半缓冲区大小 (samples) - 每次混音生成的采样数 */
#define DYNAMIC_HALF_BUFFER_SIZE    1024

/** 16bit I2S 输出时每个采样占用 1 个16位传输字 */
#define DYNAMIC_I2S_WORDS_PER_SAMPLE 1

/** DMA总缓冲区大小 (halfwords) */
#define DYNAMIC_DMA_BUFFER_SIZE     (DYNAMIC_AUDIO_BUFFER_SIZE * DYNAMIC_I2S_WORDS_PER_SAMPLE)

/** DMA半缓冲区大小 (halfwords) */
#define DYNAMIC_DMA_HALF_BUFFER_SIZE (DYNAMIC_HALF_BUFFER_SIZE * DYNAMIC_I2S_WORDS_PER_SAMPLE)

/** 每通道读取缓冲区大小 (samples) */
#define DYNAMIC_READ_BUFFER_SIZE    1024

/** 按键释放延音时间 (毫秒) */
#define DYNAMIC_RELEASE_DELAY_MS    2000

/** 采样率 */
#define DYNAMIC_SAMPLE_RATE         48000

/** SD卡路径 */
#define DYNAMIC_SD_PATH             "1:/"

/** 音频文件前缀 */
#define DYNAMIC_FILE_PREFIX         "input"

/** 音频文件扩展名 */
#define DYNAMIC_FILE_EXT            ".pcm"

//=============================================================================
// UART7 SLAVE CHIP CONFIGURATION
//=============================================================================

/** UART7副芯片通信协议定义 */
#define UART7_CMD_ENTER_SCAN        0x01      /**< 开启扫描模式命令 */
#define UART7_RET_YES               0x00      /**< 确认响应 */
#define UART7_RET_NO                0xFF      /**< 错误响应 */
#define UART7_FRAME_HEAD_H          0xAB      /**< 数据帧头高字节 */
#define UART7_FRAME_HEAD_L          0xCD      /**< 数据帧头低字节 */
#define UART7_HEAD_SIZE             2         /**< 帧头大小 */
#define UART7_STATUS_SIZE           2         /**< 按键状态位图大小 */
#define UART7_FORCE_SIZE            12        /**< 力度数据大小 */
#define UART7_CRC_SIZE              4         /**< CRC32大小 */
#define UART7_FRAME_SIZE            (UART7_HEAD_SIZE + UART7_STATUS_SIZE + UART7_FORCE_SIZE + UART7_CRC_SIZE)

/** UART7接收任务配置 */
#define UART7_RECV_TASK_PRIO        DYNAMIC_TASK_PRIO_UART7_RECV  /**< 任务优先级 */
#define UART7_RECV_STK_SIZE         512                           /**< 栈大小 */

/** UART7 DMA接收配置 */
#define UART7_DMA_BUFFER_SIZE       2048             /**< DMA循环接收缓冲区大小 */

/** 任务优先级规划：音频输出 > UART7接收 > UI > loop */
#define DYNAMIC_TASK_PRIO_AUDIO_MIX   10
#define DYNAMIC_TASK_PRIO_UART7_RECV   9

/** UART7事件通知位 */
#define UART7_NOTIFY_DMA_RX_DONE      (1UL << 0)
#define UART7_NOTIFY_RX_BYTE          (1UL << 1)
#define UART7_NOTIFY_ALL              (UART7_NOTIFY_DMA_RX_DONE | UART7_NOTIFY_RX_BYTE)

/** 音频任务事件通知位 */
#define AUDIO_NOTIFY_DMA_TX_DONE      (1UL << 0)
#define AUDIO_NOTIFY_WORK_PENDING     (1UL << 1)
#define AUDIO_NOTIFY_ALL              (AUDIO_NOTIFY_DMA_TX_DONE | AUDIO_NOTIFY_WORK_PENDING)

//=============================================================================
// TYPE DEFINITIONS
//=============================================================================

/**
 * @brief 播放状态枚举
 */
typedef enum {
    PLAYBACK_IDLE = 0,      /**< 空闲状态 */
    PLAYBACK_PLAYING,       /**< 播放中 (按键仍按下) */
    PLAYBACK_RELEASING      /**< 延音中 (按键已释放) */
} playback_state_t;

/**
 * @brief 钢琴实时按键索引定义 (1-16)
 */
typedef enum {
    KEY_1 = 1,
    KEY_2 = 2,
    KEY_3 = 3,
    KEY_4 = 4,
    KEY_5 = 5,
    KEY_6 = 6,
    KEY_7 = 7,
    KEY_8 = 8,
    KEY_9 = 9,
    KEY_10 = 10,
    KEY_11 = 11,
    KEY_12 = 12,
    KEY_13 = 13,
    KEY_14 = 14,
    KEY_15 = 15,
    KEY_16 = 16
} key_index_t;

//=============================================================================
// PUBLIC API - Initialization
//=============================================================================

/**
 * @brief 初始化动态混音系统
 * @note  内部创建串口接收任务、机械鼓按键任务和混音任务
 */
int dynamic_audio_init(void);

/**
 * @brief 清理动态混音系统资源
 */
void dynamic_audio_deinit(void);

/**
 * @brief 按下实时输入键
 * @param key_index 按键索引 (1-38)
 * @param velocity 力度值 (0-255)
 */
void dynamic_live_note_on(uint8_t key_index, uint8_t velocity);

/**
 * @brief 释放实时输入键
 * @param key_index 按键索引 (1-38)
 */
void dynamic_live_note_off(uint8_t key_index);

/**
 * @brief 按下回放输入键
 * @param key_index 按键索引 (1-38)
 * @param velocity 力度值 (0-255)
 */
void dynamic_seq_note_on(uint8_t key_index, uint8_t velocity);

/**
 * @brief 释放回放输入键
 * @param key_index 按键索引 (1-38)
 */
void dynamic_seq_note_off(uint8_t key_index);

/**
 * @brief 释放所有回放输入键
 */
void dynamic_seq_release_all(void);


//=============================================================================
// PUBLIC API - Status Query
//=============================================================================

/**
 * @brief 获取当前活跃播放通道数
 * @return 活跃通道数 (0-48)
 */
int dynamic_get_active_count(void);

/**
 * @brief 获取当前合并后的按键状态位图
 * @return 64位位图 (bit0=live key1, bit15=live key16, bit16=seq key1, bit31=seq key16, bit32~bit37=drum1~drum6)
 */
dynamic_key_mask_t dynamic_get_key_states(void);

//=============================================================================
// PUBLIC API - Configuration
//=============================================================================

/**
 * @brief 设置延音时间
 * @param delay_ms 延音时间 (毫秒)
 */
void dynamic_set_release_delay(uint16_t delay_ms);

/**
 * @brief 获取延音时间
 * @return 延音时间 (毫秒)
 */
uint16_t dynamic_get_release_delay(void);

uint8_t dynamic_audio_is_running(void);
void dynamic_set_master_volume_q8(uint16_t volume_q8);
uint16_t dynamic_get_master_volume_q8(void);

void DynamicAudio_AudioDmaTxIsr(void);
void DynamicAudio_Uart7DmaRxIsr(void);
void DynamicAudio_Uart7RxIsr(void);

#ifdef __cplusplus
}
#endif

#endif /* __DYNAMIC_AUDIO_H */
