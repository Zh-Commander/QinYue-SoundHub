/********************************** (C) COPYRIGHT  *******************************
* File Name          : debug.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for UART
*                      Printf , Delay functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "bitop.h"

static uint8_t  p_us = 0;
static uint16_t p_ms = 0;

#define DEBUG_DATA0_ADDRESS  ((volatile uint32_t*)0xE0000380)
#define DEBUG_DATA1_ADDRESS  ((volatile uint32_t*)0xE0000384)

/*********************************************************************
 * @fn      SysTick_Init
 *
 * @brief   Initializes Delay Funcation.
 *
 * @return  none
 */
void SysTick_Init(void)
{
    p_us = SystemCoreClock / 8000000; // 每微秒SysTick数，144MHz下是18个
    p_ms = (uint16_t)p_us * 1000; // 每毫秒SysTick数
    BITOP_CLR(SysTick->CTLR, 4); // 向上计数
    BITOP_CLR(SysTick->CTLR, 3); // 一直向上计数
    BITOP_SET(SysTick->CTLR, 5); // 更新初始值
    BITOP_SET(SysTick->CTLR ,0); // 开始计数
}

/*********************************************************************
 * @fn      Timer_GetSysNs
 *
 * @brief   使用系统计时器实现纳秒计时，需占用系统计时器
 *
 * @return  当前时间戳（纳秒）
 *
 * @note    每计数 = 1/SystemCoreClock 秒
 *          例如144MHz时，每计数6.94ns
 */
float Timer_GetSysNs(void)
{
    // 读取定时器计数值
    float cnt = SysTick->CNT * 1000 / p_us;
    
    return cnt;
}

/*********************************************************************
 * @fn      Timer_GetSysUs
 *
 * @brief   使用系统计时器实现纳秒计时，需占用系统计时器
 *
 * @return  当前时间戳（纳秒）
 *
 */
float Timer_GetSysUs(void)
{
    // 读取定时器计数值
    float cnt = SysTick->CNT / p_us;

    return cnt;
}

/*********************************************************************
 * @fn      Timer_GetSysMs
 *
 * @brief   使用系统计时器实现纳秒计时，需占用系统计时器
 *
 * @return  当前时间戳（纳秒）
 *
 */
float Timer_GetSysMs(void)
{
    // 读取定时器计数值
    float cnt = SysTick->CNT / p_ms;

    return cnt;
}

/*********************************************************************
 * @fn      Delay_Us
 *
 * @brief   微妙计时。不要延时小于10us的。
 *
 * @param   n - Microsecond number.
 *
 * @return  None
 */
void Delay_Us(uint32_t n)
{
    uint32_t add = (uint32_t)n * p_us;;

    SysTick->SR &= ~(1 << 0); //  清除状态位

    SysTick->CMP = SysTick->CNT + add; // 设置比较值

    while((SysTick->SR & (1 << 0)) != (1 << 0)) // 等待计数器到达比较值
        ;
}

/*********************************************************************
 * @fn      Delay_Ms
 *
 * @brief   毫秒延时。
 *
 * @param   n - Millisecond number.
 *
 * @return  None
 */
void Delay_Ms(uint32_t n)
{
    uint32_t add = (uint32_t)n * p_ms;;

    SysTick->SR &= ~(1 << 0); //  清除状态位

    SysTick->CMP = SysTick->CNT + add; // 设置比较值

    while((SysTick->SR & (1 << 0)) != (1 << 0)) // 等待计数器到达比较值
        ;
}

/*********************************************************************
 * @fn      USART_Printf_Init
 *
 * @brief   Initializes the USARTx peripheral.
 *
 * @param   baudrate - USART communication baud rate.
 *
 * @return  None
 */
void USART_Printf_Init(uint32_t baudrate)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

#if(DEBUG == DEBUG_UART1)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif(DEBUG == DEBUG_UART2)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif(DEBUG == DEBUG_UART3)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#endif

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

#if(DEBUG == DEBUG_UART1)
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);

#elif(DEBUG == DEBUG_UART2)
    USART_Init(USART2, &USART_InitStructure);
    USART_Cmd(USART2, ENABLE);

#elif(DEBUG == DEBUG_UART3)
    USART_Init(USART3, &USART_InitStructure);
    USART_Cmd(USART3, ENABLE);

#endif
}

/*********************************************************************
 * @fn      SDI_Printf_Enable
 *
 * @brief   Initializes the SDI printf Function.
 *
 * @param   None
 *
 * @return  None
 */
void SDI_Printf_Enable(void)
{
    *(DEBUG_DATA0_ADDRESS) = 0;
    SysTick_Init();
    Delay_Ms(1);
}

/*********************************************************************
 * @fn      _write
 *
 * @brief   Support Printf Function
 *
 * @param   *buf - UART send Data.
 *          size - Data length
 *
 * @return  size: Data length
 */
__attribute__((used)) int _write(int fd, char *buf, int size)
{
    int i = 0;

#if (SDI_PRINT == SDI_PR_OPEN)
    int writeSize = size;

    do
    {

        /**
         * data0  data1 8 bytes
         * data0 The lowest byte storage length, the maximum is 7
         *
         */

        while( (*(DEBUG_DATA0_ADDRESS) != 0u))
        {

        }

        if(writeSize>7)
        {
            *(DEBUG_DATA1_ADDRESS) = (*(buf+i+3)) | (*(buf+i+4)<<8) | (*(buf+i+5)<<16) | (*(buf+i+6)<<24);
            *(DEBUG_DATA0_ADDRESS) = (7u) | (*(buf+i)<<8) | (*(buf+i+1)<<16) | (*(buf+i+2)<<24);

            i += 7;
            writeSize -= 7;
        }
        else
        {
            *(DEBUG_DATA1_ADDRESS) = (*(buf+i+3)) | (*(buf+i+4)<<8) | (*(buf+i+5)<<16) | (*(buf+i+6)<<24);
            *(DEBUG_DATA0_ADDRESS) = (writeSize) | (*(buf+i)<<8) | (*(buf+i+1)<<16) | (*(buf+i+2)<<24);

            writeSize = 0;
        }

    } while (writeSize);


#else
    for(i = 0; i < size; i++)
    {
#if(DEBUG == DEBUG_UART1)
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, *buf++);
#elif(DEBUG == DEBUG_UART2)
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2, *buf++);
#elif(DEBUG == DEBUG_UART3)
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
        USART_SendData(USART3, *buf++);
#endif
    }
#endif
    return size;
}

/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
__attribute__((used)) void *_sbrk(ptrdiff_t incr)
{
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

    curbrk += incr;
    return curbrk - incr;
}



