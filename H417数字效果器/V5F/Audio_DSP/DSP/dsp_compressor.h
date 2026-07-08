#ifndef __DSP_COMPRESSOR_H
#define __DSP_COMPRESSOR_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

#define SOFT_KNEE_WIDTH_Q24 0x0019999A

// ========================================
// 可调参数
// ========================================

static int32_t comp_threshold_q24;
static int32_t comp_inv_ratio_q24;
static int32_t comp_gain_q24;
static int32_t comp_attack_a_q24;
static int32_t comp_release_a_q24;

// ========================================
// 状态参数
// ========================================

static int32_t env_l_q24;
static int32_t env_r_q24;

static int32_t gain_l_q24;
static int32_t gain_r_q24;
static uint8_t comp_gain_update_counter;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Compressor_LoadDefaultParams(void)
{
    comp_threshold_q24 = float_to_q24(0.01f);
    comp_inv_ratio_q24 = float_to_q24(1.0f / 50.0f);
    comp_gain_q24 = float_to_q24(12.0f);

    comp_attack_a_q24 = ms_to_coeff_q24(0.5f, SAMPLE_RATE);
    comp_release_a_q24 = ms_to_coeff_q24(20.0f, SAMPLE_RATE);
}

static inline void DSP_Compressor_ClearState(void)
{
    env_l_q24 = 0;
    env_r_q24 = 0;

    gain_l_q24 = Q24_ONE;
    gain_r_q24 = Q24_ONE;
    comp_gain_update_counter = 0;
}

static inline void DSP_Compressor_Init(void)
{
    DSP_Compressor_LoadDefaultParams();
    DSP_Compressor_ClearState();
}

typedef enum
{
    DSP_COMP_PARAM_THRESHOLD = 0, /* -40 dB ~ 0 dB */
    DSP_COMP_PARAM_RATIO,         /* 1.1:1 ~ 50:1 */
    DSP_COMP_PARAM_ATTACK,        /* 0.5 ~ 100 ms */
    DSP_COMP_PARAM_RELEASE,       /* 20 ~ 500 ms */
    DSP_COMP_PARAM_MAKEUP_GAIN,   /* 0 ~ +24 dB */

    DSP_COMP_PARAM_NUM
} dsp_compressor_param_t;

static inline void DSP_Compressor_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_compressor_param_t)param)
    {
        case DSP_COMP_PARAM_THRESHOLD:
        {
            float db = DSP_Param_PotToFloat(value, -40.0f, 0.0f);
            comp_threshold_q24 = db_to_q24(db);
            break;
        }

        case DSP_COMP_PARAM_RATIO:
        {
            float ratio = DSP_Param_PotToFloat(value, 1.1f, 50.0f);
            comp_inv_ratio_q24 = float_to_q24(1.0f / ratio);
            break;
        }

        case DSP_COMP_PARAM_ATTACK:
        {
            float attack_ms = DSP_Param_PotToFloat(value, 0.5f, 100.0f);
            comp_attack_a_q24 = ms_to_coeff_q24(attack_ms, SAMPLE_RATE);
            break;
        }

        case DSP_COMP_PARAM_RELEASE:
        {
            float release_ms = DSP_Param_PotToFloat(value, 20.0f, 500.0f);
            comp_release_a_q24 = ms_to_coeff_q24(release_ms, SAMPLE_RATE);
            break;
        }

        case DSP_COMP_PARAM_MAKEUP_GAIN:
        {
            float db = DSP_Param_PotToFloat(value, 0.0f, 24.0f);
            comp_gain_q24 = db_to_q24(db);
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

static inline int32_t compute_gain_q24(int32_t env_q24)
{
    if (env_q24 <= 0 || comp_inv_ratio_q24 >= Q24_ONE)
        return Q24_ONE;

    const int32_t knee_half = SOFT_KNEE_WIDTH_Q24 >> 1;
    const int32_t knee_start = comp_threshold_q24 - knee_half;
    const int32_t knee_end = comp_threshold_q24 + knee_half;

    if (env_q24 <= knee_start)
        return Q24_ONE;

    int32_t ratio_delta = Q24_ONE - comp_inv_ratio_q24;
    int32_t over_thresh = env_q24 - comp_threshold_q24;
    int32_t frac = qdiv(over_thresh, env_q24);

    if (frac < 0)
        frac = 0;
    if (frac > Q24_ONE)
        frac = Q24_ONE;

    int32_t gain_end = Q24_ONE - qmul_fast(frac, ratio_delta);

    if (env_q24 >= knee_end)
        return gain_end;

    uint32_t t_q16 = (uint32_t)(((int64_t)(env_q24 - knee_start) << 16) / SOFT_KNEE_WIDTH_Q24);
    return lerp_fixed(Q24_ONE, gain_end, t_q16);
}

static inline __attribute__((always_inline)) int32_t process_compressor_channel(
    int32_t s,
    int32_t *env_q24,
    int32_t gain_q24)
{
    int32_t abs_s = (s < 0) ? -s : s;

    /* I2S sample is 24-bit left-aligned, while compressor thresholds are Q0.24. */
    int32_t env_in_q24 = abs_s >> COMP_ENV_SHIFT;

    if (env_in_q24 > *env_q24)
    {
        *env_q24 = qmul_fast(*env_q24, comp_attack_a_q24) +
                   qmul_fast(env_in_q24, Q24_ONE - comp_attack_a_q24);
    }
    else
    {
        *env_q24 = qmul_fast(*env_q24, comp_release_a_q24) +
                   qmul_fast(env_in_q24, Q24_ONE - comp_release_a_q24);
    }

    int32_t total_gain_q24 = qmul_fast(gain_q24, comp_gain_q24);
    int64_t y = ((int64_t)s * (int64_t)total_gain_q24) >> 24;

    return clamp24(clamp32(y));
}

static inline void compressor_update_gain_stereo(void)
{
    if (++comp_gain_update_counter >= 4)
    {
        comp_gain_update_counter = 0;
        gain_l_q24 = compute_gain_q24(env_l_q24);
        gain_r_q24 = compute_gain_q24(env_r_q24);
    }
}

static inline void compressor_update_gain_mono(void)
{
    if (++comp_gain_update_counter >= 4)
    {
        comp_gain_update_counter = 0;
        gain_l_q24 = compute_gain_q24(env_l_q24);
        gain_r_q24 = gain_l_q24;
    }
}

static inline void compressor_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_compressor_channel(
            in_l[i],
            &env_l_q24,
            gain_l_q24);

        in_r[i] = process_compressor_channel(
            in_r[i],
            &env_r_q24,
            gain_r_q24);

        compressor_update_gain_stereo();
    }
}

static inline void compressor_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_compressor_channel(
            in_l[i],
            &env_l_q24,
            gain_l_q24);

        in_r[i] = in_l[i];

        compressor_update_gain_mono();
    }
}

// ========================================
// 音频处理函数
// ========================================

static inline void compressor_process_block(int32_t *in_l, int32_t *in_r, size_t frames)
{
    if (STEREO)
    {
        compressor_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        compressor_process_block_mono(in_l, in_r, frames);
    }
}

#endif
