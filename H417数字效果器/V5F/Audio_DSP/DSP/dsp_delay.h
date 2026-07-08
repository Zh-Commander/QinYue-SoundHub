#ifndef __DSP_DELAY_H
#define __DSP_DELAY_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

#include <string.h>
#include <stdbool.h>

#define DELAY_DTCM_SAMPLES_PER_CH 24000u
#define DELAY_SRAM_SAMPLES_PER_CH 48000u
#define DELAY_MAX_SAMPLES_PER_CH (DELAY_DTCM_SAMPLES_PER_CH + DELAY_SRAM_SAMPLES_PER_CH)
#define DELAY_MIN_SAMPLES (SAMPLE_RATE / 1000u)
#define DELAY_MAX_FEEDBACK_Q16 ((Q16_ONE * 90u) / 100u)

typedef enum
{
    DELAY_MODE_PARALLEL = 0,
    DELAY_MODE_PINGPONG,
    DELAY_MODE_CROSS,
    DELAY_MODE_MIXED,
    DELAY_MODE_NUM
} delay_mode_t;

typedef enum
{
    DELAY_FRAC_QUARTER = 0,
    DELAY_FRAC_DOTTED_EIGHTH,
    DELAY_FRAC_EIGHTH,
    DELAY_FRAC_TRIPLET,
    DELAY_FRAC_HALF,
    DELAY_FRAC_WHOLE,
    DELAY_FRAC_NUM
} delay_fraction_t;

static const uint32_t dl_fraction_q16[DELAY_FRAC_NUM] =
    {
        Q16_ONE,
        (Q16_ONE * 3u) / 4u,
        Q16_ONE / 2u,
        Q16_ONE / 3u,
        Q16_ONE * 2u,
        Q16_ONE * 4u};

static int32_t dl_buf_l_dtcm[DELAY_DTCM_SAMPLES_PER_CH]
    __attribute__((section(".delay_dtcm"), aligned(4)));
static int32_t dl_buf_r_dtcm[DELAY_DTCM_SAMPLES_PER_CH]
    __attribute__((section(".delay_dtcm"), aligned(4)));
static int32_t dl_buf_l_sram[DELAY_SRAM_SAMPLES_PER_CH]
    __attribute__((section(".delay_sram"), aligned(4)));
static int32_t dl_buf_r_sram[DELAY_SRAM_SAMPLES_PER_CH]
    __attribute__((section(".delay_sram"), aligned(4)));


// ========================================
// 可调参数
// ========================================
static delay_mode_t dl_mode;
static bool dl_enabled;

static uint32_t dl_delay_samples_l;
static uint32_t dl_delay_samples_r;


static uint32_t dl_feedback_q16;
static uint32_t dl_mix_q16;
static uint32_t dl_dry_q16;
static uint32_t dl_volume_q16;

static uint32_t dl_lpf_alpha_q16;
// ========================================
// 状态参数
// ========================================

static uint32_t dl_write_pos;


static int32_t dl_lpf_state_l;
static int32_t dl_lpf_state_r;

static bool dl_tap_active_l;
static bool dl_tap_active_r;
static uint32_t dl_tap_interval_ms;
static delay_fraction_t dl_fraction_l;
static delay_fraction_t dl_fraction_r;

// ========================================
// 内存操作函数
// ========================================

static inline uint32_t delay_clamp_samples(uint32_t samples)
{
    if (samples < DELAY_MIN_SAMPLES)
        return DELAY_MIN_SAMPLES;

    if (samples >= DELAY_MAX_SAMPLES_PER_CH)
        return DELAY_MAX_SAMPLES_PER_CH - 1u;

    return samples;
}

static inline __attribute__((always_inline)) int32_t delay_read_l(uint32_t pos)
{
    if (pos < DELAY_DTCM_SAMPLES_PER_CH)
        return dl_buf_l_dtcm[pos];

    return dl_buf_l_sram[pos - DELAY_DTCM_SAMPLES_PER_CH];
}

static inline __attribute__((always_inline)) int32_t delay_read_r(uint32_t pos)
{
    if (pos < DELAY_DTCM_SAMPLES_PER_CH)
        return dl_buf_r_dtcm[pos];

    return dl_buf_r_sram[pos - DELAY_DTCM_SAMPLES_PER_CH];
}

static inline __attribute__((always_inline)) void delay_write_l(
    uint32_t pos,
    int32_t value)
{
    if (pos < DELAY_DTCM_SAMPLES_PER_CH)
    {
        dl_buf_l_dtcm[pos] = value;
    }
    else
    {
        dl_buf_l_sram[pos - DELAY_DTCM_SAMPLES_PER_CH] = value;
    }
}

static inline __attribute__((always_inline)) void delay_write_r(
    uint32_t pos,
    int32_t value)
{
    if (pos < DELAY_DTCM_SAMPLES_PER_CH)
    {
        dl_buf_r_dtcm[pos] = value;
    }
    else
    {
        dl_buf_r_sram[pos - DELAY_DTCM_SAMPLES_PER_CH] = value;
    }
}

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Delay_ApplyTapTempo(void)
{
    if (dl_tap_interval_ms == 0)
        return;

    if (dl_tap_active_l)
    {
        uint32_t base_samples =
            (uint32_t)(((uint64_t)dl_tap_interval_ms * SAMPLE_RATE) / 1000u);
        uint32_t samples =
            (uint32_t)(((uint64_t)base_samples * dl_fraction_q16[dl_fraction_l]) >> 16);

        dl_delay_samples_l = delay_clamp_samples(samples);
    }

    if (dl_tap_active_r)
    {
        uint32_t base_samples =
            (uint32_t)(((uint64_t)dl_tap_interval_ms * SAMPLE_RATE) / 1000u);
        uint32_t samples =
            (uint32_t)(((uint64_t)base_samples * dl_fraction_q16[dl_fraction_r]) >> 16);

        dl_delay_samples_r = delay_clamp_samples(samples);
    }
}

static inline void DSP_Delay_LoadDefaultParams(void)
{
    dl_mode = DELAY_MODE_PARALLEL;
    dl_enabled = true;

    dl_delay_samples_l = delay_clamp_samples(SAMPLE_RATE / 3u);
    dl_delay_samples_r = delay_clamp_samples((SAMPLE_RATE * 3u) / 8u);

    dl_feedback_q16 = (Q16_ONE * 75u) / 100u;
    dl_mix_q16 = Q16_ONE / 1u;
    dl_dry_q16 = Q16_ONE - dl_mix_q16;
    dl_volume_q16 = Q16_ONE;

    dl_lpf_alpha_q16 = Q16_ONE / 4u;

    dl_tap_active_l = false;
    dl_tap_active_r = false;
    dl_tap_interval_ms = 500u;
    dl_fraction_l = DELAY_FRAC_QUARTER;
    dl_fraction_r = DELAY_FRAC_DOTTED_EIGHTH;
}

static inline void DSP_Delay_ClearState(void)
{
    memset(dl_buf_l_dtcm, 0, sizeof(dl_buf_l_dtcm));
    memset(dl_buf_r_dtcm, 0, sizeof(dl_buf_r_dtcm));
    memset(dl_buf_l_sram, 0, sizeof(dl_buf_l_sram));
    memset(dl_buf_r_sram, 0, sizeof(dl_buf_r_sram));

    dl_write_pos = 0;

    dl_lpf_state_l = 0;
    dl_lpf_state_r = 0;
}

static inline void DSP_Delay_Init(void)
{
    DSP_Delay_LoadDefaultParams();
    DSP_Delay_ClearState();
}

static inline void DSP_Delay_SetMode(delay_mode_t mode)
{
    if (mode >= DELAY_MODE_NUM)
        mode = DELAY_MODE_PARALLEL;

    dl_mode = mode;
}

static inline void DSP_Delay_SetEnabled(bool enabled)
{
    dl_enabled = enabled;
}

static inline void DSP_Delay_SetTimeSamples(
    uint32_t samples_l,
    uint32_t samples_r)
{
    dl_tap_active_l = false;
    dl_tap_active_r = false;

    dl_delay_samples_l = delay_clamp_samples(samples_l);
    dl_delay_samples_r = delay_clamp_samples(samples_r);
}

static inline void DSP_Delay_SetTimeMs(
    uint32_t ms_l,
    uint32_t ms_r)
{
    uint32_t samples_l = (uint32_t)(((uint64_t)ms_l * SAMPLE_RATE) / 1000u);
    uint32_t samples_r = (uint32_t)(((uint64_t)ms_r * SAMPLE_RATE) / 1000u);

    DSP_Delay_SetTimeSamples(samples_l, samples_r);
}

static inline void DSP_Delay_SetFeedbackQ16(uint32_t feedback_q16)
{
    if (feedback_q16 > DELAY_MAX_FEEDBACK_Q16)
        feedback_q16 = DELAY_MAX_FEEDBACK_Q16;

    dl_feedback_q16 = feedback_q16;
}

static inline void DSP_Delay_SetMixQ16(uint32_t mix_q16)
{
    if (mix_q16 > Q16_ONE)
        mix_q16 = Q16_ONE;

    dl_mix_q16 = mix_q16;
    dl_dry_q16 = Q16_ONE - dl_mix_q16;
}

static inline void DSP_Delay_SetVolumeQ16(uint32_t volume_q16)
{
    dl_volume_q16 = volume_q16;
}

static inline void DSP_Delay_SetLpfAlphaQ16(uint32_t alpha_q16)
{
    if (alpha_q16 > Q16_ONE)
        alpha_q16 = Q16_ONE;

    dl_lpf_alpha_q16 = alpha_q16;
}

static inline void DSP_Delay_SetTapFraction(
    delay_fraction_t fraction_l,
    delay_fraction_t fraction_r)
{
    if (fraction_l >= DELAY_FRAC_NUM)
        fraction_l = DELAY_FRAC_QUARTER;
    if (fraction_r >= DELAY_FRAC_NUM)
        fraction_r = DELAY_FRAC_DOTTED_EIGHTH;

    dl_fraction_l = fraction_l;
    dl_fraction_r = fraction_r;

    DSP_Delay_ApplyTapTempo();
}

static inline void DSP_Delay_SetTapTempo(
    uint32_t interval_ms,
    bool active_l,
    bool active_r)
{
    dl_tap_interval_ms = interval_ms;
    dl_tap_active_l = active_l;
    dl_tap_active_r = active_r;

    DSP_Delay_ApplyTapTempo();
}

typedef enum
{
    DSP_DELAY_PARAM_ENABLED = 0,    /* bool: 0/1 */
    DSP_DELAY_PARAM_MODE,           /* enum: delay_mode_t */

    DSP_DELAY_PARAM_TIME_L,         /* continuous: 0~4095 */
    DSP_DELAY_PARAM_TIME_R,         /* continuous: 0~4095 */

    DSP_DELAY_PARAM_FEEDBACK,       /* continuous: 0~4095 */
    DSP_DELAY_PARAM_MIX,            /* continuous: 0~4095 */
    DSP_DELAY_PARAM_TONE,           /* continuous: 0~4095 */
    DSP_DELAY_PARAM_VOLUME,         /* continuous: 0~4095 */

    DSP_DELAY_PARAM_TAP_INTERVAL,   /* direct value: ms */
    DSP_DELAY_PARAM_TAP_ACTIVE_L,   /* bool: 0/1 */
    DSP_DELAY_PARAM_TAP_ACTIVE_R,   /* bool: 0/1 */
    DSP_DELAY_PARAM_TAP_FRACTION_L, /* enum: delay_fraction_t */
    DSP_DELAY_PARAM_TAP_FRACTION_R, /* enum: delay_fraction_t */

    DSP_DELAY_PARAM_NUM
} dsp_delay_param_t;

static inline void DSP_Delay_UpdateParamFromPot(uint8_t param, int32_t value)
{
    switch ((dsp_delay_param_t)param)
    {
        case DSP_DELAY_PARAM_ENABLED:
        {
            /*
             * 离散布尔值：
             * 0 = disabled
             * 非0 = enabled
             */
            DSP_Delay_SetEnabled(value != 0);
            break;
        }

        case DSP_DELAY_PARAM_MODE:
        {
            /*
             * 离散枚举值：
             * 0 = DELAY_MODE_PARALLEL
             * 1 = DELAY_MODE_PINGPONG
             * 2 = DELAY_MODE_CROSS
             * 3 = DELAY_MODE_MIXED
             */
            delay_mode_t mode = DELAY_MODE_PARALLEL;

            if ((value >= 0) && (value < DELAY_MODE_NUM))
            {
                mode = (delay_mode_t)value;
            }

            DSP_Delay_SetMode(mode);
            break;
        }

        case DSP_DELAY_PARAM_TIME_L:
        {
            /*
             * 连续值：
             * value = 0~4095
             * 映射到 1ms ~ 最大 delay samples
             */
            int32_t pot = DSP_Param_ClampPot(value);

            uint32_t samples =
                DELAY_MIN_SAMPLES +
                (uint32_t)(((uint64_t)pot *
                            (DELAY_MAX_SAMPLES_PER_CH - 1u - DELAY_MIN_SAMPLES)) /
                           POT_MAX);

            dl_tap_active_l = false;
            dl_delay_samples_l = delay_clamp_samples(samples);
            break;
        }

        case DSP_DELAY_PARAM_TIME_R:
        {
            /*
             * 连续值：
             * value = 0~4095
             * 映射到 1ms ~ 最大 delay samples
             */
            int32_t pot = DSP_Param_ClampPot(value);

            uint32_t samples =
                DELAY_MIN_SAMPLES +
                (uint32_t)(((uint64_t)pot *
                            (DELAY_MAX_SAMPLES_PER_CH - 1u - DELAY_MIN_SAMPLES)) /
                           POT_MAX);

            dl_tap_active_r = false;
            dl_delay_samples_r = delay_clamp_samples(samples);
            break;
        }

        case DSP_DELAY_PARAM_FEEDBACK:
        {
            /*
             * 连续值：
             * value = 0~4095
             * 映射到 0~90%
             */
            uint32_t fb = DSP_Param_PotToQ16(
                value,
                0,
                DELAY_MAX_FEEDBACK_Q16);

            DSP_Delay_SetFeedbackQ16(fb);
            break;
        }

        case DSP_DELAY_PARAM_MIX:
        {
            /*
             * 连续值：
             * value = 0~4095
             * 映射到 0~100%
             */
            uint32_t mix = DSP_Param_PotToQ16(
                value,
                0,
                Q16_ONE);

            DSP_Delay_SetMixQ16(mix);
            break;
        }

        case DSP_DELAY_PARAM_TONE:
        {
            /*
             * 连续值：
             * value = 0~4095
             * 映射到 300Hz~12000Hz
             */
            float fc = DSP_Param_PotToLogFloat(
                value,
                300.0f,
                12000.0f);

            uint32_t alpha = DSP_Param_OnePoleAlphaQ16FromHz(fc);

            DSP_Delay_SetLpfAlphaQ16(alpha);
            break;
        }

        case DSP_DELAY_PARAM_VOLUME:
        {
            /*
             * 连续值：
             * value = 0~4095
             * 映射到 0.1x~2.5x
             */
            uint32_t volume = DSP_Param_PotToQ16(
                value,
                Q16_ONE / 10u,
                (Q16_ONE * 5u) / 2u);

            DSP_Delay_SetVolumeQ16(volume);
            break;
        }

        case DSP_DELAY_PARAM_TAP_INTERVAL:
        {
            /*
             * 直接单位值：
             * value = tap interval ms
             *
             * 例如：
             * value = 500 表示 500ms
             *
             * 这里不按 0~4095 解释。
             */
            uint32_t interval_ms;

            if (value < 60)
            {
                interval_ms = 60u;
            }
            else if (value > 2000)
            {
                interval_ms = 2000u;
            }
            else
            {
                interval_ms = (uint32_t)value;
            }

            DSP_Delay_SetTapTempo(
                interval_ms,
                dl_tap_active_l,
                dl_tap_active_r);
            break;
        }

        case DSP_DELAY_PARAM_TAP_ACTIVE_L:
        {
            /*
             * 离散布尔值：
             * 0 = inactive
             * 非0 = active
             */
            dl_tap_active_l = (value != 0);

            DSP_Delay_ApplyTapTempo();
            break;
        }

        case DSP_DELAY_PARAM_TAP_ACTIVE_R:
        {
            /*
             * 离散布尔值：
             * 0 = inactive
             * 非0 = active
             */
            dl_tap_active_r = (value != 0);

            DSP_Delay_ApplyTapTempo();
            break;
        }

        case DSP_DELAY_PARAM_TAP_FRACTION_L:
        {
            /*
             * 离散枚举值：
             * 0 = DELAY_FRAC_QUARTER
             * 1 = DELAY_FRAC_DOTTED_EIGHTH
             * 2 = DELAY_FRAC_EIGHTH
             * 3 = DELAY_FRAC_TRIPLET
             * 4 = DELAY_FRAC_HALF
             * 5 = DELAY_FRAC_WHOLE
             */
            delay_fraction_t fraction = DELAY_FRAC_QUARTER;

            if ((value >= 0) && (value < DELAY_FRAC_NUM))
            {
                fraction = (delay_fraction_t)value;
            }

            DSP_Delay_SetTapFraction(
                fraction,
                dl_fraction_r);
            break;
        }

        case DSP_DELAY_PARAM_TAP_FRACTION_R:
        {
            /*
             * 离散枚举值：
             * 0 = DELAY_FRAC_QUARTER
             * 1 = DELAY_FRAC_DOTTED_EIGHTH
             * 2 = DELAY_FRAC_EIGHTH
             * 3 = DELAY_FRAC_TRIPLET
             * 4 = DELAY_FRAC_HALF
             * 5 = DELAY_FRAC_WHOLE
             */
            delay_fraction_t fraction = DELAY_FRAC_DOTTED_EIGHTH;

            if ((value >= 0) && (value < DELAY_FRAC_NUM))
            {
                fraction = (delay_fraction_t)value;
            }

            DSP_Delay_SetTapFraction(
                dl_fraction_l,
                fraction);
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

static inline __attribute__((always_inline)) int32_t delay_process_lpf_q16(
    int32_t x,
    int32_t *state,
    uint32_t alpha_q16)
{
    int64_t diff = (int64_t)x - (int64_t)(*state);
    int64_t y = (int64_t)(*state) + ((diff * alpha_q16) >> 16);

    *state = clamp32(y);
    return *state;
}

static inline __attribute__((always_inline)) int32_t delay_mix_enabled(
    int32_t dry,
    int32_t wet)
{
    int64_t y = ((int64_t)dry * dl_dry_q16 +
                 (int64_t)wet * dl_mix_q16) >>
                16;

    y = (y * dl_volume_q16) >> 16;

    return clamp24(clamp32(y));
}

static inline __attribute__((always_inline)) int32_t delay_mix_tails(
    int32_t dry,
    int32_t wet)
{
    int64_t wet_scaled = ((int64_t)wet * dl_mix_q16) >> 16;
    wet_scaled = (wet_scaled * dl_volume_q16) >> 16;

    return clamp24(clamp32((int64_t)dry + wet_scaled));
}

static inline __attribute__((always_inline)) void delay_process_sample(
    int32_t *inout_l,
    int32_t *inout_r,
    bool tails_only)
{
    int32_t dry_l = *inout_l;
    int32_t dry_r = *inout_r;

    uint32_t read_pos_l =
        (dl_write_pos + DELAY_MAX_SAMPLES_PER_CH - dl_delay_samples_l) %
        DELAY_MAX_SAMPLES_PER_CH;

    uint32_t read_pos_r =
        (dl_write_pos + DELAY_MAX_SAMPLES_PER_CH - dl_delay_samples_r) %
        DELAY_MAX_SAMPLES_PER_CH;

    int32_t delayed_l = delay_read_l(read_pos_l);
    int32_t delayed_r = delay_read_r(read_pos_r);

    int32_t write_in_l = tails_only ? 0 : dry_l;
    int32_t write_in_r = tails_only ? 0 : dry_r;

    int32_t pre_l = 0;
    int32_t pre_r = 0;

    switch (dl_mode)
    {
        case DELAY_MODE_PARALLEL:
        {
            int32_t fb_l = multiply_q16(delayed_l, dl_feedback_q16);
            int32_t fb_r = multiply_q16(delayed_r, dl_feedback_q16);

            pre_l = clamp32((int64_t)write_in_l + fb_l);
            pre_r = clamp32((int64_t)write_in_r + fb_r);
            break;
        }

        case DELAY_MODE_CROSS:
        {
            int32_t fb_l = multiply_q16(delayed_r, dl_feedback_q16);
            int32_t fb_r = multiply_q16(delayed_l, dl_feedback_q16);

            pre_l = clamp32((int64_t)write_in_l + fb_l);
            pre_r = clamp32((int64_t)write_in_r + fb_r);
            break;
        }

        case DELAY_MODE_MIXED:
        {
            int32_t mixed_delay = (delayed_l >> 1) + (delayed_r >> 1);
            int32_t fb = multiply_q16(mixed_delay, dl_feedback_q16);

            pre_l = clamp32((int64_t)write_in_l + fb);
            pre_r = clamp32((int64_t)write_in_r + fb);
            break;
        }

        case DELAY_MODE_PINGPONG:
        default:
        {
            int32_t mono = (dry_l >> 1) + (dry_r >> 1);
            int32_t inject = tails_only ? 0 : mono;

            int32_t fb_l = multiply_q16(delayed_r, dl_feedback_q16);
            int32_t fb_r = multiply_q16(delayed_l, dl_feedback_q16);

            pre_l = clamp32((int64_t)inject + fb_l);
            pre_r = fb_r;
            break;
        }
    }

    int32_t store_l = delay_process_lpf_q16(
        pre_l,
        &dl_lpf_state_l,
        dl_lpf_alpha_q16);

    int32_t store_r = delay_process_lpf_q16(
        pre_r,
        &dl_lpf_state_r,
        dl_lpf_alpha_q16);

    delay_write_l(dl_write_pos, clamp24(store_l));
    delay_write_r(dl_write_pos, clamp24(store_r));

    if (tails_only)
    {
        *inout_l = delay_mix_tails(dry_l, delayed_l);
        *inout_r = delay_mix_tails(dry_r, delayed_r);
    }
    else
    {
        *inout_l = delay_mix_enabled(dry_l, delayed_l);
        *inout_r = delay_mix_enabled(dry_r, delayed_r);
    }

    dl_write_pos++;
    if (dl_write_pos >= DELAY_MAX_SAMPLES_PER_CH)
        dl_write_pos = 0;
}

// ========================================
// 音频处理函数
// ========================================

static inline void delay_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    bool tails_only = !dl_enabled;

    for (size_t i = 0; i < frames; i++)
    {
        delay_process_sample(
            &in_l[i],
            &in_r[i],
            tails_only);
    }
}

#endif
