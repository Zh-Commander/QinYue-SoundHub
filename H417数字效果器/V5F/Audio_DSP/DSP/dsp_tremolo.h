#ifndef __DSP_TREMOLO_H
#define __DSP_TREMOLO_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

// ========================================
// 可调参数
// ========================================

/* ========== 颤音效果参数（Q16 定点格式） ========== */
static uint32_t tremolo_phase_q16; /* LFO 相位累加器，用于生成颤音调制波形 */            // 32 位相位累加器
static uint32_t tremolo_speed_q16; /* LFO 速度参数，对应颤音频率 */                      // 每个 sample 相位增量
static uint32_t tremolo_depth_q16; /* LFO 深度参数，0 表示无颤音，65536 表示最大深度 */  // 调制深度,0~Q16_ONE

// ========================================
// 状态参数
// ========================================
/* --- LFO 状态变量 --- */
/* 左右声道的低频振荡器输出值 */
static uint32_t lfo_l_q16;  // 左声道 LFO 输出
static uint32_t lfo_r_q16;  // 右声道 LFO 输出

// ========================================
// 状态操作函数
// ========================================

static inline void DSP_Tremolo_LoadDefaultParams(void)
{
    tremolo_phase_q16 = 0;       /* 初始相位 */
    tremolo_speed_q16 = 500000u; /* 默认速度为 0 */
    tremolo_depth_q16 = Q16_ONE; /* 默认深度为 0，即关闭颤音效果 */
}

static inline void DSP_Tremolo_ClearState(void)
{
    tremolo_phase_q16 = 0;
    lfo_l_q16 = 0;
    lfo_r_q16 = 0;
}

static inline void DSP_Tremolo_Init(void)
{
    DSP_Tremolo_LoadDefaultParams();
    DSP_Tremolo_ClearState();
}

typedef enum
{
    DSP_TREMOLO_PARAM_SPEED = 0, /* 0.05 ~ 10 Hz */
    DSP_TREMOLO_PARAM_DEPTH,     /* 0 ~ 100% */

    DSP_TREMOLO_PARAM_NUM
} dsp_tremolo_param_t;

static inline void DSP_Tremolo_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_tremolo_param_t)param)
    {
        case DSP_TREMOLO_PARAM_SPEED:
        {
            tremolo_speed_q16 = DSP_Param_PotToLfoInc(value, 0.05f, 10.0f);
            break;
        }

        case DSP_TREMOLO_PARAM_DEPTH:
        {
            tremolo_depth_q16 = DSP_Param_PotToQ16(value, 0, Q16_ONE);
            break;
        }

        default:
            break;
    }
}

// ========================================
// 音频处理辅助函数
// ========================================

static inline void tremolo_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        lfo_l_q16 = lfo_q16_shape(tremolo_phase_q16, LFO_TRIANGLE);

        uint32_t one_minus_depth_q16 = Q16_ONE - tremolo_depth_q16;
        uint32_t amp_q16 = one_minus_depth_q16 +
                           (uint32_t)(((uint64_t)lfo_l_q16 * tremolo_depth_q16) >> 16);

        in_l[i] = multiply_q16(in_l[i], amp_q16);
        in_r[i] = in_l[i];

        tremolo_phase_q16 += tremolo_speed_q16;
    }
}

static inline void tremolo_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        uint32_t phase_l = tremolo_phase_q16;
        uint32_t phase_r = phase_l + 0x80000000u;

        lfo_l_q16 = lfo_q16_shape(phase_l, LFO_TRIANGLE);
        lfo_r_q16 = lfo_q16_shape(phase_r, LFO_TRIANGLE);

        uint32_t one_minus_depth_q16 = Q16_ONE - tremolo_depth_q16;

        uint32_t amp_l_q16 = one_minus_depth_q16 +
                             (uint32_t)(((uint64_t)lfo_l_q16 * tremolo_depth_q16) >> 16);

        uint32_t amp_r_q16 = one_minus_depth_q16 +
                             (uint32_t)(((uint64_t)lfo_r_q16 * tremolo_depth_q16) >> 16);

        in_l[i] = multiply_q16(in_l[i], amp_l_q16);
        in_r[i] = multiply_q16(in_r[i], amp_r_q16);

        tremolo_phase_q16 += tremolo_speed_q16;
    }
}

// ========================================
// 音频处理函数
// ========================================

static inline void tremolo_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    if (STEREO)
    {
        tremolo_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        tremolo_process_block_mono(in_l, in_r, frames);
    }
}

#endif