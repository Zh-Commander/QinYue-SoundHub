#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include <stdint.h>

void System_Init(void);
void System_Loop(void);

void V3F_OnScreenParamChanged(
    uint8_t effect_type,
    uint8_t param_id,
    int32_t value);

void V3F_FlushDirtyParams50ms(void);

uint8_t V3F_SendSetSlotEffectEvent(
    uint8_t slot,
    uint8_t effect_type);

uint8_t V3F_SendLoadDefaultParamsEvent(uint8_t effect_type);

void V3F_ScreenRxPushByte(uint8_t byte);
void V3F_Tick1ms(void);

#endif