#ifndef __DSP_FLANGER_H
#define __DSP_FLANGER_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"
#include <string.h>

#define FLANGER_MAX_SAMPLES 256
#define FLANGER_MIN_DELAY_SAMPLES 8

// ========================================
// 可调参数
// ========================================

static uint32_t fl_feedback_q16;
static uint32_t fl_mix_q16;
static int32_t fl_volume_q24;

static uint32_t fl_depth_q16;

static uint32_t fl_lfo_inc;

static uint32_t fl_lpf_coef_q16;

// ========================================
// 状态参数
// ========================================

static int32_t fl_buffer_l[FLANGER_MAX_SAMPLES];
static int32_t fl_buffer_r[FLANGER_MAX_SAMPLES];
static uint32_t fl_write_pos;

static uint32_t fl_lfo_phase_l;
static uint32_t fl_lfo_phase_r;

static int32_t fl_lpf_state_l;
static int32_t fl_lpf_state_r;

static int32_t fl_thiran_y_prev_l;
static int32_t fl_thiran_y_prev_r;

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Flanger_LoadDefaultParams(void)
{
    fl_depth_q16 = Q16_ONE;  // 最大扫动范围
    fl_lfo_inc = 120000u;    // LFO 速度

    fl_feedback_q16 = Q16_ONE / 4;
    fl_mix_q16 = Q16_ONE / 2;  // dry/wet
    fl_volume_q24 = Q24_ONE;   // 音量

    fl_lpf_coef_q16 = 0x4000;
}

static inline void DSP_Flanger_ClearState(void)
{
    memset(fl_buffer_l, 0, sizeof(fl_buffer_l));
    memset(fl_buffer_r, 0, sizeof(fl_buffer_r));

    fl_write_pos = 0;

    fl_lfo_phase_l = 0;
    fl_lfo_phase_r = 0x80000000u;

    fl_lpf_state_l = 0;
    fl_lpf_state_r = 0;

    fl_thiran_y_prev_l = 0;
    fl_thiran_y_prev_r = 0;
}

static inline void DSP_Flanger_Init(void)
{
    DSP_Flanger_LoadDefaultParams();
    DSP_Flanger_ClearState();
}

  typedef enum
  {
      DSP_FLANGER_PARAM_SPEED = 0,     /* 0.05 ~ 5 Hz */
      DSP_FLANGER_PARAM_DEPTH,         /* 0 ~ 100% */
      DSP_FLANGER_PARAM_FEEDBACK,      /* 0 ~ 90% */
      DSP_FLANGER_PARAM_MIX,           /* 0 ~ 100% */
      DSP_FLANGER_PARAM_TONE,          /* 100 Hz ~ 8 kHz */
      DSP_FLANGER_PARAM_VOLUME,        /* 0.1x ~ 3.0x */

      DSP_FLANGER_PARAM_NUM
  } dsp_flanger_param_t;

    static inline void DSP_Flanger_UpdateParamFromPot(uint8_t param, int32_t value)
  {
      value = DSP_Param_ClampPot(value);

      switch ((dsp_flanger_param_t)param)
      {
          case DSP_FLANGER_PARAM_SPEED:
          {
              fl_lfo_inc = DSP_Param_PotToLfoInc(value, 0.05f, 5.0f);
              break;
          }

          case DSP_FLANGER_PARAM_DEPTH:
          {
              fl_depth_q16 = DSP_Param_PotToQ16(value, 0, Q16_ONE);
              break;
          }

          case DSP_FLANGER_PARAM_FEEDBACK:
          {
              fl_feedback_q16 = DSP_Param_PotToQ16(
                  value,
                  0,
                  (Q16_ONE * 90u) / 100u);
              break;
          }

          case DSP_FLANGER_PARAM_MIX:
          {
              fl_mix_q16 = DSP_Param_PotToQ16(value, 0, Q16_ONE);
              break;
          }

          case DSP_FLANGER_PARAM_TONE:
          {
              float fc = DSP_Param_PotToLogFloat(value, 100.0f, 8000.0f);
              fl_lpf_coef_q16 = DSP_Param_OnePoleDecayQ16FromHz(fc);
              break;
          }

          case DSP_FLANGER_PARAM_VOLUME:
          {
              fl_volume_q24 = DSP_Param_PotToQ24(
                  value,
                  float_to_q24(0.1f),
                  float_to_q24(3.0f));
              break;
          }

          default:
              break;
      }
  }

// ========================================
// 音频处理辅助函数
// ========================================

/* 一阶Thiran全通分数延时读取函数
 * mu 为分数部分，Q16格式，范围 [0,1)
 * base_idx 指向 x[n-1] （当前写入位置向前 int_delay-1 处）
 */
static inline __attribute__((always_inline)) int32_t flanger_read_thiran_q16(
    const int32_t *buf,
    uint32_t base_idx,
    uint32_t frac_q16,
    int32_t *y_prev)
{
    uint32_t i0 = base_idx;
    uint32_t i1 = (base_idx + 1) % FLANGER_MAX_SAMPLES;

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

/* 一阶低通滤波器处理函数（Q16系数）
 * x: 输入信号, state: 滤波器状态（即上一次输出）, coef_q16: 低通系数（Q16）
 */
static inline __attribute__((always_inline)) int32_t flanger_process_lpf_q16(
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

static inline void flanger_calc_delay(
    uint32_t phase,
    uint32_t *base_idx,
    uint32_t *frac_q16)
{
    const uint32_t max_depth_samples = FLANGER_MAX_SAMPLES - FLANGER_MIN_DELAY_SAMPLES - 4;

    uint32_t lfo_q16 = lfo_q16_shape(phase, LFO_TRIANGLE);

    uint64_t scaled_q32 = (uint64_t)lfo_q16 * (uint64_t)fl_depth_q16;

    uint32_t offset_q16 =
        (uint32_t)((((uint64_t)max_depth_samples << 16) * scaled_q32) >> 32);

    uint32_t delay_q16 = (FLANGER_MIN_DELAY_SAMPLES << 16) + offset_q16;

    uint32_t int_delay = delay_q16 >> 16;
    *frac_q16 = delay_q16 & 0xFFFF;

    *base_idx = (fl_write_pos + FLANGER_MAX_SAMPLES - int_delay - 1) % FLANGER_MAX_SAMPLES;
}

static inline __attribute__((always_inline)) int32_t process_flanger_channel(
    int32_t dry,
    int32_t *buffer,
    uint32_t phase,
    int32_t *lpf_state,
    int32_t *thiran_y_prev,
    int32_t *write_value)
{
    uint32_t base_idx;
    uint32_t frac_q16;

    flanger_calc_delay(phase, &base_idx, &frac_q16);

    int32_t delayed = flanger_read_thiran_q16(
        buffer,
        base_idx,
        frac_q16,
        thiran_y_prev);

    int32_t fb = multiply_q16(delayed, fl_feedback_q16);

    int64_t wr = (int64_t)dry + fb;
    *write_value = clamp24(clamp32(wr));

    delayed = flanger_process_lpf_q16(
        delayed,
        lpf_state,
        fl_lpf_coef_q16);

    int64_t mix = ((int64_t)dry * (Q16_ONE - fl_mix_q16) +
                   (int64_t)delayed * fl_mix_q16) >>
                  16;

    mix = (mix * fl_volume_q24) >> 24;

    return clamp24(clamp32(mix));
}

static inline void flanger_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        fl_lfo_phase_l += fl_lfo_inc;

        int32_t write_l;

        in_l[i] = process_flanger_channel(
            in_l[i],
            fl_buffer_l,
            fl_lfo_phase_l,
            &fl_lpf_state_l,
            &fl_thiran_y_prev_l,
            &write_l);

        fl_buffer_l[fl_write_pos] = write_l;
        fl_buffer_r[fl_write_pos] = write_l;

        in_r[i] = in_l[i];

        fl_write_pos++;
        if (fl_write_pos >= FLANGER_MAX_SAMPLES)
            fl_write_pos = 0;
    }
}

static inline void flanger_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        fl_lfo_phase_l += fl_lfo_inc;
        fl_lfo_phase_r = fl_lfo_phase_l + 0x80000000u;

        int32_t write_l;
        int32_t write_r;

        in_l[i] = process_flanger_channel(
            in_l[i],
            fl_buffer_l,
            fl_lfo_phase_l,
            &fl_lpf_state_l,
            &fl_thiran_y_prev_l,
            &write_l);

        in_r[i] = process_flanger_channel(
            in_r[i],
            fl_buffer_r,
            fl_lfo_phase_r,
            &fl_lpf_state_r,
            &fl_thiran_y_prev_r,
            &write_r);

        fl_buffer_l[fl_write_pos] = write_l;
        fl_buffer_r[fl_write_pos] = write_r;

        fl_write_pos++;
        if (fl_write_pos >= FLANGER_MAX_SAMPLES)
            fl_write_pos = 0;
    }
}

// ========================================
// 音频处理函数
// ========================================

static inline void flanger_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    if (STEREO)
    {
        flanger_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        flanger_process_block_mono(in_l, in_r, frames);
    }
}

#endif