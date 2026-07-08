#ifndef __DSP_FUZZ_H
#define __DSP_FUZZ_H

#include <stdint.h>

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

// ========================================
// 可调参数
// ========================================

/* ========== 模糊效果参数（Q8.24 定点格式） ========== */
/* 增益控制：Q8.24 定点数，1.0 = 0x01000000 */
static int32_t fz_gain;          /* 总增益，初始为 0.1 */
static int32_t fz_volume;        /* 最终输出音量，初始为 0.5 */
static int32_t fz_low_gain_q24;  /* 低频搁架增益，初始 1.0 */
static int32_t fz_mid_gain_q24;  /* 中频带通增益，初始 1.0 */
static int32_t fz_mid_a_q24;     /* 中频中心频率系数，由外部定义 MID_A_Q24 给出 */
static int32_t fz_high_gain_q24; /* 高频搁架增益，初始 1.0 */
static int32_t fz_asym_q24;      /* 不对称失真系数，~1.25，使负半周失真更强 */
static uint32_t fz_asym_inv_q11;

// ========================================
// 状态参数
// ========================================

/* 以下变量用于保存各滤波器的历史状态，实现一阶 IIR 滤波 */
/* 左声道低频/中频低通/中频高通/高频状态 */
static int32_t fz_low_state_l;
static int32_t fz_mid_lp_state_l;
static int32_t fz_mid_hp_state_l;
static int32_t fz_high_state_l;

/* 右声道对应状态 */
static int32_t fz_low_state_r;
static int32_t fz_mid_lp_state_r;
static int32_t fz_mid_hp_state_r;
static int32_t fz_high_state_r;

/* 左/右声道全局低通滤波器状态 */
static int32_t fz_lpf_state_l;
static int32_t fz_lpf_state_r;

/* 左/右声道全局高通滤波器状态 */
static int32_t fz_hpf_state_l;
static int32_t fz_hpf_state_r;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Fuzz_LoadDefaultParams(void)
{
    fz_gain = float_to_q24(0.5f);
    fz_volume = float_to_q24(6.0f);
    fz_low_gain_q24 = float_to_q24(1.0f);
    fz_mid_gain_q24 = float_to_q24(1.0f);
    fz_mid_a_q24 = MID_A_Q24;
    fz_high_gain_q24 = float_to_q24(1.0f);
    fz_asym_q24 = 0x01400000;
    fz_asym_inv_q11 = (uint32_t)((((uint64_t)Q24_ONE << 11) + ((uint32_t)fz_asym_q24 >> 1)) / (uint32_t)fz_asym_q24);
}

static inline void DSP_Fuzz_ClearState(void)
{
    fz_low_state_l = 0;
    fz_mid_lp_state_l = 0;
    fz_mid_hp_state_l = 0;
    fz_high_state_l = 0;

    fz_low_state_r = 0;
    fz_mid_lp_state_r = 0;
    fz_mid_hp_state_r = 0;
    fz_high_state_r = 0;

    fz_lpf_state_l = 0;
    fz_lpf_state_r = 0;
    fz_hpf_state_l = 0;
    fz_hpf_state_r = 0;
}

static inline void DSP_Fuzz_Init(void)
{
    DSP_Fuzz_LoadDefaultParams();
    DSP_Fuzz_ClearState();
}

typedef enum
{
    DSP_FUZZ_PARAM_GAIN = 0, /* 0.05x ~ 1.0x */
    DSP_FUZZ_PARAM_BASS,     /* 0.25x ~ 2.0x */
    DSP_FUZZ_PARAM_MID,      /* 0.25x ~ 3.0x */
    DSP_FUZZ_PARAM_MID_FREQ, /* 400 ~ 1000 Hz */
    DSP_FUZZ_PARAM_TREBLE,   /* 0.25x ~ 2.0x */
    DSP_FUZZ_PARAM_VOLUME,   /* 0.5x ~ 26.0x */
    DSP_FUZZ_PARAM_ASYM,     /* 0.75x ~ 2.0x */

    DSP_FUZZ_PARAM_NUM
} dsp_fuzz_param_t;

static inline void DSP_Fuzz_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_fuzz_param_t)param)
    {
        case DSP_FUZZ_PARAM_GAIN:
        {
            fz_gain = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.05f),
                float_to_q24(1.0f));
            break;
        }

        case DSP_FUZZ_PARAM_BASS:
        {
            fz_low_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_FUZZ_PARAM_MID:
        {
            fz_mid_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(3.0f));
            break;
        }

        case DSP_FUZZ_PARAM_MID_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 400u, 1000u);
            fz_mid_a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_FUZZ_PARAM_TREBLE:
        {
            fz_high_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.25f),
                float_to_q24(2.0f));
            break;
        }

        case DSP_FUZZ_PARAM_VOLUME:
        {
            fz_volume = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.5f),
                float_to_q24(26.0f));
            break;
        }

        case DSP_FUZZ_PARAM_ASYM:
        {
            fz_asym_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.75f),
                float_to_q24(2.0f));

            if (fz_asym_q24 <= 0)
                fz_asym_q24 = Q24_ONE;

            fz_asym_inv_q11 =
                (uint32_t)((((uint64_t)Q24_ONE << 11) +
                            ((uint32_t)fz_asym_q24 >> 1)) /
                           (uint32_t)fz_asym_q24);
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

/* ========== 优化的模糊削波函数 ========== */
/**
 * @brief 硬削波与软削波结合的失真处理，实现非对称的过载效果
 * @param x 输入样本（Q8.24 定点数）
 * @return 处理后的样本（Q8.24 定点数）
 * 该函数先对信号进行对称的硬限制（范围 ±0x300000，约 ±0.1875），
 * 然后使用平方函数产生软削波特性，并在负半周引入不对称失真，使音色更丰富。
 * 通过 fz_asym_q24 控制负半周的额外偏置量。
 */
static inline __attribute__((always_inline)) int32_t hard_clip(int32_t x)
{
    /* 硬限制：将信号钳位在 ±0x300000 内 */
    if (x > 0x300000)
        x = 0x300000;
    if (x < -0x300000)
        x = -0x300000;

    /* 快速计算 x 的平方，使用降低精度以提升速度 */
    int32_t xr = x >> 12; /* 缩小 2^12 倍，降低后续乘法位数 */
    int32_t x2 = xr * xr; /* 平方运算，结果未按 Q 格式对齐（特意如此） */

    if (x >= 0)
    {
        /* 正半周对称软削波：x 减去 x^2 的衰减项 */
        int32_t soft = x - (x2 >> 13); /* 原实现: (x - (x2 >> 13)) * 8 */
        return soft << 3;
    }
    else
    {
        /* 负半周非对称软削波：通过除以 fz_asym_q24 引入额外偏置 */
        /* 带符号的四舍五入除法：(num / fz_asym_q24) ，其中 num = x^2 * 2^24 */
        int64_t num = ((int64_t)x2 << 24); /* 将平方值提升到 Q8.24 的比例 */
        int64_t den = (int64_t)fz_asym_q24;
        if (num >= 0)
        {
            num += (den >> 1); /* 正数加 0.5 除数实现四舍五入 */
        }
        else
        {
            num -= (den >> 1);                   /* 负数减 0.5 除数 */
        }
        int32_t bias_q24 = (int32_t)(num / den); /* 得到 Q8.24 格式的偏置量 */
        int32_t bias = bias_q24 >> 13;           /* 匹配原始缩放比例 */
        return (x + bias) * 8;                   /* 最终输出 */

        // uint32_t bias = (((uint32_t)x2 * (uint32_t)fz_asym_inv_q11) + (1u << 23)) >> 24;
        // return (x + (int32_t)bias) << 3;
    }
}

/* ========== 单通道模糊处理流水线 ========== */
/**
 * @brief 对单个音频样本进行完整的模糊效果处理
 * @param s               输入样本（Q8.24）
 * @param low_state       低频搁架滤波器状态指针
 * @param mid_lp_state    中频带通低通部分状态指针
 * @param mid_hp_state    中频带通高通部分状态指针
 * @param high_state      高频搁架滤波器状态指针
 * @param lpf_state       全局低通滤波器状态指针
 * @param hpf_state       全局高通滤波器状态指针
 * @return 处理后的样本（Q8.24 已限制在有效范围）
 * 处理流程：增益 -> 高通滤波 -> 削波 -> 低通滤波 -> 分段均衡 -> 混音 -> 总音量
 */
static inline __attribute__((always_inline)) int32_t process_fz_channel(
    int32_t s,
    int32_t *low_state,
    int32_t *mid_lp_state,
    int32_t *mid_hp_state,
    int32_t *high_state,
    int32_t *lpf_state,
    int32_t *hpf_state)
{
    /* 第一步：整体增益控制 */
    s = qmul_fast(s, fz_gain); /* Q8.24 定点乘法，实现增益调整 */

    /* 第二步：削波前的高通滤波，去除低频隆隆声（使用全局 HPF 系数） */
    s = apply_1pole_hpf_fast(s, hpf_state, HPF_A_Q24);

    /* 第三步：硬/软削波处理，产生泛音丰富的失真 */
    s = hard_clip(s);

    /* 第四步：削波后的低通滤波，减少刺耳的高频嘶嘶声（使用全局 LPF 系数） */
    s = apply_1pole_lpf_fast(s, lpf_state, LPF_A_Q24);

    /* 第五步：低频搁架滤波器，提取并增益低频分量 */
    int32_t low_out = apply_1pole_lpf_fast(s, low_state, BASS_A_Q24);
    low_out = qmul_fast(low_out, fz_low_gain_q24); /* 应用低频搁架增益 */

    /* 第六步：中频带通滤波，通过高通+低通级联实现带通 */
    int32_t mid_band = apply_1pole_lpf_fast(
        apply_1pole_hpf_fast(s, mid_hp_state, fz_mid_a_q24),
        mid_lp_state, fz_mid_a_q24);
    int32_t mid_out = qmul_fast(mid_band, fz_mid_gain_q24); /* 应用中频增益 */

    /* 第七步：高频搁架滤波器，用原始信号减去低通滤波结果得到高频分量 */
    int32_t high_out = s - apply_1pole_lpf_fast(s, high_state, TREBLE_A_Q24);
    high_out = qmul_fast(high_out, fz_high_gain_q24); /* 应用高频增益 */

    /* 第八步：将低频、中频、高频分量混合，并施加总音量，同时进行四舍五入和限幅 */
    int64_t y = (int64_t)low_out + (int64_t)mid_out + (int64_t)high_out; /* 64 位累加避免溢出 */
    y = y * (int64_t)fz_volume;                                          /* 总音量缩放 */
    y += (y >= 0) ? (1LL << 23) : -(1LL << 23);                          /* 舍入到最近整数 (0.5 LSB) */
    int32_t output = clamp24((int32_t)(y >> 24));                        /* 丢弃小数部分并限幅到 24 位范围 */
    return output;
}

/**
 * @brief 处理单声道样本
 * @param in_l 左声道样本指针（输入/输出）
 * @param in_r 右声道样本指针（输入/输出）
 * @param frames 样本数
 */
static inline void fuzz_process_block_mono(int32_t *in_l, int32_t *in_r, size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_fz_channel(
            in_l[i],
            &fz_low_state_l,
            &fz_mid_lp_state_l,
            &fz_mid_hp_state_l,
            &fz_high_state_l,
            &fz_lpf_state_l,
            &fz_hpf_state_l);

        in_r[i] = in_l[i];
    }
}

/**
 * @brief 处理双声道样本
 * @param in_l 左声道样本指针（输入/输出）
 * @param in_r 右声道样本指针（输入/输出）
 * @param frames 样本数
 */
static inline void fuzz_process_block_stereo(int32_t *in_l, int32_t *in_r, size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_fz_channel(
            in_l[i],
            &fz_low_state_l,
            &fz_mid_lp_state_l,
            &fz_mid_hp_state_l,
            &fz_high_state_l,
            &fz_lpf_state_l,
            &fz_hpf_state_l);

        in_r[i] = process_fz_channel(
            in_r[i],
            &fz_low_state_r,
            &fz_mid_lp_state_r,
            &fz_mid_hp_state_r,
            &fz_high_state_r,
            &fz_lpf_state_r,
            &fz_hpf_state_r);
    }
}

// ========================================
// 音频处理函数
// ========================================

/**
 * @brief 对一整块音频数据执行模糊效果处理
 * @param in_l   左声道样本数组（输入兼输出）
 * @param in_r   右声道样本数组（输入兼输出）
 * @param frames 帧数（每帧为一个立体声或单声道采样对）
 * @param stereo 立体声标志，true 表示独立处理双声道，false 表示单声道
 */
static inline void fuzz_process_block(int32_t *in_l, int32_t *in_r, size_t frames)
{
    if (STEREO)
    {
        fuzz_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        fuzz_process_block_mono(in_l, in_r, frames);
    }
}

#endif