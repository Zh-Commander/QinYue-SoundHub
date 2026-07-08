#ifndef __DSP_CHORUS_H
#define __DSP_CHORUS_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

#define CHORUS_MAX_SAMPLES 512
#define CHORUS_MIN_DELAY_SAMPLES 16
#define CHORUS_GUARD 6

// ========================================
// 可调参数
// ========================================

static uint32_t ch_depth_q16;
static uint32_t ch_mix_q16;
static int32_t ch_volume_q24;

static uint32_t ch_lfo_inc;

static uint32_t ch_lpf_coef_q16;

static uint32_t ch_delay_smooth_coef_q16;

// ========================================
// 状态参数
// ========================================

static int32_t ch_buf_l[CHORUS_MAX_SAMPLES];
static int32_t ch_buf_r[CHORUS_MAX_SAMPLES];
static int32_t ch_buf_c[CHORUS_MAX_SAMPLES];

static uint32_t ch_write_pos;

static uint32_t ch_lfo_phase_l;
static uint32_t ch_lfo_phase_r;
static uint32_t ch_lfo_phase_c;

static int32_t ch_lpf_state_l;
static int32_t ch_lpf_state_r;

static int32_t ch_delay_q16_state_l;
static int32_t ch_delay_q16_state_r;
static int32_t ch_delay_q16_state_c;

static int32_t ch_thiran_y_prev_l;
static int32_t ch_thiran_y_prev_r;
static int32_t ch_thiran_y_prev_c;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Chorus_LoadDefaultParams(void)
{
    ch_depth_q16 = Q16_ONE / 2;
    ch_mix_q16 = Q16_ONE / 2;
    ch_volume_q24 = Q24_ONE;

    ch_lfo_inc = 70000u;
    ch_lpf_coef_q16 = 0x4000;
    ch_delay_smooth_coef_q16 = 0xF000;
}

static inline void DSP_Chorus_ClearState(void)
{
    for (uint16_t i = 0; i < CHORUS_MAX_SAMPLES; i++)
    {
        ch_buf_l[i] = 0;
        ch_buf_r[i] = 0;
        ch_buf_c[i] = 0;
    }

    ch_write_pos = 0;

    ch_lfo_phase_l = 0;
    ch_lfo_phase_r = 0x80000000u;
    ch_lfo_phase_c = 0x55555555u;

    ch_lpf_state_l = 0;
    ch_lpf_state_r = 0;

    ch_delay_q16_state_l = CHORUS_MIN_DELAY_SAMPLES << 16;
    ch_delay_q16_state_r = CHORUS_MIN_DELAY_SAMPLES << 16;
    ch_delay_q16_state_c = CHORUS_MIN_DELAY_SAMPLES << 16;

    ch_thiran_y_prev_l = 0;
    ch_thiran_y_prev_r = 0;
    ch_thiran_y_prev_c = 0;
}

static inline void DSP_Chorus_Init(void)
{
    DSP_Chorus_LoadDefaultParams();
    DSP_Chorus_ClearState();
}

  typedef enum
  {
      DSP_CHORUS_PARAM_SPEED = 0,      /* 0.05 ~ 5 Hz */
      DSP_CHORUS_PARAM_DEPTH,          /* 0 ~ 100% */
      DSP_CHORUS_PARAM_MIX,            /* 0 ~ 100% */
      DSP_CHORUS_PARAM_TONE,           /* 100 Hz ~ 8 kHz */
      DSP_CHORUS_PARAM_VOLUME,         /* 0.1x ~ 3.0x */
      DSP_CHORUS_PARAM_SMOOTH,         /* 0 ~ 0xFFF0 */

      DSP_CHORUS_PARAM_NUM
  } dsp_chorus_param_t;

    static inline void DSP_Chorus_UpdateParamFromPot(uint8_t param, int32_t value)
  {
      value = DSP_Param_ClampPot(value);

      switch ((dsp_chorus_param_t)param)
      {
          case DSP_CHORUS_PARAM_SPEED:
          {
              ch_lfo_inc = DSP_Param_PotToLfoInc(value, 0.05f, 5.0f);
              break;
          }

          case DSP_CHORUS_PARAM_DEPTH:
          {
              ch_depth_q16 = DSP_Param_PotToQ16(value, 0, Q16_ONE);
              break;
          }

          case DSP_CHORUS_PARAM_MIX:
          {
              ch_mix_q16 = DSP_Param_PotToQ16(value, 0, Q16_ONE);
              break;
          }

          case DSP_CHORUS_PARAM_TONE:
          {
              float fc = DSP_Param_PotToLogFloat(value, 100.0f, 8000.0f);
              ch_lpf_coef_q16 = DSP_Param_OnePoleDecayQ16FromHz(fc);
              break;
          }

          case DSP_CHORUS_PARAM_VOLUME:
          {
              ch_volume_q24 = DSP_Param_PotToQ24(
                  value,
                  float_to_q24(0.1f),
                  float_to_q24(3.0f));
              break;
          }

          case DSP_CHORUS_PARAM_SMOOTH:
          {
              ch_delay_smooth_coef_q16 = DSP_Param_PotToQ16(value, 0, 0xFFF0u);
              break;
          }

          default:
              break;
      }
  }

// ========================================
// 音频处理辅助函数
// ========================================

static inline __attribute__((always_inline)) int32_t chorus_process_lpf_q16(
    int32_t x,
    int32_t *state,
    uint32_t coef_q16)
{
    int64_t y = ((int64_t)(Q16_ONE - coef_q16) * x +
                 (int64_t)coef_q16 * (*state)) >>
                16;

    *state = clamp32(y);
    return *state;
}

static inline __attribute__((always_inline)) int32_t chorus_read_thiran_q16(
    const int32_t *buf,
    uint32_t base_idx,
    uint32_t frac_q16,
    int32_t *y_prev)
{
    uint32_t i0 = base_idx;
    uint32_t i1 = (base_idx + 1) % CHORUS_MAX_SAMPLES;

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

static inline int32_t chorus_calc_smoothed_delay_q16(
    uint32_t phase,
    int32_t *delay_state_q16)
{
    const uint32_t max_depth =
        CHORUS_MAX_SAMPLES - CHORUS_MIN_DELAY_SAMPLES - CHORUS_GUARD;

    uint32_t lfo_q16 = lfo_q16_shape(phase, LFO_TRIANGLE);

    uint64_t scaled_q32 = (uint64_t)lfo_q16 * (uint64_t)ch_depth_q16;

    uint32_t offset_q16 =
        (uint32_t)((((uint64_t)max_depth << 16) * scaled_q32) >> 32);

    uint32_t target_delay_q16 =
        (CHORUS_MIN_DELAY_SAMPLES << 16) + offset_q16;

    int64_t y = ((int64_t)(Q16_ONE - ch_delay_smooth_coef_q16) * target_delay_q16 +
                 (int64_t)ch_delay_smooth_coef_q16 * (*delay_state_q16)) >>
                16;

    int32_t delay_q16 = clamp32(y);

    const int32_t min_q16 = CHORUS_MIN_DELAY_SAMPLES << 16;
    const int32_t max_q16 =
        ((CHORUS_MIN_DELAY_SAMPLES +
          (CHORUS_MAX_SAMPLES - CHORUS_MIN_DELAY_SAMPLES - CHORUS_GUARD) - 1)
         << 16) |
        0xFFFF;

    if (delay_q16 < min_q16)
        delay_q16 = min_q16;
    if (delay_q16 > max_q16)
        delay_q16 = max_q16;

    *delay_state_q16 = delay_q16;

    return delay_q16;
}

static inline __attribute__((always_inline)) int32_t chorus_read_tap(
    const int32_t *buf,
    uint32_t phase,
    int32_t *delay_state_q16,
    int32_t *thiran_y_prev)
{
    int32_t delay_q16 = chorus_calc_smoothed_delay_q16(
        phase,
        delay_state_q16);

    uint32_t int_delay = (uint32_t)delay_q16 >> 16;
    uint32_t frac_q16 = (uint32_t)delay_q16 & 0xFFFF;

    uint32_t base =
        (ch_write_pos + CHORUS_MAX_SAMPLES - int_delay - 1) % CHORUS_MAX_SAMPLES;

    return chorus_read_thiran_q16(
        buf,
        base,
        frac_q16,
        thiran_y_prev);
}

static inline __attribute__((always_inline)) int32_t chorus_mix_output(
    int32_t dry,
    int32_t wet)
{
    int64_t mix = ((int64_t)dry * (Q16_ONE - ch_mix_q16) +
                   (int64_t)wet * ch_mix_q16) >>
                  16;

    mix = (mix * ch_volume_q24) >> 24;

    return clamp24(clamp32(mix));
}

static inline void chorus_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        ch_lfo_phase_l += ch_lfo_inc;

        int32_t tap_l = chorus_read_tap(
            ch_buf_l,
            ch_lfo_phase_l,
            &ch_delay_q16_state_l,
            &ch_thiran_y_prev_l);

        ch_buf_l[ch_write_pos] = in_l[i];
        ch_buf_r[ch_write_pos] = in_l[i];
        ch_buf_c[ch_write_pos] = in_l[i];

        tap_l = chorus_process_lpf_q16(
            tap_l,
            &ch_lpf_state_l,
            ch_lpf_coef_q16);

        in_l[i] = chorus_mix_output(in_l[i], tap_l);
        in_r[i] = in_l[i];

        ch_write_pos++;
        if (ch_write_pos >= CHORUS_MAX_SAMPLES)
            ch_write_pos = 0;
    }
}

static inline void chorus_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        ch_lfo_phase_l += ch_lfo_inc;
        ch_lfo_phase_r = ch_lfo_phase_l + 0x80000000u;

        int32_t tap_l = chorus_read_tap(
            ch_buf_l,
            ch_lfo_phase_l,
            &ch_delay_q16_state_l,
            &ch_thiran_y_prev_l);

        int32_t tap_r = chorus_read_tap(
            ch_buf_r,
            ch_lfo_phase_r,
            &ch_delay_q16_state_r,
            &ch_thiran_y_prev_r);

        ch_buf_l[ch_write_pos] = in_l[i];
        ch_buf_r[ch_write_pos] = in_r[i];
        ch_buf_c[ch_write_pos] = (in_l[i] >> 1) + (in_r[i] >> 1);

        tap_l = chorus_process_lpf_q16(
            tap_l,
            &ch_lpf_state_l,
            ch_lpf_coef_q16);

        tap_r = chorus_process_lpf_q16(
            tap_r,
            &ch_lpf_state_r,
            ch_lpf_coef_q16);

        in_l[i] = chorus_mix_output(in_l[i], tap_l);
        in_r[i] = chorus_mix_output(in_r[i], tap_r);

        ch_write_pos++;
        if (ch_write_pos >= CHORUS_MAX_SAMPLES)
            ch_write_pos = 0;
    }
}

// ========================================
// 音频处理函数
// ========================================

static inline void chorus_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    if (STEREO)
    {
        chorus_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        chorus_process_block_mono(in_l, in_r, frames);
    }
}
#endif