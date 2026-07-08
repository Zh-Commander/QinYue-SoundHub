#include <usart_extend.h>

void USART_SendDataGroup_Polling(USART_TypeDef* USARTx, uint8_t* TxBuffer, uint16_t TxSize)
{
    uint16_t TxCnt = 0;
    while(TxCnt < TxSize)
    {
        USART_SendData(USARTx, TxBuffer[TxCnt++]);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
    }
}

/*********************************************************************
 * @函数名      USART_ReceiveDataGroup_Blocking
 *
 * @描述        阻塞接收指定数量的字节
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              buffer - 存储接收数据的缓冲区指针
 *              nBytes - 要接收的字节数
 *
 * @返回值      无
 */
void USART_ReceiveDataGroup_Blocking(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes)
{
    uint32_t i = 0;

    while(i < nBytes)
    {
        /* 等待接收数据寄存器非空 (RXNE标志) */
        while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
        {
            /* 阻塞等待，直到有数据可读 */
            /* 注意：这里可以添加超时机制避免死循环 */
        }
        /* 读取接收到的数据 */
        buffer[i++] = (uint8_t)(USART_ReceiveData(USARTx) & 0xFF);
    }
}


/*********************************************************************
 * @函数名      USART_ReceiveDataGroup_BlockingWithTimeout
 *
 * @描述        带超时的阻塞接收指定数量的字节
 *
 * @参数        USARTx - x可以是1到3选择USART外设，或者4到8选择UART外设
 *              buffer - 存储接收数据的缓冲区指针
 *              nBytes - 要接收的字节数
 *              timeout - 超时时间（循环计数）
 *
 * @返回值      接收状态：SUCCESS或ERROR
 */
uint8_t USART_ReceiveDataGroup_BlockingWithTimeout(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes, uint32_t timeout)
{
    uint32_t i = 0;
    uint32_t tick = 0;
    
    while(i < nBytes)
    {
        tick = 0;
        /* 等待接收数据寄存器非空，带超时 */
        while(USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET)
        {
            if(tick++ > timeout)
            {
                /* 超时返回 */
                return 1;
            }
        }
        
        /* 读取接收到的数据 */
        buffer[i++] = (uint8_t)(USART_ReceiveData(USARTx) & 0xFF);
    }
    
    return 0;
}