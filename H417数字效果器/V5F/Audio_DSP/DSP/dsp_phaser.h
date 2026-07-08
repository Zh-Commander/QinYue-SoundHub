#ifndef __DSP_PHASER_H
#define __DSP_PHASER_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

#define PHASER_STAGE_NUM 4

typedef struct
{
    int32_t z1;
} phaser_allpass_state_t;

// ========================================
// 可调参数
// ========================================

static uint32_t ph_lfo_inc;

static int32_t ph_low_a_q24;
static int32_t ph_high_a_q24;

static int32_t ph_depth_q24;  // 没用上
static int32_t ph_feedback_q24;
static int32_t ph_mix_q24;
static int32_t ph_volume_q24;

// ========================================
// 状态参数
// ========================================

static uint32_t ph_lfo_phase_l;
static uint32_t ph_lfo_phase_r;

static phaser_allpass_state_t ph_left[PHASER_STAGE_NUM];
static phaser_allpass_state_t ph_right[PHASER_STAGE_NUM];

static int32_t ph_feedback_l;
static int32_t ph_feedback_r;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Phaser_LoadDefaultParams(void)
{
    ph_lfo_inc = 250000u;                          // LFO 速度，明显但不太快

    ph_low_a_q24 = fc_to_q24(150, SAMPLE_RATE);    // 扫频下限
    ph_high_a_q24 = fc_to_q24(5000, SAMPLE_RATE);  // 扫频上限

    ph_depth_q24 = Q24_ONE;
    ph_feedback_q24 = float_to_q24(0.70f);  // 增强 phaser 感
    ph_mix_q24 = float_to_q24(0.75f);
    ;                                       // dry/wet
    ph_volume_q24 = Q24_ONE;                // 音量
}

static inline void DSP_Phaser_ClearState(void)
{
    for (uint8_t i = 0; i < PHASER_STAGE_NUM; i++)
    {
        ph_left[i].z1 = 0;
        ph_right[i].z1 = 0;
    }

    ph_feedback_l = 0;
    ph_feedback_r = 0;

    ph_lfo_phase_l = 0;
    ph_lfo_phase_r = 0x80000000u;
}

static inline void DSP_Phaser_Init(void)
{
    DSP_Phaser_LoadDefaultParams();
    DSP_Phaser_ClearState();
}

typedef enum
{
    DSP_PHASER_PARAM_SPEED = 0, /* 0.05 ~ 4 Hz */
    DSP_PHASER_PARAM_LOW_FREQ,  /* 100 ~ 1000 Hz */
    DSP_PHASER_PARAM_HIGH_FREQ, /* 1500 ~ 6000 Hz */
    DSP_PHASER_PARAM_FEEDBACK,  /* 0 ~ 95%, square curve */
    DSP_PHASER_PARAM_MIX,       /* 0 ~ 100% */
    DSP_PHASER_PARAM_VOLUME,    /* 0.1x ~ 4.0x */

    DSP_PHASER_PARAM_NUM
} dsp_phaser_param_t;

static inline void DSP_Phaser_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_phaser_param_t)param)
    {
        case DSP_PHASER_PARAM_SPEED:
        {
            ph_lfo_inc = DSP_Param_PotToLfoInc(value, 0.05f, 4.0f);
            break;
        }

        case DSP_PHASER_PARAM_LOW_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 100u, 1000u);
            ph_low_a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_PHASER_PARAM_HIGH_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 1500u, 6000u);
            ph_high_a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_PHASER_PARAM_FEEDBACK:
        {
            int32_t norm_q24 = DSP_Param_PotToQ24(value, 0, Q24_ONE);
            int32_t norm_sq_q24 = qmul_fast(norm_q24, norm_q24);
            ph_feedback_q24 = qmul_fast(norm_sq_q24, float_to_q24(0.95f));
            break;
        }

        case DSP_PHASER_PARAM_MIX:
        {
            ph_mix_q24 = DSP_Param_PotToQ24(value, 0, Q24_ONE);
            break;
        }

        case DSP_PHASER_PARAM_VOLUME:
        {
            ph_volume_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.1f),
                float_to_q24(4.0f));
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================
static inline __attribute__((always_inline)) int32_t phaser_allpass_process(
    int32_t x,
    int32_t a_q24,
    phaser_allpass_state_t *state)
{
    int32_t diff = x - state->z1;
    int32_t temp = state->z1 + qmul_fast(diff, a_q24);
    state->z1 = temp;
    return temp;
}

static inline int32_t phaser_lfo_coef(uint32_t phase)
{
    int32_t tri_q24 = (int32_t)(lfo_q16_shape(phase, LFO_TRIANGLE_SMOOTH) << 8);

    int64_t sweep = (int64_t)ph_low_a_q24 * (Q24_ONE - tri_q24) +
                    (int64_t)ph_high_a_q24 * tri_q24;

    return (int32_t)(sweep >> 24);
}

static inline void phaser_update_lfo(void)
{
    ph_lfo_phase_l += ph_lfo_inc;
    ph_lfo_phase_r += ph_lfo_inc;
}

static inline __attribute__((always_inline)) int32_t process_phaser_channel(
    int32_t s,
    int32_t coef_q24,
    phaser_allpass_state_t *stages,
    int32_t *feedback_state)
{
    int32_t in = s >> 1; /* -6 dB headroom */

    int32_t x = in - *feedback_state;

    for (uint8_t i = 0; i < PHASER_STAGE_NUM; i++)
    {
        x = phaser_allpass_process(x, coef_q24, &stages[i]);
    }

    *feedback_state = qmul_fast(x, ph_feedback_q24);

    int64_t dry = ((int64_t)s * (Q24_ONE - ph_mix_q24)) >> 24;
    int64_t wet = ((int64_t)x * ph_mix_q24) >> 24;

    int64_t y = dry + wet;
    y = (y * ph_volume_q24) >> 24;

    return clamp24(clamp32(y));
}

static inline void phaser_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        phaser_update_lfo();

        int32_t coef = phaser_lfo_coef(ph_lfo_phase_l);

        in_l[i] = process_phaser_channel(
            in_l[i],
            coef,
            ph_left,
            &ph_feedback_l);

        in_r[i] = in_l[i];
    }
}

static inline void phaser_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        phaser_update_lfo();

        int32_t coef_l = phaser_lfo_coef(ph_lfo_phase_l);
        int32_t coef_r = phaser_lfo_coef(ph_lfo_phase_r);

        in_l[i] = process_phaser_channel(
            in_l[i],
            coef_l,
            ph_left,
            &ph_feedback_l);

        in_r[i] = process_phaser_channel(
            in_r[i],
            coef_r,
            ph_right,
            &ph_feedback_r);
    }
}

// ========================================
// 音频处理函数
// ========================================

static inline void phaser_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    if (STEREO)
    {
        phaser_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        phaser_process_block_mono(in_l, in_r, frames);
    }
}

#endif