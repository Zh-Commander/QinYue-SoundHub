#ifndef __DSP_PARAM_HELPER_H
#define __DSP_PARAM_HELPER_H

static inline int32_t DSP_Param_ClampPot(int32_t value)
{
    if (value < 0)
        return 0;

    if (value > POT_MAX)
        return POT_MAX;

    return value;
}

static inline float DSP_Param_PotToFloat(
    int32_t value,
    float min_val,
    float max_val)
{
    value = DSP_Param_ClampPot(value);

    return min_val + ((float)value / (float)POT_MAX) * (max_val - min_val);
}

static inline float DSP_Param_PotToLogFloat(
    int32_t value,
    float min_val,
    float max_val)
{
    value = DSP_Param_ClampPot(value);

    if (min_val <= 0.0f || max_val <= min_val)
        return DSP_Param_PotToFloat(value, min_val, max_val);

    float norm = (float)value / (float)POT_MAX;

    return min_val * powf(max_val / min_val, norm);
}

static inline uint32_t DSP_Param_PotToQ16(
    int32_t value,
    uint32_t min_q16,
    uint32_t max_q16)
{
    value = DSP_Param_ClampPot(value);

    return min_q16 +
           (uint32_t)(((uint64_t)value * (uint64_t)(max_q16 - min_q16)) / POT_MAX);
}

static inline int32_t DSP_Param_PotToQ24(
    int32_t value,
    int32_t min_q24,
    int32_t max_q24)
{
    value = DSP_Param_ClampPot(value);

    return min_q24 +
           (int32_t)(((int64_t)value * (int64_t)(max_q24 - min_q24)) / POT_MAX);
}

static inline uint32_t DSP_Param_PotToU32(
    int32_t value,
    uint32_t min_val,
    uint32_t max_val)
{
    value = DSP_Param_ClampPot(value);

    return min_val +
           (uint32_t)(((uint64_t)value * (uint64_t)(max_val - min_val)) / POT_MAX);
}

static inline uint8_t DSP_Param_PotToIndex(
    int32_t value,
    uint8_t count)
{
    if (count == 0)
        return 0;

    value = DSP_Param_ClampPot(value);

    uint32_t idx =
        ((uint32_t)value * (uint32_t)count) / ((uint32_t)POT_MAX + 1u);

    if (idx >= count)
        idx = count - 1u;

    return (uint8_t)idx;
}

static inline uint32_t DSP_Param_LfoIncFromHz(float hz)
{
    if (hz <= 0.0f)
        return 0;

    double inc =
        ((double)hz * 4294967296.0) / (double)SAMPLE_RATE;

    if (inc >= 4294967295.0)
        return 0xFFFFFFFFu;

    return (uint32_t)(inc + 0.5);
}

static inline uint32_t DSP_Param_PotToLfoInc(
    int32_t value,
    float min_hz,
    float max_hz)
{
    float hz = DSP_Param_PotToFloat(value, min_hz, max_hz);

    return DSP_Param_LfoIncFromHz(hz);
}

/*
 * 用于 Chorus / Flanger 这种代码：
 *
 * y = (1 - coef) * x + coef * state
 *
 * coef 越大，截止频率越低，声音越暗。
 */
static inline uint32_t DSP_Param_OnePoleDecayQ16FromHz(float fc_hz)
{
    if (fc_hz <= 0.0f)
        return Q16_ONE;

    if (fc_hz >= (float)SAMPLE_RATE * 0.5f)
        return 0;

    float a = expf(-2.0f * (float)M_PI * fc_hz / (float)SAMPLE_RATE);

    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;

    return float_to_q16(a);
}

/*
 * 用于 Delay 这种代码：
 *
 * state += (x - state) * alpha
 *
 * alpha 越大，截止频率越高，声音越亮。
 */
static inline uint32_t DSP_Param_OnePoleAlphaQ16FromHz(float fc_hz)
{
    if (fc_hz <= 0.0f)
        return 0;

    if (fc_hz >= (float)SAMPLE_RATE * 0.5f)
        return Q16_ONE;

    float a = 1.0f - expf(-2.0f * (float)M_PI * fc_hz / (float)SAMPLE_RATE);

    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;

    return float_to_q16(a);
}

#endif