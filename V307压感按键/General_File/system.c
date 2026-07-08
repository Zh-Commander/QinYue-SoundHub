#include "system.h"
#include "system_config.h"
#include "param.h"
#include "ch32v30x_usart.h"
#include "usart_extend.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "key.h"

System_State_t System_State;

void UART5_Return_NO();
void UART5_Return_YES();

void System_Init(void)
{
    System_State = SYSTEM_STATE_IDLE;
}

void System_Loop(void)
{
    //printf("STATE:%d", System_State);
    switch (System_State)
    {
        case SYSTEM_STATE_IDLE:
        {
            // 接收指令
            uint8_t Rx_CMD;
            USART_ReceiveDataGroup_Blocking(UART5, &Rx_CMD, 1);
            //Rx_CMD = UART5_CMD_IS_IDLE;
            // 指令处理
            switch (Rx_CMD)
            {
                case UART5_CMD_IS_IDLE:
                    UART5_Return_YES();  // 返回YES
                    break;
                case UART5_IDLECMD_ENTER_SCAN:
                    //UART5_Return_YES();  // 返回YES
                    System_State = SYSTEM_STATE_SCAN;
                    Scan_Enable();
                    break;
                case UART5_IDLECMD_ENTER_SET:
                    UART5_Return_YES();  // 返回YES
                    System_State = SYSTEM_STATE_SET;
                    break;
                case UART5_IDLECMD_ENTER_ADJUST:
                    UART5_Return_YES();  // 返回YES
                    System_State = SYSTEM_STATE_ADJUST;
                    break;
                case UART5_IDLECMD_ENTER_RESTORE:
                    UART5_Return_YES();  // 返回YES
                    EraseParam();
                    InitParam();
                    break;
                default:
                    UART5_Return_NO();  // 返回NO
                    break;
            }
            break;
        }
        case SYSTEM_STATE_SCAN:
        {
            break;
        }
        case SYSTEM_STATE_SET:
        {
            // 接收指令
            uint8_t Rx_CMD;
            USART_ReceiveDataGroup_Blocking(UART5, &Rx_CMD, 1);
            switch (Rx_CMD)
            {
                case UART5_CMD_IS_IDLE:
                    UART5_Return_NO();       // 返回NO
                    break;
                case UART5_SETCMD_EXIT_SET:  // 退出设置模式
                    UART5_Return_YES();      // 返回YES
                    System_State = SYSTEM_STATE_IDLE;
                    break;
                case UART5_SETCMD_GETUL:  // 获取UPPERLINE
                    USART_SendDataGroup_Polling(UART5, (uint8_t *)(&UPPER_LINE), sizeof(UPPER_LINE));
                    break;
                case UART5_SETCMD_SETUL:  // 设置UPPERLINE
                    USART_ReceiveDataGroup_Blocking(UART5, (uint8_t *)(&UPPER_LINE), sizeof(UPPER_LINE));
                    SaveParam();
                    UART5_Return_YES();   // 返回YES
                    break;
                case UART5_SETCMD_GETLL:  // 获取LOWERLINE
                    USART_SendDataGroup_Polling(UART5, (uint8_t *)(&LOWER_LINE), sizeof(LOWER_LINE));
                    break;
                case UART5_SETCMD_SETLL:  // 设置LOWERLINE
                    USART_ReceiveDataGroup_Blocking(UART5, (uint8_t *)(&LOWER_LINE), sizeof(LOWER_LINE));
                    SaveParam();
                    UART5_Return_YES();     // 返回YES
                    break;
                case UART5_SETCMD_GETMAXL:  // 获取maxADC_One_D
                    USART_SendDataGroup_Polling(UART5, (uint8_t *)(&maxADC_One_D), sizeof(maxADC_One_D));
                    break;
                case UART5_SETCMD_SETMAXL:  // 设置maxADC_One_D
                    USART_ReceiveDataGroup_Blocking(UART5, (uint8_t *)(&maxADC_One_D), sizeof(maxADC_One_D));
                    SaveParam();
                    UART5_Return_YES();     // 返回YES
                case UART5_SETCMD_GETMINL:  // 获取minADC_One_D
                    USART_SendDataGroup_Polling(UART5, (uint8_t *)(&minADC_One_D), sizeof(minADC_One_D));
                    break;
                case UART5_SETCMD_SETMINL:  // 设置minADC_One_D
                    USART_ReceiveDataGroup_Blocking(UART5, (uint8_t *)(&minADC_One_D), sizeof(minADC_One_D));
                    SaveParam();
                    UART5_Return_YES();     // 返回YES
                case UART5_SETCMD_GETSCNT:  // 获取send_count
                    USART_SendDataGroup_Polling(UART5, (uint8_t *)(&send_count), sizeof(send_count));
                    break;
                case UART5_SETCMD_SETSCNT:  // 设置send_count
                    USART_ReceiveDataGroup_Blocking(UART5, (uint8_t *)(&send_count), sizeof(send_count));
                    SaveParam();
                    UART5_Return_YES();  // 返回YES
                    break;
            }
            break;
        }
        case SYSTEM_STATE_ADJUST:
        {
            // 接收指令
            uint8_t Rx_CMD;
            USART_ReceiveDataGroup_Blocking(UART5, &Rx_CMD, 1);
            switch (Rx_CMD)
            {
                case UART5_CMD_IS_IDLE:
                    UART5_Return_NO();  // 返回NO
                    break;
                case UART5_ADJUSTCMD_EXIT_ADJUST:
                    UART5_Return_YES();  // 返回YES
                    System_State = SYSTEM_STATE_IDLE;
                    break;
                case UART5_ADJUSTCMD_ADJMAXL:
                {
                    USART_ReceiveDataGroup_Blocking(UART5, (uint8_t *)(&Key_Ready), 2);  // 接收需要校准的按键
                    for(uint8_t i = 0; i < KEY_NUM; i++){AdjustBuffer[i] = 0.0f;}        // 校准缓冲区归零
                    isAdjustMode = true;                                                 // 开启校准模式
                    isAdjustFinished = false;                                            // 完成校准标志位RESET
                    AdjustParam = ADJUST_MAXADC;                                         // 需要校准的参数
                    Adjust_Init();
                    TIM_Cmd(TIM4, ENABLE);                                               // 开始扫描
                    while (!isAdjustFinished) {};                                        // 等待校准完成
                    TIM_Cmd(TIM4, DISABLE);                                              // 关闭扫描
                    isAdjustMode = false;                                                // 关闭校准模式
                    Adjust_Update();

                    SaveParam();  // 保存校准后的参数 !!!,校准后不能用
                    UART5_Return_YES();
                    break;
                }
                case UART5_ADJUSTCMD_ADJMINL:
                    USART_ReceiveDataGroup_Blocking(UART5, (uint8_t *)(&Key_Ready), 2);  // 接收需要校准的按键
                    for(uint8_t i = 0; i < KEY_NUM; i++){AdjustBuffer[i] = 0.0f;}        // 校准缓冲区归零
                    //memset(AdjustBuffer, 0, sizeof(float) * KEY_NUM);                    // 校准缓冲区归零
                    isAdjustMode = true;                                                 // 开启校准模式
                    isAdjustFinished = false;                                            // 完成校准标志位RESET
                    AdjustParam = ADJUST_MINADC;                                         // 需要校准的参数
                    Adjust_Init();
                    TIM_Cmd(TIM4, ENABLE);                                               // 开始扫描
                    while (!isAdjustFinished) {};                                        // 等待校准完成
                    TIM_Cmd(TIM4, DISABLE);                                              // 关闭扫描
                    isAdjustMode = false;                                                // 关闭校准模式
                    Adjust_Update();

                    SaveParam();  // 保存校准后的参数 !!!,校准后不能用
                    UART5_Return_YES();
                    break;
                    break;
            }
            break;
        }
        default:

            break;
    }
}

void UART5_Return_NO()
{
    USART_SendData(UART5, UART5_RET_NO);
    while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
}

void UART5_Return_YES()
{
    USART_SendData(UART5, UART5_RET_YES);
    while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
}

void Scan_Enable()
{
    Key_Init();

    // 开启USART5 发送 DMA接口
    USART_DMACmd(UART5, USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(UART7, USART_DMAReq_Tx, ENABLE);

    // 开始扫描
    TIM_Cmd(TIM4, ENABLE);
}

void Scan_Disable()
{
    Key_DeInit();

    // 关闭扫描
    TIM_Cmd(TIM4, DISABLE);

    // 关闭USART 发送 DMA接口
    USART_DMACmd(UART5, USART_DMAReq_Tx, DISABLE);
    USART_DMACmd(UART7, USART_DMAReq_Tx, DISABLE);

    // 更新系统状态
    System_State = SYSTEM_STATE_IDLE;

    UART5_Return_YES();
}