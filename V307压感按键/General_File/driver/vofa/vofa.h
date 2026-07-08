#ifndef __VOFA_H
#define __VOFA_H

#include "ch32v30x.h"
#include "usart_extend.h"

void Vofa_JF_SendFloat(USART_TypeDef *USARTx, float fData[], uint8_t count);

#endif