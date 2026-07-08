#ifndef __CRC_FORM_H
#define __CRC_FORM_H

#include <stdint.h>

// === CRC8 ===
uint8_t crc8_calculate(const uint8_t *data, uint32_t len);      // CRC-8(不带最终异或值)
uint8_t crc8_calculate_xor(const uint8_t *data, uint32_t len);  // CRC-8(带最终异或值)

// === CRC16 ===
uint16_t crc16_calculate(const uint8_t *data, uint32_t len);                                      // CRC-16查表法（IBM标准）
uint16_t crc16_modbus(const uint8_t *data, uint32_t len);                                         // CRC-16查表法（MODBUS标准 - 低位在前）
uint16_t crc16_calculate_xor(const uint8_t *data, uint32_t len, uint16_t init, uint16_t xorout);  // CRC-16查表法（带最终异或）

// === CRC32 ===
uint32_t crc32_calculate(const uint8_t *data, uint32_t len);                                      // CRC-32查表法（以太网标准）
uint32_t crc32_calculate_ext(const uint8_t *data, uint32_t len, uint32_t init, uint32_t xorout);  // CRC-32查表法（可配置参数）


#endif