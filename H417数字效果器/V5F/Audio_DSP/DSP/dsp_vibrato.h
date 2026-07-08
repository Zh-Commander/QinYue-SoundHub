#ifndef __DSP_VIBRATO_H
#define __DSP_VIBRATO_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define VIBRATO_MAX_SAMPLES 512
#define VIBRATO_MIN_DELAY_SAMPLES 8
#define VIBRATO_GUARD 6

// ========================================
// 可调参数
// ========================================

static uint32_t vib_depth_q16;
static uint32_t vib_lfo_inc;
static int32_t vib_volume_q24;
static uint32_t vib_delay_smooth_coef_q16;

// ========================================
// 状态参数
// ========================================
static int32_t vib_buffer_l[VIBRATO_MAX_SAMPLES];
static int32_t vib_buffer_r[VIBRATO_MAX_SAMPLES];

static uint32_t vib_write_pos;


static uint32_t vib_lfo_phase_l;
static uint32_t vib_lfo_phase_r;

static int32_t vib_delay_q16_state_l;
static int32_t vib_delay_q16_state_r;

static int32_t vib_thiran_y_prev_l;
static int32_t vib_thiran_y_prev_r;

// ========================================
// 状态操作函数
// ========================================
// 可调参数初始化
static inline void DSP_Vibrato_LoadDefaultParams(void)
{
    vib_depth_q16 = Q16_ONE / 3;  // 不要太深
    vib_lfo_inc = 180000u;        // LFO 速度，约 1Hz 左右量级
    vib_volume_q24 = Q24_ONE;

    vib_delay_smooth_coef_q16 = 0xF000;
}

// 状态参数初始化
static inline void DSP_Vibrato_ClearState(void)
{
    memset(vib_buffer_l, 0, sizeof(vib_buffer_l));
    memset(vib_buffer_r, 0, sizeof(vib_buffer_r));

    vib_write_pos = 0;

    vib_lfo_phase_l = 0;
    vib_lfo_phase_r = 0x80000000u;

    vib_delay_q16_state_l = VIBRATO_MIN_DELAY_SAMPLES << 16;
    vib_delay_q16_state_r = VIBRATO_MIN_DELAY_SAMPLES << 16;

    vib_thiran_y_prev_l = 0;
    vib_thiran_y_prev_r = 0;
}

static inline void DSP_Vibrato_Init(void)
{
    DSP_Vibrato_LoadDefaultParams();
    DSP_Vibrato_ClearState();
}

typedef enum
{
    DSP_VIBRATO_PARAM_SPEED = 0, /* 0.05 ~ 8 Hz */
    DSP_VIBRATO_PARAM_DEPTH,     /* 0 ~ 75%, 可改成 Q16_ONE */
    DSP_VIBRATO_PARAM_VOLUME,    /* 0.1x ~ 3.0x */
    DSP_VIBRATO_PARAM_SMOOTH,    /* 0 ~ 0xFFF0 */

    DSP_VIBRATO_PARAM_NUM
} dsp_vibrato_param_t;

static inline void DSP_Vibrato_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_vibrato_param_t)param)
    {
        case DSP_VIBRATO_PARAM_SPEED:
        {
            vib_lfo_inc = DSP_Param_PotToLfoInc(value, 0.05f, 8.0f);
            break;
        }

        case DSP_VIBRATO_PARAM_DEPTH:
        {
            vib_depth_q16 = DSP_Param_PotToQ16(
                value,
                0,
                (Q16_ONE * 75u) / 100u);
            break;
        }

        case DSP_VIBRATO_PARAM_VOLUME:
        {
            vib_volume_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.1f),
                float_to_q24(3.0f));
            break;
        }

        case DSP_VIBRATO_PARAM_SMOOTH:
        {
            vib_delay_smooth_coef_q16 = DSP_Param_PotToQ16(value, 0, 0xFFF0u);
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

static inline __attribute__((always_inline)) int32_t vibrato_read_thiran_q16(
    const int32_t *buf,
    uint32_t base_idx,
    uint32_t frac_q16,
    int32_t *y_prev)
{
    uint32_t i0 = base_idx;
    uint32_t i1 = (base_idx + 1) % VIBRATO_MAX_SAMPLES;

    int32_t x0 = buf[i0];
    int32_t x1 = buf[i1];

    uint32_t one = Q16_ONE;
    uint32_t num = (frac_q16 >= one) ? 0 : (one - frac_q16);
    uint32_t den = one + frac_q16;
    uint32_t a_q16 = (uint32_t)(((uint64_t)num << 16) / (uint64_t)den);

    int64_t acc = x0;
    acc += ((int64_t)a_q16 * x1) >> 16;
    acc -= ((int64_t)a_q16 * (*y_prev)) >> 16;

    int32_t y = clamp32(acc);
    *y_prev = y;

    return y;
}

static inline int32_t vibrato_calc_smoothed_delay_q16(
    uint32_t phase,
    int32_t *delay_state_q16)
{
    const uint32_t max_depth =
        VIBRATO_MAX_SAMPLES - VIBRATO_MIN_DELAY_SAMPLES - VIBRATO_GUARD;

    uint32_t lfo_q16 = lfo_q16_shape(phase, LFO_TRIANGLE_SMOOTH);

    uint64_t scaled_q32 = (uint64_t)lfo_q16 * (uint64_t)vib_depth_q16;

    uint32_t offset_q16 =
        (uint32_t)((((uint64_t)max_depth << 16) * scaled_q32) >> 32);

    uint32_t target_delay_q16 =
        (VIBRATO_MIN_DELAY_SAMPLES << 16) + offset_q16;

    int64_t y = ((int64_t)(Q16_ONE - vib_delay_smooth_coef_q16) * target_delay_q16 +
                 (int64_t)vib_delay_smooth_coef_q16 * (*delay_state_q16)) >>
                16;

    int32_t delay_q16 = clamp32(y);

    const int32_t min_q16 = VIBRATO_MIN_DELAY_SAMPLES << 16;
    const int32_t max_q16 =
        ((VIBRATO_MIN_DELAY_SAMPLES +
          (VIBRATO_MAX_SAMPLES - VIBRATO_MIN_DELAY_SAMPLES - VIBRATO_GUARD) - 1)
         << 16) |
        0xFFFF;

    if (delay_q16 < min_q16)
        delay_q16 = min_q16;
    if (delay_q16 > max_q16)
        delay_q16 = max_q16;

    *delay_state_q16 = delay_q16;

    return delay_q16;
}

static inline __attribute__((always_inline)) int32_t vibrato_read_tap(
    const int32_t *buf,
    uint32_t phase,
    int32_t *delay_state_q16,
    int32_t *thiran_y_prev)
{
    int32_t delay_q16 = vibrato_calc_smoothed_delay_q16(
        phase,
        delay_state_q16);

    uint32_t int_delay = (uint32_t)delay_q16 >> 16;
    uint32_t frac_q16 = (uint32_t)delay_q16 & 0xFFFF;

    uint32_t base =
        (vib_write_pos + VIBRATO_MAX_SAMPLES - int_delay - 1) %
        VIBRATO_MAX_SAMPLES;

    return vibrato_read_thiran_q16(
        buf,
        base,
        frac_q16,
        thiran_y_prev);
}

static inline __attribute__((always_inline)) int32_t vibrato_apply_volume(
    int32_t x)
{
    int64_t y = ((int64_t)x * vib_volume_q24) >> 24;

    return clamp24(clamp32(y));
}

static inline void vibrato_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        vib_lfo_phase_l += vib_lfo_inc;

        int32_t tap_l = vibrato_read_tap(
            vib_buffer_l,
            vib_lfo_phase_l,
            &vib_delay_q16_state_l,
            &vib_thiran_y_prev_l);

        vib_buffer_l[vib_write_pos] = in_l[i];
        vib_buffer_r[vib_write_pos] = in_l[i];

        in_l[i] = vibrato_apply_volume(tap_l);
        in_r[i] = in_l[i];

        vib_write_pos++;
        if (vib_write_pos >= VIBRATO_MAX_SAMPLES)
            vib_write_pos = 0;
    }
}

static inline void vibrato_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        vib_lfo_phase_l += vib_lfo_inc;
        vib_lfo_phase_r = vib_lfo_phase_l + 0x80000000u;

        int32_t tap_l = vibrato_read_tap(
            vib_buffer_l,
            vib_lfo_phase_l,
            &vib_delay_q16_state_l,
            &vib_thiran_y_prev_l);

        int32_t tap_r = vibrato_read_tap(
            vib_buffer_r,
            vib_lfo_phase_r,
            &vib_delay_q16_state_r,
            &vib_thiran_y_prev_r);

        vib_buffer_l[vib_write_pos] = in_l[i];
        vib_buffer_r[vib_write_pos] = in_r[i];

        in_l[i] = vibrato_apply_volume(tap_l);
        in_r[i] = vibrato_apply_volume(tap_r);

        vib_write_pos++;
        if (vib_write_pos >= VIBRATO_MAX_SAMPLES)
            vib_write_pos = 0;
    }
}

// ========================================
// 音频处理函数
// ========================================
/* 以块为单位处理振音效果
 * in_l, in_r: 左右声道样本数组（输入和输出共用）
 * frames: 块中的采样数
 * mode: 效果模式 */
static inline void vibrato_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    if (STEREO)
    {
        vibrato_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        vibrato_process_block_mono(in_l, in_r, frames);
    }
}

#endif