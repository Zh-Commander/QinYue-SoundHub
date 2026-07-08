#ifndef __DSP_BASE_H
#define __DSP_BASE_H

#include <stdint.h>
#include <math.h>
#include "config.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// ========================================
// I2S u16<->u32(左对齐编解码)
// ========================================

static inline uint32_t I2S_32_UnpackSample(volatile uint16_t *buf)
{
    return (uint32_t)(((uint32_t)buf[0] << 16) | (uint32_t)buf[1]);
}

static inline void I2S_32_PackSample(volatile uint16_t *buf, uint32_t sample)
{
    buf[0] = (uint16_t)(((uint32_t)sample >> 16) & 0xFFFF);
    buf[1] = (uint16_t)((uint32_t)sample & 0xFFFF);
}

// ========================================
// 常量定义
// ========================================

#ifndef SAMPLE_RATE
    #define SAMPLE_RATE (I2S_SAMPLING_RATE)
#endif

#ifndef POT_MAX
    #define POT_MAX 4095  // 旋钮最大值
#endif

#define Q31_ONE 0x7FFFFFFF /* Q31格式的1.0, 即2^31 - 1 */
#define Q24_ONE 0x01000000 /* Q24格式的1.0, 即2^24 */
#define Q16_ONE 0x00010000 /* Q16格式的1.0, 即2^16 */
#define Q15_ONE 0x00008000 /* Q15格式的1.0, 即2^15 */

// ========================================
// 电位器转换
// ========================================

/* 将电位器读数映射到 Q16 定点数范围
 * pot: 电位器原始值 (0 ~ POT_MAX)
 * min_q16, max_q16: 目标范围的最小值和最大值 (Q16格式)
 * 返回值: 映射后的 Q16 值
 */
static inline uint32_t map_pot_to_q16(int32_t pot, uint32_t min_q16, uint32_t max_q16)
{
    /* 线性映射: min_q16 + (pot / POT_MAX) * (max_q16 - min_q16) */
    return min_q16 + ((uint64_t)pot * (max_q16 - min_q16)) / POT_MAX;
}

/* 将电位器读数映射到 Q24 定点数范围
 * pot: 电位器原始值 (0 ~ POT_MAX)
 * min_q24, max_q24: 目标范围的最小值和最大值 (Q24格式)
 * 返回值: 映射后的 Q24 值
 */
static inline int32_t map_pot_to_q24(int32_t pot, int32_t min_q24, int32_t max_q24)
{
    /* 线性映射，使用 int64_t 中间结果避免溢出 */
    return min_q24 + ((int64_t)pot * (max_q24 - min_q24)) / POT_MAX;
}

/* 将电位器读数映射到整数范围
 * pot: 电位器原始值 (0 ~ POT_MAX)
 * min_int, max_int: 目标整数范围的最小值和最大值
 * 返回值: 映射后的整数值
 */
static inline int32_t map_pot_to_int(int32_t pot, int32_t min_int, int32_t max_int)
{
    return min_int + ((int64_t)pot * (max_int - min_int)) / POT_MAX;
}

/* 将电位器读数映射到步长为2的偶数范围
 * pot: 电位器原始值 (0 ~ POT_MAX)
 * min_even, max_even: 目标偶数范围的最小值和最大值（假设差值能被2整除）
 * 返回值: 映射后的偶数值，步长为2
 * 原理: 将总步数除以2，映射到连续的索引，然后再乘以2得到偶数
 */
static inline int32_t map_pot_to_even_range(int32_t pot, int32_t min_even, int32_t max_even)
{
    int steps = (max_even - min_even) / 2;     /* 偶数步数 */
    int index = map_pot_to_int(pot, 0, steps); /* 将pot映射到0~steps */
    return min_even + 2 * index;               /* 得到目标偶数 */
}

/* 将电位器读数映射到频率范围 (浮点数)
 * pot: 电位器原始值 (0 ~ POT_MAX)
 * min_hz, max_hz: 目标频率范围的最小值和最大值 (Hz)
 * 返回值: 映射后的频率值 (float)
 */
static inline float map_pot_to_freq(int pot, float min_hz, float max_hz)
{
    return min_hz + ((float)pot / POT_MAX) * (max_hz - min_hz);
}

// ========================================
// 转换
// ========================================

/* 将浮点数转换为 Q16 定点格式
 * x: 浮点数值
 * 返回值: Q16 定点数 (乘以 2^16)
 */
static inline uint32_t float_to_q16(float x)
{
    return (uint32_t)(x * Q16_ONE); /* Q16_ONE = 2^16 */
}

/* 将 Q16 定点数转换为浮点数
 * x: Q16 定点数值
 * 返回值: 浮点数 (除以 2^16)
 */
static inline float q16_to_float(int32_t x)
{
    return x / 65536.0f; /* 65536.0 = 2^16 */
}

/* 将浮点数转换为 Q24 定点格式
 * x: 浮点数值
 * 返回值: Q24 定点数 (乘以 2^24)
 */
static inline int32_t float_to_q24(float x)
{
    return (int32_t)(x * (1 << 24)); /* (1 << 24) 即 16777216 */
}

/* 将 Q24 定点数转换为浮点数
 * x: Q24 定点数值
 * 返回值: 浮点数 (除以 2^24)
 */
static inline float q24_to_float(int32_t x)
{
    return x / 16777216.0f; /* 16777216.0 = 2^24 */
}

/* 将分贝值快速近似转换为Q24格式的线性增益
 * db: 分贝值 (dB)
 * 返回值: Q24 格式的线性增益 (近似值)
 * 转换公式: gain = 10^(db/20), 再乘以 2^24
 */
static inline int32_t db_to_q24(float db)
{
    /* 20*log10(gain) = db  ->  gain = 10^(db/20) */
    float lin = powf(10.0f, db / 20.0f);
    return (int32_t)(lin * (1 << 24));
}

/* 根据截止频率计算一阶低通滤波器的 alpha 系数（仅当参数更新时调用）
 * fc_hz: 截止频率 (Hz)
 * 返回值: Q24 格式的 alpha 系数，范围 [0, 1]
 * 公式: alpha = 1 - exp(-2 * PI * fc / fs)，其中 fs = SAMPLE_RATE
 */
static inline int32_t alpha_from_hz(float fc_hz)
{
    if (fc_hz <= 0.0f)
        return 0; /* 频率非正数时返回 0 */
    float a = 1.0f - expf(-2.0f * (float)M_PI * fc_hz / (float)SAMPLE_RATE);
    if (a < 0.0f)
        a = 0.0f;
    if (a > 1.0f)
        a = 1.0f;           /* 饱和处理 */
    return float_to_q24(a); /* 转换为 Q24 格式 */
}

/* 将时间常数（毫秒）转换为 Q8.24 系数
 * ms: 时间常数 (毫秒)
 * fs: 采样率 (Hz)
 * 返回值: Q8.24 格式的系数，a = exp(-1 / (tau * fs))，其中 tau = ms * 0.001
 */
static inline int32_t ms_to_coeff_q24(float ms, float fs)
{
    float coeff = expf(-1.0f / (ms * 0.001f * fs)); /* tau = ms * 0.001 秒 */
    return float_to_q24(coeff);
}

// ========================================
// 数学函数
// ========================================

/* Q8.24 定点乘法（带四舍五入）
 * a, b: Q8.24 格式的操作数
 * 返回值: Q8.24 格式的乘积，使用四舍五入到最接近的值
 * 实现: 64位中间乘积，加上 2^23 用于舍入，然后右移 24 位
 */
static inline int32_t qmul(int32_t a, int32_t b)
{
    int64_t p = (int64_t)a * b;                 /* 64位乘积 */
    p += (p >= 0) ? (1LL << 23) : -(1LL << 23); /* 加 0.5 * 2^24 进行四舍五入 */
    return (int32_t)(p >> 24);                  /* 右移 24 位得到 Q8.24 */
}

/* Q8.24 定点乘法快速版（带四舍五入）
 * a, b: Q8.24 格式的操作数
 * 返回值: Q8.24 格式的乘积，使用四舍五入到最接近的值
 * 实现: 64位中间乘积，加上 2^23 用于舍入，然后右移 24 位
 */
static inline int32_t qmul_fast(int32_t a, int32_t b)
{
    return (int32_t)(((int64_t)a * (int64_t)b) >> 24); // 测试是否有更快,是的,Fuzz单声道由1435us->655us
}

// === 线性插值与乘法辅助函数 ===

/* 固定点线性插值 (参数 frac 为 Q16 格式)
 * a: 起始值
 * b: 结束值
 * frac_q16: 插值系数，Q16 格式 (范围 0~1.0 对应 0~65536)
 * 返回值: a + (b-a) * frac，结果与 a,b 格式相同
 */
static inline int32_t lerp_fixed(int32_t a, int32_t b, uint32_t frac_q16)
{
    return a + (int32_t)((((int64_t)b - (int64_t)a) * frac_q16) >> 16);//return a + ((int64_t)(b - a) * frac_q16 >> 16); /* 乘法后右移16位恢复定点 */
}

/* Q16 定点乘法: a * b，其中 a 为有符号 Q16，b 为无符号 Q16
 * a: 有符号 Q16 值
 * b: 无符号 Q16 值
 * 返回值: 有符号 Q16 乘积
 */
static inline __attribute__((always_inline)) int32_t multiply_q16(int32_t a, uint32_t b)
{
    return (int32_t)(((int64_t)a * b) >> 16); /* 64位乘积累，右移16位得 Q16 */
}

/* 定点除法: (num << 24) / den，结果视为 Q24 格式
 * num: 分子（通常为定点数）
 * den: 分母（不可为0）
 * 返回值: Q24 格式的商；分母为0时返回 Q24_ONE
 */
static inline int32_t qdiv(int32_t num, int32_t den)
{
    if (den == 0)
        return Q24_ONE;                           /* 避免除零错误，返回1.0 */
    return (int32_t)(((int64_t)num * Q24_ONE) / den); //return (int32_t)(((int64_t)num << 24) / den);/* 左移24再除，得到Q24 */
}

/* 将截止频率转换为用于一阶滤波器的 Q24 系数
 * fc: 截止频率 (Hz)
 * fs: 采样率 (Hz)
 * 返回值: Q24 格式的系数，当 fc >= fs/2 时返回 Q24_ONE
 * 公式: coeff = 2 * sin(pi * fc / fs)，然后转换为 Q24
 */
static inline int32_t fc_to_q24(uint32_t fc, uint32_t fs)
{
    if (fc >= fs / 2)
        return 0xFFFFFF;                         /* Q24_ONE，奈奎斯特频率以上返回1 */

    double normalized = (double)fc / fs;         /* 归一化频率 */
    double coeff = 2.0 * sin(M_PI * normalized); /* 计算系数 */
    return (int32_t)(coeff * (1 << 24) + 0.5);   /* 转换为 Q24，四舍五入 */
}

#endif