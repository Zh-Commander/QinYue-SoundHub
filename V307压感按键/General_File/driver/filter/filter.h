#ifndef __FILTER_H
#define __FILTER_H

#include <stdint.h>
#include <stdbool.h>

/* ========== 滑动平均滤波器 ========== */

typedef struct Filter_MA_Instance_t *Filter_MA_Handle_t;

typedef struct
{
    float InitDataSingle;
    float *InitData;
    bool isUseDataSingle;
    uint8_t WindowSize;
} Filter_MA_InitTypeDef;

Filter_MA_Handle_t Filter_MA_Init(Filter_MA_InitTypeDef *InitStructure);
bool Filter_MA_DeInit(Filter_MA_Handle_t FHandle);
float Filter_MA_Update(Filter_MA_Handle_t FHandle, float newData);

/* ========== 滑动平均滤波器（窗口为2的正整数次幂，且输入和返回值为正整数） ========== */
struct Filter_MA2D_Instance_t
{
    uint32_t Sum;
    uint32_t *DataList;
    uint8_t Index;
    uint8_t ExponentOfTwo;
    uint8_t WindowSize;
};

typedef struct Filter_MA2D_Instance_t *Filter_MA2D_Handle_t;

typedef struct
{
    uint32_t InitDataSingle;
    uint32_t *InitData;
    bool isUseDataSingle;
    uint8_t ExponentOfTwo;
} Filter_MA2D_InitTypeDef;

Filter_MA2D_Handle_t Filter_MA2D_Init(Filter_MA2D_InitTypeDef *InitStructure);
bool Filter_MA2D_DeInit(Filter_MA2D_Handle_t FHandle);
uint32_t Filter_MA2D_Update(Filter_MA2D_Handle_t FHandle, uint32_t newData);

/* ========== 高斯加权滑动平均滤波器 ========== */

struct Filter_GWMA_Instance_t
{
    float *DataList;
    float *WeightList;
    uint8_t Index;
    uint8_t WindowSize;
};

typedef struct Filter_GWMA_Instance_t *Filter_GWMA_Handle_t;

typedef struct
{
    float Sigma;
    float InitDataSingle;
    float *InitData;
    bool isUseDataSingle;
    uint8_t WindowSize;
} Filter_GWMA_InitTypeDef;

Filter_GWMA_Handle_t Filter_GWMA_Init(Filter_GWMA_InitTypeDef *InitStructure);
bool Filter_GWMA_DeInit(Filter_GWMA_Handle_t FHandle);
void Filter_GWMA_Update(Filter_GWMA_Handle_t FHandle, float newData);
float Filter_GWMA_Calculate(Filter_GWMA_Handle_t FHandle);

/* ========== 中值滤波器 ========== */
struct Filter_Median_Instance_t
{
    float *DataBuffer;         // 数据缓冲区
    float *SortedBuffer;       // 排序缓冲区（用于快速查找中值）
    uint8_t Index;             // 当前写入位置
    uint8_t WindowSize;        // 窗口大小
    bool IsInitialized;        // 是否已初始化
    bool OptimizedSort;        // 是否启用优化排序
    uint8_t HalfWindow;        // 窗口大小的一半（预计算，提高效率）
};

struct Filter_MA_Instance_t
{
    float Sum;
    float *DataList;
    uint8_t Index;
    uint8_t WindowSize;
};

typedef struct Filter_Median_Instance_t *Filter_Median_Handle_t;

typedef struct
{
    float InitDataSingle;      // 单个初始化数据
    float *InitData;           // 数组初始化数据
    bool isUseDataSingle;      // 是否使用单个数据初始化
    uint8_t WindowSize;        // 窗口大小（建议使用奇数）
    bool EnableOptimizedSort;  // 启用优化排序（提高效率）
} Filter_Median_InitTypeDef;

/* 函数声明 */
Filter_Median_Handle_t Filter_Median_Init(Filter_Median_InitTypeDef *InitStructure);
bool Filter_Median_DeInit(Filter_Median_Handle_t FHandle);
float Filter_Median_Update(Filter_Median_Handle_t FHandle, float newData);
void Filter_Median_Reset(Filter_Median_Handle_t FHandle, float resetValue);
uint8_t Filter_Median_GetWindowSize(Filter_Median_Handle_t FHandle);

/* ========== 卡尔曼滤波器 ========== */
/* 一维卡尔曼滤波器实例 */
struct Filter_KF1D_Instance_t
{
    float x;  // 状态估计值
    float P;  // 估计误差协方差
    float Q;  // 过程噪声方差
    float R;  // 测量噪声方差
    float F;  // 状态转移系数
    float H;  // 测量系数
};

typedef struct Filter_KF1D_Instance_t *Filter_KF1D_Handle_t;

// 一维卡尔曼滤波初始化结构体
typedef struct
{
    float initState;     // 初始状态估计值
    float initError;     // 初始估计误差
    float processNoise;  // 过程噪声 (Q)
    float measureNoise;  // 测量噪声 (R)
    float stateTrans;    // 状态转移系数 (F, 通常为1或接近1)
    float measureCoeff;  // 测量系数 (H, 通常为1)
} Filter_KF1D_InitTypeDef;

// 一维卡尔曼滤波函数声明
Filter_KF1D_Handle_t Filter_KF1D_Init(Filter_KF1D_InitTypeDef *InitStructure);
bool Filter_KF1D_DeInit(Filter_KF1D_Handle_t FHandle);
float Filter_KF1D_Update(Filter_KF1D_Handle_t FHandle, float measurement);

/* ========== 求导 ========== */
typedef struct Filter_D_Instance_t *Filter_D_Handle_t;

typedef struct
{
    uint64_t InitSysTime;  // 初始系统Tick
    float InitValue;       // 初始值
} Filter_D_InitTypeDef;

Filter_D_Handle_t FIlter_D_Init(Filter_D_InitTypeDef *InitStructure);
bool Filter_D_DeInit(Filter_D_Handle_t FHandle);
float Filter_D_Update(Filter_D_Handle_t FHandle, float newData, float newSysTime);

#endif