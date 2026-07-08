/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "..\\Component\\SD\\sdio.h"
#include "..\\Component\\SD\\FATFS\\ff.h"
#include "..\\Component\\SD\\fs_lock.h"
#include "..\\Component\\midi\\midi_mode.h"
#include "..\\Component\\SGL\\App\\sgl_demo_app.h"

FATFS fs;  // FatFS file system object

/*********************************************************************
 * @fn      vApplicationStackOverflowHook
 *
 * @brief   Stack overflow hook function
 *
 * @param   xTask - Task handle
 *          pcTaskName - Task name
 *
 * @return  none
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    taskDISABLE_INTERRUPTS();
    Debug_Printf("Stack overflow: %s\r\n", pcTaskName);
    while(1)
    {
    }
}

void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    Debug_Printf("Malloc failed\r\n");
    while(1)
    {
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(1843200);
	Debug_Printf_Init();

	Debug_Printf("SystemClk:%d\r\n",SystemCoreClock);
	Debug_Printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
	Debug_Printf("FreeRTOS Kernel Version:%s\r\n",tskKERNEL_VERSION_NUMBER);

	/* SD Card and FatFS Initialization */
	Debug_Printf("Initializing SD card...\r\n");
	while(SD_Init())
	{
		Debug_Printf("SD card initialization failed, retrying...\r\n");
		Delay_Ms(100);
	}
	Debug_Printf("SD card initialized successfully\r\n");

	FRESULT res = f_mount(&fs, "1:", 1);
	if(res != FR_OK)
	{
		Debug_Printf("File system mount failed, error code: %d\r\n", res);
	}
	else
	{
		Debug_Printf("File system mounted successfully\r\n");
	}

	fs_lock_init();

	/* 初始化应用模式，默认进入自由演奏模式 */
	Debug_Printf("Before app_mode_init\r\n");
	if(app_mode_init() != 0)
	{
		Debug_Printf("app_mode_init failed: %s\r\n", app_mode_get_last_error());
	}
	else
	{
		Debug_Printf("After app_mode_init\r\n");
		Debug_Printf("Default mode: %s\r\n", app_mode_get_name(app_mode_get()));
	}

	Debug_Printf("ADC volume channels: PB0(ADC8) PB1(ADC9) PC4(ADC14) PC5(ADC15)\r\n");
	Debug_Printf("Before scheduler\r\n");

	/* 初始化并创建 SGL 屏幕任务 */
	SGL_SystemInit();
	vTaskStartScheduler();
	Debug_Printf("Scheduler returned\r\n");

	while(1)
	{
	}
}
