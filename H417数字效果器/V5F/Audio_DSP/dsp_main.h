#ifndef __DSP_MAIN_H
#define __DSP_MAIN_H

#include <stdint.h>

void DSP_Init(void);
void Audio_DSP_Main(volatile uint16_t* I2S_SourceBuffer, volatile uint16_t* I2S_ObjectBuffer);

#endif