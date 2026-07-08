/*-----------------------------------------------------------------------/
/  底层磁盘接口模块包含文件  (C)ChaN, 2019                              /
/-----------------------------------------------------------------------*/

#ifndef _DISKIO_DEFINED
#define _DISKIO_DEFINED

#ifdef __cplusplus
extern "C"
{  // 如果使用C++编译器，使用C语言链接规范
#endif

    /* 磁盘函数状态类型定义 */
    typedef BYTE DSTATUS;

    /* 获取当前时间（用于文件时间戳） */
    DWORD get_fattime(void);

    /* 磁盘函数返回结果枚举 */
    typedef enum
    {
        RES_OK = 0, /* 0: 操作成功 */
        RES_ERROR,  /* 1: 读/写错误 */
        RES_WRPRT,  /* 2: 写保护 */
        RES_NOTRDY, /* 3: 设备未就绪 */
        RES_PARERR  /* 4: 无效参数 */
    } DRESULT;

    /*---------------------------------------*/
    /* 磁盘控制函数原型声明 */


    DSTATUS disk_initialize(BYTE pdrv);                                         // 初始化磁盘驱动器
    DSTATUS disk_status(BYTE pdrv);                                             // 获取磁盘状态
    DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count);         // 从磁盘读取数据
    DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count);  // 向磁盘写入数据
    DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);                        // 磁盘控制命令


    /* 磁盘状态位定义 (DSTATUS) */

#define STA_NOINIT 0x01  /* 驱动器未初始化 */
#define STA_NODISK 0x02  /* 驱动器中没有介质 */
#define STA_PROTECT 0x04 /* 写保护 */


/* disk_ioctrl函数的命令码 */

/* 通用命令（由FatFs使用） */
#define CTRL_SYNC 0        /* 完成待处理的写入操作（在FF_FS_READONLY == 0时必需） */
#define GET_SECTOR_COUNT 1 /* 获取介质容量（在FF_USE_MKFS == 1时必需） */
#define GET_SECTOR_SIZE 2  /* 获取扇区大小（在FF_MAX_SS != FF_MIN_SS时必需） */
#define GET_BLOCK_SIZE 3   /* 获取擦除块大小（在FF_USE_MKFS == 1时必需） */
#define CTRL_TRIM 4        /* 通知设备某些扇区块的数据不再使用（在FF_USE_TRIM == 1时必需） */

/* 通用命令（FatFs不使用） */
#define CTRL_POWER 5  /* 获取/设置电源状态 */
#define CTRL_LOCK 6   /* 锁定/解锁介质移除 */
#define CTRL_EJECT 7  /* 弹出介质 */
#define CTRL_FORMAT 8 /* 在介质上创建物理格式 */

/* MMC/SDC特定的ioctl命令 */
#define MMC_GET_TYPE 10   /* 获取卡类型 */
#define MMC_GET_CSD 11    /* 获取CSD寄存器内容 */
#define MMC_GET_CID 12    /* 获取CID寄存器内容 */
#define MMC_GET_OCR 13    /* 获取OCR寄存器内容 */
#define MMC_GET_SDSTAT 14 /* 获取SD状态 */
#define ISDIO_READ 55     /* 从SD iSDIO寄存器读取数据 */
#define ISDIO_WRITE 56    /* 向SD iSDIO寄存器写入数据 */
#define ISDIO_MRITE 57    /* 向SD iSDIO寄存器掩码写入数据 */

/* ATA/CF特定的ioctl命令 */
#define ATA_GET_REV 20   /* 获取固件版本 */
#define ATA_GET_MODEL 21 /* 获取型号名称 */
#define ATA_GET_SN 22    /* 获取序列号 */

#ifdef __cplusplus
}
#endif

#endif
