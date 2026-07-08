#ifndef __DSP_SPEAKERSIM_H
#define __DSP_SPEAKERSIM_H

#include "dsp_base.h"
#include "dsp_audio_process_base.h"
#include "dsp_param_helper.h"

// ========================================
// 可调参数
// ========================================

static int32_t cab_body_fc_hz = 120;
static int32_t cab_body_bw_hz = 80;

static int32_t cab_mid_fc_hz = 600;
static int32_t cab_mid_bw_hz = 500;

static int32_t cab_presence_fc_hz = 2500;
static int32_t cab_presence_bw_hz = 1200;

// ========================================
// 状态参数
// ========================================
// === 滤波器实例 ===
static OnePole hpf0, lpf4, lpf5;              /* 定义三个一阶滤波器：hpf0用于80Hz高通，lpf4用于5kHz低通，lpf5用于8kHz低通 */
static BPFPair bpf1, bpf2, bpf3;              /* 定义三个带通滤波器对：bpf1用于低频体感，bpf2用于中频凹陷，bpf3用于高频临场感 */
static int32_t cab_output_gain_q24 = Q24_ONE; /* 箱体输出增益，Q24定点格式，初始化为0dB */

// ========================================
// 状态操作函数
// ========================================

/**
 * 设置带通滤波器对的截止频率
 * @param f  带通滤波器对指针
 * @param fc 中心频率 (Hz)
 * @param bw 带宽 (Hz)
 */
static inline void set_bpf_cutoffs(BPFPair *f, int32_t fc, int32_t bw)
{
    int32_t fc_low = fc - bw / 2;  /* 计算低频截止点 */
    int32_t fc_high = fc + bw / 2; /* 计算高频截止点 */

    /* 将频率限制在有效音频范围内 */
    if (fc_low < 20)
        fc_low = 20;
    if (fc_high > SAMPLE_RATE / 2)
        fc_high = SAMPLE_RATE / 2;

    /* 设置滤波器的 alpha 系数（Q24格式） */
    f->hpf.a_q24 = fc_to_q24(fc_low, SAMPLE_RATE);
    f->lpf.a_q24 = fc_to_q24(fc_high, SAMPLE_RATE);
}

// === 初始化 ===

static inline void DSP_SpeakerSim_LoadDefaultParams(void)
{
    cab_body_fc_hz = 120;
    cab_body_bw_hz = 80;

    cab_mid_fc_hz = 600;
    cab_mid_bw_hz = 500;

    cab_presence_fc_hz = 2500;
    cab_presence_bw_hz = 1200;

    hpf0.a_q24 = fc_to_q24(80, SAMPLE_RATE);

    set_bpf_cutoffs(&bpf1, cab_body_fc_hz, cab_body_bw_hz);
    bpf1.gain_q24 = db_to_q24(5.0f);

    set_bpf_cutoffs(&bpf2, cab_mid_fc_hz, cab_mid_bw_hz);
    bpf2.gain_q24 = db_to_q24(-4.0f);

    set_bpf_cutoffs(&bpf3, cab_presence_fc_hz, cab_presence_bw_hz);
    bpf3.gain_q24 = db_to_q24(6.0f);

    lpf4.a_q24 = fc_to_q24(5000, SAMPLE_RATE);
    lpf5.a_q24 = fc_to_q24(8000, SAMPLE_RATE);

    cab_output_gain_q24 = Q24_ONE;
}

static inline void speaker_sim_clear_bpf_state(BPFPair *f)
{
    f->hpf.state_l = 0;
    f->hpf.state_r = 0;
    f->lpf.state_l = 0;
    f->lpf.state_r = 0;
}

static inline void DSP_SpeakerSim_ClearState(void)
{
    hpf0.state_l = 0;
    hpf0.state_r = 0;
    lpf4.state_l = 0;
    lpf4.state_r = 0;
    lpf5.state_l = 0;
    lpf5.state_r = 0;

    speaker_sim_clear_bpf_state(&bpf1);
    speaker_sim_clear_bpf_state(&bpf2);
    speaker_sim_clear_bpf_state(&bpf3);
}

/**
 * 初始化扬声器模拟器的所有滤波器系数
 */
static inline void DSP_SpeakerSim_Init(void)
{
    DSP_SpeakerSim_LoadDefaultParams();
    DSP_SpeakerSim_ClearState();
}

typedef enum
{
    DSP_CAB_PARAM_LOW_CUT = 0,    /* 30 ~ 200 Hz */

    DSP_CAB_PARAM_BODY_GAIN,      /* -6 ~ +12 dB */
    DSP_CAB_PARAM_BODY_FREQ,      /* 80 ~ 250 Hz */
    DSP_CAB_PARAM_BODY_WIDTH,     /* 40 ~ 200 Hz */

    DSP_CAB_PARAM_MID_SCOOP,      /* -14 ~ 0 dB */
    DSP_CAB_PARAM_MID_FREQ,       /* 300 ~ 1000 Hz */
    DSP_CAB_PARAM_MID_WIDTH,      /* 200 ~ 1000 Hz */

    DSP_CAB_PARAM_PRESENCE_GAIN,  /* -6 ~ +12 dB */
    DSP_CAB_PARAM_PRESENCE_FREQ,  /* 1500 ~ 4000 Hz */
    DSP_CAB_PARAM_PRESENCE_WIDTH, /* 500 ~ 2000 Hz */

    DSP_CAB_PARAM_AIR_FREQ,       /* 3 kHz ~ 10 kHz */
    DSP_CAB_PARAM_VOLUME,         /* 0.1x ~ 2.0x */

    DSP_CAB_PARAM_NUM
} dsp_cab_param_t;

static inline void DSP_SpeakerSim_UpdateParamFromPot(uint8_t param, int32_t value)
{
    value = DSP_Param_ClampPot(value);

    switch ((dsp_cab_param_t)param)
    {
        case DSP_CAB_PARAM_LOW_CUT:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 30u, 200u);
            hpf0.a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_CAB_PARAM_BODY_GAIN:
        {
            float db = DSP_Param_PotToFloat(value, -6.0f, 12.0f);
            bpf1.gain_q24 = db_to_q24(db);
            break;
        }

        case DSP_CAB_PARAM_BODY_FREQ:
        {
            cab_body_fc_hz = (int32_t)DSP_Param_PotToU32(value, 80u, 250u);
            set_bpf_cutoffs(&bpf1, cab_body_fc_hz, cab_body_bw_hz);
            break;
        }

        case DSP_CAB_PARAM_BODY_WIDTH:
        {
            cab_body_bw_hz = (int32_t)DSP_Param_PotToU32(value, 40u, 200u);
            set_bpf_cutoffs(&bpf1, cab_body_fc_hz, cab_body_bw_hz);
            break;
        }

        case DSP_CAB_PARAM_MID_SCOOP:
        {
            float db = DSP_Param_PotToFloat(value, -14.0f, 0.0f);
            bpf2.gain_q24 = db_to_q24(db);
            break;
        }

        case DSP_CAB_PARAM_MID_FREQ:
        {
            cab_mid_fc_hz = (int32_t)DSP_Param_PotToU32(value, 300u, 1000u);
            set_bpf_cutoffs(&bpf2, cab_mid_fc_hz, cab_mid_bw_hz);
            break;
        }

        case DSP_CAB_PARAM_MID_WIDTH:
        {
            cab_mid_bw_hz = (int32_t)DSP_Param_PotToU32(value, 200u, 1000u);
            set_bpf_cutoffs(&bpf2, cab_mid_fc_hz, cab_mid_bw_hz);
            break;
        }

        case DSP_CAB_PARAM_PRESENCE_GAIN:
        {
            float db = DSP_Param_PotToFloat(value, -6.0f, 12.0f);
            bpf3.gain_q24 = db_to_q24(db);
            break;
        }

        case DSP_CAB_PARAM_PRESENCE_FREQ:
        {
            cab_presence_fc_hz = (int32_t)DSP_Param_PotToU32(value, 1500u, 4000u);
            set_bpf_cutoffs(&bpf3, cab_presence_fc_hz, cab_presence_bw_hz);
            break;
        }

        case DSP_CAB_PARAM_PRESENCE_WIDTH:
        {
            cab_presence_bw_hz = (int32_t)DSP_Param_PotToU32(value, 500u, 2000u);
            set_bpf_cutoffs(&bpf3, cab_presence_fc_hz, cab_presence_bw_hz);
            break;
        }

        case DSP_CAB_PARAM_AIR_FREQ:
        {
            uint32_t fc = DSP_Param_PotToU32(value, 3000u, 10000u);
            lpf5.a_q24 = fc_to_q24(fc, SAMPLE_RATE);
            break;
        }

        case DSP_CAB_PARAM_VOLUME:
        {
            cab_output_gain_q24 = DSP_Param_PotToQ24(
                value,
                float_to_q24(0.1f),
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

/**
 * 对单个声道应用扬声器箱体模拟处理
 * @param x  输入采样值 (Q24格式)
 * @param ch 声道索引 (0: 左声道, 1: 右声道)
 * @return 处理后的采样值 (Q24格式)
 */
static inline __attribute__((always_inline)) int32_t process_speaker_sim_channel(int32_t x, int ch)
{
    // 阶段0: 高通滤波器 (HPF)，去除极低频
    int32_t *hpf_state = (ch == 0) ? &hpf0.state_l : &hpf0.state_r;
    int32_t y = apply_1pole_hpf(x, hpf_state, hpf0.a_q24) >> 1;  // 除以2，略微降低增益

    // 并联带通滤波器组
    int32_t p1 = apply_1pole_bpf(x, &bpf1, ch); /* 低频体感 (80-160Hz) */
    int32_t p2 = apply_1pole_bpf(x, &bpf2, ch); /* 中频凹陷 (375-825Hz) */
    int32_t p3 = apply_1pole_bpf(x, &bpf3, ch); /* 高频临场感 (1900-3100Hz) */
    // y += ((p1 >> 1) + (p2 >> 1) + (p3 >> 1)) >> 1;  // 将三个带通输出平均后叠加
    y += (p1 >> 1);
    y -= (p2 >> 2);
    y += (p3 >> 2);

    // LPF 5kHz，增益大约 -2dB
    int32_t *lpf4_state = (ch == 0) ? &lpf4.state_l : &lpf4.state_r;
    y = apply_1pole_lpf(y, lpf4_state, lpf4.a_q24);

    // LPF 8kHz，增益大约 +6dB
    int32_t *lpf5_state = (ch == 0) ? &lpf5.state_l : &lpf5.state_r;
    y = apply_1pole_lpf(y, lpf5_state, lpf5.a_q24);

    // 对输出应用约 2dB 的增益（Q24格式）
    y = qmul(y, 0x1420000);  // 0x1420000 对应约 2dB 增益

    // 应用箱体输出增益（由电位器控制），并钳位到24位范围
    return clamp24(qmul(y, cab_output_gain_q24));

    //  int32_t *hpf_state = (ch == 0) ? &hpf0.state_l : &hpf0.state_r;
    // int32_t *lpf4_state = (ch == 0) ? &lpf4.state_l : &lpf4.state_r;
    // int32_t *lpf5_state = (ch == 0) ? &lpf5.state_l : &lpf5.state_r;

    //  int32_t y = apply_1pole_hpf(x, hpf_state, hpf0.a_q24);
    //  y = apply_1pole_lpf(y, lpf4_state, lpf4.a_q24);
    //  y = apply_1pole_lpf(y, lpf5_state, lpf5.a_q24);

    //  int64_t yy = ((int64_t)y * (int64_t)cab_output_gain_q24) >> 24;
    //  return clamp24(clamp32(yy));
}

static inline void speaker_sim_process_block_mono(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_speaker_sim_channel(in_l[i], 0);
        in_r[i] = in_l[i];
    }
}

static inline void speaker_sim_process_block_stereo(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    for (size_t i = 0; i < frames; i++)
    {
        in_l[i] = process_speaker_sim_channel(in_l[i], 0);
        in_r[i] = process_speaker_sim_channel(in_r[i], 1);
    }
}

// ========================================
// 音频处理函数
// ========================================
static inline void speaker_sim_process_block(
    int32_t *in_l,
    int32_t *in_r,
    size_t frames)
{
    if (STEREO)
    {
        speaker_sim_process_block_stereo(in_l, in_r, frames);
    }
    else
    {
        speaker_sim_process_block_mono(in_l, in_r, frames);
    }
}

#endif