#ifndef __PARAM_H
#define __PARAM_H
#include <stdint.h>
#include <stdbool.h>
#include "ch32v30x_gpio.h"
// ========================================
// Define
// ========================================
#define KEY_NUM 12
#define KEY_IS_DATA_SIZE 2
#define HEAD_SIZE 2
#define CRC_SIZE 4
#define SAMPLING_INTERVAL_TIME 70  // 用于设定TIM4比较值,TIM4 n us触发一次采样,要小于采样和计算时间
#define HEAD_VALUE 0xABCD

#define ADJUST_MAXADC 0x01
#define ADJUST_MINADC 0x02
#define ADJUST_MAXADC_D 0x03
#define ADJUST_MINADC_D 0x04

#define ADJ_SKIP 50 // 等待稳定而跳过的前采样点个数
#define ADJ_CNT 512

// ========================================
// Variable (Need To Save)
// ========================================

// === Key ===
// 按键弹起按下ADC值 - 用于归一化
extern uint16_t Key_One_Top[KEY_NUM];
extern uint16_t Key_One_Bottom[KEY_NUM];

// 按键状态转换阈值
extern float UPPER_LINE;
extern float LOWER_LINE;

// 最大导数值 - 用于力度映射
extern float maxADC_One_D;
extern float minADC_One_D;

// 每n个扫描周期回报一次
extern uint8_t send_count;

// ========================================
// Other
// ========================================
extern uint32_t Collection_Buffer[(KEY_NUM + 1) / 2 + 1];  // 双ADC采集Buffer
extern uint8_t KeyData_Tx_Buffer[HEAD_SIZE + KEY_IS_DATA_SIZE + KEY_NUM + CRC_SIZE];             // UART5按键回传数据，用于配置DMA2

extern volatile bool isAdjustMode; // 校准模式
extern volatile bool isAdjustFinished; // 校准完成标志位
extern uint8_t AdjustParam; // 待校准参数
extern uint16_t Key_Ready; // 待校准按键
extern float AdjustBuffer[KEY_NUM]; // 校准缓冲区

// 按键ID
typedef enum
{
    Key_ID_11 = 0,
    Key_ID_12,
    Key_ID_13,
    Key_ID_14,
    Key_ID_21,
    Key_ID_22,
    Key_ID_23,
    Key_ID_24,
    Key_ID_31,
    Key_ID_32,
    Key_ID_33,
    Key_ID_34,
    Key_ID_41,
    Key_ID_42,
    Key_ID_43,
    Key_ID_44,
} Key_ID_t;

// 按键霍尔传感器所接引脚 - 默认为16个ADC通道
typedef struct
{
    GPIO_TypeDef *GPIOx;
    uint16_t GPIO_Pin;
} Key_Pin_t;

extern const Key_Pin_t Key_Pin_Group[16];

// i转Key Number
extern uint8_t i2key[KEY_NUM];

void InitParam(void);
void SaveParam(void);
void EraseParam(void);

#endif