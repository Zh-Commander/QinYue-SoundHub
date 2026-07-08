#include "param.h"
#include "ch32v30x_gpio.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ========================================
// Variable (Need To Save)
// ========================================

// 按键弹起按下ADC值 - 用于归一化
uint16_t Key_One_Top[KEY_NUM] = {2033, 2040, 1954, 1950, 2050, 1954, 2034, 1981, 1977, 2013, 1952, 2012};
uint16_t Key_One_Bottom[KEY_NUM] = {1460, 1509, 1515, 1527, 1597, 1603, 1657, 1497, 1442, 1510, 1410, 1466};

// 按键状态转换阈值
float UPPER_LINE = 0.95f;
float LOWER_LINE = 0.05f;

// 最大导数值 - 用于力度映射
float maxADC_One_D = 4.5f;
float minADC_One_D = 0.24f;

// 每n个扫描周期回报一次
uint8_t send_count = 14;

typedef struct
{
    char isSaved[4]; // 用于表明flash中此处存有配置参数
    float UPPER_LINE;
    float LOWER_LINE;
    float maxADC_One_D;
    float minADC_One_D;
    uint16_t Key_One_Top[KEY_NUM];
    uint16_t Key_One_Bottom[KEY_NUM];
    uint8_t send_count;
} Data_t;

// ========================================
// Other
// ========================================

// Buffer
uint32_t Collection_Buffer[(KEY_NUM + 1) / 2 + 1] = {0};  // 双ADC采集Buffer
uint8_t KeyData_Tx_Buffer[HEAD_SIZE + KEY_IS_DATA_SIZE + KEY_NUM + CRC_SIZE] = {0};             // UART5按键回传数据

volatile bool isAdjustMode = false;
volatile bool isAdjustFinished = false;
uint8_t AdjustParam;
uint16_t Key_Ready = 0;
float AdjustBuffer[KEY_NUM] = {0};

const Key_Pin_t Key_Pin_Group[16] = {
    {GPIOA, GPIO_Pin_0},
    {GPIOA, GPIO_Pin_1},
    {GPIOA, GPIO_Pin_2},
    {GPIOA, GPIO_Pin_3},
    {GPIOA, GPIO_Pin_4},
    {GPIOA, GPIO_Pin_5},
    {GPIOA, GPIO_Pin_6},
    {GPIOA, GPIO_Pin_7},
    {GPIOB, GPIO_Pin_0},
    {GPIOB, GPIO_Pin_1},
    {GPIOC, GPIO_Pin_0},
    {GPIOC, GPIO_Pin_1},
    {GPIOC, GPIO_Pin_2},
    {GPIOC, GPIO_Pin_3},
    {GPIOC, GPIO_Pin_4},
    {GPIOC, GPIO_Pin_5},
};

// i转Key Number
uint8_t i2key[KEY_NUM] = {8,1,5,9,2,6,10,11,7,3,0,4};

#define SaveADDR ((uint32_t)0x0800F000)  // 60KB地址
#define FLASH_PAGE_SIZE 4096             // 4KB页大小
uint32_t Address = 0x00;                 // 写入地址

void SystemFreqChange_120MHz(void);
void SystemFreqChange_144MHz(void);

void InitParam(void)
{
    // 改变系统频率
    /* SystemFreqChange_120MHz();
    FLASH_Access_Clock_Cfg(FLASH_Access_SYSTEM_HALF);
    SysTick_Init();
    USART_Printf_Init(115200);  // 调试输出波特率 */

    // 更新数据
    Address = SaveADDR;
    Data_t Data;
        for (uint32_t i = 0; i < sizeof(Data_t); i++)
    {
        uint8_t read_data = *(__IO uint8_t *)Address;
        ((uint8_t*)(&Data))[i] = read_data;
        Address += 1;
    }

    if(!memcmp(Data.isSaved,"yes",4))
    {
    printf("Found Saved params");
    UPPER_LINE = Data.UPPER_LINE;
    LOWER_LINE = Data.LOWER_LINE;
    maxADC_One_D = Data.maxADC_One_D;
    minADC_One_D = Data.minADC_One_D;
    send_count = Data.send_count;
    memcpy(Key_One_Top, Data.Key_One_Top, sizeof(Key_One_Top));
    memcpy(Key_One_Bottom, Data.Key_One_Bottom, sizeof(Key_One_Bottom));
    }
    else
    {
        printf("Not Found Saved params");
    }

    // 恢复系统时钟频率
    /* SystemFreqChange_144MHz();
    SysTick_Init();
    USART_Printf_Init(115200);  // 调试输出波特率 */
}

void SaveParam(void)
{
    // 改变系统频率
    /* SystemFreqChange_120MHz();
    FLASH_Access_Clock_Cfg(FLASH_Access_SYSTEM_HALF);
    SysTick_Init();
    USART_Printf_Init(115200);  // 调试输出波特率 */
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    // 解锁FLASH
    FLASH_Unlock();
    // 数据打包
    Data_t* Data = malloc(sizeof(Data_t) + sizeof(Data_t)%2);
    Data->UPPER_LINE = UPPER_LINE;
    Data->LOWER_LINE = LOWER_LINE;
    Data->maxADC_One_D = maxADC_One_D;
    Data->minADC_One_D = minADC_One_D;
    Data->send_count = send_count;
    memcpy(Data->isSaved, "yes", 4);
    memcpy(Data->Key_One_Top, Key_One_Top, sizeof(Key_One_Top));
    memcpy(Data->Key_One_Bottom, Key_One_Bottom, sizeof(Key_One_Bottom));
    uint8_t *Data_p = (uint8_t *)(Data);  // 用于单个单个字节访问
    printf("Data Size:%d",sizeof(Data));
    
    // 擦除扇区
    Address = SaveADDR;
    FLASHStatus = FLASH_ErasePage(Address);
    if(FLASHStatus != FLASH_COMPLETE)
    {
        printf("Page Erase Error: %d\n", FLASHStatus);
    }
    printf("Page Erase Success\n");

    // 写入数据
    Address = SaveADDR;
    for (uint32_t i = 0; i < (sizeof(Data_t) + sizeof(Data_t)%2)/2; i++, Address+=2)
    {
        FLASHStatus = FLASH_ProgramHalfWord(Address, ((uint16_t*)Data)[i]);
        if (FLASHStatus != FLASH_COMPLETE)
        {
            printf("Program Error at 0x%08X: %d\n", Address, FLASHStatus);
            break;
        }
    }

    // 验证数据
    Address = SaveADDR;
    for (uint32_t i = 0; i < sizeof(Data_t); i++, Address++)
    {
        uint8_t read_data = *(__IO uint8_t *)Address;
        if (read_data != Data_p[i])
        {
            printf("Address 0x%08X: Write=0x%02X, Read=0x%02X\n",
                   Address, Data_p[i], read_data);
                   
        }
    }

    // 加锁FLASH
    //FLASH_Lock();

    // 恢复系统时钟频率
    /* SystemFreqChange_144MHz();
    SysTick_Init();
    USART_Printf_Init(115200);  // 调试输出波特率 */
}

void EraseParam(void)
{
    FLASH_Status FLASHStatus = FLASH_COMPLETE;

    // 解锁FLASH
    FLASH_Unlock();
    // 擦除扇区
    Address = SaveADDR;
    FLASHStatus = FLASH_ErasePage(Address);
    if(FLASHStatus != FLASH_COMPLETE)
    {
        printf("Page Erase Error: %d\n", FLASHStatus);
    }
    printf("Page Erase Success\n");
}

void SystemFreqChange_120MHz(void)
{
    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
    RCC->CTLR &= (uint32_t)0xFEF6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t)0xFF00FFFF;
    RCC->CTLR &= (uint32_t)0xEBFFFFFF;
    RCC->INTR = 0x00FF0000;
    RCC->CFGR2 = 0x00000000;

    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    RCC->CTLR |= ((uint32_t)RCC_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CTLR & RCC_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CTLR & RCC_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* HCLK = SYSCLK */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
        /* PCLK2 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
        /* PCLK1 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

        /*  PLL configuration: PLLCLK = HSE * 15 = 120 MHz */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_PLLSRC | RCC_PLLXTPRE |
                                              RCC_PLLMULL));
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL15_EXTEN);

        /* Enable PLL */
        RCC->CTLR |= RCC_PLLON;
        /* Wait till PLL is ready */
        while ((RCC->CTLR & RCC_PLLRDY) == 0)
        {
        }
        /* Select PLL as system clock source */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_SW));
        RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /*
         * If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error
         */
    }
    SystemCoreClockUpdate();
}

void SystemFreqChange_144MHz(void)
{
    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0xF0FF0000;
    RCC->CTLR &= (uint32_t)0xFEF6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t)0xFF00FFFF;
    RCC->CTLR &= (uint32_t)0xEBFFFFFF;
    RCC->INTR = 0x00FF0000;
    RCC->CFGR2 = 0x00000000;

    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    RCC->CTLR |= ((uint32_t)RCC_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CTLR & RCC_HSERDY;
        StartUpCounter++;
    } while ((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CTLR & RCC_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* HCLK = SYSCLK */
        RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;
        /* PCLK2 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE2_DIV1;
        /* PCLK1 = HCLK */
        RCC->CFGR0 |= (uint32_t)RCC_PPRE1_DIV2;

        /*  PLL configuration: PLLCLK = HSE * 15 = 120 MHz */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_PLLSRC | RCC_PLLXTPRE |
                                              RCC_PLLMULL));
        RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HSE | RCC_PLLXTPRE_HSE | RCC_PLLMULL15_EXTEN);

        /* Enable PLL */
        RCC->CTLR |= RCC_PLLON;
        /* Wait till PLL is ready */
        while ((RCC->CTLR & RCC_PLLRDY) == 0)
        {
        }
        /* Select PLL as system clock source */
        RCC->CFGR0 &= (uint32_t)((uint32_t) ~(RCC_SW));
        RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;
        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /*
         * If HSE fails to start-up, the application will have wrong clock
         * configuration. User can add here some code to deal with this error
         */
    }
    SystemCoreClockUpdate();
}