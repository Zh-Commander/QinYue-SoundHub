#ifndef __PCM5102_H
#define __PCM5102_H

#include <stdint.h>
#include "debug.h"


void I2S2_Init(void);
void DMA_Tx_Init( DMA_Channel_TypeDef* DMA_CHx, u32 ppadr, u32 memadr, u16 bufsize);
void PCM5102_Pack16To24(u16 *dst, const int16_t *src, u16 sampleCount);

void PlayAudioFromW25Q128(u32 flashAddr, u32 dataSize);
void PlayAudioFromSDCard(const char *filename);

#endif /* __PCM5102_H */
