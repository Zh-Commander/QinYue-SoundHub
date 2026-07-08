#ifndef __DSP_AUDIO_PROCESS_BASE_H
#define __DSP_AUDIO_PROCESS_BASE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#include "dsp_base.h"

#define PEAK_MAX 0x7FFFFF00   // ���ڷ�ֵ�������24λ����ֵ��Լ24λ���ֵ��
#define PEAK_MIN -0x7FFFFF00  // ���ڷ�ֵ������С24λ����ֵ��Լ24λ��Сֵ��
#define COMP_ENV_SHIFT 7

// ========================================
// �����ͺ�
// ========================================

// --- �˲���ϵ����Q8.24 ��ʽ�� ---
// Ϊ������������������Ƶ��
#define BASS_A_Q24 (0x0003FD65)    // 120 Hz
#define MID_A_Q24 (0x0013563F)     // 600 Hz
#define TREBLE_A_Q24 (0x00579B7C)  // 3.2 kHz

// ȫ�ָ�ͨ�͵�ͨ�˲���
#define HPF_A_Q24 (0x0002FF8C)  // 90  Hz
#define LPF_A_Q24 (0x0092ACAE)  // 6.5 kHz

// ����ľ�����Ƶ��
#define LOW_A_Q24 (0x00355EC)       // 100 Hz
#define LOW_MID_A_Q24 (0x009DE1C)   // 300 Hz
#define HIGH_MID_A_Q24 (0x01E0870)  // 1.0 kHz
#define HIGH_A_Q24 (0x0385A9C)      // 2.0 kHz

// --- ���泣����Q8.24 ��ʽ�� ---
#define MIN_GAIN_Q24 0x00000000  // 0.0

// ============================================================================
// === ������������ ============================================================
// ============================================================================

// ��64λֵǯλ�� int32_t ��Χ������ʹ�ã�
static inline __attribute__((always_inline)) int32_t clamp32(int64_t x)
{
    if (x > INT32_MAX)
        return INT32_MAX;
    if (x < INT32_MIN)
        return INT32_MIN;
    return (int32_t)x;
}

// ��32λֵǯλ��24λֵ
static inline __attribute__((always_inline)) int32_t clamp24(int32_t x)
{
    if (x > PEAK_MAX)
        x = PEAK_MAX;
    if (x < PEAK_MIN)
        x = PEAK_MIN;
    return (int32_t)x;
}

static inline __attribute__((always_inline))
int32_t multiply_q16_clamp24(int32_t sample, uint32_t gain_q16)
{
    int64_t scaled = ((int64_t)sample * gain_q16) >> 16;

    return clamp24(clamp32(scaled));
}

// ============================================================================
// === ��Ƶ�������� ============================================================
// ============================================================================

// === �˲����ṹ�� ===
typedef struct
{
    int32_t a_q24;             // Q8.24 �˲���ϵ��
    int32_t state_l, state_r;  // ��������״̬
} OnePole;

typedef struct
{
    OnePole hpf, lpf;    // ��ͨ�͵�ͨ�������˲���
    int32_t gain_q24;    // Q8.24 ����
    int32_t s1_l, s2_l;  // ����������״̬
    int32_t s1_r, s2_r;  // ����������״̬
} BPFPair;

// Ӧ��һ�� IIR ��ͨ�˲���
static inline int32_t apply_1pole_lpf(int32_t x, int32_t *state, int32_t a_q24)
{
    int32_t diff = x - *state;
    *state += qmul(diff, a_q24);
    return *state;
}

// Ӧ��һ�� IIR ��ͨ�˲���
static inline int32_t apply_1pole_hpf(int32_t x, int32_t *state, int32_t a_q24)
{
    int32_t prev = *state;
    int32_t diff = x - prev;
    *state += qmul(diff, a_q24);
    return x - *state;
}

// Ӧ��һ�� IIR ��ͨ�˲���(���ٰ�)
static inline __attribute__((always_inline)) int32_t apply_1pole_lpf_fast(int32_t x, int32_t *state, int32_t a_q24)
{
    int32_t s = *state;
    s += qmul_fast(x - s, a_q24);
    *state = s;
    return s;
}

// Ӧ��һ�� IIR ��ͨ�˲���(���ٰ�)
static inline __attribute__((always_inline)) int32_t apply_1pole_hpf_fast(int32_t x, int32_t *state, int32_t a_q24)
{
    int32_t s = *state;
    s += qmul_fast(x - s, a_q24);
    *state = s;
    return x - s;
}

// ��ͨ�˲�������ͨ + ��ͨ������
static inline int32_t apply_1pole_bpf(int32_t x, BPFPair *f, int ch)
{
    int32_t *s1 = (ch == 0) ? &f->hpf.state_l : &f->hpf.state_r;
    int32_t *s2 = (ch == 0) ? &f->lpf.state_l : &f->lpf.state_r;

    int32_t hp = apply_1pole_hpf(x, s1, f->hpf.a_q24);
    int32_t bp = apply_1pole_lpf(hp, s2, f->lpf.a_q24);

    return (f->gain_q24 == Q24_ONE) ? bp : qmul(bp, f->gain_q24);
}

// �����˲������ݲ�����
static inline int32_t apply_1pole_bsf(int32_t x, BPFPair *f, int ch)
{
    int32_t *s1 = (ch == 0) ? &f->hpf.state_l : &f->hpf.state_r;
    int32_t *s2 = (ch == 0) ? &f->lpf.state_l : &f->lpf.state_r;

    int32_t hp = apply_1pole_hpf(x, s1, f->hpf.a_q24);
    int32_t bp = apply_1pole_lpf(hp, s2, f->lpf.a_q24);

    int32_t notch = x - bp;
    return qmul(notch, f->gain_q24);
}

// // ���ٷ�ֵ��ƽ�����޷��� VU ����24λ������
// static inline void process_audio_clipping(int32_t sample_left, int32_t sample_right, volatile int32_t* local_peak_left, volatile int32_t* local_peak_right) {
//     int32_t abs_left = (sample_left < 0) ? -sample_left : sample_left;
//     if (abs_left > *local_peak_left) *local_peak_left = abs_left;

//  int32_t abs_right = (sample_right < 0) ? -sample_right : sample_right;
//  if (abs_right > *local_peak_right) *local_peak_right = abs_right;
// }

// �����Զ���,��ʱ��Ҫ�ĳɴӴ�����������������

// �ӵ�λ������������pot_value ��ΧΪ 0..POT_MAX��
static inline void update_volume_from_pot(void)
{
    // volume_q16 = 2048;  //((uint32_t)pot_value[6] * Q16_ONE) / POT_MAX;
}

// �� 48kHz ��ռԼ 1% �� core 0 CPU ʱ��
// ��һ������������Ӧ��������24λ���ȣ�
static inline void process_audio_volume_sample(int32_t *inout_l, int32_t *inout_r)
{
    *inout_l = multiply_q16(*inout_l, Q16_ONE);
    *inout_r = multiply_q16(*inout_r, Q16_ONE);
}

// ============================================================================
// === LFO ���� ================================================================
// ============================================================================

// ģʽ
#define LFO_TRIANGLE 0         // ���ǲ�
#define LFO_TRIANGLE_SMOOTH 1  // ƽ�����ǲ�
#define LFO_SINE 2             // ���Ҳ�

// ���룺32λ��λ�ۼ���
// �����Q16 LFO ֵ (0..65535)
static inline uint32_t lfo_q16_shape(uint32_t phase, uint8_t mode)
{
    uint32_t folded = (phase >> 15) & 0x1FFFF;  // 17λ�۵���λ
    if (folded >= 65536)
        folded = 131071 - folded;               // ���� [0..65535] ��Χ�ڵ������ǲ���

    if (mode == LFO_TRIANGLE)
    {
        return folded;
    }
    else if (mode == LFO_TRIANGLE_SMOOTH)
    {
        // ƽ��������y = 3x^2 - 2x^3
        uint32_t x = folded;                    // Q16
        uint64_t x2 = ((uint64_t)x * x) >> 16;  // Q32 >> 16 = Q16
        uint64_t x3 = (x2 * x) >> 16;           // Q32 >> 16 = Q16
        uint64_t y = (3 * x2) - (2 * x3);       // Q16
        return (y > 65535) ? 65535 : (uint32_t)y;
    }
    else if (mode == LFO_SINE)
    {
        // �����߽��Ƶ����ң�y = 1 - 4(x - 0.5)^2
        int32_t x_q16 = (int32_t)folded - 32768;
        int64_t x2 = ((int64_t)x_q16 * x_q16) >> 15;  // Q17
        int32_t y_q16 = 65535 - (int32_t)x2;
        return (y_q16 < 0) ? 0 : (uint32_t)y_q16;
    }

    // Ĭ�ϻ��ˣ����ǲ���
    return folded;
}

// ============================================================================
// === ��չ�ģ�� ==============================================================
// ============================================================================

// �������������������ԣ������ܽ��ж���ѹ�� (amount_q24 * x^2)�������ܽ��м����� (neg_scale_q24)��
static inline __attribute__((always_inline))
int32_t
cathode_squish_q24(int32_t x, int32_t amount_q24, int32_t neg_scale_q24)
{
    if (x > 0)
    {
        // x - amount * x^2 �������룩
        int32_t x2 = qmul(x, x);
        int32_t comp = qmul(amount_q24, x2);
        return x - comp;
    }
    else
    {
        return qmul(x, neg_scale_q24);
    }
}

// ���������ܵĲ�����������
// y = x - k3*x^3 + k5*x^5 (k5 ���ѡ���� |x| > x5_gate_thresh ����)
// k3/k5 ���Բ��Գƣ��ֱ�������ϵ�����ϡ�
// use_x5 ��0 ʹ�� x^5 ͨ·����Ϊ 0 �ɽ��ö������޸Ĵ��롣
static inline __attribute__((always_inline))
int32_t
triode_ws_35_asym_fast_q24(int32_t x,
                           int32_t k3_pos_q24, int32_t k5_pos_q24,
                           int32_t k3_neg_q24, int32_t k5_neg_q24,
                           int32_t x5_gate_thresh_q24,
                           int use_x5)
{
    // ���Ƶ� Q8.24 �� ��1.0
    if (x > 0x01000000)
        x = 0x01000000;
    if (x < -0x01000000)
        x = -0x01000000;

    // ����ĳ˷���
    int32_t x2 = qmul(x, x);   // x^2
    int32_t x3 = qmul(x2, x);  // x^3

    // y = x - k3*x^3
    const int32_t k3 = (x >= 0) ? k3_pos_q24 : k3_neg_q24;
    int32_t y = x - qmul(k3, x3);

    if (use_x5)
    {
        int32_t ax = (x >= 0) ? x : -x;
        if (ax > x5_gate_thresh_q24)
        {
            int32_t x5 = qmul(x3, x2);  // x^5
            const int32_t k5 = (x >= 0) ? k5_pos_q24 : k5_neg_q24;
            y += qmul(k5, x5);
        }
    }

    // ��ȫ�޷�
    if (y > 0x01000000)
        y = 0x01000000;
    if (y < -0x01000000)
        y = -0x01000000;
    return y;
}


#endif