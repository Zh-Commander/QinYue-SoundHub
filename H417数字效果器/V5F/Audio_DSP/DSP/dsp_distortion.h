#ifndef __DSP_DISTORTION_H
#define __DSP_DISTORTION_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

/* 固定的限幅阈值和非对称系数 */
#define DS_CLIP_THRESH_Q24 0x400000 /* 限幅阈值，Q8.24 格式，±0.25 */

// ========================================
// 可调参数
// ========================================

/* ========== 失真参数（Q8.24 定点格式） ========== */
/* 增益参数，值 0x01000000 在 Q8.24 中表示 1.0 (线性增益，0 dB) */
static int32_t ds_gain;          /* 输入增益 */
static int32_t ds_volume;        /* 输出音量 */
static int32_t ds_low_gain_q24;  /* 低频增益 */
static int32_t ds_mid_gain_q24;  /* 中频增益 */
static int32_t ds_mid_a_q24;     /* 中频滤波器系数 */
static int32_t ds_high_gain_q24; /* 高频增益 */
static int32_t ds_asym_q24;      /* 非对称系数，固定约 40%，用于负向限幅阈值计算 */

// ========================================
// 状态参数
// ========================================
/* --- 滤波器状态变量 --- */
/* 左声道各频段状态 */
static int32_t ds_low_state_l;
static int32_t ds_mid_lp_state_l;
static int32_t ds_mid_hp_state_l;
static int32_t ds_high_state_l;

/* 右声道各频段状态 */
static int32_t ds_low_state_r;
static int32_t ds_mid_lp_state_r;
static int32_t ds_mid_hp_state_r;
static int32_t ds_high_state_r;

/* 全局低通和高通状态 */
static int32_t ds_lpf_state_l;
static int32_t ds_lpf_state_r;
static int32_t ds_hpf_state_l;
static int32_t ds_hpf_state_r;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Distortion_LoadDefaultParams(void)
{
    ds_gain = float_to_q24(1.0f);          /* 输入增益，默认 1.0 */
    ds_volume = float_to_q24(6.0f);        /* 输出音量，默认 1.0 */
    ds_low_gain_q24 = float_to_q24(1.0f);  /* 低频增益，默认 1.0 (0 dB) */
    ds_mid_gain_q24 = float_to_q24(1.0f);  /* 中频增益，默认 1.0 (0 dB) */
    ds_mid_a_q24 = MID_A_Q24;              /* 中频滤波器系数 */
    ds_high_gain_q24 = float_to_q24(1.0f); /* 高频增益，默认 1.0 (0 dB) */
    ds_asym_q24 = 0x0119999A;              /* 非对称系数，固定约 40% */
}

static inline void DSP_Distortion_ClearState(void)
{
    /* 左声道各频段状态 */
    ds_low_state_l = 0;
    ds_mid_lp_state_l = 0;
    ds_mid_hp_state_l = 0;
    ds_high_state_l = 0;

    /* 右声道各频段状态 */
    ds_low_state_r = 0;
    ds_mid_lp_state_r = 0;
    ds_mid_hp_state_r = 0;
    ds_high_state_r = 0;

    /* 全局低通和高通状态 */
    ds_lpf_state_l = 0;
    ds_lpf_state_r = 0;
    ds_hpf_state_l = 0;
    ds_hpf_state_r = 0;
}

static inline void DSP_Distortion_Init(void)
{
    DSP_Distortion_LoadDefaultParams();
    DSP_Distortion_ClearState();
}

typedef enum
{
    DSP_DIST_PARAM_GAIN = 0, /* 0.05x ~ 1.0x */
    DSP_DIST_PARAM_BASS,     /* 0.25x ~ 2.0x */
    DSP_DIST_PARAM_MID,      /* 0.25x ~ 3.0x */
    DSP_DIST_PARAM_MID_FREQ, /* 400 ~ 1000 Hz */
    DSP_DIST_PARAM_TREBLE,   /* 0.25x ~ 2.0x */
    DSP_DIST_PARAM_VOLUME,   /* 0.5x ~ 26.0x */
    DSP_DIST_PARAM_ASYM,     /* 0.5x ~ 2.0x */

    DSP_DIST_PARAM_NUM
} dsp_distortion_param_t;

static inline void DSP_Distortion_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_distortion_param_t)param)
    {
        case DSP_DIST_PARAM_GAIN:
        {
            ds_gain = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.05f),
                float_to_q24(1.0f));
            break;
        }

        case DSP_DIST_PARAM_BASS:
        {
            ds_low_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_DIST_PARAM_MID:
        {
            ds_mid_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(3.0f));
            break;
        }

        case DSP_DIST_PARAM_MID_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 400u, 1000u);
            ds_mid_a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_DIST_PARAM_TREBLE:
        {
            ds_high_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_DIST_PARAM_VOLUME:
        {
            ds_volume = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.5f),
                float_to_q24(26.0f));
            break;
        }

        case DSP_DIST_PARAM_ASYM:
        {
            ds_asym_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.5f),
                float_to_q24(2.0f));
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

/* 带非对称和软拐点的限幅削波阶段 */
static inline int32_t diode_clip(int32_t x)
{
    const int32_t knee = 0x040000; /* 软拐点宽度，0x040000 表示 Q8.24 的 0.0625 */

    int32_t pos_thresh = DS_CLIP_THRESH_Q24;
    /* 原代码：
       int32_t neg_thresh = -((int32_t)(((int64_t)DS_CLIP_THRESH_Q24 * ds_asym_q24) >> 24));
       现使用优化后的定点乘法 qmul_fast 计算负向阈值 */
    int32_t neg_thresh = -qmul_fast(DS_CLIP_THRESH_Q24, ds_asym_q24);

    /* 软拐点限幅：在阈值附近使用线性插值，超出拐点范围则直接钳位 */
    if (x > pos_thresh + knee)
    {
        x = pos_thresh; /* 超出正阈值加拐点范围，硬钳位为 pos_thresh */
    }
    else if (x > pos_thresh)
    {
        x = pos_thresh - ((x - pos_thresh) >> 1); /* 软拐点区间线性衰减 */
    }
    else if (x < neg_thresh - knee)
    {
        x = neg_thresh; /* 超出负阈值减拐点范围，硬钳位为 neg_thresh */
    }
    else if (x < neg_thresh)
    {
        x = neg_thresh + ((neg_thresh - x) >> 1); /* 软拐点区间线性衰减 */
    }

    return x * 6; /* 补偿增益（纯整数乘法，结果仍在 Q8.24 域）*/
}

/* 单通道失真处理 */
static inline __attribute__((always_inline)) int32_t process_ds_channel(
    int32_t s,
    int32_t *low_state,
    int32_t *mid_lp_state,
    int32_t *mid_hp_state,
    int32_t *high_state,
    int32_t *lpf_state,
    int32_t *hpf_state)
{
    /* 原代码：s = (int32_t)(((int64_t)s * ds_gain) >> 24); 现改用定点乘法 qmul_fast */
    s = qmul_fast(s, ds_gain); /* 施加输入增益 */

    /* 在削波前使用高通滤波器去除低频杂音 (rumble) */
    s = apply_1pole_hpf_fast(s, hpf_state, HPF_A_Q24);

    /* 削波处理 */
    s = diode_clip(s);

    /* 削波后使用低通滤波器平滑高频毛刺 (fizz) */
    s = apply_1pole_lpf_fast(s, lpf_state, LPF_A_Q24);

    /* 低频搁架滤波 */
    int32_t low_out = apply_1pole_lpf_fast(s, low_state, BASS_A_Q24);
    /* 原代码：low_out = (int32_t)(((int64_t)low_out * ds_low_gain_q24) >> 24); */
    low_out = qmul_fast(low_out, ds_low_gain_q24); /* 低频增益调节 */

    /* 中频带通滤波：先高通后低通，形成带通 */
    int32_t mid_band = apply_1pole_lpf_fast(
        apply_1pole_hpf_fast(s, mid_hp_state, ds_mid_a_q24),
        mid_lp_state, ds_mid_a_q24);
    /* 原代码：mid_out = (int32_t)(((int64_t)mid_band * ds_mid_gain_q24) >> 24); */
    int32_t mid_out = qmul_fast(mid_band, ds_mid_gain_q24); /* 中频增益调节 */

    /* 高频搁架滤波：原始信号减去低通部分得到高频 */
    int32_t high_out = s - apply_1pole_lpf_fast(s, high_state, TREBLE_A_Q24);
    /* 原代码：high_out = (int32_t)(((int64_t)high_out * ds_high_gain_q24) >> 24); */
    high_out = qmul_fast(high_out, ds_high_gain_q24); /* 高频增益调节 */

    /* 混合三段均衡，并乘以总音量 */
    int64_t sum = (int64_t)low_out + (int64_t)mid_out + (int64_t)high_out;
    /* 原代码：int64_t y = sum; y = (y * ds_volume) >> 24; 现合并为一步 */
    int64_t y = sum * (int64_t)ds_volume;
    /* 四舍五入取整（符号感知的最近整数舍入） */
    y += (y >= 0) ? (1LL << 23) : -(1LL << 23);
    int32_t output = clamp24((int32_t)(y >> 24)); /* 钳位到 24 位范围并输出 */
    return output;
}

static inline void distortion_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_ds_channel(
            in_l[i],
            &ds_low_state_l,
            &ds_mid_lp_state_l,
            &ds_mid_hp_state_l,
            &ds_high_state_l,
            &ds_lpf_state_l,
            &ds_hpf_state_l);

        in_r[i] = in_l[i];
    }
}

static inline void distortion_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_ds_channel(
            in_l[i],
            &ds_low_state_l,
            &ds_mid_lp_state_l,
            &ds_mid_hp_state_l,
            &ds_high_state_l,
            &ds_lpf_state_l,
            &ds_hpf_state_l);

        in_r[i] = process_ds_channel(
            in_r[i],
            &ds_low_state_r,
            &ds_mid_lp_state_r,
            &ds_mid_hp_state_r,
            &ds_high_state_r,
            &ds_lpf_state_r,
            &ds_hpf_state_r);
    }
}

// ========================================
// 音频处理函数
// ========================================

/* 对一段音频数据块应用失真处理 */
static inline void distortion_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    if (STEREO)
    {
        distortion_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        distortion_process_block_mono(in_l, in_r, frames);
    }
}

#endif