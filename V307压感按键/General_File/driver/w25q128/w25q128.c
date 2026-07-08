/**
  ******************************************************************************
  * @file    w25q128_for_ch32v307.c
  * @brief   W25Q128 Flash memory driver for CH32V307 RISC-V MCU
  * @note    This driver is adapted from STM32 examples for QingHeng CH32V307
  ******************************************************************************
  */

#include "w25q128.h"
#include <string.h>

/* Private function prototypes */
static void W25Q128_SendCommand(uint8_t cmd);
static void W25Q128_SendAddress(uint32_t addr);

/* Private variables */
static SPI_TypeDef *W25Q128_SPI = W25Q128_SPI_x;  /* Default to SPI1, can be changed by user */

/**
  * @brief  Initialize SPI for W25Q128 communication
  * @note   Configures SPI1 in master mode with proper clock settings for W25Q128
  *         Default pins: CS=PA4, SCK=PA5, MISO=PA6, MOSI=PA7
  *         User can modify these defaults by defining different macros
  */
void W25Q128_SPI_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    SPI_InitTypeDef SPI_InitStructure = {0};

    /* Enable SPI clocks */
    RCC_APB2PeriphClockCmd(W25Q128_SPI_RCC, ENABLE);

    
    /* Configure CS pin (Default PA4) as output push-pull */
    RCC_APB2PeriphClockCmd(W25Q128_CS_GPIO_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = W25Q128_CS_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W25Q128_CS_GPIO_PORT, &GPIO_InitStructure);

    /* Configure SCK pin (Default PA5) as output push-pull */
    RCC_APB2PeriphClockCmd(W25Q128_SCK_GPIO_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = W25Q128_SCK_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W25Q128_SCK_GPIO_PORT, &GPIO_InitStructure);

    /* Configure MOSI pin (Default PA7) as output push-pull */
    RCC_APB2PeriphClockCmd(W25Q128_MOSI_GPIO_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = W25Q128_MOSI_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W25Q128_MOSI_GPIO_PORT, &GPIO_InitStructure);

    /* Configure MISO pin (Default PA6) as output push-pull */
    RCC_APB2PeriphClockCmd(W25Q128_MISO_GPIO_RCC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = W25Q128_MISO_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(W25Q128_MISO_GPIO_PORT, &GPIO_InitStructure);

    /* Set CS high (inactive) */
    W25Q128_CS_High();

    /* Configure SPI1 */
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;  /* 8-bit data for W25Q128 */
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;         /* Clock polarity low */
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;       /* Clock phase 1 edge */
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;          /* Software NSS management */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; /* Adjust as needed */
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; /* MSB first */
    SPI_InitStructure.SPI_CRCPolynomial = 7;           /* CRC polynomial */

    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);

    /* Send dummy byte to initialize SPI */
    W25Q128_SPI_ReadWriteByte(0xFF);
}

/**
  * @brief  Read/Write a single byte via SPI
  * @param  data: Byte to send
  * @return Received byte
  */
uint8_t W25Q128_SPI_ReadWriteByte(uint8_t data)
{
    /* Wait until transmit buffer is empty */
    while(SPI_I2S_GetFlagStatus(W25Q128_SPI, SPI_I2S_FLAG_TXE) == RESET);

    /* Send data */
    SPI_I2S_SendData(W25Q128_SPI, data);

    /* Wait until receive buffer is not empty */
    while(SPI_I2S_GetFlagStatus(W25Q128_SPI, SPI_I2S_FLAG_RXNE) == RESET);

    /* Return received data */
    return SPI_I2S_ReceiveData(W25Q128_SPI);
}


/**
  * @brief  Send command to W25Q128
  * @param  cmd: Command byte
  */
static void W25Q128_SendCommand(uint8_t cmd)
{
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(cmd);
}

/**
  * @brief  Send 24-bit address to W25Q128
  * @param  addr: 24-bit address
  */
static void W25Q128_SendAddress(uint32_t addr)
{
    W25Q128_SPI_ReadWriteByte((uint8_t)((addr) >> 16));  /* Address byte 2 (A23-A16) */
    W25Q128_SPI_ReadWriteByte((uint8_t)((addr) >> 8));   /* Address byte 1 (A15-A8) */
    W25Q128_SPI_ReadWriteByte((uint8_t)(addr));          /* Address byte 0 (A7-A0) */
}

/**
  * @brief  Read status register
  * @param  reg: Register number (1, 2, or 3)
  * @return Status register value
  */
uint8_t W25Q128_ReadStatusReg(uint8_t reg)
{
    uint8_t status = 0;
    uint8_t cmd;

    switch(reg)
    {
        case 1:
            cmd = W25Q128_ReadStatusReg1_CMD;
            break;
        case 2:
            cmd = W25Q128_ReadStatusReg2_CMD;
            break;
        case 3:
            cmd = W25Q128_ReadStatusReg3_CMD;
            break;
        default:
            return 0;
    }

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(cmd);
    status = W25Q128_SPI_ReadWriteByte(0xFF);
    W25Q128_CS_High();

    return status;
}

/**
  * @brief  Write status register
  * @param  reg: Register number (1, 2, or 3)
  * @param  data: Data to write
  */
void W25Q128_WriteStatusReg(uint8_t reg, uint8_t data)
{
    uint8_t cmd;

    switch(reg)
    {
        case 1:
            cmd = W25Q128_WriteStatusReg1_CMD;
            break;
        case 2:
            cmd = W25Q128_WriteStatusReg2_CMD;
            break;
        case 3:
            cmd = W25Q128_WriteStatusReg3_CMD;
            break;
        default:
            return;
    }

    W25Q128_WriteEnable();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(cmd);
    W25Q128_SPI_ReadWriteByte(data);
    W25Q128_CS_High();

    W25Q128_WaitForReady();
}

/**
  * @brief  Enable write operations
  */
void W25Q128_WriteEnable(void)
{
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_WriteEnable_CMD);
    W25Q128_CS_High();
}

/**
  * @brief  Disable write operations
  */
void W25Q128_WriteDisable(void)
{
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_WriteDisable_CMD);
    W25Q128_CS_High();
}

/**
  * @brief  Read manufacturer and device ID
  * @return ID: Upper 16 bits = Manufacturer ID (0xEF), Lower 16 bits = Device ID (0x17)
  */
uint32_t W25Q128_ReadID(void)
{
    uint32_t id = 0;
    uint8_t temp[2] = {0};

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_ManufacturerDeviceID_CMD);
    W25Q128_SPI_ReadWriteByte(0x00);
    W25Q128_SPI_ReadWriteByte(0x00);
    W25Q128_SPI_ReadWriteByte(0x00);
    temp[0] = W25Q128_SPI_ReadWriteByte(0xFF);  /* Manufacturer ID */
    temp[1] = W25Q128_SPI_ReadWriteByte(0xFF);  /* Device ID */
    W25Q128_CS_High();

    id = (temp[0] << 8) | temp[1];
    return id;
}

/**
  * @brief  Read JEDEC ID
  * @return JEDEC ID (24-bit)
  */
uint32_t W25Q128_ReadJEDECID(void)
{
    uint32_t jedec_id = 0;

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_JedecID_CMD);
    jedec_id = W25Q128_SPI_ReadWriteByte(0xFF) << 16;
    jedec_id |= W25Q128_SPI_ReadWriteByte(0xFF) << 8;
    jedec_id |= W25Q128_SPI_ReadWriteByte(0xFF);
    W25Q128_CS_High();

    return jedec_id;
}

/**
  * @brief  Wait until flash is not busy
  */
void W25Q128_WaitForReady(void)
{
    while(W25Q128_IsBusy());
}

/**
  * @brief  Check if flash is busy
  * @return 1 if busy, 0 if ready
  */
uint8_t W25Q128_IsBusy(void)
{
    return (W25Q128_ReadStatusReg(1) & W25Q128_STATUS_BUSY);
}

/**
  * @brief  Read data from flash
  * @param  addr: Starting address (24-bit)
  * @param  buffer: Buffer to store read data
  * @param  len: Number of bytes to read
  */
void W25Q128_ReadData(uint32_t addr, uint8_t *buffer, uint32_t len)
{
    uint32_t i;

    W25Q128_WaitForReady();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_ReadData_CMD);
    W25Q128_SendAddress(addr);

    for(i = 0; i < len; i++)
    {
        buffer[i] = W25Q128_SPI_ReadWriteByte(0xFF);
    }

    W25Q128_CS_High();
}

/**
  * @brief  Fast read data from flash
  * @param  addr: Starting address (24-bit)
  * @param  buffer: Buffer to store read data
  * @param  len: Number of bytes to read
  */
void W25Q128_FastReadData(uint32_t addr, uint8_t *buffer, uint32_t len)
{
    uint32_t i;

    W25Q128_WaitForReady();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_FastRead_CMD);
    W25Q128_SendAddress(addr);
    W25Q128_SPI_ReadWriteByte(0xFF);  /* Dummy byte for fast read */

    for(i = 0; i < len; i++)
    {
        buffer[i] = W25Q128_SPI_ReadWriteByte(0xFF);
    }

    W25Q128_CS_High();
}

/**
  * @brief  Program a page (up to 256 bytes)
  * @param  addr: Starting address (must be page-aligned)
  * @param  data: Data to write
  * @param  len: Number of bytes to write (max 256)
  */
void W25Q128_PageProgram(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t i;

    if(len > W25Q128_PAGE_SIZE)
        len = W25Q128_PAGE_SIZE;

    W25Q128_WaitForReady();
    W25Q128_WriteEnable();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_PageProgram_CMD);
    W25Q128_SendAddress(addr);

    for(i = 0; i < len; i++)
    {
        W25Q128_SPI_ReadWriteByte(data[i]);
    }

    W25Q128_CS_High();
    W25Q128_WaitForReady();
}

/**
  * @brief  Write data to flash with automatic page boundary handling
  * @param  addr: Starting address
  * @param  data: Data to write
  * @param  len: Number of bytes to write
  */
void W25Q128_WriteData(uint32_t addr, const uint8_t *data, uint32_t len)
{
    uint32_t page_remain;
    uint32_t write_len;
    uint32_t write_addr = addr;
    const uint8_t *write_data = data;

    while(len > 0)
    {
        /* Calculate remaining bytes in current page */
        page_remain = W25Q128_PAGE_SIZE - (write_addr % W25Q128_PAGE_SIZE);

        /* Determine how many bytes to write in this iteration */
        write_len = (len < page_remain) ? len : page_remain;

        /* Program the page */
        W25Q128_PageProgram(write_addr, write_data, write_len);

        /* Update pointers and counters */
        write_addr += write_len;
        write_data += write_len;
        len -= write_len;
    }
}

/**
  * @brief  Erase a 4KB sector
  * @param  addr: Address within the sector to erase
  */
void W25Q128_SectorErase(uint32_t addr)
{
    W25Q128_WaitForReady();
    W25Q128_WriteEnable();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_SectorErase_CMD);
    W25Q128_SendAddress(addr);
    W25Q128_CS_High();

    W25Q128_WaitForReady();
}

/**
  * @brief  Erase a 32KB block
  * @param  addr: Address within the block to erase
  */
void W25Q128_BlockErase32K(uint32_t addr)
{
    W25Q128_WaitForReady();
    W25Q128_WriteEnable();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_BlockErase32K_CMD);
    W25Q128_SendAddress(addr);
    W25Q128_CS_High();

    W25Q128_WaitForReady();
}

/**
  * @brief  Erase a 64KB block
  * @param  addr: Address within the block to erase
  */
void W25Q128_BlockErase64K(uint32_t addr)
{
    W25Q128_WaitForReady();
    W25Q128_WriteEnable();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_BlockErase64K_CMD);
    W25Q128_SendAddress(addr);
    W25Q128_CS_High();

    W25Q128_WaitForReady();
}

/**
  * @brief  Erase entire chip
  */
void W25Q128_ChipErase(void)
{
    W25Q128_WaitForReady();
    W25Q128_WriteEnable();

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_ChipErase_CMD);
    W25Q128_CS_High();

    W25Q128_WaitForReady();
}

/**
  * @brief  Enter power-down mode
  */
void W25Q128_PowerDown(void)
{
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_PowerDown_CMD);
    W25Q128_CS_High();

    /* Wait for power-down to complete */
    W25Q128_DELAY_US(3);  /* tDP = 3us typical */
}

/**
  * @brief  Release from power-down mode
  */
void W25Q128_ReleasePowerDown(void)
{
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_ReleasePowerDown_CMD);
    W25Q128_CS_High();

    /* Wait for release to complete */
    W25Q128_DELAY_US(3);  /* tRES1 = 3us typical */
}

/**
  * @brief  Enable 4-byte address mode
  */
void W25Q128_Enable4ByteAddressMode(void)
{
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_Enable4ByteAddr_CMD);
    W25Q128_CS_High();
}

/**
  * @brief  Exit 4-byte address mode
  */
void W25Q128_Exit4ByteAddressMode(void)
{
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(W25Q128_Exit4ByteAddr_CMD);
    W25Q128_CS_High();
}

/**
  * @brief  Reset the flash memory
  */
void W25Q128_Reset(void)
{
    /* Software reset sequence */
    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(0x66);  /* Enable Reset */
    W25Q128_CS_High();

    W25Q128_DELAY_US(1);

    W25Q128_CS_Low();
    W25Q128_SPI_ReadWriteByte(0x99);  /* Reset Device */
    W25Q128_CS_High();

    /* Wait for reset to complete */
    W25Q128_DELAY_MS(30);  /* tRST = 30ms maximum */
}

/************************ (C) COPYRIGHT QingHeng Microelectronics *****END OF FILE****/