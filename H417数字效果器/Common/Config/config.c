#include "config.h"

// ===  全局指针定义 ===
volatile uint16_t* I2S_RX_DMA_Buffer;
volatile uint16_t* I2S_TX_DMA_Buffer;

int32_t* I2S_RightDecode_Buffer;
int32_t* I2S_LeftDecode_Buffer;

volatile I2S_Buffer_TransPkg_t* I2S_Buffer_TransPkg;
volatile Effect_Algorithm_TransPkg_t* Effect_Algorithm_TransPkg;
volatile Trans_Structure_t* Trans_Structure;