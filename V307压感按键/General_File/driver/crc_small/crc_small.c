#include "crc_small.h"

/**
 * @brief 优化的CRC-8（小内存设备）
 * @param data: 指向待校验数据的指针
 * @param len: 待校验数据的字节长度
 * @return: 8位CRC校验码
 * 
 * @note 使用说明：
 * - 使用CRC-8多项式：0x07
 * - 初始值：0x00
 * - 最终异或值：0x00
 * - 使用直接计算法，不占用查找表内存
 * - 速度较慢，但适合ROM受限的嵌入式系统
 * - 数据量小（<100字节）时性能可接受
 * 
 * @performance 计算一个字节需要8次循环，N字节数据需要8N次循环
 */
uint8_t crc8_small(const uint8_t *data, uint32_t len) {
    uint8_t crc = 0x00;
    uint8_t i;
    
    while (len--) {
        crc ^= *data++;
        for (i = 0; i < 8; i++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}

/**
 * @brief 优化的CRC-16(小内存设备)
 * @param data: 指向待校验数据的指针
 * @param len: 待校验数据的字节长度
 * @return: 16位CRC校验码
 * 
 * @note 使用说明：
 * - 使用MODBUS多项式：0xA001
 * - 初始值：0xFFFF
 * - 最终异或值：0x0000
 * - 使用直接计算法，不占用查找表内存
 * - 适合ROM小于4KB的嵌入式MCU
 * - 性能：每字节8次循环，每字节约20-30个CPU周期
 * 
 * @performance 相比查表法慢约8倍，但节省256字节ROM
 * @memory 仅需代码空间，无查找表占用
 */
uint16_t crc16_small(const uint8_t *data, uint32_t len) {
    uint16_t crc = 0xFFFF;
    uint8_t i;
    
    while (len--) {
        crc ^= *data++;
        for (i = 0; i < 8; i++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;  // 0xA001是0x8005的反转
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

/**
 * @brief 增强版小内存CRC-32（支持不同配置）
 * @param data: 指向待校验数据的指针
 * @param len: 待校验数据的字节长度
 * @param init: CRC初始值
 * @param xorout: 最终异或值
 * @param refin: 输入是否反转 (0: 否, 1: 是)
 * @param refout: 输出是否反转 (0: 否, 1: 是)
 * @return: 32位CRC校验码
 * 
 * @note 使用说明：
 * - 支持多种CRC-32变种配置
 * - 多项式固定为0x04C11DB7
 * - 可根据参数配置不同初始值、最终异或值和反转设置
 * 
 * @示例：标准CRC-32（以太网）
 * uint32_t crc = crc32_small_ext(data, len, 0xFFFFFFFF, 0xFFFFFFFF, 1, 1);
 * 
 * @示例：无反转CRC-32
 * uint32_t crc = crc32_small_ext(data, len, 0x00000000, 0x00000000, 0, 0);
 */
uint32_t crc32_small_ext(const uint8_t *data, uint32_t len, 
                        uint32_t init, uint32_t xorout, 
                        uint8_t refin, uint8_t refout) {
    uint32_t crc = init;
    uint8_t i;
    uint8_t byte;
    
    while (len--) {
        byte = *data++;
        
        // 输入反转处理
        if (refin) {
            // 反转输入字节的位顺序
            byte = (byte & 0x01) << 7 | (byte & 0x02) << 5 |
                   (byte & 0x04) << 3 | (byte & 0x08) << 1 |
                   (byte & 0x10) >> 1 | (byte & 0x20) >> 3 |
                   (byte & 0x40) >> 5 | (byte & 0x80) >> 7;
        }
        
        crc ^= (refin ? byte : ((uint32_t)byte << 24));
        
        // 处理每个字节的8位
        for (i = 0; i < 8; i++) {
            // 根据输入反转选择移位方向
            if (refin) {
                // 输入反转时使用右移
                crc = (crc & 1) ? ((crc >> 1) ^ 0xEDB88320) : (crc >> 1);
            } else {
                // 输入不反转时使用左移
                crc = (crc & 0x80000000) ? ((crc << 1) ^ 0x04C11DB7) : (crc << 1);
            }
        }
    }
    
    // 输出反转处理
    if (refout) {
        uint32_t reversed = 0;
        for (i = 0; i < 32; i++) {
            reversed <<= 1;
            reversed |= (crc & 1);
            crc >>= 1;
        }
        crc = reversed;
    }
    
    return crc ^ xorout;
}