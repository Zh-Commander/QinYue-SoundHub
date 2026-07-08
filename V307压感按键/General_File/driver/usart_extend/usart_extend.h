#ifndef __USART_EXTEND_H
#define __USART_EXTEND_H

#include "ch32v30x.h"

void USART_SendDataGroup_Polling(USART_TypeDef* USARTx, uint8_t* TxBuffer, uint16_t TxSize);
void USART_ReceiveDataGroup_Blocking(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes);                                 /* 阻塞接受定长数据 */
uint8_t USART_ReceiveDataGroup_BlockingWithTimeout(USART_TypeDef *USARTx, uint8_t *buffer, uint32_t nBytes, uint32_t timeout); /* 阻塞接受定长数据，带超时检测 */

#endif