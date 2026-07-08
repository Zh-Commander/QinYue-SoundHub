/********************************** (C) COPYRIGHT  *******************************
 * 文件名             : ch32v30x.h
 * 作者               : WCH
 * 版本               : V1.0.1
 * 日期               : 2025/04/09
 * 描述               : CH32V30x 设备外设访问层头文件。
 *********************************************************************************
 * 版权 (c) 2021 南京沁恒微电子股份有限公司 版权所有。
 * 注意：本软件（修改或未修改）和二进制代码仅可用于南京沁恒微电子股份有限公司制造的微控制器。
 *******************************************************************************/
#ifndef __CH32V30x_H
#define __CH32V30x_H

#ifdef __cplusplus
extern "C"
{
#endif

/* 根据所选设备选择宏定义，未定义时默认为 CH32V30x_D8C */
#if !defined(CH32V30x_D8) && !defined(CH32V30x_D8C)
// #define CH32V30x_D8              /* CH32V303x 系列 */
#define CH32V30x_D8C /* CH32V307x / CH32V305x / CH32V317x 系列 */
#endif

#define __MPU_PRESENT 0          /* 其他 CH32 设备不提供 MPU */
#define __Vendor_SysTickConfig 0 /* 若使用不同的 SysTick 配置，则设为 1 */

#ifndef HSE_VALUE
#define HSE_VALUE ((uint32_t)8000000) /* 外部高速振荡器频率值 (单位: Hz) */
#endif

/* 根据需要调整外部高速振荡器启动超时时间 */
#define HSE_STARTUP_TIMEOUT ((uint16_t)0x1000) /* HSE 启动超时时间 */

#define HSI_VALUE ((uint32_t)8000000) /* 内部高速振荡器频率值 (单位: Hz) */

/* CH32V30x 标准外设库版本号 */
#define __CH32V30x_STDPERIPH_VERSION_MAIN (0x02) /* [15:8] 主版本号 */
#define __CH32V30x_STDPERIPH_VERSION_SUB (0x09)  /* [7:0]  子版本号 */
#define __CH32V30x_STDPERIPH_VERSION ((__CH32V30x_STDPERIPH_VERSION_MAIN << 8) | (__CH32V30x_STDPERIPH_VERSION_SUB << 0))

  /* 中断号定义，根据所选设备 */
  typedef enum IRQn
  {
    /******  RISC-V 处理器异常编号 *******************************************************/
    NonMaskableInt_IRQn = 2, /* 2 不可屏蔽中断                             */
    EXC_IRQn = 3,            /* 3 异常中断                                */
    Ecall_M_Mode_IRQn = 5,   /* 5 机器模式 Ecall 中断                             */
    Ecall_U_Mode_IRQn = 8,   /* 8 用户模式 Ecall 中断                             */
    Break_Point_IRQn = 9,    /* 9 断点中断                              */
    SysTick_IRQn = 12,       /* 12 系统定时器中断                            */
    Software_IRQn = 14,      /* 14 软件中断                                */

    /******  RISC-V 特定中断编号 *********************************************************/
    WWDG_IRQn = 16,            /* 窗口看门狗中断                            */
    PVD_IRQn = 17,             /* 通过 EXTI 线检测的 PVD 中断            */
    TAMPER_IRQn = 18,          /* 篡改中断                                     */
    RTC_IRQn = 19,             /* RTC 全局中断                                 */
    FLASH_IRQn = 20,           /* FLASH 全局中断                               */
    RCC_IRQn = 21,             /* RCC 全局中断                                 */
    EXTI0_IRQn = 22,           /* EXTI 线0中断                                 */
    EXTI1_IRQn = 23,           /* EXTI 线1中断                                 */
    EXTI2_IRQn = 24,           /* EXTI 线2中断                                 */
    EXTI3_IRQn = 25,           /* EXTI 线3中断                                 */
    EXTI4_IRQn = 26,           /* EXTI 线4中断                                 */
    DMA1_Channel1_IRQn = 27,   /* DMA1 通道1 全局中断                      */
    DMA1_Channel2_IRQn = 28,   /* DMA1 通道2 全局中断                      */
    DMA1_Channel3_IRQn = 29,   /* DMA1 通道3 全局中断                      */
    DMA1_Channel4_IRQn = 30,   /* DMA1 通道4 全局中断                      */
    DMA1_Channel5_IRQn = 31,   /* DMA1 通道5 全局中断                      */
    DMA1_Channel6_IRQn = 32,   /* DMA1 通道6 全局中断                      */
    DMA1_Channel7_IRQn = 33,   /* DMA1 通道7 全局中断                      */
    ADC_IRQn = 34,             /* ADC1 和 ADC2 全局中断                       */
    USB_HP_CAN1_TX_IRQn = 35,  /* USB 设备高优先级 或 CAN1 TX 中断       */
    USB_LP_CAN1_RX0_IRQn = 36, /* USB 设备低优先级 或 CAN1 RX0 中断        */
    CAN1_RX1_IRQn = 37,        /* CAN1 RX1 中断                                   */
    CAN1_SCE_IRQn = 38,        /* CAN1 SCE 中断                                   */
    EXTI9_5_IRQn = 39,         /* 外部线[9:5] 中断                        */
    TIM1_BRK_IRQn = 40,        /* TIM1 刹车中断                                 */
    TIM1_UP_IRQn = 41,         /* TIM1 更新中断                                */
    TIM1_TRG_COM_IRQn = 42,    /* TIM1 触发和通信中断               */
    TIM1_CC_IRQn = 43,         /* TIM1 捕获比较中断                       */
    TIM2_IRQn = 44,            /* TIM2 全局中断                                */
    TIM3_IRQn = 45,            /* TIM3 全局中断                                */
    TIM4_IRQn = 46,            /* TIM4 全局中断                                */
    I2C1_EV_IRQn = 47,         /* I2C1 事件中断                                 */
    I2C1_ER_IRQn = 48,         /* I2C1 错误中断                                 */
    I2C2_EV_IRQn = 49,         /* I2C2 事件中断                                 */
    I2C2_ER_IRQn = 50,         /* I2C2 错误中断                                 */
    SPI1_IRQn = 51,            /* SPI1 全局中断                                */
    SPI2_IRQn = 52,            /* SPI2 全局中断                                */
    USART1_IRQn = 53,          /* USART1 全局中断                              */
    USART2_IRQn = 54,          /* USART2 全局中断                              */
    USART3_IRQn = 55,          /* USART3 全局中断                              */
    EXTI15_10_IRQn = 56,       /* 外部线[15:10] 中断                      */
    RTCAlarm_IRQn = 57,        /* 通过 EXTI 线检测的 RTC 闹钟中断                */

#ifdef CH32V30x_D8
    TIM8_BRK_IRQn = 59,        /* TIM8 刹车中断                                 */
    TIM8_UP_IRQn = 60,         /* TIM8 更新中断                                */
    TIM8_TRG_COM_IRQn = 61,    /* TIM8 触发和通信中断               */
    TIM8_CC_IRQn = 62,         /* TIM8 捕获比较中断                       */
    RNG_IRQn = 63,             /* RNG 全局中断                                 */
    SDIO_IRQn = 65,            /* SDIO 全局中断                                */
    TIM5_IRQn = 66,            /* TIM5 全局中断                                */
    SPI3_IRQn = 67,            /* SPI3 全局中断                                */
    UART4_IRQn = 68,           /* UART4 全局中断                               */
    UART5_IRQn = 69,           /* UART5 全局中断                               */
    TIM6_IRQn = 70,            /* TIM6 全局中断                                */
    TIM7_IRQn = 71,            /* TIM7 全局中断                                */
    DMA2_Channel1_IRQn = 72,   /* DMA2 通道1 全局中断                      */
    DMA2_Channel2_IRQn = 73,   /* DMA2 通道2 全局中断                      */
    DMA2_Channel3_IRQn = 74,   /* DMA2 通道3 全局中断                      */
    DMA2_Channel4_IRQn = 75,   /* DMA2 通道4 全局中断                      */
    DMA2_Channel5_IRQn = 76,   /* DMA2 通道5 全局中断                      */
    USBFS_IRQn = 83,           /* USBFS 全局中断                               */
    UART6_IRQn = 87,           /* UART6 全局中断                               */
    UART7_IRQn = 88,           /* UART7 全局中断                               */
    UART8_IRQn = 89,           /* UART8 全局中断                               */
    TIM9_BRK_IRQn = 90,        /* TIM9 刹车中断                                 */
    TIM9_UP_IRQn = 91,         /* TIM9 更新中断                                */
    TIM9_TRG_COM_IRQn = 92,    /* TIM9 触发和通信中断               */
    TIM9_CC_IRQn = 93,         /* TIM9 捕获比较中断                       */
    TIM10_BRK_IRQn = 94,       /* TIM10 刹车中断                                */
    TIM10_UP_IRQn = 95,        /* TIM10 更新中断                               */
    TIM10_TRG_COM_IRQn = 96,   /* TIM10 触发和通信中断              */
    TIM10_CC_IRQn = 97,        /* TIM10 捕获比较中断                       */
    DMA2_Channel6_IRQn = 98,   /* DMA2 通道6 全局中断                      */
    DMA2_Channel7_IRQn = 99,   /* DMA2 通道7 全局中断                      */
    DMA2_Channel8_IRQn = 100,  /* DMA2 通道8 全局中断                      */
    DMA2_Channel9_IRQn = 101,  /* DMA2 通道9 全局中断                      */
    DMA2_Channel10_IRQn = 102, /* DMA2 通道10 全局中断                     */
    DMA2_Channel11_IRQn = 103, /* DMA2 通道11 全局中断                     */

#elif defined(CH32V30x_D8C)
  USBWakeUp_IRQn = 58,       /* USB 设备从挂起唤醒通过 EXTI 线中断 */
  TIM8_BRK_IRQn = 59,        /* TIM8 刹车中断                                 */
  TIM8_UP_IRQn = 60,         /* TIM8 更新中断                                */
  TIM8_TRG_COM_IRQn = 61,    /* TIM8 触发和通信中断               */
  TIM8_CC_IRQn = 62,         /* TIM8 捕获比较中断                       */
  RNG_IRQn = 63,             /* RNG 全局中断                                 */
  SDIO_IRQn = 65,            /* SDIO 全局中断                                */
  TIM5_IRQn = 66,            /* TIM5 全局中断                                */
  SPI3_IRQn = 67,            /* SPI3 全局中断                                */
  UART4_IRQn = 68,           /* UART4 全局中断                               */
  UART5_IRQn = 69,           /* UART5 全局中断                               */
  TIM6_IRQn = 70,            /* TIM6 全局中断                                */
  TIM7_IRQn = 71,            /* TIM7 全局中断                                */
  DMA2_Channel1_IRQn = 72,   /* DMA2 通道1 全局中断                      */
  DMA2_Channel2_IRQn = 73,   /* DMA2 通道2 全局中断                      */
  DMA2_Channel3_IRQn = 74,   /* DMA2 通道3 全局中断                      */
  DMA2_Channel4_IRQn = 75,   /* DMA2 通道4 全局中断                      */
  DMA2_Channel5_IRQn = 76,   /* DMA2 通道5 全局中断                      */
  ETH_IRQn = 77,             /* ETH 全局中断                                 */
  ETH_WKUP_IRQn = 78,        /* ETH 唤醒中断                                 */
  CAN2_TX_IRQn = 79,         /* CAN2 TX 中断                                   */
  CAN2_RX0_IRQn = 80,        /* CAN2 RX0 中断                                  */
  CAN2_RX1_IRQn = 81,        /* CAN2 RX1 中断                                   */
  CAN2_SCE_IRQn = 82,        /* CAN2 SCE 中断                                   */
  USBFS_IRQn = 83,           /* USBFS 全局中断                               */
  USBHSWakeup_IRQn = 84,     /* USBHS 唤醒中断                               */
  USBHS_IRQn = 85,           /* USBHS 全局中断                               */
  DVP_IRQn = 86,             /* DVP 全局中断                                 */
  UART6_IRQn = 87,           /* UART6 全局中断                               */
  UART7_IRQn = 88,           /* UART7 全局中断                               */
  UART8_IRQn = 89,           /* UART8 全局中断                               */
  TIM9_BRK_IRQn = 90,        /* TIM9 刹车中断                                 */
  TIM9_UP_IRQn = 91,         /* TIM9 更新中断                                */
  TIM9_TRG_COM_IRQn = 92,    /* TIM9 触发和通信中断               */
  TIM9_CC_IRQn = 93,         /* TIM9 捕获比较中断                       */
  TIM10_BRK_IRQn = 94,       /* TIM10 刹车中断                                */
  TIM10_UP_IRQn = 95,        /* TIM10 更新中断                               */
  TIM10_TRG_COM_IRQn = 96,   /* TIM10 触发和通信中断              */
  TIM10_CC_IRQn = 97,        /* TIM10 捕获比较中断                       */
  DMA2_Channel6_IRQn = 98,   /* DMA2 通道6 全局中断                      */
  DMA2_Channel7_IRQn = 99,   /* DMA2 通道7 全局中断                      */
  DMA2_Channel8_IRQn = 100,  /* DMA2 通道8 全局中断                      */
  DMA2_Channel9_IRQn = 101,  /* DMA2 通道9 全局中断                      */
  DMA2_Channel10_IRQn = 102, /* DMA2 通道10 全局中断                     */
  DMA2_Channel11_IRQn = 103, /* DMA2 通道11 全局中断                     */

#endif
  } IRQn_Type;

#define HardFault_IRQn EXC_IRQn
#define ADC1_2_IRQn ADC_IRQn

#define SysTicK_IRQn SysTick_IRQn
#define OTG_FS_IRQn USBFS_IRQn
#define OTG_FS_IRQHandler USBFS_IRQHandler

#define USBHD_IRQHandler USBFS_IRQHandler

#define USBOTG_FS USBFSD
#define USBOTG_H_FS USBFSH

#include <stdint.h>
#include "core_riscv.h"
#include "system_ch32v30x.h"

/* 标准外设库旧定义（为兼容旧代码保留） */
#define HSI_Value HSI_VALUE
#define HSE_Value HSE_VALUE
#define HSEStartUp_TimeOut HSE_STARTUP_TIMEOUT

  /* 模数转换器 */
  typedef struct
  {
    __IO uint32_t STATR;   /* 状态寄存器 */
    __IO uint32_t CTLR1;   /* 控制寄存器1 */
    __IO uint32_t CTLR2;   /* 控制寄存器2 */
    __IO uint32_t SAMPTR1; /* 采样时间寄存器1 */
    __IO uint32_t SAMPTR2; /* 采样时间寄存器2 */
    __IO uint32_t IOFR1;   /* 注入通道数据偏移寄存器1 */
    __IO uint32_t IOFR2;   /* 注入通道数据偏移寄存器2 */
    __IO uint32_t IOFR3;   /* 注入通道数据偏移寄存器3 */
    __IO uint32_t IOFR4;   /* 注入通道数据偏移寄存器4 */
    __IO uint32_t WDHTR;   /* 看门狗高阈值寄存器 */
    __IO uint32_t WDLTR;   /* 看门狗低阈值寄存器 */
    __IO uint32_t RSQR1;   /* 规则序列寄存器1 */
    __IO uint32_t RSQR2;   /* 规则序列寄存器2 */
    __IO uint32_t RSQR3;   /* 规则序列寄存器3 */
    __IO uint32_t ISQR;    /* 注入序列寄存器 */
    __IO uint32_t IDATAR1; /* 注入数据寄存器1 */
    __IO uint32_t IDATAR2; /* 注入数据寄存器2 */
    __IO uint32_t IDATAR3; /* 注入数据寄存器3 */
    __IO uint32_t IDATAR4; /* 注入数据寄存器4 */
    __IO uint32_t RDATAR;  /* 规则数据寄存器 */
    uint32_t RESERVED0;    /* 保留 */
    __IO uint32_t AUX;     /* 辅助寄存器 */
  } ADC_TypeDef;

  /* 备份寄存器 */
  typedef struct
  {
    uint32_t RESERVED0;     /* 保留 */
    __IO uint16_t DATAR1;   /* 备份数据寄存器1 */
    uint16_t RESERVED1;     /* 保留 */
    __IO uint16_t DATAR2;   /* 备份数据寄存器2 */
    uint16_t RESERVED2;     /* 保留 */
    __IO uint16_t DATAR3;   /* 备份数据寄存器3 */
    uint16_t RESERVED3;     /* 保留 */
    __IO uint16_t DATAR4;   /* 备份数据寄存器4 */
    uint16_t RESERVED4;     /* 保留 */
    __IO uint16_t DATAR5;   /* 备份数据寄存器5 */
    uint16_t RESERVED5;     /* 保留 */
    __IO uint16_t DATAR6;   /* 备份数据寄存器6 */
    uint16_t RESERVED6;     /* 保留 */
    __IO uint16_t DATAR7;   /* 备份数据寄存器7 */
    uint16_t RESERVED7;     /* 保留 */
    __IO uint16_t DATAR8;   /* 备份数据寄存器8 */
    uint16_t RESERVED8;     /* 保留 */
    __IO uint16_t DATAR9;   /* 备份数据寄存器9 */
    uint16_t RESERVED9;     /* 保留 */
    __IO uint16_t DATAR10;  /* 备份数据寄存器10 */
    uint16_t RESERVED10;    /* 保留 */
    __IO uint16_t OCTLR;    /* 输出控制寄存器 */
    uint16_t RESERVED11;    /* 保留 */
    __IO uint16_t TPCTLR;   /* 篡改引脚控制寄存器 */
    uint16_t RESERVED12;    /* 保留 */
    __IO uint16_t TPCSR;    /* 篡改引脚控制/状态寄存器 */
    uint16_t RESERVED13[5]; /* 保留 */
    __IO uint16_t DATAR11;  /* 备份数据寄存器11 */
    uint16_t RESERVED14;    /* 保留 */
    __IO uint16_t DATAR12;  /* 备份数据寄存器12 */
    uint16_t RESERVED15;    /* 保留 */
    __IO uint16_t DATAR13;  /* 备份数据寄存器13 */
    uint16_t RESERVED16;    /* 保留 */
    __IO uint16_t DATAR14;  /* 备份数据寄存器14 */
    uint16_t RESERVED17;    /* 保留 */
    __IO uint16_t DATAR15;  /* 备份数据寄存器15 */
    uint16_t RESERVED18;    /* 保留 */
    __IO uint16_t DATAR16;  /* 备份数据寄存器16 */
    uint16_t RESERVED19;    /* 保留 */
    __IO uint16_t DATAR17;  /* 备份数据寄存器17 */
    uint16_t RESERVED20;    /* 保留 */
    __IO uint16_t DATAR18;  /* 备份数据寄存器18 */
    uint16_t RESERVED21;    /* 保留 */
    __IO uint16_t DATAR19;  /* 备份数据寄存器19 */
    uint16_t RESERVED22;    /* 保留 */
    __IO uint16_t DATAR20;  /* 备份数据寄存器20 */
    uint16_t RESERVED23;    /* 保留 */
    __IO uint16_t DATAR21;  /* 备份数据寄存器21 */
    uint16_t RESERVED24;    /* 保留 */
    __IO uint16_t DATAR22;  /* 备份数据寄存器22 */
    uint16_t RESERVED25;    /* 保留 */
    __IO uint16_t DATAR23;  /* 备份数据寄存器23 */
    uint16_t RESERVED26;    /* 保留 */
    __IO uint16_t DATAR24;  /* 备份数据寄存器24 */
    uint16_t RESERVED27;    /* 保留 */
    __IO uint16_t DATAR25;  /* 备份数据寄存器25 */
    uint16_t RESERVED28;    /* 保留 */
    __IO uint16_t DATAR26;  /* 备份数据寄存器26 */
    uint16_t RESERVED29;    /* 保留 */
    __IO uint16_t DATAR27;  /* 备份数据寄存器27 */
    uint16_t RESERVED30;    /* 保留 */
    __IO uint16_t DATAR28;  /* 备份数据寄存器28 */
    uint16_t RESERVED31;    /* 保留 */
    __IO uint16_t DATAR29;  /* 备份数据寄存器29 */
    uint16_t RESERVED32;    /* 保留 */
    __IO uint16_t DATAR30;  /* 备份数据寄存器30 */
    uint16_t RESERVED33;    /* 保留 */
    __IO uint16_t DATAR31;  /* 备份数据寄存器31 */
    uint16_t RESERVED34;    /* 保留 */
    __IO uint16_t DATAR32;  /* 备份数据寄存器32 */
    uint16_t RESERVED35;    /* 保留 */
    __IO uint16_t DATAR33;  /* 备份数据寄存器33 */
    uint16_t RESERVED36;    /* 保留 */
    __IO uint16_t DATAR34;  /* 备份数据寄存器34 */
    uint16_t RESERVED37;    /* 保留 */
    __IO uint16_t DATAR35;  /* 备份数据寄存器35 */
    uint16_t RESERVED38;    /* 保留 */
    __IO uint16_t DATAR36;  /* 备份数据寄存器36 */
    uint16_t RESERVED39;    /* 保留 */
    __IO uint16_t DATAR37;  /* 备份数据寄存器37 */
    uint16_t RESERVED40;    /* 保留 */
    __IO uint16_t DATAR38;  /* 备份数据寄存器38 */
    uint16_t RESERVED41;    /* 保留 */
    __IO uint16_t DATAR39;  /* 备份数据寄存器39 */
    uint16_t RESERVED42;    /* 保留 */
    __IO uint16_t DATAR40;  /* 备份数据寄存器40 */
    uint16_t RESERVED43;    /* 保留 */
    __IO uint16_t DATAR41;  /* 备份数据寄存器41 */
    uint16_t RESERVED44;    /* 保留 */
    __IO uint16_t DATAR42;  /* 备份数据寄存器42 */
    uint16_t RESERVED45;    /* 保留 */
  } BKP_TypeDef;

  /* 控制器局域网 Tx 邮箱 */
  typedef struct
  {
    __IO uint32_t TXMIR;  /* 发送邮箱标识符寄存器 */
    __IO uint32_t TXMDTR; /* 发送邮箱数据长度和时间寄存器 */
    __IO uint32_t TXMDLR; /* 发送邮箱低字节数据寄存器 */
    __IO uint32_t TXMDHR; /* 发送邮箱高字节数据寄存器 */
  } CAN_TxMailBox_TypeDef;

  /* 控制器局域网 FIFO 邮箱 */
  typedef struct
  {
    __IO uint32_t RXMIR;  /* 接收邮箱标识符寄存器 */
    __IO uint32_t RXMDTR; /* 接收邮箱数据长度和时间寄存器 */
    __IO uint32_t RXMDLR; /* 接收邮箱低字节数据寄存器 */
    __IO uint32_t RXMDHR; /* 接收邮箱高字节数据寄存器 */
  } CAN_FIFOMailBox_TypeDef;

  /* 控制器局域网 过滤器寄存器 */
  typedef struct
  {
    __IO uint32_t FR1; /* 过滤器寄存器1 (32位) */
    __IO uint32_t FR2; /* 过滤器寄存器2 (32位) */
  } CAN_FilterRegister_TypeDef;

  /* 控制器局域网 */
  typedef struct
  {
    __IO uint32_t CTLR;                             /* 控制寄存器 */
    __IO uint32_t STATR;                            /* 状态寄存器 */
    __IO uint32_t TSTATR;                           /* 发送状态寄存器 */
    __IO uint32_t RFIFO0;                           /* 接收FIFO0寄存器 */
    __IO uint32_t RFIFO1;                           /* 接收FIFO1寄存器 */
    __IO uint32_t INTENR;                           /* 中断使能寄存器 */
    __IO uint32_t ERRSR;                            /* 错误状态寄存器 */
    __IO uint32_t BTIMR;                            /* 位时间寄存器 */
    uint32_t RESERVED0[88];                         /* 保留 */
    CAN_TxMailBox_TypeDef sTxMailBox[3];            /* 3个发送邮箱 */
    CAN_FIFOMailBox_TypeDef sFIFOMailBox[2];        /* 2个接收FIFO邮箱 */
    uint32_t RESERVED1[12];                         /* 保留 */
    __IO uint32_t FCTLR;                            /* 过滤器控制寄存器 */
    __IO uint32_t FMCFGR;                           /* 过滤器模式配置寄存器 */
    uint32_t RESERVED2;                             /* 保留 */
    __IO uint32_t FSCFGR;                           /* 过滤器尺度配置寄存器 */
    uint32_t RESERVED3;                             /* 保留 */
    __IO uint32_t FAFIFOR;                          /* 过滤器FIFO关联寄存器 */
    uint32_t RESERVED4;                             /* 保留 */
    __IO uint32_t FWR;                              /* 过滤器工作寄存器 */
    uint32_t RESERVED5[8];                          /* 保留 */
    CAN_FilterRegister_TypeDef sFilterRegister[28]; /* 28个过滤器组 */
  } CAN_TypeDef;

  /* CRC 计算单元 */
  typedef struct
  {
    __IO uint32_t DATAR; /* 数据寄存器 */
    __IO uint8_t IDATAR; /* 独立数据寄存器 (8位) */
    uint8_t RESERVED0;   /* 保留 */
    uint16_t RESERVED1;  /* 保留 */
    __IO uint32_t CTLR;  /* 控制寄存器 */
  } CRC_TypeDef;

  /* 数模转换器 */
  typedef struct
  {
    __IO uint32_t CTLR;     /* 控制寄存器 */
    __IO uint32_t SWTR;     /* 软件触发寄存器 */
    __IO uint32_t R12BDHR1; /* 通道1 12位右对齐数据保持寄存器 */
    __IO uint32_t L12BDHR1; /* 通道1 12位左对齐数据保持寄存器 */
    __IO uint32_t R8BDHR1;  /* 通道1 8位右对齐数据保持寄存器 */
    __IO uint32_t R12BDHR2; /* 通道2 12位右对齐数据保持寄存器 */
    __IO uint32_t L12BDHR2; /* 通道2 12位左对齐数据保持寄存器 */
    __IO uint32_t R8BDHR2;  /* 通道2 8位右对齐数据保持寄存器 */
    __IO uint32_t RD12BDHR; /* 双通道 12位右对齐数据保持寄存器 */
    __IO uint32_t LD12BDHR; /* 双通道 12位左对齐数据保持寄存器 */
    __IO uint32_t RD8BDHR;  /* 双通道 8位右对齐数据保持寄存器 */
    __IO uint32_t DOR1;     /* 通道1 数据输出寄存器 */
    __IO uint32_t DOR2;     /* 通道2 数据输出寄存器 */
  } DAC_TypeDef;

  /* DMA 通道控制器 */
  typedef struct
  {
    __IO uint32_t CFGR;  /* 配置寄存器 */
    __IO uint32_t CNTR;  /* 计数器寄存器 */
    __IO uint32_t PADDR; /* 外设地址寄存器 */
    __IO uint32_t MADDR; /* 存储器地址寄存器 */
  } DMA_Channel_TypeDef;

  /* DMA 控制器 */
  typedef struct
  {
    __IO uint32_t INTFR;  /* 中断标志寄存器 */
    __IO uint32_t INTFCR; /* 中断标志清除寄存器 */
  } DMA_TypeDef;

  /* 外部中断/事件控制器 */
  typedef struct
  {
    __IO uint32_t INTENR; /* 中断使能寄存器 */
    __IO uint32_t EVENR;  /* 事件使能寄存器 */
    __IO uint32_t RTENR;  /* 上升沿触发使能寄存器 */
    __IO uint32_t FTENR;  /* 下降沿触发使能寄存器 */
    __IO uint32_t SWIEVR; /* 软件中断事件寄存器 */
    __IO uint32_t INTFR;  /* 中断挂起寄存器 */
  } EXTI_TypeDef;

  /* FLASH 寄存器 */
  typedef struct
  {
    __IO uint32_t ACTLR;    /* 访问控制寄存器 */
    __IO uint32_t KEYR;     /* 解锁键寄存器 */
    __IO uint32_t OBKEYR;   /* 选项字节解锁键寄存器 */
    __IO uint32_t STATR;    /* 状态寄存器 */
    __IO uint32_t CTLR;     /* 控制寄存器 */
    __IO uint32_t ADDR;     /* 地址寄存器 */
    __IO uint32_t RESERVED; /* 保留 */
    __IO uint32_t OBR;      /* 选项字节寄存器 */
    __IO uint32_t WPR;      /* 写保护寄存器 */
    __IO uint32_t MODEKEYR; /* 模式解锁键寄存器 */
  } FLASH_TypeDef;

  /* 选项字节寄存器 */
  typedef struct
  {
    __IO uint16_t RDPR;  /* 读保护寄存器 */
    __IO uint16_t USER;  /* 用户选项字节 */
    __IO uint16_t Data0; /* 数据0 */
    __IO uint16_t Data1; /* 数据1 */
    __IO uint16_t WRPR0; /* 写保护0 */
    __IO uint16_t WRPR1; /* 写保护1 */
    __IO uint16_t WRPR2; /* 写保护2 */
    __IO uint16_t WRPR3; /* 写保护3 */
  } OB_TypeDef;

  /* FSMC Bank1 寄存器 */
  typedef struct
  {
    __IO uint32_t BTCR[8]; /* 总线时序寄存器 (BANK1 的 NOR/PSRAM 控制时序) */
  } FSMC_Bank1_TypeDef;

  /* FSMC Bank1E 寄存器 */
  typedef struct
  {
    __IO uint32_t BWTR[7]; /* 总线写时序寄存器 (BANK1 的扩展时序) */
  } FSMC_Bank1E_TypeDef;

  /* FSMC Bank2 寄存器 */
  typedef struct
  {
    __IO uint32_t PCR2;  /* PC卡控制器寄存器2 (BANK2 控制寄存器) */
    __IO uint32_t SR2;   /* 状态寄存器2 */
    __IO uint32_t PMEM2; /* 通用存储器时序寄存器2 */
    __IO uint32_t PATT2; /* 属性存储器时序寄存器2 */
    uint32_t RESERVED0;  /* 保留 */
    __IO uint32_t ECCR2; /* ECC 结果寄存器2 */
  } FSMC_Bank2_TypeDef;

  /* 通用目的 I/O */
  typedef struct
  {
    __IO uint32_t CFGLR; /* 配置低寄存器 (引脚0-7) */
    __IO uint32_t CFGHR; /* 配置高寄存器 (引脚8-15) */
    __IO uint32_t INDR;  /* 输入数据寄存器 */
    __IO uint32_t OUTDR; /* 输出数据寄存器 */
    __IO uint32_t BSHR;  /* 位设置/清除寄存器 */
    __IO uint32_t BCR;   /* 位清除寄存器 */
    __IO uint32_t LCKR;  /* 锁定寄存器 */
  } GPIO_TypeDef;

  /* 复用功能 I/O */
  typedef struct
  {
    __IO uint32_t ECR;       /* 事件控制寄存器 */
    __IO uint32_t PCFR1;     /* 复用功能配置寄存器1 */
    __IO uint32_t EXTICR[4]; /* 外部中断配置寄存器 (4个，共16条线) */
    uint32_t RESERVED0;      /* 保留 */
    __IO uint32_t PCFR2;     /* 复用功能配置寄存器2 */
  } AFIO_TypeDef;

  /* 内部集成电路接口 */
  typedef struct
  {
    __IO uint16_t CTLR1;  /* 控制寄存器1 */
    uint16_t RESERVED0;   /* 保留 */
    __IO uint16_t CTLR2;  /* 控制寄存器2 */
    uint16_t RESERVED1;   /* 保留 */
    __IO uint16_t OADDR1; /* 自身地址寄存器1 */
    uint16_t RESERVED2;   /* 保留 */
    __IO uint16_t OADDR2; /* 自身地址寄存器2 */
    uint16_t RESERVED3;   /* 保留 */
    __IO uint16_t DATAR;  /* 数据寄存器 */
    uint16_t RESERVED4;   /* 保留 */
    __IO uint16_t STAR1;  /* 状态寄存器1 */
    uint16_t RESERVED5;   /* 保留 */
    __IO uint16_t STAR2;  /* 状态寄存器2 */
    uint16_t RESERVED6;   /* 保留 */
    __IO uint16_t CKCFGR; /* 时钟配置寄存器 */
    uint16_t RESERVED7;   /* 保留 */
    __IO uint16_t RTR;    /* 上升时间寄存器 */
    uint16_t RESERVED8;   /* 保留 */
  } I2C_TypeDef;

  /* 独立看门狗 */
  typedef struct
  {
    __IO uint32_t CTLR;  /* 控制寄存器 */
    __IO uint32_t PSCR;  /* 预分频寄存器 */
    __IO uint32_t RLDR;  /* 重装载寄存器 */
    __IO uint32_t STATR; /* 状态寄存器 */
  } IWDG_TypeDef;

  /* 电源控制 */
  typedef struct
  {
    __IO uint32_t CTLR; /* 控制寄存器 */
    __IO uint32_t CSR;  /* 控制/状态寄存器 */
  } PWR_TypeDef;

  /* 复位和时钟控制 */
  typedef struct
  {
    __IO uint32_t CTLR;      /* 控制寄存器 (时钟控制) */
    __IO uint32_t CFGR0;     /* 配置寄存器0 */
    __IO uint32_t INTR;      /* 中断寄存器 */
    __IO uint32_t APB2PRSTR; /* APB2 外设复位寄存器 */
    __IO uint32_t APB1PRSTR; /* APB1 外设复位寄存器 */
    __IO uint32_t AHBPCENR;  /* AHB 外设时钟使能寄存器 */
    __IO uint32_t APB2PCENR; /* APB2 外设时钟使能寄存器 */
    __IO uint32_t APB1PCENR; /* APB1 外设时钟使能寄存器 */
    __IO uint32_t BDCTLR;    /* 备份域控制寄存器 */
    __IO uint32_t RSTSCKR;   /* 复位/时钟状态寄存器 */
    __IO uint32_t AHBRSTR;   /* AHB 外设复位寄存器 */
    __IO uint32_t CFGR2;     /* 配置寄存器2 */
  } RCC_TypeDef;

  /* 实时时钟 */
  typedef struct
  {
    __IO uint16_t CTLRH; /* 控制寄存器高 */
    uint16_t RESERVED0;  /* 保留 */
    __IO uint16_t CTLRL; /* 控制寄存器低 */
    uint16_t RESERVED1;  /* 保留 */
    __IO uint16_t PSCRH; /* 预分频器高 */
    uint16_t RESERVED2;  /* 保留 */
    __IO uint16_t PSCRL; /* 预分频器低 */
    uint16_t RESERVED3;  /* 保留 */
    __IO uint16_t DIVH;  /* 分频器高 */
    uint16_t RESERVED4;  /* 保留 */
    __IO uint16_t DIVL;  /* 分频器低 */
    uint16_t RESERVED5;  /* 保留 */
    __IO uint16_t CNTH;  /* 计数器高 */
    uint16_t RESERVED6;  /* 保留 */
    __IO uint16_t CNTL;  /* 计数器低 */
    uint16_t RESERVED7;  /* 保留 */
    __IO uint16_t ALRMH; /* 闹钟高 */
    uint16_t RESERVED8;  /* 保留 */
    __IO uint16_t ALRML; /* 闹钟低 */
    uint16_t RESERVED9;  /* 保留 */
  } RTC_TypeDef;

  /* SDIO 寄存器 */
  typedef struct
  {
    __IO uint32_t POWER;   /* 电源控制寄存器 */
    __IO uint32_t CLKCR;   /* 时钟控制寄存器 */
    __IO uint32_t ARG;     /* 命令参数字节寄存器 */
    __IO uint32_t CMD;     /* 命令寄存器 */
    __I uint32_t RESPCMD;  /* 响应命令寄存器 */
    __I uint32_t RESP1;    /* 响应寄存器1 */
    __I uint32_t RESP2;    /* 响应寄存器2 */
    __I uint32_t RESP3;    /* 响应寄存器3 */
    __I uint32_t RESP4;    /* 响应寄存器4 */
    __IO uint32_t DTIMER;  /* 数据超时寄存器 */
    __IO uint32_t DLEN;    /* 数据长度寄存器 */
    __IO uint32_t DCTRL;   /* 数据控制寄存器 */
    __I uint32_t DCOUNT;   /* 数据计数器 */
    __I uint32_t STA;      /* 状态寄存器 */
    __IO uint32_t ICR;     /* 中断清除寄存器 */
    __IO uint32_t MASK;    /* 中断掩码寄存器 */
    uint32_t RESERVED0[2]; /* 保留 */
    __I uint32_t FIFOCNT;  /* FIFO 计数器 */
    uint32_t RESERVED1[5]; /* 保留 */
    __IO uint32_t DCTRL2;  /* 数据控制寄存器2 */
    uint32_t RESERVED2[7]; /* 保留 */
    __IO uint32_t FIFO;    /* FIFO 数据寄存器 (突发访问) */
  } SDIO_TypeDef;

  /* 串行外设接口 */
  typedef struct
  {
    __IO uint16_t CTLR1;   /* 控制寄存器1 */
    uint16_t RESERVED0;    /* 保留 */
    __IO uint16_t CTLR2;   /* 控制寄存器2 */
    uint16_t RESERVED1;    /* 保留 */
    __IO uint16_t STATR;   /* 状态寄存器 */
    uint16_t RESERVED2;    /* 保留 */
    __IO uint16_t DATAR;   /* 数据寄存器 */
    uint16_t RESERVED3;    /* 保留 */
    __IO uint16_t CRCR;    /* CRC 多项式寄存器 */
    uint16_t RESERVED4;    /* 保留 */
    __IO uint16_t RCRCR;   /* 接收 CRC 寄存器 */
    uint16_t RESERVED5;    /* 保留 */
    __IO uint16_t TCRCR;   /* 发送 CRC 寄存器 */
    uint16_t RESERVED6;    /* 保留 */
    __IO uint16_t I2SCFGR; /* I2S 配置寄存器 */
    uint16_t RESERVED7;    /* 保留 */
    __IO uint16_t I2SPR;   /* I2S 预分频寄存器 */
    uint16_t RESERVED8;    /* 保留 */
    __IO uint16_t HSCR;    /* HSCR 寄存器 (硬件流控) */
    uint16_t RESERVED9;    /* 保留 */
  } SPI_TypeDef;

  /* TIM 定时器 */
  typedef struct
  {
    __IO uint16_t CTLR1;     /* 控制寄存器1 */
    uint16_t RESERVED0;      /* 保留 */
    __IO uint16_t CTLR2;     /* 控制寄存器2 */
    uint16_t RESERVED1;      /* 保留 */
    __IO uint16_t SMCFGR;    /* 从模式控制寄存器 */
    uint16_t RESERVED2;      /* 保留 */
    __IO uint16_t DMAINTENR; /* DMA/中断使能寄存器 */
    uint16_t RESERVED3;      /* 保留 */
    __IO uint16_t INTFR;     /* 中断标志寄存器 */
    uint16_t RESERVED4;      /* 保留 */
    __IO uint16_t SWEVGR;    /* 事件生成寄存器 */
    uint16_t RESERVED5;      /* 保留 */
    __IO uint16_t CHCTLR1;   /* 捕获/比较模式寄存器1 (通道1-2) */
    uint16_t RESERVED6;      /* 保留 */
    __IO uint16_t CHCTLR2;   /* 捕获/比较模式寄存器2 (通道3-4) */
    uint16_t RESERVED7;      /* 保留 */
    __IO uint16_t CCER;      /* 捕获/比较使能寄存器 */
    uint16_t RESERVED8;      /* 保留 */
    __IO uint16_t CNT;       /* 计数器 */
    uint16_t RESERVED9;      /* 保留 */
    __IO uint16_t PSC;       /* 预分频器 */
    uint16_t RESERVED10;     /* 保留 */
    __IO uint16_t ATRLR;     /* 自动重装载寄存器 */
    uint16_t RESERVED11;     /* 保留 */
    __IO uint16_t RPTCR;     /* 重复计数器寄存器 */
    uint16_t RESERVED12;     /* 保留 */
    __IO uint16_t CH1CVR;    /* 捕获/比较寄存器1 (通道1) */
    uint16_t RESERVED13;     /* 保留 */
    __IO uint16_t CH2CVR;    /* 捕获/比较寄存器2 (通道2) */
    uint16_t RESERVED14;     /* 保留 */
    __IO uint16_t CH3CVR;    /* 捕获/比较寄存器3 (通道3) */
    uint16_t RESERVED15;     /* 保留 */
    __IO uint16_t CH4CVR;    /* 捕获/比较寄存器4 (通道4) */
    uint16_t RESERVED16;     /* 保留 */
    __IO uint16_t BDTR;      /* 刹车和死区寄存器 */
    uint16_t RESERVED17;     /* 保留 */
    __IO uint16_t DMACFGR;   /* DMA 控制寄存器 */
    uint16_t RESERVED18;     /* 保留 */
    __IO uint16_t DMAADR;    /* DMA 地址寄存器 */
    uint16_t RESERVED19;     /* 保留 */
    __IO uint16_t AUX;       /* 辅助寄存器 */
    uint16_t RESERVED20;     /* 保留 */
  } TIM_TypeDef;

  /* 通用同步异步收发器 */
  typedef struct
  {
    __IO uint16_t STATR; /* 状态寄存器 */
    uint16_t RESERVED0;  /* 保留 */
    __IO uint16_t DATAR; /* 数据寄存器 */
    uint16_t RESERVED1;  /* 保留 */
    __IO uint16_t BRR;   /* 波特率寄存器 */
    uint16_t RESERVED2;  /* 保留 */
    __IO uint16_t CTLR1; /* 控制寄存器1 */
    uint16_t RESERVED3;  /* 保留 */
    __IO uint16_t CTLR2; /* 控制寄存器2 */
    uint16_t RESERVED4;  /* 保留 */
    __IO uint16_t CTLR3; /* 控制寄存器3 */
    uint16_t RESERVED5;  /* 保留 */
    __IO uint16_t GPR;   /* 保护时间和预分频寄存器 */
    uint16_t RESERVED6;  /* 保留 */
    __IO uint16_t CTLR4; /* 控制寄存器4 */
    uint16_t RESERVED7;  /* 保留 */
  } USART_TypeDef;

  /* 窗口看门狗 */
  typedef struct
  {
    __IO uint32_t CTLR;  /* 控制寄存器 */
    __IO uint32_t CFGR;  /* 配置寄存器 */
    __IO uint32_t STATR; /* 状态寄存器 */
  } WWDG_TypeDef;

  /* 增强功能寄存器 */
  typedef struct
  {
    __IO uint32_t EXTEN_CTR;  /* 扩展控制寄存器 */
    uint32_t RESERVED0;       /* 保留 */
    __IO uint32_t EXTEN_CTR2; /* 扩展控制寄存器2 */
  } EXTEN_TypeDef;

  /* OPA 运算放大器寄存器 */
  typedef struct
  {
    __IO uint32_t CR; /* 控制寄存器 */
  } OPA_TypeDef;

  /* RNG 随机数发生器寄存器 */
  typedef struct
  {
    __IO uint32_t CR; /* 控制寄存器 */
    __IO uint32_t SR; /* 状态寄存器 */
    __IO uint32_t DR; /* 数据寄存器 */
  } RNG_TypeDef;

  /* DVP 数字摄像头接口寄存器 */
  typedef struct
  {
    __IO uint8_t CR0;        /* 控制寄存器0 */
    __IO uint8_t CR1;        /* 控制寄存器1 */
    __IO uint8_t IER;        /* 中断使能寄存器 */
    __IO uint8_t Reserved0;  /* 保留 */
    __IO uint16_t ROW_NUM;   /* 行数 (有效行数) */
    __IO uint16_t COL_NUM;   /* 列数 (有效列数) */
    __IO uint32_t DMA_BUF0;  /* DMA 缓冲区0 地址 */
    __IO uint32_t DMA_BUF1;  /* DMA 缓冲区1 地址 */
    __IO uint8_t IFR;        /* 中断标志寄存器 */
    __IO uint8_t STATUS;     /* 状态寄存器 */
    __IO uint16_t Reserved1; /* 保留 */
    __IO uint16_t ROW_CNT;   /* 行计数器 (已接收的行数) */
    __IO uint16_t Reserved2; /* 保留 */
    __IO uint16_t HOFFCNT;   /* 水平偏移计数 */
    __IO uint16_t VST;       /* 垂直开始 (垂直同步后需要跳过的行数) */
    __IO uint16_t CAPCNT;    /* 捕获计数 (有效行计数) */
    __IO uint16_t VLINE;     /* 垂直行数 (每帧总行数) */
    __IO uint32_t DR;        /* 数据寄存器 (FIFO 读取) */
  } DVP_TypeDef;

  /* USBHS 设备寄存器 */
  typedef struct
  {
    __IO uint8_t CONTROL;        /* 控制寄存器 */
    __IO uint8_t HOST_CTRL;      /* 主机控制寄存器 */
    __IO uint8_t INT_EN;         /* 中断使能寄存器 */
    __IO uint8_t DEV_AD;         /* 设备地址寄存器 */
    __IO uint16_t FRAME_NO;      /* 帧号寄存器 */
    __IO uint8_t SUSPEND;        /* 挂起控制寄存器 */
    __IO uint8_t RESERVED0;      /* 保留 */
    __IO uint8_t SPEED_TYPE;     /* 速度类型寄存器 */
    __IO uint8_t MIS_ST;         /* 杂项状态寄存器 */
    __IO uint8_t INT_FG;         /* 中断标志寄存器 */
    __IO uint8_t INT_ST;         /* 中断状态寄存器 */
    __IO uint16_t RX_LEN;        /* 接收长度寄存器 */
    __IO uint16_t RESERVED1;     /* 保留 */
    __IO uint32_t ENDP_CONFIG;   /* 端点配置寄存器 */
    __IO uint32_t ENDP_TYPE;     /* 端点类型寄存器 */
    __IO uint32_t BUF_MODE;      /* 缓冲区模式寄存器 */
    __IO uint32_t UEP0_DMA;      /* 端点0 DMA 地址 */
    __IO uint32_t UEP1_RX_DMA;   /* 端点1 RX DMA 地址 */
    __IO uint32_t UEP2_RX_DMA;   /* 端点2 RX DMA 地址 */
    __IO uint32_t UEP3_RX_DMA;   /* 端点3 RX DMA 地址 */
    __IO uint32_t UEP4_RX_DMA;   /* 端点4 RX DMA 地址 */
    __IO uint32_t UEP5_RX_DMA;   /* 端点5 RX DMA 地址 */
    __IO uint32_t UEP6_RX_DMA;   /* 端点6 RX DMA 地址 */
    __IO uint32_t UEP7_RX_DMA;   /* 端点7 RX DMA 地址 */
    __IO uint32_t UEP8_RX_DMA;   /* 端点8 RX DMA 地址 */
    __IO uint32_t UEP9_RX_DMA;   /* 端点9 RX DMA 地址 */
    __IO uint32_t UEP10_RX_DMA;  /* 端点10 RX DMA 地址 */
    __IO uint32_t UEP11_RX_DMA;  /* 端点11 RX DMA 地址 */
    __IO uint32_t UEP12_RX_DMA;  /* 端点12 RX DMA 地址 */
    __IO uint32_t UEP13_RX_DMA;  /* 端点13 RX DMA 地址 */
    __IO uint32_t UEP14_RX_DMA;  /* 端点14 RX DMA 地址 */
    __IO uint32_t UEP15_RX_DMA;  /* 端点15 RX DMA 地址 */
    __IO uint32_t UEP1_TX_DMA;   /* 端点1 TX DMA 地址 */
    __IO uint32_t UEP2_TX_DMA;   /* 端点2 TX DMA 地址 */
    __IO uint32_t UEP3_TX_DMA;   /* 端点3 TX DMA 地址 */
    __IO uint32_t UEP4_TX_DMA;   /* 端点4 TX DMA 地址 */
    __IO uint32_t UEP5_TX_DMA;   /* 端点5 TX DMA 地址 */
    __IO uint32_t UEP6_TX_DMA;   /* 端点6 TX DMA 地址 */
    __IO uint32_t UEP7_TX_DMA;   /* 端点7 TX DMA 地址 */
    __IO uint32_t UEP8_TX_DMA;   /* 端点8 TX DMA 地址 */
    __IO uint32_t UEP9_TX_DMA;   /* 端点9 TX DMA 地址 */
    __IO uint32_t UEP10_TX_DMA;  /* 端点10 TX DMA 地址 */
    __IO uint32_t UEP11_TX_DMA;  /* 端点11 TX DMA 地址 */
    __IO uint32_t UEP12_TX_DMA;  /* 端点12 TX DMA 地址 */
    __IO uint32_t UEP13_TX_DMA;  /* 端点13 TX DMA 地址 */
    __IO uint32_t UEP14_TX_DMA;  /* 端点14 TX DMA 地址 */
    __IO uint32_t UEP15_TX_DMA;  /* 端点15 TX DMA 地址 */
    __IO uint16_t UEP0_MAX_LEN;  /* 端点0 最大包长度 */
    __IO uint16_t RESERVED2;     /* 保留 */
    __IO uint16_t UEP1_MAX_LEN;  /* 端点1 最大包长度 */
    __IO uint16_t RESERVED3;     /* 保留 */
    __IO uint16_t UEP2_MAX_LEN;  /* 端点2 最大包长度 */
    __IO uint16_t RESERVED4;     /* 保留 */
    __IO uint16_t UEP3_MAX_LEN;  /* 端点3 最大包长度 */
    __IO uint16_t RESERVED5;     /* 保留 */
    __IO uint16_t UEP4_MAX_LEN;  /* 端点4 最大包长度 */
    __IO uint16_t RESERVED6;     /* 保留 */
    __IO uint16_t UEP5_MAX_LEN;  /* 端点5 最大包长度 */
    __IO uint16_t RESERVED7;     /* 保留 */
    __IO uint16_t UEP6_MAX_LEN;  /* 端点6 最大包长度 */
    __IO uint16_t RESERVED8;     /* 保留 */
    __IO uint16_t UEP7_MAX_LEN;  /* 端点7 最大包长度 */
    __IO uint16_t RESERVED9;     /* 保留 */
    __IO uint16_t UEP8_MAX_LEN;  /* 端点8 最大包长度 */
    __IO uint16_t RESERVED10;    /* 保留 */
    __IO uint16_t UEP9_MAX_LEN;  /* 端点9 最大包长度 */
    __IO uint16_t RESERVED11;    /* 保留 */
    __IO uint16_t UEP10_MAX_LEN; /* 端点10 最大包长度 */
    __IO uint16_t RESERVED12;    /* 保留 */
    __IO uint16_t UEP11_MAX_LEN; /* 端点11 最大包长度 */
    __IO uint16_t RESERVED13;    /* 保留 */
    __IO uint16_t UEP12_MAX_LEN; /* 端点12 最大包长度 */
    __IO uint16_t RESERVED14;    /* 保留 */
    __IO uint16_t UEP13_MAX_LEN; /* 端点13 最大包长度 */
    __IO uint16_t RESERVED15;    /* 保留 */
    __IO uint16_t UEP14_MAX_LEN; /* 端点14 最大包长度 */
    __IO uint16_t RESERVED16;    /* 保留 */
    __IO uint16_t UEP15_MAX_LEN; /* 端点15 最大包长度 */
    __IO uint16_t RESERVED17;    /* 保留 */
    __IO uint16_t UEP0_TX_LEN;   /* 端点0 发送长度 */
    __IO uint8_t UEP0_TX_CTRL;   /* 端点0 发送控制 */
    __IO uint8_t UEP0_RX_CTRL;   /* 端点0 接收控制 */
    __IO uint16_t UEP1_TX_LEN;   /* 端点1 发送长度 */
    __IO uint8_t UEP1_TX_CTRL;   /* 端点1 发送控制 */
    __IO uint8_t UEP1_RX_CTRL;   /* 端点1 接收控制 */
    __IO uint16_t UEP2_TX_LEN;   /* 端点2 发送长度 */
    __IO uint8_t UEP2_TX_CTRL;   /* 端点2 发送控制 */
    __IO uint8_t UEP2_RX_CTRL;   /* 端点2 接收控制 */
    __IO uint16_t UEP3_TX_LEN;   /* 端点3 发送长度 */
    __IO uint8_t UEP3_TX_CTRL;   /* 端点3 发送控制 */
    __IO uint8_t UEP3_RX_CTRL;   /* 端点3 接收控制 */
    __IO uint16_t UEP4_TX_LEN;   /* 端点4 发送长度 */
    __IO uint8_t UEP4_TX_CTRL;   /* 端点4 发送控制 */
    __IO uint8_t UEP4_RX_CTRL;   /* 端点4 接收控制 */
    __IO uint16_t UEP5_TX_LEN;   /* 端点5 发送长度 */
    __IO uint8_t UEP5_TX_CTRL;   /* 端点5 发送控制 */
    __IO uint8_t UEP5_RX_CTRL;   /* 端点5 接收控制 */
    __IO uint16_t UEP6_TX_LEN;   /* 端点6 发送长度 */
    __IO uint8_t UEP6_TX_CTRL;   /* 端点6 发送控制 */
    __IO uint8_t UEP6_RX_CTRL;   /* 端点6 接收控制 */
    __IO uint16_t UEP7_TX_LEN;   /* 端点7 发送长度 */
    __IO uint8_t UEP7_TX_CTRL;   /* 端点7 发送控制 */
    __IO uint8_t UEP7_RX_CTRL;   /* 端点7 接收控制 */
    __IO uint16_t UEP8_TX_LEN;   /* 端点8 发送长度 */
    __IO uint8_t UEP8_TX_CTRL;   /* 端点8 发送控制 */
    __IO uint8_t UEP8_RX_CTRL;   /* 端点8 接收控制 */
    __IO uint16_t UEP9_TX_LEN;   /* 端点9 发送长度 */
    __IO uint8_t UEP9_TX_CTRL;   /* 端点9 发送控制 */
    __IO uint8_t UEP9_RX_CTRL;   /* 端点9 接收控制 */
    __IO uint16_t UEP10_TX_LEN;  /* 端点10 发送长度 */
    __IO uint8_t UEP10_TX_CTRL;  /* 端点10 发送控制 */
    __IO uint8_t UEP10_RX_CTRL;  /* 端点10 接收控制 */
    __IO uint16_t UEP11_TX_LEN;  /* 端点11 发送长度 */
    __IO uint8_t UEP11_TX_CTRL;  /* 端点11 发送控制 */
    __IO uint8_t UEP11_RX_CTRL;  /* 端点11 接收控制 */
    __IO uint16_t UEP12_TX_LEN;  /* 端点12 发送长度 */
    __IO uint8_t UEP12_TX_CTRL;  /* 端点12 发送控制 */
    __IO uint8_t UEP12_RX_CTRL;  /* 端点12 接收控制 */
    __IO uint16_t UEP13_TX_LEN;  /* 端点13 发送长度 */
    __IO uint8_t UEP13_TX_CTRL;  /* 端点13 发送控制 */
    __IO uint8_t UEP13_RX_CTRL;  /* 端点13 接收控制 */
    __IO uint16_t UEP14_TX_LEN;  /* 端点14 发送长度 */
    __IO uint8_t UEP14_TX_CTRL;  /* 端点14 发送控制 */
    __IO uint8_t UEP14_RX_CTRL;  /* 端点14 接收控制 */
    __IO uint16_t UEP15_TX_LEN;  /* 端点15 发送长度 */
    __IO uint8_t UEP15_TX_CTRL;  /* 端点15 发送控制 */
    __IO uint8_t UEP15_RX_CTRL;  /* 端点15 接收控制 */
  } USBHSD_TypeDef;

  /* USBHS 主机寄存器 (紧凑打包) */
  typedef struct __attribute__((packed))
  {
    __IO uint8_t CONTROL;          /* 控制寄存器 */
    __IO uint8_t HOST_CTRL;        /* 主机控制寄存器 */
    __IO uint8_t INT_EN;           /* 中断使能寄存器 */
    __IO uint8_t DEV_AD;           /* 设备地址寄存器 */
    __IO uint16_t FRAME_NO;        /* 帧号寄存器 */
    __IO uint8_t SUSPEND;          /* 挂起控制寄存器 */
    __IO uint8_t RESERVED0;        /* 保留 */
    __IO uint8_t SPEED_TYPE;       /* 速度类型寄存器 */
    __IO uint8_t MIS_ST;           /* 杂项状态寄存器 */
    __IO uint8_t INT_FG;           /* 中断标志寄存器 */
    __IO uint8_t INT_ST;           /* 中断状态寄存器 */
    __IO uint16_t RX_LEN;          /* 接收长度寄存器 */
    __IO uint16_t RESERVED1;       /* 保留 */
    __IO uint32_t HOST_EP_CONFIG;  /* 主机端点配置寄存器 */
    __IO uint32_t HOST_EP_TYPE;    /* 主机端点类型寄存器 */
    __IO uint32_t RESERVED2;       /* 保留 */
    __IO uint32_t RESERVED3;       /* 保留 */
    __IO uint32_t RESERVED4;       /* 保留 */
    __IO uint32_t HOST_RX_DMA;     /* 主机接收 DMA 地址 */
    __IO uint32_t RESERVED5;       /* 保留 */
    __IO uint32_t RESERVED6;       /* 保留 */
    __IO uint32_t RESERVED7;       /* 保留 */
    __IO uint32_t RESERVED8;       /* 保留 */
    __IO uint32_t RESERVED9;       /* 保留 */
    __IO uint32_t RESERVED10;      /* 保留 */
    __IO uint32_t RESERVED11;      /* 保留 */
    __IO uint32_t RESERVED12;      /* 保留 */
    __IO uint32_t RESERVED13;      /* 保留 */
    __IO uint32_t RESERVED14;      /* 保留 */
    __IO uint32_t RESERVED15;      /* 保留 */
    __IO uint32_t RESERVED16;      /* 保留 */
    __IO uint32_t RESERVED17;      /* 保留 */
    __IO uint32_t RESERVED18;      /* 保留 */
    __IO uint32_t RESERVED19;      /* 保留 */
    __IO uint32_t HOST_TX_DMA;     /* 主机发送 DMA 地址 */
    __IO uint32_t RESERVED20;      /* 保留 */
    __IO uint32_t RESERVED21;      /* 保留 */
    __IO uint32_t RESERVED22;      /* 保留 */
    __IO uint32_t RESERVED23;      /* 保留 */
    __IO uint32_t RESERVED24;      /* 保留 */
    __IO uint32_t RESERVED25;      /* 保留 */
    __IO uint32_t RESERVED26;      /* 保留 */
    __IO uint32_t RESERVED27;      /* 保留 */
    __IO uint32_t RESERVED28;      /* 保留 */
    __IO uint32_t RESERVED29;      /* 保留 */
    __IO uint32_t RESERVED30;      /* 保留 */
    __IO uint32_t RESERVED31;      /* 保留 */
    __IO uint32_t RESERVED32;      /* 保留 */
    __IO uint32_t RESERVED33;      /* 保留 */
    __IO uint16_t HOST_RX_MAX_LEN; /* 主机接收最大长度 */
    __IO uint16_t RESERVED34;      /* 保留 */
    __IO uint32_t RESERVED35;      /* 保留 */
    __IO uint32_t RESERVED36;      /* 保留 */
    __IO uint32_t RESERVED37;      /* 保留 */
    __IO uint32_t RESERVED38;      /* 保留 */
    __IO uint32_t RESERVED39;      /* 保留 */
    __IO uint32_t RESERVED40;      /* 保留 */
    __IO uint32_t RESERVED41;      /* 保留 */
    __IO uint32_t RESERVED42;      /* 保留 */
    __IO uint32_t RESERVED43;      /* 保留 */
    __IO uint32_t RESERVED44;      /* 保留 */
    __IO uint32_t RESERVED45;      /* 保留 */
    __IO uint32_t RESERVED46;      /* 保留 */
    __IO uint32_t RESERVED47;      /* 保留 */
    __IO uint32_t RESERVED48;      /* 保留 */
    __IO uint32_t RESERVED49;      /* 保留 */
    __IO uint8_t HOST_EP_PID;      /* 主机端点 PID */
    __IO uint8_t RESERVED50;       /* 保留 */
    __IO uint8_t RESERVED51;       /* 保留 */
    __IO uint8_t HOST_RX_CTRL;     /* 主机接收控制 */
    __IO uint16_t HOST_TX_LEN;     /* 主机发送长度 */
    __IO uint8_t HOST_TX_CTRL;     /* 主机发送控制 */
    __IO uint8_t RESERVED52;       /* 保留 */
    __IO uint16_t HOST_SPLIT_DATA; /* 主机分裂事务数据 */
  } USBHSH_TypeDef;

  /* USBOTG_FS 全速 USB OTG 设备寄存器 */
  typedef struct
  {
    __IO uint8_t BASE_CTRL;    /* 基础控制寄存器 */
    __IO uint8_t UDEV_CTRL;    /* 设备控制寄存器 */
    __IO uint8_t INT_EN;       /* 中断使能寄存器 */
    __IO uint8_t DEV_ADDR;     /* 设备地址寄存器 */
    __IO uint8_t Reserve0;     /* 保留 */
    __IO uint8_t MIS_ST;       /* 杂项状态寄存器 */
    __IO uint8_t INT_FG;       /* 中断标志寄存器 */
    __IO uint8_t INT_ST;       /* 中断状态寄存器 */
    __IO uint16_t RX_LEN;      /* 接收长度寄存器 */
    __IO uint16_t Reserve1;    /* 保留 */
    __IO uint8_t UEP4_1_MOD;   /* 端点4-1 模式 */
    __IO uint8_t UEP2_3_MOD;   /* 端点2-3 模式 */
    __IO uint8_t UEP5_6_MOD;   /* 端点5-6 模式 */
    __IO uint8_t UEP7_MOD;     /* 端点7 模式 */
    __IO uint32_t UEP0_DMA;    /* 端点0 DMA 地址 */
    __IO uint32_t UEP1_DMA;    /* 端点1 DMA 地址 */
    __IO uint32_t UEP2_DMA;    /* 端点2 DMA 地址 */
    __IO uint32_t UEP3_DMA;    /* 端点3 DMA 地址 */
    __IO uint32_t UEP4_DMA;    /* 端点4 DMA 地址 */
    __IO uint32_t UEP5_DMA;    /* 端点5 DMA 地址 */
    __IO uint32_t UEP6_DMA;    /* 端点6 DMA 地址 */
    __IO uint32_t UEP7_DMA;    /* 端点7 DMA 地址 */
    __IO uint16_t UEP0_TX_LEN; /* 端点0 发送长度 */
    __IO uint8_t UEP0_TX_CTRL; /* 端点0 发送控制 */
    __IO uint8_t UEP0_RX_CTRL; /* 端点0 接收控制 */
    __IO uint16_t UEP1_TX_LEN; /* 端点1 发送长度 */
    __IO uint8_t UEP1_TX_CTRL; /* 端点1 发送控制 */
    __IO uint8_t UEP1_RX_CTRL; /* 端点1 接收控制 */
    __IO uint16_t UEP2_TX_LEN; /* 端点2 发送长度 */
    __IO uint8_t UEP2_TX_CTRL; /* 端点2 发送控制 */
    __IO uint8_t UEP2_RX_CTRL; /* 端点2 接收控制 */
    __IO uint16_t UEP3_TX_LEN; /* 端点3 发送长度 */
    __IO uint8_t UEP3_TX_CTRL; /* 端点3 发送控制 */
    __IO uint8_t UEP3_RX_CTRL; /* 端点3 接收控制 */
    __IO uint16_t UEP4_TX_LEN; /* 端点4 发送长度 */
    __IO uint8_t UEP4_TX_CTRL; /* 端点4 发送控制 */
    __IO uint8_t UEP4_RX_CTRL; /* 端点4 接收控制 */
    __IO uint16_t UEP5_TX_LEN; /* 端点5 发送长度 */
    __IO uint8_t UEP5_TX_CTRL; /* 端点5 发送控制 */
    __IO uint8_t UEP5_RX_CTRL; /* 端点5 接收控制 */
    __IO uint16_t UEP6_TX_LEN; /* 端点6 发送长度 */
    __IO uint8_t UEP6_TX_CTRL; /* 端点6 发送控制 */
    __IO uint8_t UEP6_RX_CTRL; /* 端点6 接收控制 */
    __IO uint16_t UEP7_TX_LEN; /* 端点7 发送长度 */
    __IO uint8_t UEP7_TX_CTRL; /* 端点7 发送控制 */
    __IO uint8_t UEP7_RX_CTRL; /* 端点7 接收控制 */
    __IO uint32_t Reserve2;    /* 保留 */
    __IO uint32_t OTG_CR;      /* OTG 控制寄存器 */
    __IO uint32_t OTG_SR;      /* OTG 状态寄存器 */
  } USBFSD_TypeDef;

  /* USBOTG_FS 全速 USB OTG 主机寄存器 (紧凑打包) */
  typedef struct __attribute__((packed))
  {
    __IO uint8_t BASE_CTRL;    /* 基础控制寄存器 */
    __IO uint8_t HOST_CTRL;    /* 主机控制寄存器 */
    __IO uint8_t INT_EN;       /* 中断使能寄存器 */
    __IO uint8_t DEV_ADDR;     /* 设备地址寄存器 */
    __IO uint8_t Reserve0;     /* 保留 */
    __IO uint8_t MIS_ST;       /* 杂项状态寄存器 */
    __IO uint8_t INT_FG;       /* 中断标志寄存器 */
    __IO uint8_t INT_ST;       /* 中断状态寄存器 */
    __IO uint16_t RX_LEN;      /* 接收长度寄存器 */
    __IO uint16_t Reserve1;    /* 保留 */
    __IO uint8_t Reserve2;     /* 保留 */
    __IO uint8_t HOST_EP_MOD;  /* 主机端点模式 */
    __IO uint16_t Reserve3;    /* 保留 */
    __IO uint32_t Reserve4;    /* 保留 */
    __IO uint32_t Reserve5;    /* 保留 */
    __IO uint32_t HOST_RX_DMA; /* 主机接收 DMA 地址 */
    __IO uint32_t HOST_TX_DMA; /* 主机发送 DMA 地址 */
    __IO uint32_t Reserve6;    /* 保留 */
    __IO uint32_t Reserve7;    /* 保留 */
    __IO uint32_t Reserve8;    /* 保留 */
    __IO uint32_t Reserve9;    /* 保留 */
    __IO uint32_t Reserve10;   /* 保留 */
    __IO uint32_t Reserve11;   /* 保留 */
    __IO uint16_t Reserve12;   /* 保留 */
    __IO uint16_t HOST_SETUP;  /* 主机设置包 */
    __IO uint8_t HOST_EP_PID;  /* 主机端点 PID */
    __IO uint8_t Reserve13;    /* 保留 */
    __IO uint8_t Reserve14;    /* 保留 */
    __IO uint8_t HOST_RX_CTRL; /* 主机接收控制 */
    __IO uint16_t HOST_TX_LEN; /* 主机发送长度 */
    __IO uint8_t HOST_TX_CTRL; /* 主机发送控制 */
    __IO uint8_t Reserve15;    /* 保留 */
    __IO uint32_t Reserve16;   /* 保留 */
    __IO uint32_t Reserve17;   /* 保留 */
    __IO uint32_t Reserve18;   /* 保留 */
    __IO uint32_t Reserve19;   /* 保留 */
    __IO uint32_t Reserve20;   /* 保留 */
    __IO uint32_t OTG_CR;      /* OTG 控制寄存器 */
    __IO uint32_t OTG_SR;      /* OTG 状态寄存器 */
  } USBFSH_TypeDef;

  /* 以太网 MAC */
  typedef struct
  {
    __IO uint32_t MACCR;       /* MAC 配置寄存器 */
    __IO uint32_t MACFFR;      /* MAC 帧过滤寄存器 */
    __IO uint32_t MACHTHR;     /* MAC 哈希表高寄存器 */
    __IO uint32_t MACHTLR;     /* MAC 哈希表低寄存器 */
    __IO uint32_t MACMIIAR;    /* MAC MII 地址寄存器 */
    __IO uint32_t MACMIIDR;    /* MAC MII 数据寄存器 */
    __IO uint32_t MACFCR;      /* MAC 流控寄存器 */
    __IO uint32_t MACVLANTR;   /* MAC VLAN 标签寄存器 */
    uint32_t RESERVED0[2];     /* 保留 */
    __IO uint32_t MACRWUFFR;   /* MAC 远程唤醒帧过滤寄存器 */
    __IO uint32_t MACPMTCSR;   /* MAC 电源管理控制/状态寄存器 */
    uint32_t RESERVED1[2];     /* 保留 */
    __IO uint32_t MACSR;       /* MAC 状态寄存器 */
    __IO uint32_t MACIMR;      /* MAC 中断掩码寄存器 */
    __IO uint32_t MACA0HR;     /* MAC 地址0 高寄存器 */
    __IO uint32_t MACA0LR;     /* MAC 地址0 低寄存器 */
    __IO uint32_t MACA1HR;     /* MAC 地址1 高寄存器 */
    __IO uint32_t MACA1LR;     /* MAC 地址1 低寄存器 */
    __IO uint32_t MACA2HR;     /* MAC 地址2 高寄存器 */
    __IO uint32_t MACA2LR;     /* MAC 地址2 低寄存器 */
    __IO uint32_t MACA3HR;     /* MAC 地址3 高寄存器 */
    __IO uint32_t MACA3LR;     /* MAC 地址3 低寄存器 */
    uint32_t RESERVED2[14];    /* 保留 */
    __IO uint32_t MACCFG0;     /* MAC 配置0 (扩展) */
    uint32_t RESERVED10[25];   /* 保留 */
    __IO uint32_t MMCCR;       /* MMC 控制寄存器 */
    __IO uint32_t MMCRIR;      /* MMC 接收中断寄存器 */
    __IO uint32_t MMCTIR;      /* MMC 发送中断寄存器 */
    __IO uint32_t MMCRIMR;     /* MMC 接收中断掩码寄存器 */
    __IO uint32_t MMCTIMR;     /* MMC 发送中断掩码寄存器 */
    uint32_t RESERVED3[14];    /* 保留 */
    __IO uint32_t MMCTGFSCCR;  /* MMC 发送好单冲突计数器 */
    __IO uint32_t MMCTGFMSCCR; /* MMC 发送好多次冲突计数器 */
    uint32_t RESERVED4[5];     /* 保留 */
    __IO uint32_t MMCTGFCR;    /* MMC 发送好流控计数器 */
    uint32_t RESERVED5[10];    /* 保留 */
    __IO uint32_t MMCRFCECR;   /* MMC 接收 CRC 错误计数器 */
    __IO uint32_t MMCRFAECR;   /* MMC 接收对齐错误计数器 */
    uint32_t RESERVED6[10];    /* 保留 */
    __IO uint32_t MMCRGUFCR;   /* MMC 接收好单播帧计数器 */
    uint32_t RESERVED7[334];   /* 保留 */
    __IO uint32_t PTPTSCR;     /* PTP 时间戳控制寄存器 */
    __IO uint32_t PTPSSIR;     /* PTP 子秒递增寄存器 */
    __IO uint32_t PTPTSHR;     /* PTP 时间戳高寄存器 */
    __IO uint32_t PTPTSLR;     /* PTP 时间戳低寄存器 */
    __IO uint32_t PTPTSHUR;    /* PTP 时间戳高更新寄存器 */
    __IO uint32_t PTPTSLUR;    /* PTP 时间戳低更新寄存器 */
    __IO uint32_t PTPTSAR;     /* PTP 时间戳添加寄存器 */
    __IO uint32_t PTPTTHR;     /* PTP 目标时间高寄存器 */
    __IO uint32_t PTPTTLR;     /* PTP 目标时间低寄存器 */
    uint32_t RESERVED8[567];   /* 保留 */
    __IO uint32_t DMABMR;      /* DMA 总线模式寄存器 */
    __IO uint32_t DMATPDR;     /* DMA 发送轮询请求寄存器 */
    __IO uint32_t DMARPDR;     /* DMA 接收轮询请求寄存器 */
    __IO uint32_t DMARDLAR;    /* DMA 接收描述符链表地址寄存器 */
    __IO uint32_t DMATDLAR;    /* DMA 发送描述符链表地址寄存器 */
    __IO uint32_t DMASR;       /* DMA 状态寄存器 */
    __IO uint32_t DMAOMR;      /* DMA 操作模式寄存器 */
    __IO uint32_t DMAIER;      /* DMA 中断使能寄存器 */
    __IO uint32_t DMAMFBOCR;   /* DMA 丢失帧和缓冲区溢出计数器 */
    uint32_t RESERVED9[9];     /* 保留 */
    __IO uint32_t DMACHTDR;    /* DMA 当前主机发送描述符 */
    __IO uint32_t DMACHRDR;    /* DMA 当前主机接收描述符 */
    __IO uint32_t DMACHTBAR;   /* DMA 当前主机发送缓冲区地址 */
    __IO uint32_t DMACHRBAR;   /* DMA 当前主机接收缓冲区地址 */
  } ETH_TypeDef;

/* 外设内存映射 */
#define FLASH_BASE ((uint32_t)0x08000000)  /* FLASH 基地址 (别名区域) */
#define SRAM_BASE ((uint32_t)0x20000000)   /* SRAM 基地址 (别名区域) */
#define PERIPH_BASE ((uint32_t)0x40000000) /* 外设基地址 (别名区域) */

#define FSMC_R_BASE ((uint32_t)0xA0000000) /* FSMC 寄存器基地址 */

#define APB1PERIPH_BASE (PERIPH_BASE)           /* APB1 外设基地址 */
#define APB2PERIPH_BASE (PERIPH_BASE + 0x10000) /* APB2 外设基地址 */
#define AHBPERIPH_BASE (PERIPH_BASE + 0x20000)  /* AHB 外设基地址 */

/* 各外设具体基地址定义 */
#define TIM2_BASE (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE (APB1PERIPH_BASE + 0x1400)
#define UART6_BASE (APB1PERIPH_BASE + 0x1800)
#define UART7_BASE (APB1PERIPH_BASE + 0x1C00)
#define UART8_BASE (APB1PERIPH_BASE + 0x2000)
#define RTC_BASE (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE (APB1PERIPH_BASE + 0x3800)
#define SPI3_BASE (APB1PERIPH_BASE + 0x3C00)
#define USART2_BASE (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE (APB1PERIPH_BASE + 0x4800)
#define UART4_BASE (APB1PERIPH_BASE + 0x4C00)
#define UART5_BASE (APB1PERIPH_BASE + 0x5000)
#define I2C1_BASE (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE (APB1PERIPH_BASE + 0x5800)
#define CAN1_BASE (APB1PERIPH_BASE + 0x6400)
#define CAN2_BASE (APB1PERIPH_BASE + 0x6800)
#define BKP_BASE (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE (APB1PERIPH_BASE + 0x7000)
#define DAC_BASE (APB1PERIPH_BASE + 0x7400)

#define AFIO_BASE (APB2PERIPH_BASE + 0x0000)
#define EXTI_BASE (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE (APB2PERIPH_BASE + 0x1400)
#define GPIOE_BASE (APB2PERIPH_BASE + 0x1800)
#define ADC1_BASE (APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE (APB2PERIPH_BASE + 0x2800)
#define TIM1_BASE (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE (APB2PERIPH_BASE + 0x3000)
#define TIM8_BASE (APB2PERIPH_BASE + 0x3400)
#define USART1_BASE (APB2PERIPH_BASE + 0x3800)
#define ADC3_BASE (APB2PERIPH_BASE + 0x3C00)
#define TIM9_BASE (APB2PERIPH_BASE + 0x4C00)
#define TIM10_BASE (APB2PERIPH_BASE + 0x5000)
#define SDIO_BASE (APB2PERIPH_BASE + 0x8000)

#define DMA1_BASE (AHBPERIPH_BASE + 0x0000)
#define DMA1_Channel1_BASE (AHBPERIPH_BASE + 0x0008)
#define DMA1_Channel2_BASE (AHBPERIPH_BASE + 0x001C)
#define DMA1_Channel3_BASE (AHBPERIPH_BASE + 0x0030)
#define DMA1_Channel4_BASE (AHBPERIPH_BASE + 0x0044)
#define DMA1_Channel5_BASE (AHBPERIPH_BASE + 0x0058)
#define DMA1_Channel6_BASE (AHBPERIPH_BASE + 0x006C)
#define DMA1_Channel7_BASE (AHBPERIPH_BASE + 0x0080)
#define DMA2_BASE (AHBPERIPH_BASE + 0x0400)
#define DMA2_Channel1_BASE (AHBPERIPH_BASE + 0x0408)
#define DMA2_Channel2_BASE (AHBPERIPH_BASE + 0x041C)
#define DMA2_Channel3_BASE (AHBPERIPH_BASE + 0x0430)
#define DMA2_Channel4_BASE (AHBPERIPH_BASE + 0x0444)
#define DMA2_Channel5_BASE (AHBPERIPH_BASE + 0x0458)
#define DMA2_Channel6_BASE (AHBPERIPH_BASE + 0x046C)
#define DMA2_Channel7_BASE (AHBPERIPH_BASE + 0x0480)
#define DMA2_Channel8_BASE (AHBPERIPH_BASE + 0x0490)
#define DMA2_Channel9_BASE (AHBPERIPH_BASE + 0x04A0)
#define DMA2_Channel10_BASE (AHBPERIPH_BASE + 0x04B0)
#define DMA2_Channel11_BASE (AHBPERIPH_BASE + 0x04C0)
#define DMA2_EXTEN_BASE (AHBPERIPH_BASE + 0x04D0)
#define RCC_BASE (AHBPERIPH_BASE + 0x1000)
#define FLASH_R_BASE (AHBPERIPH_BASE + 0x2000)
#define CRC_BASE (AHBPERIPH_BASE + 0x3000)
#define USBHS_BASE (AHBPERIPH_BASE + 0x3400)
#define EXTEN_BASE (AHBPERIPH_BASE + 0x3800)
#define OPA_BASE (AHBPERIPH_BASE + 0x3804)
#define RNG_BASE (AHBPERIPH_BASE + 0x3C00)

#define ETH_BASE (AHBPERIPH_BASE + 0x8000)
#define ETH_MAC_BASE (ETH_BASE)
#define ETH_MMC_BASE (ETH_BASE + 0x0100)
#define ETH_PTP_BASE (ETH_BASE + 0x0700)
#define ETH_DMA_BASE (ETH_BASE + 0x1000)

#define USBFS_BASE ((uint32_t)0x50000000)
#define DVP_BASE ((uint32_t)0x50050000)

#define FSMC_Bank1_R_BASE (FSMC_R_BASE + 0x0000)
#define FSMC_Bank1E_R_BASE (FSMC_R_BASE + 0x0104)
#define FSMC_Bank2_R_BASE (FSMC_R_BASE + 0x0060)

#define OB_BASE ((uint32_t)0x1FFFF800)      /* 选项字节基地址 */
#define FEATURE_SIGN ((uint32_t)0x1FFFF7D0) /* 特征签名地址 */

/* 外设声明 (指针) */
#define TIM2 ((TIM_TypeDef *)TIM2_BASE)
#define TIM3 ((TIM_TypeDef *)TIM3_BASE)
#define TIM4 ((TIM_TypeDef *)TIM4_BASE)
#define TIM5 ((TIM_TypeDef *)TIM5_BASE)
#define TIM6 ((TIM_TypeDef *)TIM6_BASE)
#define TIM7 ((TIM_TypeDef *)TIM7_BASE)
#define UART6 ((USART_TypeDef *)UART6_BASE)
#define UART7 ((USART_TypeDef *)UART7_BASE)
#define UART8 ((USART_TypeDef *)UART8_BASE)
#define RTC ((RTC_TypeDef *)RTC_BASE)
#define WWDG ((WWDG_TypeDef *)WWDG_BASE)
#define IWDG ((IWDG_TypeDef *)IWDG_BASE)
#define SPI2 ((SPI_TypeDef *)SPI2_BASE)
#define SPI3 ((SPI_TypeDef *)SPI3_BASE)
#define USART2 ((USART_TypeDef *)USART2_BASE)
#define USART3 ((USART_TypeDef *)USART3_BASE)
#define UART4 ((USART_TypeDef *)UART4_BASE)
#define UART5 ((USART_TypeDef *)UART5_BASE)
#define I2C1 ((I2C_TypeDef *)I2C1_BASE)
#define I2C2 ((I2C_TypeDef *)I2C2_BASE)
#define CAN1 ((CAN_TypeDef *)CAN1_BASE)
#define CAN2 ((CAN_TypeDef *)CAN2_BASE)
#define BKP ((BKP_TypeDef *)BKP_BASE)
#define PWR ((PWR_TypeDef *)PWR_BASE)
#define DAC ((DAC_TypeDef *)DAC_BASE)

#define AFIO ((AFIO_TypeDef *)AFIO_BASE)
#define EXTI ((EXTI_TypeDef *)EXTI_BASE)
#define GPIOA ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE ((GPIO_TypeDef *)GPIOE_BASE)
#define ADC1 ((ADC_TypeDef *)ADC1_BASE)
#define ADC2 ((ADC_TypeDef *)ADC2_BASE)
#define TKey1 ((ADC_TypeDef *)ADC1_BASE) /* TKey 复用 ADC1 */
#define TKey2 ((ADC_TypeDef *)ADC2_BASE) /* TKey 复用 ADC2 */
#define TIM1 ((TIM_TypeDef *)TIM1_BASE)
#define SPI1 ((SPI_TypeDef *)SPI1_BASE)
#define TIM8 ((TIM_TypeDef *)TIM8_BASE)
#define USART1 ((USART_TypeDef *)USART1_BASE)
#define ADC3 ((ADC_TypeDef *)ADC3_BASE)
#define TIM9 ((TIM_TypeDef *)TIM9_BASE)
#define TIM10 ((TIM_TypeDef *)TIM10_BASE)
#define SDIO ((SDIO_TypeDef *)SDIO_BASE)

#define DMA1 ((DMA_TypeDef *)DMA1_BASE)
#define DMA2 ((DMA_TypeDef *)DMA2_BASE)
#define DMA2_EXTEN ((DMA_TypeDef *)DMA2_EXTEN_BASE)
#define DMA1_Channel1 ((DMA_Channel_TypeDef *)DMA1_Channel1_BASE)
#define DMA1_Channel2 ((DMA_Channel_TypeDef *)DMA1_Channel2_BASE)
#define DMA1_Channel3 ((DMA_Channel_TypeDef *)DMA1_Channel3_BASE)
#define DMA1_Channel4 ((DMA_Channel_TypeDef *)DMA1_Channel4_BASE)
#define DMA1_Channel5 ((DMA_Channel_TypeDef *)DMA1_Channel5_BASE)
#define DMA1_Channel6 ((DMA_Channel_TypeDef *)DMA1_Channel6_BASE)
#define DMA1_Channel7 ((DMA_Channel_TypeDef *)DMA1_Channel7_BASE)
#define DMA2_Channel1 ((DMA_Channel_TypeDef *)DMA2_Channel1_BASE)
#define DMA2_Channel2 ((DMA_Channel_TypeDef *)DMA2_Channel2_BASE)
#define DMA2_Channel3 ((DMA_Channel_TypeDef *)DMA2_Channel3_BASE)
#define DMA2_Channel4 ((DMA_Channel_TypeDef *)DMA2_Channel4_BASE)
#define DMA2_Channel5 ((DMA_Channel_TypeDef *)DMA2_Channel5_BASE)
#define DMA2_Channel6 ((DMA_Channel_TypeDef *)DMA2_Channel6_BASE)
#define DMA2_Channel7 ((DMA_Channel_TypeDef *)DMA2_Channel7_BASE)
#define DMA2_Channel8 ((DMA_Channel_TypeDef *)DMA2_Channel8_BASE)
#define DMA2_Channel9 ((DMA_Channel_TypeDef *)DMA2_Channel9_BASE)
#define DMA2_Channel10 ((DMA_Channel_TypeDef *)DMA2_Channel10_BASE)
#define DMA2_Channel11 ((DMA_Channel_TypeDef *)DMA2_Channel11_BASE)
#define RCC ((RCC_TypeDef *)RCC_BASE)
#define FLASH ((FLASH_TypeDef *)FLASH_R_BASE)
#define CRC ((CRC_TypeDef *)CRC_BASE)
#define USBHSD ((USBHSD_TypeDef *)USBHS_BASE)
#define USBHSH ((USBHSH_TypeDef *)USBHS_BASE)
#define USBFSD ((USBFSD_TypeDef *)USBFS_BASE)
#define USBFSH ((USBFSH_TypeDef *)USBFS_BASE)
#define EXTEN ((EXTEN_TypeDef *)EXTEN_BASE)
#define OPA ((OPA_TypeDef *)OPA_BASE)
#define RNG ((RNG_TypeDef *)RNG_BASE)
#define ETH ((ETH_TypeDef *)ETH_BASE)

#define DVP ((DVP_TypeDef *)DVP_BASE)

#define FSMC_Bank1 ((FSMC_Bank1_TypeDef *)FSMC_Bank1_R_BASE)
#define FSMC_Bank1E ((FSMC_Bank1E_TypeDef *)FSMC_Bank1E_R_BASE)
#define FSMC_Bank2 ((FSMC_Bank2_TypeDef *)FSMC_Bank2_R_BASE)

#define OB ((OB_TypeDef *)OB_BASE)

/******************************************************************************/
/*                         外设寄存器位定义                                      */
/******************************************************************************/

/******************************************************************************/
/*                        模数转换器 (ADC)                                      */
/******************************************************************************/

/********************  ADC_STATR 寄存器位定义  ********************/
#define ADC_AWD ((uint8_t)0x01)   /* 模拟看门狗标志 */
#define ADC_EOC ((uint8_t)0x02)   /* 转换结束 (规则通道) */
#define ADC_JEOC ((uint8_t)0x04)  /* 注入通道转换结束 */
#define ADC_JSTRT ((uint8_t)0x08) /* 注入通道启动标志 */
#define ADC_STRT ((uint8_t)0x10)  /* 规则通道启动标志 */

/*******************  ADC_CTLR1 寄存器位定义  ********************/
#define ADC_AWDCH ((uint32_t)0x0000001F)   /* AWDCH[4:0] 位 (模拟看门狗通道选择) */
#define ADC_AWDCH_0 ((uint32_t)0x00000001) /* 位0 */
#define ADC_AWDCH_1 ((uint32_t)0x00000002) /* 位1 */
#define ADC_AWDCH_2 ((uint32_t)0x00000004) /* 位2 */
#define ADC_AWDCH_3 ((uint32_t)0x00000008) /* 位3 */
#define ADC_AWDCH_4 ((uint32_t)0x00000010) /* 位4 */

#define ADC_EOCIE ((uint32_t)0x00000020)   /* 规则通道转换结束中断使能 */
#define ADC_AWDIE ((uint32_t)0x00000040)   /* 模拟看门狗中断使能 */
#define ADC_JEOCIE ((uint32_t)0x00000080)  /* 注入通道转换结束中断使能 */
#define ADC_SCAN ((uint32_t)0x00000100)    /* 扫描模式 */
#define ADC_AWDSGL ((uint32_t)0x00000200)  /* 在扫描模式下对单个通道使能看门狗 */
#define ADC_JAUTO ((uint32_t)0x00000400)   /* 自动注入组转换 */
#define ADC_DISCEN ((uint32_t)0x00000800)  /* 规则通道间断模式 */
#define ADC_JDISCEN ((uint32_t)0x00001000) /* 注入通道间断模式 */

#define ADC_DISCNUM ((uint32_t)0x0000E000)   /* DISCNUM[2:0] 位 (间断模式通道数) */
#define ADC_DISCNUM_0 ((uint32_t)0x00002000) /* 位0 */
#define ADC_DISCNUM_1 ((uint32_t)0x00004000) /* 位1 */
#define ADC_DISCNUM_2 ((uint32_t)0x00008000) /* 位2 */

#define ADC_DUALMOD ((uint32_t)0x000F0000)   /* DUALMOD[3:0] 位 (双模式选择) */
#define ADC_DUALMOD_0 ((uint32_t)0x00010000) /* 位0 */
#define ADC_DUALMOD_1 ((uint32_t)0x00020000) /* 位1 */
#define ADC_DUALMOD_2 ((uint32_t)0x00040000) /* 位2 */
#define ADC_DUALMOD_3 ((uint32_t)0x00080000) /* 位3 */

#define ADC_JAWDEN ((uint32_t)0x00400000) /* 注入通道模拟看门狗使能 */
#define ADC_AWDEN ((uint32_t)0x00800000)  /* 规则通道模拟看门狗使能 */

#define ADC_TKENABLE ((uint32_t)0x01000000) /* TKEY (触摸按键) 使能 */
#define ADC_TKITUNE ((uint32_t)0x02000000)  /* TKEY 调谐 */
#define ADC_BUFEN ((uint32_t)0x04000000)    /* 缓冲区使能 */

#define ADC_PGA ((uint32_t)0x18000000)   /* PGA (可编程增益放大器) 控制 */
#define ADC_PGA_0 ((uint32_t)0x08000000) /* 位0 */
#define ADC_PGA_1 ((uint32_t)0x10000000) /* 位1 */

/*******************  ADC_CTLR2 寄存器位定义  ********************/
#define ADC_ADON ((uint32_t)0x00000001)   /* A/D 转换器开启/关闭 */
#define ADC_CONT ((uint32_t)0x00000002)   /* 连续转换模式 */
#define ADC_CAL ((uint32_t)0x00000004)    /* A/D 校准 */
#define ADC_RSTCAL ((uint32_t)0x00000008) /* 复位校准 */
#define ADC_DMA ((uint32_t)0x00000100)    /* 直接存储器访问模式 */
#define ADC_ALIGN ((uint32_t)0x00000800)  /* 数据对齐 (左/右) */

#define ADC_JEXTSEL ((uint32_t)0x00007000)   /* JEXTSEL[2:0] 位 (注入组外部事件选择) */
#define ADC_JEXTSEL_0 ((uint32_t)0x00001000) /* 位0 */
#define ADC_JEXTSEL_1 ((uint32_t)0x00002000) /* 位1 */
#define ADC_JEXTSEL_2 ((uint32_t)0x00004000) /* 位2 */

#define ADC_JEXTTRIG ((uint32_t)0x00008000) /* 注入通道外部触发转换模式 */

#define ADC_EXTSEL ((uint32_t)0x000E0000)   /* EXTSEL[2:0] 位 (规则组外部事件选择) */
#define ADC_EXTSEL_0 ((uint32_t)0x00020000) /* 位0 */
#define ADC_EXTSEL_1 ((uint32_t)0x00040000) /* 位1 */
#define ADC_EXTSEL_2 ((uint32_t)0x00080000) /* 位2 */

#define ADC_EXTTRIG ((uint32_t)0x00100000)  /* 规则通道外部触发转换模式 */
#define ADC_JSWSTART ((uint32_t)0x00200000) /* 启动注入通道转换 (软件触发) */
#define ADC_SWSTART ((uint32_t)0x00400000)  /* 启动规则通道转换 (软件触发) */
#define ADC_TSVREFE ((uint32_t)0x00800000)  /* 温度传感器和 VREFINT 使能 */

/******************  ADC_SAMPTR1 寄存器位定义  *******************/
#define ADC_SMP10 ((uint32_t)0x00000007)   /* SMP10[2:0] 位 (通道10 采样时间选择) */
#define ADC_SMP10_0 ((uint32_t)0x00000001) /* 位0 */
#define ADC_SMP10_1 ((uint32_t)0x00000002) /* 位1 */
#define ADC_SMP10_2 ((uint32_t)0x00000004) /* 位2 */

#define ADC_SMP11 ((uint32_t)0x00000038)   /* SMP11[2:0] 位 (通道11 采样时间选择) */
#define ADC_SMP11_0 ((uint32_t)0x00000008) /* 位0 */
#define ADC_SMP11_1 ((uint32_t)0x00000010) /* 位1 */
#define ADC_SMP11_2 ((uint32_t)0x00000020) /* 位2 */

#define ADC_SMP12 ((uint32_t)0x000001C0)   /* SMP12[2:0] 位 (通道12 采样时间选择) */
#define ADC_SMP12_0 ((uint32_t)0x00000040) /* 位0 */
#define ADC_SMP12_1 ((uint32_t)0x00000080) /* 位1 */
#define ADC_SMP12_2 ((uint32_t)0x00000100) /* 位2 */

#define ADC_SMP13 ((uint32_t)0x00000E00)   /* SMP13[2:0] 位 (通道13 采样时间选择) */
#define ADC_SMP13_0 ((uint32_t)0x00000200) /* 位0 */
#define ADC_SMP13_1 ((uint32_t)0x00000400) /* 位1 */
#define ADC_SMP13_2 ((uint32_t)0x00000800) /* 位2 */

#define ADC_SMP14 ((uint32_t)0x00007000)   /* SMP14[2:0] 位 (通道14 采样时间选择) */
#define ADC_SMP14_0 ((uint32_t)0x00001000) /* 位0 */
#define ADC_SMP14_1 ((uint32_t)0x00002000) /* 位1 */
#define ADC_SMP14_2 ((uint32_t)0x00004000) /* 位2 */

#define ADC_SMP15 ((uint32_t)0x00038000)   /* SMP15[2:0] 位 (通道15 采样时间选择) */
#define ADC_SMP15_0 ((uint32_t)0x00008000) /* 位0 */
#define ADC_SMP15_1 ((uint32_t)0x00010000) /* 位1 */
#define ADC_SMP15_2 ((uint32_t)0x00020000) /* 位2 */

#define ADC_SMP16 ((uint32_t)0x001C0000)   /* SMP16[2:0] 位 (通道16 采样时间选择) */
#define ADC_SMP16_0 ((uint32_t)0x00040000) /* 位0 */
#define ADC_SMP16_1 ((uint32_t)0x00080000) /* 位1 */
#define ADC_SMP16_2 ((uint32_t)0x00100000) /* 位2 */

#define ADC_SMP17 ((uint32_t)0x00E00000)   /* SMP17[2:0] 位 (通道17 采样时间选择) */
#define ADC_SMP17_0 ((uint32_t)0x00200000) /* 位0 */
#define ADC_SMP17_1 ((uint32_t)0x00400000) /* 位1 */
#define ADC_SMP17_2 ((uint32_t)0x00800000) /* 位2 */

/******************  ADC_SAMPTR2 寄存器位定义  *******************/
#define ADC_SMP0 ((uint32_t)0x00000007)   /* SMP0[2:0] 位 (通道0 采样时间选择) */
#define ADC_SMP0_0 ((uint32_t)0x00000001) /* 位0 */
#define ADC_SMP0_1 ((uint32_t)0x00000002) /* 位1 */
#define ADC_SMP0_2 ((uint32_t)0x00000004) /* 位2 */

#define ADC_SMP1 ((uint32_t)0x00000038)   /* SMP1[2:0] 位 (通道1 采样时间选择) */
#define ADC_SMP1_0 ((uint32_t)0x00000008) /* 位0 */
#define ADC_SMP1_1 ((uint32_t)0x00000010) /* 位1 */
#define ADC_SMP1_2 ((uint32_t)0x00000020) /* 位2 */

#define ADC_SMP2 ((uint32_t)0x000001C0)   /* SMP2[2:0] 位 (通道2 采样时间选择) */
#define ADC_SMP2_0 ((uint32_t)0x00000040) /* 位0 */
#define ADC_SMP2_1 ((uint32_t)0x00000080) /* 位1 */
#define ADC_SMP2_2 ((uint32_t)0x00000100) /* 位2 */

#define ADC_SMP3 ((uint32_t)0x00000E00)   /* SMP3[2:0] 位 (通道3 采样时间选择) */
#define ADC_SMP3_0 ((uint32_t)0x00000200) /* 位0 */
#define ADC_SMP3_1 ((uint32_t)0x00000400) /* 位1 */
#define ADC_SMP3_2 ((uint32_t)0x00000800) /* 位2 */

#define ADC_SMP4 ((uint32_t)0x00007000)   /* SMP4[2:0] 位 (通道4 采样时间选择) */
#define ADC_SMP4_0 ((uint32_t)0x00001000) /* 位0 */
#define ADC_SMP4_1 ((uint32_t)0x00002000) /* 位1 */
#define ADC_SMP4_2 ((uint32_t)0x00004000) /* 位2 */

#define ADC_SMP5 ((uint32_t)0x00038000)   /* SMP5[2:0] 位 (通道5 采样时间选择) */
#define ADC_SMP5_0 ((uint32_t)0x00008000) /* 位0 */
#define ADC_SMP5_1 ((uint32_t)0x00010000) /* 位1 */
#define ADC_SMP5_2 ((uint32_t)0x00020000) /* 位2 */

#define ADC_SMP6 ((uint32_t)0x001C0000)   /* SMP6[2:0] 位 (通道6 采样时间选择) */
#define ADC_SMP6_0 ((uint32_t)0x00040000) /* 位0 */
#define ADC_SMP6_1 ((uint32_t)0x00080000) /* 位1 */
#define ADC_SMP6_2 ((uint32_t)0x00100000) /* 位2 */

#define ADC_SMP7 ((uint32_t)0x00E00000)   /* SMP7[2:0] 位 (通道7 采样时间选择) */
#define ADC_SMP7_0 ((uint32_t)0x00200000) /* 位0 */
#define ADC_SMP7_1 ((uint32_t)0x00400000) /* 位1 */
#define ADC_SMP7_2 ((uint32_t)0x00800000) /* 位2 */

#define ADC_SMP8 ((uint32_t)0x07000000)   /* SMP8[2:0] 位 (通道8 采样时间选择) */
#define ADC_SMP8_0 ((uint32_t)0x01000000) /* 位0 */
#define ADC_SMP8_1 ((uint32_t)0x02000000) /* 位1 */
#define ADC_SMP8_2 ((uint32_t)0x04000000) /* 位2 */

#define ADC_SMP9 ((uint32_t)0x38000000)   /* SMP9[2:0] 位 (通道9 采样时间选择) */
#define ADC_SMP9_0 ((uint32_t)0x08000000) /* 位0 */
#define ADC_SMP9_1 ((uint32_t)0x10000000) /* 位1 */
#define ADC_SMP9_2 ((uint32_t)0x20000000) /* 位2 */

/******************  ADC_IOFR1 寄存器位定义  *******************/
#define ADC_JOFFSET1 ((uint16_t)0x0FFF) /* 注入通道1 数据偏移 */

/******************  ADC_IOFR2 寄存器位定义  *******************/
#define ADC_JOFFSET2 ((uint16_t)0x0FFF) /* 注入通道2 数据偏移 */

/******************  ADC_IOFR3 寄存器位定义  *******************/
#define ADC_JOFFSET3 ((uint16_t)0x0FFF) /* 注入通道3 数据偏移 */

/******************  ADC_IOFR4 寄存器位定义  *******************/
#define ADC_JOFFSET4 ((uint16_t)0x0FFF) /* 注入通道4 数据偏移 */

/*******************  ADC_WDHTR 寄存器位定义  ********************/
#define ADC_HT ((uint16_t)0x0FFF) /* 模拟看门狗高阈值 */

/*******************  ADC_WDLTR 寄存器位定义  ********************/
#define ADC_LT ((uint16_t)0x0FFF) /* 模拟看门狗低阈值 */

/*******************  ADC_RSQR1 寄存器位定义  *******************/
#define ADC_SQ13 ((uint32_t)0x0000001F)   /* SQ13[4:0] 位 (规则序列中第13个转换的通道号) */
#define ADC_SQ13_0 ((uint32_t)0x00000001) /* 位0 */
#define ADC_SQ13_1 ((uint32_t)0x00000002) /* 位1 */
#define ADC_SQ13_2 ((uint32_t)0x00000004) /* 位2 */
#define ADC_SQ13_3 ((uint32_t)0x00000008) /* 位3 */
#define ADC_SQ13_4 ((uint32_t)0x00000010) /* 位4 */

#define ADC_SQ14 ((uint32_t)0x000003E0)   /* SQ14[4:0] 位 (规则序列中第14个转换的通道号) */
#define ADC_SQ14_0 ((uint32_t)0x00000020) /* 位0 */
#define ADC_SQ14_1 ((uint32_t)0x00000040) /* 位1 */
#define ADC_SQ14_2 ((uint32_t)0x00000080) /* 位2 */
#define ADC_SQ14_3 ((uint32_t)0x00000100) /* 位3 */
#define ADC_SQ14_4 ((uint32_t)0x00000200) /* 位4 */

#define ADC_SQ15 ((uint32_t)0x00007C00)   /* SQ15[4:0] 位 (规则序列中第15个转换的通道号) */
#define ADC_SQ15_0 ((uint32_t)0x00000400) /* 位0 */
#define ADC_SQ15_1 ((uint32_t)0x00000800) /* 位1 */
#define ADC_SQ15_2 ((uint32_t)0x00001000) /* 位2 */
#define ADC_SQ15_3 ((uint32_t)0x00002000) /* 位3 */
#define ADC_SQ15_4 ((uint32_t)0x00004000) /* 位4 */

#define ADC_SQ16 ((uint32_t)0x000F8000)   /* SQ16[4:0] 位 (规则序列中第16个转换的通道号) */
#define ADC_SQ16_0 ((uint32_t)0x00008000) /* 位0 */
#define ADC_SQ16_1 ((uint32_t)0x00010000) /* 位1 */
#define ADC_SQ16_2 ((uint32_t)0x00020000) /* 位2 */
#define ADC_SQ16_3 ((uint32_t)0x00040000) /* 位3 */
#define ADC_SQ16_4 ((uint32_t)0x00080000) /* 位4 */

#define ADC_L ((uint32_t)0x00F00000)   /* L[3:0] 位 (规则通道序列长度) */
#define ADC_L_0 ((uint32_t)0x00100000) /* 位0 */
#define ADC_L_1 ((uint32_t)0x00200000) /* 位1 */
#define ADC_L_2 ((uint32_t)0x00400000) /* 位2 */
#define ADC_L_3 ((uint32_t)0x00800000) /* 位3 */

/*******************  ADC_RSQR2 寄存器位定义  *******************/
#define ADC_SQ7 ((uint32_t)0x0000001F)   /* SQ7[4:0] 位 (规则序列中第7个转换的通道号) */
#define ADC_SQ7_0 ((uint32_t)0x00000001) /* 位0 */
#define ADC_SQ7_1 ((uint32_t)0x00000002) /* 位1 */
#define ADC_SQ7_2 ((uint32_t)0x00000004) /* 位2 */
#define ADC_SQ7_3 ((uint32_t)0x00000008) /* 位3 */
#define ADC_SQ7_4 ((uint32_t)0x00000010) /* 位4 */

#define ADC_SQ8 ((uint32_t)0x000003E0)   /* SQ8[4:0] 位 (规则序列中第8个转换的通道号) */
#define ADC_SQ8_0 ((uint32_t)0x00000020) /* 位0 */
#define ADC_SQ8_1 ((uint32_t)0x00000040) /* 位1 */
#define ADC_SQ8_2 ((uint32_t)0x00000080) /* 位2 */
#define ADC_SQ8_3 ((uint32_t)0x00000100) /* 位3 */
#define ADC_SQ8_4 ((uint32_t)0x00000200) /* 位4 */

#define ADC_SQ9 ((uint32_t)0x00007C00)   /* SQ9[4:0] 位 (规则序列中第9个转换的通道号) */
#define ADC_SQ9_0 ((uint32_t)0x00000400) /* 位0 */
#define ADC_SQ9_1 ((uint32_t)0x00000800) /* 位1 */
#define ADC_SQ9_2 ((uint32_t)0x00001000) /* 位2 */
#define ADC_SQ9_3 ((uint32_t)0x00002000) /* 位3 */
#define ADC_SQ9_4 ((uint32_t)0x00004000) /* 位4 */

#define ADC_SQ10 ((uint32_t)0x000F8000)   /* SQ10[4:0] 位 (规则序列中第10个转换的通道号) */
#define ADC_SQ10_0 ((uint32_t)0x00008000) /* 位0 */
#define ADC_SQ10_1 ((uint32_t)0x00010000) /* 位1 */
#define ADC_SQ10_2 ((uint32_t)0x00020000) /* 位2 */
#define ADC_SQ10_3 ((uint32_t)0x00040000) /* 位3 */
#define ADC_SQ10_4 ((uint32_t)0x00080000) /* 位4 */

#define ADC_SQ11 ((uint32_t)0x01F00000)   /* SQ11[4:0] 位 (规则序列中第11个转换的通道号) */
#define ADC_SQ11_0 ((uint32_t)0x00100000) /* 位0 */
#define ADC_SQ11_1 ((uint32_t)0x00200000) /* 位1 */
#define ADC_SQ11_2 ((uint32_t)0x00400000) /* 位2 */
#define ADC_SQ11_3 ((uint32_t)0x00800000) /* 位3 */
#define ADC_SQ11_4 ((uint32_t)0x01000000) /* 位4 */

#define ADC_SQ12 ((uint32_t)0x3E000000)   /* SQ12[4:0] 位 (规则序列中第12个转换的通道号) */
#define ADC_SQ12_0 ((uint32_t)0x02000000) /* 位0 */
#define ADC_SQ12_1 ((uint32_t)0x04000000) /* 位1 */
#define ADC_SQ12_2 ((uint32_t)0x08000000) /* 位2 */
#define ADC_SQ12_3 ((uint32_t)0x10000000) /* 位3 */
#define ADC_SQ12_4 ((uint32_t)0x20000000) /* 位4 */

/*******************  ADC_RSQR3 寄存器位定义  *******************/
#define ADC_SQ1 ((uint32_t)0x0000001F)   /* SQ1[4:0] 位 (规则序列中第1个转换的通道号) */
#define ADC_SQ1_0 ((uint32_t)0x00000001) /* 位0 */
#define ADC_SQ1_1 ((uint32_t)0x00000002) /* 位1 */
#define ADC_SQ1_2 ((uint32_t)0x00000004) /* 位2 */
#define ADC_SQ1_3 ((uint32_t)0x00000008) /* 位3 */
#define ADC_SQ1_4 ((uint32_t)0x00000010) /* 位4 */

#define ADC_SQ2 ((uint32_t)0x000003E0)   /* SQ2[4:0] 位 (规则序列中第2个转换的通道号) */
#define ADC_SQ2_0 ((uint32_t)0x00000020) /* 位0 */
#define ADC_SQ2_1 ((uint32_t)0x00000040) /* 位1 */
#define ADC_SQ2_2 ((uint32_t)0x00000080) /* 位2 */
#define ADC_SQ2_3 ((uint32_t)0x00000100) /* 位3 */
#define ADC_SQ2_4 ((uint32_t)0x00000200) /* 位4 */

#define ADC_SQ3 ((uint32_t)0x00007C00)   /* SQ3[4:0] 位 (规则序列中第3个转换的通道号) */
#define ADC_SQ3_0 ((uint32_t)0x00000400) /* 位0 */
#define ADC_SQ3_1 ((uint32_t)0x00000800) /* 位1 */
#define ADC_SQ3_2 ((uint32_t)0x00001000) /* 位2 */
#define ADC_SQ3_3 ((uint32_t)0x00002000) /* 位3 */
#define ADC_SQ3_4 ((uint32_t)0x00004000) /* 位4 */

#define ADC_SQ4 ((uint32_t)0x000F8000)   /* SQ4[4:0] 位 (规则序列中第4个转换的通道号) */
#define ADC_SQ4_0 ((uint32_t)0x00008000) /* 位0 */
#define ADC_SQ4_1 ((uint32_t)0x00010000) /* 位1 */
#define ADC_SQ4_2 ((uint32_t)0x00020000) /* 位2 */
#define ADC_SQ4_3 ((uint32_t)0x00040000) /* 位3 */
#define ADC_SQ4_4 ((uint32_t)0x00080000) /* 位4 */

#define ADC_SQ5 ((uint32_t)0x01F00000)   /* SQ5[4:0] 位 (规则序列中第5个转换的通道号) */
#define ADC_SQ5_0 ((uint32_t)0x00100000) /* 位0 */
#define ADC_SQ5_1 ((uint32_t)0x00200000) /* 位1 */
#define ADC_SQ5_2 ((uint32_t)0x00400000) /* 位2 */
#define ADC_SQ5_3 ((uint32_t)0x00800000) /* 位3 */
#define ADC_SQ5_4 ((uint32_t)0x01000000) /* 位4 */

#define ADC_SQ6 ((uint32_t)0x3E000000)   /* SQ6[4:0] 位 (规则序列中第6个转换的通道号) */
#define ADC_SQ6_0 ((uint32_t)0x02000000) /* 位0 */
#define ADC_SQ6_1 ((uint32_t)0x04000000) /* 位1 */
#define ADC_SQ6_2 ((uint32_t)0x08000000) /* 位2 */
#define ADC_SQ6_3 ((uint32_t)0x10000000) /* 位3 */
#define ADC_SQ6_4 ((uint32_t)0x20000000) /* 位4 */

/*******************  ADC_ISQR 寄存器位定义  *******************/
#define ADC_JSQ1 ((uint32_t)0x0000001F)   /* JSQ1[4:0] 位 (注入序列中第1个转换的通道号) */
#define ADC_JSQ1_0 ((uint32_t)0x00000001) /* 位0 */
#define ADC_JSQ1_1 ((uint32_t)0x00000002) /* 位1 */
#define ADC_JSQ1_2 ((uint32_t)0x00000004) /* 位2 */
#define ADC_JSQ1_3 ((uint32_t)0x00000008) /* 位3 */
#define ADC_JSQ1_4 ((uint32_t)0x00000010) /* 位4 */

#define ADC_JSQ2 ((uint32_t)0x000003E0)   /* JSQ2[4:0] 位 (注入序列中第2个转换的通道号) */
#define ADC_JSQ2_0 ((uint32_t)0x00000020) /* 位0 */
#define ADC_JSQ2_1 ((uint32_t)0x00000040) /* 位1 */
#define ADC_JSQ2_2 ((uint32_t)0x00000080) /* 位2 */
#define ADC_JSQ2_3 ((uint32_t)0x00000100) /* 位3 */
#define ADC_JSQ2_4 ((uint32_t)0x00000200) /* 位4 */

#define ADC_JSQ3 ((uint32_t)0x00007C00)   /* JSQ3[4:0] 位 (注入序列中第3个转换的通道号) */
#define ADC_JSQ3_0 ((uint32_t)0x00000400) /* 位0 */
#define ADC_JSQ3_1 ((uint32_t)0x00000800) /* 位1 */
#define ADC_JSQ3_2 ((uint32_t)0x00001000) /* 位2 */
#define ADC_JSQ3_3 ((uint32_t)0x00002000) /* 位3 */
#define ADC_JSQ3_4 ((uint32_t)0x00004000) /* 位4 */

#define ADC_JSQ4 ((uint32_t)0x000F8000)   /* JSQ4[4:0] 位 (注入序列中第4个转换的通道号) */
#define ADC_JSQ4_0 ((uint32_t)0x00008000) /* 位0 */
#define ADC_JSQ4_1 ((uint32_t)0x00010000) /* 位1 */
#define ADC_JSQ4_2 ((uint32_t)0x00020000) /* 位2 */
#define ADC_JSQ4_3 ((uint32_t)0x00040000) /* 位3 */
#define ADC_JSQ4_4 ((uint32_t)0x00080000) /* 位4 */

#define ADC_JL ((uint32_t)0x00300000)   /* JL[1:0] 位 (注入序列长度) */
#define ADC_JL_0 ((uint32_t)0x00100000) /* 位0 */
#define ADC_JL_1 ((uint32_t)0x00200000) /* 位1 */

/*******************  ADC_IDATAR1 寄存器位定义  *******************/
#define ADC_IDATAR1_JDATA ((uint16_t)0xFFFF) /* 注入数据 (通道1) */

/*******************  ADC_IDATAR2 寄存器位定义  *******************/
#define ADC_IDATAR2_JDATA ((uint16_t)0xFFFF) /* 注入数据 (通道2) */

/*******************  ADC_IDATAR3 寄存器位定义  *******************/
#define ADC_IDATAR3_JDATA ((uint16_t)0xFFFF) /* 注入数据 (通道3) */

/*******************  ADC_IDATAR4 寄存器位定义  *******************/
#define ADC_IDATAR4_JDATA ((uint16_t)0xFFFF) /* 注入数据 (通道4) */

/********************  ADC_RDATAR 寄存器位定义  ********************/
#define ADC_RDATAR_DATA ((uint32_t)0x0000FFFF)     /* 规则数据 (ADC1) */
#define ADC_RDATAR_ADC2DATA ((uint32_t)0xFFFF0000) /* ADC2 数据 (双模式下) */

/********************  ADC_AUX 寄存器位定义  ********************/
#define ADC_SMP_SEL_0 ((uint32_t)0x00000001)  /* 通道0 采样时间选择 (辅助位) */
#define ADC_SMP_SEL_1 ((uint32_t)0x00000002)  /* 通道1 */
#define ADC_SMP_SEL_2 ((uint32_t)0x00000004)  /* 通道2 */
#define ADC_SMP_SEL_3 ((uint32_t)0x00000008)  /* 通道3 */
#define ADC_SMP_SEL_4 ((uint32_t)0x00000010)  /* 通道4 */
#define ADC_SMP_SEL_5 ((uint32_t)0x00000020)  /* 通道5 */
#define ADC_SMP_SEL_6 ((uint32_t)0x00000040)  /* 通道6 */
#define ADC_SMP_SEL_7 ((uint32_t)0x00000080)  /* 通道7 */
#define ADC_SMP_SEL_8 ((uint32_t)0x00000100)  /* 通道8 */
#define ADC_SMP_SEL_9 ((uint32_t)0x00000200)  /* 通道9 */
#define ADC_SMP_SEL_10 ((uint32_t)0x00000400) /* 通道10 */
#define ADC_SMP_SEL_11 ((uint32_t)0x00000800) /* 通道11 */
#define ADC_SMP_SEL_12 ((uint32_t)0x00001000) /* 通道12 */
#define ADC_SMP_SEL_13 ((uint32_t)0x00002000) /* 通道13 */
#define ADC_SMP_SEL_14 ((uint32_t)0x00004000) /* 通道14 */
#define ADC_SMP_SEL_15 ((uint32_t)0x00008000) /* 通道15 */
#define ADC_SMP_SEL_16 ((uint32_t)0x00010000) /* 通道16 */
#define ADC_SMP_SEL_17 ((uint32_t)0x00020000) /* 通道17 */

/******************************************************************************/
/*                            备份寄存器 (BKP)                                 */
/******************************************************************************/

/*******************  BKP_DATAR1 寄存器位定义  ********************/
#define BKP_DATAR1_D ((uint16_t)0xFFFF) /* 备份数据 (16位) */

/*******************  BKP_DATAR2 寄存器位定义  ********************/
#define BKP_DATAR2_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR3 寄存器位定义  ********************/
#define BKP_DATAR3_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR4 寄存器位定义  ********************/
#define BKP_DATAR4_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR5 寄存器位定义  ********************/
#define BKP_DATAR5_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR6 寄存器位定义  ********************/
#define BKP_DATAR6_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR7 寄存器位定义  ********************/
#define BKP_DATAR7_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR8 寄存器位定义  ********************/
#define BKP_DATAR8_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR9 寄存器位定义  ********************/
#define BKP_DATAR9_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR10 寄存器位定义  *******************/
#define BKP_DATAR10_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR11 寄存器位定义  *******************/
#define BKP_DATAR11_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR12 寄存器位定义  *******************/
#define BKP_DATAR12_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR13 寄存器位定义  *******************/
#define BKP_DATAR13_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR14 寄存器位定义  *******************/
#define BKP_DATAR14_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR15 寄存器位定义  *******************/
#define BKP_DATAR15_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR16 寄存器位定义  *******************/
#define BKP_DATAR16_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR17 寄存器位定义  *******************/
#define BKP_DATAR17_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************  BKP_DATAR18 寄存器位定义  ********************/
#define BKP_DATAR18_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR19 寄存器位定义  *******************/
#define BKP_DATAR19_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR20 寄存器位定义  *******************/
#define BKP_DATAR20_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR21 寄存器位定义  *******************/
#define BKP_DATAR21_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR22 寄存器位定义  *******************/
#define BKP_DATAR22_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR23 寄存器位定义  *******************/
#define BKP_DATAR23_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR24 寄存器位定义  *******************/
#define BKP_DATAR24_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR25 寄存器位定义  *******************/
#define BKP_DATAR25_D ((uint16_t)0xFFFF) /* 备份数据 */

/*******************  BKP_DATAR26 寄存器位定义  *******************/
#define BKP_DATAR26_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR27 寄存器位定义 *******************/
#define BKP_DATAR27_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR28 寄存器位定义 *******************/
#define BKP_DATAR28_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR29 寄存器位定义 *******************/
#define BKP_DATAR29_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR30 寄存器位定义 *******************/
#define BKP_DATAR30_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR31 寄存器位定义 *******************/
#define BKP_DATAR31_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR32 寄存器位定义 *******************/
#define BKP_DATAR32_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR33 寄存器位定义 *******************/
#define BKP_DATAR33_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR34 寄存器位定义 *******************/
#define BKP_DATAR34_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR35 寄存器位定义 *******************/
#define BKP_DATAR35_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR36 寄存器位定义 *******************/
#define BKP_DATAR36_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR37 寄存器位定义 *******************/
#define BKP_DATAR37_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR38 寄存器位定义 *******************/
#define BKP_DATAR38_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR39 寄存器位定义 *******************/
#define BKP_DATAR39_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR40 寄存器位定义 *******************/
#define BKP_DATAR40_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR41 寄存器位定义 *******************/
#define BKP_DATAR41_D ((uint16_t)0xFFFF) /* 备份数据 */

/******************* BKP_DATAR42 寄存器位定义 *******************/
#define BKP_DATAR42_D ((uint16_t)0xFFFF) /* 备份数据 */

/****************** BKP_OCTLR 寄存器位定义 *******************/
#define BKP_CAL ((uint16_t)0x007F)  /* 校准值 (用于 RTC 校准) */
#define BKP_CCO ((uint16_t)0x0080)  /* 校准时钟输出使能 */
#define BKP_ASOE ((uint16_t)0x0100) /* 闹钟或秒脉冲输出使能 */
#define BKP_ASOS ((uint16_t)0x0200) /* 闹钟或秒脉冲输出选择 */

/******************** BKP_TPCTLR 寄存器位定义 ********************/
#define BKP_TPE ((uint8_t)0x01)  /* 篡改引脚使能 */
#define BKP_TPAL ((uint8_t)0x02) /* 篡改引脚有效电平 (0: 低电平有效, 1: 高电平有效) */

/******************* BKP_TPCSR 寄存器位定义 ********************/
#define BKP_CTE ((uint16_t)0x0001)  /* 清除篡改事件 */
#define BKP_CTI ((uint16_t)0x0002)  /* 清除篡改中断 */
#define BKP_TPIE ((uint16_t)0x0004) /* 篡改引脚中断使能 */
#define BKP_TEF ((uint16_t)0x0100)  /* 篡改事件标志 */
#define BKP_TIF ((uint16_t)0x0200)  /* 篡改中断标志 */

/******************************************************************************/
/*                         控制器区域网络 (CAN)                                */
/******************************************************************************/

/*******************  CAN_CTLR 寄存器的位定义  ********************/
#define CAN_CTLR_INRQ ((uint16_t)0x0001)  /* 初始化请求 */
#define CAN_CTLR_SLEEP ((uint16_t)0x0002) /* 睡眠模式请求 */
#define CAN_CTLR_TXFP ((uint16_t)0x0004)  /* 发送FIFO优先级 */
#define CAN_CTLR_RFLM ((uint16_t)0x0008)  /* 接收FIFO锁定模式 */
#define CAN_CTLR_NART ((uint16_t)0x0010)  /* 禁止自动重传 */
#define CAN_CTLR_AWUM ((uint16_t)0x0020)  /* 自动唤醒模式 */
#define CAN_CTLR_ABOM ((uint16_t)0x0040)  /* 自动总线关闭管理 */
#define CAN_CTLR_TTCM ((uint16_t)0x0080)  /* 时间触发通信模式 */
#define CAN_CTLR_RESET ((uint16_t)0x8000) /* CAN软件主复位 */
#define CAN_CTLR_DBF ((uint32_t)0x10000)  /* 调试冻结位 */

/*******************  CAN_STATR 寄存器的位定义  ********************/
#define CAN_STATR_INAK ((uint16_t)0x0001)  /* 初始化应答 */
#define CAN_STATR_SLAK ((uint16_t)0x0002)  /* 睡眠应答 */
#define CAN_STATR_ERRI ((uint16_t)0x0004)  /* 错误中断 */
#define CAN_STATR_WKUI ((uint16_t)0x0008)  /* 唤醒中断 */
#define CAN_STATR_SLAKI ((uint16_t)0x0010) /* 睡眠应答中断 */
#define CAN_STATR_TXM ((uint16_t)0x0100)   /* 发送模式 */
#define CAN_STATR_RXM ((uint16_t)0x0200)   /* 接收模式 */
#define CAN_STATR_SAMP ((uint16_t)0x0400)  /* 最后采样点 */
#define CAN_STATR_RX ((uint16_t)0x0800)    /* CAN Rx 信号 */

/*******************  CAN_TSTATR 寄存器的位定义  ********************/
#define CAN_TSTATR_RQCP0 ((uint32_t)0x00000001) /* 邮箱0请求完成 */
#define CAN_TSTATR_TXOK0 ((uint32_t)0x00000002) /* 邮箱0发送成功 */
#define CAN_TSTATR_ALST0 ((uint32_t)0x00000004) /* 邮箱0仲裁丢失 */
#define CAN_TSTATR_TERR0 ((uint32_t)0x00000008) /* 邮箱0发送错误 */
#define CAN_TSTATR_ABRQ0 ((uint32_t)0x00000080) /* 邮箱0中止请求 */
#define CAN_TSTATR_RQCP1 ((uint32_t)0x00000100) /* 邮箱1请求完成 */
#define CAN_TSTATR_TXOK1 ((uint32_t)0x00000200) /* 邮箱1发送成功 */
#define CAN_TSTATR_ALST1 ((uint32_t)0x00000400) /* 邮箱1仲裁丢失 */
#define CAN_TSTATR_TERR1 ((uint32_t)0x00000800) /* 邮箱1发送错误 */
#define CAN_TSTATR_ABRQ1 ((uint32_t)0x00008000) /* 邮箱1中止请求 */
#define CAN_TSTATR_RQCP2 ((uint32_t)0x00010000) /* 邮箱2请求完成 */
#define CAN_TSTATR_TXOK2 ((uint32_t)0x00020000) /* 邮箱2发送成功 */
#define CAN_TSTATR_ALST2 ((uint32_t)0x00040000) /* 邮箱2仲裁丢失 */
#define CAN_TSTATR_TERR2 ((uint32_t)0x00080000) /* 邮箱2发送错误 */
#define CAN_TSTATR_ABRQ2 ((uint32_t)0x00800000) /* 邮箱2中止请求 */
#define CAN_TSTATR_CODE ((uint32_t)0x03000000)  /* 邮箱编码 */

#define CAN_TSTATR_TME ((uint32_t)0x1C000000)  /* TME[2:0] 位：发送邮箱空标志组合 */
#define CAN_TSTATR_TME0 ((uint32_t)0x04000000) /* 发送邮箱0空 */
#define CAN_TSTATR_TME1 ((uint32_t)0x08000000) /* 发送邮箱1空 */
#define CAN_TSTATR_TME2 ((uint32_t)0x10000000) /* 发送邮箱2空 */

#define CAN_TSTATR_LOW ((uint32_t)0xE0000000)  /* LOW[2:0] 位：最低优先级标志组合 */
#define CAN_TSTATR_LOW0 ((uint32_t)0x20000000) /* 邮箱0最低优先级标志 */
#define CAN_TSTATR_LOW1 ((uint32_t)0x40000000) /* 邮箱1最低优先级标志 */
#define CAN_TSTATR_LOW2 ((uint32_t)0x80000000) /* 邮箱2最低优先级标志 */

/*******************  CAN_RFIFO0 寄存器的位定义  *******************/
#define CAN_RFIFO0_FMP0 ((uint8_t)0x03)  /* FIFO 0 消息等待数 */
#define CAN_RFIFO0_FULL0 ((uint8_t)0x08) /* FIFO 0 满 */
#define CAN_RFIFO0_FOVR0 ((uint8_t)0x10) /* FIFO 0 溢出 */
#define CAN_RFIFO0_RFOM0 ((uint8_t)0x20) /* 释放FIFO 0输出邮箱 */

/*******************  CAN_RFIFO1 寄存器的位定义  *******************/
#define CAN_RFIFO1_FMP1 ((uint8_t)0x03)  /* FIFO 1 消息等待数 */
#define CAN_RFIFO1_FULL1 ((uint8_t)0x08) /* FIFO 1 满 */
#define CAN_RFIFO1_FOVR1 ((uint8_t)0x10) /* FIFO 1 溢出 */
#define CAN_RFIFO1_RFOM1 ((uint8_t)0x20) /* 释放FIFO 1输出邮箱 */

/********************  CAN_INTENR 寄存器的位定义  *******************/
#define CAN_INTENR_TMEIE ((uint32_t)0x00000001)  /* 发送邮箱空中断使能 */
#define CAN_INTENR_FMPIE0 ((uint32_t)0x00000002) /* FIFO0消息等待中断使能 */
#define CAN_INTENR_FFIE0 ((uint32_t)0x00000004)  /* FIFO0满中断使能 */
#define CAN_INTENR_FOVIE0 ((uint32_t)0x00000008) /* FIFO0溢出中断使能 */
#define CAN_INTENR_FMPIE1 ((uint32_t)0x00000010) /* FIFO1消息等待中断使能 */
#define CAN_INTENR_FFIE1 ((uint32_t)0x00000020)  /* FIFO1满中断使能 */
#define CAN_INTENR_FOVIE1 ((uint32_t)0x00000040) /* FIFO1溢出中断使能 */
#define CAN_INTENR_EWGIE ((uint32_t)0x00000100)  /* 错误警告中断使能 */
#define CAN_INTENR_EPVIE ((uint32_t)0x00000200)  /* 错误被动中断使能 */
#define CAN_INTENR_BOFIE ((uint32_t)0x00000400)  /* 总线关闭中断使能 */
#define CAN_INTENR_LECIE ((uint32_t)0x00000800)  /* 最后错误代码中断使能 */
#define CAN_INTENR_ERRIE ((uint32_t)0x00008000)  /* 错误中断使能（总开关） */
#define CAN_INTENR_WKUIE ((uint32_t)0x00010000)  /* 唤醒中断使能 */
#define CAN_INTENR_SLKIE ((uint32_t)0x00020000)  /* 睡眠中断使能 */

/********************  CAN_ERRSR 寄存器的位定义  *******************/
#define CAN_ERRSR_EWGF ((uint32_t)0x00000001) /* 错误警告标志 */
#define CAN_ERRSR_EPVF ((uint32_t)0x00000002) /* 错误被动标志 */
#define CAN_ERRSR_BOFF ((uint32_t)0x00000004) /* 总线关闭标志 */

#define CAN_ERRSR_LEC ((uint32_t)0x00000070)   /* LEC[2:0] 位（最后错误代码） */
#define CAN_ERRSR_LEC_0 ((uint32_t)0x00000010) /* 位0 */
#define CAN_ERRSR_LEC_1 ((uint32_t)0x00000020) /* 位1 */
#define CAN_ERRSR_LEC_2 ((uint32_t)0x00000040) /* 位2 */

#define CAN_ERRSR_TEC ((uint32_t)0x00FF0000) /* 发送错误计数器低8位（9位计数器低8位） */
#define CAN_ERRSR_REC ((uint32_t)0xFF000000) /* 接收错误计数器 */

/*******************  CAN_TTCTLR 寄存器的位定义  ********************/
#define CAN_TTCTLR_TIMCMV ((uint32_t)0x0000FFFF) /* 时间比较值 */
#define CAN_TTCTLR_TIMRST ((uint32_t)0x00010000) /* 时间复位 */
#define CAN_TTCTLR_MODE ((uint32_t)0x00020000)   /* 时间触发模式选择 */

/*******************  CAN_TTCNT 寄存器的位定义  ********************/
#define CAN_TTCNT_TIMCNT ((uint32_t)0x0000FFFF) /* 时间计数器值 */

/******************  CAN_TXMI0R 寄存器的位定义  ********************/
#define CAN_TXMI0R_TXRQ ((uint32_t)0x00000001) /* 发送邮箱请求 */
#define CAN_TXMI0R_RTR ((uint32_t)0x00000002)  /* 远程传输请求 */
#define CAN_TXMI0R_IDE ((uint32_t)0x00000004)  /* 标识符扩展位 */
#define CAN_TXMI0R_EXID ((uint32_t)0x001FFFF8) /* 扩展标识符 */
#define CAN_TXMI0R_STID ((uint32_t)0xFFE00000) /* 标准标识符或扩展标识符高11位 */

/******************  CAN_TXMDT0R 寄存器的位定义  *******************/
#define CAN_TXMDT0R_DLC ((uint32_t)0x0000000F)  /* 数据长度码 */
#define CAN_TXMDT0R_TGT ((uint32_t)0x00000100)  /* 发送全局时间 */
#define CAN_TXMDT0R_TIME ((uint32_t)0xFFFF0000) /* 消息时间戳 */

/******************  CAN_TXMDL0R 寄存器的位定义  *******************/
#define CAN_TXMDL0R_DATA0 ((uint32_t)0x000000FF) /* 数据字节0 */
#define CAN_TXMDL0R_DATA1 ((uint32_t)0x0000FF00) /* 数据字节1 */
#define CAN_TXMDL0R_DATA2 ((uint32_t)0x00FF0000) /* 数据字节2 */
#define CAN_TXMDL0R_DATA3 ((uint32_t)0xFF000000) /* 数据字节3 */

/******************  CAN_TXMDH0R 寄存器的位定义  *******************/
#define CAN_TXMDH0R_DATA4 ((uint32_t)0x000000FF) /* 数据字节4 */
#define CAN_TXMDH0R_DATA5 ((uint32_t)0x0000FF00) /* 数据字节5 */
#define CAN_TXMDH0R_DATA6 ((uint32_t)0x00FF0000) /* 数据字节6 */
#define CAN_TXMDH0R_DATA7 ((uint32_t)0xFF000000) /* 数据字节7 */

/*******************  CAN_TXMI1R 寄存器的位定义  *******************/
#define CAN_TXMI1R_TXRQ ((uint32_t)0x00000001) /* 发送邮箱请求 */
#define CAN_TXMI1R_RTR ((uint32_t)0x00000002)  /* 远程传输请求 */
#define CAN_TXMI1R_IDE ((uint32_t)0x00000004)  /* 标识符扩展位 */
#define CAN_TXMI1R_EXID ((uint32_t)0x001FFFF8) /* 扩展标识符 */
#define CAN_TXMI1R_STID ((uint32_t)0xFFE00000) /* 标准标识符或扩展标识符高11位 */

/*******************  CAN_TXMDT1R 寄存器的位定义  ******************/
#define CAN_TXMDT1R_DLC ((uint32_t)0x0000000F)  /* 数据长度码 */
#define CAN_TXMDT1R_TGT ((uint32_t)0x00000100)  /* 发送全局时间 */
#define CAN_TXMDT1R_TIME ((uint32_t)0xFFFF0000) /* 消息时间戳 */

/*******************  CAN_TXMDL1R 寄存器的位定义  ******************/
#define CAN_TXMDL1R_DATA0 ((uint32_t)0x000000FF) /* 数据字节0 */
#define CAN_TXMDL1R_DATA1 ((uint32_t)0x0000FF00) /* 数据字节1 */
#define CAN_TXMDL1R_DATA2 ((uint32_t)0x00FF0000) /* 数据字节2 */
#define CAN_TXMDL1R_DATA3 ((uint32_t)0xFF000000) /* 数据字节3 */

/*******************  CAN_TXMDH1R 寄存器的位定义  ******************/
#define CAN_TXMDH1R_DATA4 ((uint32_t)0x000000FF) /* 数据字节4 */
#define CAN_TXMDH1R_DATA5 ((uint32_t)0x0000FF00) /* 数据字节5 */
#define CAN_TXMDH1R_DATA6 ((uint32_t)0x00FF0000) /* 数据字节6 */
#define CAN_TXMDH1R_DATA7 ((uint32_t)0xFF000000) /* 数据字节7 */

/*******************  CAN_TXMI2R 寄存器的位定义  *******************/
#define CAN_TXMI2R_TXRQ ((uint32_t)0x00000001) /* 发送邮箱请求 */
#define CAN_TXMI2R_RTR ((uint32_t)0x00000002)  /* 远程传输请求 */
#define CAN_TXMI2R_IDE ((uint32_t)0x00000004)  /* 标识符扩展位 */
#define CAN_TXMI2R_EXID ((uint32_t)0x001FFFF8) /* 扩展标识符 */
#define CAN_TXMI2R_STID ((uint32_t)0xFFE00000) /* 标准标识符或扩展标识符高11位 */

/*******************  CAN_TXMDT2R 寄存器的位定义  ******************/
#define CAN_TXMDT2R_DLC ((uint32_t)0x0000000F)  /* 数据长度码 */
#define CAN_TXMDT2R_TGT ((uint32_t)0x00000100)  /* 发送全局时间 */
#define CAN_TXMDT2R_TIME ((uint32_t)0xFFFF0000) /* 消息时间戳 */

/*******************  CAN_TXMDL2R 寄存器的位定义  ******************/
#define CAN_TXMDL2R_DATA0 ((uint32_t)0x000000FF) /* 数据字节0 */
#define CAN_TXMDL2R_DATA1 ((uint32_t)0x0000FF00) /* 数据字节1 */
#define CAN_TXMDL2R_DATA2 ((uint32_t)0x00FF0000) /* 数据字节2 */
#define CAN_TXMDL2R_DATA3 ((uint32_t)0xFF000000) /* 数据字节3 */

/*******************  CAN_TXMDH2R 寄存器的位定义  ******************/
#define CAN_TXMDH2R_DATA4 ((uint32_t)0x000000FF) /* 数据字节4 */
#define CAN_TXMDH2R_DATA5 ((uint32_t)0x0000FF00) /* 数据字节5 */
#define CAN_TXMDH2R_DATA6 ((uint32_t)0x00FF0000) /* 数据字节6 */
#define CAN_TXMDH2R_DATA7 ((uint32_t)0xFF000000) /* 数据字节7 */

/*******************  CAN_RXMI0R 寄存器的位定义  *******************/
#define CAN_RXMI0R_RTR ((uint32_t)0x00000002)  /* 远程传输请求 */
#define CAN_RXMI0R_IDE ((uint32_t)0x00000004)  /* 标识符扩展位 */
#define CAN_RXMI0R_EXID ((uint32_t)0x001FFFF8) /* 扩展标识符 */
#define CAN_RXMI0R_STID ((uint32_t)0xFFE00000) /* 标准标识符或扩展标识符高11位 */

/*******************  CAN_RXMDT0R 寄存器的位定义  ******************/
#define CAN_RXMDT0R_DLC ((uint32_t)0x0000000F)  /* 数据长度码 */
#define CAN_RXMDT0R_FMI ((uint32_t)0x0000FF00)  /* 过滤器匹配索引 */
#define CAN_RXMDT0R_TIME ((uint32_t)0xFFFF0000) /* 消息时间戳 */

/*******************  CAN_RXMDL0R 寄存器的位定义  ******************/
#define CAN_RXMDL0R_DATA0 ((uint32_t)0x000000FF) /* 数据字节0 */
#define CAN_RXMDL0R_DATA1 ((uint32_t)0x0000FF00) /* 数据字节1 */
#define CAN_RXMDL0R_DATA2 ((uint32_t)0x00FF0000) /* 数据字节2 */
#define CAN_RXMDL0R_DATA3 ((uint32_t)0xFF000000) /* 数据字节3 */

/*******************  CAN_RXMDH0R 寄存器的位定义  ******************/
#define CAN_RXMDH0R_DATA4 ((uint32_t)0x000000FF) /* 数据字节4 */
#define CAN_RXMDH0R_DATA5 ((uint32_t)0x0000FF00) /* 数据字节5 */
#define CAN_RXMDH0R_DATA6 ((uint32_t)0x00FF0000) /* 数据字节6 */
#define CAN_RXMDH0R_DATA7 ((uint32_t)0xFF000000) /* 数据字节7 */

/*******************  CAN_RXMI1R 寄存器的位定义  *******************/
#define CAN_RXMI1R_RTR ((uint32_t)0x00000002)  /* 远程传输请求 */
#define CAN_RXMI1R_IDE ((uint32_t)0x00000004)  /* 标识符扩展位 */
#define CAN_RXMI1R_EXID ((uint32_t)0x001FFFF8) /* 扩展标识符 */
#define CAN_RXMI1R_STID ((uint32_t)0xFFE00000) /* 标准标识符或扩展标识符高11位 */

/*******************  CAN_RXMDT1R 寄存器的位定义  ******************/
#define CAN_RXMDT1R_DLC ((uint32_t)0x0000000F)  /* 数据长度码 */
#define CAN_RXMDT1R_FMI ((uint32_t)0x0000FF00)  /* 过滤器匹配索引 */
#define CAN_RXMDT1R_TIME ((uint32_t)0xFFFF0000) /* 消息时间戳 */

/*******************  CAN_RXMDL1R 寄存器的位定义  ******************/
#define CAN_RXMDL1R_DATA0 ((uint32_t)0x000000FF) /* 数据字节0 */
#define CAN_RXMDL1R_DATA1 ((uint32_t)0x0000FF00) /* 数据字节1 */
#define CAN_RXMDL1R_DATA2 ((uint32_t)0x00FF0000) /* 数据字节2 */
#define CAN_RXMDL1R_DATA3 ((uint32_t)0xFF000000) /* 数据字节3 */

/*******************  CAN_RXMDH1R 寄存器的位定义  ******************/
#define CAN_RXMDH1R_DATA4 ((uint32_t)0x000000FF) /* 数据字节4 */
#define CAN_RXMDH1R_DATA5 ((uint32_t)0x0000FF00) /* 数据字节5 */
#define CAN_RXMDH1R_DATA6 ((uint32_t)0x00FF0000) /* 数据字节6 */
#define CAN_RXMDH1R_DATA7 ((uint32_t)0xFF000000) /* 数据字节7 */

/*******************  CAN_FCTLR 寄存器的位定义  ********************/
#define CAN_FCTLR_FINIT ((uint8_t)0x01) /* 过滤器初始化模式 */
#define CAN_FCTLR_CAN2SB ((uint16_t)0x3F00) /* CAN2起始过滤器号 */

/*******************  CAN_FMCFGR 寄存器的位定义  *******************/
#define CAN_FMCFGR_FBM ((uint16_t)0x3FFF)      /* 过滤器模式组合位 */
#define CAN_FMCFGR_FBM0 ((uint16_t)0x0001)     /* 过滤器0模式 */
#define CAN_FMCFGR_FBM1 ((uint16_t)0x0002)     /* 过滤器1模式 */
#define CAN_FMCFGR_FBM2 ((uint16_t)0x0004)     /* 过滤器2模式 */
#define CAN_FMCFGR_FBM3 ((uint16_t)0x0008)     /* 过滤器3模式 */
#define CAN_FMCFGR_FBM4 ((uint16_t)0x0010)     /* 过滤器4模式 */
#define CAN_FMCFGR_FBM5 ((uint16_t)0x0020)     /* 过滤器5模式 */
#define CAN_FMCFGR_FBM6 ((uint16_t)0x0040)     /* 过滤器6模式 */
#define CAN_FMCFGR_FBM7 ((uint16_t)0x0080)     /* 过滤器7模式 */
#define CAN_FMCFGR_FBM8 ((uint16_t)0x0100)     /* 过滤器8模式 */
#define CAN_FMCFGR_FBM9 ((uint16_t)0x0200)     /* 过滤器9模式 */
#define CAN_FMCFGR_FBM10 ((uint16_t)0x0400)    /* 过滤器10模式 */
#define CAN_FMCFGR_FBM11 ((uint16_t)0x0800)    /* 过滤器11模式 */
#define CAN_FMCFGR_FBM12 ((uint16_t)0x1000)    /* 过滤器12模式 */
#define CAN_FMCFGR_FBM13 ((uint16_t)0x2000)    /* 过滤器13模式 */
#define CAN_FMCFGR_FBM14 ((uint16_t)0x4000)    /* 过滤器14模式 */
#define CAN_FMCFGR_FBM15 ((uint16_t)0x8000)    /* 过滤器15模式 */
#define CAN_FMCFGR_FBM16 ((uint32_t)0x10000)   /* 过滤器16模式 */
#define CAN_FMCFGR_FBM17 ((uint32_t)0x20000)   /* 过滤器17模式 */
#define CAN_FMCFGR_FBM18 ((uint32_t)0x40000)   /* 过滤器18模式 */
#define CAN_FMCFGR_FBM19 ((uint32_t)0x80000)   /* 过滤器19模式 */
#define CAN_FMCFGR_FBM20 ((uint32_t)0x100000)  /* 过滤器20模式 */
#define CAN_FMCFGR_FBM21 ((uint32_t)0x200000)  /* 过滤器21模式 */
#define CAN_FMCFGR_FBM22 ((uint32_t)0x400000)  /* 过滤器22模式 */
#define CAN_FMCFGR_FBM23 ((uint32_t)0x800000)  /* 过滤器23模式 */
#define CAN_FMCFGR_FBM24 ((uint32_t)0x1000000) /* 过滤器24模式 */
#define CAN_FMCFGR_FBM25 ((uint32_t)0x2000000) /* 过滤器25模式 */
#define CAN_FMCFGR_FBM26 ((uint32_t)0x4000000) /* 过滤器26模式 */
#define CAN_FMCFGR_FBM27 ((uint32_t)0x8000000) /* 过滤器27模式 */

/*******************  CAN_FSCFGR 寄存器的位定义  *******************/
#define CAN_FSCFGR_FSC ((uint16_t)0x3FFF)      /* 过滤器缩放配置组合位 */
#define CAN_FSCFGR_FSC0 ((uint16_t)0x0001)     /* 过滤器0缩放配置 */
#define CAN_FSCFGR_FSC1 ((uint16_t)0x0002)     /* 过滤器1缩放配置 */
#define CAN_FSCFGR_FSC2 ((uint16_t)0x0004)     /* 过滤器2缩放配置 */
#define CAN_FSCFGR_FSC3 ((uint16_t)0x0008)     /* 过滤器3缩放配置 */
#define CAN_FSCFGR_FSC4 ((uint16_t)0x0010)     /* 过滤器4缩放配置 */
#define CAN_FSCFGR_FSC5 ((uint16_t)0x0020)     /* 过滤器5缩放配置 */
#define CAN_FSCFGR_FSC6 ((uint16_t)0x0040)     /* 过滤器6缩放配置 */
#define CAN_FSCFGR_FSC7 ((uint16_t)0x0080)     /* 过滤器7缩放配置 */
#define CAN_FSCFGR_FSC8 ((uint16_t)0x0100)     /* 过滤器8缩放配置 */
#define CAN_FSCFGR_FSC9 ((uint16_t)0x0200)     /* 过滤器9缩放配置 */
#define CAN_FSCFGR_FSC10 ((uint16_t)0x0400)    /* 过滤器10缩放配置 */
#define CAN_FSCFGR_FSC11 ((uint16_t)0x0800)    /* 过滤器11缩放配置 */
#define CAN_FSCFGR_FSC12 ((uint16_t)0x1000)    /* 过滤器12缩放配置 */
#define CAN_FSCFGR_FSC13 ((uint16_t)0x2000)    /* 过滤器13缩放配置 */
#define CAN_FSCFGR_FSC14 ((uint16_t)0x4000)    /* 过滤器14缩放配置 */
#define CAN_FSCFGR_FSC15 ((uint16_t)0x8000)    /* 过滤器15缩放配置 */
#define CAN_FSCFGR_FSC16 ((uint32_t)0x10000)   /* 过滤器16缩放配置 */
#define CAN_FSCFGR_FSC17 ((uint32_t)0x20000)   /* 过滤器17缩放配置 */
#define CAN_FSCFGR_FSC18 ((uint32_t)0x40000)   /* 过滤器18缩放配置 */
#define CAN_FSCFGR_FSC19 ((uint32_t)0x80000)   /* 过滤器19缩放配置 */
#define CAN_FSCFGR_FSC20 ((uint32_t)0x100000)  /* 过滤器20缩放配置 */
#define CAN_FSCFGR_FSC21 ((uint32_t)0x200000)  /* 过滤器21缩放配置 */
#define CAN_FSCFGR_FSC22 ((uint32_t)0x400000)  /* 过滤器22缩放配置 */
#define CAN_FSCFGR_FSC23 ((uint32_t)0x800000)  /* 过滤器23缩放配置 */
#define CAN_FSCFGR_FSC24 ((uint32_t)0x1000000) /* 过滤器24缩放配置 */
#define CAN_FSCFGR_FSC25 ((uint32_t)0x2000000) /* 过滤器25缩放配置 */
#define CAN_FSCFGR_FSC26 ((uint32_t)0x4000000) /* 过滤器26缩放配置 */
#define CAN_FSCFGR_FSC27 ((uint32_t)0x8000000) /* 过滤器27缩放配置 */

/******************  CAN_FAFIFOR 寄存器的位定义  *******************/
#define CAN_FAFIFOR_FFA ((uint16_t)0x3FFF)      /* 过滤器FIFO分配组合位 */
#define CAN_FAFIFOR_FFA0 ((uint16_t)0x0001)     /* 过滤器0分配到FIFO0 (0) 或 FIFO1 (1) */
#define CAN_FAFIFOR_FFA1 ((uint16_t)0x0002)     /* 过滤器1分配到FIFO0或FIFO1 */
#define CAN_FAFIFOR_FFA2 ((uint16_t)0x0004)     /* 过滤器2分配 */
#define CAN_FAFIFOR_FFA3 ((uint16_t)0x0008)     /* 过滤器3分配 */
#define CAN_FAFIFOR_FFA4 ((uint16_t)0x0010)     /* 过滤器4分配 */
#define CAN_FAFIFOR_FFA5 ((uint16_t)0x0020)     /* 过滤器5分配 */
#define CAN_FAFIFOR_FFA6 ((uint16_t)0x0040)     /* 过滤器6分配 */
#define CAN_FAFIFOR_FFA7 ((uint16_t)0x0080)     /* 过滤器7分配 */
#define CAN_FAFIFOR_FFA8 ((uint16_t)0x0100)     /* 过滤器8分配 */
#define CAN_FAFIFOR_FFA9 ((uint16_t)0x0200)     /* 过滤器9分配 */
#define CAN_FAFIFOR_FFA10 ((uint16_t)0x0400)    /* 过滤器10分配 */
#define CAN_FAFIFOR_FFA11 ((uint16_t)0x0800)    /* 过滤器11分配 */
#define CAN_FAFIFOR_FFA12 ((uint16_t)0x1000)    /* 过滤器12分配 */
#define CAN_FAFIFOR_FFA13 ((uint16_t)0x2000)    /* 过滤器13分配 */
#define CAN_FAFIFOR_FFA14 ((uint32_t)0x4000)    /* 过滤器14分配 */
#define CAN_FAFIFOR_FFA15 ((uint32_t)0x8000)    /* 过滤器15分配 */
#define CAN_FAFIFOR_FFA16 ((uint32_t)0x10000)   /* 过滤器16分配 */
#define CAN_FAFIFOR_FFA17 ((uint32_t)0x20000)   /* 过滤器17分配 */
#define CAN_FAFIFOR_FFA18 ((uint32_t)0x40000)   /* 过滤器18分配 */
#define CAN_FAFIFOR_FFA19 ((uint32_t)0x80000)   /* 过滤器19分配 */
#define CAN_FAFIFOR_FFA20 ((uint32_t)0x100000)  /* 过滤器20分配 */
#define CAN_FAFIFOR_FFA21 ((uint32_t)0x200000)  /* 过滤器21分配 */
#define CAN_FAFIFOR_FFA22 ((uint32_t)0x400000)  /* 过滤器22分配 */
#define CAN_FAFIFOR_FFA23 ((uint32_t)0x800000)  /* 过滤器23分配 */
#define CAN_FAFIFOR_FFA24 ((uint32_t)0x1000000) /* 过滤器24分配 */
#define CAN_FAFIFOR_FFA25 ((uint32_t)0x2000000) /* 过滤器25分配 */
#define CAN_FAFIFOR_FFA26 ((uint32_t)0x4000000) /* 过滤器26分配 */
#define CAN_FAFIFOR_FFA27 ((uint32_t)0x8000000) /* 过滤器27分配 */

/*******************  CAN_FWR 寄存器的位定义  *******************/
#define CAN_FWR_FACT ((uint16_t)0x3FFF)      /* 过滤器激活组合位 */
#define CAN_FWR_FACT0 ((uint16_t)0x0001)     /* 过滤器0激活 */
#define CAN_FWR_FACT1 ((uint16_t)0x0002)     /* 过滤器1激活 */
#define CAN_FWR_FACT2 ((uint16_t)0x0004)     /* 过滤器2激活 */
#define CAN_FWR_FACT3 ((uint16_t)0x0008)     /* 过滤器3激活 */
#define CAN_FWR_FACT4 ((uint16_t)0x0010)     /* 过滤器4激活 */
#define CAN_FWR_FACT5 ((uint16_t)0x0020)     /* 过滤器5激活 */
#define CAN_FWR_FACT6 ((uint16_t)0x0040)     /* 过滤器6激活 */
#define CAN_FWR_FACT7 ((uint16_t)0x0080)     /* 过滤器7激活 */
#define CAN_FWR_FACT8 ((uint16_t)0x0100)     /* 过滤器8激活 */
#define CAN_FWR_FACT9 ((uint16_t)0x0200)     /* 过滤器9激活 */
#define CAN_FWR_FACT10 ((uint16_t)0x0400)    /* 过滤器10激活 */
#define CAN_FWR_FACT11 ((uint16_t)0x0800)    /* 过滤器11激活 */
#define CAN_FWR_FACT12 ((uint16_t)0x1000)    /* 过滤器12激活 */
#define CAN_FWR_FACT13 ((uint16_t)0x2000)    /* 过滤器13激活 */
#define CAN_FWR_FACT14 ((uint16_t)0x4000)    /* 过滤器14激活 */
#define CAN_FWR_FACT15 ((uint16_t)0x8000)    /* 过滤器15激活 */
#define CAN_FWR_FACT16 ((uint32_t)0x10000)   /* 过滤器16激活 */
#define CAN_FWR_FACT17 ((uint32_t)0x20000)   /* 过滤器17激活 */
#define CAN_FWR_FACT18 ((uint32_t)0x40000)   /* 过滤器18激活 */
#define CAN_FWR_FACT19 ((uint32_t)0x80000)   /* 过滤器19激活 */
#define CAN_FWR_FACT20 ((uint32_t)0x100000)  /* 过滤器20激活 */
#define CAN_FWR_FACT21 ((uint32_t)0x200000)  /* 过滤器21激活 */
#define CAN_FWR_FACT22 ((uint32_t)0x400000)  /* 过滤器22激活 */
#define CAN_FWR_FACT23 ((uint32_t)0x800000)  /* 过滤器23激活 */
#define CAN_FWR_FACT24 ((uint32_t)0x1000000) /* 过滤器24激活 */
#define CAN_FWR_FACT25 ((uint32_t)0x2000000) /* 过滤器25激活 */
#define CAN_FWR_FACT26 ((uint32_t)0x4000000) /* 过滤器26激活 */
#define CAN_FWR_FACT27 ((uint32_t)0x8000000) /* 过滤器27激活 */

/*******************  CAN_F0R1 寄存器的位定义 (过滤器0，寄存器1)  *******************/
#define CAN_F0R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F0R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F0R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F0R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F0R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F0R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F0R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F0R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F0R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F0R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F0R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F0R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F0R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F0R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F0R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F0R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F0R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F0R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F0R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F0R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F0R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F0R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F0R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F0R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F0R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F0R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F0R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F0R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F0R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F0R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F0R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F0R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F1R1 寄存器的位定义 (过滤器1，寄存器1)  *******************/
#define CAN_F1R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F1R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F1R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F1R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F1R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F1R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F1R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F1R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F1R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F1R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F1R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F1R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F1R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F1R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F1R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F1R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F1R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F1R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F1R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F1R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F1R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F1R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F1R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F1R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F1R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F1R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F1R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F1R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F1R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F1R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F1R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F1R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F2R1 寄存器的位定义 (过滤器2，寄存器1)  *******************/
#define CAN_F2R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F2R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F2R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F2R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F2R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F2R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F2R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F2R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F2R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F2R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F2R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F2R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F2R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F2R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F2R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F2R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F2R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F2R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F2R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F2R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F2R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F2R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F2R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F2R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F2R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F2R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F2R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F2R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F2R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F2R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F2R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F2R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F3R1 寄存器的位定义 (过滤器3，寄存器1)  *******************/
#define CAN_F3R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F3R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F3R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F3R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F3R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F3R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F3R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F3R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F3R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F3R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F3R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F3R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F3R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F3R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F3R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F3R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F3R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F3R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F3R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F3R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F3R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F3R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F3R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F3R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F3R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F3R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F3R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F3R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F3R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F3R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F3R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F3R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F4R1 寄存器的位定义 (过滤器4，寄存器1)  *******************/
#define CAN_F4R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F4R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F4R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F4R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F4R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F4R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F4R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F4R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F4R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F4R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F4R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F4R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F4R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F4R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F4R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F4R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F4R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F4R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F4R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F4R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F4R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F4R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F4R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F4R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F4R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F4R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F4R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F4R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F4R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F4R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F4R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F4R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F5R1 寄存器的位定义 (过滤器5，寄存器1)  *******************/
#define CAN_F5R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F5R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F5R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F5R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F5R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F5R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F5R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F5R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F5R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F5R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F5R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F5R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F5R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F5R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F5R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F5R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F5R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F5R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F5R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F5R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F5R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F5R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F5R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F5R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F5R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F5R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F5R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F5R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F5R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F5R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F5R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F5R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F6R1 寄存器的位定义 (过滤器6，寄存器1)  *******************/
#define CAN_F6R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F6R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F6R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F6R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F6R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F6R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F6R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F6R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F6R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F6R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F6R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F6R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F6R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F6R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F6R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F6R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F6R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F6R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F6R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F6R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F6R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F6R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F6R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F6R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F6R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F6R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F6R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F6R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F6R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F6R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F6R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F6R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F7R1 寄存器的位定义 (过滤器7，寄存器1)  *******************/
#define CAN_F7R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F7R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F7R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F7R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F7R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F7R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F7R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F7R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F7R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F7R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F7R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F7R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F7R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F7R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F7R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F7R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F7R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F7R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F7R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F7R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F7R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F7R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F7R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F7R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F7R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F7R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F7R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F7R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F7R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F7R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F7R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F7R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F8R1 寄存器的位定义 (过滤器8，寄存器1)  *******************/
#define CAN_F8R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F8R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F8R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F8R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F8R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F8R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F8R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F8R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F8R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F8R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F8R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F8R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F8R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F8R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F8R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F8R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F8R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F8R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F8R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F8R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F8R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F8R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F8R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F8R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F8R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F8R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F8R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F8R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F8R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F8R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F8R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F8R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F9R1 寄存器的位定义 (过滤器9，寄存器1)  *******************/
#define CAN_F9R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F9R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F9R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F9R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F9R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F9R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F9R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F9R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F9R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F9R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F9R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F9R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F9R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F9R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F9R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F9R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F9R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F9R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F9R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F9R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F9R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F9R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F9R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F9R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F9R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F9R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F9R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F9R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F9R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F9R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F9R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F9R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F10R1 寄存器的位定义 (过滤器10，寄存器1)  ******************/
#define CAN_F10R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F10R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F10R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F10R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F10R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F10R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F10R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F10R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F10R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F10R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F10R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F10R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F10R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F10R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F10R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F10R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F10R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F10R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F10R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F10R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F10R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F10R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F10R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F10R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F10R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F10R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F10R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F10R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F10R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F10R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F10R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F10R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F11R1 寄存器的位定义 (过滤器11，寄存器1)  ******************/
#define CAN_F11R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F11R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F11R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F11R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F11R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F11R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F11R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F11R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F11R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F11R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F11R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F11R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F11R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F11R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F11R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F11R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F11R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F11R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F11R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F11R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F11R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F11R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F11R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F11R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F11R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F11R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F11R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F11R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F11R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F11R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F11R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F11R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F12R1 寄存器的位定义 (过滤器12，寄存器1)  ******************/
#define CAN_F12R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F12R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F12R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F12R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F12R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F12R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F12R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F12R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F12R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F12R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F12R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F12R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F12R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F12R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F12R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F12R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F12R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F12R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F12R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F12R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F12R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F12R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F12R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F12R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F12R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F12R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F12R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F12R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F12R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F12R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F12R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F12R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F13R1 寄存器的位定义 (过滤器13，寄存器1)  ******************/
#define CAN_F13R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F13R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F13R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F13R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F13R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F13R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F13R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F13R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F13R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F13R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F13R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F13R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F13R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F13R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F13R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F13R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F13R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F13R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F13R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F13R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F13R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F13R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F13R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F13R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F13R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F13R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F13R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F13R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F13R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F13R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F13R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F13R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F14R1 寄存器的位定义 (过滤器14，寄存器1)  ******************/
#define CAN_F14R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F14R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F14R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F14R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F14R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F14R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F14R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F14R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F14R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F14R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F14R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F14R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F14R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F14R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F14R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F14R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F14R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F14R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F14R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F14R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F14R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F14R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F14R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F14R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F14R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F14R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F14R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F14R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F14R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F14R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F14R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F14R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F15R1 寄存器的位定义 (过滤器15，寄存器1)  *******************/
#define CAN_F15R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F15R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F15R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F15R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F15R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F15R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F15R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F15R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F15R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F15R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F15R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F15R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F15R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F15R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F15R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F15R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F15R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F15R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F15R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F15R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F15R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F15R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F15R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F15R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F15R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F15R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F15R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F15R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F15R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F15R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F15R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F15R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F16R1 寄存器的位定义 (过滤器16，寄存器1)  *******************/
#define CAN_F16R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F16R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F16R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F16R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F16R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F16R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F16R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F16R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F16R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F16R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F16R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F16R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F16R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F16R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F16R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F16R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F16R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F16R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F16R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F16R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F16R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F16R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F16R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F16R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F16R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F16R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F16R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F16R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F16R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F16R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F16R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F16R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F17R1 寄存器的位定义 (过滤器17，寄存器1)  *******************/
#define CAN_F17R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F17R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F17R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F17R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F17R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F17R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F17R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F17R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F17R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F17R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F17R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F17R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F17R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F17R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F17R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F17R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F17R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F17R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F17R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F17R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F17R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F17R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F17R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F17R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F17R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F17R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F17R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F17R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F17R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F17R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F17R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F17R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F18R1 寄存器的位定义 (过滤器18，寄存器1)  *******************/
#define CAN_F18R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F18R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F18R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F18R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F18R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F18R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F18R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F18R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F18R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F18R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F18R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F18R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F18R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F18R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F18R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F18R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F18R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F18R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F18R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F18R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F18R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F18R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F18R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F18R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F18R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F18R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F18R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F18R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F18R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F18R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F18R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F18R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F19R1 寄存器的位定义 (过滤器19，寄存器1)  *******************/
#define CAN_F19R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F19R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F19R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F19R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F19R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F19R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F19R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F19R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F19R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F19R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F19R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F19R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F19R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F19R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F19R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F19R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F19R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F19R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F19R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F19R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F19R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F19R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F19R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F19R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F19R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F19R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F19R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F19R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F19R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F19R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F19R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F19R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F20R1 寄存器的位定义 (过滤器20，寄存器1)  *******************/
#define CAN_F20R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F20R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F20R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F20R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F20R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F20R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F20R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F20R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F20R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F20R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F20R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F20R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F20R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F20R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F20R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F20R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F20R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F20R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F20R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F20R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F20R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F20R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F20R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F20R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F20R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F20R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F20R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F20R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F20R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F20R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F20R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F20R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F21R1 寄存器的位定义 (过滤器21，寄存器1)  *******************/
#define CAN_F21R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F21R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F21R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F21R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F21R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F21R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F21R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F21R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F21R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F21R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F21R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F21R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F21R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F21R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F21R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F21R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F21R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F21R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F21R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F21R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F21R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F21R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F21R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F21R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F21R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F21R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F21R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F21R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F21R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F21R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F21R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F21R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F22R1 寄存器的位定义 (过滤器22，寄存器1)  *******************/
#define CAN_F22R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F22R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F22R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F22R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F22R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F22R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F22R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F22R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F22R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F22R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F22R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F22R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F22R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F22R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F22R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F22R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F22R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F22R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F22R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F22R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F22R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F22R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F22R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F22R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F22R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F22R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F22R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F22R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F22R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F22R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F22R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F22R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F23R1 寄存器的位定义 (过滤器23，寄存器1)  ******************/
#define CAN_F23R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F23R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F23R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F23R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F23R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F23R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F23R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F23R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F23R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F23R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F23R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F23R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F23R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F23R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F23R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F23R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F23R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F23R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F23R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F23R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F23R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F23R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F23R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F23R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F23R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F23R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F23R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F23R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F23R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F23R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F23R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F23R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F24R1 寄存器的位定义 (过滤器24，寄存器1)  ******************/
#define CAN_F24R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F24R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F24R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F24R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F24R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F24R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F24R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F24R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F24R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F24R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F24R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F24R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F24R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F24R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F24R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F24R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F24R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F24R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F24R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F24R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F24R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F24R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F24R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F24R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F24R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F24R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F24R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F24R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F24R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F24R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F24R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F24R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F25R1 寄存器的位定义 (过滤器25，寄存器1)  ******************/
#define CAN_F25R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F25R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F25R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F25R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F25R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F25R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F25R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F25R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F25R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F25R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F25R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F25R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F25R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F25R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F25R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F25R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F25R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F25R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F25R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F25R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F25R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F25R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F25R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F25R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F25R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F25R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F25R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F25R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F25R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F25R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F25R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F25R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F26R1 寄存器的位定义 (过滤器26，寄存器1)  ******************/
#define CAN_F26R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F26R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F26R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F26R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F26R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F26R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F26R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F26R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F26R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F26R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F26R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F26R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F26R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F26R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F26R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F26R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F26R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F26R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F26R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F26R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F26R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F26R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F26R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F26R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F26R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F26R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F26R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F26R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F26R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F26R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F26R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F26R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F27R1 寄存器的位定义 (过滤器27，寄存器1)  ******************/
#define CAN_F27R1_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F27R1_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F27R1_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F27R1_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F27R1_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F27R1_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F27R1_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F27R1_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F27R1_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F27R1_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F27R1_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F27R1_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F27R1_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F27R1_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F27R1_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F27R1_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F27R1_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F27R1_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F27R1_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F27R1_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F27R1_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F27R1_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F27R1_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F27R1_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F27R1_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F27R1_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F27R1_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F27R1_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F27R1_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F27R1_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F27R1_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F27R1_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F0R2 寄存器的位定义 (过滤器0，寄存器2)  *******************/
#define CAN_F0R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F0R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F0R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F0R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F0R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F0R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F0R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F0R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F0R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F0R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F0R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F0R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F0R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F0R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F0R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F0R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F0R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F0R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F0R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F0R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F0R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F0R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F0R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F0R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F0R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F0R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F0R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F0R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F0R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F0R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F0R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F0R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F1R2 寄存器的位定义 (过滤器1，寄存器2)  *******************/
#define CAN_F1R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F1R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F1R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F1R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F1R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F1R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F1R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F1R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F1R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F1R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F1R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F1R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F1R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F1R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F1R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F1R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F1R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F1R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F1R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F1R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F1R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F1R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F1R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F1R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F1R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F1R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F1R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F1R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F1R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F1R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F1R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F1R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F2R2 寄存器的位定义 (过滤器2，寄存器2)  *******************/
#define CAN_F2R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F2R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F2R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F2R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F2R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F2R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F2R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F2R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F2R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F2R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F2R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F2R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F2R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F2R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F2R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F2R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F2R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F2R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F2R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F2R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F2R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F2R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F2R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F2R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F2R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F2R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F2R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F2R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F2R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F2R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F2R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F2R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F3R2 寄存器的位定义 (过滤器3，寄存器2)  *******************/
#define CAN_F3R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F3R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F3R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F3R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F3R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F3R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F3R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F3R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F3R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F3R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F3R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F3R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F3R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F3R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F3R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F3R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F3R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F3R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F3R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F3R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F3R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F3R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F3R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F3R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F3R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F3R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F3R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F3R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F3R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F3R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F3R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F3R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F4R2 寄存器的位定义 (过滤器4，寄存器2)  *******************/
#define CAN_F4R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F4R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F4R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F4R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F4R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F4R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F4R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F4R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F4R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F4R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F4R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F4R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F4R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F4R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F4R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F4R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F4R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F4R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F4R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F4R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F4R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F4R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F4R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F4R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F4R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F4R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F4R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F4R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F4R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F4R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F4R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F4R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F5R2 寄存器的位定义 (过滤器5，寄存器2)  *******************/
#define CAN_F5R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F5R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F5R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F5R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F5R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F5R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F5R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F5R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F5R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F5R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F5R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F5R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F5R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F5R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F5R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F5R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F5R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F5R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F5R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F5R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F5R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F5R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F5R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F5R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F5R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F5R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F5R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F5R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F5R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F5R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F5R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F5R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F6R2 寄存器的位定义 (过滤器6，寄存器2)  *******************/
#define CAN_F6R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F6R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F6R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F6R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F6R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F6R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F6R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F6R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F6R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F6R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F6R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F6R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F6R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F6R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F6R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F6R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F6R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F6R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F6R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F6R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F6R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F6R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F6R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F6R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F6R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F6R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F6R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F6R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F6R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F6R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F6R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F6R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F7R2 寄存器的位定义 (过滤器7，寄存器2)  *******************/
#define CAN_F7R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F7R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F7R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F7R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F7R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F7R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F7R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F7R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F7R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F7R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F7R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F7R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F7R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F7R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F7R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F7R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F7R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F7R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F7R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F7R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F7R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F7R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F7R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F7R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F7R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F7R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F7R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F7R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F7R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F7R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F7R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F7R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F8R2 寄存器的位定义 (过滤器8，寄存器2)  *******************/
#define CAN_F8R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F8R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F8R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F8R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F8R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F8R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F8R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F8R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F8R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F8R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F8R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F8R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F8R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F8R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F8R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F8R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F8R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F8R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F8R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F8R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F8R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F8R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F8R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F8R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F8R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F8R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F8R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F8R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F8R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F8R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F8R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F8R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F9R2 寄存器的位定义 (过滤器9，寄存器2)  *******************/
#define CAN_F9R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F9R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F9R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F9R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F9R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F9R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F9R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F9R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F9R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F9R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F9R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F9R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F9R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F9R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F9R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F9R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F9R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F9R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F9R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F9R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F9R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F9R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F9R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F9R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F9R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F9R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F9R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F9R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F9R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F9R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F9R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F9R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F10R2 寄存器的位定义 (过滤器10，寄存器2)  ******************/
#define CAN_F10R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F10R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F10R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F10R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F10R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F10R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F10R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F10R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F10R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F10R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F10R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F10R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F10R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F10R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F10R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F10R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F10R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F10R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F10R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F10R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F10R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F10R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F10R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F10R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F10R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F10R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F10R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F10R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F10R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F10R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F10R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F10R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F11R2 寄存器的位定义 (过滤器11，寄存器2)  ******************/
#define CAN_F11R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F11R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F11R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F11R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F11R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F11R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F11R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F11R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F11R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F11R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F11R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F11R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F11R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F11R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F11R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F11R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F11R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F11R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F11R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F11R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F11R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F11R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F11R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F11R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F11R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F11R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F11R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F11R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F11R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F11R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F11R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F11R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F12R2 寄存器的位定义 (过滤器12，寄存器2)  ******************/
#define CAN_F12R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F12R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F12R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F12R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F12R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F12R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F12R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F12R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F12R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F12R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F12R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F12R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F12R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F12R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F12R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F12R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F12R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F12R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F12R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F12R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F12R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F12R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F12R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F12R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F12R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F12R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F12R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F12R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F12R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F12R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F12R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F12R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F13R2 寄存器的位定义 (过滤器13，寄存器2)  ******************/
#define CAN_F13R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F13R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F13R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F13R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F13R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F13R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F13R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F13R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F13R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F13R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F13R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F13R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F13R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F13R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F13R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F13R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F13R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F13R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F13R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F13R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F13R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F13R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F13R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F13R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F13R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F13R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F13R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F13R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F13R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F13R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F13R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F13R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F14R2 寄存器的位定义 (过滤器14，寄存器2)  *******************/
#define CAN_F14R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F14R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F14R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F14R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F14R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F14R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F14R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F14R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F14R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F14R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F14R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F14R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F14R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F14R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F14R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F14R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F14R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F14R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F14R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F14R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F14R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F14R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F14R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F14R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F14R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F14R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F14R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F14R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F14R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F14R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F14R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F14R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F15R2 寄存器的位定义 (过滤器15，寄存器2)  *******************/
#define CAN_F15R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F15R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F15R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F15R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F15R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F15R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F15R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F15R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F15R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F15R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F15R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F15R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F15R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F15R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F15R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F15R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F15R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F15R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F15R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F15R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F15R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F15R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F15R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F15R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F15R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F15R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F15R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F15R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F15R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F15R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F15R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F15R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F16R2 寄存器的位定义 (过滤器16，寄存器2)  *******************/
#define CAN_F16R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F16R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F16R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F16R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F16R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F16R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F16R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F16R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F16R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F16R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F16R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F16R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F16R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F16R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F16R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F16R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F16R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F16R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F16R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F16R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F16R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F16R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F16R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F16R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F16R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F16R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F16R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F16R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F16R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F16R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F16R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F16R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F17R2 寄存器的位定义 (过滤器17，寄存器2)  *******************/
#define CAN_F17R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F17R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F17R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F17R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F17R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F17R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F17R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F17R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F17R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F17R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F17R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F17R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F17R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F17R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F17R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F17R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F17R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F17R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F17R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F17R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F17R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F17R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F17R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F17R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F17R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F17R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F17R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F17R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F17R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F17R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F17R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F17R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F18R2 寄存器的位定义 (过滤器18，寄存器2)  *******************/
#define CAN_F18R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F18R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F18R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F18R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F18R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F18R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F18R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F18R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F18R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F18R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F18R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F18R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F18R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F18R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F18R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F18R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F18R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F18R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F18R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F18R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F18R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F18R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F18R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F18R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F18R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F18R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F18R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F18R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F18R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F18R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F18R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F18R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F19R2 寄存器的位定义 (过滤器19，寄存器2)  *******************/
#define CAN_F19R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F19R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F19R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F19R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F19R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F19R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F19R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F19R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F19R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F19R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F19R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F19R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F19R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F19R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F19R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F19R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F19R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F19R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F19R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F19R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F19R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F19R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F19R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F19R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F19R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F19R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F19R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F19R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F19R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F19R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F19R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F19R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F20R2 寄存器的位定义 (过滤器20，寄存器2)  *******************/
#define CAN_F20R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F20R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F20R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F20R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F20R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F20R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F20R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F20R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F20R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F20R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F20R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F20R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F20R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F20R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F20R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F20R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F20R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F20R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F20R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F20R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F20R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F20R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F20R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F20R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F20R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F20R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F20R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F20R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F20R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F20R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F20R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F20R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F21R2 寄存器的位定义 (过滤器21，寄存器2)  *******************/
#define CAN_F21R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F21R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F21R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F21R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F21R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F21R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F21R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F21R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F21R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F21R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F21R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F21R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F21R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F21R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F21R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F21R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F21R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F21R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F21R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F21R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F21R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F21R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F21R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F21R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F21R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F21R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F21R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F21R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F21R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F21R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F21R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F21R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F22R2 寄存器的位定义 (过滤器22，寄存器2)  *******************/
#define CAN_F22R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F22R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F22R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F22R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F22R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F22R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F22R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F22R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F22R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F22R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F22R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F22R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F22R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F22R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F22R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F22R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F22R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F22R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F22R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F22R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F22R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F22R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F22R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F22R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F22R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F22R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F22R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F22R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F22R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F22R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F22R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F22R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F23R2 寄存器的位定义 (过滤器23，寄存器2)  *******************/
#define CAN_F23R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F23R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F23R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F23R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F23R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F23R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F23R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F23R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F23R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F23R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F23R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F23R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F23R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F23R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F23R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F23R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F23R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F23R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F23R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F23R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F23R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F23R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F23R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F23R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F23R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F23R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F23R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F23R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F23R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F23R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F23R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F23R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F24R2 寄存器的位定义 (过滤器24，寄存器2)  ******************/
#define CAN_F24R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F24R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F24R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F24R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F24R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F24R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F24R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F24R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F24R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F24R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F24R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F24R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F24R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F24R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F24R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F24R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F24R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F24R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F24R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F24R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F24R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F24R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F24R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F24R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F24R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F24R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F24R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F24R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F24R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F24R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F24R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F24R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F25R2 寄存器的位定义 (过滤器25，寄存器2)  ******************/
#define CAN_F25R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F25R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F25R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F25R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F25R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F25R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F25R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F25R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F25R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F25R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F25R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F25R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F25R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F25R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F25R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F25R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F25R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F25R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F25R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F25R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F25R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F25R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F25R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F25R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F25R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F25R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F25R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F25R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F25R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F25R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F25R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F25R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F26R2 寄存器的位定义 (过滤器26，寄存器2)  ******************/
#define CAN_F26R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F26R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F26R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F26R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F26R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F26R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F26R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F26R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F26R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F26R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F26R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F26R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F26R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F26R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F26R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F26R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F26R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F26R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F26R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F26R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F26R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F26R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F26R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F26R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F26R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F26R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F26R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F26R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F26R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F26R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F26R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F26R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/*******************  CAN_F27R2 寄存器的位定义 (过滤器27，寄存器2)  ******************/
#define CAN_F27R2_FB0 ((uint32_t)0x00000001)  /* 过滤器位0 */
#define CAN_F27R2_FB1 ((uint32_t)0x00000002)  /* 过滤器位1 */
#define CAN_F27R2_FB2 ((uint32_t)0x00000004)  /* 过滤器位2 */
#define CAN_F27R2_FB3 ((uint32_t)0x00000008)  /* 过滤器位3 */
#define CAN_F27R2_FB4 ((uint32_t)0x00000010)  /* 过滤器位4 */
#define CAN_F27R2_FB5 ((uint32_t)0x00000020)  /* 过滤器位5 */
#define CAN_F27R2_FB6 ((uint32_t)0x00000040)  /* 过滤器位6 */
#define CAN_F27R2_FB7 ((uint32_t)0x00000080)  /* 过滤器位7 */
#define CAN_F27R2_FB8 ((uint32_t)0x00000100)  /* 过滤器位8 */
#define CAN_F27R2_FB9 ((uint32_t)0x00000200)  /* 过滤器位9 */
#define CAN_F27R2_FB10 ((uint32_t)0x00000400) /* 过滤器位10 */
#define CAN_F27R2_FB11 ((uint32_t)0x00000800) /* 过滤器位11 */
#define CAN_F27R2_FB12 ((uint32_t)0x00001000) /* 过滤器位12 */
#define CAN_F27R2_FB13 ((uint32_t)0x00002000) /* 过滤器位13 */
#define CAN_F27R2_FB14 ((uint32_t)0x00004000) /* 过滤器位14 */
#define CAN_F27R2_FB15 ((uint32_t)0x00008000) /* 过滤器位15 */
#define CAN_F27R2_FB16 ((uint32_t)0x00010000) /* 过滤器位16 */
#define CAN_F27R2_FB17 ((uint32_t)0x00020000) /* 过滤器位17 */
#define CAN_F27R2_FB18 ((uint32_t)0x00040000) /* 过滤器位18 */
#define CAN_F27R2_FB19 ((uint32_t)0x00080000) /* 过滤器位19 */
#define CAN_F27R2_FB20 ((uint32_t)0x00100000) /* 过滤器位20 */
#define CAN_F27R2_FB21 ((uint32_t)0x00200000) /* 过滤器位21 */
#define CAN_F27R2_FB22 ((uint32_t)0x00400000) /* 过滤器位22 */
#define CAN_F27R2_FB23 ((uint32_t)0x00800000) /* 过滤器位23 */
#define CAN_F27R2_FB24 ((uint32_t)0x01000000) /* 过滤器位24 */
#define CAN_F27R2_FB25 ((uint32_t)0x02000000) /* 过滤器位25 */
#define CAN_F27R2_FB26 ((uint32_t)0x04000000) /* 过滤器位26 */
#define CAN_F27R2_FB27 ((uint32_t)0x08000000) /* 过滤器位27 */
#define CAN_F27R2_FB28 ((uint32_t)0x10000000) /* 过滤器位28 */
#define CAN_F27R2_FB29 ((uint32_t)0x20000000) /* 过滤器位29 */
#define CAN_F27R2_FB30 ((uint32_t)0x40000000) /* 过滤器位30 */
#define CAN_F27R2_FB31 ((uint32_t)0x80000000) /* 过滤器位31 */

/******************************************************************************/
/*                          CRC 计算单元                                       */
/******************************************************************************/

/*******************  CRC_DATAR 寄存器的位定义  *********************/
#define CRC_DATAR_DR ((uint32_t)0xFFFFFFFF) /* 数据寄存器位 */

/*******************  CRC_IDATAR 寄存器的位定义  ********************/
#define CRC_IDR_IDATAR ((uint8_t)0xFF) /* 通用8位数据寄存器位 */

/********************  CRC_CTLR 寄存器的位定义  ********************/
#define CRC_CTLR_RESET ((uint8_t)0x01) /* 复位位 */

/******************************************************************************/
/*                      数字模拟转换器 (DAC)                                  */
/******************************************************************************/

/********************  DAC_CTLR 寄存器的位定义  ********************/
#define DAC_EN1 ((uint32_t)0x00000001)   /* DAC通道1使能 */
#define DAC_BOFF1 ((uint32_t)0x00000002) /* DAC通道1输出缓冲禁用 */
#define DAC_TEN1 ((uint32_t)0x00000004)  /* DAC通道1触发使能 */

#define DAC_TSEL1 ((uint32_t)0x00000038)   /* TSEL1[2:0] (DAC通道1触发选择) */
#define DAC_TSEL1_0 ((uint32_t)0x00000008) /* 位0 */
#define DAC_TSEL1_1 ((uint32_t)0x00000010) /* 位1 */
#define DAC_TSEL1_2 ((uint32_t)0x00000020) /* 位2 */

#define DAC_WAVE1 ((uint32_t)0x000000C0)   /* WAVE1[1:0] (DAC通道1噪声/三角波生成使能) */
#define DAC_WAVE1_0 ((uint32_t)0x00000040) /* 位0 */
#define DAC_WAVE1_1 ((uint32_t)0x00000080) /* 位1 */

#define DAC_MAMP1 ((uint32_t)0x00000F00)   /* MAMP1[3:0] (DAC通道1掩码/幅度选择) */
#define DAC_MAMP1_0 ((uint32_t)0x00000100) /* 位0 */
#define DAC_MAMP1_1 ((uint32_t)0x00000200) /* 位1 */
#define DAC_MAMP1_2 ((uint32_t)0x00000400) /* 位2 */
#define DAC_MAMP1_3 ((uint32_t)0x00000800) /* 位3 */

#define DAC_DMAEN1 ((uint32_t)0x00001000) /* DAC通道1 DMA使能 */
#define DAC_EN2 ((uint32_t)0x00010000)    /* DAC通道2使能 */
#define DAC_BOFF2 ((uint32_t)0x00020000)  /* DAC通道2输出缓冲禁用 */
#define DAC_TEN2 ((uint32_t)0x00040000)   /* DAC通道2触发使能 */

#define DAC_TSEL2 ((uint32_t)0x00380000)   /* TSEL2[2:0] (DAC通道2触发选择) */
#define DAC_TSEL2_0 ((uint32_t)0x00080000) /* 位0 */
#define DAC_TSEL2_1 ((uint32_t)0x00100000) /* 位1 */
#define DAC_TSEL2_2 ((uint32_t)0x00200000) /* 位2 */

#define DAC_WAVE2 ((uint32_t)0x00C00000)   /* WAVE2[1:0] (DAC通道2噪声/三角波生成使能) */
#define DAC_WAVE2_0 ((uint32_t)0x00400000) /* 位0 */
#define DAC_WAVE2_1 ((uint32_t)0x00800000) /* 位1 */

#define DAC_MAMP2 ((uint32_t)0x0F000000)   /* MAMP2[3:0] (DAC通道2掩码/幅度选择) */
#define DAC_MAMP2_0 ((uint32_t)0x01000000) /* 位0 */
#define DAC_MAMP2_1 ((uint32_t)0x02000000) /* 位1 */
#define DAC_MAMP2_2 ((uint32_t)0x04000000) /* 位2 */
#define DAC_MAMP2_3 ((uint32_t)0x08000000) /* 位3 */

#define DAC_DMAEN2 ((uint32_t)0x10000000) /* DAC通道2 DMA使能 */

/*****************  DAC_SWTR 寄存器的位定义  ******************/
#define DAC_SWTRIG1 ((uint8_t)0x01) /* DAC通道1软件触发 */
#define DAC_SWTRIG2 ((uint8_t)0x02) /* DAC通道2软件触发 */

/*****************  DAC_R12BDHR1 寄存器的位定义  ******************/
#define DAC_DHR12R1 ((uint16_t)0x0FFF) /* DAC通道1 12位右对齐数据 */

/*****************  DAC_L12BDHR1 寄存器的位定义  ******************/
#define DAC_DHR12L1 ((uint16_t)0xFFF0) /* DAC通道1 12位左对齐数据 */

/******************  DAC_R8BDHR1 寄存器的位定义  ******************/
#define DAC_DHR8R1 ((uint8_t)0xFF) /* DAC通道1 8位右对齐数据 */

/*****************  DAC_R12BDHR2 寄存器的位定义  ******************/
#define DAC_DHR12R2 ((uint16_t)0x0FFF) /* DAC通道2 12位右对齐数据 */

/*****************  DAC_L12BDHR2 寄存器的位定义  ******************/
#define DAC_DHR12L2 ((uint16_t)0xFFF0) /* DAC通道2 12位左对齐数据 */

/******************  DAC_R8BDHR2 寄存器的位定义  ******************/
#define DAC_DHR8R2 ((uint8_t)0xFF) /* DAC通道2 8位右对齐数据 */

/*****************  DAC_RD12BDHR 寄存器的位定义  ******************/
#define DAC_RD12BDHR_DACC1DHR ((uint32_t)0x00000FFF) /* DAC通道1 12位右对齐数据 */
#define DAC_RD12BDHR_DACC2DHR ((uint32_t)0x0FFF0000) /* DAC通道2 12位右对齐数据 */

/*****************  DAC_LD12BDHR 寄存器的位定义  ******************/
#define DAC_LD12BDHR_DACC1DHR ((uint32_t)0x0000FFF0) /* DAC通道1 12位左对齐数据 */
#define DAC_LD12BDHR_DACC2DHR ((uint32_t)0xFFF00000) /* DAC通道2 12位左对齐数据 */

/******************  DAC_RD8BDHR 寄存器的位定义  ******************/
#define DAC_RD8BDHR_DACC1DHR ((uint16_t)0x00FF) /* DAC通道1 8位右对齐数据 */
#define DAC_RD8BDHR_DACC2DHR ((uint16_t)0xFF00) /* DAC通道2 8位右对齐数据 */

/*******************  DAC_DOR1 寄存器的位定义  *******************/
#define DAC_DACC1DOR ((uint16_t)0x0FFF) /* DAC通道1数据输出 */

/*******************  DAC_DOR2 寄存器的位定义  *******************/
#define DAC_DACC2DOR ((uint16_t)0x0FFF) /* DAC通道2数据输出 */

/******************************************************************************/
/*                             DMA 控制器                                     */
/******************************************************************************/

/*******************  DMA_INTFR 寄存器的位定义  ********************/
#define DMA_GIF1 ((uint32_t)0x00000001)  /* 通道1全局中断标志 */
#define DMA_TCIF1 ((uint32_t)0x00000002) /* 通道1传输完成标志 */
#define DMA_HTIF1 ((uint32_t)0x00000004) /* 通道1半传输标志 */
#define DMA_TEIF1 ((uint32_t)0x00000008) /* 通道1传输错误标志 */
#define DMA_GIF2 ((uint32_t)0x00000010)  /* 通道2全局中断标志 */
#define DMA_TCIF2 ((uint32_t)0x00000020) /* 通道2传输完成标志 */
#define DMA_HTIF2 ((uint32_t)0x00000040) /* 通道2半传输标志 */
#define DMA_TEIF2 ((uint32_t)0x00000080) /* 通道2传输错误标志 */
#define DMA_GIF3 ((uint32_t)0x00000100)  /* 通道3全局中断标志 */
#define DMA_TCIF3 ((uint32_t)0x00000200) /* 通道3传输完成标志 */
#define DMA_HTIF3 ((uint32_t)0x00000400) /* 通道3半传输标志 */
#define DMA_TEIF3 ((uint32_t)0x00000800) /* 通道3传输错误标志 */
#define DMA_GIF4 ((uint32_t)0x00001000)  /* 通道4全局中断标志 */
#define DMA_TCIF4 ((uint32_t)0x00002000) /* 通道4传输完成标志 */
#define DMA_HTIF4 ((uint32_t)0x00004000) /* 通道4半传输标志 */
#define DMA_TEIF4 ((uint32_t)0x00008000) /* 通道4传输错误标志 */
#define DMA_GIF5 ((uint32_t)0x00010000)  /* 通道5全局中断标志 */
#define DMA_TCIF5 ((uint32_t)0x00020000) /* 通道5传输完成标志 */
#define DMA_HTIF5 ((uint32_t)0x00040000) /* 通道5半传输标志 */
#define DMA_TEIF5 ((uint32_t)0x00080000) /* 通道5传输错误标志 */
#define DMA_GIF6 ((uint32_t)0x00100000)  /* 通道6全局中断标志 */
#define DMA_TCIF6 ((uint32_t)0x00200000) /* 通道6传输完成标志 */
#define DMA_HTIF6 ((uint32_t)0x00400000) /* 通道6半传输标志 */
#define DMA_TEIF6 ((uint32_t)0x00800000) /* 通道6传输错误标志 */
#define DMA_GIF7 ((uint32_t)0x01000000)  /* 通道7全局中断标志 */
#define DMA_TCIF7 ((uint32_t)0x02000000) /* 通道7传输完成标志 */
#define DMA_HTIF7 ((uint32_t)0x04000000) /* 通道7半传输标志 */
#define DMA_TEIF7 ((uint32_t)0x08000000) /* 通道7传输错误标志 */

#define DMA_GIF8 ((uint32_t)0x00000001)   /* 通道8全局中断标志 */
#define DMA_TCIF8 ((uint32_t)0x00000002)  /* 通道8传输完成标志 */
#define DMA_HTIF8 ((uint32_t)0x00000004)  /* 通道8半传输标志 */
#define DMA_TEIF8 ((uint32_t)0x00000008)  /* 通道8传输错误标志 */
#define DMA_GIF9 ((uint32_t)0x00000010)   /* 通道9全局中断标志 */
#define DMA_TCIF9 ((uint32_t)0x00000020)  /* 通道9传输完成标志 */
#define DMA_HTIF9 ((uint32_t)0x00000040)  /* 通道9半传输标志 */
#define DMA_TEIF9 ((uint32_t)0x00000080)  /* 通道9传输错误标志 */
#define DMA_GIF10 ((uint32_t)0x00000100)  /* 通道10全局中断标志 */
#define DMA_TCIF10 ((uint32_t)0x00000200) /* 通道10传输完成标志 */
#define DMA_HTIF10 ((uint32_t)0x00000400) /* 通道10半传输标志 */
#define DMA_TEIF10 ((uint32_t)0x00000800) /* 通道10传输错误标志 */
#define DMA_GIF11 ((uint32_t)0x00001000)  /* 通道11全局中断标志 */
#define DMA_TCIF11 ((uint32_t)0x00002000) /* 通道11传输完成标志 */
#define DMA_HTIF11 ((uint32_t)0x00004000) /* 通道11半传输标志 */
#define DMA_TEIF11 ((uint32_t)0x00008000) /* 通道11传输错误标志 */

/*******************  DMA_INTFCR 寄存器的位定义  *******************/
#define DMA_CGIF1 ((uint32_t)0x00000001)  /* 通道1全局中断清除 */
#define DMA_CTCIF1 ((uint32_t)0x00000002) /* 通道1传输完成清除 */
#define DMA_CHTIF1 ((uint32_t)0x00000004) /* 通道1半传输清除 */
#define DMA_CTEIF1 ((uint32_t)0x00000008) /* 通道1传输错误清除 */
#define DMA_CGIF2 ((uint32_t)0x00000010)  /* 通道2全局中断清除 */
#define DMA_CTCIF2 ((uint32_t)0x00000020) /* 通道2传输完成清除 */
#define DMA_CHTIF2 ((uint32_t)0x00000040) /* 通道2半传输清除 */
#define DMA_CTEIF2 ((uint32_t)0x00000080) /* 通道2传输错误清除 */
#define DMA_CGIF3 ((uint32_t)0x00000100)  /* 通道3全局中断清除 */
#define DMA_CTCIF3 ((uint32_t)0x00000200) /* 通道3传输完成清除 */
#define DMA_CHTIF3 ((uint32_t)0x00000400) /* 通道3半传输清除 */
#define DMA_CTEIF3 ((uint32_t)0x00000800) /* 通道3传输错误清除 */
#define DMA_CGIF4 ((uint32_t)0x00001000)  /* 通道4全局中断清除 */
#define DMA_CTCIF4 ((uint32_t)0x00002000) /* 通道4传输完成清除 */
#define DMA_CHTIF4 ((uint32_t)0x00004000) /* 通道4半传输清除 */
#define DMA_CTEIF4 ((uint32_t)0x00008000) /* 通道4传输错误清除 */
#define DMA_CGIF5 ((uint32_t)0x00010000)  /* 通道5全局中断清除 */
#define DMA_CTCIF5 ((uint32_t)0x00020000) /* 通道5传输完成清除 */
#define DMA_CHTIF5 ((uint32_t)0x00040000) /* 通道5半传输清除 */
#define DMA_CTEIF5 ((uint32_t)0x00080000) /* 通道5传输错误清除 */
#define DMA_CGIF6 ((uint32_t)0x00100000)  /* 通道6全局中断清除 */
#define DMA_CTCIF6 ((uint32_t)0x00200000) /* 通道6传输完成清除 */
#define DMA_CHTIF6 ((uint32_t)0x00400000) /* 通道6半传输清除 */
#define DMA_CTEIF6 ((uint32_t)0x00800000) /* 通道6传输错误清除 */
#define DMA_CGIF7 ((uint32_t)0x01000000)  /* 通道7全局中断清除 */
#define DMA_CTCIF7 ((uint32_t)0x02000000) /* 通道7传输完成清除 */
#define DMA_CHTIF7 ((uint32_t)0x04000000) /* 通道7半传输清除 */
#define DMA_CTEIF7 ((uint32_t)0x08000000) /* 通道7传输错误清除 */
#define DMA_CGIF8 ((uint32_t)0x10000000)  /* 通道8全局中断清除 */
#define DMA_CTCIF8 ((uint32_t)0x20000000) /* 通道8传输完成清除 */
#define DMA_CHTIF8 ((uint32_t)0x40000000) /* 通道8半传输清除 */
#define DMA_CTEIF8 ((uint32_t)0x80000000) /* 通道8传输错误清除 */

/*******************  DMA_CFGR1 寄存器的位定义 (通道1)  *******************/
#define DMA_CFGR1_EN ((uint16_t)0x0001)   /* 通道使能 */
#define DMA_CFGR1_TCIE ((uint16_t)0x0002) /* 传输完成中断使能 */
#define DMA_CFGR1_HTIE ((uint16_t)0x0004) /* 半传输中断使能 */
#define DMA_CFGR1_TEIE ((uint16_t)0x0008) /* 传输错误中断使能 */
#define DMA_CFGR1_DIR ((uint16_t)0x0010)  /* 数据传输方向：0=从外设读，1=从存储器读 */
#define DMA_CFGR1_CIRC ((uint16_t)0x0020) /* 循环模式 */
#define DMA_CFGR1_PINC ((uint16_t)0x0040) /* 外设地址增量模式 */
#define DMA_CFGR1_MINC ((uint16_t)0x0080) /* 存储器地址增量模式 */

#define DMA_CFGR1_PSIZE ((uint16_t)0x0300)   /* PSIZE[1:0] 位 (外设数据宽度) */
#define DMA_CFGR1_PSIZE_0 ((uint16_t)0x0100) /* 位0 */
#define DMA_CFGR1_PSIZE_1 ((uint16_t)0x0200) /* 位1 */

#define DMA_CFGR1_MSIZE ((uint16_t)0x0C00)   /* MSIZE[1:0] 位 (存储器数据宽度) */
#define DMA_CFGR1_MSIZE_0 ((uint16_t)0x0400) /* 位0 */
#define DMA_CFGR1_MSIZE_1 ((uint16_t)0x0800) /* 位1 */

#define DMA_CFGR1_PL ((uint16_t)0x3000)   /* PL[1:0] 位 (通道优先级) */
#define DMA_CFGR1_PL_0 ((uint16_t)0x1000) /* 位0 */
#define DMA_CFGR1_PL_1 ((uint16_t)0x2000) /* 位1 */

#define DMA_CFGR1_MEM2MEM ((uint16_t)0x4000) /* 存储器到存储器模式 */

/*******************  DMA_CFGR2 寄存器的位定义 (通道2)  *******************/
#define DMA_CFGR2_EN ((uint16_t)0x0001)   /* 通道使能 */
#define DMA_CFGR2_TCIE ((uint16_t)0x0002) /* 传输完成中断使能 */
#define DMA_CFGR2_HTIE ((uint16_t)0x0004) /* 半传输中断使能 */
#define DMA_CFGR2_TEIE ((uint16_t)0x0008) /* 传输错误中断使能 */
#define DMA_CFGR2_DIR ((uint16_t)0x0010)  /* 数据传输方向 */
#define DMA_CFGR2_CIRC ((uint16_t)0x0020) /* 循环模式 */
#define DMA_CFGR2_PINC ((uint16_t)0x0040) /* 外设地址增量 */
#define DMA_CFGR2_MINC ((uint16_t)0x0080) /* 存储器地址增量 */

#define DMA_CFGR2_PSIZE ((uint16_t)0x0300)   /* 外设数据宽度 */
#define DMA_CFGR2_PSIZE_0 ((uint16_t)0x0100) /* 位0 */
#define DMA_CFGR2_PSIZE_1 ((uint16_t)0x0200) /* 位1 */

#define DMA_CFGR2_MSIZE ((uint16_t)0x0C00)   /* 存储器数据宽度 */
#define DMA_CFGR2_MSIZE_0 ((uint16_t)0x0400) /* 位0 */
#define DMA_CFGR2_MSIZE_1 ((uint16_t)0x0800) /* 位1 */

#define DMA_CFGR2_PL ((uint16_t)0x3000)   /* 优先级 */
#define DMA_CFGR2_PL_0 ((uint16_t)0x1000) /* 位0 */
#define DMA_CFGR2_PL_1 ((uint16_t)0x2000) /* 位1 */

#define DMA_CFGR2_MEM2MEM ((uint16_t)0x4000) /* 存储器到存储器模式 */

/*******************  DMA_CFGR3 寄存器的位定义 (通道3)  *******************/
#define DMA_CFGR3_EN ((uint16_t)0x0001)   /* 通道使能 */
#define DMA_CFGR3_TCIE ((uint16_t)0x0002) /* 传输完成中断使能 */
#define DMA_CFGR3_HTIE ((uint16_t)0x0004) /* 半传输中断使能 */
#define DMA_CFGR3_TEIE ((uint16_t)0x0008) /* 传输错误中断使能 */
#define DMA_CFGR3_DIR ((uint16_t)0x0010)  /* 数据传输方向 */
#define DMA_CFGR3_CIRC ((uint16_t)0x0020) /* 循环模式 */
#define DMA_CFGR3_PINC ((uint16_t)0x0040) /* 外设地址增量 */
#define DMA_CFGR3_MINC ((uint16_t)0x0080) /* 存储器地址增量 */

#define DMA_CFGR3_PSIZE ((uint16_t)0x0300)   /* 外设数据宽度 */
#define DMA_CFGR3_PSIZE_0 ((uint16_t)0x0100) /* 位0 */
#define DMA_CFGR3_PSIZE_1 ((uint16_t)0x0200) /* 位1 */

#define DMA_CFGR3_MSIZE ((uint16_t)0x0C00)   /* 存储器数据宽度 */
#define DMA_CFGR3_MSIZE_0 ((uint16_t)0x0400) /* 位0 */
#define DMA_CFGR3_MSIZE_1 ((uint16_t)0x0800) /* 位1 */

#define DMA_CFGR3_PL ((uint16_t)0x3000)   /* 优先级 */
#define DMA_CFGR3_PL_0 ((uint16_t)0x1000) /* 位0 */
#define DMA_CFGR3_PL_1 ((uint16_t)0x2000) /* 位1 */

#define DMA_CFGR3_MEM2MEM ((uint16_t)0x4000) /* 存储器到存储器模式 */

/*******************  DMA_CFG4 寄存器的位定义 (通道4)  *******************/
#define DMA_CFG4_EN ((uint16_t)0x0001)   /* 通道使能 */
#define DMA_CFG4_TCIE ((uint16_t)0x0002) /* 传输完成中断使能 */
#define DMA_CFG4_HTIE ((uint16_t)0x0004) /* 半传输中断使能 */
#define DMA_CFG4_TEIE ((uint16_t)0x0008) /* 传输错误中断使能 */
#define DMA_CFG4_DIR ((uint16_t)0x0010)  /* 数据传输方向 */
#define DMA_CFG4_CIRC ((uint16_t)0x0020) /* 循环模式 */
#define DMA_CFG4_PINC ((uint16_t)0x0040) /* 外设地址增量 */
#define DMA_CFG4_MINC ((uint16_t)0x0080) /* 存储器地址增量 */

#define DMA_CFG4_PSIZE ((uint16_t)0x0300)   /* 外设数据宽度 */
#define DMA_CFG4_PSIZE_0 ((uint16_t)0x0100) /* 位0 */
#define DMA_CFG4_PSIZE_1 ((uint16_t)0x0200) /* 位1 */

#define DMA_CFG4_MSIZE ((uint16_t)0x0C00)   /* 存储器数据宽度 */
#define DMA_CFG4_MSIZE_0 ((uint16_t)0x0400) /* 位0 */
#define DMA_CFG4_MSIZE_1 ((uint16_t)0x0800) /* 位1 */

#define DMA_CFG4_PL ((uint16_t)0x3000)   /* 优先级 */
#define DMA_CFG4_PL_0 ((uint16_t)0x1000) /* 位0 */
#define DMA_CFG4_PL_1 ((uint16_t)0x2000) /* 位1 */

#define DMA_CFG4_MEM2MEM ((uint16_t)0x4000) /* 存储器到存储器模式 */

/******************  DMA_CFG5 寄存器的位定义 (通道5)  *******************/
#define DMA_CFG5_EN ((uint16_t)0x0001)   /* 通道使能 */
#define DMA_CFG5_TCIE ((uint16_t)0x0002) /* 传输完成中断使能 */
#define DMA_CFG5_HTIE ((uint16_t)0x0004) /* 半传输中断使能 */
#define DMA_CFG5_TEIE ((uint16_t)0x0008) /* 传输错误中断使能 */
#define DMA_CFG5_DIR ((uint16_t)0x0010)  /* 数据传输方向 */
#define DMA_CFG5_CIRC ((uint16_t)0x0020) /* 循环模式 */
#define DMA_CFG5_PINC ((uint16_t)0x0040) /* 外设地址增量 */
#define DMA_CFG5_MINC ((uint16_t)0x0080) /* 存储器地址增量 */

#define DMA_CFG5_PSIZE ((uint16_t)0x0300)   /* 外设数据宽度 */
#define DMA_CFG5_PSIZE_0 ((uint16_t)0x0100) /* 位0 */
#define DMA_CFG5_PSIZE_1 ((uint16_t)0x0200) /* 位1 */

#define DMA_CFG5_MSIZE ((uint16_t)0x0C00)   /* 存储器数据宽度 */
#define DMA_CFG5_MSIZE_0 ((uint16_t)0x0400) /* 位0 */
#define DMA_CFG5_MSIZE_1 ((uint16_t)0x0800) /* 位1 */

#define DMA_CFG5_PL ((uint16_t)0x3000)   /* 优先级 */
#define DMA_CFG5_PL_0 ((uint16_t)0x1000) /* 位0 */
#define DMA_CFG5_PL_1 ((uint16_t)0x2000) /* 位1 */

#define DMA_CFG5_MEM2MEM ((uint16_t)0x4000) /* 存储器到存储器模式 */

/*******************  DMA_CFG6 寄存器的位定义 (通道6)  *******************/
#define DMA_CFG6_EN ((uint16_t)0x0001)   /* 通道使能 */
#define DMA_CFG6_TCIE ((uint16_t)0x0002) /* 传输完成中断使能 */
#define DMA_CFG6_HTIE ((uint16_t)0x0004) /* 半传输中断使能 */
#define DMA_CFG6_TEIE ((uint16_t)0x0008) /* 传输错误中断使能 */
#define DMA_CFG6_DIR ((uint16_t)0x0010)  /* 数据传输方向 */
#define DMA_CFG6_CIRC ((uint16_t)0x0020) /* 循环模式 */
#define DMA_CFG6_PINC ((uint16_t)0x0040) /* 外设地址增量 */
#define DMA_CFG6_MINC ((uint16_t)0x0080) /* 存储器地址增量 */

#define DMA_CFG6_PSIZE ((uint16_t)0x0300)   /* 外设数据宽度 */
#define DMA_CFG6_PSIZE_0 ((uint16_t)0x0100) /* 位0 */
#define DMA_CFG6_PSIZE_1 ((uint16_t)0x0200) /* 位1 */

#define DMA_CFG6_MSIZE ((uint16_t)0x0C00)   /* 存储器数据宽度 */
#define DMA_CFG6_MSIZE_0 ((uint16_t)0x0400) /* 位0 */
#define DMA_CFG6_MSIZE_1 ((uint16_t)0x0800) /* 位1 */

#define DMA_CFG6_PL ((uint16_t)0x3000)   /* 优先级 */
#define DMA_CFG6_PL_0 ((uint16_t)0x1000) /* 位0 */
#define DMA_CFG6_PL_1 ((uint16_t)0x2000) /* 位1 */

#define DMA_CFG6_MEM2MEM ((uint16_t)0x4000) /* 存储器到存储器模式 */

/*******************  DMA_CFG7 寄存器的位定义 (通道7)  *******************/
#define DMA_CFG7_EN ((uint16_t)0x0001)   /* 通道使能 */
#define DMA_CFG7_TCIE ((uint16_t)0x0002) /* 传输完成中断使能 */
#define DMA_CFG7_HTIE ((uint16_t)0x0004) /* 半传输中断使能 */
#define DMA_CFG7_TEIE ((uint16_t)0x0008) /* 传输错误中断使能 */
#define DMA_CFG7_DIR ((uint16_t)0x0010)  /* 数据传输方向 */
#define DMA_CFG7_CIRC ((uint16_t)0x0020) /* 循环模式 */
#define DMA_CFG7_PINC ((uint16_t)0x0040) /* 外设地址增量 */
#define DMA_CFG7_MINC ((uint16_t)0x0080) /* 存储器地址增量 */

#define DMA_CFG7_PSIZE ((uint16_t)0x0300)   /* 外设数据宽度 */
#define DMA_CFG7_PSIZE_0 ((uint16_t)0x0100) /* 位0 */
#define DMA_CFG7_PSIZE_1 ((uint16_t)0x0200) /* 位1 */

#define DMA_CFG7_MSIZE ((uint16_t)0x0C00)   /* 存储器数据宽度 */
#define DMA_CFG7_MSIZE_0 ((uint16_t)0x0400) /* 位0 */
#define DMA_CFG7_MSIZE_1 ((uint16_t)0x0800) /* 位1 */

#define DMA_CFG7_PL ((uint16_t)0x3000)   /* 优先级 */
#define DMA_CFG7_PL_0 ((uint16_t)0x1000) /* 位0 */
#define DMA_CFG7_PL_1 ((uint16_t)0x2000) /* 位1 */

#define DMA_CFG7_MEM2MEM ((uint16_t)0x4000) /* 存储器到存储器模式 */

/******************  DMA_CNTR1 寄存器的位定义 (通道1)  ******************/
#define DMA_CNTR1_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_CNTR2 寄存器的位定义 (通道2)  ******************/
#define DMA_CNTR2_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_CNTR3 寄存器的位定义 (通道3)  ******************/
#define DMA_CNTR3_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_CNTR4 寄存器的位定义 (通道4)  ******************/
#define DMA_CNTR4_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_CNTR5 寄存器的位定义 (通道5)  ******************/
#define DMA_CNTR5_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_CNTR6 寄存器的位定义 (通道6)  ******************/
#define DMA_CNTR6_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_CNTR7 寄存器的位定义 (通道7)  ******************/
#define DMA_CNTR7_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_CNTR8 寄存器的位定义 (通道8)  ******************/
#define DMA_CNTR8_NDT ((uint16_t)0xFFFF) /* 要传输的数据数量 */

/******************  DMA_PADDR1 寄存器的位定义 (通道1)  *******************/
#define DMA_PADDR1_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_PADDR2 寄存器的位定义 (通道2)  *******************/
#define DMA_PADDR2_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_PADDR3 寄存器的位定义 (通道3)  *******************/
#define DMA_PADDR3_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_PADDR4 寄存器的位定义 (通道4)  *******************/
#define DMA_PADDR4_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_PADDR5 寄存器的位定义 (通道5)  *******************/
#define DMA_PADDR5_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_PADDR6 寄存器的位定义 (通道6)  *******************/
#define DMA_PADDR6_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_PADDR7 寄存器的位定义 (通道7)  *******************/
#define DMA_PADDR7_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_PADDR8 寄存器的位定义 (通道8)  *******************/
#define DMA_PADDR8_PA ((uint32_t)0xFFFFFFFF) /* 外设地址 */

/******************  DMA_MADDR1 寄存器的位定义 (通道1)  *******************/
#define DMA_MADDR1_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************  DMA_MADDR2 寄存器的位定义 (通道2)  *******************/
#define DMA_MADDR2_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************  DMA_MADDR3 寄存器的位定义 (通道3)  *******************/
#define DMA_MADDR3_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************  DMA_MADDR4 寄存器的位定义 (通道4)  *******************/
#define DMA_MADDR4_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************  DMA_MADDR5 寄存器的位定义 (通道5)  *******************/
#define DMA_MADDR5_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************  DMA_MADDR6 寄存器的位定义 (通道6)  *******************/
#define DMA_MADDR6_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************  DMA_MADDR7 寄存器的位定义 (通道7)  *******************/
#define DMA_MADDR7_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************  DMA_MADDR8 寄存器的位定义 (通道8)  *******************/
#define DMA_MADDR8_MA ((uint32_t)0xFFFFFFFF) /* 存储器地址 */

/******************************************************************************/
/*                    外部中断/事件控制器 (EXTI)                              */
/******************************************************************************/

/*******************  EXTI_INTENR 寄存器的位定义  *******************/
#define EXTI_INTENR_MR0 ((uint32_t)0x00000001)  /* 线路0中断屏蔽 */
#define EXTI_INTENR_MR1 ((uint32_t)0x00000002)  /* 线路1中断屏蔽 */
#define EXTI_INTENR_MR2 ((uint32_t)0x00000004)  /* 线路2中断屏蔽 */
#define EXTI_INTENR_MR3 ((uint32_t)0x00000008)  /* 线路3中断屏蔽 */
#define EXTI_INTENR_MR4 ((uint32_t)0x00000010)  /* 线路4中断屏蔽 */
#define EXTI_INTENR_MR5 ((uint32_t)0x00000020)  /* 线路5中断屏蔽 */
#define EXTI_INTENR_MR6 ((uint32_t)0x00000040)  /* 线路6中断屏蔽 */
#define EXTI_INTENR_MR7 ((uint32_t)0x00000080)  /* 线路7中断屏蔽 */
#define EXTI_INTENR_MR8 ((uint32_t)0x00000100)  /* 线路8中断屏蔽 */
#define EXTI_INTENR_MR9 ((uint32_t)0x00000200)  /* 线路9中断屏蔽 */
#define EXTI_INTENR_MR10 ((uint32_t)0x00000400) /* 线路10中断屏蔽 */
#define EXTI_INTENR_MR11 ((uint32_t)0x00000800) /* 线路11中断屏蔽 */
#define EXTI_INTENR_MR12 ((uint32_t)0x00001000) /* 线路12中断屏蔽 */
#define EXTI_INTENR_MR13 ((uint32_t)0x00002000) /* 线路13中断屏蔽 */
#define EXTI_INTENR_MR14 ((uint32_t)0x00004000) /* 线路14中断屏蔽 */
#define EXTI_INTENR_MR15 ((uint32_t)0x00008000) /* 线路15中断屏蔽 */
#define EXTI_INTENR_MR16 ((uint32_t)0x00010000) /* 线路16中断屏蔽 */
#define EXTI_INTENR_MR17 ((uint32_t)0x00020000) /* 线路17中断屏蔽 */
#define EXTI_INTENR_MR18 ((uint32_t)0x00040000) /* 线路18中断屏蔽 */
#define EXTI_INTENR_MR19 ((uint32_t)0x00080000) /* 线路19中断屏蔽 */
#define EXTI_INTENR_MR20 ((uint32_t)0x00100000) /* 线路20中断屏蔽 */

/*******************  EXTI_EVENR 寄存器的位定义  *******************/
#define EXTI_EVENR_MR0 ((uint32_t)0x00000001)  /* 线路0事件屏蔽 */
#define EXTI_EVENR_MR1 ((uint32_t)0x00000002)  /* 线路1事件屏蔽 */
#define EXTI_EVENR_MR2 ((uint32_t)0x00000004)  /* 线路2事件屏蔽 */
#define EXTI_EVENR_MR3 ((uint32_t)0x00000008)  /* 线路3事件屏蔽 */
#define EXTI_EVENR_MR4 ((uint32_t)0x00000010)  /* 线路4事件屏蔽 */
#define EXTI_EVENR_MR5 ((uint32_t)0x00000020)  /* 线路5事件屏蔽 */
#define EXTI_EVENR_MR6 ((uint32_t)0x00000040)  /* 线路6事件屏蔽 */
#define EXTI_EVENR_MR7 ((uint32_t)0x00000080)  /* 线路7事件屏蔽 */
#define EXTI_EVENR_MR8 ((uint32_t)0x00000100)  /* 线路8事件屏蔽 */
#define EXTI_EVENR_MR9 ((uint32_t)0x00000200)  /* 线路9事件屏蔽 */
#define EXTI_EVENR_MR10 ((uint32_t)0x00000400) /* 线路10事件屏蔽 */
#define EXTI_EVENR_MR11 ((uint32_t)0x00000800) /* 线路11事件屏蔽 */
#define EXTI_EVENR_MR12 ((uint32_t)0x00001000) /* 线路12事件屏蔽 */
#define EXTI_EVENR_MR13 ((uint32_t)0x00002000) /* 线路13事件屏蔽 */
#define EXTI_EVENR_MR14 ((uint32_t)0x00004000) /* 线路14事件屏蔽 */
#define EXTI_EVENR_MR15 ((uint32_t)0x00008000) /* 线路15事件屏蔽 */
#define EXTI_EVENR_MR16 ((uint32_t)0x00010000) /* 线路16事件屏蔽 */
#define EXTI_EVENR_MR17 ((uint32_t)0x00020000) /* 线路17事件屏蔽 */
#define EXTI_EVENR_MR18 ((uint32_t)0x00040000) /* 线路18事件屏蔽 */
#define EXTI_EVENR_MR19 ((uint32_t)0x00080000) /* 线路19事件屏蔽 */
#define EXTI_EVENR_MR20 ((uint32_t)0x00100000) /* 线路20事件屏蔽 */

/******************  EXTI_RTENR 寄存器的位定义  *******************/
#define EXTI_RTENR_TR0 ((uint32_t)0x00000001)  /* 线路0上升沿触发配置 */
#define EXTI_RTENR_TR1 ((uint32_t)0x00000002)  /* 线路1上升沿触发配置 */
#define EXTI_RTENR_TR2 ((uint32_t)0x00000004)  /* 线路2上升沿触发配置 */
#define EXTI_RTENR_TR3 ((uint32_t)0x00000008)  /* 线路3上升沿触发配置 */
#define EXTI_RTENR_TR4 ((uint32_t)0x00000010)  /* 线路4上升沿触发配置 */
#define EXTI_RTENR_TR5 ((uint32_t)0x00000020)  /* 线路5上升沿触发配置 */
#define EXTI_RTENR_TR6 ((uint32_t)0x00000040)  /* 线路6上升沿触发配置 */
#define EXTI_RTENR_TR7 ((uint32_t)0x00000080)  /* 线路7上升沿触发配置 */
#define EXTI_RTENR_TR8 ((uint32_t)0x00000100)  /* 线路8上升沿触发配置 */
#define EXTI_RTENR_TR9 ((uint32_t)0x00000200)  /* 线路9上升沿触发配置 */
#define EXTI_RTENR_TR10 ((uint32_t)0x00000400) /* 线路10上升沿触发配置 */
#define EXTI_RTENR_TR11 ((uint32_t)0x00000800) /* 线路11上升沿触发配置 */
#define EXTI_RTENR_TR12 ((uint32_t)0x00001000) /* 线路12上升沿触发配置 */
#define EXTI_RTENR_TR13 ((uint32_t)0x00002000) /* 线路13上升沿触发配置 */
#define EXTI_RTENR_TR14 ((uint32_t)0x00004000) /* 线路14上升沿触发配置 */
#define EXTI_RTENR_TR15 ((uint32_t)0x00008000) /* 线路15上升沿触发配置 */
#define EXTI_RTENR_TR16 ((uint32_t)0x00010000) /* 线路16上升沿触发配置 */
#define EXTI_RTENR_TR17 ((uint32_t)0x00020000) /* 线路17上升沿触发配置 */
#define EXTI_RTENR_TR18 ((uint32_t)0x00040000) /* 线路18上升沿触发配置 */
#define EXTI_RTENR_TR19 ((uint32_t)0x00080000) /* 线路19上升沿触发配置 */
#define EXTI_RTENR_TR20 ((uint32_t)0x00100000) /* 线路20上升沿触发配置 */

/******************  EXTI_FTENR 寄存器的位定义  *******************/
#define EXTI_FTENR_TR0 ((uint32_t)0x00000001)  /* 线路0下降沿触发配置 */
#define EXTI_FTENR_TR1 ((uint32_t)0x00000002)  /* 线路1下降沿触发配置 */
#define EXTI_FTENR_TR2 ((uint32_t)0x00000004)  /* 线路2下降沿触发配置 */
#define EXTI_FTENR_TR3 ((uint32_t)0x00000008)  /* 线路3下降沿触发配置 */
#define EXTI_FTENR_TR4 ((uint32_t)0x00000010)  /* 线路4下降沿触发配置 */
#define EXTI_FTENR_TR5 ((uint32_t)0x00000020)  /* 线路5下降沿触发配置 */
#define EXTI_FTENR_TR6 ((uint32_t)0x00000040)  /* 线路6下降沿触发配置 */
#define EXTI_FTENR_TR7 ((uint32_t)0x00000080)  /* 线路7下降沿触发配置 */
#define EXTI_FTENR_TR8 ((uint32_t)0x00000100)  /* 线路8下降沿触发配置 */
#define EXTI_FTENR_TR9 ((uint32_t)0x00000200)  /* 线路9下降沿触发配置 */
#define EXTI_FTENR_TR10 ((uint32_t)0x00000400) /* 线路10下降沿触发配置 */
#define EXTI_FTENR_TR11 ((uint32_t)0x00000800) /* 线路11下降沿触发配置 */
#define EXTI_FTENR_TR12 ((uint32_t)0x00001000) /* 线路12下降沿触发配置 */
#define EXTI_FTENR_TR13 ((uint32_t)0x00002000) /* 线路13下降沿触发配置 */
#define EXTI_FTENR_TR14 ((uint32_t)0x00004000) /* 线路14下降沿触发配置 */
#define EXTI_FTENR_TR15 ((uint32_t)0x00008000) /* 线路15下降沿触发配置 */
#define EXTI_FTENR_TR16 ((uint32_t)0x00010000) /* 线路16下降沿触发配置 */
#define EXTI_FTENR_TR17 ((uint32_t)0x00020000) /* 线路17下降沿触发配置 */
#define EXTI_FTENR_TR18 ((uint32_t)0x00040000) /* 线路18下降沿触发配置 */
#define EXTI_FTENR_TR19 ((uint32_t)0x00080000) /* 线路19下降沿触发配置 */
#define EXTI_FTENR_TR20 ((uint32_t)0x00100000) /* 线路20下降沿触发配置 */

/******************  EXTI_SWIEVR 寄存器的位定义  ******************/
#define EXTI_SWIEVR_SWIEVR0 ((uint32_t)0x00000001)  /* 线路0软件中断 */
#define EXTI_SWIEVR_SWIEVR1 ((uint32_t)0x00000002)  /* 线路1软件中断 */
#define EXTI_SWIEVR_SWIEVR2 ((uint32_t)0x00000004)  /* 线路2软件中断 */
#define EXTI_SWIEVR_SWIEVR3 ((uint32_t)0x00000008)  /* 线路3软件中断 */
#define EXTI_SWIEVR_SWIEVR4 ((uint32_t)0x00000010)  /* 线路4软件中断 */
#define EXTI_SWIEVR_SWIEVR5 ((uint32_t)0x00000020)  /* 线路5软件中断 */
#define EXTI_SWIEVR_SWIEVR6 ((uint32_t)0x00000040)  /* 线路6软件中断 */
#define EXTI_SWIEVR_SWIEVR7 ((uint32_t)0x00000080)  /* 线路7软件中断 */
#define EXTI_SWIEVR_SWIEVR8 ((uint32_t)0x00000100)  /* 线路8软件中断 */
#define EXTI_SWIEVR_SWIEVR9 ((uint32_t)0x00000200)  /* 线路9软件中断 */
#define EXTI_SWIEVR_SWIEVR10 ((uint32_t)0x00000400) /* 线路10软件中断 */
#define EXTI_SWIEVR_SWIEVR11 ((uint32_t)0x00000800) /* 线路11软件中断 */
#define EXTI_SWIEVR_SWIEVR12 ((uint32_t)0x00001000) /* 线路12软件中断 */
#define EXTI_SWIEVR_SWIEVR13 ((uint32_t)0x00002000) /* 线路13软件中断 */
#define EXTI_SWIEVR_SWIEVR14 ((uint32_t)0x00004000) /* 线路14软件中断 */
#define EXTI_SWIEVR_SWIEVR15 ((uint32_t)0x00008000) /* 线路15软件中断 */
#define EXTI_SWIEVR_SWIEVR16 ((uint32_t)0x00010000) /* 线路16软件中断 */
#define EXTI_SWIEVR_SWIEVR17 ((uint32_t)0x00020000) /* 线路17软件中断 */
#define EXTI_SWIEVR_SWIEVR18 ((uint32_t)0x00040000) /* 线路18软件中断 */
#define EXTI_SWIEVR_SWIEVR19 ((uint32_t)0x00080000) /* 线路19软件中断 */
#define EXTI_SWIEVR_SWIEVR20 ((uint32_t)0x00100000) /* 线路20软件中断 */

/*******************  EXTI_INTFR 寄存器的位定义  ********************/
#define EXTI_INTF_INTF0 ((uint32_t)0x00000001)  /* 线路0挂起位 */
#define EXTI_INTF_INTF1 ((uint32_t)0x00000002)  /* 线路1挂起位 */
#define EXTI_INTF_INTF2 ((uint32_t)0x00000004)  /* 线路2挂起位 */
#define EXTI_INTF_INTF3 ((uint32_t)0x00000008)  /* 线路3挂起位 */
#define EXTI_INTF_INTF4 ((uint32_t)0x00000010)  /* 线路4挂起位 */
#define EXTI_INTF_INTF5 ((uint32_t)0x00000020)  /* 线路5挂起位 */
#define EXTI_INTF_INTF6 ((uint32_t)0x00000040)  /* 线路6挂起位 */
#define EXTI_INTF_INTF7 ((uint32_t)0x00000080)  /* 线路7挂起位 */
#define EXTI_INTF_INTF8 ((uint32_t)0x00000100)  /* 线路8挂起位 */
#define EXTI_INTF_INTF9 ((uint32_t)0x00000200)  /* 线路9挂起位 */
#define EXTI_INTF_INTF10 ((uint32_t)0x00000400) /* 线路10挂起位 */
#define EXTI_INTF_INTF11 ((uint32_t)0x00000800) /* 线路11挂起位 */
#define EXTI_INTF_INTF12 ((uint32_t)0x00001000) /* 线路12挂起位 */
#define EXTI_INTF_INTF13 ((uint32_t)0x00002000) /* 线路13挂起位 */
#define EXTI_INTF_INTF14 ((uint32_t)0x00004000) /* 线路14挂起位 */
#define EXTI_INTF_INTF15 ((uint32_t)0x00008000) /* 线路15挂起位 */
#define EXTI_INTF_INTF16 ((uint32_t)0x00010000) /* 线路16挂起位 */
#define EXTI_INTF_INTF17 ((uint32_t)0x00020000) /* 线路17挂起位 */
#define EXTI_INTF_INTF18 ((uint32_t)0x00040000) /* 线路18挂起位 */
#define EXTI_INTF_INTF19 ((uint32_t)0x00080000) /* 线路19挂起位 */
#define EXTI_INTF_INTF20 ((uint32_t)0x00100000) /* 线路20挂起位 */

/******************************************************************************/
/*                      FLASH 和选项字节寄存器                                 */
/******************************************************************************/

/*******************  FLASH_ACTLR 寄存器的位定义  ******************/
/* (该寄存器未定义位，留空) */

/******************  FLASH_KEYR 寄存器的位定义  ******************/
#define FLASH_KEYR_FKEYR ((uint32_t)0xFFFFFFFF) /* FPEC 密钥 */
#define FLASH_KEYR_KEY1 ((uint32_t)0x45670123)  /* 密钥1 */
#define FLASH_KEYR_KEY2 ((uint32_t)0xCDEF89AB)  /* 密钥2 */

/*****************  FLASH_OBKEYR 寄存器的位定义  ****************/
#define FLASH_OBKEYR_OBKEYR ((uint32_t)0xFFFFFFFF) /* 选项字节密钥 */

/******************  FLASH_STATR 寄存器的位定义  *******************/
#define FLASH_STATR_BSY ((uint8_t)0x01)   /* 忙 */
#define FLASH_STATR_WRBSY ((uint8_t)0x02) /* 写忙 */
#define FLASH_STATR_WRPRTERR ((uint8_t)0x10) /* 写保护错误 */
#define FLASH_STATR_EOP ((uint8_t)0x20)      /* 操作结束 */
#define FLASH_STATR_EHMODS ((uint8_t)0x80)   /* EHM 模式状态 */

/*******************  FLASH_CTLR 寄存器的位定义  *******************/
#define FLASH_CTLR_PG ((uint32_t)0x00000001)         /* 编程 */
#define FLASH_CTLR_PER ((uint32_t)0x00000002)        /* 扇区擦除 4K */
#define FLASH_CTLR_MER ((uint32_t)0x00000004)        /* 整片擦除 */
#define FLASH_CTLR_OPTPG ((uint32_t)0x00000010)      /* 选项字节编程 */
#define FLASH_CTLR_OPTER ((uint32_t)0x00000020)      /* 选项字节擦除 */
#define FLASH_CTLR_STRT ((uint32_t)0x00000040)       /* 开始 */
#define FLASH_CTLR_LOCK ((uint32_t)0x00000080)       /* 锁定 */
#define FLASH_CTLR_OPTWRE ((uint32_t)0x00000200)     /* 选项字节写使能 */
#define FLASH_CTLR_ERRIE ((uint32_t)0x00000400)      /* 错误中断使能 */
#define FLASH_CTLR_EOPIE ((uint32_t)0x00001000)      /* 操作结束中断使能 */
#define FLASH_CTLR_FAST_LOCK ((uint32_t)0x00008000)  /* 快速锁定 */
#define FLASH_CTLR_PAGE_PG ((uint32_t)0x00010000)    /* 页编程 256 字节 */
#define FLASH_CTLR_PAGE_ER ((uint32_t)0x00020000)    /* 页擦除 256 字节 */
#define FLASH_CTLR_PAGE_BER32 ((uint32_t)0x00040000) /* 块擦除 32K */
#define FLASH_CTLR_PAGE_BER64 ((uint32_t)0x00080000) /* 块擦除 64K */
#define FLASH_CTLR_PG_STRT ((uint32_t)0x00200000)    /* 页编程开始 */
#define FLASH_CTLR_RSENACT ((uint32_t)0x00400000)    /* RSEN 激活 */
#define FLASH_CTLR_EHMOD ((uint32_t)0x01000000)      /* EHM 模式 */
#define FLASH_CTLR_SCKMOD ((uint32_t)0x02000000)     /* SCK 模式 */

/*******************  FLASH_ADDR 寄存器的位定义  *******************/
#define FLASH_ADDR_FAR ((uint32_t)0xFFFFFFFF) /* Flash 地址 */

/******************  FLASH_OBR 寄存器的位定义  *******************/
#define FLASH_OBR_OPTERR ((uint16_t)0x0001) /* 选项字节错误 */
#define FLASH_OBR_RDPRT ((uint16_t)0x0002)  /* 读保护 */

#define FLASH_OBR_USER ((uint16_t)0x03FC)       /* 用户选项字节 */
#define FLASH_OBR_WDG_SW ((uint16_t)0x0004)     /* WDG_SW */
#define FLASH_OBR_nRST_STOP ((uint16_t)0x0008)  /* nRST_STOP */
#define FLASH_OBR_nRST_STDBY ((uint16_t)0x0010) /* nRST_STDBY */
#define FLASH_OBR_RAM_CODE_MOD ((uint16_t)0x0300) /* RAM 代码模式 */
#define FLASH_OBR_RAM_CODE_MOD_BIT1 ((uint16_t)0x0100) /* 位1 */
#define FLASH_OBR_RAM_CODE_MOD_BIT2 ((uint16_t)0x0200) /* 位2 */

/******************  FLASH_WPR 寄存器的位定义  ******************/
#define FLASH_WPR_WRP ((uint32_t)0xFFFFFFFF) /* 写保护 */

/******************  FLASH_MODEKEYR 寄存器的位定义  ******************/
#define FLASH_MODEKEYR_KEY1 ((uint32_t)0x45670123) /* 模式密钥1 */
#define FLASH_MODEKEYR_KEY2 ((uint32_t)0xCDEF89AB) /* 模式密钥2 */

/******************  FLASH_RDPR 寄存器的位定义  *******************/
#define FLASH_RDPR_RDPR ((uint32_t)0x000000FF)  /* 读保护选项字节 */
#define FLASH_RDPR_nRDPR ((uint32_t)0x0000FF00) /* 读保护反码选项字节 */

/******************  FLASH_USER 寄存器的位定义  ******************/
#define FLASH_USER_USER ((uint32_t)0x00FF0000)  /* 用户选项字节 */
#define FLASH_USER_nUSER ((uint32_t)0xFF000000) /* 用户选项字节反码 */

/******************  FLASH_Data0 寄存器的位定义  *****************/
#define FLASH_Data0_Data0 ((uint32_t)0x000000FF)  /* 用户数据存储选项字节 */
#define FLASH_Data0_nData0 ((uint32_t)0x0000FF00) /* 用户数据存储反码选项字节 */

/******************  FLASH_Data1 寄存器的位定义  *****************/
#define FLASH_Data1_Data1 ((uint32_t)0x00FF0000)  /* 用户数据存储选项字节 */
#define FLASH_Data1_nData1 ((uint32_t)0xFF000000) /* 用户数据存储反码选项字节 */

/******************  FLASH_WRPR0 寄存器的位定义  ******************/
#define FLASH_WRPR0_WRPR0 ((uint32_t)0x000000FF)  /* Flash 写保护选项字节 */
#define FLASH_WRPR0_nWRPR0 ((uint32_t)0x0000FF00) /* Flash 写保护反码选项字节 */

/******************  FLASH_WRPR1 寄存器的位定义  ******************/
#define FLASH_WRPR1_WRPR1 ((uint32_t)0x00FF0000)  /* Flash 写保护选项字节 */
#define FLASH_WRPR1_nWRPR1 ((uint32_t)0xFF000000) /* Flash 写保护反码选项字节 */

/******************  FLASH_WRPR2 寄存器的位定义  ******************/
#define FLASH_WRPR2_WRPR2 ((uint32_t)0x000000FF)  /* Flash 写保护选项字节 */
#define FLASH_WRPR2_nWRPR2 ((uint32_t)0x0000FF00) /* Flash 写保护反码选项字节 */

/******************  FLASH_WRPR3 寄存器的位定义  ******************/
#define FLASH_WRPR3_WRPR3 ((uint32_t)0x00FF0000)  /* Flash 写保护选项字节 */
#define FLASH_WRPR3_nWRPR3 ((uint32_t)0xFF000000) /* Flash 写保护反码选项字节 */

/******************************************************************************/
/*                通用和复用功能 I/O (GPIO 和 AFIO)                           */
/******************************************************************************/

/*******************  GPIO_CFGLR 寄存器的位定义 (低8位配置)  *******************/
#define GPIO_CFGLR_MODE ((uint32_t)0x33333333) /* 端口 x 模式位组合 */

#define GPIO_CFGLR_MODE0 ((uint32_t)0x00000003)   /* MODE0[1:0] 位 (端口 x 模式，引脚0) */
#define GPIO_CFGLR_MODE0_0 ((uint32_t)0x00000001) /* 位0 */
#define GPIO_CFGLR_MODE0_1 ((uint32_t)0x00000002) /* 位1 */

#define GPIO_CFGLR_MODE1 ((uint32_t)0x00000030)   /* MODE1[1:0] 位 (端口 x 模式，引脚1) */
#define GPIO_CFGLR_MODE1_0 ((uint32_t)0x00000010) /* 位0 */
#define GPIO_CFGLR_MODE1_1 ((uint32_t)0x00000020) /* 位1 */

#define GPIO_CFGLR_MODE2 ((uint32_t)0x00000300)   /* MODE2[1:0] 位 (端口 x 模式，引脚2) */
#define GPIO_CFGLR_MODE2_0 ((uint32_t)0x00000100) /* 位0 */
#define GPIO_CFGLR_MODE2_1 ((uint32_t)0x00000200) /* 位1 */

#define GPIO_CFGLR_MODE3 ((uint32_t)0x00003000)   /* MODE3[1:0] 位 (端口 x 模式，引脚3) */
#define GPIO_CFGLR_MODE3_0 ((uint32_t)0x00001000) /* 位0 */
#define GPIO_CFGLR_MODE3_1 ((uint32_t)0x00002000) /* 位1 */

#define GPIO_CFGLR_MODE4 ((uint32_t)0x00030000)   /* MODE4[1:0] 位 (端口 x 模式，引脚4) */
#define GPIO_CFGLR_MODE4_0 ((uint32_t)0x00010000) /* 位0 */
#define GPIO_CFGLR_MODE4_1 ((uint32_t)0x00020000) /* 位1 */

#define GPIO_CFGLR_MODE5 ((uint32_t)0x00300000)   /* MODE5[1:0] 位 (端口 x 模式，引脚5) */
#define GPIO_CFGLR_MODE5_0 ((uint32_t)0x00100000) /* 位0 */
#define GPIO_CFGLR_MODE5_1 ((uint32_t)0x00200000) /* 位1 */

#define GPIO_CFGLR_MODE6 ((uint32_t)0x03000000)   /* MODE6[1:0] 位 (端口 x 模式，引脚6) */
#define GPIO_CFGLR_MODE6_0 ((uint32_t)0x01000000) /* 位0 */
#define GPIO_CFGLR_MODE6_1 ((uint32_t)0x02000000) /* 位1 */

#define GPIO_CFGLR_MODE7 ((uint32_t)0x30000000)   /* MODE7[1:0] 位 (端口 x 模式，引脚7) */
#define GPIO_CFGLR_MODE7_0 ((uint32_t)0x10000000) /* 位0 */
#define GPIO_CFGLR_MODE7_1 ((uint32_t)0x20000000) /* 位1 */

#define GPIO_CFGLR_CNF ((uint32_t)0xCCCCCCCC) /* 端口 x 配置位组合 */

#define GPIO_CFGLR_CNF0 ((uint32_t)0x0000000C)   /* CNF0[1:0] 位 (端口 x 配置，引脚0) */
#define GPIO_CFGLR_CNF0_0 ((uint32_t)0x00000004) /* 位0 */
#define GPIO_CFGLR_CNF0_1 ((uint32_t)0x00000008) /* 位1 */

#define GPIO_CFGLR_CNF1 ((uint32_t)0x000000C0)   /* CNF1[1:0] 位 (端口 x 配置，引脚1) */
#define GPIO_CFGLR_CNF1_0 ((uint32_t)0x00000040) /* 位0 */
#define GPIO_CFGLR_CNF1_1 ((uint32_t)0x00000080) /* 位1 */

#define GPIO_CFGLR_CNF2 ((uint32_t)0x00000C00)   /* CNF2[1:0] 位 (端口 x 配置，引脚2) */
#define GPIO_CFGLR_CNF2_0 ((uint32_t)0x00000400) /* 位0 */
#define GPIO_CFGLR_CNF2_1 ((uint32_t)0x00000800) /* 位1 */

#define GPIO_CFGLR_CNF3 ((uint32_t)0x0000C000)   /* CNF3[1:0] 位 (端口 x 配置，引脚3) */
#define GPIO_CFGLR_CNF3_0 ((uint32_t)0x00004000) /* 位0 */
#define GPIO_CFGLR_CNF3_1 ((uint32_t)0x00008000) /* 位1 */

#define GPIO_CFGLR_CNF4 ((uint32_t)0x000C0000)   /* CNF4[1:0] 位 (端口 x 配置，引脚4) */
#define GPIO_CFGLR_CNF4_0 ((uint32_t)0x00040000) /* 位0 */
#define GPIO_CFGLR_CNF4_1 ((uint32_t)0x00080000) /* 位1 */

#define GPIO_CFGLR_CNF5 ((uint32_t)0x00C00000)   /* CNF5[1:0] 位 (端口 x 配置，引脚5) */
#define GPIO_CFGLR_CNF5_0 ((uint32_t)0x00400000) /* 位0 */
#define GPIO_CFGLR_CNF5_1 ((uint32_t)0x00800000) /* 位1 */

#define GPIO_CFGLR_CNF6 ((uint32_t)0x0C000000)   /* CNF6[1:0] 位 (端口 x 配置，引脚6) */
#define GPIO_CFGLR_CNF6_0 ((uint32_t)0x04000000) /* 位0 */
#define GPIO_CFGLR_CNF6_1 ((uint32_t)0x08000000) /* 位1 */

#define GPIO_CFGLR_CNF7 ((uint32_t)0xC0000000)   /* CNF7[1:0] 位 (端口 x 配置，引脚7) */
#define GPIO_CFGLR_CNF7_0 ((uint32_t)0x40000000) /* 位0 */
#define GPIO_CFGLR_CNF7_1 ((uint32_t)0x80000000) /* 位1 */

/*******************  GPIO_CFGHR 寄存器的位定义 (高8位配置)  *******************/
#define GPIO_CFGHR_MODE ((uint32_t)0x33333333) /* 端口 x 模式位组合 */

#define GPIO_CFGHR_MODE8 ((uint32_t)0x00000003)   /* MODE8[1:0] 位 (端口 x 模式，引脚8) */
#define GPIO_CFGHR_MODE8_0 ((uint32_t)0x00000001) /* 位0 */
#define GPIO_CFGHR_MODE8_1 ((uint32_t)0x00000002) /* 位1 */

#define GPIO_CFGHR_MODE9 ((uint32_t)0x00000030)   /* MODE9[1:0] 位 (端口 x 模式，引脚9) */
#define GPIO_CFGHR_MODE9_0 ((uint32_t)0x00000010) /* 位0 */
#define GPIO_CFGHR_MODE9_1 ((uint32_t)0x00000020) /* 位1 */

#define GPIO_CFGHR_MODE10 ((uint32_t)0x00000300)   /* MODE10[1:0] 位 (端口 x 模式，引脚10) */
#define GPIO_CFGHR_MODE10_0 ((uint32_t)0x00000100) /* 位0 */
#define GPIO_CFGHR_MODE10_1 ((uint32_t)0x00000200) /* 位1 */

#define GPIO_CFGHR_MODE11 ((uint32_t)0x00003000)   /* MODE11[1:0] 位 (端口 x 模式，引脚11) */
#define GPIO_CFGHR_MODE11_0 ((uint32_t)0x00001000) /* 位0 */
#define GPIO_CFGHR_MODE11_1 ((uint32_t)0x00002000) /* 位1 */

#define GPIO_CFGHR_MODE12 ((uint32_t)0x00030000)   /* MODE12[1:0] 位 (端口 x 模式，引脚12) */
#define GPIO_CFGHR_MODE12_0 ((uint32_t)0x00010000) /* 位0 */
#define GPIO_CFGHR_MODE12_1 ((uint32_t)0x00020000) /* 位1 */

#define GPIO_CFGHR_MODE13 ((uint32_t)0x00300000)   /* MODE13[1:0] 位 (端口 x 模式，引脚13) */
#define GPIO_CFGHR_MODE13_0 ((uint32_t)0x00100000) /* 位0 */
#define GPIO_CFGHR_MODE13_1 ((uint32_t)0x00200000) /* 位1 */

#define GPIO_CFGHR_MODE14 ((uint32_t)0x03000000)   /* MODE14[1:0] 位 (端口 x 模式，引脚14) */
#define GPIO_CFGHR_MODE14_0 ((uint32_t)0x01000000) /* 位0 */
#define GPIO_CFGHR_MODE14_1 ((uint32_t)0x02000000) /* 位1 */

#define GPIO_CFGHR_MODE15 ((uint32_t)0x30000000)   /* MODE15[1:0] 位 (端口 x 模式，引脚15) */
#define GPIO_CFGHR_MODE15_0 ((uint32_t)0x10000000) /* 位0 */
#define GPIO_CFGHR_MODE15_1 ((uint32_t)0x20000000) /* 位1 */

#define GPIO_CFGHR_CNF ((uint32_t)0xCCCCCCCC) /* 端口 x 配置位组合 */

#define GPIO_CFGHR_CNF8 ((uint32_t)0x0000000C)   /* CNF8[1:0] 位 (端口 x 配置，引脚8) */
#define GPIO_CFGHR_CNF8_0 ((uint32_t)0x00000004) /* 位0 */
#define GPIO_CFGHR_CNF8_1 ((uint32_t)0x00000008) /* 位1 */

#define GPIO_CFGHR_CNF9 ((uint32_t)0x000000C0)   /* CNF9[1:0] 位 (端口 x 配置，引脚9) */
#define GPIO_CFGHR_CNF9_0 ((uint32_t)0x00000040) /* 位0 */
#define GPIO_CFGHR_CNF9_1 ((uint32_t)0x00000080) /* 位1 */

#define GPIO_CFGHR_CNF10 ((uint32_t)0x00000C00)   /* CNF10[1:0] 位 (端口 x 配置，引脚10) */
#define GPIO_CFGHR_CNF10_0 ((uint32_t)0x00000400) /* 位0 */
#define GPIO_CFGHR_CNF10_1 ((uint32_t)0x00000800) /* 位1 */

#define GPIO_CFGHR_CNF11 ((uint32_t)0x0000C000)   /* CNF11[1:0] 位 (端口 x 配置，引脚11) */
#define GPIO_CFGHR_CNF11_0 ((uint32_t)0x00004000) /* 位0 */
#define GPIO_CFGHR_CNF11_1 ((uint32_t)0x00008000) /* 位1 */

#define GPIO_CFGHR_CNF12 ((uint32_t)0x000C0000)   /* CNF12[1:0] 位 (端口 x 配置，引脚12) */
#define GPIO_CFGHR_CNF12_0 ((uint32_t)0x00040000) /* 位0 */
#define GPIO_CFGHR_CNF12_1 ((uint32_t)0x00080000) /* 位1 */

#define GPIO_CFGHR_CNF13 ((uint32_t)0x00C00000)   /* CNF13[1:0] 位 (端口 x 配置，引脚13) */
#define GPIO_CFGHR_CNF13_0 ((uint32_t)0x00400000) /* 位0 */
#define GPIO_CFGHR_CNF13_1 ((uint32_t)0x00800000) /* 位1 */

#define GPIO_CFGHR_CNF14 ((uint32_t)0x0C000000)   /* CNF14[1:0] 位 (端口 x 配置，引脚14) */
#define GPIO_CFGHR_CNF14_0 ((uint32_t)0x04000000) /* 位0 */
#define GPIO_CFGHR_CNF14_1 ((uint32_t)0x08000000) /* 位1 */

#define GPIO_CFGHR_CNF15 ((uint32_t)0xC0000000)   /* CNF15[1:0] 位 (端口 x 配置，引脚15) */
#define GPIO_CFGHR_CNF15_0 ((uint32_t)0x40000000) /* 位0 */
#define GPIO_CFGHR_CNF15_1 ((uint32_t)0x80000000) /* 位1 */

/*******************  GPIO_INDR 寄存器的位定义  *******************/
#define GPIO_INDR_IDR0 ((uint16_t)0x0001)  /* 端口输入数据，位0 */
#define GPIO_INDR_IDR1 ((uint16_t)0x0002)  /* 端口输入数据，位1 */
#define GPIO_INDR_IDR2 ((uint16_t)0x0004)  /* 端口输入数据，位2 */
#define GPIO_INDR_IDR3 ((uint16_t)0x0008)  /* 端口输入数据，位3 */
#define GPIO_INDR_IDR4 ((uint16_t)0x0010)  /* 端口输入数据，位4 */
#define GPIO_INDR_IDR5 ((uint16_t)0x0020)  /* 端口输入数据，位5 */
#define GPIO_INDR_IDR6 ((uint16_t)0x0040)  /* 端口输入数据，位6 */
#define GPIO_INDR_IDR7 ((uint16_t)0x0080)  /* 端口输入数据，位7 */
#define GPIO_INDR_IDR8 ((uint16_t)0x0100)  /* 端口输入数据，位8 */
#define GPIO_INDR_IDR9 ((uint16_t)0x0200)  /* 端口输入数据，位9 */
#define GPIO_INDR_IDR10 ((uint16_t)0x0400) /* 端口输入数据，位10 */
#define GPIO_INDR_IDR11 ((uint16_t)0x0800) /* 端口输入数据，位11 */
#define GPIO_INDR_IDR12 ((uint16_t)0x1000) /* 端口输入数据，位12 */
#define GPIO_INDR_IDR13 ((uint16_t)0x2000) /* 端口输入数据，位13 */
#define GPIO_INDR_IDR14 ((uint16_t)0x4000) /* 端口输入数据，位14 */
#define GPIO_INDR_IDR15 ((uint16_t)0x8000) /* 端口输入数据，位15 */

/*******************  GPIO_OUTDR 寄存器的位定义  *******************/
#define GPIO_OUTDR_ODR0 ((uint16_t)0x0001)  /* 端口输出数据，位0 */
#define GPIO_OUTDR_ODR1 ((uint16_t)0x0002)  /* 端口输出数据，位1 */
#define GPIO_OUTDR_ODR2 ((uint16_t)0x0004)  /* 端口输出数据，位2 */
#define GPIO_OUTDR_ODR3 ((uint16_t)0x0008)  /* 端口输出数据，位3 */
#define GPIO_OUTDR_ODR4 ((uint16_t)0x0010)  /* 端口输出数据，位4 */
#define GPIO_OUTDR_ODR5 ((uint16_t)0x0020)  /* 端口输出数据，位5 */
#define GPIO_OUTDR_ODR6 ((uint16_t)0x0040)  /* 端口输出数据，位6 */
#define GPIO_OUTDR_ODR7 ((uint16_t)0x0080)  /* 端口输出数据，位7 */
#define GPIO_OUTDR_ODR8 ((uint16_t)0x0100)  /* 端口输出数据，位8 */
#define GPIO_OUTDR_ODR9 ((uint16_t)0x0200)  /* 端口输出数据，位9 */
#define GPIO_OUTDR_ODR10 ((uint16_t)0x0400) /* 端口输出数据，位10 */
#define GPIO_OUTDR_ODR11 ((uint16_t)0x0800) /* 端口输出数据，位11 */
#define GPIO_OUTDR_ODR12 ((uint16_t)0x1000) /* 端口输出数据，位12 */
#define GPIO_OUTDR_ODR13 ((uint16_t)0x2000) /* 端口输出数据，位13 */
#define GPIO_OUTDR_ODR14 ((uint16_t)0x4000) /* 端口输出数据，位14 */
#define GPIO_OUTDR_ODR15 ((uint16_t)0x8000) /* 端口输出数据，位15 */

/******************  GPIO_BSHR 寄存器的位定义 (位设置/复位)  *******************/
#define GPIO_BSHR_BS0 ((uint32_t)0x00000001)  /* 端口 x 置位位0 */
#define GPIO_BSHR_BS1 ((uint32_t)0x00000002)  /* 端口 x 置位位1 */
#define GPIO_BSHR_BS2 ((uint32_t)0x00000004)  /* 端口 x 置位位2 */
#define GPIO_BSHR_BS3 ((uint32_t)0x00000008)  /* 端口 x 置位位3 */
#define GPIO_BSHR_BS4 ((uint32_t)0x00000010)  /* 端口 x 置位位4 */
#define GPIO_BSHR_BS5 ((uint32_t)0x00000020)  /* 端口 x 置位位5 */
#define GPIO_BSHR_BS6 ((uint32_t)0x00000040)  /* 端口 x 置位位6 */
#define GPIO_BSHR_BS7 ((uint32_t)0x00000080)  /* 端口 x 置位位7 */
#define GPIO_BSHR_BS8 ((uint32_t)0x00000100)  /* 端口 x 置位位8 */
#define GPIO_BSHR_BS9 ((uint32_t)0x00000200)  /* 端口 x 置位位9 */
#define GPIO_BSHR_BS10 ((uint32_t)0x00000400) /* 端口 x 置位位10 */
#define GPIO_BSHR_BS11 ((uint32_t)0x00000800) /* 端口 x 置位位11 */
#define GPIO_BSHR_BS12 ((uint32_t)0x00001000) /* 端口 x 置位位12 */
#define GPIO_BSHR_BS13 ((uint32_t)0x00002000) /* 端口 x 置位位13 */
#define GPIO_BSHR_BS14 ((uint32_t)0x00004000) /* 端口 x 置位位14 */
#define GPIO_BSHR_BS15 ((uint32_t)0x00008000) /* 端口 x 置位位15 */

#define GPIO_BSHR_BR0 ((uint32_t)0x00010000)  /* 端口 x 复位位0 */
#define GPIO_BSHR_BR1 ((uint32_t)0x00020000)  /* 端口 x 复位位1 */
#define GPIO_BSHR_BR2 ((uint32_t)0x00040000)  /* 端口 x 复位位2 */
#define GPIO_BSHR_BR3 ((uint32_t)0x00080000)  /* 端口 x 复位位3 */
#define GPIO_BSHR_BR4 ((uint32_t)0x00100000)  /* 端口 x 复位位4 */
#define GPIO_BSHR_BR5 ((uint32_t)0x00200000)  /* 端口 x 复位位5 */
#define GPIO_BSHR_BR6 ((uint32_t)0x00400000)  /* 端口 x 复位位6 */
#define GPIO_BSHR_BR7 ((uint32_t)0x00800000)  /* 端口 x 复位位7 */
#define GPIO_BSHR_BR8 ((uint32_t)0x01000000)  /* 端口 x 复位位8 */
#define GPIO_BSHR_BR9 ((uint32_t)0x02000000)  /* 端口 x 复位位9 */
#define GPIO_BSHR_BR10 ((uint32_t)0x04000000) /* 端口 x 复位位10 */
#define GPIO_BSHR_BR11 ((uint32_t)0x08000000) /* 端口 x 复位位11 */
#define GPIO_BSHR_BR12 ((uint32_t)0x10000000) /* 端口 x 复位位12 */
#define GPIO_BSHR_BR13 ((uint32_t)0x20000000) /* 端口 x 复位位13 */
#define GPIO_BSHR_BR14 ((uint32_t)0x40000000) /* 端口 x 复位位14 */
#define GPIO_BSHR_BR15 ((uint32_t)0x80000000) /* 端口 x 复位位15 */

/*******************  GPIO_BCR 寄存器的位定义 (位清除)  *******************/
#define GPIO_BCR_BR0 ((uint16_t)0x0001)  /* 端口 x 复位位0 */
#define GPIO_BCR_BR1 ((uint16_t)0x0002)  /* 端口 x 复位位1 */
#define GPIO_BCR_BR2 ((uint16_t)0x0004)  /* 端口 x 复位位2 */
#define GPIO_BCR_BR3 ((uint16_t)0x0008)  /* 端口 x 复位位3 */
#define GPIO_BCR_BR4 ((uint16_t)0x0010)  /* 端口 x 复位位4 */
#define GPIO_BCR_BR5 ((uint16_t)0x0020)  /* 端口 x 复位位5 */
#define GPIO_BCR_BR6 ((uint16_t)0x0040)  /* 端口 x 复位位6 */
#define GPIO_BCR_BR7 ((uint16_t)0x0080)  /* 端口 x 复位位7 */
#define GPIO_BCR_BR8 ((uint16_t)0x0100)  /* 端口 x 复位位8 */
#define GPIO_BCR_BR9 ((uint16_t)0x0200)  /* 端口 x 复位位9 */
#define GPIO_BCR_BR10 ((uint16_t)0x0400) /* 端口 x 复位位10 */
#define GPIO_BCR_BR11 ((uint16_t)0x0800) /* 端口 x 复位位11 */
#define GPIO_BCR_BR12 ((uint16_t)0x1000) /* 端口 x 复位位12 */
#define GPIO_BCR_BR13 ((uint16_t)0x2000) /* 端口 x 复位位13 */
#define GPIO_BCR_BR14 ((uint16_t)0x4000) /* 端口 x 复位位14 */
#define GPIO_BCR_BR15 ((uint16_t)0x8000) /* 端口 x 复位位15 */

/******************  GPIO_LCKR 寄存器的位定义 (锁定)  *******************/
#define GPIO_LCK0 ((uint32_t)0x00000001)  /* 端口 x 锁定位0 */
#define GPIO_LCK1 ((uint32_t)0x00000002)  /* 端口 x 锁定位1 */
#define GPIO_LCK2 ((uint32_t)0x00000004)  /* 端口 x 锁定位2 */
#define GPIO_LCK3 ((uint32_t)0x00000008)  /* 端口 x 锁定位3 */
#define GPIO_LCK4 ((uint32_t)0x00000010)  /* 端口 x 锁定位4 */
#define GPIO_LCK5 ((uint32_t)0x00000020)  /* 端口 x 锁定位5 */
#define GPIO_LCK6 ((uint32_t)0x00000040)  /* 端口 x 锁定位6 */
#define GPIO_LCK7 ((uint32_t)0x00000080)  /* 端口 x 锁定位7 */
#define GPIO_LCK8 ((uint32_t)0x00000100)  /* 端口 x 锁定位8 */
#define GPIO_LCK9 ((uint32_t)0x00000200)  /* 端口 x 锁定位9 */
#define GPIO_LCK10 ((uint32_t)0x00000400) /* 端口 x 锁定位10 */
#define GPIO_LCK11 ((uint32_t)0x00000800) /* 端口 x 锁定位11 */
#define GPIO_LCK12 ((uint32_t)0x00001000) /* 端口 x 锁定位12 */
#define GPIO_LCK13 ((uint32_t)0x00002000) /* 端口 x 锁定位13 */
#define GPIO_LCK14 ((uint32_t)0x00004000) /* 端口 x 锁定位14 */
#define GPIO_LCK15 ((uint32_t)0x00008000) /* 端口 x 锁定位15 */
#define GPIO_LCKK ((uint32_t)0x00010000)  /* 锁定键 */

/******************  AFIO_ECR 寄存器的位定义 (事件控制)  *******************/
#define AFIO_ECR_PIN ((uint8_t)0x0F)   /* PIN[3:0] 位 (引脚选择) */
#define AFIO_ECR_PIN_0 ((uint8_t)0x01) /* 位0 */
#define AFIO_ECR_PIN_1 ((uint8_t)0x02) /* 位1 */
#define AFIO_ECR_PIN_2 ((uint8_t)0x04) /* 位2 */
#define AFIO_ECR_PIN_3 ((uint8_t)0x08) /* 位3 */

#define AFIO_ECR_PIN_PX0 ((uint8_t)0x00)  /* 选择引脚0 */
#define AFIO_ECR_PIN_PX1 ((uint8_t)0x01)  /* 选择引脚1 */
#define AFIO_ECR_PIN_PX2 ((uint8_t)0x02)  /* 选择引脚2 */
#define AFIO_ECR_PIN_PX3 ((uint8_t)0x03)  /* 选择引脚3 */
#define AFIO_ECR_PIN_PX4 ((uint8_t)0x04)  /* 选择引脚4 */
#define AFIO_ECR_PIN_PX5 ((uint8_t)0x05)  /* 选择引脚5 */
#define AFIO_ECR_PIN_PX6 ((uint8_t)0x06)  /* 选择引脚6 */
#define AFIO_ECR_PIN_PX7 ((uint8_t)0x07)  /* 选择引脚7 */
#define AFIO_ECR_PIN_PX8 ((uint8_t)0x08)  /* 选择引脚8 */
#define AFIO_ECR_PIN_PX9 ((uint8_t)0x09)  /* 选择引脚9 */
#define AFIO_ECR_PIN_PX10 ((uint8_t)0x0A) /* 选择引脚10 */
#define AFIO_ECR_PIN_PX11 ((uint8_t)0x0B) /* 选择引脚11 */
#define AFIO_ECR_PIN_PX12 ((uint8_t)0x0C) /* 选择引脚12 */
#define AFIO_ECR_PIN_PX13 ((uint8_t)0x0D) /* 选择引脚13 */
#define AFIO_ECR_PIN_PX14 ((uint8_t)0x0E) /* 选择引脚14 */
#define AFIO_ECR_PIN_PX15 ((uint8_t)0x0F) /* 选择引脚15 */

#define AFIO_ECR_PORT ((uint8_t)0x70)   /* PORT[2:0] 位 (端口选择) */
#define AFIO_ECR_PORT_0 ((uint8_t)0x10) /* 位0 */
#define AFIO_ECR_PORT_1 ((uint8_t)0x20) /* 位1 */
#define AFIO_ECR_PORT_2 ((uint8_t)0x40) /* 位2 */

#define AFIO_ECR_PORT_PA ((uint8_t)0x00) /* 选择端口A */
#define AFIO_ECR_PORT_PB ((uint8_t)0x10) /* 选择端口B */
#define AFIO_ECR_PORT_PC ((uint8_t)0x20) /* 选择端口C */
#define AFIO_ECR_PORT_PD ((uint8_t)0x30) /* 选择端口D */
#define AFIO_ECR_PORT_PE ((uint8_t)0x40) /* 选择端口E */

#define AFIO_ECR_EVOE ((uint8_t)0x80) /* 事件输出使能 */

/******************  AFIO_PCFR1 寄存器的位定义 (复用功能配置1)  *******************/
#define AFIO_PCFR1_SPI1_REMAP ((uint32_t)0x00000001)   /* SPI1 重映射 */
#define AFIO_PCFR1_I2C1_REMAP ((uint32_t)0x00000002)   /* I2C1 重映射 */
#define AFIO_PCFR1_USART1_REMAP ((uint32_t)0x00000004) /* USART1 重映射 */
#define AFIO_PCFR1_USART2_REMAP ((uint32_t)0x00000008) /* USART2 重映射 */

#define AFIO_PCFR1_USART3_REMAP ((uint32_t)0x00000030)   /* USART3_REMAP[1:0] 位 (USART3重映射) */
#define AFIO_PCFR1_USART3_REMAP_0 ((uint32_t)0x00000010) /* 位0 */
#define AFIO_PCFR1_USART3_REMAP_1 ((uint32_t)0x00000020) /* 位1 */

#define AFIO_PCFR1_USART3_REMAP_NOREMAP ((uint32_t)0x00000000)       /* 无重映射 (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_PARTIALREMAP1 ((uint32_t)0x00000010) /* 部分重映射1 (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_PARTIALREMAP2 ((uint32_t)0x00000020) /* 部分重映射2 */
#define AFIO_PCFR1_USART3_REMAP_FULLREMAP ((uint32_t)0x00000030)    /* 完全重映射 (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12) */

#define AFIO_PCFR1_TIM1_REMAP ((uint32_t)0x000000C0)   /* TIM1_REMAP[1:0] 位 (TIM1重映射) */
#define AFIO_PCFR1_TIM1_REMAP_0 ((uint32_t)0x00000040) /* 位0 */
#define AFIO_PCFR1_TIM1_REMAP_1 ((uint32_t)0x00000080) /* 位1 */

#define AFIO_PCFR1_TIM1_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15) */
#define AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP ((uint32_t)0x00000040) /* 部分重映射 (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1) */
#define AFIO_PCFR1_TIM1_REMAP_FULLREMAP ((uint32_t)0x000000C0)    /* 完全重映射 (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12) */

#define AFIO_PCFR1_TIM2_REMAP ((uint32_t)0x00000300)   /* TIM2_REMAP[1:0] 位 (TIM2重映射) */
#define AFIO_PCFR1_TIM2_REMAP_0 ((uint32_t)0x00000100) /* 位0 */
#define AFIO_PCFR1_TIM2_REMAP_1 ((uint32_t)0x00000200) /* 位1 */

#define AFIO_PCFR1_TIM2_REMAP_NOREMAP ((uint32_t)0x00000000)       /* 无重映射 (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP1 ((uint32_t)0x00000100) /* 部分重映射1 (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP2 ((uint32_t)0x00000200) /* 部分重映射2 (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11) */
#define AFIO_PCFR1_TIM2_REMAP_FULLREMAP ((uint32_t)0x00000300)     /* 完全重映射 (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11) */

#define AFIO_PCFR1_TIM3_REMAP ((uint32_t)0x00000C00)   /* TIM3_REMAP[1:0] 位 (TIM3重映射) */
#define AFIO_PCFR1_TIM3_REMAP_0 ((uint32_t)0x00000400) /* 位0 */
#define AFIO_PCFR1_TIM3_REMAP_1 ((uint32_t)0x00000800) /* 位1 */

#define AFIO_PCFR1_TIM3_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_PARTIALREMAP ((uint32_t)0x00000800) /* 部分重映射 (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_FULLREMAP ((uint32_t)0x00000C00)    /* 完全重映射 (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9) */

#define AFIO_PCFR1_TIM4_REMAP ((uint32_t)0x00001000) /* TIM4_REMAP 位 (TIM4重映射) */

#define AFIO_PCFR1_CAN_REMAP ((uint32_t)0x00006000)   /* CAN_REMAP[1:0] 位 (CAN复用功能重映射) */
#define AFIO_PCFR1_CAN_REMAP_0 ((uint32_t)0x00002000) /* 位0 */
#define AFIO_PCFR1_CAN_REMAP_1 ((uint32_t)0x00004000) /* 位1 */

#define AFIO_PCFR1_CAN_REMAP_REMAP1 ((uint32_t)0x00000000) /* CANRX 映射到 PA11, CANTX 映射到 PA12 */
#define AFIO_PCFR1_CAN_REMAP_REMAP2 ((uint32_t)0x00004000) /* CANRX 映射到 PB8, CANTX 映射到 PB9 */
#define AFIO_PCFR1_CAN_REMAP_REMAP3 ((uint32_t)0x00006000) /* CANRX 映射到 PD0, CANTX 映射到 PD1 */

#define AFIO_PCFR1_PD01_REMAP ((uint32_t)0x00008000)         /* 端口 D0/D1 映射到 OSC_IN/OSC_OUT */
#define AFIO_PCFR1_TIM5CH4_IREMAP ((uint32_t)0x00010000)     /* TIM5 通道4 内部重映射 */
#define AFIO_PCFR1_ADC1_ETRGINJ_REMAP ((uint32_t)0x00020000) /* ADC1 外部触发注入转换重映射 */
#define AFIO_PCFR1_ADC1_ETRGREG_REMAP ((uint32_t)0x00040000) /* ADC1 外部触发规则转换重映射 */
#define AFIO_PCFR1_ADC2_ETRGINJ_REMAP ((uint32_t)0x00080000) /* ADC2 外部触发注入转换重映射 */
#define AFIO_PCFR1_ADC2_ETRGREG_REMAP ((uint32_t)0x00100000) /* ADC2 外部触发规则转换重映射 */

#define AFIO_PCFR1_ETH_REMAP ((uint32_t)0x00200000) /* 以太网重映射 */

#define AFIO_PCFR1_CAN2_REMAP ((uint32_t)0x00400000) /* CAN2 重映射 */

#define AFIO_PCFR1_MII_RMII_REMAP ((uint32_t)0x00800000) /* MII/RMII 选择重映射 */

#define AFIO_PCFR1_SWJ_CFG ((uint32_t)0x07000000)   /* SWJ_CFG[2:0] 位 (串行线JTAG配置) */
#define AFIO_PCFR1_SWJ_CFG_0 ((uint32_t)0x01000000) /* 位0 */
#define AFIO_PCFR1_SWJ_CFG_1 ((uint32_t)0x02000000) /* 位1 */
#define AFIO_PCFR1_SWJ_CFG_2 ((uint32_t)0x04000000) /* 位2 */

#define AFIO_PCFR1_SWJ_CFG_RESET ((uint32_t)0x00000000)       /* 完全SWJ (JTAG-DP + SW-DP)：复位状态 */
#define AFIO_PCFR1_SWJ_CFG_NOJNTRST ((uint32_t)0x01000000)    /* 完全SWJ (JTAG-DP + SW-DP) 但不含 JNTRST */
#define AFIO_PCFR1_SWJ_CFG_JTAGDISABLE ((uint32_t)0x02000000) /* JTAG-DP 禁用，SW-DP 使能 */
#define AFIO_PCFR1_SWJ_CFG_DISABLE ((uint32_t)0x04000000)     /* JTAG-DP 禁用，SW-DP 禁用 */

#define AFIO_PCFR1_SPI3_REMAP ((uint32_t)0x10000000) /* SPI3 重映射 */

#define AFIO_PCFR1_TIM2ITR1_REMAP ((uint32_t)0x20000000) /* TIM2 内部触发1 重映射 */

#define AFIO_PCFR1_PTP_PPS_REMAP ((uint32_t)0x40000000) /* PTP PPS 重映射 */

/*****************  AFIO_EXTICR1 寄存器的位定义 (外部中断配置1)  *****************/
#define AFIO_EXTICR1_EXTI0 ((uint16_t)0x000F) /* EXTI0 配置 */
#define AFIO_EXTICR1_EXTI1 ((uint16_t)0x00F0) /* EXTI1 配置 */
#define AFIO_EXTICR1_EXTI2 ((uint16_t)0x0F00) /* EXTI2 配置 */
#define AFIO_EXTICR1_EXTI3 ((uint16_t)0xF000) /* EXTI3 配置 */

#define AFIO_EXTICR1_EXTI0_PA ((uint16_t)0x0000) /* PA[0] 引脚 */
#define AFIO_EXTICR1_EXTI0_PB ((uint16_t)0x0001) /* PB[0] 引脚 */
#define AFIO_EXTICR1_EXTI0_PC ((uint16_t)0x0002) /* PC[0] 引脚 */
#define AFIO_EXTICR1_EXTI0_PD ((uint16_t)0x0003) /* PD[0] 引脚 */
#define AFIO_EXTICR1_EXTI0_PE ((uint16_t)0x0004) /* PE[0] 引脚 */

#define AFIO_EXTICR1_EXTI1_PA ((uint16_t)0x0000) /* PA[1] 引脚 */
#define AFIO_EXTICR1_EXTI1_PB ((uint16_t)0x0010) /* PB[1] 引脚 */
#define AFIO_EXTICR1_EXTI1_PC ((uint16_t)0x0020) /* PC[1] 引脚 */
#define AFIO_EXTICR1_EXTI1_PD ((uint16_t)0x0030) /* PD[1] 引脚 */
#define AFIO_EXTICR1_EXTI1_PE ((uint16_t)0x0040) /* PE[1] 引脚 */

#define AFIO_EXTICR1_EXTI2_PA ((uint16_t)0x0000) /* PA[2] 引脚 */
#define AFIO_EXTICR1_EXTI2_PB ((uint16_t)0x0100) /* PB[2] 引脚 */
#define AFIO_EXTICR1_EXTI2_PC ((uint16_t)0x0200) /* PC[2] 引脚 */
#define AFIO_EXTICR1_EXTI2_PD ((uint16_t)0x0300) /* PD[2] 引脚 */
#define AFIO_EXTICR1_EXTI2_PE ((uint16_t)0x0400) /* PE[2] 引脚 */

#define AFIO_EXTICR1_EXTI3_PA ((uint16_t)0x0000) /* PA[3] 引脚 */
#define AFIO_EXTICR1_EXTI3_PB ((uint16_t)0x1000) /* PB[3] 引脚 */
#define AFIO_EXTICR1_EXTI3_PC ((uint16_t)0x2000) /* PC[3] 引脚 */
#define AFIO_EXTICR1_EXTI3_PD ((uint16_t)0x3000) /* PD[3] 引脚 */
#define AFIO_EXTICR1_EXTI3_PE ((uint16_t)0x4000) /* PE[3] 引脚 */

/*****************  AFIO_EXTICR2 寄存器的位定义 (外部中断配置2)  *****************/
#define AFIO_EXTICR2_EXTI4 ((uint16_t)0x000F) /* EXTI4 配置 */
#define AFIO_EXTICR2_EXTI5 ((uint16_t)0x00F0) /* EXTI5 配置 */
#define AFIO_EXTICR2_EXTI6 ((uint16_t)0x0F00) /* EXTI6 配置 */
#define AFIO_EXTICR2_EXTI7 ((uint16_t)0xF000) /* EXTI7 配置 */

#define AFIO_EXTICR2_EXTI4_PA ((uint16_t)0x0000) /* PA[4] 引脚 */
#define AFIO_EXTICR2_EXTI4_PB ((uint16_t)0x0001) /* PB[4] 引脚 */
#define AFIO_EXTICR2_EXTI4_PC ((uint16_t)0x0002) /* PC[4] 引脚 */
#define AFIO_EXTICR2_EXTI4_PD ((uint16_t)0x0003) /* PD[4] 引脚 */
#define AFIO_EXTICR2_EXTI4_PE ((uint16_t)0x0004) /* PE[4] 引脚 */

#define AFIO_EXTICR2_EXTI5_PA ((uint16_t)0x0000) /* PA[5] 引脚 */
#define AFIO_EXTICR2_EXTI5_PB ((uint16_t)0x0010) /* PB[5] 引脚 */
#define AFIO_EXTICR2_EXTI5_PC ((uint16_t)0x0020) /* PC[5] 引脚 */
#define AFIO_EXTICR2_EXTI5_PD ((uint16_t)0x0030) /* PD[5] 引脚 */
#define AFIO_EXTICR2_EXTI5_PE ((uint16_t)0x0040) /* PE[5] 引脚 */

#define AFIO_EXTICR2_EXTI6_PA ((uint16_t)0x0000) /* PA[6] 引脚 */
#define AFIO_EXTICR2_EXTI6_PB ((uint16_t)0x0100) /* PB[6] 引脚 */
#define AFIO_EXTICR2_EXTI6_PC ((uint16_t)0x0200) /* PC[6] 引脚 */
#define AFIO_EXTICR2_EXTI6_PD ((uint16_t)0x0300) /* PD[6] 引脚 */
#define AFIO_EXTICR2_EXTI6_PE ((uint16_t)0x0400) /* PE[6] 引脚 */

#define AFIO_EXTICR2_EXTI7_PA ((uint16_t)0x0000) /* PA[7] 引脚 */
#define AFIO_EXTICR2_EXTI7_PB ((uint16_t)0x1000) /* PB[7] 引脚 */
#define AFIO_EXTICR2_EXTI7_PC ((uint16_t)0x2000) /* PC[7] 引脚 */
#define AFIO_EXTICR2_EXTI7_PD ((uint16_t)0x3000) /* PD[7] 引脚 */
#define AFIO_EXTICR2_EXTI7_PE ((uint16_t)0x4000) /* PE[7] 引脚 */

/*****************  AFIO_EXTICR3 寄存器的位定义 (外部中断配置3)  *****************/
#define AFIO_EXTICR3_EXTI8 ((uint16_t)0x000F)  /* EXTI8 配置 */
#define AFIO_EXTICR3_EXTI9 ((uint16_t)0x00F0)  /* EXTI9 配置 */
#define AFIO_EXTICR3_EXTI10 ((uint16_t)0x0F00) /* EXTI10 配置 */
#define AFIO_EXTICR3_EXTI11 ((uint16_t)0xF000) /* EXTI11 配置 */

#define AFIO_EXTICR3_EXTI8_PA ((uint16_t)0x0000) /* PA[8] 引脚 */
#define AFIO_EXTICR3_EXTI8_PB ((uint16_t)0x0001) /* PB[8] 引脚 */
#define AFIO_EXTICR3_EXTI8_PC ((uint16_t)0x0002) /* PC[8] 引脚 */
#define AFIO_EXTICR3_EXTI8_PD ((uint16_t)0x0003) /* PD[8] 引脚 */
#define AFIO_EXTICR3_EXTI8_PE ((uint16_t)0x0004) /* PE[8] 引脚 */

#define AFIO_EXTICR3_EXTI9_PA ((uint16_t)0x0000) /* PA[9] 引脚 */
#define AFIO_EXTICR3_EXTI9_PB ((uint16_t)0x0010) /* PB[9] 引脚 */
#define AFIO_EXTICR3_EXTI9_PC ((uint16_t)0x0020) /* PC[9] 引脚 */
#define AFIO_EXTICR3_EXTI9_PD ((uint16_t)0x0030) /* PD[9] 引脚 */
#define AFIO_EXTICR3_EXTI9_PE ((uint16_t)0x0040) /* PE[9] 引脚 */

#define AFIO_EXTICR3_EXTI10_PA ((uint16_t)0x0000) /* PA[10] 引脚 */
#define AFIO_EXTICR3_EXTI10_PB ((uint16_t)0x0100) /* PB[10] 引脚 */
#define AFIO_EXTICR3_EXTI10_PC ((uint16_t)0x0200) /* PC[10] 引脚 */
#define AFIO_EXTICR3_EXTI10_PD ((uint16_t)0x0300) /* PD[10] 引脚 */
#define AFIO_EXTICR3_EXTI10_PE ((uint16_t)0x0400) /* PE[10] 引脚 */

#define AFIO_EXTICR3_EXTI11_PA ((uint16_t)0x0000) /* PA[11] 引脚 */
#define AFIO_EXTICR3_EXTI11_PB ((uint16_t)0x1000) /* PB[11] 引脚 */
#define AFIO_EXTICR3_EXTI11_PC ((uint16_t)0x2000) /* PC[11] 引脚 */
#define AFIO_EXTICR3_EXTI11_PD ((uint16_t)0x3000) /* PD[11] 引脚 */
#define AFIO_EXTICR3_EXTI11_PE ((uint16_t)0x4000) /* PE[11] 引脚 */

/*****************  AFIO_EXTICR4 寄存器的位定义 (外部中断配置4)  *****************/
#define AFIO_EXTICR4_EXTI12 ((uint16_t)0x000F) /* EXTI12 配置 */
#define AFIO_EXTICR4_EXTI13 ((uint16_t)0x00F0) /* EXTI13 配置 */
#define AFIO_EXTICR4_EXTI14 ((uint16_t)0x0F00) /* EXTI14 配置 */
#define AFIO_EXTICR4_EXTI15 ((uint16_t)0xF000) /* EXTI15 配置 */

#define AFIO_EXTICR4_EXTI12_PA ((uint16_t)0x0000) /* PA[12] 引脚 */
#define AFIO_EXTICR4_EXTI12_PB ((uint16_t)0x0001) /* PB[12] 引脚 */
#define AFIO_EXTICR4_EXTI12_PC ((uint16_t)0x0002) /* PC[12] 引脚 */
#define AFIO_EXTICR4_EXTI12_PD ((uint16_t)0x0003) /* PD[12] 引脚 */
#define AFIO_EXTICR4_EXTI12_PE ((uint16_t)0x0004) /* PE[12] 引脚 */

#define AFIO_EXTICR4_EXTI13_PA ((uint16_t)0x0000) /* PA[13] 引脚 */
#define AFIO_EXTICR4_EXTI13_PB ((uint16_t)0x0010) /* PB[13] 引脚 */
#define AFIO_EXTICR4_EXTI13_PC ((uint16_t)0x0020) /* PC[13] 引脚 */
#define AFIO_EXTICR4_EXTI13_PD ((uint16_t)0x0030) /* PD[13] 引脚 */
#define AFIO_EXTICR4_EXTI13_PE ((uint16_t)0x0040) /* PE[13] 引脚 */

#define AFIO_EXTICR4_EXTI14_PA ((uint16_t)0x0000) /* PA[14] 引脚 */
#define AFIO_EXTICR4_EXTI14_PB ((uint16_t)0x0100) /* PB[14] 引脚 */
#define AFIO_EXTICR4_EXTI14_PC ((uint16_t)0x0200) /* PC[14] 引脚 */
#define AFIO_EXTICR4_EXTI14_PD ((uint16_t)0x0300) /* PD[14] 引脚 */
#define AFIO_EXTICR4_EXTI14_PE ((uint16_t)0x0400) /* PE[14] 引脚 */

#define AFIO_EXTICR4_EXTI15_PA ((uint16_t)0x0000) /* PA[15] 引脚 */
#define AFIO_EXTICR4_EXTI15_PB ((uint16_t)0x1000) /* PB[15] 引脚 */
#define AFIO_EXTICR4_EXTI15_PC ((uint16_t)0x2000) /* PC[15] 引脚 */
#define AFIO_EXTICR4_EXTI15_PD ((uint16_t)0x3000) /* PD[15] 引脚 */
#define AFIO_EXTICR4_EXTI15_PE ((uint16_t)0x4000) /* PE[15] 引脚 */

/******************  AFIO_PCFR2 寄存器的位定义 (复用功能配置2)  *******************/
#define AFIO_PCFR2_TIM8_REMAP ((uint32_t)0x00000004) /* TIM8 重映射 */

#define AFIO_PCFR2_TIM9_REMAP ((uint32_t)0x00000018)   /* TIM9_REMAP[1:0] 位 (TIM9重映射) */
#define AFIO_PCFR2_TIM9_REMAP_0 ((uint32_t)0x00000008) /* 位0 */
#define AFIO_PCFR2_TIM9_REMAP_1 ((uint32_t)0x00000010) /* 位1 */

#define AFIO_PCFR2_TIM9_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 */
#define AFIO_PCFR2_TIM9_REMAP_PARTIALREMAP ((uint32_t)0x00000008) /* 部分重映射 */
#define AFIO_PCFR2_TIM9_REMAP_FULLREMAP ((uint32_t)0x00000010)    /* 完全重映射 */

#define AFIO_PCFR2_TIM10_REMAP ((uint32_t)0x00000060)   /* TIM10_REMAP[1:0] 位 (TIM10重映射) */
#define AFIO_PCFR2_TIM10_REMAP_0 ((uint32_t)0x00000020) /* 位0 */
#define AFIO_PCFR2_TIM10_REMAP_1 ((uint32_t)0x00000040) /* 位1 */

#define AFIO_PCFR2_TIM10_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 */
#define AFIO_PCFR2_TIM10_REMAP_PARTIALREMAP ((uint32_t)0x00000020) /* 部分重映射 */
#define AFIO_PCFR2_TIM10_REMAP_FULLREMAP ((uint32_t)0x00000040)    /* 完全重映射 */

#define AFIO_PCFR2_FSMC_NADV_REMAP ((uint32_t)0x00000400) /* FSMC NADV 重映射 */

#define AFIO_PCFR2_USART4_REMAP ((uint32_t)0x00030000)   /* USART4_REMAP[1:0] 位 (USART4重映射) */
#define AFIO_PCFR2_USART4_REMAP_0 ((uint32_t)0x00010000) /* 位0 */
#define AFIO_PCFR2_USART4_REMAP_1 ((uint32_t)0x00020000) /* 位1 */

#define AFIO_PCFR2_USART4_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 */
#define AFIO_PCFR2_USART4_REMAP_PARTIALREMAP ((uint32_t)0x00010000) /* 部分重映射 */
#define AFIO_PCFR2_USART4_REMAP_FULLREMAP ((uint32_t)0x00020000)    /* 完全重映射 */

#define AFIO_PCFR2_USART5_REMAP ((uint32_t)0x000C0000)   /* USART5_REMAP[1:0] 位 (USART5重映射) */
#define AFIO_PCFR2_USART5_REMAP_0 ((uint32_t)0x00040000) /* 位0 */
#define AFIO_PCFR2_USART5_REMAP_1 ((uint32_t)0x00080000) /* 位1 */

#define AFIO_PCFR2_USART5_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 */
#define AFIO_PCFR2_USART5_REMAP_PARTIALREMAP ((uint32_t)0x00040000) /* 部分重映射 */
#define AFIO_PCFR2_USART5_REMAP_FULLREMAP ((uint32_t)0x00080000)    /* 完全重映射 */

#define AFIO_PCFR2_USART6_REMAP ((uint32_t)0x00300000)   /* USART6_REMAP[1:0] 位 (USART6重映射) */
#define AFIO_PCFR2_USART6_REMAP_0 ((uint32_t)0x00100000) /* 位0 */
#define AFIO_PCFR2_USART6_REMAP_1 ((uint32_t)0x00200000) /* 位1 */

#define AFIO_PCFR2_USART6_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 */
#define AFIO_PCFR2_USART6_REMAP_PARTIALREMAP ((uint32_t)0x00100000) /* 部分重映射 */
#define AFIO_PCFR2_USART6_REMAP_FULLREMAP ((uint32_t)0x00200000)    /* 完全重映射 */

#define AFIO_PCFR2_USART7_REMAP ((uint32_t)0x00C00000)   /* USART7_REMAP[1:0] 位 (USART7重映射) */
#define AFIO_PCFR2_USART7_REMAP_0 ((uint32_t)0x00400000) /* 位0 */
#define AFIO_PCFR2_USART7_REMAP_1 ((uint32_t)0x00800000) /* 位1 */

#define AFIO_PCFR2_USART7_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 */
#define AFIO_PCFR2_USART7_REMAP_PARTIALREMAP ((uint32_t)0x00400000) /* 部分重映射 */
#define AFIO_PCFR2_USART7_REMAP_FULLREMAP ((uint32_t)0x00800000)    /* 完全重映射 */

#define AFIO_PCFR2_USART8_REMAP ((uint32_t)0x03000000)   /* USART8_REMAP[1:0] 位 (USART8重映射) */
#define AFIO_PCFR2_USART8_REMAP_0 ((uint32_t)0x01000000) /* 位0 */
#define AFIO_PCFR2_USART8_REMAP_1 ((uint32_t)0x02000000) /* 位1 */

#define AFIO_PCFR2_USART8_REMAP_NOREMAP ((uint32_t)0x00000000)      /* 无重映射 */
#define AFIO_PCFR2_USART8_REMAP_PARTIALREMAP ((uint32_t)0x01000000) /* 部分重映射 */
#define AFIO_PCFR2_USART8_REMAP_FULLREMAP ((uint32_t)0x02000000)    /* 完全重映射 */

#define AFIO_PCFR2_USART1_REMAP ((uint32_t)0x04000000) /* USART1 重映射 */

/******************************************************************************/
/*                           独立看门狗 (IWDG)                                 */
/******************************************************************************/

/*******************  IWDG_CTLR 寄存器的位定义  ********************/
#define IWDG_KEY ((uint16_t)0xFFFF) /* 键值（只写，读为0000h） */

/*******************  IWDG_PSCR 寄存器的位定义  ********************/
#define IWDG_PR ((uint8_t)0x07)   /* PR[2:0] (预分频器分频系数) */
#define IWDG_PR_0 ((uint8_t)0x01) /* 位0 */
#define IWDG_PR_1 ((uint8_t)0x02) /* 位1 */
#define IWDG_PR_2 ((uint8_t)0x04) /* 位2 */

/*******************  IWDG_RLDR 寄存器的位定义  *******************/
#define IWDG_RL ((uint16_t)0x0FFF) /* 看门狗计数器重装载值 */

/*******************  IWDG_STATR 寄存器的位定义  ********************/
#define IWDG_PVU ((uint8_t)0x01) /* 看门狗预分频值更新进行中 */
#define IWDG_RVU ((uint8_t)0x02) /* 看门狗计数器重装载值更新进行中 */

/******************************************************************************/
/*                      集成电路互联总线 (I2C)                                 */
/******************************************************************************/

/*******************  I2C_CTLR1 寄存器的位定义  ********************/
#define I2C_CTLR1_PE ((uint16_t)0x0001)        /* 外设使能 */
#define I2C_CTLR1_SMBUS ((uint16_t)0x0002)     /* SMBus 模式 */
#define I2C_CTLR1_SMBTYPE ((uint16_t)0x0008)   /* SMBus 类型 */
#define I2C_CTLR1_ENARP ((uint16_t)0x0010)     /* ARP 使能 */
#define I2C_CTLR1_ENPEC ((uint16_t)0x0020)     /* PEC 使能 */
#define I2C_CTLR1_ENGC ((uint16_t)0x0040)      /* 广播呼叫使能 */
#define I2C_CTLR1_NOSTRETCH ((uint16_t)0x0080) /* 时钟延长禁止（从模式） */
#define I2C_CTLR1_START ((uint16_t)0x0100)     /* 起始条件生成 */
#define I2C_CTLR1_STOP ((uint16_t)0x0200)      /* 停止条件生成 */
#define I2C_CTLR1_ACK ((uint16_t)0x0400)       /* 应答使能 */
#define I2C_CTLR1_POS ((uint16_t)0x0800)       /* 应答/PEC 位置（用于数据接收） */
#define I2C_CTLR1_PEC ((uint16_t)0x1000)       /* 包错误校验 */
#define I2C_CTLR1_ALERT ((uint16_t)0x2000)     /* SMBus 警报 */
#define I2C_CTLR1_SWRST ((uint16_t)0x8000)     /* 软件复位 */

/*******************  I2C_CTLR2 寄存器的位定义  ********************/
#define I2C_CTLR2_FREQ ((uint16_t)0x003F)   /* FREQ[5:0] 位 (外设时钟频率) */
#define I2C_CTLR2_FREQ_0 ((uint16_t)0x0001) /* 位0 */
#define I2C_CTLR2_FREQ_1 ((uint16_t)0x0002) /* 位1 */
#define I2C_CTLR2_FREQ_2 ((uint16_t)0x0004) /* 位2 */
#define I2C_CTLR2_FREQ_3 ((uint16_t)0x0008) /* 位3 */
#define I2C_CTLR2_FREQ_4 ((uint16_t)0x0010) /* 位4 */
#define I2C_CTLR2_FREQ_5 ((uint16_t)0x0020) /* 位5 */

#define I2C_CTLR2_ITERREN ((uint16_t)0x0100) /* 错误中断使能 */
#define I2C_CTLR2_ITEVTEN ((uint16_t)0x0200) /* 事件中断使能 */
#define I2C_CTLR2_ITBUFEN ((uint16_t)0x0400) /* 缓冲区中断使能 */
#define I2C_CTLR2_DMAEN ((uint16_t)0x0800)   /* DMA 请求使能 */
#define I2C_CTLR2_LAST ((uint16_t)0x1000)    /* DMA 最后一次传输 */

/*******************  I2C_OADDR1 寄存器的位定义  *******************/
#define I2C_OADDR1_ADD0 ((uint16_t)0x0001)     /* 地址位0 */
#define I2C_OADDR1_ADD1_7 ((uint16_t)0x00FE)   /* 地址位1-7 */
#define I2C_OADDR1_ADD8_9 ((uint16_t)0x0300)   /* 地址位8-9 */

#define I2C_OADDR1_ADD0 ((uint16_t)0x0001)      /* 位0 */
#define I2C_OADDR1_ADD1 ((uint16_t)0x0002)      /* 位1 */
#define I2C_OADDR1_ADD2 ((uint16_t)0x0004)      /* 位2 */
#define I2C_OADDR1_ADD3 ((uint16_t)0x0008)      /* 位3 */
#define I2C_OADDR1_ADD4 ((uint16_t)0x0010)      /* 位4 */
#define I2C_OADDR1_ADD5 ((uint16_t)0x0020)      /* 位5 */
#define I2C_OADDR1_ADD6 ((uint16_t)0x0040)      /* 位6 */
#define I2C_OADDR1_ADD7 ((uint16_t)0x0080)      /* 位7 */
#define I2C_OADDR1_ADD8 ((uint16_t)0x0100)      /* 位8 */
#define I2C_OADDR1_ADD9 ((uint16_t)0x0200)      /* 位9 */

#define I2C_OADDR1_ADDMODE ((uint16_t)0x8000) /* 寻址模式（从模式） */

/*******************  I2C_OADDR2 寄存器的位定义  *******************/
#define I2C_OADDR2_ENDUAL ((uint8_t)0x01) /* 双地址模式使能 */
#define I2C_OADDR2_ADD2 ((uint8_t)0xFE)   /* 接口地址 */

/********************  I2C_DATAR 寄存器的位定义  ********************/
#define I2C_DR_DATAR ((uint8_t)0xFF) /* 8位数据寄存器 */

/*******************  I2C_STAR1 寄存器的位定义  ********************/
#define I2C_STAR1_SB ((uint16_t)0x0001)       /* 起始位（主模式） */
#define I2C_STAR1_ADDR ((uint16_t)0x0002)     /* 地址已发送（主模式）/已匹配（从模式） */
#define I2C_STAR1_BTF ((uint16_t)0x0004)      /* 字节传输完成 */
#define I2C_STAR1_ADD10 ((uint16_t)0x0008)    /* 10位头已发送（主模式） */
#define I2C_STAR1_STOPF ((uint16_t)0x0010)    /* 停止检测（从模式） */
#define I2C_STAR1_RXNE ((uint16_t)0x0040)     /* 数据寄存器非空（接收器） */
#define I2C_STAR1_TXE ((uint16_t)0x0080)      /* 数据寄存器空（发送器） */
#define I2C_STAR1_BERR ((uint16_t)0x0100)     /* 总线错误 */
#define I2C_STAR1_ARLO ((uint16_t)0x0200)     /* 仲裁丢失（主模式） */
#define I2C_STAR1_AF ((uint16_t)0x0400)       /* 应答失败 */
#define I2C_STAR1_OVR ((uint16_t)0x0800)      /* 溢出/下溢 */
#define I2C_STAR1_PECERR ((uint16_t)0x1000)   /* 接收时PEC错误 */
#define I2C_STAR1_TIMEOUT ((uint16_t)0x4000)  /* 超时或Tlow错误 */
#define I2C_STAR1_SMBALERT ((uint16_t)0x8000) /* SMBus 警报 */

/*******************  I2C_STAR2 寄存器的位定义  ********************/
#define I2C_STAR2_MSL ((uint16_t)0x0001)        /* 主/从 */
#define I2C_STAR2_BUSY ((uint16_t)0x0002)       /* 总线忙 */
#define I2C_STAR2_TRA ((uint16_t)0x0004)        /* 发送器/接收器 */
#define I2C_STAR2_GENCALL ((uint16_t)0x0010)    /* 广播呼叫地址（从模式） */
#define I2C_STAR2_SMBDEFAULT ((uint16_t)0x0020) /* SMBus 设备默认地址（从模式） */
#define I2C_STAR2_SMBHOST ((uint16_t)0x0040)    /* SMBus 主机头（从模式） */
#define I2C_STAR2_DUALF ((uint16_t)0x0080)      /* 双标志（从模式） */
#define I2C_STAR2_PEC ((uint16_t)0xFF00)        /* 包错误校验寄存器 */

/*******************  I2C_CKCFGR 寄存器的位定义  ********************/
#define I2C_CKCFGR_CCR ((uint16_t)0x0FFF)  /* 时钟控制寄存器（主模式，标准/快速模式） */
#define I2C_CKCFGR_DUTY ((uint16_t)0x4000) /* 快速模式占空比 */
#define I2C_CKCFGR_FS ((uint16_t)0x8000)   /* I2C 主模式选择 */

/******************  I2C_RTR 寄存器的位定义  *******************/
#define I2C_RTR_TRISE ((uint8_t)0x3F) /* 最大上升时间（主模式，标准/快速模式） */

/******************************************************************************/
/*                             电源控制 (PWR)                                  */
/******************************************************************************/

/********************  PWR_CTLR 寄存器的位定义  ********************/
#define PWR_CTLR_LPDS ((uint16_t)0x0001) /* 低功耗深度睡眠 */
#define PWR_CTLR_PDDS ((uint16_t)0x0002) /* 断电深度睡眠 */
#define PWR_CTLR_CWUF ((uint16_t)0x0004) /* 清除唤醒标志 */
#define PWR_CTLR_CSBF ((uint16_t)0x0008) /* 清除待机标志 */
#define PWR_CTLR_PVDE ((uint16_t)0x0010) /* 电源电压检测器使能 */

#define PWR_CTLR_PLS ((uint16_t)0x00E0)   /* PLS[2:0] 位 (PVD 电平选择) */
#define PWR_CTLR_PLS_0 ((uint16_t)0x0020) /* 位0 */
#define PWR_CTLR_PLS_1 ((uint16_t)0x0040) /* 位1 */
#define PWR_CTLR_PLS_2 ((uint16_t)0x0080) /* 位2 */

#define PWR_CTLR_PLS_MODE0 ((uint16_t)0x0000) /* 2.2V */
#define PWR_CTLR_PLS_MODE1 ((uint16_t)0x0020) /* 2.3V */
#define PWR_CTLR_PLS_MODE2 ((uint16_t)0x0040) /* 2.4V */
#define PWR_CTLR_PLS_MODE3 ((uint16_t)0x0060) /* 2.5V */
#define PWR_CTLR_PLS_MODE4 ((uint16_t)0x0080) /* 2.6V */
#define PWR_CTLR_PLS_MODE5 ((uint16_t)0x00A0) /* 2.7V */
#define PWR_CTLR_PLS_MODE6 ((uint16_t)0x00C0) /* 2.8V */
#define PWR_CTLR_PLS_MODE7 ((uint16_t)0x00E0) /* 2.9V */

#define PWR_CTLR_DBP ((uint16_t)0x0100) /* 禁用备份域写保护 */

#define PWR_CTLR_R2KSTY ((uint32_t)0x10000)   /* 2K 保持 */
#define PWR_CTLR_R30KSTY ((uint32_t)0x20000)  /* 30K 保持 */
#define PWR_CTLR_R2KVBAT ((uint32_t)0x40000)  /* 2K VBAT 保持 */
#define PWR_CTLR_R30KVBAT ((uint32_t)0x80000) /* 30K VBAT 保持 */
#define PWR_CTLR_RAMLV ((uint32_t)0x100000)   /* RAM 电平 */

/*******************  PWR_CSR 寄存器的位定义  ********************/
#define PWR_CSR_WUF ((uint16_t)0x0001)  /* 唤醒标志 */
#define PWR_CSR_SBF ((uint16_t)0x0002)  /* 待机标志 */
#define PWR_CSR_PVDO ((uint16_t)0x0004) /* PVD 输出 */
#define PWR_CSR_EWUP ((uint16_t)0x0100) /* 使能 WKUP 引脚 */

/******************************************************************************/
/*                         复位和时钟控制 (RCC)                                */
/******************************************************************************/

/********************  RCC_CTLR 寄存器的位定义  ********************/
#define RCC_HSION ((uint32_t)0x00000001)   /* 内部高速时钟使能 */
#define RCC_HSIRDY ((uint32_t)0x00000002)  /* 内部高速时钟就绪标志 */
#define RCC_HSITRIM ((uint32_t)0x000000F8) /* 内部高速时钟修调 */
#define RCC_HSICAL ((uint32_t)0x0000FF00)  /* 内部高速时钟校准 */
#define RCC_HSEON ((uint32_t)0x00010000)   /* 外部高速时钟使能 */
#define RCC_HSERDY ((uint32_t)0x00020000)  /* 外部高速时钟就绪标志 */
#define RCC_HSEBYP ((uint32_t)0x00040000)  /* 外部高速时钟旁路 */
#define RCC_CSSON ((uint32_t)0x00080000)   /* 时钟安全系统使能 */
#define RCC_PLLON ((uint32_t)0x01000000)   /* PLL 使能 */
#define RCC_PLLRDY ((uint32_t)0x02000000)  /* PLL 时钟就绪标志 */
#define RCC_PLL2ON ((uint32_t)0x04000000)  /* PLL2 使能 */
#define RCC_PLL2RDY ((uint32_t)0x08000000) /* PLL2 就绪 */
#define RCC_PLL3ON ((uint32_t)0x10000000)  /* PLL3 使能 */
#define RCC_PLL3RDY ((uint32_t)0x20000000) /* PLL3 就绪 */

/*******************  RCC_CFGR0 寄存器的位定义  *******************/
#define RCC_SW ((uint32_t)0x00000003)   /* SW[1:0] 位 (系统时钟切换) */
#define RCC_SW_0 ((uint32_t)0x00000001) /* 位0 */
#define RCC_SW_1 ((uint32_t)0x00000002) /* 位1 */

#define RCC_SW_HSI ((uint32_t)0x00000000) /* HSI 选择为系统时钟 */
#define RCC_SW_HSE ((uint32_t)0x00000001) /* HSE 选择为系统时钟 */
#define RCC_SW_PLL ((uint32_t)0x00000002) /* PLL 选择为系统时钟 */

#define RCC_SWS ((uint32_t)0x0000000C)   /* SWS[1:0] 位 (系统时钟切换状态) */
#define RCC_SWS_0 ((uint32_t)0x00000004) /* 位0 */
#define RCC_SWS_1 ((uint32_t)0x00000008) /* 位1 */

#define RCC_SWS_HSI ((uint32_t)0x00000000) /* HSI 振荡器用作系统时钟 */
#define RCC_SWS_HSE ((uint32_t)0x00000004) /* HSE 振荡器用作系统时钟 */
#define RCC_SWS_PLL ((uint32_t)0x00000008) /* PLL 用作系统时钟 */

#define RCC_HPRE ((uint32_t)0x000000F0)   /* HPRE[3:0] 位 (AHB 预分频器) */
#define RCC_HPRE_0 ((uint32_t)0x00000010) /* 位0 */
#define RCC_HPRE_1 ((uint32_t)0x00000020) /* 位1 */
#define RCC_HPRE_2 ((uint32_t)0x00000040) /* 位2 */
#define RCC_HPRE_3 ((uint32_t)0x00000080) /* 位3 */

#define RCC_HPRE_MASK ((uint32_t)0x000000F0)   /* 清空位，配合&= ~使用 */
#define RCC_HPRE_DIV1 ((uint32_t)0x00000000)   /* SYSCLK 不分频 */
#define RCC_HPRE_DIV2 ((uint32_t)0x00000080)   /* SYSCLK 除以2 */
#define RCC_HPRE_DIV4 ((uint32_t)0x00000090)   /* SYSCLK 除以4 */
#define RCC_HPRE_DIV8 ((uint32_t)0x000000A0)   /* SYSCLK 除以8 */
#define RCC_HPRE_DIV16 ((uint32_t)0x000000B0)  /* SYSCLK 除以16 */
#define RCC_HPRE_DIV64 ((uint32_t)0x000000C0)  /* SYSCLK 除以64 */
#define RCC_HPRE_DIV128 ((uint32_t)0x000000D0) /* SYSCLK 除以128 */
#define RCC_HPRE_DIV256 ((uint32_t)0x000000E0) /* SYSCLK 除以256 */
#define RCC_HPRE_DIV512 ((uint32_t)0x000000F0) /* SYSCLK 除以512 */

#define RCC_PPRE1 ((uint32_t)0x00000700)   /* PRE1[2:0] 位 (APB1 预分频器) */
#define RCC_PPRE1_0 ((uint32_t)0x00000100) /* 位0 */
#define RCC_PPRE1_1 ((uint32_t)0x00000200) /* 位1 */
#define RCC_PPRE1_2 ((uint32_t)0x00000400) /* 位2 */

#define RCC_PPRE1_DIV1 ((uint32_t)0x00000000)  /* HCLK 不分频 */
#define RCC_PPRE1_DIV2 ((uint32_t)0x00000400)  /* HCLK 除以2 */
#define RCC_PPRE1_DIV4 ((uint32_t)0x00000500)  /* HCLK 除以4 */
#define RCC_PPRE1_DIV8 ((uint32_t)0x00000600)  /* HCLK 除以8 */
#define RCC_PPRE1_DIV16 ((uint32_t)0x00000700) /* HCLK 除以16 */

#define RCC_PPRE2 ((uint32_t)0x00003800)   /* PRE2[2:0] 位 (APB2 预分频器) */
#define RCC_PPRE2_0 ((uint32_t)0x00000800) /* 位0 */
#define RCC_PPRE2_1 ((uint32_t)0x00001000) /* 位1 */
#define RCC_PPRE2_2 ((uint32_t)0x00002000) /* 位2 */

#define RCC_PPRE2_DIV1 ((uint32_t)0x00000000)  /* HCLK 不分频 */
#define RCC_PPRE2_DIV2 ((uint32_t)0x00002000)  /* HCLK 除以2 */
#define RCC_PPRE2_DIV4 ((uint32_t)0x00002800)  /* HCLK 除以4 */
#define RCC_PPRE2_DIV8 ((uint32_t)0x00003000)  /* HCLK 除以8 */
#define RCC_PPRE2_DIV16 ((uint32_t)0x00003800) /* HCLK 除以16 */

#define RCC_ADCPRE ((uint32_t)0x0000C000)   /* ADCPRE[1:0] 位 (ADC 预分频器) */
#define RCC_ADCPRE_0 ((uint32_t)0x00004000) /* 位0 */
#define RCC_ADCPRE_1 ((uint32_t)0x00008000) /* 位1 */

#define RCC_ADCPRE_DIV2 ((uint32_t)0x00000000) /* PCLK2 除以2 */
#define RCC_ADCPRE_DIV4 ((uint32_t)0x00004000) /* PCLK2 除以4 */
#define RCC_ADCPRE_DIV6 ((uint32_t)0x00008000) /* PCLK2 除以6 */
#define RCC_ADCPRE_DIV8 ((uint32_t)0x0000C000) /* PCLK2 除以8 */

#define RCC_PLLSRC ((uint32_t)0x00010000) /* PLL 输入时钟源 */

#define RCC_PLLXTPRE ((uint32_t)0x00020000) /* HSE 分频器用于 PLL 输入 */

#define RCC_PLLMULL ((uint32_t)0x003C0000)   /* PLLMUL[3:0] 位 (PLL 倍频因子) */
#define RCC_PLLMULL_0 ((uint32_t)0x00040000) /* 位0 */
#define RCC_PLLMULL_1 ((uint32_t)0x00080000) /* 位1 */
#define RCC_PLLMULL_2 ((uint32_t)0x00100000) /* 位2 */
#define RCC_PLLMULL_3 ((uint32_t)0x00200000) /* 位3 */

#define RCC_PLLSRC_HSI_Div2 ((uint32_t)0x00000000) /* HSI 时钟除以2 作为 PLL 输入 */
#define RCC_PLLSRC_HSE ((uint32_t)0x00010000)      /* HSE 时钟作为 PLL 输入 */

#define RCC_PLLXTPRE_HSE ((uint32_t)0x00000000)      /* HSE 时钟不分频作为 PLL 输入 */
#define RCC_PLLXTPRE_HSE_Div2 ((uint32_t)0x00020000) /* HSE 时钟除以2 作为 PLL 输入 */

/* CH32V303x */
#define RCC_PLLMULL2 ((uint32_t)0x00000000)  /* PLL 输入时钟 *2 */
#define RCC_PLLMULL3 ((uint32_t)0x00040000)  /* PLL 输入时钟 *3 */
#define RCC_PLLMULL4 ((uint32_t)0x00080000)  /* PLL 输入时钟 *4 */
#define RCC_PLLMULL5 ((uint32_t)0x000C0000)  /* PLL 输入时钟 *5 */
#define RCC_PLLMULL6 ((uint32_t)0x00100000)  /* PLL 输入时钟 *6 */
#define RCC_PLLMULL7 ((uint32_t)0x00140000)  /* PLL 输入时钟 *7 */
#define RCC_PLLMULL8 ((uint32_t)0x00180000)  /* PLL 输入时钟 *8 */
#define RCC_PLLMULL9 ((uint32_t)0x001C0000)  /* PLL 输入时钟 *9 */
#define RCC_PLLMULL10 ((uint32_t)0x00200000) /* PLL 输入时钟 *10 */
#define RCC_PLLMULL11 ((uint32_t)0x00240000) /* PLL 输入时钟 *11 */
#define RCC_PLLMULL12 ((uint32_t)0x00280000) /* PLL 输入时钟 *12 */
#define RCC_PLLMULL13 ((uint32_t)0x002C0000) /* PLL 输入时钟 *13 */
#define RCC_PLLMULL14 ((uint32_t)0x00300000) /* PLL 输入时钟 *14 */
#define RCC_PLLMULL15 ((uint32_t)0x00340000) /* PLL 输入时钟 *15 */
#define RCC_PLLMULL16 ((uint32_t)0x00380000) /* PLL 输入时钟 *16 */
#define RCC_PLLMULL18 ((uint32_t)0x003C0000) /* PLL 输入时钟 *18 */

/* CH32V307x-CH32V305x */
#define RCC_PLLMULL18_EXTEN ((uint32_t)0x00000000)  /* PLL 输入时钟 *18 */
#define RCC_PLLMULL3_EXTEN ((uint32_t)0x00040000)   /* PLL 输入时钟 *3 */
#define RCC_PLLMULL4_EXTEN ((uint32_t)0x00080000)   /* PLL 输入时钟 *4 */
#define RCC_PLLMULL5_EXTEN ((uint32_t)0x000C0000)   /* PLL 输入时钟 *5 */
#define RCC_PLLMULL6_EXTEN ((uint32_t)0x00100000)   /* PLL 输入时钟 *6 */
#define RCC_PLLMULL7_EXTEN ((uint32_t)0x00140000)   /* PLL 输入时钟 *7 */
#define RCC_PLLMULL8_EXTEN ((uint32_t)0x00180000)   /* PLL 输入时钟 *8 */
#define RCC_PLLMULL9_EXTEN ((uint32_t)0x001C0000)   /* PLL 输入时钟 *9 */
#define RCC_PLLMULL10_EXTEN ((uint32_t)0x00200000)  /* PLL 输入时钟 *10 */
#define RCC_PLLMULL11_EXTEN ((uint32_t)0x00240000)  /* PLL 输入时钟 *11 */
#define RCC_PLLMULL12_EXTEN ((uint32_t)0x00280000)  /* PLL 输入时钟 *12 */
#define RCC_PLLMULL13_EXTEN ((uint32_t)0x002C0000)  /* PLL 输入时钟 *13 */
#define RCC_PLLMULL14_EXTEN ((uint32_t)0x00300000)  /* PLL 输入时钟 *14 */
#define RCC_PLLMULL6_5_EXTEN ((uint32_t)0x00340000) /* PLL 输入时钟 *6.5 */
#define RCC_PLLMULL15_EXTEN ((uint32_t)0x00380000)  /* PLL 输入时钟 *15 */
#define RCC_PLLMULL16_EXTEN ((uint32_t)0x003C0000)  /* PLL 输入时钟 *16 */

#define RCC_USBPRE ((uint32_t)0x00400000) /* USB 设备预分频器 */

#define RCC_CFGR0_MCO ((uint32_t)0x0F000000) /* MCO[3:0] 位 (微控制器时钟输出) */
#define RCC_MCO_0 ((uint32_t)0x01000000)     /* 位0 */
#define RCC_MCO_1 ((uint32_t)0x02000000)     /* 位1 */
#define RCC_MCO_2 ((uint32_t)0x04000000)     /* 位2 */
#define RCC_MCO_3 ((uint32_t)0x08000000)     /* 位3 */

#define RCC_MCO_NOCLOCK ((uint32_t)0x00000000)      /* 无时钟 */
#define RCC_CFGR0_MCO_SYSCLK ((uint32_t)0x04000000) /* 系统时钟作为 MCO 源 */
#define RCC_CFGR0_MCO_HSI ((uint32_t)0x05000000)    /* HSI 时钟作为 MCO 源 */
#define RCC_CFGR0_MCO_HSE ((uint32_t)0x06000000)    /* HSE 时钟作为 MCO 源 */
#define RCC_CFGR0_MCO_PLL ((uint32_t)0x07000000)    /* PLL 时钟除以2 作为 MCO 源 */
#define RCC_CFGR0_MCO_PLL2 ((uint32_t)0x08000000)   /* PLL2 作为 MCO 源 */
#define RCC_CFGR0_MCO_PLL3D2 ((uint32_t)0x09000000) /* PLL3 时钟除以2 作为 MCO 源 */
#define RCC_CFGR0_MCO_XT1 ((uint32_t)0x0A000000)    /* XT1 作为 MCO 源 */
#define RCC_CFGR0_MCO_PLL3 ((uint32_t)0x0B000000)   /* PLL3 作为 MCO 源 */

#define RCC_CFGR0_ETHPRE ((uint32_t)0x10000000) /* 以太网预分频器 */
#define RCC_CFGR0_ADCDUTY ((uint32_t)0x80000000) /* ADC 占空比 */
#define RCC_CFGR0_ADCDUTY_SEL ((uint32_t)0x40000000) /* ADC 占空比选择 */

/*******************  RCC_INTR 寄存器的位定义  ********************/
#define RCC_LSIRDYF ((uint32_t)0x00000001) /* LSI 就绪中断标志 */
#define RCC_LSERDYF ((uint32_t)0x00000002) /* LSE 就绪中断标志 */
#define RCC_HSIRDYF ((uint32_t)0x00000004) /* HSI 就绪中断标志 */
#define RCC_HSERDYF ((uint32_t)0x00000008) /* HSE 就绪中断标志 */
#define RCC_PLLRDYF ((uint32_t)0x00000010) /* PLL 就绪中断标志 */
#define RCC_PLL2RDYF ((uint32_t)0x00000020) /* PLL2 就绪中断标志 */
#define RCC_PLL3RDYF ((uint32_t)0x00000040) /* PLL3 就绪中断标志 */
#define RCC_CSSF ((uint32_t)0x00000080)     /* 时钟安全系统中断标志 */
#define RCC_LSIRDYIE ((uint32_t)0x00000100) /* LSI 就绪中断使能 */
#define RCC_LSERDYIE ((uint32_t)0x00000200) /* LSE 就绪中断使能 */
#define RCC_HSIRDYIE ((uint32_t)0x00000400) /* HSI 就绪中断使能 */
#define RCC_HSERDYIE ((uint32_t)0x00000800) /* HSE 就绪中断使能 */
#define RCC_PLLRDYIE ((uint32_t)0x00001000) /* PLL 就绪中断使能 */
#define RCC_PLL2RDYIE ((uint32_t)0x00002000) /* PLL2 就绪中断使能 */
#define RCC_PLL3RDYIE ((uint32_t)0x00004000) /* PLL3 就绪中断使能 */
#define RCC_LSIRDYC ((uint32_t)0x00010000) /* LSI 就绪中断清除 */
#define RCC_LSERDYC ((uint32_t)0x00020000) /* LSE 就绪中断清除 */
#define RCC_HSIRDYC ((uint32_t)0x00040000) /* HSI 就绪中断清除 */
#define RCC_HSERDYC ((uint32_t)0x00080000) /* HSE 就绪中断清除 */
#define RCC_PLLRDYC ((uint32_t)0x00100000) /* PLL 就绪中断清除 */
#define RCC_PLL2RDYC ((uint32_t)0x00200000) /* PLL2 就绪中断清除 */
#define RCC_PLL3RDYC ((uint32_t)0x00400000) /* PLL3 就绪中断清除 */
#define RCC_CSSC ((uint32_t)0x00800000) /* 时钟安全系统中断清除 */

/*****************  RCC_APB2PRSTR 寄存器的位定义  *****************/
#define RCC_AFIORST ((uint32_t)0x00000001) /* 复用功能 I/O 复位 */
#define RCC_IOPARST ((uint32_t)0x00000004) /* I/O 端口 A 复位 */
#define RCC_IOPBRST ((uint32_t)0x00000008) /* I/O 端口 B 复位 */
#define RCC_IOPCRST ((uint32_t)0x00000010) /* I/O 端口 C 复位 */
#define RCC_IOPDRST ((uint32_t)0x00000020) /* I/O 端口 D 复位 */
#define RCC_IOPERST ((uint32_t)0x00000040) /* I/O 端口 E 复位 */
#define RCC_ADC1RST ((uint32_t)0x00000200) /* ADC1 接口复位 */
#define RCC_ADC2RST ((uint32_t)0x00000400) /* ADC2 接口复位 */
#define RCC_TIM1RST ((uint32_t)0x00000800) /* TIM1 定时器复位 */
#define RCC_SPI1RST ((uint32_t)0x00001000) /* SPI1 复位 */
#define RCC_TIM8RST ((uint32_t)0x00002000) /* TIM8 复位 */
#define RCC_USART1RST ((uint32_t)0x00004000) /* USART1 复位 */
#define RCC_TIM9RST ((uint32_t)0x00080000) /* TIM9 复位 */
#define RCC_TIM10RST ((uint32_t)0x00100000) /* TIM10 复位 */

/*****************  RCC_APB1PRSTR 寄存器的位定义  *****************/
#define RCC_TIM2RST ((uint32_t)0x00000001)   /* TIM2 复位 */
#define RCC_TIM3RST ((uint32_t)0x00000002)   /* TIM3 复位 */
#define RCC_TIM4RST ((uint32_t)0x00000004)   /* TIM4 复位 */
#define RCC_TIM5RST ((uint32_t)0x00000008)   /* TIM5 复位 */
#define RCC_TIM6RST ((uint32_t)0x00000010)   /* TIM6 复位 */
#define RCC_TIM7RST ((uint32_t)0x00000020)   /* TIM7 复位 */
#define RCC_USART6RST ((uint32_t)0x00000040) /* USART6 复位 */
#define RCC_USART7RST ((uint32_t)0x00000080) /* USART7 复位 */
#define RCC_USART8RST ((uint32_t)0x00000100) /* USART8 复位 */
#define RCC_WWDGRST ((uint32_t)0x00000800)   /* 窗口看门狗复位 */
#define RCC_SPI2RST ((uint32_t)0x00004000)   /* SPI2 复位 */
#define RCC_SPI3RST ((uint32_t)0x00008000)   /* SPI3 复位 */
#define RCC_USART2RST ((uint32_t)0x00020000) /* USART2 复位 */
#define RCC_USART3RST ((uint32_t)0x00040000) /* USART3 复位 */
#define RCC_USART4RST ((uint32_t)0x00080000) /* USART4 复位 */
#define RCC_USART5RST ((uint32_t)0x00100000) /* USART5 复位 */
#define RCC_I2C1RST ((uint32_t)0x00200000)   /* I2C1 复位 */
#define RCC_I2C2RST ((uint32_t)0x00400000)   /* I2C2 复位 */
#define RCC_USBRST ((uint32_t)0x00800000)    /* USB 设备复位 */
#define RCC_CAN1RST ((uint32_t)0x02000000)   /* CAN1 复位 */
#define RCC_CAN2RST ((uint32_t)0x04000000)   /* CAN2 复位 */
#define RCC_BKPRST ((uint32_t)0x08000000)    /* 备份接口复位 */
#define RCC_PWRRST ((uint32_t)0x10000000)    /* 电源接口复位 */
#define RCC_DACRST ((uint32_t)0x20000000)    /* DAC 复位 */

/******************  RCC_AHBPCENR 寄存器的位定义  ******************/
#define RCC_DMA1EN ((uint16_t)0x0001) /* DMA1 时钟使能 */
#define RCC_DMA2EN ((uint16_t)0x0002) /* DMA2 时钟使能 */
#define RCC_SRAMEN ((uint16_t)0x0004) /* SRAM 接口时钟使能 */
#define RCC_CRCEN ((uint16_t)0x0040)  /* CRC 时钟使能 */
#define RCC_FSMCEN ((uint16_t)0x0100) /* FSMC 时钟使能 */
#define RCC_RNGEN ((uint16_t)0x0200)  /* RNG 时钟使能 */
#define RCC_SDIOEN ((uint16_t)0x0400) /* SDIO 时钟使能 */
#define RCC_USBHSEN ((uint16_t)0x0800) /* USBHS 时钟使能 */
#define RCC_OTGFSEN ((uint16_t)0x1000) /* OTGFS 时钟使能 */
#define RCC_DVPEN ((uint16_t)0x2000)   /* DVP 时钟使能 */
#define RCC_ETHMACEN ((uint16_t)0x4000) /* 以太网 MAC 时钟使能 */
#define RCC_ETHMACTXEN ((uint16_t)0x8000) /* 以太网 MAC 发送时钟使能 */
#define RCC_ETHMACRXEN ((uint32_t)0x10000) /* 以太网 MAC 接收时钟使能 */
#define RCC_BLEC ((uint32_t)0x10000) /* BLE 控制器 */
#define RCC_BLES ((uint32_t)0x20000) /* BLE 安全 */

/******************  RCC_APB2PCENR 寄存器的位定义  *****************/
#define RCC_AFIOEN ((uint32_t)0x00000001) /* 复用功能 I/O 时钟使能 */
#define RCC_IOPAEN ((uint32_t)0x00000004) /* I/O 端口 A 时钟使能 */
#define RCC_IOPBEN ((uint32_t)0x00000008) /* I/O 端口 B 时钟使能 */
#define RCC_IOPCEN ((uint32_t)0x00000010) /* I/O 端口 C 时钟使能 */
#define RCC_IOPDEN ((uint32_t)0x00000020) /* I/O 端口 D 时钟使能 */
#define RCC_IOPEEN ((uint32_t)0x00000040) /* I/O 端口 E 时钟使能 */
#define RCC_ADC1EN ((uint32_t)0x00000200) /* ADC1 接口时钟使能 */
#define RCC_ADC2EN ((uint32_t)0x00000400) /* ADC2 接口时钟使能 */
#define RCC_TIM1EN ((uint32_t)0x00000800) /* TIM1 时钟使能 */
#define RCC_SPI1EN ((uint32_t)0x00001000) /* SPI1 时钟使能 */
#define RCC_TIM8EN ((uint32_t)0x00002000) /* TIM8 时钟使能 */
#define RCC_USART1EN ((uint32_t)0x00004000) /* USART1 时钟使能 */
#define RCC_TIM9EN ((uint32_t)0x00080000) /* TIM9 时钟使能 */
#define RCC_TIM10EN ((uint32_t)0x00100000) /* TIM10 时钟使能 */

/*****************  RCC_APB1PCENR 寄存器的位定义  ******************/
#define RCC_TIM2EN ((uint32_t)0x00000001) /* TIM2 时钟使能 */
#define RCC_TIM3EN ((uint32_t)0x00000002) /* TIM3 时钟使能 */
#define RCC_TIM4EN ((uint32_t)0x00000004) /* TIM4 时钟使能 */
#define RCC_TIM5EN ((uint32_t)0x00000008) /* TIM5 时钟使能 */
#define RCC_TIM6EN ((uint32_t)0x00000010) /* TIM6 时钟使能 */
#define RCC_TIM7EN ((uint32_t)0x00000020) /* TIM7 时钟使能 */
#define RCC_USART6EN ((uint32_t)0x00000040) /* USART6 时钟使能 */
#define RCC_USART7EN ((uint32_t)0x00000080) /* USART7 时钟使能 */
#define RCC_USART8EN ((uint32_t)0x00000100) /* USART8 时钟使能 */
#define RCC_WWDGEN ((uint32_t)0x00000800) /* 窗口看门狗时钟使能 */
#define RCC_SPI2EN ((uint32_t)0x00004000) /* SPI2 时钟使能 */
#define RCC_SPI3EN ((uint32_t)0x00008000) /* SPI3 时钟使能 */
#define RCC_USART2EN ((uint32_t)0x00020000) /* USART2 时钟使能 */
#define RCC_USART3EN ((uint32_t)0x00040000) /* USART3 时钟使能 */
#define RCC_USART4EN ((uint32_t)0x00080000) /* USART4 时钟使能 */
#define RCC_USART5EN ((uint32_t)0x00100000) /* USART5 时钟使能 */
#define RCC_I2C1EN ((uint32_t)0x00200000) /* I2C1 时钟使能 */
#define RCC_I2C2EN ((uint32_t)0x00400000) /* I2C2 时钟使能 */
#define RCC_USBEN ((uint32_t)0x00800000) /* USB 设备时钟使能 */
#define RCC_CAN1EN ((uint32_t)0x02000000) /* CAN1 时钟使能 */
#define RCC_CAN2EN ((uint32_t)0x04000000) /* CAN2 时钟使能 */
#define RCC_BKPEN ((uint32_t)0x08000000) /* 备份接口时钟使能 */
#define RCC_PWREN ((uint32_t)0x10000000) /* 电源接口时钟使能 */
#define RCC_DACEN ((uint32_t)0x20000000) /* DAC 时钟使能 */

/*******************  RCC_BDCTLR 寄存器的位定义  *******************/
#define RCC_LSEON ((uint32_t)0x00000001)  /* 外部低速振荡器使能 */
#define RCC_LSERDY ((uint32_t)0x00000002) /* 外部低速振荡器就绪 */
#define RCC_LSEBYP ((uint32_t)0x00000004) /* 外部低速振荡器旁路 */

#define RCC_RTCSEL ((uint32_t)0x00000300)   /* RTCSEL[1:0] 位 (RTC 时钟源选择) */
#define RCC_RTCSEL_0 ((uint32_t)0x00000100) /* 位0 */
#define RCC_RTCSEL_1 ((uint32_t)0x00000200) /* 位1 */

#define RCC_RTCSEL_NOCLOCK ((uint32_t)0x00000000) /* 无时钟 */
#define RCC_RTCSEL_LSE ((uint32_t)0x00000100)     /* LSE 作为 RTC 时钟 */
#define RCC_RTCSEL_LSI ((uint32_t)0x00000200)     /* LSI 作为 RTC 时钟 */
#define RCC_RTCSEL_HSE ((uint32_t)0x00000300)     /* HSE 作为 RTC 时钟 (需分频) */

#define RCC_RTCEN ((uint32_t)0x00008000) /* RTC 时钟使能 */
#define RCC_BDRST ((uint32_t)0x00010000) /* 备份域软件复位 */

/*******************  RCC_RSTSCKR 寄存器的位定义  ********************/
#define RCC_LSION ((uint32_t)0x00000001)    /* 内部低速振荡器使能 */
#define RCC_LSIRDY ((uint32_t)0x00000002)   /* 内部低速振荡器就绪 */
#define RCC_RMVF ((uint32_t)0x01000000)     /* 移除复位标志 */
#define RCC_PINRSTF ((uint32_t)0x04000000)  /* PIN 复位标志 */
#define RCC_PORRSTF ((uint32_t)0x08000000)  /* POR/PDR 复位标志 */
#define RCC_SFTRSTF ((uint32_t)0x10000000)  /* 软件复位标志 */
#define RCC_IWDGRSTF ((uint32_t)0x20000000) /* 独立看门狗复位标志 */
#define RCC_WWDGRSTF ((uint32_t)0x40000000) /* 窗口看门狗复位标志 */
#define RCC_LPWRRSTF ((uint32_t)0x80000000) /* 低功耗复位标志 */

/*******************  RCC_AHBRSTR 寄存器的位定义  ********************/
#define RCC_OTGFSRST ((uint32_t)0x00001000) /* OTGFS 复位 */
#define RCC_DVPRST ((uint32_t)0x00002000)   /* DVP 复位 */
#define RCC_ETHMACRST ((uint32_t)0x00004000) /* 以太网 MAC 复位 */

/*******************  RCC_CFGR2 寄存器的位定义  ********************/
#define RCC_PREDIV1 ((uint32_t)0x0000000F)   /* PREDIV1[3:0] 位 (分频系数) */
#define RCC_PREDIV1_0 ((uint32_t)0x00000001) /* 位0 */
#define RCC_PREDIV1_1 ((uint32_t)0x00000002) /* 位1 */
#define RCC_PREDIV1_2 ((uint32_t)0x00000004) /* 位2 */
#define RCC_PREDIV1_3 ((uint32_t)0x00000008) /* 位3 */

#define RCC_PREDIV2 ((uint32_t)0x000000F0)   /* PREDIV2[3:0] 位 (分频系数) */
#define RCC_PREDIV2_0 ((uint32_t)0x00000010) /* 位0 */
#define RCC_PREDIV2_1 ((uint32_t)0x00000020) /* 位1 */
#define RCC_PREDIV2_2 ((uint32_t)0x00000040) /* 位2 */
#define RCC_PREDIV2_3 ((uint32_t)0x00000080) /* 位3 */

#define RCC_PLL2MUL ((uint32_t)0x00000F00)   /* PLL2MUL[3:0] 位 (PLL2 倍频系数) */
#define RCC_PLL2MUL_0 ((uint32_t)0x00000100) /* 位0 */
#define RCC_PLL2MUL_1 ((uint32_t)0x00000200) /* 位1 */
#define RCC_PLL2MUL_2 ((uint32_t)0x00000400) /* 位2 */
#define RCC_PLL2MUL_3 ((uint32_t)0x00000800) /* 位3 */

#define RCC_PLL3MUL ((uint32_t)0x0000F000)   /* PLL3MUL[3:0] 位 (PLL3 倍频系数) */
#define RCC_PLL3MUL_0 ((uint32_t)0x00001000) /* 位0 */
#define RCC_PLL3MUL_1 ((uint32_t)0x00002000) /* 位1 */
#define RCC_PLL3MUL_2 ((uint32_t)0x00004000) /* 位2 */
#define RCC_PLL3MUL_3 ((uint32_t)0x00008000) /* 位3 */

#define RCC_PREDIV1SRC ((uint32_t)0x00010000) /* PREDIV1 输入源选择 */
#define RCC_I2S2SRC ((uint32_t)0x00020000)    /* I2S2 时钟源选择 */
#define RCC_I2S3SRC ((uint32_t)0x00040000)    /* I2S3 时钟源选择 */
#define RCC_RNGSRC ((uint32_t)0x00080000)     /* RNG 时钟源选择 */

#define RCC_ETH1GSRC ((uint32_t)0x00300000)   /* ETH1G 时钟源选择 */
#define RCC_ETH1GSRC_0 ((uint32_t)0x00100000) /* 位0 */
#define RCC_ETH1GSRC_1 ((uint32_t)0x00200000) /* 位1 */

#define RCC_ETH1GEN ((uint32_t)0x00400000) /* ETH1G 使能 */

#define RCC_USBHSDIV ((uint32_t)0x07000000)   /* USBHS 分频系数 */
#define RCC_USBHSDIV_0 ((uint32_t)0x01000000) /* 位0 */
#define RCC_USBHSDIV_1 ((uint32_t)0x02000000) /* 位1 */
#define RCC_USBHSDIV_2 ((uint32_t)0x04000000) /* 位2 */

#define RCC_USBHSPLLSRC ((uint32_t)0x08000000) /* USBHS PLL 源选择 */

#define RCC_USBHSCLK ((uint32_t)0x30000000)   /* USBHS 时钟选择 */
#define RCC_USBHSCLK_0 ((uint32_t)0x10000000) /* 位0 */
#define RCC_USBHSCLK_1 ((uint32_t)0x20000000) /* 位1 */

#define RCC_USBHSPLL ((uint32_t)0x40000000) /* USBHS PLL 使能 */
#define RCC_USBFSSRC ((uint32_t)0x80000000) /* USB FS 时钟源选择 */

/*******************  RCC_HSE_CAL_CTRL 寄存器的位定义  ********************/
#define RCC_HSEITRIM ((uint32_t)0x01000000) /* HSE 内部修调 */
#define RCC_HSEFAULT ((uint32_t)0x08000000) /* HSE 故障标志 */

#define RCC_HSEC ((uint32_t)0x70000000)   /* HSEC[2:0] 位 (HSE 校准控制) */
#define RCC_HSEC_0 ((uint32_t)0x10000000) /* 位0 */
#define RCC_HSEC_1 ((uint32_t)0x20000000) /* 位1 */
#define RCC_HSEC_2 ((uint32_t)0x40000000) /* 位2 */

/*******************  RCC_LSI32K_TUNE 寄存器的位定义  ********************/
#define RCC_HTUNE ((uint16_t)0x1000) /* 高修调 */
#define RCC_LTUNE ((uint16_t)0x0011) /* 低修调 */

/*******************  RCC_LSI32K_CAL_CFG 寄存器的位定义  ********************/
#define RCC_CNTVLU ((uint8_t)0x0F)   /* CNTVLU[3:0] 位 (计数值) */
#define RCC_CNTVLU_0 ((uint8_t)0x01) /* 位0 */
#define RCC_CNTVLU_1 ((uint8_t)0x02) /* 位1 */
#define RCC_CNTVLU_2 ((uint8_t)0x04) /* 位2 */
#define RCC_CNTVLU_3 ((uint8_t)0x08) /* 位3 */
#define RCC_HALTMD ((uint8_t)0x10)   /* 暂停模式 */
#define RCC_WKUPEN ((uint8_t)0x20)   /* 唤醒使能 */
#define RCC_LPEN ((uint8_t)0x40)     /* 低功耗使能 */

/*******************  RCC_LSI32K_CAL_STATR 寄存器的位定义  ********************/
#define RCC_CNTOV ((uint16_t)0x4000) /* 计数器溢出 */
#define RCC_IFEND ((uint16_t)0x8000) /* 校准结束标志 */

/*******************  RCC_LSI32K_CAL_CTRL 寄存器的位定义  ********************/
#define RCC_CALINTEN ((uint8_t)0x01) /* 校准中断使能 */
#define RCC_CALEN ((uint8_t)0x02)    /* 校准使能 */
#define RCC_HALT ((uint8_t)0x80)     /* 暂停 */

/******************************************************************************/
/*                                    RNG                                     */
/******************************************************************************/
/********************  RNG_CR 寄存器的位定义  *******************/
#define RNG_CR_RNGEN ((uint32_t)0x00000004) /* 随机数发生器使能 */
#define RNG_CR_IE ((uint32_t)0x00000008)    /* 中断使能 */

/********************  RNG_SR 寄存器的位定义  *******************/
#define RNG_SR_DRDY ((uint32_t)0x00000001) /* 数据就绪 */
#define RNG_SR_CECS ((uint32_t)0x00000002) /* 时钟错误当前状态 */
#define RNG_SR_SECS ((uint32_t)0x00000004) /* 种子错误当前状态 */
#define RNG_SR_CEIS ((uint32_t)0x00000020) /* 时钟错误中断状态 */
#define RNG_SR_SEIS ((uint32_t)0x00000040) /* 种子错误中断状态 */

/******************************************************************************/
/*                             实时时钟 (RTC)                                  */
/******************************************************************************/

/*******************  RTC_CTLRH 寄存器的位定义  ********************/
#define RTC_CTLRH_SECIE ((uint8_t)0x01) /* 秒中断使能 */
#define RTC_CTLRH_ALRIE ((uint8_t)0x02) /* 闹钟中断使能 */
#define RTC_CTLRH_OWIE ((uint8_t)0x04)  /* 溢出中断使能 */

/*******************  RTC_CTLRL 寄存器的位定义  ********************/
#define RTC_CTLRL_SECF ((uint8_t)0x01)  /* 秒标志 */
#define RTC_CTLRL_ALRF ((uint8_t)0x02)  /* 闹钟标志 */
#define RTC_CTLRL_OWF ((uint8_t)0x04)   /* 溢出标志 */
#define RTC_CTLRL_RSF ((uint8_t)0x08)   /* 寄存器同步标志 */
#define RTC_CTLRL_CNF ((uint8_t)0x10)   /* 配置标志 */
#define RTC_CTLRL_RTOFF ((uint8_t)0x20) /* RTC 操作关闭 */

/*******************  RTC_PSCH 寄存器的位定义  *******************/
#define RTC_PSCH_PRL ((uint16_t)0x000F) /* RTC 预分频器重装载值高4位 */

/*******************  RTC_PRLL 寄存器的位定义  *******************/
#define RTC_PSCL_PRL ((uint16_t)0xFFFF) /* RTC 预分频器重装载值低16位 */

/*******************  RTC_DIVH 寄存器的位定义  *******************/
#define RTC_DIVH_RTC_DIV ((uint16_t)0x000F) /* RTC 时钟分频器高4位 */

/*******************  RTC_DIVL 寄存器的位定义  *******************/
#define RTC_DIVL_RTC_DIV ((uint16_t)0xFFFF) /* RTC 时钟分频器低16位 */

/*******************  RTC_CNTH 寄存器的位定义  *******************/
#define RTC_CNTH_RTC_CNT ((uint16_t)0xFFFF) /* RTC 计数器高16位 */

/*******************  RTC_CNTL 寄存器的位定义  *******************/
#define RTC_CNTL_RTC_CNT ((uint16_t)0xFFFF) /* RTC 计数器低16位 */

/*******************  RTC_ALRMH 寄存器的位定义  *******************/
#define RTC_ALRMH_RTC_ALRM ((uint16_t)0xFFFF) /* RTC 闹钟高16位 */

/*******************  RTC_ALRML 寄存器的位定义  *******************/
#define RTC_ALRML_RTC_ALRM ((uint16_t)0xFFFF) /* RTC 闹钟低16位 */

/******************************************************************************/
/*                        串行外设接口 (SPI)                                   */
/******************************************************************************/

/*******************  SPI_CTLR1 寄存器的位定义  ********************/
#define SPI_CTLR1_CPHA ((uint16_t)0x0001) /* 时钟相位 */
#define SPI_CTLR1_CPOL ((uint16_t)0x0002) /* 时钟极性 */
#define SPI_CTLR1_MSTR ((uint16_t)0x0004) /* 主模式选择 */

#define SPI_CTLR1_BR ((uint16_t)0x0038)   /* BR[2:0] 位 (波特率控制) */
#define SPI_CTLR1_BR_0 ((uint16_t)0x0008) /* 位0 */
#define SPI_CTLR1_BR_1 ((uint16_t)0x0010) /* 位1 */
#define SPI_CTLR1_BR_2 ((uint16_t)0x0020) /* 位2 */

#define SPI_CTLR1_SPE ((uint16_t)0x0040)      /* SPI 使能 */
#define SPI_CTLR1_LSBFIRST ((uint16_t)0x0080) /* 帧格式 */
#define SPI_CTLR1_SSI ((uint16_t)0x0100)      /* 内部从选择 */
#define SPI_CTLR1_SSM ((uint16_t)0x0200)      /* 软件从管理 */
#define SPI_CTLR1_RXONLY ((uint16_t)0x0400)   /* 只接收 */
#define SPI_CTLR1_DFF ((uint16_t)0x0800)      /* 数据帧格式 */
#define SPI_CTLR1_CRCNEXT ((uint16_t)0x1000)  /* 下一个发送 CRC */
#define SPI_CTLR1_CRCEN ((uint16_t)0x2000)    /* 硬件 CRC 计算使能 */
#define SPI_CTLR1_BIDIOE ((uint16_t)0x4000)   /* 双向模式下输出使能 */
#define SPI_CTLR1_BIDIMODE ((uint16_t)0x8000) /* 双向数据模式使能 */

/*******************  SPI_CTLR2 寄存器的位定义  ********************/
#define SPI_CTLR2_RXDMAEN ((uint8_t)0x01) /* Rx 缓冲区 DMA 使能 */
#define SPI_CTLR2_TXDMAEN ((uint8_t)0x02) /* Tx 缓冲区 DMA 使能 */
#define SPI_CTLR2_SSOE ((uint8_t)0x04)    /* SS 输出使能 */
#define SPI_CTLR2_ERRIE ((uint8_t)0x20)   /* 错误中断使能 */
#define SPI_CTLR2_RXNEIE ((uint8_t)0x40)  /* RX 缓冲区非空中断使能 */
#define SPI_CTLR2_TXEIE ((uint8_t)0x80)   /* TX 缓冲区空中断使能 */

/********************  SPI_STATR 寄存器的位定义  ********************/
#define SPI_STATR_RXNE ((uint8_t)0x01)   /* 接收缓冲区非空 */
#define SPI_STATR_TXE ((uint8_t)0x02)    /* 发送缓冲区空 */
#define SPI_STATR_CHSIDE ((uint8_t)0x04) /* 通道侧 */
#define SPI_STATR_UDR ((uint8_t)0x08)    /* 下溢标志 */
#define SPI_STATR_CRCERR ((uint8_t)0x10) /* CRC 错误标志 */
#define SPI_STATR_MODF ((uint8_t)0x20)   /* 模式错误 */
#define SPI_STATR_OVR ((uint8_t)0x40)    /* 溢出标志 */
#define SPI_STATR_BSY ((uint8_t)0x80)    /* 忙标志 */

/********************  SPI_DATAR 寄存器的位定义  ********************/
#define SPI_DATAR_DR ((uint16_t)0xFFFF) /* 数据寄存器 */

/*******************  SPI_CRCR 寄存器的位定义  ******************/
#define SPI_CRCR_CRCPOLY ((uint16_t)0xFFFF) /* CRC 多项式寄存器 */

/******************  SPI_RCRCR 寄存器的位定义  ******************/
#define SPI_RCRCR_RXCRC ((uint16_t)0xFFFF) /* Rx CRC 寄存器 */

/******************  SPI_TCRCR 寄存器的位定义  ******************/
#define SPI_TCRCR_TXCRC ((uint16_t)0xFFFF) /* Tx CRC 寄存器 */

/******************  SPI_I2SCFGR 寄存器的位定义  *****************/
#define SPI_I2SCFGR_CHLEN ((uint16_t)0x0001) /* 通道长度 (每音频通道位数) */

#define SPI_I2SCFGR_DATLEN ((uint16_t)0x0006)   /* DATLEN[1:0] 位 (数据传输长度) */
#define SPI_I2SCFGR_DATLEN_0 ((uint16_t)0x0002) /* 位0 */
#define SPI_I2SCFGR_DATLEN_1 ((uint16_t)0x0004) /* 位1 */

#define SPI_I2SCFGR_CKPOL ((uint16_t)0x0008) /* 稳态时钟极性 */

#define SPI_I2SCFGR_I2SSTD ((uint16_t)0x0030)   /* I2SSTD[1:0] 位 (I2S 标准选择) */
#define SPI_I2SCFGR_I2SSTD_0 ((uint16_t)0x0010) /* 位0 */
#define SPI_I2SCFGR_I2SSTD_1 ((uint16_t)0x0020) /* 位1 */

#define SPI_I2SCFGR_PCMSYNC ((uint16_t)0x0080) /* PCM 帧同步 */

#define SPI_I2SCFGR_I2SCFG ((uint16_t)0x0300)   /* I2SCFG[1:0] 位 (I2S 配置模式) */
#define SPI_I2SCFGR_I2SCFG_0 ((uint16_t)0x0100) /* 位0 */
#define SPI_I2SCFGR_I2SCFG_1 ((uint16_t)0x0200) /* 位1 */

#define SPI_I2SCFGR_I2SE ((uint16_t)0x0400)   /* I2S 使能 */
#define SPI_I2SCFGR_I2SMOD ((uint16_t)0x0800) /* I2S 模式选择 */

/******************  SPI_I2SPR 寄存器的位定义  *******************/
#define SPI_I2SPR_I2SDIV ((uint16_t)0x00FF) /* I2S 线性预分频器 */
#define SPI_I2SPR_ODD ((uint16_t)0x0100)    /* 预分频器奇因子 */
#define SPI_I2SPR_MCKOE ((uint16_t)0x0200)  /* 主时钟输出使能 */

/******************  SPI_HSCR 寄存器的位定义  *******************/
#define SPI_HSCR_HSRXEN ((uint16_t)0x0001) /* 高速接收使能 */
#define SPI_HSCR_HSRXEN2 ((uint16_t)0x0004) /* 高速接收使能2 */

/******************************************************************************/
/*                                    TIM                                     */
/******************************************************************************/

/*******************  TIM_CTLR1 寄存器的位定义  ********************/
#define TIM_CEN ((uint16_t)0x0001)  /* 计数器使能 */
#define TIM_UDIS ((uint16_t)0x0002) /* 更新禁止 */
#define TIM_URS ((uint16_t)0x0004)  /* 更新请求源 */
#define TIM_OPM ((uint16_t)0x0008)  /* 单脉冲模式 */
#define TIM_DIR ((uint16_t)0x0010)  /* 方向 */

#define TIM_CMS ((uint16_t)0x0060)   /* CMS[1:0] 位 (中心对齐模式选择) */
#define TIM_CMS_0 ((uint16_t)0x0020) /* 位0 */
#define TIM_CMS_1 ((uint16_t)0x0040) /* 位1 */

#define TIM_ARPE ((uint16_t)0x0080) /* 自动重装载预装载使能 */

#define TIM_CTLR1_CKD ((uint16_t)0x0300) /* CKD[1:0] 位 (时钟分频) */
#define TIM_CKD_0 ((uint16_t)0x0100)     /* 位0 */
#define TIM_CKD_1 ((uint16_t)0x0200)     /* 位1 */

/*******************  TIM_CTLR2 寄存器的位定义  ********************/
#define TIM_CCPC ((uint16_t)0x0001) /* 捕获/比较预装载控制 */
#define TIM_CCUS ((uint16_t)0x0004) /* 捕获/比较控制更新选择 */
#define TIM_CCDS ((uint16_t)0x0008) /* 捕获/比较 DMA 选择 */

#define TIM_MMS ((uint16_t)0x0070)   /* MMS[2:0] 位 (主模式选择) */
#define TIM_MMS_0 ((uint16_t)0x0010) /* 位0 */
#define TIM_MMS_1 ((uint16_t)0x0020) /* 位1 */
#define TIM_MMS_2 ((uint16_t)0x0040) /* 位2 */

#define TIM_TI1S ((uint16_t)0x0080)  /* TI1 选择 */
#define TIM_OIS1 ((uint16_t)0x0100)  /* 输出空闲状态1 (OC1 输出) */
#define TIM_OIS1N ((uint16_t)0x0200) /* 输出空闲状态1 (OC1N 输出) */
#define TIM_OIS2 ((uint16_t)0x0400)  /* 输出空闲状态2 (OC2 输出) */
#define TIM_OIS2N ((uint16_t)0x0800) /* 输出空闲状态2 (OC2N 输出) */
#define TIM_OIS3 ((uint16_t)0x1000)  /* 输出空闲状态3 (OC3 输出) */
#define TIM_OIS3N ((uint16_t)0x2000) /* 输出空闲状态3 (OC3N 输出) */
#define TIM_OIS4 ((uint16_t)0x4000)  /* 输出空闲状态4 (OC4 输出) */

/*******************  TIM_SMCFGR 寄存器的位定义  *******************/
#define TIM_SMS ((uint16_t)0x0007)   /* SMS[2:0] 位 (从模式选择) */
#define TIM_SMS_0 ((uint16_t)0x0001) /* 位0 */
#define TIM_SMS_1 ((uint16_t)0x0002) /* 位1 */
#define TIM_SMS_2 ((uint16_t)0x0004) /* 位2 */

#define TIM_TS ((uint16_t)0x0070)   /* TS[2:0] 位 (触发选择) */
#define TIM_TS_0 ((uint16_t)0x0010) /* 位0 */
#define TIM_TS_1 ((uint16_t)0x0020) /* 位1 */
#define TIM_TS_2 ((uint16_t)0x0040) /* 位2 */

#define TIM_MSM ((uint16_t)0x0080) /* 主/从模式 */

#define TIM_ETF ((uint16_t)0x0F00)   /* ETF[3:0] 位 (外部触发滤波) */
#define TIM_ETF_0 ((uint16_t)0x0100) /* 位0 */
#define TIM_ETF_1 ((uint16_t)0x0200) /* 位1 */
#define TIM_ETF_2 ((uint16_t)0x0400) /* 位2 */
#define TIM_ETF_3 ((uint16_t)0x0800) /* 位3 */

#define TIM_ETPS ((uint16_t)0x3000)   /* ETPS[1:0] 位 (外部触发预分频器) */
#define TIM_ETPS_0 ((uint16_t)0x1000) /* 位0 */
#define TIM_ETPS_1 ((uint16_t)0x2000) /* 位1 */

#define TIM_ECE ((uint16_t)0x4000) /* 外部时钟使能 */
#define TIM_ETP ((uint16_t)0x8000) /* 外部触发极性 */

/*******************  TIM_DMAINTENR 寄存器的位定义  *******************/
#define TIM_UIE ((uint16_t)0x0001)   /* 更新中断使能 */
#define TIM_CC1IE ((uint16_t)0x0002) /* 捕获/比较1 中断使能 */
#define TIM_CC2IE ((uint16_t)0x0004) /* 捕获/比较2 中断使能 */
#define TIM_CC3IE ((uint16_t)0x0008) /* 捕获/比较3 中断使能 */
#define TIM_CC4IE ((uint16_t)0x0010) /* 捕获/比较4 中断使能 */
#define TIM_COMIE ((uint16_t)0x0020) /* COM 中断使能 */
#define TIM_TIE ((uint16_t)0x0040)   /* 触发中断使能 */
#define TIM_BIE ((uint16_t)0x0080)   /* 刹车中断使能 */
#define TIM_UDE ((uint16_t)0x0100)   /* 更新 DMA 请求使能 */
#define TIM_CC1DE ((uint16_t)0x0200) /* 捕获/比较1 DMA 请求使能 */
#define TIM_CC2DE ((uint16_t)0x0400) /* 捕获/比较2 DMA 请求使能 */
#define TIM_CC3DE ((uint16_t)0x0800) /* 捕获/比较3 DMA 请求使能 */
#define TIM_CC4DE ((uint16_t)0x1000) /* 捕获/比较4 DMA 请求使能 */
#define TIM_COMDE ((uint16_t)0x2000) /* COM DMA 请求使能 */
#define TIM_TDE ((uint16_t)0x4000)   /* 触发 DMA 请求使能 */

/********************  TIM_INTFR 寄存器的位定义  ********************/
#define TIM_UIF ((uint16_t)0x0001)   /* 更新中断标志 */
#define TIM_CC1IF ((uint16_t)0x0002) /* 捕获/比较1 中断标志 */
#define TIM_CC2IF ((uint16_t)0x0004) /* 捕获/比较2 中断标志 */
#define TIM_CC3IF ((uint16_t)0x0008) /* 捕获/比较3 中断标志 */
#define TIM_CC4IF ((uint16_t)0x0010) /* 捕获/比较4 中断标志 */
#define TIM_COMIF ((uint16_t)0x0020) /* COM 中断标志 */
#define TIM_TIF ((uint16_t)0x0040)   /* 触发中断标志 */
#define TIM_BIF ((uint16_t)0x0080)   /* 刹车中断标志 */
#define TIM_CC1OF ((uint16_t)0x0200) /* 捕获/比较1 过捕获标志 */
#define TIM_CC2OF ((uint16_t)0x0400) /* 捕获/比较2 过捕获标志 */
#define TIM_CC3OF ((uint16_t)0x0800) /* 捕获/比较3 过捕获标志 */
#define TIM_CC4OF ((uint16_t)0x1000) /* 捕获/比较4 过捕获标志 */

/*******************  TIM_SWEVGR 寄存器的位定义  ********************/
#define TIM_UG ((uint8_t)0x01)   /* 更新生成 */
#define TIM_CC1G ((uint8_t)0x02) /* 捕获/比较1 生成 */
#define TIM_CC2G ((uint8_t)0x04) /* 捕获/比较2 生成 */
#define TIM_CC3G ((uint8_t)0x08) /* 捕获/比较3 生成 */
#define TIM_CC4G ((uint8_t)0x10) /* 捕获/比较4 生成 */
#define TIM_COMG ((uint8_t)0x20) /* 捕获/比较控制更新生成 */
#define TIM_TG ((uint8_t)0x40)   /* 触发生成 */
#define TIM_BG ((uint8_t)0x80)   /* 刹车生成 */

/******************  TIM_CHCTLR1 寄存器的位定义  *******************/
#define TIM_CC1S ((uint16_t)0x0003)   /* CC1S[1:0] 位 (捕获/比较1 选择) */
#define TIM_CC1S_0 ((uint16_t)0x0001) /* 位0 */
#define TIM_CC1S_1 ((uint16_t)0x0002) /* 位1 */

#define TIM_OC1FE ((uint16_t)0x0004) /* 输出比较1 快速使能 */
#define TIM_OC1PE ((uint16_t)0x0008) /* 输出比较1 预装载使能 */

#define TIM_OC1M ((uint16_t)0x0070)   /* OC1M[2:0] 位 (输出比较1 模式) */
#define TIM_OC1M_0 ((uint16_t)0x0010) /* 位0 */
#define TIM_OC1M_1 ((uint16_t)0x0020) /* 位1 */
#define TIM_OC1M_2 ((uint16_t)0x0040) /* 位2 */

#define TIM_OC1CE ((uint16_t)0x0080) /* 输出比较1 清零使能 */

#define TIM_CC2S ((uint16_t)0x0300)   /* CC2S[1:0] 位 (捕获/比较2 选择) */
#define TIM_CC2S_0 ((uint16_t)0x0100) /* 位0 */
#define TIM_CC2S_1 ((uint16_t)0x0200) /* 位1 */

#define TIM_OC2FE ((uint16_t)0x0400) /* 输出比较2 快速使能 */
#define TIM_OC2PE ((uint16_t)0x0800) /* 输出比较2 预装载使能 */

#define TIM_OC2M ((uint16_t)0x7000)   /* OC2M[2:0] 位 (输出比较2 模式) */
#define TIM_OC2M_0 ((uint16_t)0x1000) /* 位0 */
#define TIM_OC2M_1 ((uint16_t)0x2000) /* 位1 */
#define TIM_OC2M_2 ((uint16_t)0x4000) /* 位2 */

#define TIM_OC2CE ((uint16_t)0x8000) /* 输出比较2 清零使能 */

#define TIM_IC1PSC ((uint16_t)0x000C)   /* IC1PSC[1:0] 位 (输入捕获1 预分频器) */
#define TIM_IC1PSC_0 ((uint16_t)0x0004) /* 位0 */
#define TIM_IC1PSC_1 ((uint16_t)0x0008) /* 位1 */

#define TIM_IC1F ((uint16_t)0x00F0)   /* IC1F[3:0] 位 (输入捕获1 滤波器) */
#define TIM_IC1F_0 ((uint16_t)0x0010) /* 位0 */
#define TIM_IC1F_1 ((uint16_t)0x0020) /* 位1 */
#define TIM_IC1F_2 ((uint16_t)0x0040) /* 位2 */
#define TIM_IC1F_3 ((uint16_t)0x0080) /* 位3 */

#define TIM_IC2PSC ((uint16_t)0x0C00)   /* IC2PSC[1:0] 位 (输入捕获2 预分频器) */
#define TIM_IC2PSC_0 ((uint16_t)0x0400) /* 位0 */
#define TIM_IC2PSC_1 ((uint16_t)0x0800) /* 位1 */

#define TIM_IC2F ((uint16_t)0xF000)   /* IC2F[3:0] 位 (输入捕获2 滤波器) */
#define TIM_IC2F_0 ((uint16_t)0x1000) /* 位0 */
#define TIM_IC2F_1 ((uint16_t)0x2000) /* 位1 */
#define TIM_IC2F_2 ((uint16_t)0x4000) /* 位2 */
#define TIM_IC2F_3 ((uint16_t)0x8000) /* 位3 */

/******************  TIM_CHCTLR2 寄存器的位定义  *******************/
#define TIM_CC3S ((uint16_t)0x0003)   /* CC3S[1:0] 位 (捕获/比较3 选择) */
#define TIM_CC3S_0 ((uint16_t)0x0001) /* 位0 */
#define TIM_CC3S_1 ((uint16_t)0x0002) /* 位1 */

#define TIM_OC3FE ((uint16_t)0x0004) /* 输出比较3 快速使能 */
#define TIM_OC3PE ((uint16_t)0x0008) /* 输出比较3 预装载使能 */

#define TIM_OC3M ((uint16_t)0x0070)   /* OC3M[2:0] 位 (输出比较3 模式) */
#define TIM_OC3M_0 ((uint16_t)0x0010) /* 位0 */
#define TIM_OC3M_1 ((uint16_t)0x0020) /* 位1 */
#define TIM_OC3M_2 ((uint16_t)0x0040) /* 位2 */

#define TIM_OC3CE ((uint16_t)0x0080) /* 输出比较3 清零使能 */

#define TIM_CC4S ((uint16_t)0x0300)   /* CC4S[1:0] 位 (捕获/比较4 选择) */
#define TIM_CC4S_0 ((uint16_t)0x0100) /* 位0 */
#define TIM_CC4S_1 ((uint16_t)0x0200) /* 位1 */

#define TIM_OC4FE ((uint16_t)0x0400) /* 输出比较4 快速使能 */
#define TIM_OC4PE ((uint16_t)0x0800) /* 输出比较4 预装载使能 */

#define TIM_OC4M ((uint16_t)0x7000)   /* OC4M[2:0] 位 (输出比较4 模式) */
#define TIM_OC4M_0 ((uint16_t)0x1000) /* 位0 */
#define TIM_OC4M_1 ((uint16_t)0x2000) /* 位1 */
#define TIM_OC4M_2 ((uint16_t)0x4000) /* 位2 */

#define TIM_OC4CE ((uint16_t)0x8000) /* 输出比较4 清零使能 */

#define TIM_IC3PSC ((uint16_t)0x000C)   /* IC3PSC[1:0] 位 (输入捕获3 预分频器) */
#define TIM_IC3PSC_0 ((uint16_t)0x0004) /* 位0 */
#define TIM_IC3PSC_1 ((uint16_t)0x0008) /* 位1 */

#define TIM_IC3F ((uint16_t)0x00F0)   /* IC3F[3:0] 位 (输入捕获3 滤波器) */
#define TIM_IC3F_0 ((uint16_t)0x0010) /* 位0 */
#define TIM_IC3F_1 ((uint16_t)0x0020) /* 位1 */
#define TIM_IC3F_2 ((uint16_t)0x0040) /* 位2 */
#define TIM_IC3F_3 ((uint16_t)0x0080) /* 位3 */

#define TIM_IC4PSC ((uint16_t)0x0C00)   /* IC4PSC[1:0] 位 (输入捕获4 预分频器) */
#define TIM_IC4PSC_0 ((uint16_t)0x0400) /* 位0 */
#define TIM_IC4PSC_1 ((uint16_t)0x0800) /* 位1 */

#define TIM_IC4F ((uint16_t)0xF000)   /* IC4F[3:0] 位 (输入捕获4 滤波器) */
#define TIM_IC4F_0 ((uint16_t)0x1000) /* 位0 */
#define TIM_IC4F_1 ((uint16_t)0x2000) /* 位1 */
#define TIM_IC4F_2 ((uint16_t)0x4000) /* 位2 */
#define TIM_IC4F_3 ((uint16_t)0x8000) /* 位3 */

/*******************  TIM_CCER 寄存器的位定义  *******************/
#define TIM_CC1E ((uint16_t)0x0001)  /* 捕获/比较1 输出使能 */
#define TIM_CC1P ((uint16_t)0x0002)  /* 捕获/比较1 输出极性 */
#define TIM_CC1NE ((uint16_t)0x0004) /* 捕获/比较1 互补输出使能 */
#define TIM_CC1NP ((uint16_t)0x0008) /* 捕获/比较1 互补输出极性 */
#define TIM_CC2E ((uint16_t)0x0010)  /* 捕获/比较2 输出使能 */
#define TIM_CC2P ((uint16_t)0x0020)  /* 捕获/比较2 输出极性 */
#define TIM_CC2NE ((uint16_t)0x0040) /* 捕获/比较2 互补输出使能 */
#define TIM_CC2NP ((uint16_t)0x0080) /* 捕获/比较2 互补输出极性 */
#define TIM_CC3E ((uint16_t)0x0100)  /* 捕获/比较3 输出使能 */
#define TIM_CC3P ((uint16_t)0x0200)  /* 捕获/比较3 输出极性 */
#define TIM_CC3NE ((uint16_t)0x0400) /* 捕获/比较3 互补输出使能 */
#define TIM_CC3NP ((uint16_t)0x0800) /* 捕获/比较3 互补输出极性 */
#define TIM_CC4E ((uint16_t)0x1000)  /* 捕获/比较4 输出使能 */
#define TIM_CC4P ((uint16_t)0x2000)  /* 捕获/比较4 输出极性 */

/*******************  TIM_CNT 寄存器的位定义  ********************/
#define TIM_CNT ((uint16_t)0xFFFF) /* 计数器值 */

/*******************  TIM_PSC 寄存器的位定义  ********************/
#define TIM_PSC ((uint16_t)0xFFFF) /* 预分频器值 */

/*******************  TIM_ATRLR 寄存器的位定义  ********************/
#define TIM_ARR ((uint16_t)0xFFFF) /* 实际自动重装载值 */

/*******************  TIM_RPTCR 寄存器的位定义  ********************/
#define TIM_REP ((uint8_t)0xFF) /* 重复计数器值 */

/*******************  TIM_CH1CVR 寄存器的位定义  *******************/
#define TIM_CCR1 ((uint16_t)0xFFFF) /* 捕获/比较1 值 */

/*******************  TIM_CH2CVR 寄存器的位定义  *******************/
#define TIM_CCR2 ((uint16_t)0xFFFF) /* 捕获/比较2 值 */

/*******************  TIM_CH3CVR 寄存器的位定义  *******************/
#define TIM_CCR3 ((uint16_t)0xFFFF) /* 捕获/比较3 值 */

/*******************  TIM_CH4CVR 寄存器的位定义  *******************/
#define TIM_CCR4 ((uint16_t)0xFFFF) /* 捕获/比较4 值 */

/*******************  TIM_BDTR 寄存器的位定义  *******************/
#define TIM_DTG ((uint16_t)0x00FF)   /* DTG[0:7] 位 (死区时间发生器设置) */
#define TIM_DTG_0 ((uint16_t)0x0001) /* 位0 */
#define TIM_DTG_1 ((uint16_t)0x0002) /* 位1 */
#define TIM_DTG_2 ((uint16_t)0x0004) /* 位2 */
#define TIM_DTG_3 ((uint16_t)0x0008) /* 位3 */
#define TIM_DTG_4 ((uint16_t)0x0010) /* 位4 */
#define TIM_DTG_5 ((uint16_t)0x0020) /* 位5 */
#define TIM_DTG_6 ((uint16_t)0x0040) /* 位6 */
#define TIM_DTG_7 ((uint16_t)0x0080) /* 位7 */

#define TIM_LOCK ((uint16_t)0x0300)   /* LOCK[1:0] 位 (锁定配置) */
#define TIM_LOCK_0 ((uint16_t)0x0100) /* 位0 */
#define TIM_LOCK_1 ((uint16_t)0x0200) /* 位1 */

#define TIM_OSSI ((uint16_t)0x0400) /* 空闲模式关闭状态选择 */
#define TIM_OSSR ((uint16_t)0x0800) /* 运行模式关闭状态选择 */
#define TIM_BKE ((uint16_t)0x1000)  /* 刹车使能 */
#define TIM_BKP ((uint16_t)0x2000)  /* 刹车极性 */
#define TIM_AOE ((uint16_t)0x4000)  /* 自动输出使能 */
#define TIM_MOE ((uint16_t)0x8000)  /* 主输出使能 */

/*******************  TIM_DMACFGR 寄存器的位定义  ********************/
#define TIM_DBA ((uint16_t)0x001F)   /* DBA[4:0] 位 (DMA 基地址) */
#define TIM_DBA_0 ((uint16_t)0x0001) /* 位0 */
#define TIM_DBA_1 ((uint16_t)0x0002) /* 位1 */
#define TIM_DBA_2 ((uint16_t)0x0004) /* 位2 */
#define TIM_DBA_3 ((uint16_t)0x0008) /* 位3 */
#define TIM_DBA_4 ((uint16_t)0x0010) /* 位4 */

#define TIM_DBL ((uint16_t)0x1F00)   /* DBL[4:0] 位 (DMA 突发长度) */
#define TIM_DBL_0 ((uint16_t)0x0100) /* 位0 */
#define TIM_DBL_1 ((uint16_t)0x0200) /* 位1 */
#define TIM_DBL_2 ((uint16_t)0x0400) /* 位2 */
#define TIM_DBL_3 ((uint16_t)0x0800) /* 位3 */
#define TIM_DBL_4 ((uint16_t)0x1000) /* 位4 */

/*******************  TIM_DMAADR 寄存器的位定义  *******************/
#define TIM_DMAR_DMAB ((uint16_t)0xFFFF) /* DMA 突发访问寄存器 */

/*******************  TIM_AUX 寄存器的位定义  *******************/
#define TIM_AUX_CAPCH2_ED ((uint16_t)0x0001) /* 捕获通道2 边沿检测 */
#define TIM_AUX_CAPCH3_ED ((uint16_t)0x0002) /* 捕获通道3 边沿检测 */
#define TIM_AUX_CAPCH4_ED ((uint16_t)0x0004) /* 捕获通道4 边沿检测 */

/******************************************************************************/
/*         通用同步异步收发器 (USART)                                          */
/******************************************************************************/

/*******************  USART_STATR 寄存器的位定义  *******************/
#define USART_STATR_PE ((uint16_t)0x0001)   /* 奇偶错误 */
#define USART_STATR_FE ((uint16_t)0x0002)   /* 帧错误 */
#define USART_STATR_NE ((uint16_t)0x0004)   /* 噪声错误标志 */
#define USART_STATR_ORE ((uint16_t)0x0008)  /* 溢出错误 */
#define USART_STATR_IDLE ((uint16_t)0x0010) /* 空闲线路检测 */
#define USART_STATR_RXNE ((uint16_t)0x0020) /* 读数据寄存器非空 */
#define USART_STATR_TC ((uint16_t)0x0040)   /* 发送完成 */
#define USART_STATR_TXE ((uint16_t)0x0080)  /* 发送数据寄存器空 */
#define USART_STATR_LBD ((uint16_t)0x0100)  /* LIN 断开检测标志 */
#define USART_STATR_CTS ((uint16_t)0x0200)  /* CTS 标志 */
#define USART_STATR_RX_BUSY ((uint16_t)0x0400) /* 接收忙 */
#define USART_STATR_MS_ERR ((uint16_t)0x0800) /* 多处理器错误 */

/*******************  USART_DATAR 寄存器的位定义  *******************/
#define USART_DATAR_DR ((uint16_t)0x01FF) /* 数据值 */

/******************  USART_BRR 寄存器的位定义  *******************/
#define USART_BRR_DIV_Fraction ((uint16_t)0x000F) /* USARTDIV 的小数部分 */
#define USART_BRR_DIV_Mantissa ((uint16_t)0xFFF0) /* USARTDIV 的整数部分 */

/******************  USART_CTLR1 寄存器的位定义  *******************/
#define USART_CTLR1_SBK ((uint16_t)0x0001)    /* 发送断开 */
#define USART_CTLR1_RWU ((uint16_t)0x0002)    /* 接收器唤醒 */
#define USART_CTLR1_RE ((uint16_t)0x0004)     /* 接收器使能 */
#define USART_CTLR1_TE ((uint16_t)0x0008)     /* 发送器使能 */
#define USART_CTLR1_IDLEIE ((uint16_t)0x0010) /* IDLE 中断使能 */
#define USART_CTLR1_RXNEIE ((uint16_t)0x0020) /* RXNE 中断使能 */
#define USART_CTLR1_TCIE ((uint16_t)0x0040)   /* 发送完成中断使能 */
#define USART_CTLR1_TXEIE ((uint16_t)0x0080)  /* TXE 中断使能 */
#define USART_CTLR1_PEIE ((uint16_t)0x0100)   /* PE 中断使能 */
#define USART_CTLR1_PS ((uint16_t)0x0200)     /* 奇偶选择 */
#define USART_CTLR1_PCE ((uint16_t)0x0400)    /* 奇偶控制使能 */
#define USART_CTLR1_WAKE ((uint16_t)0x0800)   /* 唤醒方法 */
#define USART_CTLR1_M ((uint16_t)0x1000)      /* 字长 */
#define USART_CTLR1_UE ((uint16_t)0x2000)     /* USART 使能 */
#define USART_CTLR1_M_EXT_0 ((uint16_t)0x4000) /* 字长扩展位0 */
#define USART_CTLR1_M_EXT_1 ((uint16_t)0x8000) /* 字长扩展位1 */
#define USART_CTLR1_M_EXT5 ((uint16_t)0xC000)  /* 字长扩展5 */
#define USART_CTLR1_M_EXT6 ((uint16_t)0x8000)  /* 字长扩展6 */
#define USART_CTLR1_M_EXT7 ((uint16_t)0x4000)  /* 字长扩展7 */

/******************  USART_CTLR2 寄存器的位定义  *******************/
#define USART_CTLR2_ADD ((uint16_t)0x000F)   /* USART 节点地址 */
#define USART_CTLR2_LBDL ((uint16_t)0x0020)  /* LIN 断开检测长度 */
#define USART_CTLR2_LBDIE ((uint16_t)0x0040) /* LIN 断开检测中断使能 */
#define USART_CTLR2_LBCL ((uint16_t)0x0100)  /* 最后一个位时钟脉冲 */
#define USART_CTLR2_CPHA ((uint16_t)0x0200)  /* 时钟相位 */
#define USART_CTLR2_CPOL ((uint16_t)0x0400)  /* 时钟极性 */
#define USART_CTLR2_CLKEN ((uint16_t)0x0800) /* 时钟使能 */

#define USART_CTLR2_STOP ((uint16_t)0x3000)   /* STOP[1:0] 位 (停止位) */
#define USART_CTLR2_STOP_0 ((uint16_t)0x1000) /* 位0 */
#define USART_CTLR2_STOP_1 ((uint16_t)0x2000) /* 位1 */

#define USART_CTLR2_LINEN ((uint16_t)0x4000) /* LIN 模式使能 */

/******************  USART_CTLR3 寄存器的位定义  *******************/
#define USART_CTLR3_EIE ((uint16_t)0x0001)    /* 错误中断使能 */
#define USART_CTLR3_IREN ((uint16_t)0x0002)   /* IrDA 模式使能 */
#define USART_CTLR3_IRLP ((uint16_t)0x0004)   /* IrDA 低功耗 */
#define USART_CTLR3_HDSEL ((uint16_t)0x0008)  /* 半双工选择 */
#define USART_CTLR3_NACK ((uint16_t)0x0010)   /* 智能卡 NACK 使能 */
#define USART_CTLR3_SCEN ((uint16_t)0x0020)   /* 智能卡模式使能 */
#define USART_CTLR3_DMAR ((uint16_t)0x0040)   /* DMA 接收器使能 */
#define USART_CTLR3_DMAT ((uint16_t)0x0080)   /* DMA 发送器使能 */
#define USART_CTLR3_RTSE ((uint16_t)0x0100)   /* RTS 使能 */
#define USART_CTLR3_CTSE ((uint16_t)0x0200)   /* CTS 使能 */
#define USART_CTLR3_CTSIE ((uint16_t)0x0400)  /* CTS 中断使能 */
#define USART_CTLR3_ONEBIT ((uint16_t)0x0800) /* 单比特方法 */

/******************  USART_GPR 寄存器的位定义  ******************/
#define USART_GPR_PSC ((uint16_t)0x00FF)   /* PSC[7:0] 位 (预分频器值) */
#define USART_GPR_PSC_0 ((uint16_t)0x0001) /* 位0 */
#define USART_GPR_PSC_1 ((uint16_t)0x0002) /* 位1 */
#define USART_GPR_PSC_2 ((uint16_t)0x0004) /* 位2 */
#define USART_GPR_PSC_3 ((uint16_t)0x0008) /* 位3 */
#define USART_GPR_PSC_4 ((uint16_t)0x0010) /* 位4 */
#define USART_GPR_PSC_5 ((uint16_t)0x0020) /* 位5 */
#define USART_GPR_PSC_6 ((uint16_t)0x0040) /* 位6 */
#define USART_GPR_PSC_7 ((uint16_t)0x0080) /* 位7 */

#define USART_GPR_GT ((uint16_t)0xFF00) /* 保护时间值 */

/******************  USART_CTLR4 寄存器的位定义  ******************/
#define USART_CTLR4_MS_ERRIE ((uint16_t)0x0002) /* 多处理器错误中断使能 */
#define USART_CTLR4_CHECK_SEL ((uint16_t)0x000C) /* 校验选择 */
#define USART_CTLR4_CHECK_MARKENABLE ((uint16_t)0x0008) /* 校验标记使能 */
#define USART_CTLR4_CHECK_APACEENABLE ((uint16_t)0x000C) /* 校验间隔使能 */

/******************************************************************************/
/*                            OPA                                             */
/******************************************************************************/

/*******************  WWDG_CTLR 寄存器的位定义  ********************/
#define OPA_EN1 ((uint32_t)0x0001)    /* OPA1 使能 */
#define OPA_MODE1 ((uint32_t)0x0002)  /* OPA1 模式 */
#define OPA_NSEL1 ((uint32_t)0x0004)  /* OPA1 负输入选择 */
#define OPA_PSEL1 ((uint32_t)0x0008)  /* OPA1 正输入选择 */
#define OPA_EN2 ((uint32_t)0x0010)    /* OPA2 使能 */
#define OPA_MODE2 ((uint32_t)0x0020)  /* OPA2 模式 */
#define OPA_NSEL2 ((uint32_t)0x0040)  /* OPA2 负输入选择 */
#define OPA_PSEL2 ((uint32_t)0x0080)  /* OPA2 正输入选择 */
#define OPA_EN3 ((uint32_t)0x0100)    /* OPA3 使能 */
#define OPA_MODE3 ((uint32_t)0x0200)  /* OPA3 模式 */
#define OPA_NSEL3 ((uint32_t)0x0400)  /* OPA3 负输入选择 */
#define OPA_PSEL3 ((uint32_t)0x0800)  /* OPA3 正输入选择 */
#define OPA_EN4 ((uint32_t)0x1000)    /* OPA4 使能 */
#define OPA_MODE4 ((uint32_t)0x2000)  /* OPA4 模式 */
#define OPA_NSEL4 ((uint32_t)0x4000)  /* OPA4 负输入选择 */
#define OPA_PSEL4 ((uint32_t)0x8000)  /* OPA4 正输入选择 */

/******************************************************************************/
/*                            窗口看门狗 (WWDG)                                */
/******************************************************************************/

/*******************  WWDG_CTLR 寄存器的位定义  ********************/
#define WWDG_CTLR_T ((uint8_t)0x7F)  /* T[6:0] 位 (7位计数器，MSB到LSB) */
#define WWDG_CTLR_T0 ((uint8_t)0x01) /* 位0 */
#define WWDG_CTLR_T1 ((uint8_t)0x02) /* 位1 */
#define WWDG_CTLR_T2 ((uint8_t)0x04) /* 位2 */
#define WWDG_CTLR_T3 ((uint8_t)0x08) /* 位3 */
#define WWDG_CTLR_T4 ((uint8_t)0x10) /* 位4 */
#define WWDG_CTLR_T5 ((uint8_t)0x20) /* 位5 */
#define WWDG_CTLR_T6 ((uint8_t)0x40) /* 位6 */

#define WWDG_CTLR_WDGA ((uint8_t)0x80) /* 激活位 */

/*******************  WWDG_CFGR 寄存器的位定义  *******************/
#define WWDG_CFGR_W ((uint16_t)0x007F)  /* W[6:0] 位 (7位窗口值) */
#define WWDG_CFGR_W0 ((uint16_t)0x0001) /* 位0 */
#define WWDG_CFGR_W1 ((uint16_t)0x0002) /* 位1 */
#define WWDG_CFGR_W2 ((uint16_t)0x0004) /* 位2 */
#define WWDG_CFGR_W3 ((uint16_t)0x0008) /* 位3 */
#define WWDG_CFGR_W4 ((uint16_t)0x0010) /* 位4 */
#define WWDG_CFGR_W5 ((uint16_t)0x0020) /* 位5 */
#define WWDG_CFGR_W6 ((uint16_t)0x0040) /* 位6 */

#define WWDG_CFGR_WDGTB ((uint16_t)0x0180)  /* WDGTB[1:0] 位 (时基) */
#define WWDG_CFGR_WDGTB0 ((uint16_t)0x0080) /* 位0 */
#define WWDG_CFGR_WDGTB1 ((uint16_t)0x0100) /* 位1 */

#define WWDG_CFGR_EWI ((uint16_t)0x0200) /* 早期唤醒中断 */

/*******************  WWDG_STATR 寄存器的位定义  ********************/
#define WWDG_STATR_EWIF ((uint8_t)0x01) /* 早期唤醒中断标志 */

/******************************************************************************/
/*                          增强功能 (EXTEN)                                   */
/******************************************************************************/

/****************************  Enhanced CTLR1 寄存器  *****************************/
#define EXTEN_USBD_LS ((uint32_t)0x00000001)       /* USB D+ 上拉控制 */
#define EXTEN_USBD_PU_EN ((uint32_t)0x00000002)    /* USB D+ 上拉使能 */
#define EXTEN_ETH_10M_EN ((uint32_t)0x00000004)    /* 以太网 10M 模式使能 */
#define EXTEN_ETH_RGMII_SEL ((uint32_t)0x00000008) /* 以太网 RGMII 选择 */
#define EXTEN_PLL_HSI_PRE ((uint32_t)0x00000010)   /* PLL HSI 预分频 */
#define EXTEN_LOCKUP_EN ((uint32_t)0x00000040)     /* 锁存使能 */
#define EXTEN_LOCKUP_RSTF ((uint32_t)0x00000080)   /* 锁存复位标志 */

#define EXTEN_ULLDO_TRIM ((uint32_t)0x00000300)  /* ULLDO_TRIM[1:0] 位 (超低功耗 LDO 修调) */
#define EXTEN_ULLDO_TRIM0 ((uint32_t)0x00000100) /* 位0 */
#define EXTEN_ULLDO_TRIM1 ((uint32_t)0x00000200) /* 位1 */

#define EXTEN_LDO_TRIM ((uint32_t)0x00000C00)  /* LDO_TRIM[1:0] 位 (LDO 修调) */
#define EXTEN_LDO_TRIM0 ((uint32_t)0x00000400) /* 位0 */
#define EXTEN_LDO_TRIM1 ((uint32_t)0x00000800) /* 位1 */
#define EXTEN_HSEKPLP ((uint32_t)0x00001000)   /* HSE 保持低功耗 */

/****************************  Enhanced CTLR2 寄存器  *****************************/
#define EXTEN_CTLR2_OPA1_HSMD ((uint32_t)0x00000001) /* OPA1 高速模式 */
#define EXTEN_CTLR2_OPA2_HSMD ((uint32_t)0x00000002) /* OPA2 高速模式 */
#define EXTEN_CTLR2_OPA3_HSMD ((uint32_t)0x00000004) /* OPA3 高速模式 */
#define EXTEN_CTLR2_OPA4_HSMD ((uint32_t)0x00000008) /* OPA4 高速模式 */

  /****************************  Enhanced FEATURE_SIGN 寄存器  *****************************/

#define FEATURE_SIGN_VLEVEL ((uint32_t)0x00000001) /* 特征版本级别 */

/******************************************************************************/
/*                                  DVP                                       */
/******************************************************************************/

/*******************  DVP_CR0 寄存器的位定义  ********************/
#define RB_DVP_ENABLE 0x01  // RW, DVP 使能
#define RB_DVP_V_POLAR 0x02 // RW, DVP VSYNC 极性控制: 1=反相, 0=不反相
#define RB_DVP_H_POLAR 0x04 // RW, DVP HSYNC 极性控制: 1=反相, 0=不反相
#define RB_DVP_P_POLAR 0x08 // RW, DVP PCLK 极性控制: 1=反相, 0=不反相
#define RB_DVP_MSK_DAT_MOD 0x30 // 数据模式掩码
#define RB_DVP_D8_MOD 0x00  // RW, DVP 8位数据模式
#define RB_DVP_D10_MOD 0x10 // RW, DVP 10位数据模式
#define RB_DVP_D12_MOD 0x20 // RW, DVP 12位数据模式
#define RB_DVP_JPEG 0x40    // RW, DVP JPEG 模式

/*******************  DVP_CR1 寄存器的位定义  ********************/
#define RB_DVP_DMA_EN 0x01  // RW, DVP DMA 使能
#define RB_DVP_ALL_CLR 0x02 // RW, DVP 全部清除，高有效
#define RB_DVP_RCV_CLR 0x04 // RW, DVP 接收逻辑清除，高有效
#define RB_DVP_BUF_TOG 0x08 // RW, DVP 缓冲区软件翻转，写1翻转，写0忽略
#define RB_DVP_CM 0x10      // RW, DVP 捕获模式
#define RB_DVP_CROP 0x20    // RW, DVP 裁剪功能使能
#define RB_DVP_FCRC 0xC0    // RW, DVP 帧捕获率控制:
#define DVP_RATE_100P 0x00  // 00 = 每帧捕获 (100%)
#define DVP_RATE_50P 0x40   // 01 = 隔帧捕获 (50%)
#define DVP_RATE_25P 0x80   // 10 = 每四帧捕获一帧 (25%)

/*******************  DVP_IER 寄存器的位定义  ********************/
#define RB_DVP_IE_STR_FRM 0x01  // RW, DVP 帧起始中断使能
#define RB_DVP_IE_ROW_DONE 0x02 // RW, DVP 行接收完成中断使能
#define RB_DVP_IE_FRM_DONE 0x04 // RW, DVP 帧接收完成中断使能
#define RB_DVP_IE_FIFO_OV 0x08  // RW, DVP 接收 FIFO 溢出中断使能
#define RB_DVP_IE_STP_FRM 0x10  // RW, DVP 帧停止中断使能

/*******************  DVP_IFR 寄存器的位定义  ********************/
#define RB_DVP_IF_STR_FRM 0x01  // RW1, DVP 帧起始中断标志
#define RB_DVP_IF_ROW_DONE 0x02 // RW1, DVP 行接收完成中断标志
#define RB_DVP_IF_FRM_DONE 0x04 // RW1, DVP 帧接收完成中断标志
#define RB_DVP_IF_FIFO_OV 0x08  // RW1, DVP 接收 FIFO 溢出中断标志
#define RB_DVP_IF_STP_FRM 0x10  // RW1, DVP 帧停止中断标志

/*******************  DVP_STATUS 寄存器的位定义  ********************/
#define RB_DVP_FIFO_RDY 0x01     // RO, DVP 接收 FIFO 就绪
#define RB_DVP_FIFO_FULL 0x02    // RO, DVP 接收 FIFO 满
#define RB_DVP_FIFO_OV 0x04      // RO, DVP 接收 FIFO 溢出
#define RB_DVP_MSK_FIFO_CNT 0x70 // RO, DVP 接收 FIFO 计数掩码

/*******************  DVP_ROW_CNT 寄存器的位定义  ********************/
#define RB_DVP_ROW_CNT ((uint16_t)0xFF) /* 行计数 */

/*******************  DVP_HOFFCNT 寄存器的位定义  ********************/
#define RB_DVP_HOFFCNT ((uint16_t)0xFF) /* 水平偏移计数 */

/*******************  DVP_VST 寄存器的位定义  ********************/
#define RB_DVP_VST ((uint16_t)0xFF) /* 垂直起始 */

/*******************  DVP_CAPCNT 寄存器的位定义  ********************/
#define RB_DVP_CAPCNT ((uint16_t)0xFF) /* 捕获计数 */

/*******************  DVP_VLINE 寄存器的位定义  ********************/
#define RB_DVP_VLINE ((uint16_t)0xFF) /* 垂直行数 */

/*******************  DVP_DR 寄存器的位定义  ********************/
#define RB_DVP_DR ((uint16_t)0xFF) /* 数据寄存器 */

/******************************************************************************/
/*                                  TKEY                                      */
/******************************************************************************/

/*******************  TKEY_CHARGE1 寄存器的位定义  *******************/
#define TKEY_CHARGE1_TKCG10 ((uint32_t)0x0007) /* 触摸键通道10 充电时间配置 */
#define TKEY_CHARGE1_TKCG10_1C5 ((uint32_t)0x0000) /* 1.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG10_7C5 ((uint32_t)0x0001) /* 7.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG10_13C5 ((uint32_t)0x0002) /* 13.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG10_28C5 ((uint32_t)0x0003) /* 28.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG10_41C5 ((uint32_t)0x0004) /* 41.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG10_55C5 ((uint32_t)0x0005) /* 55.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG10_71C5 ((uint32_t)0x0006) /* 71.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG10_239C5 ((uint32_t)0x0007) /* 239.5 个时钟周期 */

#define TKEY_CHARGE1_TKCG11 ((uint32_t)0x0038) /* 触摸键通道11 充电时间配置 */
#define TKEY_CHARGE1_TKCG11_1C5 ((uint32_t)0x0000) /* 1.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG11_7C5 ((uint32_t)0x0008) /* 7.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG11_13C5 ((uint32_t)0x0010) /* 13.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG11_28C5 ((uint32_t)0x0018) /* 28.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG11_41C5 ((uint32_t)0x0020) /* 41.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG11_55C5 ((uint32_t)0x0028) /* 55.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG11_71C5 ((uint32_t)0x0030) /* 71.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG11_239C5 ((uint32_t)0x0038) /* 239.5 个时钟周期 */

#define TKEY_CHARGE1_TKCG12 ((uint32_t)0x01C0) /* 触摸键通道12 充电时间配置 */
#define TKEY_CHARGE1_TKCG12_1C5 ((uint32_t)0x0000) /* 1.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG12_7C5 ((uint32_t)0x0040) /* 7.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG12_13C5 ((uint32_t)0x0080) /* 13.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG12_28C5 ((uint32_t)0x00C0) /* 28.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG12_41C5 ((uint32_t)0x0100) /* 41.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG12_55C5 ((uint32_t)0x0140) /* 55.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG12_71C5 ((uint32_t)0x0180) /* 71.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG12_239C5 ((uint32_t)0x01C0) /* 239.5 个时钟周期 */

#define TKEY_CHARGE1_TKCG13 ((uint32_t)0x0E00) /* 触摸键通道13 充电时间配置 */
#define TKEY_CHARGE1_TKCG13_1C5 ((uint32_t)0x0000) /* 1.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG13_7C5 ((uint32_t)0x0200) /* 7.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG13_13C5 ((uint32_t)0x0400) /* 13.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG13_28C5 ((uint32_t)0x0600) /* 28.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG13_41C5 ((uint32_t)0x0800) /* 41.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG13_55C5 ((uint32_t)0x0A00) /* 55.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG13_71C5 ((uint32_t)0x0C00) /* 71.5 个时钟周期 */
#define TKEY_CHARGE1_TKCG13_239C5 ((uint32_t)0x0E00) /* 239.5 个时钟周期 */

#define TKEY_CHARGE1_TKCG14 ((uint32_t)0x7000) /* 触摸键通道14 充电时间配置 */
#define TKEY_CHARGE1_TKCG15 ((uint32_t)0x38000) /* 触摸键通道15 充电时间配置 */
#define TKEY_CHARGE1_TKCG16 ((uint32_t)0x1C0000) /* 触摸键通道16 充电时间配置 */
#define TKEY_CHARGE1_TKCG17 ((uint32_t)0xE00000) /* 触摸键通道17 充电时间配置 */

#include "ch32v30x_conf.h"

#ifdef __cplusplus
}
#endif

#endif