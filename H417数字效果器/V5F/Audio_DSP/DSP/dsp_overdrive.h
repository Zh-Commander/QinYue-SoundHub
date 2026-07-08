#ifndef __DSP_OVERDRIVE_H
#define __DSP_OVERDRIVE_H

#include <stdint.h>

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

// ========================================
// 可调参数
// ========================================

/* ========== 模糊效果参数（Q8.24 定点格式） ========== */
/* 增益参数，值 0x01000000 在 Q8.24 中表示 1.0 (线性增益，0 dB) */
static int32_t od_gain;          /* 总输入增益 */
static int32_t od_volume;        /* 总输出音量 */
static int32_t od_low_gain_q24;  /* 低频搁架增益 */
static int32_t od_mid_gain_q24;  /* 中频带通增益 */
static int32_t od_mid_a_q24;     /* 中频谐振频率系数 */
static int32_t od_high_gain_q24; /* 高频搁架增益 */
static int32_t od_asym_q24;      /* 不对称软削波比例，固定为约70% */
static uint32_t od_asym_inv_q15; /* 不对称软削波比例的倒数 */

// ========================================
// 状态参数
// ========================================

/* --- 滤波器状态变量 --- */
/* 左声道各频段状态 */
static int32_t od_low_state_l;
static int32_t od_mid_lp_state_l;
static int32_t od_mid_hp_state_l;
static int32_t od_high_state_l;

/* 右声道各频段状态 */
static int32_t od_low_state_r;
static int32_t od_mid_lp_state_r;
static int32_t od_mid_hp_state_r;
static int32_t od_high_state_r;

/* 全局低通和高通状态 */
static int32_t od_lpf_state_l;
static int32_t od_lpf_state_r;
static int32_t od_hpf_state_l;
static int32_t od_hpf_state_r;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Overdrive_LoadDefaultParams(void)
{
    od_gain = float_to_q24(0.70f);         /* 总输入增益 */
    od_volume = float_to_q24(6.0f);        /* 总输出音量 */
    od_low_gain_q24 = float_to_q24(1.0f);  /* 低频搁架增益 */
    od_mid_gain_q24 = float_to_q24(1.0f);  /* 中频带通增益 */
    od_mid_a_q24 = MID_A_Q24;              /* 中频谐振频率系数 */
    od_high_gain_q24 = float_to_q24(1.0f); /* 高频搁架增益 */
    od_asym_q24 = 0x018C28F6;              /* 不对称软削波比例，固定为约70% */
    od_asym_inv_q15 = (uint32_t)((((uint64_t)Q24_ONE << 15) + ((uint64_t)od_asym_q24 * 2u)) /
                                 ((uint64_t)od_asym_q24 * 4u));
}

static inline void DSP_Overdrive_ClearState(void)
{
    /* 左声道各频段状态 */
    od_low_state_l = 0;
    od_mid_lp_state_l = 0;
    od_mid_hp_state_l = 0;
    od_high_state_l = 0;

    /* 右声道各频段状态 */
    od_low_state_r = 0;
    od_mid_lp_state_r = 0;
    od_mid_hp_state_r = 0;
    od_high_state_r = 0;

    /* 全局低通和高通状态 */
    od_lpf_state_l = 0;
    od_lpf_state_r = 0;
    od_hpf_state_l = 0;
    od_hpf_state_r = 0;
}

static inline void DSP_Overdrive_Init(void)
{
    DSP_Overdrive_LoadDefaultParams();
    DSP_Overdrive_ClearState();
}

typedef enum
{
    DSP_OD_PARAM_GAIN = 0, /* 0.05x ~ 1.0x */
    DSP_OD_PARAM_BASS,     /* 0.25x ~ 2.0x */
    DSP_OD_PARAM_MID,      /* 0.25x ~ 3.0x */
    DSP_OD_PARAM_MID_FREQ, /* 400 ~ 1000 Hz */
    DSP_OD_PARAM_TREBLE,   /* 0.25x ~ 2.0x */
    DSP_OD_PARAM_VOLUME,   /* 0.5x ~ 20.0x */
    DSP_OD_PARAM_ASYM,     /* 1.0x ~ 2.0x */

    DSP_OD_PARAM_NUM
} dsp_overdrive_param_t;

static inline void DSP_Overdrive_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_overdrive_param_t)param)
    {
        case DSP_OD_PARAM_GAIN:
        {
            od_gain = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.05f),
                float_to_q24(1.0f));
            break;
        }

        case DSP_OD_PARAM_BASS:
        {
            od_low_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_OD_PARAM_MID:
        {
            od_mid_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(3.0f));
            break;
        }

        case DSP_OD_PARAM_MID_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 400u, 1000u);
            od_mid_a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_OD_PARAM_TREBLE:
        {
            od_high_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_OD_PARAM_VOLUME:
        {
            od_volume = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.5f),
                float_to_q24(20.0f));
            break;
        }

        case DSP_OD_PARAM_ASYM:
        {
            od_asym_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(1.0f),
                float_to_q24(2.0f));

            od_asym_inv_q15 =
                (uint32_t)((((uint64_t)Q24_ONE << 15) +
                            ((uint64_t)od_asym_q24 * 2u)) /
                           ((uint64_t)od_asym_q24 * 4u));
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

/* --- 近似的三次函数软削波，带不对称特性 --- */
static inline __attribute__((always_inline)) int32_t soft_clip(int32_t x)
{
    if (x > 0x01000000)
        x = 0x01000000;

    if (x < -0x01000000)
        x = -0x01000000;

    int32_t x12 = x >> 12;
    int32_t x2 = x12 * x12;
    int32_t x3 = (x2 >> 12) * x12;

    int32_t cubic;

    if (x >= 0)
    {
        cubic = x3 >> 2;
    }
    else
    {
        cubic = (int32_t)(((int64_t)x3 * (int64_t)od_asym_inv_q15) >> 15);
    }

    return (x - cubic) * 3;
}

/* --- 单声道过载处理流水线 --- */
static inline __attribute__((always_inline)) int32_t process_od_channel(
    int32_t s,
    int32_t *low_state,
    int32_t *mid_lp_state,
    int32_t *mid_hp_state,
    int32_t *high_state,
    int32_t *lpf_state,
    int32_t *hpf_state)
{
    /* 输入增益调节：Q24 定点乘法 */
    s = qmul_fast(s, od_gain);

    /* 削波前应用全局高通滤波器，消除低频隆隆声 */
    s = apply_1pole_hpf_fast(s, hpf_state, HPF_A_Q24); /* HPF_A_Q24 为高通系数 */

    /* 软削波产生失真 */
    s = soft_clip(s);

    /* 削波后应用全局低通滤波器，减少嘶嘶声 */
    s = apply_1pole_lpf_fast(s, lpf_state, LPF_A_Q24); /* LPF_A_Q24 为低通系数 */

    /* 低频段处理：通过低通滤波器提取低频，并施加低频增益 */
    int32_t low_out = apply_1pole_lpf_fast(s, low_state, BASS_A_Q24); /* BASS_A_Q24 为低频搁架系数 */
    low_out = qmul_fast(low_out, od_low_gain_q24);

    /* 中频带通：先高通后低通，形成带通，并施加中频增益 */
    int32_t mid_band = apply_1pole_lpf_fast(
        apply_1pole_hpf_fast(s, mid_hp_state, od_mid_a_q24),
        mid_lp_state, od_mid_a_q24);
    int32_t mid_out = qmul_fast(mid_band, od_mid_gain_q24);

    /* 高频段：用原始信号减去低通信号得到高频搁架，并施加高频增益 */
    int32_t high_out = s - apply_1pole_lpf_fast(s, high_state, TREBLE_A_Q24); /* TREBLE_A_Q24 为高频搁架系数 */
    high_out = qmul_fast(high_out, od_high_gain_q24);

    /* 三频段混合，乘以总音量，四舍五入并限制到24位范围 */
    int64_t y = (int64_t)low_out + (int64_t)mid_out + (int64_t)high_out;
    y = y * (int64_t)od_volume;
    y += (y >= 0) ? (1LL << 23) : -(1LL << 23); /* 四舍五入至最近的整数 */
    int32_t output = clamp24((int32_t)(y >> 24));
    return output;
}

static inline void overdrive_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_od_channel(
            in_l[i],
            &od_low_state_l,
            &od_mid_lp_state_l,
            &od_mid_hp_state_l,
            &od_high_state_l,
            &od_lpf_state_l,
            &od_hpf_state_l);

        in_r[i] = in_l[i];
    }
}

static inline void overdrive_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_od_channel(
            in_l[i],
            &od_low_state_l,
            &od_mid_lp_state_l,
            &od_mid_hp_state_l,
            &od_high_state_l,
            &od_lpf_state_l,
            &od_hpf_state_l);

        in_r[i] = process_od_channel(
            in_r[i],
            &od_low_state_r,
            &od_mid_lp_state_r,
            &od_mid_hp_state_r,
            &od_high_state_r,
            &od_lpf_state_r,
            &od_hpf_state_r);
    }
}

// ========================================
// 音频处理函数
// ========================================

/* 对多帧立体声音频数据进行过载处理 */
static inline void overdrive_process_block(int32_t *in_l, int32_t *in_r, size_t frames)
{
    if (STEREO)
    {
        overdrive_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        overdrive_process_block_mono(in_l, in_r, frames);
    }
}

#endif