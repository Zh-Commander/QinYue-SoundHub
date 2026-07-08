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
#include "FreeRTOS.h"
#include "semphr.h"
#include <stdarg.h>

static uint8_t  p_us = 0;
static uint16_t p_ms = 0;

static SemaphoreHandle_t xDebugMutex = NULL;
static QueueHandle_t xDebugRxQueue = NULL;
static volatile uint8_t g_debug_rx_buffer[DEBUG_RX_BUFFER_SIZE];
static volatile uint16_t g_debug_rx_head = 0;
static volatile uint16_t g_debug_rx_tail = 0;

#define DEBUG_DATA0_ADDRESS  ((volatile uint32_t*)0xE0000380)
#define DEBUG_DATA1_ADDRESS  ((volatile uint32_t*)0xE0000384)

static USART_TypeDef *Debug_GetUSART(void)
{
#if(DEBUG == DEBUG_UART1)
    return USART1;
#elif(DEBUG == DEBUG_UART2)
    return USART2;
#elif(DEBUG == DEBUG_UART3)
    return USART3;
#else
    return USART1;
#endif
}

static IRQn_Type Debug_GetIRQn(void)
{
#if(DEBUG == DEBUG_UART1)
    return USART1_IRQn;
#elif(DEBUG == DEBUG_UART2)
    return USART2_IRQn;
#elif(DEBUG == DEBUG_UART3)
    return USART3_IRQn;
#else
    return USART1_IRQn;
#endif
}

static void Debug_RX_BufferPush(uint8_t ch)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint16_t next_head = (uint16_t)((g_debug_rx_head + 1U) % DEBUG_RX_BUFFER_SIZE);

    if(next_head == g_debug_rx_tail)
    {
        g_debug_rx_tail = (uint16_t)((g_debug_rx_tail + 1U) % DEBUG_RX_BUFFER_SIZE);
    }

    g_debug_rx_buffer[g_debug_rx_head] = ch;
    g_debug_rx_head = next_head;

    if(xDebugRxQueue != NULL)
    {
        xQueueSendFromISR(xDebugRxQueue, &ch, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/*********************************************************************
 * @fn      Delay_Init
 *
 * @brief   Initializes Delay Funcation.
 *
 * @return  none
 */
void Delay_Init(void)
{
    p_us = SystemCoreClock / 8000000;
    p_ms = (uint16_t)p_us * 1000;
}

/*********************************************************************
 * @fn      Delay_Us
 *
 * @brief   Microsecond Delay Time.
 *
 * @param   n - Microsecond number.
 *
 * @return  None
 */
void Delay_Us(uint32_t n)
{
    uint32_t i;

    SysTick->SR &= ~(1 << 0);
    i = (uint32_t)n * p_us;

    SysTick->CMP = i;
    SysTick->CTLR |= (1 << 4);
    SysTick->CTLR |= (1 << 5) | (1 << 0);

    while((SysTick->SR & (1 << 0)) != (1 << 0))
        ;
    SysTick->CTLR &= ~(1 << 0);
}

/*********************************************************************
 * @fn      Delay_Ms
 *
 * @brief   Millisecond Delay Time.
 *
 * @param   n - Millisecond number.
 *
 * @return  None
 */
void Delay_Ms(uint32_t n)
{
    uint32_t i;

    SysTick->SR &= ~(1 << 0);
    i = (uint32_t)n * p_ms;

    SysTick->CMP = i;
    SysTick->CTLR |= (1 << 4);
    SysTick->CTLR |= (1 << 5) | (1 << 0);

    while((SysTick->SR & (1 << 0)) != (1 << 0))
        ;
    SysTick->CTLR &= ~(1 << 0);
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
    NVIC_InitTypeDef  NVIC_InitStructure;

#if(DEBUG == DEBUG_UART1)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif(DEBUG == DEBUG_UART2)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

#elif(DEBUG == DEBUG_UART3)
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

#endif

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

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

    g_debug_rx_head = 0;
    g_debug_rx_tail = 0;

    NVIC_InitStructure.NVIC_IRQChannel = Debug_GetIRQn();
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_ITConfig(Debug_GetUSART(), USART_IT_RXNE, ENABLE);
}


/*********************************************************************
 * @fn      SDI_Printf_Enable
 *
 * @param   None
 *
 * @return  None
 */
void SDI_Printf_Enable(void)
{
    *(DEBUG_DATA0_ADDRESS) = 0;
    Delay_Init();
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

/*********************************************************************
 * @fn      Debug_Printf_Init
 *
 * @brief   初始化调试打印互斥锁
 *
 * @return  none
 */
void Debug_Printf_Init(void)
{
    if(xDebugMutex == NULL)
    {
        xDebugMutex = xSemaphoreCreateMutex();
        if(xDebugMutex == NULL)
        {
            /* Mutex creation failed, output error message using printf */
            for(int i = 0; i < 20; i++)
            {
#if(DEBUG == DEBUG_UART1)
                while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
                USART_SendData(USART1, "Mutex create failed!"[i]);
#elif(DEBUG == DEBUG_UART2)
                while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
                USART_SendData(USART2, "Mutex create failed!"[i]);
#elif(DEBUG == DEBUG_UART3)
                while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
                USART_SendData(USART3, "Mutex create failed!"[i]);
#endif
            }
        }
    }

    if(xDebugRxQueue == NULL)
    {
        xDebugRxQueue = xQueueCreate(DEBUG_RX_QUEUE_LENGTH, sizeof(uint8_t));
    }
}

/*********************************************************************
 * @fn      Debug_Printf
 *
 * @brief   线程安全的格式化打印函数
 *
 * @param   format - 格式化字符串
 * @param   ... - 可变参数
 *
 * @return  none
 */
int Debug_ReadChar(uint8_t *ch)
{
    if (ch == NULL) {
        return 0;
    }

    if (g_debug_rx_head == g_debug_rx_tail) {
        return 0;
    }

    *ch = g_debug_rx_buffer[g_debug_rx_tail];
    g_debug_rx_tail = (uint16_t)((g_debug_rx_tail + 1U) % DEBUG_RX_BUFFER_SIZE);
    return 1;
}

BaseType_t Debug_ReadCharBlocking(uint8_t *ch, TickType_t timeout_ticks)
{
    if (ch == NULL || xDebugRxQueue == NULL) {
        return pdFALSE;
    }

    return xQueueReceive(xDebugRxQueue, ch, timeout_ticks);
}

void Debug_UART_IRQHandler(void)
{
    USART_TypeDef *usart = Debug_GetUSART();

    if (USART_GetITStatus(usart, USART_IT_RXNE) != RESET) {
        Debug_RX_BufferPush((uint8_t)(USART_ReceiveData(usart) & 0xFF));
    }
}

void Debug_Printf(const char *format, ...)
{
    char buffer[256];
    va_list args;
    int i;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    /* If mutex is not initialized, send directly without mutex protection */
    if(xDebugMutex == NULL)
    {
        for(i = 0; buffer[i] != '\0'; i++)
        {
#if(DEBUG == DEBUG_UART1)
            while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
            USART_SendData(USART1, buffer[i]);
#elif(DEBUG == DEBUG_UART2)
            while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
            USART_SendData(USART2, buffer[i]);
#elif(DEBUG == DEBUG_UART3)
            while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
            USART_SendData(USART3, buffer[i]);
#endif
        }
    }
    else
    {
        if(xSemaphoreTake(xDebugMutex, portMAX_DELAY) == pdTRUE)
        {
            for(i = 0; buffer[i] != '\0'; i++)
            {
#if(DEBUG == DEBUG_UART1)
                while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
                USART_SendData(USART1, buffer[i]);
#elif(DEBUG == DEBUG_UART2)
                while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
                USART_SendData(USART2, buffer[i]);
#elif(DEBUG == DEBUG_UART3)
                while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
                USART_SendData(USART3, buffer[i]);
#endif
            }
            xSemaphoreGive(xDebugMutex);
        }
    }
}



