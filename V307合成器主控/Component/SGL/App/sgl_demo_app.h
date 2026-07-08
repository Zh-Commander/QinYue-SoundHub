#ifndef __SGL_DEMO_APP_H__
#define __SGL_DEMO_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * @fn      SGL_SystemInit
 *
 * @brief   初始化并创建 SGL 屏幕显示任务
 *
 * @return  none
 */
void SGL_SystemInit(void);
void SGL_DemoApp_Init(void);
void SGL_DemoApp_Loop(void);
void SGL_DemoApp_TickISR(void);
void SGL_DemoApp_TouchScanISR(void);
void SGL_DemoApp_LcdDmaISR(void);

#ifdef __cplusplus
}
#endif

#endif
