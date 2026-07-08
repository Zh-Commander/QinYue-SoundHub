#ifndef __CRC_SMALL_H
#define __CRC_SMALL_H

#include <stdint.h>

uint8_t crc8_small(const uint8_t *data, uint32_t len); // 优化的CRC-8(小内存设备)
uint16_t crc16_small(const uint8_t *data, uint32_t len); // 优化的CRC-16(小内存设备)
uint32_t crc32_small_ext(const uint8_t *data, uint32_t len,  // 增强版小内存CRC-32(支持不同配置)
                        uint32_t init, uint32_t xorout, 
                        uint8_t refin, uint8_t refout);

#endif