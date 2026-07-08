/**
  ******************************************************************************
  * @file    w25q128_for_ch32v307.h
  * @brief   W25Q128 Flash memory driver for CH32V307 RISC-V MCU
  * @note    This driver is adapted from STM32 examples for QingHeng CH32V307
  ******************************************************************************
  */

#ifndef __W25Q128_FOR_CH32V307_H
#define __W25Q128_FOR_CH32V307_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v30x.h"

/* W25Q128 SPI Commands */
#define W25Q128_WriteEnable_CMD          0x06
#define W25Q128_WriteDisable_CMD         0x04
#define W25Q128_ReadStatusReg1_CMD       0x05
#define W25Q128_ReadStatusReg2_CMD       0x35
#define W25Q128_ReadStatusReg3_CMD       0x15
#define W25Q128_WriteStatusReg1_CMD      0x01
#define W25Q128_WriteStatusReg2_CMD      0x31
#define W25Q128_WriteStatusReg3_CMD      0x11
#define W25Q128_ReadData_CMD             0x03
#define W25Q128_FastRead_CMD             0x0B
#define W25Q128_FastReadDualIO_CMD       0x3B
#define W25Q128_PageProgram_CMD          0x02
#define W25Q128_SectorErase_CMD          0x20
#define W25Q128_BlockErase32K_CMD        0x52
#define W25Q128_BlockErase64K_CMD        0xD8
#define W25Q128_ChipErase_CMD            0xC7
#define W25Q128_PowerDown_CMD            0xB9
#define W25Q128_ReleasePowerDown_CMD     0xAB
#define W25Q128_ManufacturerDeviceID_CMD 0x90
#define W25Q128_JedecID_CMD              0x9F
#define W25Q128_ReadUniqueID_CMD         0x4B
#define W25Q128_Enable4ByteAddr_CMD      0xB7
#define W25Q128_Exit4ByteAddr_CMD        0xE9

/* W25Q128 Chip IDs - Support both Winbond and GigaDevice */
#define W25Q128_MANUFACTURER_WINBOND      0xEF
#define W25Q128_MANUFACTURER_GIGADEVICE   0xC8
#define W25Q128_DEVICE_ID                 0x17  /* W25Q128/GD25Q128 device ID */
#define W25Q128_JEDEC_ID_WINBOND          0xEF4018
#define W25Q128_JEDEC_ID_GIGADEVICE       0xC84018  /* Update based on actual read */

/* W25Q128 Memory Parameters */
#define W25Q128_FLASH_SIZE               (16 * 1024 * 1024)  /* 16 MBits = 2 MBytes */
#define W25Q128_SECTOR_SIZE              4096    /* 4 KB */
#define W25Q128_BLOCK_SIZE               65536   /* 64 KB */
#define W25Q128_PAGE_SIZE                256     /* 256 bytes */
#define W25Q128_NUM_SECTORS              (W25Q128_FLASH_SIZE / W25Q128_SECTOR_SIZE)
#define W25Q128_NUM_BLOCKS               (W25Q128_FLASH_SIZE / W25Q128_BLOCK_SIZE)
#define W25Q128_NUM_PAGES                (W25Q128_FLASH_SIZE / W25Q128_PAGE_SIZE)

/* Status Register Bit Definitions */
#define W25Q128_STATUS_BUSY              (1 << 0)  /* Write/Erase in progress */
#define W25Q128_STATUS_WEL               (1 << 1)  /* Write Enable Latch */
#define W25Q128_STATUS_BP0               (1 << 2)  /* Block Protect Bit 0 */
#define W25Q128_STATUS_BP1               (1 << 3)  /* Block Protect Bit 1 */
#define W25Q128_STATUS_BP2               (1 << 4)  /* Block Protect Bit 2 */
#define W25Q128_STATUS_TB                (1 << 5)  /* Top/Bottom Protect */
#define W25Q128_STATUS_SEC               (1 << 6)  /* Sector Protect */
#define W25Q128_STATUS_SRP0              (1 << 7)  /* Status Register Protect 0 */

/* Pin Configuration - User should define these based on hardware */
#define W25Q128_CS_GPIO_PORT             GPIOA
#define W25Q128_CS_GPIO_RCC              RCC_APB2Periph_GPIOA
#define W25Q128_CS_GPIO_PIN              GPIO_Pin_4

#define W25Q128_SCK_GPIO_PORT            GPIOA
#define W25Q128_SCK_GPIO_RCC             RCC_APB2Periph_GPIOA
#define W25Q128_SCK_GPIO_PIN             GPIO_Pin_5

#define W25Q128_MOSI_GPIO_PORT           GPIOA
#define W25Q128_MOSI_GPIO_RCC            RCC_APB2Periph_GPIOA
#define W25Q128_MOSI_GPIO_PIN            GPIO_Pin_7

#define W25Q128_MISO_GPIO_PORT           GPIOA
#define W25Q128_MISO_GPIO_RCC            RCC_APB2Periph_GPIOA
#define W25Q128_MISO_GPIO_PIN            GPIO_Pin_6

/* SPI Configuration - User should define these based on hardware*/
#define W25Q128_SPI_x                    SPI1
#define W25Q128_SPI_RCC                  RCC_APB2Periph_SPI1

/* User-configurable delay functions */
/* User should provide delay_us function if not defined */
#define W25Q128_DELAY_US(us) Delay_Us(us)

/* User should provide delay_ms function if not defined */
#define W25Q128_DELAY_MS(ms) Delay_Ms(ms)

/* Function Prototypes */

/* Low-level SPI functions */
void W25Q128_SPI_Init(void);
uint8_t W25Q128_SPI_ReadWriteByte(uint8_t data);

/* CS pin control - inline for efficiency */
static inline void W25Q128_CS_Low(void)
{
    GPIO_ResetBits(W25Q128_CS_GPIO_PORT, W25Q128_CS_GPIO_PIN);
}

static inline void W25Q128_CS_High(void)
{
    GPIO_SetBits(W25Q128_CS_GPIO_PORT, W25Q128_CS_GPIO_PIN);
}

/* Basic Flash operations */
uint8_t W25Q128_ReadStatusReg(uint8_t reg);
void W25Q128_WriteStatusReg(uint8_t reg, uint8_t data);
void W25Q128_WriteEnable(void);
void W25Q128_WriteDisable(void);
uint32_t W25Q128_ReadID(void);
uint32_t W25Q128_ReadJEDECID(void);
void W25Q128_WaitForReady(void);

/* Read operations */
void W25Q128_ReadData(uint32_t addr, uint8_t *buffer, uint32_t len);
void W25Q128_FastReadData(uint32_t addr, uint8_t *buffer, uint32_t len);

/* Write operations */
void W25Q128_PageProgram(uint32_t addr, const uint8_t *data, uint32_t len);
void W25Q128_WriteData(uint32_t addr, const uint8_t *data, uint32_t len);

/* Erase operations */
void W25Q128_SectorErase(uint32_t addr);
void W25Q128_BlockErase32K(uint32_t addr);
void W25Q128_BlockErase64K(uint32_t addr);
void W25Q128_ChipErase(void);

/* Power management */
void W25Q128_PowerDown(void);
void W25Q128_ReleasePowerDown(void);

/* 4-Byte Address Mode */
void W25Q128_Enable4ByteAddressMode(void);
void W25Q128_Exit4ByteAddressMode(void);

/* Helper functions */
uint8_t W25Q128_IsBusy(void);
void W25Q128_Reset(void);

#ifdef __cplusplus
}
#endif

#endif /* __W25Q128_FOR_CH32V307_H */