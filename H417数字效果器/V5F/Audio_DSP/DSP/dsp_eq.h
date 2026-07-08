#ifndef __DSP_EQ_H
#define __DSP_EQ_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

// ========================================
// 可调参数
// ========================================

/* ========== 均衡器参数（Q8.24 定点格式） ========== */
/* 增益参数，值 0x01000000 在 Q8.24 中表示 1.0 (线性增益，0 dB) */
static int32_t eq_volume;        /* 均衡器总音量 */
static int32_t eq_low_gain_q24;  /* 低频增益 */
static int32_t eq_mid_gain_q24;  /* 中频增益 */
static int32_t eq_mid_a_q24;     /* 中频频带滤波器系数 */
static int32_t eq_high_gain_q24; /* 高频增益 */
static int32_t eq_lpf_a_q24;     /* 全局低通滤波器系数 */

// ========================================
// 状态参数
// ========================================

/* --- 滤波器状态变量 --- */
/* 左声道各频段状态 */
static int32_t eq_low_state_l;
static int32_t eq_mid_lp_state_l;
static int32_t eq_mid_hp_state_l;
static int32_t eq_high_state_l;

/* 右声道各频段状态 */
static int32_t eq_low_state_r;
static int32_t eq_mid_lp_state_r;
static int32_t eq_mid_hp_state_r;
static int32_t eq_high_state_r;

/* 全局低通和高通状态 */
static int32_t eq_lpf_state_l;
static int32_t eq_lpf_state_r;
static int32_t eq_hpf_state_l;
static int32_t eq_hpf_state_r;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_EQ_LoadDefaultParams(void)
{
    eq_volume = float_to_q24(8.0f);        /* 均衡器总音量，默认 1.0 */
    eq_low_gain_q24 = float_to_q24(2.3f);  /* 低频增益，默认 1.0 (0 dB) */
    eq_mid_gain_q24 = float_to_q24(0.3f);  /* 中频增益，默认 1.0 (0 dB) */
    eq_mid_a_q24 = MID_A_Q24;              /* 中频频带滤波器系数 */
    eq_high_gain_q24 = float_to_q24(0.3f); /* 高频增益，默认 1.0 (0 dB) */
    eq_lpf_a_q24 = LPF_A_Q24;              /* 全局低通滤波器系数 */
}

static inline void DSP_EQ_ClearState(void)
{
    /* 左声道各频段状态 */
    eq_low_state_l = 0;
    eq_mid_lp_state_l = 0;
    eq_mid_hp_state_l = 0;
    eq_high_state_l = 0;

    /* 右声道各频段状态 */
    eq_low_state_r = 0;
    eq_mid_lp_state_r = 0;
    eq_mid_hp_state_r = 0;
    eq_high_state_r = 0;

    /* 全局低通和高通状态 */
    eq_lpf_state_l = 0;
    eq_lpf_state_r = 0;
    eq_hpf_state_l = 0;
    eq_hpf_state_r = 0;
}

static inline void DSP_EQ_Init(void)
{
    DSP_EQ_LoadDefaultParams();
    DSP_EQ_ClearState();
}

typedef enum
{
    DSP_EQ_PARAM_BASS = 0, /* 0.25x ~ 2.0x */
    DSP_EQ_PARAM_MID,      /* 0.25x ~ 3.0x */
    DSP_EQ_PARAM_MID_FREQ, /* 300 ~ 1000 Hz */
    DSP_EQ_PARAM_TREBLE,   /* 0.25x ~ 2.0x */
    DSP_EQ_PARAM_LPF_FREQ, /* 3 kHz ~ 16 kHz */
    DSP_EQ_PARAM_VOLUME,   /* 0.1x ~ 8.0x */

    DSP_EQ_PARAM_NUM
} dsp_eq_param_t;

static inline void DSP_EQ_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_eq_param_t)param)
    {
        case DSP_EQ_PARAM_BASS:
        {
            eq_low_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_EQ_PARAM_MID:
        {
            eq_mid_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(3.0f));
            break;
        }

        case DSP_EQ_PARAM_MID_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 300u, 1000u);
            eq_mid_a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_EQ_PARAM_TREBLE:
        {
            eq_high_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_EQ_PARAM_LPF_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 3000u, 16000u);
            eq_lpf_a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_EQ_PARAM_VOLUME:
        {
            eq_volume = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.1f),
                float_to_q24(8.0f));
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

// --- 单通道均衡器处理 ---
static inline __attribute__((always_inline)) int32_t process_eq_channel(
    int32_t s,             /* 当前采样输入 */
    int32_t *low_state,    /* 低音滤波器状态指针 */
    int32_t *mid_lp_state, /* 中频带通滤波器的低通部分状态指针 */
    int32_t *mid_hp_state, /* 中频带通滤波器的高通部分状态指针 */
    int32_t *high_state,   /* 高音滤波器状态指针 */
    int32_t *lpf_state,    /* 全局低通滤波器状态指针 */
    int32_t *hpf_state     /* 全局高通滤波器状态指针（本函数未使用） */
)
{
    /* 输入衰减 -12dB 以防止削波 (右移2位相当于除以4，约-12dB) */
    s = s >> 2;

    /* 低频搁架式滤波器 */
    int32_t low_out = apply_1pole_lpf(s, low_state, BASS_A_Q24);     /* 全局低音：单极点低通 */
    low_out = (int32_t)(((int64_t)low_out * eq_low_gain_q24) >> 24); /* 乘以低音增益并转换为Q24整数 */

    /* 中频带通滤波器 */
    int32_t mid_band = apply_1pole_lpf(
        apply_1pole_hpf(s, mid_hp_state, eq_mid_a_q24),                       /* 先高通滤波 */
        mid_lp_state, eq_mid_a_q24                                            /* 再低通滤波，形成带通 */
    );
    int32_t mid_out = (int32_t)(((int64_t)mid_band * eq_mid_gain_q24) >> 24); /* 应用中音增益 */

    /* 高频搁架式滤波器（通过全通减去低通实现） */
    int32_t high_out = s - apply_1pole_lpf(s, high_state, TREBLE_A_Q24); /* 全局高音：全通信号减去低通部分 */
    high_out = (int32_t)(((int64_t)high_out * eq_high_gain_q24) >> 24);  /* 乘以高音增益 */

                                                                         /* 混合音调堆栈（将各频段的信号相加） */
    int64_t y = (int64_t)low_out + (int64_t)mid_out + (int64_t)high_out;
    y = (y * (int64_t)eq_volume) >> 24;

    int32_t y32 = clamp32(y);

    y32 = apply_1pole_lpf_fast(y32, lpf_state, eq_lpf_a_q24);

    return clamp24(y32);
}

static inline void eq_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_eq_channel(
            in_l[i],
            &eq_low_state_l,
            &eq_mid_lp_state_l,
            &eq_mid_hp_state_l,
            &eq_high_state_l,
            &eq_lpf_state_l,
            &eq_hpf_state_l);

        in_r[i] = in_l[i];
    }
}

static inline void eq_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_eq_channel(
            in_l[i],
            &eq_low_state_l,
            &eq_mid_lp_state_l,
            &eq_mid_hp_state_l,
            &eq_high_state_l,
            &eq_lpf_state_l,
            &eq_hpf_state_l);

        in_r[i] = process_eq_channel(
            in_r[i],
            &eq_low_state_r,
            &eq_mid_lp_state_r,
            &eq_mid_hp_state_r,
            &eq_high_state_r,
            &eq_lpf_state_r,
            &eq_hpf_state_r);
    }
}

// ========================================
// 音频处理函数
// ========================================

/* 对音频数据块逐帧进行均衡器处理 */
static inline void eq_process_block(int32_t *in_l, int32_t *in_r, size_t frames)
{
    if (STEREO)
    {
        eq_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        eq_process_block_mono(in_l, in_r, frames);
    }
}

#endif