#ifndef __ILI9341_H__
#define __ILI9341_H__

#include "debug.h"

#define LCD_W 320
#define LCD_H 240

void ILI9341_Init(void);
void ILI9341_FlushArea(uint16_t x1, uint16_t x2, uint16_t y1, uint16_t y2, const void *data, uint16_t len);
void ILI9341_FlushCompleteISR(void);
uint32_t ILI9341_GetFlushStartCount(void);
uint32_t ILI9341_GetFlushDoneCount(void);
uint32_t ILI9341_GetFlushBusyTimeoutCount(void);
uint32_t ILI9341_GetFlushTxWaitTimeoutCount(void);
uint32_t ILI9341_GetFlushIdleWaitTimeoutCount(void);
uint32_t ILI9341_GetFlushLenMismatchCount(void);
uint32_t ILI9341_GetFlushStageWaitCount(void);

#endif
