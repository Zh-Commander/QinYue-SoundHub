#include <vofa.h>

static uint8_t tail[4] = {0x00, 0x00, 0x80, 0x7f};

void Vofa_JF_SendFloat(USART_TypeDef* USARTx, float fData[], uint8_t count)
{
    USART_SendDataGroup_Polling(USARTx, (uint8_t*)fData, sizeof(float) * count);
    USART_SendDataGroup_Polling(USARTx, tail, 4);
}