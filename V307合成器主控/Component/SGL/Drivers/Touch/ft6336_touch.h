#ifndef __FT6336_TOUCH_H__
#define __FT6336_TOUCH_H__

#include "debug.h"
#include <stdbool.h>

bool FT6336_Touch_Init(void);
bool FT6336_Touch_ReadState(uint16_t *x, uint16_t *y, bool *pressed);
bool FT6336_Touch_ReadPoint(uint16_t *x, uint16_t *y);
bool FT6336_Touch_HasPoint(void);
bool FT6336_Touch_IntActive(void);
uint32_t FT6336_GetReadFailCount(void);
uint32_t FT6336_GetRecoverCount(void);
uint32_t FT6336_GetBusyTimeoutCount(void);
uint32_t FT6336_GetEventTimeoutCount(void);

#endif
