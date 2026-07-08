#include "filter.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ========== 滑动平均滤波器 ========== */

Filter_MA_Handle_t Filter_MA_Init(Filter_MA_InitTypeDef *InitStructure)
{
    // 给实例分配内存
    Filter_MA_Handle_t FHandle = malloc(sizeof(struct Filter_MA_Instance_t));
    if (FHandle == NULL)
        return NULL;

    // 给滑动窗口分配内存
    float *DataList = malloc(InitStructure->WindowSize * sizeof(float));

    // 填充滑动窗口和初次求和
    float Sum = 0;
    if (InitStructure->isUseDataSingle == true)
    {
        Sum = InitStructure->InitDataSingle;
        for (uint8_t i = 0; i < InitStructure->WindowSize; i++)
        {
            DataList[i] = InitStructure->InitDataSingle;
        }
    }
    else
    {
        memcpy(DataList, InitStructure->InitData, InitStructure->WindowSize * sizeof(float));
        for (uint8_t i = 0; i < InitStructure->WindowSize; i++)
        {
            Sum += InitStructure->InitData[i];
        }
        Sum = Sum / InitStructure->WindowSize;
    }

    // 初始化MA示例
    FHandle->DataList = DataList;
    FHandle->Index = 0;
    FHandle->Sum = Sum;
    FHandle->WindowSize = InitStructure->WindowSize;

    return FHandle;
}

bool Filter_MA_DeInit(Filter_MA_Handle_t FHandle)
{
    free(FHandle->DataList);
    free(FHandle);
    return true;
}

float Filter_MA_Update(Filter_MA_Handle_t FHandle, float newData)
{
    // 计算Sum
    FHandle->Sum = FHandle->Sum - (FHandle->DataList[FHandle->Index] - newData) / FHandle->WindowSize;

    // 用新数据覆盖最后一个数据
    FHandle->DataList[FHandle->Index] = newData;

    // 更新index
    if (FHandle->Index == FHandle->WindowSize - 1)
    {
        FHandle->Index = 0;
    }
    else
    {
        FHandle->Index++;
    }

    return FHandle->Sum;
}

/* ========== 滑动平均滤波器（窗口为2的正整数次幂，且输入和返回值为正整数） ========== */

Filter_MA2D_Handle_t Filter_MA2D_Init(Filter_MA2D_InitTypeDef *InitStructure)
{
    // 给实例分配内存
    Filter_MA2D_Handle_t FHandle = malloc(sizeof(struct Filter_MA2D_Instance_t));
    if (FHandle == NULL)
    {
        free(FHandle);
        return NULL;
    }

    uint8_t WindowSize = 1 << InitStructure->ExponentOfTwo;

    // 给滑动窗口分配内存
    uint32_t *DataList = malloc(sizeof(uint32_t) << InitStructure->ExponentOfTwo);

    // 填充滑动窗口和初次求和
    uint32_t Sum = 0;
    if (InitStructure->isUseDataSingle == true)
    {
        Sum = WindowSize * InitStructure->InitDataSingle;
        for (uint8_t i = 0; i < WindowSize; i++)
        {
            DataList[i] = InitStructure->InitDataSingle;
        }
    }
    else
    {
        memcpy(DataList, InitStructure->InitData, WindowSize * sizeof(uint32_t));
        for (uint8_t i = 0; i < WindowSize; i++)
        {
            Sum += InitStructure->InitData[i];
        }
    }

    // 初始化MA示例
    FHandle->DataList = DataList;
    FHandle->Index = 0;
    FHandle->Sum = Sum;
    FHandle->ExponentOfTwo = InitStructure->ExponentOfTwo;
    FHandle->WindowSize = WindowSize;

    return FHandle;
}

bool Filter_MA2D_DeInit(Filter_MA2D_Handle_t FHandle)
{
    free(FHandle->DataList);
    free(FHandle);
    return true;
}

uint32_t Filter_MA2D_Update(Filter_MA2D_Handle_t FHandle, uint32_t newData)
{
    // 计算Sum
    FHandle->Sum = FHandle->Sum - (FHandle->DataList[FHandle->Index]) + newData;

    // 用新数据覆盖最后一个数据
    FHandle->DataList[FHandle->Index] = newData;

    // 更新index
    if (FHandle->Index == FHandle->WindowSize - 1)
    {
        FHandle->Index = 0;
    }
    else
    {
        FHandle->Index++;
    }

    return (FHandle->Sum) >> (FHandle->ExponentOfTwo);
}

/* ========== 高斯加权滑动平均滤波器 ========== */

Filter_GWMA_Handle_t Filter_GWMA_Init(Filter_GWMA_InitTypeDef *InitStructure)
{
    // 给实例分配内存
    Filter_GWMA_Handle_t FHandle = malloc(sizeof(struct Filter_GWMA_Instance_t));
    if (FHandle == NULL)
        return NULL;

    // 给数据窗口和权重列表分配内存
    float *DataList = malloc(sizeof(float) * InitStructure->WindowSize);
    float *WeightList = malloc(sizeof(float) * InitStructure->WindowSize);

    // 初始化数据窗口
    if (InitStructure->isUseDataSingle == true)
    {
        for (uint8_t i = 0; i < InitStructure->WindowSize; i++)
        {
            DataList[i] = InitStructure->InitDataSingle;
        }
    }
    else
    {
        memcpy(DataList, InitStructure->InitData, sizeof(float) * InitStructure->WindowSize);
    }

    // 计算权重
    float sum = 1;
    for (uint8_t i = 1; i < InitStructure->WindowSize; i++)
    {
        WeightList[i] = expf(-(i * i) / (2 * InitStructure->Sigma * InitStructure->Sigma));
        sum += WeightList[i];
    }

    WeightList[0] = 1 / sum;
    for (uint8_t i = 1; i < InitStructure->WindowSize; i++)
    {
        WeightList[i] /= sum;
    }

    // 初始化实例
    FHandle->DataList = DataList;
    FHandle->WeightList = WeightList;
    FHandle->Index = 0;
    FHandle->WindowSize = InitStructure->WindowSize;
    return FHandle;
}

bool Filter_GWMA_DeInit(Filter_GWMA_Handle_t FHandle)
{
    free(FHandle->DataList);
    free(FHandle->WeightList);
    free(FHandle);
    return true;
}

void Filter_GWMA_Update(Filter_GWMA_Handle_t FHandle, float newData)
{
    FHandle->DataList[FHandle->Index] = newData;
    if (FHandle->Index == FHandle->WindowSize - 1)
    {
        FHandle->Index = 0;
    }
    else
    {
        FHandle->Index++;
    }
}

float Filter_GWMA_Calculate(Filter_GWMA_Handle_t FHandle)
{
    float *WeightList = FHandle->WeightList;
    float *DataList = FHandle->DataList;
    uint8_t WindowSize = FHandle->WindowSize;
    float sum = 0;
    uint8_t readp = FHandle->Index;

    for (uint8_t i = 0; i < WindowSize; i++)
    {
        sum += WeightList[WindowSize - i - 1] * DataList[readp];
        if (readp == WindowSize - 1)
        {
            readp = 0;
        }
        else
        {
            readp++;
        }
    }

    return sum;
}

/* ========== 中值滤波器 ========== */
/* 私有函数声明 */
static void InsertionSort(float *array, uint8_t size);
static uint8_t FindInsertPosition(float *sortedArray, uint8_t size, float value);
static void InsertValueToSortedArray(float *sortedArray, uint8_t size, float value, uint8_t position);
static void RemoveValueFromSortedArray(float *sortedArray, uint8_t size, uint8_t position);
static void InitializeBuffer(Filter_Median_Handle_t FHandle, float initValue);

/* 快速交换宏 */
#define SWAP_FLOAT(a, b) { float temp = (a); (a) = (b); (b) = temp; }

/* ========== 公开函数实现 ========== */

/**
  * @brief  初始化中值滤波器
  * @param  InitStructure: 初始化结构体指针
  * @retval 滤波器句柄，失败返回NULL
  */
Filter_Median_Handle_t Filter_Median_Init(Filter_Median_InitTypeDef *InitStructure)
{
    // 检查参数有效性
    if (InitStructure == NULL || InitStructure->WindowSize == 0)
        return NULL;

    // 分配滤波器实例内存
    Filter_Median_Handle_t FHandle = malloc(sizeof(struct Filter_Median_Instance_t));
    if (FHandle == NULL)
        return NULL;

    // 分配数据缓冲区内存
    FHandle->DataBuffer = malloc(InitStructure->WindowSize * sizeof(float));
    FHandle->SortedBuffer = malloc(InitStructure->WindowSize * sizeof(float));
    
    if (FHandle->DataBuffer == NULL || FHandle->SortedBuffer == NULL)
    {
        free(FHandle->DataBuffer);
        free(FHandle->SortedBuffer);
        free(FHandle);
        return NULL;
    }

    // 初始化滤波器参数
    FHandle->WindowSize = InitStructure->WindowSize;
    FHandle->HalfWindow = InitStructure->WindowSize / 2;
    FHandle->Index = 0;
    FHandle->OptimizedSort = InitStructure->EnableOptimizedSort;
    FHandle->IsInitialized = false;

    // 根据初始化类型填充缓冲区
    if (InitStructure->isUseDataSingle)
    {
        InitializeBuffer(FHandle, InitStructure->InitDataSingle);
    }
    else if (InitStructure->InitData != NULL)
    {
        // 复制初始化数据
        memcpy(FHandle->DataBuffer, InitStructure->InitData, 
               InitStructure->WindowSize * sizeof(float));
        
        // 复制到排序缓冲区并排序
        memcpy(FHandle->SortedBuffer, InitStructure->InitData,
               InitStructure->WindowSize * sizeof(float));
        InsertionSort(FHandle->SortedBuffer, InitStructure->WindowSize);
        
        FHandle->IsInitialized = true;
    }
    else
    {
        // 无初始化数据，标记为未初始化
        FHandle->IsInitialized = false;
    }

    return FHandle;
}

/**
  * @brief  反初始化中值滤波器
  * @param  FHandle: 滤波器句柄
  * @retval 操作结果
  */
bool Filter_Median_DeInit(Filter_Median_Handle_t FHandle)
{
    if (FHandle == NULL)
        return false;

    free(FHandle->DataBuffer);
    free(FHandle->SortedBuffer);
    free(FHandle);
    
    return true;
}

/**
  * @brief  更新中值滤波器
  * @param  FHandle: 滤波器句柄
  * @param  newData: 新输入数据
  * @retval 滤波后的中值
  */
float Filter_Median_Update(Filter_Median_Handle_t FHandle, float newData)
{
    if (FHandle == NULL)
        return 0.0f;

    // 如果未初始化，先填充缓冲区
    if (!FHandle->IsInitialized)
    {
        InitializeBuffer(FHandle, newData);
        FHandle->IsInitialized = true;
    }

    uint8_t oldIndex = FHandle->Index;
    float oldData = FHandle->DataBuffer[oldIndex];
    
    // 更新数据缓冲区
    FHandle->DataBuffer[oldIndex] = newData;
    
    // 更新索引（环形缓冲区）
    FHandle->Index = (oldIndex + 1) % FHandle->WindowSize;

    if (FHandle->OptimizedSort)
    {
        // 优化版本：在已排序数组中移除旧数据并插入新数据
        uint8_t removePos = FindInsertPosition(FHandle->SortedBuffer, 
                                               FHandle->WindowSize, 
                                               oldData);
        
        // 移除旧数据
        RemoveValueFromSortedArray(FHandle->SortedBuffer, 
                                   FHandle->WindowSize, 
                                   removePos);
        
        // 找到新数据的插入位置
        uint8_t insertPos = FindInsertPosition(FHandle->SortedBuffer, 
                                               FHandle->WindowSize - 1, 
                                               newData);
        
        // 插入新数据
        InsertValueToSortedArray(FHandle->SortedBuffer, 
                                 FHandle->WindowSize - 1, 
                                 newData, 
                                 insertPos);
    }
    else
    {
        // 简化版本：直接重新排序（适用于小窗口）
        memcpy(FHandle->SortedBuffer, FHandle->DataBuffer, 
               FHandle->WindowSize * sizeof(float));
        InsertionSort(FHandle->SortedBuffer, FHandle->WindowSize);
    }

    // 返回中值（窗口大小为奇数时取中间值，偶数时取两个中间值的平均）
    if (FHandle->WindowSize % 2 == 1)
    {
        // 奇数窗口，直接返回中间值
        return FHandle->SortedBuffer[FHandle->HalfWindow];
    }
    else
    {
        // 偶数窗口，返回两个中间值的平均值
        uint8_t mid1 = FHandle->HalfWindow - 1;
        uint8_t mid2 = FHandle->HalfWindow;
        return (FHandle->SortedBuffer[mid1] + FHandle->SortedBuffer[mid2]) * 0.5f;
    }
}

/**
  * @brief  重置滤波器
  * @param  FHandle: 滤波器句柄
  * @param  resetValue: 重置值
  */
void Filter_Median_Reset(Filter_Median_Handle_t FHandle, float resetValue)
{
    if (FHandle == NULL)
        return;
    
    InitializeBuffer(FHandle, resetValue);
    FHandle->IsInitialized = true;
}

/**
  * @brief  获取窗口大小
  * @param  FHandle: 滤波器句柄
  * @retval 窗口大小
  */
uint8_t Filter_Median_GetWindowSize(Filter_Median_Handle_t FHandle)
{
    return (FHandle != NULL) ? FHandle->WindowSize : 0;
}

/* ========== 私有函数实现 ========== */

/**
  * @brief  初始化缓冲区
  * @param  FHandle: 滤波器句柄
  * @param  initValue: 初始值
  */
static void InitializeBuffer(Filter_Median_Handle_t FHandle, float initValue)
{
    for (uint8_t i = 0; i < FHandle->WindowSize; i++)
    {
        FHandle->DataBuffer[i] = initValue;
        FHandle->SortedBuffer[i] = initValue;
    }
}

/**
  * @brief  插入排序（适用于小数据量排序）
  * @param  array: 待排序数组
  * @param  size: 数组大小
  */
static void InsertionSort(float *array, uint8_t size)
{
    for (uint8_t i = 1; i < size; i++)
    {
        float key = array[i];
        int8_t j = i - 1;
        
        // 将比key大的元素向后移动
        while (j >= 0 && array[j] > key)
        {
            array[j + 1] = array[j];
            j--;
        }
        array[j + 1] = key;
    }
}

/**
  * @brief  在已排序数组中查找插入位置（二分查找）
  * @param  sortedArray: 已排序数组
  * @param  size: 数组大小
  * @param  value: 要插入的值
  * @retval 插入位置
  */
static uint8_t FindInsertPosition(float *sortedArray, uint8_t size, float value)
{
    uint8_t left = 0;
    uint8_t right = size;
    
    while (left < right)
    {
        uint8_t mid = left + (right - left) / 2;
        
        if (sortedArray[mid] < value)
            left = mid + 1;
        else
            right = mid;
    }
    
    return left;
}

/**
  * @brief  向已排序数组中插入值
  * @param  sortedArray: 已排序数组
  * @param  size: 当前数组大小（插入前）
  * @param  value: 要插入的值
  * @param  position: 插入位置
  */
static void InsertValueToSortedArray(float *sortedArray, uint8_t size, 
                                     float value, uint8_t position)
{
    // 将position及之后的元素向后移动
    for (uint8_t i = size; i > position; i--)
    {
        sortedArray[i] = sortedArray[i - 1];
    }
    
    // 插入新值
    sortedArray[position] = value;
}

/**
  * @brief  从已排序数组中移除值
  * @param  sortedArray: 已排序数组
  * @param  size: 数组大小
  * @param  position: 要移除的位置
  */
static void RemoveValueFromSortedArray(float *sortedArray, uint8_t size, 
                                       uint8_t position)
{
    // 将position之后的元素向前移动
    for (uint8_t i = position; i < size - 1; i++)
    {
        sortedArray[i] = sortedArray[i + 1];
    }
}

/* ========== 卡尔曼滤波器 ========== */

/* ===== 一维卡尔曼滤波器实现 ===== */

Filter_KF1D_Handle_t Filter_KF1D_Init(Filter_KF1D_InitTypeDef *InitStructure)
{
    // 给实例分配内存
    Filter_KF1D_Handle_t FHandle = malloc(sizeof(struct Filter_KF1D_Instance_t));
    if (FHandle == NULL)
        return NULL;

    // 初始化卡尔曼滤波器参数
    FHandle->x = InitStructure->initState;
    FHandle->P = InitStructure->initError;
    FHandle->Q = InitStructure->processNoise;
    FHandle->R = InitStructure->measureNoise;
    FHandle->F = InitStructure->stateTrans;
    FHandle->H = InitStructure->measureCoeff;

    return FHandle;
}

bool Filter_KF1D_DeInit(Filter_KF1D_Handle_t FHandle)
{
    if (FHandle != NULL)
    {
        free(FHandle);
        return true;
    }
    return false;
}

float Filter_KF1D_Update(Filter_KF1D_Handle_t FHandle, float measurement)
{
    // ========== 预测步骤 ==========
    // 1. 状态预测: x_k|k-1 = F * x_k-1|k-1
    FHandle->x = FHandle->F * FHandle->x;

    // 2. 协方差预测: P_k|k-1 = F * P_k-1|k-1 * F^T + Q
    FHandle->P = FHandle->F * FHandle->P * FHandle->F + FHandle->Q;

    // ========== 更新步骤 ==========
    // 1. 计算卡尔曼增益: K = P * H^T / (H * P * H^T + R)
    // 一维情况下，H^T = H，所以简化为:
    float K = (FHandle->P * FHandle->H) / (FHandle->H * FHandle->P * FHandle->H + FHandle->R);

    // 2. 状态更新: x_k|k = x_k|k-1 + K * (z_k - H * x_k|k-1)
    float innovation = measurement - FHandle->H * FHandle->x;
    FHandle->x = FHandle->x + K * innovation;

    // 3. 协方差更新: P_k|k = (I - K * H) * P_k|k-1
    // 一维情况下，I = 1，所以简化为:
    FHandle->P = (1.0f - K * FHandle->H) * FHandle->P;

    return FHandle->x;
}

/* ========== 求导（带时间） ========== */
struct Filter_D_Instance_t
{
    float BeforeSysTime;
    float BeforeValue;
};

Filter_D_Handle_t FIlter_D_Init(Filter_D_InitTypeDef *InitStructure)
{
    Filter_D_Handle_t FHandle = malloc(sizeof(struct Filter_D_Instance_t));
    if (FHandle == NULL)
        return NULL;

    FHandle->BeforeSysTime = InitStructure->InitSysTime;
    FHandle->BeforeValue = InitStructure->InitValue;

    return FHandle;
}

bool Filter_D_DeInit(Filter_D_Handle_t FHandle)
{
    free(FHandle);
    return true;
}

float Filter_D_Update(Filter_D_Handle_t FHandle, float newData, float DeltaTime)//newSysTime)
{
    /* float output = (newData - FHandle->BeforeValue) / (newSysTime - FHandle->BeforeSysTime);
    FHandle->BeforeSysTime = newSysTime;
    FHandle->BeforeValue = newData; */
    float output = (newData - FHandle->BeforeValue) / DeltaTime;
    FHandle->BeforeValue = newData;
    return output;
}

