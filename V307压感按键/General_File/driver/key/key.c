#include "key.h"
#include "param.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "vofa.h"
#include "debug.h"
#include "system.h"
#include "crc_form.h"

// 按键状态
typedef enum
{
    KEY_STATE_UP = 0,
    KEY_STATE_UP_TO_FALL,
    KEY_STATE_FALL,
    KEY_STATE_FALL_TO_DOWN,
    KEY_STATE_DOWN,
    KEY_STATE_DOWN_TO_RISE,
    KEY_STATE_RISE,
    KEY_STATE_RISE_TO_UP,
} Key_State_t;

// 按键实例
typedef struct
{
    Filter_MA_Handle_t Filter_MA_Handle;
    Filter_Median_Handle_t Filter_Median_Handle;
    Filter_D_Handle_t Filter_D_Handle;
    Filter_MA_Handle_t Filter_MA_Handle2;
    Key_ID_t Key_ID;
    Key_State_t Key_State;
    float Key_maxADC_One_D;
} Key_Instance_t;

uint8_t CalculatePushForce(float ADC_One_D);  // 力度计算函数

// 外部变量
// extern int16_t Calibrattion_Val1,Calibrattion_Val2; // 校准偏移数据

// 全局变量
float K_REF_Prepare = (1.2f * 4095.0f) / 3.3f;  // VREFINT校准倍率预计算
Key_Instance_t *Key_Instance_Group[KEY_NUM];    // 按键实例指针数组
float Key_One_Factor[KEY_NUM] = {0};            // ADC值归一化乘积因数
float CalculateForce_Factor;                    // 将导数值映射到256级的乘积因数
uint8_t count = 0;

// 调试变量
float output[KEY_NUM + 1] = {0};  // VOFA输出

// 按键初始化
void Key_Init(void)
{
    Filter_MA_InitTypeDef Filter_MA_InitStructure = {
        .isUseDataSingle = true,
        .WindowSize = 16,
    };
    Filter_Median_InitTypeDef Filter_Median_InitStructure = {
        .InitData = NULL,
        .isUseDataSingle = true,
        .WindowSize = 11,            // 窗口大小（奇数）
        .EnableOptimizedSort = true  // 启用优化排序
    };
    Filter_D_InitTypeDef Filter_D_Initstructure = {
        .InitSysTime = 0,
    };
    Filter_MA_InitTypeDef Filter_MA_InitStructure2 = {
        .InitDataSingle = 0,
        .isUseDataSingle = true,
        .WindowSize = 2,
    };
    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        // 计算归一化乘积因子
        Key_One_Factor[i] = 1.0 / (Key_One_Top[i] - Key_One_Bottom[i]);

        // 为每个按键创建实例
        Key_Instance_t *Key_Instance_p = malloc(sizeof(Key_Instance_t));

        Key_Instance_p->Key_ID = i;
        Key_Instance_p->Key_State = KEY_STATE_UP;
        Key_Instance_p->Key_maxADC_One_D = 0;

        // 创建滑动平均滤波器
        Filter_MA_InitStructure.InitDataSingle = (int32_t)Key_One_Top[i];
        Filter_MA_Handle_t Filter_MA_Handle = Filter_MA_Init(&Filter_MA_InitStructure);
        Key_Instance_p->Filter_MA_Handle = Filter_MA_Handle;
        // 创建中值滤波器
        Filter_Median_InitStructure.InitDataSingle = (int32_t)Key_One_Top[i];
        Filter_Median_Handle_t Filter_Median_Handle = Filter_Median_Init(&Filter_Median_InitStructure);
        Key_Instance_p->Filter_Median_Handle = Filter_Median_Handle;
        // 创建导数计算器
        Filter_D_Initstructure.InitValue = (int32_t)Key_One_Top[i];
        Filter_D_Handle_t Filter_D_Handle = FIlter_D_Init(&Filter_D_Initstructure);
        Key_Instance_p->Filter_D_Handle = Filter_D_Handle;
        // 创建滑动平均滤波器2
        Filter_MA_InitStructure.InitDataSingle = 0;
        Filter_MA_Handle_t Filter_MA_Handle2 = Filter_MA_Init(&Filter_MA_InitStructure2);
        Key_Instance_p->Filter_MA_Handle2 = Filter_MA_Handle2;

        Key_Instance_Group[i] = Key_Instance_p;  // 存入Key_Instance_Group
    }
    CalculateForce_Factor = 1 / (maxADC_One_D - minADC_One_D);

    KeyData_Tx_Buffer[0] = HEAD_VALUE>>8;
    KeyData_Tx_Buffer[1] = HEAD_VALUE;
    ((uint32_t *)output)[KEY_NUM] = 0x7F800000;
}

void Key_DeInit(void)
{
    memset(KeyData_Tx_Buffer, 0, 2+KEY_NUM);
    for(uint8_t i = 0; i < KEY_NUM; i++)
    {
        free(Key_Instance_Group[i]->Filter_MA_Handle);
        free(Key_Instance_Group[i]->Filter_MA_Handle2);
        free(Key_Instance_Group[i]->Filter_Median_Handle);
        free(Key_Instance_Group[i]->Filter_D_Handle);
        free(Key_Instance_Group[i]);
    }
}

// 按键扫描
void Key_Scan(void)
{
    // TIMER_START_NS();
    ADC1->CTLR2 |= 0x00500000U;                    // 开始数据转换
    while ((DMA1->INTFR & DMA1_IT_TC1) == RESET);  // 等待DMA搬运完成
    DMA1->INTFCR = DMA1_IT_GL1;                    // DMA转换完成标志复位
    // TIMER_STOP_NS("ADC");
}

// 按键数据处理
void Data_Process(void)
{
    // TIMER_START_NS();

    // 计算时间间隔，用于导数计算
    // float SysTickValue = SysTick->CNT/100.0f;
    // VREFINT校准倍率预计算
    float K_REF_ADC1 = K_REF_Prepare / ((uint16_t *)Collection_Buffer)[12];
    float K_REF_ADC2 = K_REF_Prepare / ((uint16_t *)Collection_Buffer)[13];

    for (uint8_t i = 0; i < KEY_NUM; i++)
    {
        // 取按键序号
        uint8_t Key_Index = i2key[i];
        // 取按键实例指针
        Key_Instance_t *Key_Instance_p = Key_Instance_Group[i];
        // VREFINT校准
        float KeyADC = ((uint16_t *)Collection_Buffer)[i] * ((i % 2) ? K_REF_ADC2 : K_REF_ADC1);
        // 滑动平均滤波
        KeyADC = Filter_MA_Update(Key_Instance_p->Filter_MA_Handle, KeyADC);  //((uint16_t *)Collection_Buffer)[i];//
        // 中值滤波
        KeyADC = Filter_Median_Update(Key_Instance_p->Filter_Median_Handle, KeyADC);
        // 归一化（双阈值线性插值）
        KeyADC = (KeyADC > Key_One_Top[i]) ? Key_One_Top[i] : (KeyADC < Key_One_Bottom[i]) ? Key_One_Bottom[i]: KeyADC;  // 阈值约束
        float KeyADC_One = (KeyADC - Key_One_Bottom[i]) * Key_One_Factor[i];                          // 线性映射
        // 计算导数的绝对值
        float KeyADC_One_D = fabsf(Filter_D_Update(Key_Instance_p->Filter_D_Handle, KeyADC_One * 1000.0f, 18 * 60 / 100));
        KeyADC_One_D = Filter_MA_Update(Key_Instance_p->Filter_MA_Handle2, KeyADC_One_D);
        // 按键状态更新
        switch (Key_Instance_p->Key_State)
        {
            case KEY_STATE_UP:
                if (KeyADC_One > UPPER_LINE)
                {
                    break;
                }
                else
                {
                    Key_Instance_p->Key_State = KEY_STATE_UP_TO_FALL;
                    break;
                }
                break;
            case KEY_STATE_UP_TO_FALL:
                if (KeyADC_One > UPPER_LINE)
                {
                    Key_Instance_p->Key_State = KEY_STATE_UP;
                    break;
                }
                else
                {
                    Key_Instance_p->Key_State = KEY_STATE_FALL;
                    Key_Instance_p->Key_maxADC_One_D = KeyADC_One_D;
                    break;
                }
                break;
            case KEY_STATE_FALL:
                if (KeyADC_One > UPPER_LINE)
                {
                    Key_Instance_p->Key_State = KEY_STATE_UP;
                    Key_Instance_p->Key_maxADC_One_D = 0;
                    break;
                }
                else if (KeyADC_One > LOWER_LINE)
                {
                    if (KeyADC_One_D > Key_Instance_p->Key_maxADC_One_D)
                    {
                        Key_Instance_p->Key_maxADC_One_D = KeyADC_One_D;
                    }
                    break;
                }
                else
                {
                    if (KeyADC_One > Key_Instance_p->Key_maxADC_One_D)
                    {
                        Key_Instance_p->Key_maxADC_One_D = KeyADC_One_D;
                    }
                    Key_Instance_p->Key_State = KEY_STATE_FALL_TO_DOWN;
                    break;
                }
                break;
            case KEY_STATE_FALL_TO_DOWN:
                if (KeyADC_One > LOWER_LINE)
                {
                    Key_Instance_p->Key_State = KEY_STATE_FALL;
                    break;
                }
                else
                {
                    if (KeyADC_One > Key_Instance_p->Key_maxADC_One_D)
                    {
                        Key_Instance_p->Key_maxADC_One_D = KeyADC_One_D;
                    }
                    ((uint16_t *)KeyData_Tx_Buffer)[HEAD_SIZE/2] |= (1 << Key_Index);  // 标记被按下
                    KeyData_Tx_Buffer[HEAD_SIZE + KEY_IS_DATA_SIZE + Key_Index] = CalculatePushForce(Key_Instance_p->Key_maxADC_One_D);
                    Key_Instance_p->Key_State = KEY_STATE_DOWN;
                    break;
                }
                break;
            case KEY_STATE_DOWN:
                if (KeyADC_One > LOWER_LINE)
                {
                    Key_Instance_p->Key_State = KEY_STATE_DOWN_TO_RISE;
                    break;
                }
                else
                {
                    break;
                }
                break;
            case KEY_STATE_DOWN_TO_RISE:
                if (KeyADC_One > LOWER_LINE)
                {
                    /* ((uint16_t *)KeyData_Tx_Buffer)[0] &= ~(1 << i);  // 标记被弹起
                    KeyData_Tx_Buffer[2 + i] = 0; */
                    Key_Instance_p->Key_State = KEY_STATE_RISE;
                    break;
                }
                else
                {
                    Key_Instance_p->Key_State = KEY_STATE_DOWN;
                }
                break;
            case KEY_STATE_RISE:
                if (KeyADC_One > UPPER_LINE)
                {
                    Key_Instance_p->Key_State = KEY_STATE_RISE_TO_UP;
                }
                else if (KeyADC_One > LOWER_LINE)
                {
                    break;
                }
                else
                {
                    Key_Instance_p->Key_State = KEY_STATE_DOWN;
                }
                break;
            case KEY_STATE_RISE_TO_UP:
                if (KeyADC_One > UPPER_LINE)
                {
                    ((uint16_t *)KeyData_Tx_Buffer)[HEAD_SIZE/2] &= ~(1 << Key_Index);  // 标记被弹起
                    // KeyData_Tx_Buffer[2 + i] = 0;
                    Key_Instance_p->Key_State = KEY_STATE_UP;
                }
                else
                {
                    Key_Instance_p->Key_State = KEY_STATE_RISE;
                }
                break;
        }
        //output[Key_Index] = KeyADC;//KeyData_Tx_Buffer[2 + Key_Index];//KeyADC_One;// Key_Instance_p->Key_State;//KeyADC_One_D;//
    }
    // TIMER_STOP_NS("ADC2");
    // Vofa_JF_SendFloat(USART1, output, KEY_NUM);
}

// 发送数据
uint8_t UART7_Tx_Buffer[2] = {0};
void Send_Data(void)
{
    if (count == send_count)
    {
        uint32_t crc32 = crc32_calculate(KeyData_Tx_Buffer + HEAD_SIZE, KEY_IS_DATA_SIZE + KEY_NUM);
        KeyData_Tx_Buffer[HEAD_SIZE + KEY_IS_DATA_SIZE + KEY_NUM + 0] = ((uint8_t*)(&crc32))[0];
        KeyData_Tx_Buffer[HEAD_SIZE + KEY_IS_DATA_SIZE + KEY_NUM + 1] = ((uint8_t*)(&crc32))[1];
        KeyData_Tx_Buffer[HEAD_SIZE + KEY_IS_DATA_SIZE + KEY_NUM + 2] = ((uint8_t*)(&crc32))[2];
        KeyData_Tx_Buffer[HEAD_SIZE + KEY_IS_DATA_SIZE + KEY_NUM + 3] = ((uint8_t*)(&crc32))[3];

        while (DMA_GetFlagStatus(DMA2_FLAG_TC4) == RESET);// || DMA_GetFlagStatus(DMA2_FLAG_TC8) == RESET);  // 等待上次DMA传输完成

        // UART5发送
        DMA2_Channel4->CFGR &= (uint16_t)(~DMA_CFGR1_EN);   // 清零使能位
        DMA2_Channel4->CNTR = HEAD_SIZE + KEY_IS_DATA_SIZE + KEY_NUM + CRC_SIZE;//(KEY_NUM + 1)*4; // 重置待发送数据计数值
        DMA2_Channel4->MADDR = (uint32_t)KeyData_Tx_Buffer;//(uint32_t)output;// 重置DMA指针（似乎不用）
        DMA2_Channel4->CFGR |= DMA_CFGR1_EN;                // 置位使能位
        UART5->CTLR3 |= USART_DMAReq_Tx;                    // UART5 发送DMA请求

        // UART7发送
        uint16_t key_bits = ((uint16_t *)KeyData_Tx_Buffer)[HEAD_SIZE / 2] & 0x0FFF;
        uint8_t check4 = ((key_bits >> 0) ^ (key_bits >> 4) ^ (key_bits >> 8)) & 0x0F;
        uint16_t uart7_packet = key_bits | ((uint16_t)check4 << 12);
        UART7_Tx_Buffer[0] = (uint8_t)(uart7_packet & 0xFF);
        UART7_Tx_Buffer[1] = (uint8_t)(uart7_packet >> 8);

        DMA2_Channel8->CFGR &= (uint16_t)(~DMA_CFGR1_EN);   // 清零使能位
        DMA2_Channel8->CNTR = KEY_IS_DATA_SIZE;            // 重置待发送数据计数值
        DMA2_Channel8->MADDR = (uint32_t)UART7_Tx_Buffer;//(KeyDat、_Tx_Buffer + HEAD_SIZE);            // 重置DMA指针（似乎不用）
        DMA2_Channel8->CFGR |= DMA_CFGR1_EN;                // 置位使能位
        UART7->CTLR3 |= USART_DMAReq_Tx;                    // UART7 发送DMA请求a
        
        count = 0;
    }
    count++;
}

// ========================================
// 按键校准部分
// ========================================
uint16_t adj_count = 0;
uint8_t is_start_adjust = 0;

// 校准初始化
void Adjust_Init()
{
    adj_count = 0;
    is_start_adjust = 0;
}

// 校准模式下的ADC数据处理
void Adjust_Process()
{
    switch (AdjustParam)
    {
        case ADJUST_MAXADC:  // 都是测量ADC值后取平均值，只是保存的参数不一样
        case ADJUST_MINADC:
        {
            float K_REF_ADC1 = K_REF_Prepare / ((uint16_t *)Collection_Buffer)[12];
            float K_REF_ADC2 = K_REF_Prepare / ((uint16_t *)Collection_Buffer)[13];

            for (uint8_t i = 0; i < KEY_NUM; i++)
            {
                if ((Key_Ready >> i) & 0x0001)  // 仅计算必要的按键
                {
                    // 取按键实例指针
                    Key_Instance_t *Key_Instance_p = Key_Instance_Group[i];
                    // VREFINT校准
                    float KeyADC = ((uint16_t *)Collection_Buffer)[i] * ((i % 2) ? K_REF_ADC2 : K_REF_ADC1);
                    // 滑动平均滤波
                    KeyADC = Filter_MA_Update(Key_Instance_p->Filter_MA_Handle, KeyADC);  //((uint16_t *)Collection_Buffer)[i];//
                    // 中值滤波
                    KeyADC = Filter_Median_Update(Key_Instance_p->Filter_Median_Handle, KeyADC);

                    // 更新每个按键校准缓冲区
                    if (is_start_adjust)
                    {
                        AdjustBuffer[i] += KeyADC / ADJ_CNT;
                    }
                }
            }
            break;
        }
        default:

            break;
    }
    adj_count++;
    if (adj_count >= ADJ_SKIP && is_start_adjust != 1)  // 稳定期判断
    {
        adj_count = 0;
        is_start_adjust = 1;
    }
    if (adj_count >= ADJ_CNT && is_start_adjust == 1)  // 数据采样完成判断
    {
        isAdjustFinished = true;
        TIM_Cmd(TIM4, DISABLE);  // 关闭扫描
    }
}

// 更新按键数据
void Adjust_Update(void)
{
    switch (AdjustParam)
    {
        case ADJUST_MAXADC:
        {
            for (uint8_t i = 0; i < KEY_NUM; i++)
            {
                if ((Key_Ready >> i) & 0x0001)
                {
                    Key_One_Top[i] = AdjustBuffer[i] + 0.5f;
                    // 重新计算归一化乘积因子
                    Key_One_Factor[i] = 1.0 / (Key_One_Top[i] - Key_One_Bottom[i]);
                }
            }
            break;
        }
        case ADJUST_MINADC:
        {
            for (uint8_t i = 0; i < KEY_NUM; i++)
            {
                if ((Key_Ready >> i) & 0x0001)
                {
                    Key_One_Bottom[i] = AdjustBuffer[i] + 0.5f;
                    // 重新计算归一化乘积因子
                    Key_One_Factor[i] = 1.0 / (Key_One_Top[i] - Key_One_Bottom[i]);
                }
            }
            break;
        }
    }
}

// 计算力度函数 - 双阈值线性映射
uint8_t CalculatePushForce(float ADC_One_D)
{
    if (ADC_One_D >= maxADC_One_D)
    {
        return 255;
    }
    else if (ADC_One_D <= minADC_One_D)
    {
        return 0;
    }
    else
    {
        return (uint8_t)((ADC_One_D - minADC_One_D) * CalculateForce_Factor * 255 + 0.5f);
    }
}