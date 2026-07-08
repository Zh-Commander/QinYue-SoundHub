#include "system.h"
#include "hardware_v5f.h"
#include "config.h"
#include "dsp_main.h" 

// 私有函数声明
static void Config_Params_Receive(void);

void System_Init(void)
{
    while (IPC->MSG[0] == 0); // 等待V3F核发来参数
    
    CPU_FENCE();
    Config_Params_Receive(); // 接收V3F的参数并更新Config里的参数

    CPU_FENCE();
    DSP_Init();

    CPU_FENCE();
    Hardware_V5f();

    CPU_FENCE();
    IPC_WriteMSG(IPC_MSG1, 1); // 通知V3F核完成
    CPU_FENCE();
}

void System_Loop(void)
{
    
}

static void Config_Params_Receive(void)
{
    // === 接收参数结构体 ===
    Trans_Structure = (Trans_Structure_t*)IPC_ReadMSG(IPC_MSG0);
    // 挂靠到V5F核下的全局配置指针
    I2S_Buffer_TransPkg = Trans_Structure->I2S_Buffer_TransPkg;
    Effect_Algorithm_TransPkg = Trans_Structure->Effect_Algorithm_TransPkg;

    // === 全局指针更新 ===
    I2S_RX_DMA_Buffer = I2S_Buffer_TransPkg->I2S_RX_DMA_Buffer;
    I2S_TX_DMA_Buffer = I2S_Buffer_TransPkg->I2S_TX_DMA_Buffer;
    I2S_RightDecode_Buffer = I2S_Buffer_TransPkg->I2S_RightDecode_Buffer;
    I2S_LeftDecode_Buffer = I2S_Buffer_TransPkg->I2S_LeftDecode_Buffer;
}