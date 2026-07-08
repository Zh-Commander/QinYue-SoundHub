#ifndef __CONFIG_H
#define __CONFIG_H

#include "ch32h417.h"
#include "stdint.h"

// === I2S配置 ===
#define I2S_SAMPLING_RATE (48 * 1000)
#define I2S_BIT_DEEPTH 24

#define I2S_RX SPI3
#define I2S_TX SPI2

#define I2S_BUFFER_FRAME 256 // 发送接收缓冲区帧数
#define I2S_BUFFER_HALF_FRAME (I2S_BUFFER_FRAME / 2)
#define I2S_BUFFER_DMA_CNT (I2S_BUFFER_FRAME * 2 * 2)
#define I2S_BUFFER_SIZE (I2S_BUFFER_FRAME * 2 * sizeof(uint32_t))

#define STEREO false  // 双声道开关,不超频不要开

typedef struct
{
    volatile uint16_t *I2S_RX_DMA_Buffer;
    volatile uint16_t *I2S_TX_DMA_Buffer;

    int32_t *I2S_RightDecode_Buffer;
    int32_t *I2S_LeftDecode_Buffer;
} I2S_Buffer_TransPkg_t;

// === 效果算法配置 ===
#define MAX_DSP_NUM 3            // 效果槽个数

#define DSP_EVENT_QUEUE_LEN 32u  // 效果参数更新队列长度
#define DSP_EVENT_QUEUE_MASK (DSP_EVENT_QUEUE_LEN - 1u)

#define DSP_EFFECT_TYPE_MAX 32u
#define DSP_PARAM_ID_MAX 32u

#ifndef POT_MAX
#define POT_MAX 4095
#endif

#define DSP_GLOBAL_VOLUME_CODE_MAX 999u
#define DSP_GLOBAL_VOLUME_Q16_ONE  0x00010000u
#define DSP_GLOBAL_VOLUME_Q16_MAX  (8u * DSP_GLOBAL_VOLUME_Q16_ONE)

enum                             // 效果枚举
{
    DSP_NULL_enum = 0,           // 无效果

    DSP_Chorus_enum = 1,
    DSP_Flanger_enum = 2,
    DSP_Phaser_enum = 3,
    DSP_Tremolo_enum = 4,
    DSP_Vibrato_enum = 5,

    DSP_Delay_enum = 6,
    DSP_EQ_enum = 7,

    DSP_Compressor_enum = 8,
    DSP_Distortion_enum = 9,
    DSP_Overdrive_enum = 10,
    DSP_Fuzz_enum = 11,

    DSP_SpeakerSim_enum = 12,

    DSP_Reverb_enum = 13,

    DSP_PreampMarshall_enum = 14,
    DSP_PreampVox_enum = 15,
    DSP_PreampFender_enum = 16,
    DSP_PreampSoldano_enum = 17,
};

typedef enum
{
    DSP_None_CMD = 0,

    DSP_SetSlotEffect_CMD,      // 设置效果槽
    DSP_SetEffectParam_CMD,     // 设置效果参数
    DSP_LoadDefaultParams_CMD,  // 恢复效果默认参数

} dsp_event_cmd_t;

typedef struct // 事件结构体
{
    uint32_t seq;
    uint32_t cmd;

    uint32_t slot;
    uint32_t effect_type;
    uint32_t param_id;

    int32_t value;

} dsp_event_t;

typedef struct // 事件队列
{
    volatile uint32_t wr;
    volatile uint32_t rd;

    volatile uint32_t overflow_count;
    volatile uint32_t last_applied_seq;
    volatile uint32_t error_count;

    volatile dsp_event_t event[DSP_EVENT_QUEUE_LEN];

} dsp_event_queue_t;

typedef struct
{
    // 效果部分传递结构体
    uint8_t Audio_DSP_Type[MAX_DSP_NUM];

    volatile uint32_t InputVolume_Q16;
    volatile uint32_t OutputVolume_Q16;

    dsp_event_queue_t EventQueue;

} Effect_Algorithm_TransPkg_t;

// === V5F通信结构体 ===
typedef struct
{
    I2S_Buffer_TransPkg_t *I2S_Buffer_TransPkg;
    Effect_Algorithm_TransPkg_t *Effect_Algorithm_TransPkg;
} Trans_Structure_t;

// ===  全局指针 ===
extern volatile uint16_t *I2S_RX_DMA_Buffer;
extern volatile uint16_t *I2S_TX_DMA_Buffer;

extern int32_t *I2S_RightDecode_Buffer;  // 右声道解码缓冲区
extern int32_t *I2S_LeftDecode_Buffer;   // 左声道解码缓冲区

extern volatile I2S_Buffer_TransPkg_t *I2S_Buffer_TransPkg;
extern volatile Effect_Algorithm_TransPkg_t *Effect_Algorithm_TransPkg;
extern volatile Trans_Structure_t *Trans_Structure;

#endif