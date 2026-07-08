/*----------------------------------------------------------------------------/
/  FatFs - 通用 FAT 文件系统模块 R0.14b                                      /
/-----------------------------------------------------------------------------/
/
/ 版权所有 (C) 2021, ChaN，保留所有权利。
/
/ FatFs 模块是开源软件。允许以源代码和二进制形式重新分发和使用 FatFs，
/ 无论是否修改，前提是满足以下条件：
/
/ 1. 源代码的重新分发必须保留上述版权声明、此条件以及以下免责声明。
/
/ 本软件由版权持有者和贡献者按“原样”提供，不提供任何明示或暗示的保证，
/ 包括但不限于适销性和特定用途适用性的暗示保证。在任何情况下，版权所有者
/ 或贡献者均不对因使用本软件而引起的任何损害负责。
/
/----------------------------------------------------------------------------*/

#include <string.h>
#include "ff.h"     /* FatFs API 声明 */
#include "diskio.h" /* 设备 I/O 函数声明 */

/*--------------------------------------------------------------------------

   模块私有定义

---------------------------------------------------------------------------*/

#if FF_DEFINED != 86631 /* 版本 ID */
    #error 包含错误的头文件 (ff.h)。
#endif

/* 限制和边界 */
#define MAX_DIR 0x200000      /* FAT 目录的最大大小 */
#define MAX_DIR_EX 0x10000000 /* exFAT 目录的最大大小 */
#define MAX_FAT12 0xFF5       /* 最大 FAT12 簇数（与规范略有不同，但符合实际 DOS/Windows 行为） */
#define MAX_FAT16 0xFFF5      /* 最大 FAT16 簇数（与规范略有不同，但符合实际 DOS/Windows 行为） */
#define MAX_FAT32 0x0FFFFFF5  /* 最大 FAT32 簇数（未指定，实际限制） */
#define MAX_EXFAT 0x7FFFFFFD  /* 最大 exFAT 簇数（与规范不同，实现限制） */

/* 字符代码支持宏 */
#define IsUpper(c) ((c) >= 'A' && (c) <= 'Z')                  /* 判断是否为大写字母 */
#define IsLower(c) ((c) >= 'a' && (c) <= 'z')                  /* 判断是否为小写字母 */
#define IsDigit(c) ((c) >= '0' && (c) <= '9')                  /* 判断是否为数字 */
#define IsSeparator(c) ((c) == '/' || (c) == '\\')             /* 判断是否为路径分隔符 */
#define IsTerminator(c) ((UINT)(c) < (FF_USE_LFN ? ' ' : '!')) /* 判断是否为字符串终止符 */
#define IsSurrogate(c) ((c) >= 0xD800 && (c) <= 0xDFFF)        /* 判断是否为 UTF-16 代理项 */
#define IsSurrogateH(c) ((c) >= 0xD800 && (c) <= 0xDBFF)       /* 判断是否为高代理项 */
#define IsSurrogateL(c) ((c) >= 0xDC00 && (c) <= 0xDFFF)       /* 判断是否为低代理项 */

/* 附加的文件访问控制和文件状态标志（内部使用） */
#define FA_SEEKEND 0x20  /* 打开文件时定位到文件末尾 */
#define FA_MODIFIED 0x40 /* 文件已被修改 */
#define FA_DIRTY 0x80    /* FIL.buf[] 需要回写 */

/* 附加的文件属性位（内部使用） */
#define AM_VOL 0x08   /* 卷标 */
#define AM_LFN 0x0F   /* 长文件名条目 */
#define AM_MASK 0x3F  /* FAT 中已定义位的掩码 */
#define AM_MASKX 0x37 /* exFAT 中已定义位的掩码 */

/* fn[11] 中的名称状态标志 */
#define NSFLAG 11      /* 名称状态字节的索引 */
#define NS_LOSS 0x01   /* 不符合 8.3 格式 */
#define NS_LFN 0x02    /* 强制创建 LFN 条目 */
#define NS_LAST 0x04   /* 最后一个段 */
#define NS_BODY 0x08   /* 小写标志（主体） */
#define NS_EXT 0x10    /* 小写标志（扩展名） */
#define NS_DOT 0x20    /* 点条目（. 或 ..） */
#define NS_NOLFN 0x40  /* 不查找 LFN */
#define NS_NONAME 0x80 /* 不跟随 */

/* exFAT 目录条目类型 */
#define ET_BITMAP 0x81   /* 分配位图 */
#define ET_UPCASE 0x82   /* 大写表 */
#define ET_VLABEL 0x83   /* 卷标 */
#define ET_FILEDIR 0x85  /* 文件和目录 */
#define ET_STREAM 0xC0   /* 流扩展 */
#define ET_FILENAME 0xC1 /* 名称扩展 */

/* FatFs 将 FAT 结构视为简单的字节数组，而不是结构成员，
   因为不同平台间的 C 结构二进制兼容性无法保证 */

/* BIOS 参数块 (BPB) 和引导扇区中各字段的偏移 */
#define BS_JmpBoot 0          /* x86 跳转指令（3字节） */
#define BS_OEMName 3          /* OEM 名称（8字节） */
#define BPB_BytsPerSec 11     /* 扇区大小（字节）（WORD） */
#define BPB_SecPerClus 13     /* 每簇扇区数（BYTE） */
#define BPB_RsvdSecCnt 14     /* 保留区大小（扇区）（WORD） */
#define BPB_NumFATs 16        /* FAT 数量（BYTE） */
#define BPB_RootEntCnt 17     /* FAT 根目录项数（WORD） */
#define BPB_TotSec16 19       /* 卷大小（16位，扇区）（WORD） */
#define BPB_Media 21          /* 介质描述字节（BYTE） */
#define BPB_FATSz16 22        /* FAT 大小（16位，扇区）（WORD） */
#define BPB_SecPerTrk 24      /* int13h 每磁道扇区数（扇区）（WORD） */
#define BPB_NumHeads 26       /* int13h 磁头数（WORD） */
#define BPB_HiddSec 28        /* 驱动器起始偏移（扇区）（DWORD） */
#define BPB_TotSec32 32       /* 卷大小（32位，扇区）（DWORD） */
#define BS_DrvNum 36          /* int13h 物理驱动器号（BYTE） */
#define BS_NTres 37           /* WindowsNT 错误标志（BYTE） */
#define BS_BootSig 38         /* 扩展引导签名（BYTE） */
#define BS_VolID 39           /* 卷序列号（DWORD） */
#define BS_VolLab 43          /* 卷标字符串（8字节） */
#define BS_FilSysType 54      /* 文件系统类型字符串（8字节） */
#define BS_BootCode 62        /* 引导代码（448字节） */
#define BS_55AA 510           /* 签名字（WORD） */

#define BPB_FATSz32 36        /* FAT32: FAT 大小（扇区）（DWORD） */
#define BPB_ExtFlags32 40     /* FAT32: 扩展标志（WORD） */
#define BPB_FSVer32 42        /* FAT32: 文件系统版本（WORD） */
#define BPB_RootClus32 44     /* FAT32: 根目录簇（DWORD） */
#define BPB_FSInfo32 48       /* FAT32: FSINFO 扇区偏移（WORD） */
#define BPB_BkBootSec32 50    /* FAT32: 备份引导扇区偏移（WORD） */
#define BS_DrvNum32 64        /* FAT32: int13h 物理驱动器号（BYTE） */
#define BS_NTres32 65         /* FAT32: 错误标志（BYTE） */
#define BS_BootSig32 66       /* FAT32: 扩展引导签名（BYTE） */
#define BS_VolID32 67         /* FAT32: 卷序列号（DWORD） */
#define BS_VolLab32 71        /* FAT32: 卷标字符串（8字节） */
#define BS_FilSysType32 82    /* FAT32: 文件系统类型字符串（8字节） */
#define BS_BootCode32 90      /* FAT32: 引导代码（420字节） */

#define BPB_ZeroedEx 11       /* exFAT: 必须为零的字段（53字节） */
#define BPB_VolOfsEx 64       /* exFAT: 卷在驱动器上的起始偏移（扇区）（QWORD） */
#define BPB_TotSecEx 72       /* exFAT: 卷大小（扇区）（QWORD） */
#define BPB_FatOfsEx 80       /* exFAT: FAT 在卷中的偏移（扇区）（DWORD） */
#define BPB_FatSzEx 84        /* exFAT: FAT 大小（扇区）（DWORD） */
#define BPB_DataOfsEx 88      /* exFAT: 数据区在卷中的偏移（扇区）（DWORD） */
#define BPB_NumClusEx 92      /* exFAT: 簇数量（DWORD） */
#define BPB_RootClusEx 96     /* exFAT: 根目录起始簇（DWORD） */
#define BPB_VolIDEx 100       /* exFAT: 卷序列号（DWORD） */
#define BPB_FSVerEx 104       /* exFAT: 文件系统版本（WORD） */
#define BPB_VolFlagEx 106     /* exFAT: 卷标志（WORD） */
#define BPB_BytsPerSecEx 108  /* exFAT: 以字节为单位的扇区大小的对数（BYTE） */
#define BPB_SecPerClusEx 109  /* exFAT: 以扇区为单位的簇大小的对数（BYTE） */
#define BPB_NumFATsEx 110     /* exFAT: FAT 数量（BYTE） */
#define BPB_DrvNumEx 111      /* exFAT: int13h 物理驱动器号（BYTE） */
#define BPB_PercInUseEx 112   /* exFAT: 使用百分比（BYTE） */
#define BPB_RsvdEx 113        /* exFAT: 保留（7字节） */
#define BS_BootCodeEx 120     /* exFAT: 引导代码（390字节） */

#define DIR_Name 0            /* 短文件名（11字节） */
#define DIR_Attr 11           /* 属性（BYTE） */
#define DIR_NTres 12          /* 小写标志（BYTE） */
#define DIR_CrtTime10 13      /* 创建时间的亚秒（BYTE） */
#define DIR_CrtTime 14        /* 创建时间（DWORD） */
#define DIR_LstAccDate 18     /* 最后访问日期（WORD） */
#define DIR_FstClusHI 20      /* 首簇高16位（WORD） */
#define DIR_ModTime 22        /* 修改时间（DWORD） */
#define DIR_FstClusLO 26      /* 首簇低16位（WORD） */
#define DIR_FileSize 28       /* 文件大小（DWORD） */
#define LDIR_Ord 0            /* LFN: LFN 顺序和 LLE 标志（BYTE） */
#define LDIR_Attr 11          /* LFN: LFN 属性（BYTE） */
#define LDIR_Type 12          /* LFN: 条目类型（BYTE） */
#define LDIR_Chksum 13        /* LFN: SFN 的校验和（BYTE） */
#define LDIR_FstClusLO 26     /* LFN: 必须为零的字段（WORD） */
#define XDIR_Type 0           /* exFAT: exFAT 目录条目类型（BYTE） */
#define XDIR_NumLabel 1       /* exFAT: 卷标字符数（BYTE） */
#define XDIR_Label 2          /* exFAT: 卷标（11-WORD） */
#define XDIR_CaseSum 4        /* exFAT: 大小写转换表总和（DWORD） */
#define XDIR_NumSec 1         /* exFAT: 二级条目数（BYTE） */
#define XDIR_SetSum 2         /* exFAT: 目录条目集总和（WORD） */
#define XDIR_Attr 4           /* exFAT: 文件属性（WORD） */
#define XDIR_CrtTime 8        /* exFAT: 创建时间（DWORD） */
#define XDIR_ModTime 12       /* exFAT: 修改时间（DWORD） */
#define XDIR_AccTime 16       /* exFAT: 最后访问时间（DWORD） */
#define XDIR_CrtTime10 20     /* exFAT: 创建时间亚秒（BYTE） */
#define XDIR_ModTime10 21     /* exFAT: 修改时间亚秒（BYTE） */
#define XDIR_CrtTZ 22         /* exFAT: 创建时区（BYTE） */
#define XDIR_ModTZ 23         /* exFAT: 修改时区（BYTE） */
#define XDIR_AccTZ 24         /* exFAT: 最后访问时区（BYTE） */
#define XDIR_GenFlags 33      /* exFAT: 通用二级标志（BYTE） */
#define XDIR_NumName 35       /* exFAT: 文件名字符数（BYTE） */
#define XDIR_NameHash 36      /* exFAT: 文件名的哈希值（WORD） */
#define XDIR_ValidFileSize 40 /* exFAT: 有效文件大小（QWORD） */
#define XDIR_FstClus 52       /* exFAT: 文件数据的首簇（DWORD） */
#define XDIR_FileSize 56      /* exFAT: 文件/目录大小（QWORD） */

#define SZDIRE 32             /* 目录条目大小 */
#define DDEM 0xE5             /* 已删除的目录条目标记，置于 DIR_Name[0] */
#define RDDEM 0x05            /* 与 DDEM 冲突的字符的替代 */
#define LLEF 0x40             /* LDIR_Ord 中的最后长条目标志 */

#define FSI_LeadSig 0         /* FAT32 FSI: 引导签名（DWORD） */
#define FSI_StrucSig 484      /* FAT32 FSI: 结构签名（DWORD） */
#define FSI_Free_Count 488    /* FAT32 FSI: 空闲簇数（DWORD） */
#define FSI_Nxt_Free 492      /* FAT32 FSI: 最后分配的簇（DWORD） */

#define MBR_Table 446         /* MBR: 分区表在 MBR 中的偏移 */
#define SZ_PTE 16             /* MBR: 分区表项的大小 */
#define PTE_Boot 0            /* MBR PTE: 引导指示符 */
#define PTE_StHead 1          /* MBR PTE: 起始磁头 */
#define PTE_StSec 2           /* MBR PTE: 起始扇区 */
#define PTE_StCyl 3           /* MBR PTE: 起始柱面 */
#define PTE_System 4          /* MBR PTE: 系统 ID */
#define PTE_EdHead 5          /* MBR PTE: 结束磁头 */
#define PTE_EdSec 6           /* MBR PTE: 结束扇区 */
#define PTE_EdCyl 7           /* MBR PTE: 结束柱面 */
#define PTE_StLba 8           /* MBR PTE: LBA 起始地址 */
#define PTE_SizLba 12         /* MBR PTE: LBA 大小 */

#define GPTH_Sign 0           /* GPT: 头签名（8字节） */
#define GPTH_Rev 8            /* GPT: 修订版本（DWORD） */
#define GPTH_Size 12          /* GPT: 头大小（DWORD） */
#define GPTH_Bcc 16           /* GPT: 头 BCC（DWORD） */
#define GPTH_CurLba 24        /* GPT: 主头 LBA（QWORD） */
#define GPTH_BakLba 32        /* GPT: 备份头 LBA（QWORD） */
#define GPTH_FstLba 40        /* GPT: 第一个可分配 LBA（QWORD） */
#define GPTH_LstLba 48        /* GPT: 最后一个可分配 LBA（QWORD） */
#define GPTH_DskGuid 56       /* GPT: 磁盘 GUID（16字节） */
#define GPTH_PtOfs 72         /* GPT: 分区表 LBA（QWORD） */
#define GPTH_PtNum 80         /* GPT: 分区表项数（DWORD） */
#define GPTH_PteSize 84       /* GPT: 分区表项大小（DWORD） */
#define GPTH_PtBcc 88         /* GPT: 分区表 BCC（DWORD） */
#define SZ_GPTE 128           /* GPT: 分区表项大小 */
#define GPTE_PtGuid 0         /* GPT PTE: 分区类型 GUID（16字节） */
#define GPTE_UpGuid 16        /* GPT PTE: 分区唯一 GUID（16字节） */
#define GPTE_FstLba 32        /* GPT PTE: 起始 LBA（QWORD） */
#define GPTE_LstLba 40        /* GPT PTE: 结束 LBA（包含）（QWORD） */
#define GPTE_Flags 48         /* GPT PTE: 标志（QWORD） */
#define GPTE_Name 56          /* GPT PTE: 名称 */

/* 文件操作中发生致命错误时的后处理 */
#define ABORT(fs, res)         \
    {                          \
        fp->err = (BYTE)(res); \
        LEAVE_FF(fs, res);     \
    }

/* 可重入相关 */
#if FF_FS_REENTRANT
    #if FF_USE_LFN == 1
        #error 在支持多线程的配置中不能使用静态 LFN 工作区
    #endif
    #define LEAVE_FF(fs, res)   \
        {                       \
            unlock_fs(fs, res); \
            return res;         \
        }
#else
    #define LEAVE_FF(fs, res) return res
#endif

/* 逻辑驱动器到物理位置的转换定义 */
#if FF_MULTI_PARTITION
    #define LD2PD(vol) VolToPart[vol].pd /* 获取物理驱动器号 */
    #define LD2PT(vol) VolToPart[vol].pt /* 获取分区号（0：自动搜索，1..：强制分区号） */
#else
    #define LD2PD(vol) (BYTE)(vol)       /* 每个逻辑驱动器关联到相同的物理驱动器号 */
    #define LD2PT(vol) 0                 /* 自动搜索分区 */
#endif

/* 扇区大小定义 */
#if (FF_MAX_SS < FF_MIN_SS) || (FF_MAX_SS != 512 && FF_MAX_SS != 1024 && FF_MAX_SS != 2048 && FF_MAX_SS != 4096) || (FF_MIN_SS != 512 && FF_MIN_SS != 1024 && FF_MIN_SS != 2048 && FF_MIN_SS != 4096)
    #error 错误的扇区大小配置
#endif
#if FF_MAX_SS == FF_MIN_SS
    #define SS(fs) ((UINT)FF_MAX_SS) /* 固定扇区大小 */
#else
    #define SS(fs) ((fs)->ssize)     /* 可变扇区大小 */
#endif

/* 时间戳 */
#if FF_FS_NORTC == 1
    #if FF_NORTC_YEAR < 1980 || FF_NORTC_YEAR > 2107 || FF_NORTC_MON < 1 || FF_NORTC_MON > 12 || FF_NORTC_MDAY < 1 || FF_NORTC_MDAY > 31
        #error 无效的 FF_FS_NORTC 设置
    #endif
    #define GET_FATTIME() ((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16)
#else
    #define GET_FATTIME() get_fattime()
#endif

/* 文件锁控制 */
#if FF_FS_LOCK != 0
    #if FF_FS_READONLY
        #error 在只读配置下 FF_FS_LOCK 必须为 0
    #endif
typedef struct
{
    FATFS *fs; /* 对象 ID 1，卷（NULL：空条目） */
    DWORD clu; /* 对象 ID 2，包含目录（0：根目录） */
    DWORD ofs; /* 对象 ID 3，目录中的偏移 */
    WORD ctr;  /* 对象打开计数器，0：无，0x01..0xFF：读模式打开计数，0x100：写模式 */
} FILESEM;
#endif

/* SBCS 大写表（\x80-\xFF） */
#define TBL_CT437 {0x80, 0x9A, 0x45, 0x41, 0x8E, 0x41, 0x8F, 0x80, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x8E, 0x8F, \
                   0x90, 0x92, 0x92, 0x4F, 0x99, 0x4F, 0x55, 0x55, 0x59, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0x41, 0x49, 0x4F, 0x55, 0xA5, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT720 {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT737 {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x92, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, \
                   0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0xAA, 0x92, 0x93, 0x94, 0x95, 0x96, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0x97, 0xEA, 0xEB, 0xEC, 0xE4, 0xED, 0xEE, 0xEF, 0xF5, 0xF0, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT771 {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDC, 0xDE, 0xDE, \
                   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0xF0, 0xF0, 0xF2, 0xF2, 0xF4, 0xF4, 0xF6, 0xF6, 0xF8, 0xF8, 0xFA, 0xFA, 0xFC, 0xFC, 0xFE, 0xFF}
#define TBL_CT775 {0x80, 0x9A, 0x91, 0xA0, 0x8E, 0x95, 0x8F, 0x80, 0xAD, 0xED, 0x8A, 0x8A, 0xA1, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x92, 0x92, 0xE2, 0x99, 0x95, 0x96, 0x97, 0x97, 0x99, 0x9A, 0x9D, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0xA0, 0xA1, 0xE0, 0xA3, 0xA3, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xB5, 0xB6, 0xB7, 0xB8, 0xBD, 0xBE, 0xC6, 0xC7, 0xA5, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE5, 0xE5, 0xE6, 0xE3, 0xE8, 0xE8, 0xEA, 0xEA, 0xEE, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT850 {0x43, 0x55, 0x45, 0x41, 0x41, 0x41, 0x41, 0x43, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x41, 0x41, \
                   0x45, 0x92, 0x92, 0x4F, 0x4F, 0x4F, 0x55, 0x55, 0x59, 0x4F, 0x55, 0x4F, 0x9C, 0x4F, 0x9E, 0x9F, \
                   0x41, 0x49, 0x4F, 0x55, 0xA5, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0x41, 0x41, 0x41, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0x41, 0x41, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD1, 0xD1, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x49, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0x49, 0xDF, \
                   0x4F, 0xE1, 0x4F, 0x4F, 0x4F, 0x4F, 0xE6, 0xE8, 0xE8, 0x55, 0x55, 0x55, 0x59, 0x59, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT852 {0x80, 0x9A, 0x90, 0xB6, 0x8E, 0xDE, 0x8F, 0x80, 0x9D, 0xD3, 0x8A, 0x8A, 0xD7, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x91, 0x91, 0xE2, 0x99, 0x95, 0x95, 0x97, 0x97, 0x99, 0x9A, 0x9B, 0x9B, 0x9D, 0x9E, 0xAC, \
                   0xB5, 0xD6, 0xE0, 0xE9, 0xA4, 0xA4, 0xA6, 0xA6, 0xA8, 0xA8, 0xAA, 0x8D, 0xAC, 0xB8, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBD, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC6, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD1, 0xD1, 0xD2, 0xD3, 0xD2, 0xD5, 0xD6, 0xD7, 0xB7, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE3, 0xD5, 0xE6, 0xE6, 0xE8, 0xE9, 0xE8, 0xEB, 0xED, 0xED, 0xDD, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xEB, 0xFC, 0xFC, 0xFE, 0xFF}
#define TBL_CT855 {0x81, 0x81, 0x83, 0x83, 0x85, 0x85, 0x87, 0x87, 0x89, 0x89, 0x8B, 0x8B, 0x8D, 0x8D, 0x8F, 0x8F, \
                   0x91, 0x91, 0x93, 0x93, 0x95, 0x95, 0x97, 0x97, 0x99, 0x99, 0x9B, 0x9B, 0x9D, 0x9D, 0x9F, 0x9F, \
                   0xA1, 0xA1, 0xA3, 0xA3, 0xA5, 0xA5, 0xA7, 0xA7, 0xA9, 0xA9, 0xAB, 0xAB, 0xAD, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB6, 0xB6, 0xB8, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBE, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC7, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD1, 0xD1, 0xD3, 0xD3, 0xD5, 0xD5, 0xD7, 0xD7, 0xDD, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xE0, 0xDF, \
                   0xE0, 0xE2, 0xE2, 0xE4, 0xE4, 0xE6, 0xE6, 0xE8, 0xE8, 0xEA, 0xEA, 0xEC, 0xEC, 0xEE, 0xEE, 0xEF, \
                   0xF0, 0xF2, 0xF2, 0xF4, 0xF4, 0xF6, 0xF6, 0xF8, 0xF8, 0xFA, 0xFA, 0xFC, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT857 {0x80, 0x9A, 0x90, 0xB6, 0x8E, 0xB7, 0x8F, 0x80, 0xD2, 0xD3, 0xD4, 0xD8, 0xD7, 0x49, 0x8E, 0x8F, \
                   0x90, 0x92, 0x92, 0xE2, 0x99, 0xE3, 0xEA, 0xEB, 0x98, 0x99, 0x9A, 0x9D, 0x9C, 0x9D, 0x9E, 0x9E, \
                   0xB5, 0xD6, 0xE0, 0xE9, 0xA5, 0xA5, 0xA6, 0xA6, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC7, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0x49, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE5, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xDE, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT860 {0x80, 0x9A, 0x90, 0x8F, 0x8E, 0x91, 0x86, 0x80, 0x89, 0x89, 0x92, 0x8B, 0x8C, 0x98, 0x8E, 0x8F, \
                   0x90, 0x91, 0x92, 0x8C, 0x99, 0xA9, 0x96, 0x9D, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0x86, 0x8B, 0x9F, 0x96, 0xA5, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT861 {0x80, 0x9A, 0x90, 0x41, 0x8E, 0x41, 0x8F, 0x80, 0x45, 0x45, 0x45, 0x8B, 0x8B, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x92, 0x92, 0x4F, 0x99, 0x8D, 0x55, 0x97, 0x97, 0x99, 0x9A, 0x9D, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0xA4, 0xA5, 0xA6, 0xA7, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT862 {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0x41, 0x49, 0x4F, 0x55, 0xA5, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT863 {0x43, 0x55, 0x45, 0x41, 0x41, 0x41, 0x86, 0x43, 0x45, 0x45, 0x45, 0x49, 0x49, 0x8D, 0x41, 0x8F, \
                   0x45, 0x45, 0x45, 0x4F, 0x45, 0x49, 0x55, 0x55, 0x98, 0x4F, 0x55, 0x9B, 0x9C, 0x55, 0x55, 0x9F, \
                   0xA0, 0xA1, 0x4F, 0x55, 0xA4, 0xA5, 0xA6, 0xA7, 0x49, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT864 {0x80, 0x9A, 0x45, 0x41, 0x8E, 0x41, 0x8F, 0x80, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x8E, 0x8F, \
                   0x90, 0x92, 0x92, 0x4F, 0x99, 0x4F, 0x55, 0x55, 0x59, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0x41, 0x49, 0x4F, 0x55, 0xA5, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT865 {0x80, 0x9A, 0x90, 0x41, 0x8E, 0x41, 0x8F, 0x80, 0x45, 0x45, 0x45, 0x49, 0x49, 0x49, 0x8E, 0x8F, \
                   0x90, 0x92, 0x92, 0x4F, 0x99, 0x4F, 0x55, 0x55, 0x59, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0x41, 0x49, 0x4F, 0x55, 0xA5, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, \
                   0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT866 {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF, \
                   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, \
                   0xF0, 0xF0, 0xF2, 0xF2, 0xF4, 0xF4, 0xF6, 0xF6, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF}
#define TBL_CT869 {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, \
                   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x86, 0x9C, 0x8D, 0x8F, 0x90, \
                   0x91, 0x90, 0x92, 0x95, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, \
                   0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, \
                   0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, \
                   0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xA4, 0xA5, 0xA6, 0xD9, 0xDA, 0xDB, 0xDC, 0xA7, 0xA8, 0xDF, \
                   0xA9, 0xAA, 0xAC, 0xAD, 0xB5, 0xB6, 0xB7, 0xB8, 0xBD, 0xBE, 0xC6, 0xC7, 0xCF, 0xCF, 0xD0, 0xEF, \
                   0xF0, 0xF1, 0xD1, 0xD2, 0xD3, 0xF5, 0xD4, 0xF7, 0xF8, 0xF9, 0xD5, 0x96, 0x95, 0x98, 0xFE, 0xFF}


/* DBCS code range |----- 1st byte -----|  |----------- 2nd byte -----------| */
/*                  <------>    <------>    <------>    <------>    <------>  */
#define TBL_DC932 {0x81, 0x9F, 0xE0, 0xFC, 0x40, 0x7E, 0x80, 0xFC, 0x00, 0x00}
#define TBL_DC936 {0x81, 0xFE, 0x00, 0x00, 0x40, 0x7E, 0x80, 0xFE, 0x00, 0x00}
#define TBL_DC949 {0x81, 0xFE, 0x00, 0x00, 0x41, 0x5A, 0x61, 0x7A, 0x81, 0xFE}
#define TBL_DC950 {0x81, 0xFE, 0x00, 0x00, 0x40, 0x7E, 0xA1, 0xFE, 0x00, 0x00}


/* Macros for table definitions */
#define MERGE_2STR(a, b) a##b
#define MKCVTBL(hd, cp) MERGE_2STR(hd, cp)

/*--------------------------------------------------------------------------

   模块私有工作区

---------------------------------------------------------------------------*/
/* 注：此处定义的未初始化的变量在启动时必须为零/空。如果不是，则链接器选项或启动例程不符合 C 标准。 */

/*--------------------------------*/
/* 文件/卷控制                    */
/*--------------------------------*/

#if FF_VOLUMES < 1 || FF_VOLUMES > 10
    #error 错误的 FF_VOLUMES 设置
#endif
static FATFS *FatFs[FF_VOLUMES]; /* 指向文件系统对象的指针（逻辑驱动器） */
static WORD Fsid;                /* 文件系统挂载 ID */

#if FF_FS_RPATH != 0
static BYTE CurrVol; /* 当前驱动器 */
#endif

#if FF_FS_LOCK != 0
static FILESEM Files[FF_FS_LOCK]; /* 打开对象锁信号量 */
#endif

#if FF_STR_VOLUME_ID
    #ifdef FF_VOLUME_STRS
static const char *const VolumeStr[FF_VOLUMES] = {FF_VOLUME_STRS}; /* 预定义的卷 ID */
    #endif
#endif

#if FF_LBA64
    #if FF_MIN_GPT > 0x100000000
        #error 错误的 FF_MIN_GPT 设置
    #endif
static const BYTE GUID_MS_Basic[16] = {0xA2, 0xA0, 0xD0, 0xEB, 0xE5, 0xB9, 0x33, 0x44, 0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7};
#endif

/*--------------------------------*/
/* LFN/目录工作缓冲区              */
/*--------------------------------*/

#if FF_USE_LFN == 0 /* 非 LFN 配置 */
    #if FF_FS_EXFAT
        #error 启用 exFAT 时必须启用 LFN
    #endif
    #define DEF_NAMBUF
    #define INIT_NAMBUF(fs)
    #define FREE_NAMBUF()
    #define LEAVE_MKFS(res) return res

#else /* LFN 配置 */
    #if FF_MAX_LFN < 12 || FF_MAX_LFN > 255
        #error 错误的 FF_MAX_LFN 设置
    #endif
    #if FF_LFN_BUF < FF_SFN_BUF || FF_SFN_BUF < 12
        #error 错误的 FF_LFN_BUF 或 FF_SFN_BUF 设置
    #endif
    #if FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3
        #error 错误的 FF_LFN_UNICODE 设置
    #endif
static const BYTE LfnOfs[] = {1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30}; /* FAT: LFN 字符在目录条目中的偏移 */
    #define MAXDIRB(nc) ((nc + 44U) / 15 * SZDIRE) /* exFAT: 所需名称长度的目录项块暂存缓冲区大小 */

    #if FF_USE_LFN == 1                            /* 启用 LFN，使用静态工作缓冲区 */
        #if FF_FS_EXFAT
static BYTE DirBuf[MAXDIRB(FF_MAX_LFN)]; /* 目录项块暂存缓冲区 */
        #endif
static WCHAR LfnBuf[FF_MAX_LFN + 1];     /* LFN 工作缓冲区 */
        #define DEF_NAMBUF
        #define INIT_NAMBUF(fs)
        #define FREE_NAMBUF()
        #define LEAVE_MKFS(res) return res

    #elif FF_USE_LFN == 2 /* 启用 LFN，使用栈上的动态工作缓冲区 */
        #if FF_FS_EXFAT
            #define DEF_NAMBUF              \
                WCHAR lbuf[FF_MAX_LFN + 1]; \
                BYTE dbuf[MAXDIRB(FF_MAX_LFN)]; /* LFN 工作缓冲区和目录项块暂存缓冲区 */
            #define INIT_NAMBUF(fs)      \
                {                        \
                    (fs)->lfnbuf = lbuf; \
                    (fs)->dirbuf = dbuf; \
                }
            #define FREE_NAMBUF()
        #else
            #define DEF_NAMBUF WCHAR lbuf[FF_MAX_LFN + 1]; /* LFN 工作缓冲区 */
            #define INIT_NAMBUF(fs)      \
                {                        \
                    (fs)->lfnbuf = lbuf; \
                }
            #define FREE_NAMBUF()
        #endif
        #define LEAVE_MKFS(res) return res

    #elif FF_USE_LFN == 3                  /* 启用 LFN，使用堆上的动态工作缓冲区 */
        #if FF_FS_EXFAT
            #define DEF_NAMBUF WCHAR *lfn; /* 指向 LFN 工作缓冲区和目录项块暂存缓冲区的指针 */
            #define INIT_NAMBUF(fs)                                                \
                {                                                                  \
                    lfn = ff_memalloc((FF_MAX_LFN + 1) * 2 + MAXDIRB(FF_MAX_LFN)); \
                    if (!lfn)                                                      \
                        LEAVE_FF(fs, FR_NOT_ENOUGH_CORE);                          \
                    (fs)->lfnbuf = lfn;                                            \
                    (fs)->dirbuf = (BYTE *)(lfn + FF_MAX_LFN + 1);                 \
                }
            #define FREE_NAMBUF() ff_memfree(lfn)
        #else
            #define DEF_NAMBUF WCHAR *lfn; /* 指向 LFN 工作缓冲区的指针 */
            #define INIT_NAMBUF(fs)                          \
                {                                            \
                    lfn = ff_memalloc((FF_MAX_LFN + 1) * 2); \
                    if (!lfn)                                \
                        LEAVE_FF(fs, FR_NOT_ENOUGH_CORE);    \
                    (fs)->lfnbuf = lfn;                      \
                }
            #define FREE_NAMBUF() ff_memfree(lfn)
        #endif
        #define LEAVE_MKFS(res)      \
            {                        \
                if (!work)           \
                    ff_memfree(buf); \
                return res;          \
            }
        #define MAX_MALLOC 0x8000 /* 必须 >= FF_MAX_SS */

    #else
        #error 错误的 FF_USE_LFN 设置

    #endif /* FF_USE_LFN == 1 */
#endif     /* FF_USE_LFN == 0 */

/*--------------------------------*/
/* 代码转换表                     */
/*--------------------------------*/

#if FF_CODE_PAGE == 0              /* 运行时代码页配置 */
    #define CODEPAGE CodePage
static WORD CodePage;              /* 当前代码页 */
static const BYTE *ExCvt, *DbcTbl; /* 指向当前 SBCS 大写表和 DBCS 代码范围表的指针 */

static const BYTE Ct437[] = TBL_CT437;
static const BYTE Ct720[] = TBL_CT720;
static const BYTE Ct737[] = TBL_CT737;
static const BYTE Ct771[] = TBL_CT771;
static const BYTE Ct775[] = TBL_CT775;
static const BYTE Ct850[] = TBL_CT850;
static const BYTE Ct852[] = TBL_CT852;
static const BYTE Ct855[] = TBL_CT855;
static const BYTE Ct857[] = TBL_CT857;
static const BYTE Ct860[] = TBL_CT860;
static const BYTE Ct861[] = TBL_CT861;
static const BYTE Ct862[] = TBL_CT862;
static const BYTE Ct863[] = TBL_CT863;
static const BYTE Ct864[] = TBL_CT864;
static const BYTE Ct865[] = TBL_CT865;
static const BYTE Ct866[] = TBL_CT866;
static const BYTE Ct869[] = TBL_CT869;
static const BYTE Dc932[] = TBL_DC932;
static const BYTE Dc936[] = TBL_DC936;
static const BYTE Dc949[] = TBL_DC949;
static const BYTE Dc950[] = TBL_DC950;

#elif FF_CODE_PAGE < 900 /* 静态代码页配置（SBCS） */
    #define CODEPAGE FF_CODE_PAGE
static const BYTE ExCvt[] = MKCVTBL(TBL_CT, FF_CODE_PAGE);

#else /* 静态代码页配置（DBCS） */
    #define CODEPAGE FF_CODE_PAGE
static const BYTE DbcTbl[] = MKCVTBL(TBL_DC, FF_CODE_PAGE);

#endif

/*--------------------------------------------------------------------------

   模块私有函数

---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* 在 FAT 结构中加载/存储多字节字                                        */
/*-----------------------------------------------------------------------*/

/* 加载一个2字节小端序的字 */
static WORD ld_word(const BYTE *ptr)
{
    WORD rv;
    rv = ptr[1];
    rv = rv << 8 | ptr[0];
    return rv;
}

/* 加载一个4字节小端序的双字 */
static DWORD ld_dword(const BYTE *ptr)
{
    DWORD rv;
    rv = ptr[3];
    rv = rv << 8 | ptr[2];
    rv = rv << 8 | ptr[1];
    rv = rv << 8 | ptr[0];
    return rv;
}

#if FF_FS_EXFAT
/* 加载一个8字节小端序的四字（仅 exFAT） */
static QWORD ld_qword(const BYTE *ptr)
{
    QWORD rv;
    rv = ptr[7];
    rv = rv << 8 | ptr[6];
    rv = rv << 8 | ptr[5];
    rv = rv << 8 | ptr[4];
    rv = rv << 8 | ptr[3];
    rv = rv << 8 | ptr[2];
    rv = rv << 8 | ptr[1];
    rv = rv << 8 | ptr[0];
    return rv;
}
#endif

#if !FF_FS_READONLY
/* 存储一个2字节小端序的字 */
static void st_word(BYTE *ptr, WORD val)
{
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
}

/* 存储一个4字节小端序的双字 */
static void st_dword(BYTE *ptr, DWORD val)
{
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
}

    #if FF_FS_EXFAT
/* 存储一个8字节小端序的四字（仅 exFAT） */
static void st_qword(BYTE *ptr, QWORD val)
{
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
    val >>= 8;
    *ptr++ = (BYTE)val;
}
    #endif
#endif /* !FF_FS_READONLY */

/*-----------------------------------------------------------------------*/
/* 字符串函数                                                            */
/*-----------------------------------------------------------------------*/

/* 测试字节是否为 DBCS 首字节 */
static int dbc_1st(BYTE c)
{
#if FF_CODE_PAGE == 0 /* 可变代码页 */
    if (DbcTbl && c >= DbcTbl[0])
    {
        if (c <= DbcTbl[1])
            return 1;     /* 首字节范围1 */
        if (c >= DbcTbl[2] && c <= DbcTbl[3])
            return 1;     /* 首字节范围2 */
    }
#elif FF_CODE_PAGE >= 900 /* 固定 DBCS 代码页 */
    if (c >= DbcTbl[0])
    {
        if (c <= DbcTbl[1])
            return 1;
        if (c >= DbcTbl[2] && c <= DbcTbl[3])
            return 1;
    }
#else                     /* 固定 SBCS 代码页 */
    if (c != 0)
        return 0; /* 始终为假 */
#endif
    return 0;
}

/* 测试字节是否为 DBCS 次字节 */
static int dbc_2nd(BYTE c)
{
#if FF_CODE_PAGE == 0 /* 可变代码页 */
    if (DbcTbl && c >= DbcTbl[4])
    {
        if (c <= DbcTbl[5])
            return 1;     /* 次字节范围1 */
        if (c >= DbcTbl[6] && c <= DbcTbl[7])
            return 1;     /* 次字节范围2 */
        if (c >= DbcTbl[8] && c <= DbcTbl[9])
            return 1;     /* 次字节范围3 */
    }
#elif FF_CODE_PAGE >= 900 /* 固定 DBCS 代码页 */
    if (c >= DbcTbl[4])
    {
        if (c <= DbcTbl[5])
            return 1;
        if (c >= DbcTbl[6] && c <= DbcTbl[7])
            return 1;
        if (c >= DbcTbl[8] && c <= DbcTbl[9])
            return 1;
    }
#else                     /* 固定 SBCS 代码页 */
    if (c != 0)
        return 0; /* 始终为假 */
#endif
    return 0;
}

#if FF_USE_LFN

/* 从 TCHAR 字符串（按定义 API 编码）中获取一个 Unicode 码点 */
/* 返回值：UTF-16 编码的字符（>=0x10000 时为代理对，0xFFFFFFFF 表示解码错误） */
static DWORD tchar2uni(const TCHAR **str)
{
    DWORD uc;
    const TCHAR *p = *str;

    #if FF_LFN_UNICODE == 1 /* UTF-16 输入 */
    WCHAR wc;

    uc = *p++;                 /* 获取一个单元 */
    if (IsSurrogate(uc))
    {                          /* 是否为代理项？ */
        wc = *p++;             /* 获取低代理项 */
        if (!IsSurrogateH(uc) || !IsSurrogateL(wc))
            return 0xFFFFFFFF; /* 错误的代理项？ */
        uc = uc << 16 | wc;
    }

    #elif FF_LFN_UNICODE == 2 /* UTF-8 输入 */
    BYTE b;
    int nf;

    uc = (BYTE)*p++; /* 获取一个编码单元 */
    if (uc & 0x80)
    {                /* 多字节编码？ */
        if ((uc & 0xE0) == 0xC0)
        {            /* 2字节序列？ */
            uc &= 0x1F;
            nf = 1;
        }
        else if ((uc & 0xF0) == 0xE0)
        { /* 3字节序列？ */
            uc &= 0x0F;
            nf = 2;
        }
        else if ((uc & 0xF8) == 0xF0)
        { /* 4字节序列？ */
            uc &= 0x07;
            nf = 3;
        }
        else
        { /* 错误序列 */
            return 0xFFFFFFFF;
        }
        do
        { /* 获取后续字节 */
            b = (BYTE)*p++;
            if ((b & 0xC0) != 0x80)
                return 0xFFFFFFFF; /* 错误的序列？ */
            uc = uc << 6 | (b & 0x3F);
        } while (--nf != 0);
        if (uc < 0x80 || IsSurrogate(uc) || uc >= 0x110000)
            return 0xFFFFFFFF;                                                  /* 错误的码点？ */
        if (uc >= 0x010000)
            uc = 0xD800DC00 | ((uc - 0x10000) << 6 & 0x3FF0000) | (uc & 0x3FF); /* 需要时构造代理对 */
    }

    #elif FF_LFN_UNICODE == 3 /* UTF-32 输入 */
    uc = (TCHAR)*p++;                                                       /* 获取一个单元 */
    if (uc >= 0x110000 || IsSurrogate(uc))
        return 0xFFFFFFFF;                                                  /* 错误的码点？ */
    if (uc >= 0x010000)
        uc = 0xD800DC00 | ((uc - 0x10000) << 6 & 0x3FF0000) | (uc & 0x3FF); /* 需要时构造代理对 */

    #else                     /* ANSI/OEM 输入 */
    BYTE b;
    WCHAR wc;

    wc = (BYTE)*p++;           /* 获取一个字节 */
    if (dbc_1st((BYTE)wc))
    {                          /* 是否为 DBCS 首字节？ */
        b = (BYTE)*p++;        /* 获取次字节 */
        if (!dbc_2nd(b))
            return 0xFFFFFFFF; /* 无效代码？ */
        wc = (wc << 8) + b;    /* 组成 DBCS 字符 */
    }
    if (wc != 0)
    {
        wc = ff_oem2uni(wc, CODEPAGE); /* ANSI/OEM ==> Unicode */
        if (wc == 0)
            return 0xFFFFFFFF;         /* 无效代码？ */
    }
    uc = wc;

    #endif
    *str = p; /* 更新读取指针 */
    return uc;
}

/* 在定义的 API 编码中存储一个 Unicode 字符 */
/* 返回值：写入的编码单元数（0：缓冲区溢出或错误编码） */
static UINT put_utf(DWORD chr, TCHAR *buf, UINT szb)
{
    #if FF_LFN_UNICODE == 1 /* UTF-16 输出 */
    WCHAR hs, wc;

    hs = (WCHAR)(chr >> 16);
    wc = (WCHAR)chr;
    if (hs == 0)
    {                 /* 单个编码单元？ */
        if (szb < 1 || IsSurrogate(wc))
            return 0; /* 缓冲区溢出或错误的编码？ */
        *buf = wc;
        return 1;
    }
    if (szb < 2 || !IsSurrogateH(hs) || !IsSurrogateL(wc))
        return 0; /* 缓冲区溢出或错误的代理对？ */
    *buf++ = hs;
    *buf++ = wc;
    return 2;

    #elif FF_LFN_UNICODE == 2 /* UTF-8 输出 */
    DWORD hc;

    if (chr < 0x80)
    {                 /* 单字节编码？ */
        if (szb < 1)
            return 0; /* 缓冲区溢出？ */
        *buf = (TCHAR)chr;
        return 1;
    }
    if (chr < 0x800)
    {                 /* 2字节序列？ */
        if (szb < 2)
            return 0; /* 缓冲区溢出？ */
        *buf++ = (TCHAR)(0xC0 | (chr >> 6 & 0x1F));
        *buf++ = (TCHAR)(0x80 | (chr >> 0 & 0x3F));
        return 2;
    }
    if (chr < 0x10000)
    {                 /* 3字节序列？ */
        if (szb < 3 || IsSurrogate(chr))
            return 0; /* 缓冲区溢出或错误的编码？ */
        *buf++ = (TCHAR)(0xE0 | (chr >> 12 & 0x0F));
        *buf++ = (TCHAR)(0x80 | (chr >> 6 & 0x3F));
        *buf++ = (TCHAR)(0x80 | (chr >> 0 & 0x3F));
        return 3;
    }
    /* 4字节序列 */
    if (szb < 4)
        return 0;                                /* 缓冲区溢出？ */
    hc = ((chr & 0xFFFF0000) - 0xD8000000) >> 6; /* 获取高10位 */
    chr = (chr & 0xFFFF) - 0xDC00;               /* 获取低10位 */
    if (hc >= 0x100000 || chr >= 0x400)
        return 0;                                /* 错误的代理对？ */
    chr = (hc | chr) + 0x10000;
    *buf++ = (TCHAR)(0xF0 | (chr >> 18 & 0x07));
    *buf++ = (TCHAR)(0x80 | (chr >> 12 & 0x3F));
    *buf++ = (TCHAR)(0x80 | (chr >> 6 & 0x3F));
    *buf++ = (TCHAR)(0x80 | (chr >> 0 & 0x3F));
    return 4;

    #elif FF_LFN_UNICODE == 3 /* UTF-32 输出 */
    DWORD hc;

    if (szb < 1)
        return 0;                                    /* 缓冲区溢出？ */
    if (chr >= 0x10000)
    {                                                /* 超出 BMP？ */
        hc = ((chr & 0xFFFF0000) - 0xD8000000) >> 6; /* 获取高10位 */
        chr = (chr & 0xFFFF) - 0xDC00;               /* 获取低10位 */
        if (hc >= 0x100000 || chr >= 0x400)
            return 0;                                /* 错误的代理对？ */
        chr = (hc | chr) + 0x10000;
    }
    *buf++ = (TCHAR)chr;
    return 1;

    #else                     /* ANSI/OEM 输出 */
    WCHAR wc;

    wc = ff_uni2oem(chr, CODEPAGE);
    if (wc >= 0x100)
    { /* 是否为 DBCS 字符？ */
        if (szb < 2)
            return 0;
        *buf++ = (char)(wc >> 8); /* 存储 DBCS 首字节 */
        *buf++ = (TCHAR)wc;       /* 存储 DBCS 次字节 */
        return 2;
    }
    if (wc == 0 || szb < 1)
        return 0;       /* 无效字符或缓冲区溢出？ */
    *buf++ = (TCHAR)wc; /* 存储字符 */
    return 1;
    #endif
}
#endif /* FF_USE_LFN */

#if FF_FS_REENTRANT
/*-----------------------------------------------------------------------*/
/* 请求/释放访问卷的权限                                                  */
/*-----------------------------------------------------------------------*/
/* 锁定文件系统对象，返回1表示成功，0表示超时 */
static int lock_fs(FATFS *fs)
{
    return ff_req_grant(fs->sobj);
}

/* 解锁文件系统对象 */
static void unlock_fs(FATFS *fs, FRESULT res)
{
    if (fs && res != FR_NOT_ENABLED && res != FR_INVALID_DRIVE && res != FR_TIMEOUT)
    {
        ff_rel_grant(fs->sobj);
    }
}
#endif

#if FF_FS_LOCK != 0
/*-----------------------------------------------------------------------*/
/* 文件锁控制函数                                                        */
/*-----------------------------------------------------------------------*/

/* 检查文件是否可以被访问 */
static FRESULT chk_lock(DIR *dp, int acc)
{
    UINT i, be;

    /* 在打开对象表中搜索该对象 */
    be = 0;
    for (i = 0; i < FF_FS_LOCK; i++)
    {
        if (Files[i].fs)
        {                                    /* 已有条目 */
            if (Files[i].fs == dp->obj.fs && /* 检查是否与已打开对象匹配 */
                Files[i].clu == dp->obj.sclust &&
                Files[i].ofs == dp->dptr)
                break;
        }
        else
        { /* 空白条目 */
            be = 1;
        }
    }
    if (i == FF_FS_LOCK)
    {                                                              /* 对象尚未打开 */
        return (!be && acc != 2) ? FR_TOO_MANY_OPEN_FILES : FR_OK; /* 是否有空白条目供新对象使用？ */
    }

    /* 对象已打开。拒绝任何对正在写入的文件的打开操作以及所有写模式打开 */
    return (acc != 0 || Files[i].ctr == 0x100) ? FR_LOCKED : FR_OK;
}

/* 检查是否有空白条目可供新对象使用 */
static int enq_lock(void)
{
    UINT i;
    for (i = 0; i < FF_FS_LOCK && Files[i].fs; i++);
    return (i == FF_FS_LOCK) ? 0 : 1;
}

/* 增加对象打开计数器，并返回其索引（0：内部错误） */
static UINT inc_lock(DIR *dp, int acc)
{
    UINT i;

    for (i = 0; i < FF_FS_LOCK; i++)
    { /* 查找对象 */
        if (Files[i].fs == dp->obj.fs && Files[i].clu == dp->obj.sclust && Files[i].ofs == dp->dptr)
            break;
    }

    if (i == FF_FS_LOCK)
    { /* 未打开，注册为新对象 */
        for (i = 0; i < FF_FS_LOCK && Files[i].fs; i++);
        if (i == FF_FS_LOCK)
            return 0; /* 无空闲条目（内部错误） */
        Files[i].fs = dp->obj.fs;
        Files[i].clu = dp->obj.sclust;
        Files[i].ofs = dp->dptr;
        Files[i].ctr = 0;
    }

    if (acc >= 1 && Files[i].ctr)
        return 0;                                  /* 访问冲突（内部错误） */

    Files[i].ctr = acc ? 0x100 : Files[i].ctr + 1; /* 设置信号量值 */

    return i + 1;                                  /* 索引号从1开始 */
}

/* 减少对象打开计数器 */
static FRESULT dec_lock(UINT i)
{
    WORD n;
    FRESULT res;

    if (--i < FF_FS_LOCK)
    { /* 索引号从0开始 */
        n = Files[i].ctr;
        if (n == 0x100)
            n = 0; /* 如果是写模式打开，删除条目 */
        if (n > 0)
            n--;   /* 减少读模式打开计数 */
        Files[i].ctr = n;
        if (n == 0)
            Files[i].fs = 0; /* 如果打开计数为零，删除条目 */
        res = FR_OK;
    }
    else
    {
        res = FR_INT_ERR; /* 无效的索引号 */
    }
    return res;
}

/* 清除指定卷的所有锁条目 */
static void clear_lock(FATFS *fs)
{
    UINT i;
    for (i = 0; i < FF_FS_LOCK; i++)
    {
        if (Files[i].fs == fs)
            Files[i].fs = 0;
    }
}

#endif /* FF_FS_LOCK != 0 */

/*-----------------------------------------------------------------------*/
/* 移动/刷新文件系统对象中的磁盘访问窗口                                  */
/*-----------------------------------------------------------------------*/
#if !FF_FS_READONLY
/* 同步窗口：将窗口中的脏数据写回磁盘 */
static FRESULT sync_window(FATFS *fs)
{
    FRESULT res = FR_OK;

    if (fs->wflag)
    {                                                                          /* 磁盘访问窗口是否为脏？ */
        if (disk_write(fs->pdrv, fs->win, fs->winsect, 1) == RES_OK)
        {                                                                      /* 写回卷 */
            fs->wflag = 0;                                                     /* 清除窗口脏标志 */
            if (fs->winsect - fs->fatbase < fs->fsize)
            {                                                                  /* 是否在第一个FAT中？ */
                if (fs->n_fats == 2)
                    disk_write(fs->pdrv, fs->win, fs->winsect + fs->fsize, 1); /* 如果需要，镜像到第二个FAT */
            }
        }
        else
        {
            res = FR_DISK_ERR;
        }
    }
    return res;
}
#endif

/* 移动窗口到指定扇区，如果扇区不在窗口中则加载新扇区 */
static FRESULT move_window(FATFS *fs, LBA_t sect)
{
    FRESULT res = FR_OK;

    if (sect != fs->winsect)
    {                          /* 窗口偏移已改变？ */
#if !FF_FS_READONLY
        res = sync_window(fs); /* 刷新窗口 */
#endif
        if (res == FR_OK)
        { /* 用新数据填充窗口 */
            if (disk_read(fs->pdrv, fs->win, sect, 1) != RES_OK)
            {
                sect = (LBA_t)0 - 1; /* 如果读数据无效，使窗口无效 */
                res = FR_DISK_ERR;
            }
            fs->winsect = sect;
        }
    }
    return res;
}

#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 同步文件系统和存储设备上的数据                                          */
/*-----------------------------------------------------------------------*/
static FRESULT sync_fs(FATFS *fs)
{
    FRESULT res;

    res = sync_window(fs);
    if (res == FR_OK)
    {
        if (fs->fs_type == FS_FAT32 && fs->fsi_flag == 1)
        { /* FAT32：如果需要更新FSInfo扇区 */
            /* 创建FSInfo结构 */
            memset(fs->win, 0, sizeof fs->win);
            st_word(fs->win + BS_55AA, 0xAA55);                /* 引导签名 */
            st_dword(fs->win + FSI_LeadSig, 0x41615252);       /* 引导签名 */
            st_dword(fs->win + FSI_StrucSig, 0x61417272);      /* 结构签名 */
            st_dword(fs->win + FSI_Free_Count, fs->free_clst); /* 空闲簇数 */
            st_dword(fs->win + FSI_Nxt_Free, fs->last_clst);   /* 最后分配的簇 */
            fs->winsect = fs->volbase + 1;                     /* 写入FSInfo扇区（VBR后一扇区） */
            disk_write(fs->pdrv, fs->win, fs->winsect, 1);
            fs->fsi_flag = 0;
        }
        /* 确保下层没有挂起的写操作 */
        if (disk_ioctl(fs->pdrv, CTRL_SYNC, 0) != RES_OK)
            res = FR_DISK_ERR;
    }
    return res;
}
#endif

/*-----------------------------------------------------------------------*/
/* 从簇号获取物理扇区号                                                  */
/*-----------------------------------------------------------------------*/
static LBA_t clst2sect(FATFS *fs, DWORD clst)
{
    clst -= 2;                                     /* 簇号从2开始 */
    if (clst >= fs->n_fatent - 2)
        return 0;                                  /* 无效簇号？ */
    return fs->database + (LBA_t)fs->csize * clst; /* 簇的起始扇区号 */
}

/*-----------------------------------------------------------------------*/
/* FAT访问 - 读取FAT条目的值                                              */
/*-----------------------------------------------------------------------*/
static DWORD get_fat(FFOBJID *obj, DWORD clst)
{
    UINT wc, bc;
    DWORD val;
    FATFS *fs = obj->fs;

    if (clst < 2 || clst >= fs->n_fatent)
    {            /* 检查是否在有效范围内 */
        val = 1; /* 内部错误 */
    }
    else
    {
        val = 0xFFFFFFFF; /* 默认值，遇到磁盘错误时设置为此值 */

        switch (fs->fs_type)
        {
            case FS_FAT12:
                bc = (UINT)clst;
                bc += bc / 2;
                if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK)
                    break;
                wc = fs->win[bc++ % SS(fs)]; /* 获取条目的第一个字节 */
                if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK)
                    break;
                wc |= fs->win[bc % SS(fs)] << 8;             /* 合并第二个字节 */
                val = (clst & 1) ? (wc >> 4) : (wc & 0xFFF); /* 调整位位置 */
                break;

            case FS_FAT16:
                if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) != FR_OK)
                    break;
                val = ld_word(fs->win + clst * 2 % SS(fs)); /* 简单的字数组 */
                break;

            case FS_FAT32:
                if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK)
                    break;
                val = ld_dword(fs->win + clst * 4 % SS(fs)) & 0x0FFFFFFF; /* 简单的双字数组，屏蔽高4位 */
                break;
#if FF_FS_EXFAT
            case FS_EXFAT:
                if ((obj->objsize != 0 && obj->sclust != 0) || obj->stat == 0)
                {                                                                           /* 除根目录外的对象必须有有效数据长度 */
                    DWORD cofs = clst - obj->sclust;                                        /* 相对于起始簇的偏移 */
                    DWORD clen = (DWORD)((LBA_t)((obj->objsize - 1) / SS(fs)) / fs->csize); /* 簇数 - 1 */

                    if (obj->stat == 2 && cofs <= clen)
                    {                                                 /* 是否为连续链？ */
                        val = (cofs == clen) ? 0x7FFFFFFF : clst + 1; /* FAT中无数据，生成值 */
                        break;
                    }
                    if (obj->stat == 3 && cofs < obj->n_cont)
                    {                   /* 是否在第一个片段中？ */
                        val = clst + 1; /* 生成值 */
                        break;
                    }
                    if (obj->stat != 2)
                    {                         /* 如果FAT链有效，从FAT获取值 */
                        if (obj->n_frag != 0)
                        {                     /* 是否在增长边缘？ */
                            val = 0x7FFFFFFF; /* 生成EOC */
                        }
                        else
                        {
                            if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK)
                                break;
                            val = ld_dword(fs->win + clst * 4 % SS(fs)) & 0x7FFFFFFF;
                        }
                        break;
                    }
                }
                val = 1; /* 内部错误 */
                break;
#endif
            default:
                val = 1; /* 内部错误 */
        }
    }
    return val;
}

#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT访问 - 更改FAT条目的值                                              */
/*-----------------------------------------------------------------------*/
static FRESULT put_fat(FATFS *fs, DWORD clst, DWORD val)
{
    UINT bc;
    BYTE *p;
    FRESULT res = FR_INT_ERR;

    if (clst >= 2 && clst < fs->n_fatent)
    { /* 检查是否在有效范围内 */
        switch (fs->fs_type)
        {
            case FS_FAT12:
                bc = (UINT)clst;
                bc += bc / 2; /* bc：条目的字节偏移 */
                res = move_window(fs, fs->fatbase + (bc / SS(fs)));
                if (res != FR_OK)
                    break;
                p = fs->win + bc++ % SS(fs);
                *p = (clst & 1) ? ((*p & 0x0F) | ((BYTE)val << 4)) : (BYTE)val; /* 更新第一个字节 */
                fs->wflag = 1;
                res = move_window(fs, fs->fatbase + (bc / SS(fs)));
                if (res != FR_OK)
                    break;
                p = fs->win + bc % SS(fs);
                *p = (clst & 1) ? (BYTE)(val >> 4) : ((*p & 0xF0) | ((BYTE)(val >> 8) & 0x0F)); /* 更新第二个字节 */
                fs->wflag = 1;
                break;

            case FS_FAT16:
                res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
                if (res != FR_OK)
                    break;
                st_word(fs->win + clst * 2 % SS(fs), (WORD)val); /* 简单的字数组 */
                fs->wflag = 1;
                break;

            case FS_FAT32:
    #if FF_FS_EXFAT
            case FS_EXFAT:
    #endif
                res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
                if (res != FR_OK)
                    break;
                if (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT)
                {
                    val = (val & 0x0FFFFFFF) | (ld_dword(fs->win + clst * 4 % SS(fs)) & 0xF0000000);
                }
                st_dword(fs->win + clst * 4 % SS(fs), val);
                fs->wflag = 1;
                break;
        }
    }
    return res;
}
#endif /* !FF_FS_READONLY */

#if FF_FS_EXFAT && !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* exFAT：访问 FAT 和分配位图                                             */
/*-----------------------------------------------------------------------*/

/*--------------------------------------*/
/* 查找一个连续的空闲簇块                */
/*--------------------------------------*/
static DWORD find_bitmap(FATFS *fs, DWORD clst, DWORD ncl)
{
    BYTE bm, bv;
    UINT i;
    DWORD val, scl, ctr;

    clst -= 2; /* 位图中第一个位对应簇 #2 */
    if (clst >= fs->n_fatent - 2)
        clst = 0;
    scl = val = clst;
    ctr = 0;
    for (;;)
    {
        if (move_window(fs, fs->bitbase + val / 8 / SS(fs)) != FR_OK)
            return 0xFFFFFFFF;
        i = val / 8 % SS(fs);
        bm = 1 << (val % 8);
        do
        {
            do
            {
                bv = fs->win[i] & bm;
                bm <<= 1; /* 获取位值 */
                if (++val >= fs->n_fatent - 2)
                {         /* 下一个簇（带回绕） */
                    val = 0;
                    bm = 0;
                    i = SS(fs);
                }
                if (bv == 0)
                {                       /* 是否为空闲簇？ */
                    if (++ctr == ncl)
                        return scl + 2; /* 检查运行长度是否满足要求 */
                }
                else
                {
                    scl = val;
                    ctr = 0; /* 遇到已用簇，重新开始扫描 */
                }
                if (val == clst)
                    return 0; /* 所有簇已扫描？ */
            } while (bm != 0);
            bm = 1;
        } while (++i < SS(fs));
    }
}

/*----------------------------------------*/
/* 设置/清除分配位图的一块                */
/*----------------------------------------*/
static FRESULT change_bitmap(FATFS *fs, DWORD clst, DWORD ncl, int bv)
{
    BYTE bm;
    UINT i;
    LBA_t sect;

    clst -= 2;                              /* 第一个位对应簇 #2 */
    sect = fs->bitbase + clst / 8 / SS(fs); /* 扇区地址 */
    i = clst / 8 % SS(fs);                  /* 扇区内的字节偏移 */
    bm = 1 << (clst % 8);                   /* 字节内的位掩码 */
    for (;;)
    {
        if (move_window(fs, sect++) != FR_OK)
            return FR_DISK_ERR;
        do
        {
            do
            {
                if (bv == (int)((fs->win[i] & bm) != 0))
                    return FR_INT_ERR; /* 位值是否与期望一致？ */
                fs->win[i] ^= bm;      /* 翻转位 */
                fs->wflag = 1;
                if (--ncl == 0)
                    return FR_OK; /* 所有位处理完毕？ */
            } while (bm <<= 1); /* 下一个位 */
            bm = 1;
        } while (++i < SS(fs)); /* 下一个字节 */
        i = 0;
    }
}

/*---------------------------------------------*/
/* 填充 FAT 链的第一个片段                      */
/*---------------------------------------------*/
static FRESULT fill_first_frag(FFOBJID *obj)
{
    FRESULT res;
    DWORD cl, n;

    if (obj->stat == 3)
    {     /* 此会话中对象是否已更改为“碎片化”？ */
        for (cl = obj->sclust, n = obj->n_cont; n; cl++, n--)
        { /* 在 FAT 上创建簇链 */
            res = put_fat(obj->fs, cl, cl + 1);
            if (res != FR_OK)
                return res;
        }
        obj->stat = 0; /* 将状态更改为“FAT 链有效” */
    }
    return FR_OK;
}

/*---------------------------------------------*/
/* 填充 FAT 链的最后一个片段                    */
/*---------------------------------------------*/
static FRESULT fill_last_frag(FFOBJID *obj, DWORD lcl, DWORD term)
{
    FRESULT res;

    while (obj->n_frag > 0)
    { /* 创建最后一个片段的链 */
        res = put_fat(obj->fs, lcl - obj->n_frag + 1, (obj->n_frag > 1) ? lcl - obj->n_frag + 2 : term);
        if (res != FR_OK)
            return res;
        obj->n_frag--;
    }
    return FR_OK;
}
#endif /* FF_FS_EXFAT && !FF_FS_READONLY */

#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT 处理 - 移除一个簇链                                               */
/*-----------------------------------------------------------------------*/
static FRESULT remove_chain(FFOBJID *obj, DWORD clst, DWORD pclst)
{
    FRESULT res = FR_OK;
    DWORD nxt;
    FATFS *fs = obj->fs;
    #if FF_FS_EXFAT || FF_USE_TRIM
    DWORD scl = clst, ecl = clst;
    #endif
    #if FF_USE_TRIM
    LBA_t rt[2];
    #endif

    if (clst < 2 || clst >= fs->n_fatent)
        return FR_INT_ERR; /* 检查是否在有效范围内 */

    /* 如果存在，将前一个簇标记为“EOC”（链结束） */
    if (pclst != 0 && (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT || obj->stat != 2))
    {
        res = put_fat(fs, pclst, 0xFFFFFFFF);
        if (res != FR_OK)
            return res;
    }

    /* 移除链 */
    do
    {
        nxt = get_fat(obj, clst); /* 获取簇状态 */
        if (nxt == 0)
            break;                /* 空簇？ */
        if (nxt == 1)
            return FR_INT_ERR;    /* 内部错误？ */
        if (nxt == 0xFFFFFFFF)
            return FR_DISK_ERR;   /* 磁盘错误？ */
        if (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT)
        {
            res = put_fat(fs, clst, 0); /* 在 FAT 上标记簇为“空闲” */
            if (res != FR_OK)
                return res;
        }
        if (fs->free_clst < fs->n_fatent - 2)
        { /* 更新 FSINFO */
            fs->free_clst++;
            fs->fsi_flag |= 1;
        }
    #if FF_FS_EXFAT || FF_USE_TRIM
        if (ecl + 1 == nxt)
        { /* 下一个簇是否连续？ */
            ecl = nxt;
        }
        else
        { /* 连续簇块结束 */
        #if FF_FS_EXFAT
            if (fs->fs_type == FS_EXFAT)
            {
                res = change_bitmap(fs, scl, ecl - scl + 1, 0); /* 在位图中将簇块标记为“空闲” */
                if (res != FR_OK)
                    return res;
            }
        #endif
        #if FF_USE_TRIM
            rt[0] = clst2sect(fs, scl);                 /* 要释放的数据区起始 */
            rt[1] = clst2sect(fs, ecl) + fs->csize - 1; /* 要释放的数据区结束 */
            disk_ioctl(fs->pdrv, CTRL_TRIM, rt);        /* 通知存储设备块中的数据可能被擦除 */
        #endif
            scl = ecl = nxt;
        }
    #endif
        clst = nxt; /* 下一个簇 */
    } while (clst < fs->n_fatent); /* 重复直到最后一个链接 */

    #if FF_FS_EXFAT
    /* 链状态的一些后处理 */
    if (fs->fs_type == FS_EXFAT)
    {
        if (pclst == 0)
        {                  /* 是否移除了整个链？ */
            obj->stat = 0; /* 将链状态更改为“初始” */
        }
        else
        {
            if (obj->stat == 0)
            {                       /* 此会话开始时是否为碎片化链？ */
                clst = obj->sclust; /* 沿着链检查是否变得连续 */
                while (clst != pclst)
                {
                    nxt = get_fat(obj, clst);
                    if (nxt < 2)
                        return FR_INT_ERR;
                    if (nxt == 0xFFFFFFFF)
                        return FR_DISK_ERR;
                    if (nxt != clst + 1)
                        break; /* 不连续？ */
                    clst++;
                }
                if (clst == pclst)
                {                  /* 链是否再次变得连续？ */
                    obj->stat = 2; /* 将链状态更改为“连续” */
                }
            }
            else
            {
                if (obj->stat == 3 && pclst >= obj->sclust && pclst <= obj->sclust + obj->n_cont)
                {                  /* 此会话中链已碎片化，但再次变得连续？ */
                    obj->stat = 2; /* 将链状态更改为“连续” */
                }
            }
        }
    }
    #endif
    return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* FAT 处理 - 扩展链或创建新链                                            */
/*-----------------------------------------------------------------------*/
static DWORD create_chain(FFOBJID *obj, DWORD clst)
{
    DWORD cs, ncl, scl;
    FRESULT res;
    FATFS *fs = obj->fs;

    if (clst == 0)
    {                        /* 创建新链 */
        scl = fs->last_clst; /* 建议开始查找的簇 */
        if (scl == 0 || scl >= fs->n_fatent)
            scl = 1;
    }
    else
    {                            /* 扩展链 */
        cs = get_fat(obj, clst); /* 检查簇状态 */
        if (cs < 2)
            return 1;            /* 检查是否异常 */
        if (cs == 0xFFFFFFFF)
            return cs;           /* 检查磁盘错误 */
        if (cs < fs->n_fatent)
            return cs;           /* 如果后面已有簇，返回该簇 */
        scl = clst;              /* 开始查找的簇 */
    }
    if (fs->free_clst == 0)
        return 0; /* 无空闲簇 */

    #if FF_FS_EXFAT
    if (fs->fs_type == FS_EXFAT)
    {                                       /* 在 exFAT 卷上 */
        ncl = find_bitmap(fs, scl, 1);      /* 查找一个空闲簇 */
        if (ncl == 0 || ncl == 0xFFFFFFFF)
            return ncl;                     /* 无空闲簇或硬件错误？ */
        res = change_bitmap(fs, ncl, 1, 1); /* 将簇标记为“已用” */
        if (res == FR_INT_ERR)
            return 1;
        if (res == FR_DISK_ERR)
            return 0xFFFFFFFF;
        if (clst == 0)
        {                  /* 是否为新链？ */
            obj->stat = 2; /* 设置状态为“连续” */
        }
        else
        {                                        /* 扩展的链 */
            if (obj->stat == 2 && ncl != scl + 1)
            {                                    /* 链是否变得碎片化？ */
                obj->n_cont = scl - obj->sclust; /* 设置连续部分的大小 */
                obj->stat = 3;                   /* 将状态更改为“刚刚碎片化” */
            }
        }
        if (obj->stat != 2)
        {                                                        /* 文件是否不连续？ */
            if (ncl == clst + 1)
            {                                                    /* 簇是否紧邻前一个？ */
                obj->n_frag = obj->n_frag ? obj->n_frag + 1 : 2; /* 增加最后一个片段的大小 */
            }
            else
            { /* 新片段 */
                if (obj->n_frag == 0)
                    obj->n_frag = 1;
                res = fill_last_frag(obj, clst, ncl); /* 在 FAT 上填充最后一个片段并链接到新片段 */
                if (res == FR_OK)
                    obj->n_frag = 1;
            }
        }
    }
    else
    #endif
    { /* 在 FAT/FAT32 卷上 */
        ncl = 0;
        if (scl == clst)
        {                  /* 扩展现有链？ */
            ncl = scl + 1; /* 测试下一个簇是否空闲 */
            if (ncl >= fs->n_fatent)
                ncl = 2;
            cs = get_fat(obj, ncl); /* 获取下一个簇状态 */
            if (cs == 1 || cs == 0xFFFFFFFF)
                return cs;          /* 检查错误 */
            if (cs != 0)
            {                       /* 不空闲？ */
                cs = fs->last_clst; /* 如果有效，从建议簇开始 */
                if (cs >= 2 && cs < fs->n_fatent)
                    scl = cs;
                ncl = 0;
            }
        }
        if (ncl == 0)
        {              /* 新簇不能连续，查找另一个片段 */
            ncl = scl; /* 起始簇 */
            for (;;)
            {
                ncl++; /* 下一个簇 */
                if (ncl >= fs->n_fatent)
                {      /* 检查回绕 */
                    ncl = 2;
                    if (ncl > scl)
                        return 0;       /* 未找到空闲簇？ */
                }
                cs = get_fat(obj, ncl); /* 获取簇状态 */
                if (cs == 0)
                    break;              /* 找到空闲簇？ */
                if (cs == 1 || cs == 0xFFFFFFFF)
                    return cs;          /* 检查错误 */
                if (ncl == scl)
                    return 0;           /* 未找到空闲簇？ */
            }
        }
        res = put_fat(fs, ncl, 0xFFFFFFFF); /* 将新簇标记为“EOC” */
        if (res == FR_OK && clst != 0)
        {
            res = put_fat(fs, clst, ncl); /* 如果需要，将其链接到前一个簇 */
        }
    }

    if (res == FR_OK)
    { /* 如果函数成功，更新 FSINFO */
        fs->last_clst = ncl;
        if (fs->free_clst <= fs->n_fatent - 2)
            fs->free_clst--;
        fs->fsi_flag |= 1;
    }
    else
    {
        ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1; /* 失败，生成错误状态 */
    }

    return ncl; /* 返回新簇号或错误状态 */
}
#endif          /* !FF_FS_READONLY */

#if FF_USE_FASTSEEK
/*-----------------------------------------------------------------------*/
/* FAT 处理 - 使用链接映射表将偏移量转换为簇                              */
/*-----------------------------------------------------------------------*/
static DWORD clmt_clust(FIL *fp, FSIZE_t ofs)
{
    DWORD cl, ncl, *tbl;
    FATFS *fs = fp->obj.fs;

    tbl = fp->cltbl + 1;                    /* CLMT 的顶部 */
    cl = (DWORD)(ofs / SS(fs) / fs->csize); /* 从文件顶部开始的簇顺序 */
    for (;;)
    {
        ncl = *tbl++; /* 片段中的簇数 */
        if (ncl == 0)
            return 0; /* 表结束？（错误） */
        if (cl < ncl)
            break;    /* 在此片段中？ */
        cl -= ncl;
        tbl++;        /* 下一个片段 */
    }
    return cl + *tbl; /* 返回簇号 */
}
#endif                /* FF_USE_FASTSEEK */

/*-----------------------------------------------------------------------*/
/* 目录处理 - 用零填充一个簇                                              */
/*-----------------------------------------------------------------------*/
#if !FF_FS_READONLY
static FRESULT dir_clear(FATFS *fs, DWORD clst)
{
    LBA_t sect;
    UINT n, szb;
    BYTE *ibuf;

    if (sync_window(fs) != FR_OK)
        return FR_DISK_ERR;             /* 刷新磁盘访问窗口 */
    sect = clst2sect(fs, clst);         /* 簇的顶部 */
    fs->winsect = sect;                 /* 将窗口设置到簇的顶部 */
    memset(fs->win, 0, sizeof fs->win); /* 清除窗口缓冲区 */
    #if FF_USE_LFN == 3                 /* 通过使用多扇区写入快速清除表 */
    /* 分配临时缓冲区 */
    for (szb = ((DWORD)fs->csize * SS(fs) >= MAX_MALLOC) ? MAX_MALLOC : fs->csize * SS(fs), ibuf = 0; szb > SS(fs) && (ibuf = ff_memalloc(szb)) == 0; szb /= 2);
    if (szb > SS(fs))
    {                                                                                                /* 缓冲区已分配？ */
        memset(ibuf, 0, szb);
        szb /= SS(fs);                                                                               /* 字节 -> 扇区 */
        for (n = 0; n < fs->csize && disk_write(fs->pdrv, ibuf, sect + n, szb) == RES_OK; n += szb); /* 用0填充簇 */
        ff_memfree(ibuf);
    }
    else
    #endif
    {
        ibuf = fs->win;
        szb = 1;                                                                                     /* 使用窗口缓冲区（多次单扇区写入可能需要时间） */
        for (n = 0; n < fs->csize && disk_write(fs->pdrv, ibuf, sect + n, szb) == RES_OK; n += szb); /* 用0填充簇 */
    }
    return (n == fs->csize) ? FR_OK : FR_DISK_ERR;
}
#endif /* !FF_FS_READONLY */

/*-----------------------------------------------------------------------*/
/* 目录处理 - 设置目录索引                                                */
/*-----------------------------------------------------------------------*/
static FRESULT dir_sdi(DIR *dp, DWORD ofs)
{
    DWORD csz, clst;
    FATFS *fs = dp->obj.fs;

    if (ofs >= (DWORD)((FF_FS_EXFAT && fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR) || ofs % SZDIRE)
    { /* 检查偏移范围和对齐 */
        return FR_INT_ERR;
    }
    dp->dptr = ofs;        /* 设置当前偏移 */
    clst = dp->obj.sclust; /* 表起始簇（0：根目录） */
    if (clst == 0 && fs->fs_type >= FS_FAT32)
    {                      /* 将簇#0替换为根目录簇# */
        clst = (DWORD)fs->dirbase;
        if (FF_FS_EXFAT)
            dp->obj.stat = 0; /* exFAT：根目录有 FAT 链 */
    }

    if (clst == 0)
    {                          /* 静态表（FAT 卷上的根目录） */
        if (ofs / SZDIRE >= fs->n_rootdir)
            return FR_INT_ERR; /* 索引超出范围？ */
        dp->sect = fs->dirbase;
    }
    else
    {                                       /* 动态表（FAT32/exFAT 卷上的子目录或根目录） */
        csz = (DWORD)fs->csize * SS(fs);    /* 每簇字节数 */
        while (ofs >= csz)
        {                                   /* 沿着簇链 */
            clst = get_fat(&dp->obj, clst); /* 获取下一个簇 */
            if (clst == 0xFFFFFFFF)
                return FR_DISK_ERR;         /* 磁盘错误 */
            if (clst < 2 || clst >= fs->n_fatent)
                return FR_INT_ERR;          /* 到达表末尾或内部错误 */
            ofs -= csz;
        }
        dp->sect = clst2sect(fs, clst);
    }
    dp->clust = clst; /* 当前簇# */
    if (dp->sect == 0)
        return FR_INT_ERR;
    dp->sect += ofs / SS(fs);           /* 目录条目的扇区# */
    dp->dir = fs->win + (ofs % SS(fs)); /* win[] 中指向条目的指针 */

    return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* 目录处理 - 将目录表索引移动到下一个                                    */
/*-----------------------------------------------------------------------*/
static FRESULT dir_next(DIR *dp, int stretch)
{
    DWORD ofs, clst;
    FATFS *fs = dp->obj.fs;

    ofs = dp->dptr + SZDIRE; /* 下一个条目 */
    if (ofs >= (DWORD)((FF_FS_EXFAT && fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR))
        dp->sect = 0;        /* 如果偏移达到最大值，禁用 */
    if (dp->sect == 0)
        return FR_NO_FILE;   /* 如果已禁用，报告 EOT */

    if (ofs % SS(fs) == 0)
    {               /* 扇区改变？ */
        dp->sect++; /* 下一个扇区 */

        if (dp->clust == 0)
        {     /* 静态表 */
            if (ofs / SZDIRE >= fs->n_rootdir)
            { /* 如果到达静态表末尾，报告 EOT */
                dp->sect = 0;
                return FR_NO_FILE;
            }
        }
        else
        {                                            /* 动态表 */
            if ((ofs / SS(fs) & (fs->csize - 1)) == 0)
            {                                        /* 簇改变？ */
                clst = get_fat(&dp->obj, dp->clust); /* 获取下一个簇 */
                if (clst <= 1)
                    return FR_INT_ERR;               /* 内部错误 */
                if (clst == 0xFFFFFFFF)
                    return FR_DISK_ERR;              /* 磁盘错误 */
                if (clst >= fs->n_fatent)
                {                                    /* 到达动态表末尾 */
#if !FF_FS_READONLY
                    if (!stretch)
                    { /* 如果不拉伸，报告 EOT */
                        dp->sect = 0;
                        return FR_NO_FILE;
                    }
                    clst = create_chain(&dp->obj, dp->clust); /* 分配一个簇 */
                    if (clst == 0)
                        return FR_DENIED;                     /* 无空闲簇 */
                    if (clst == 1)
                        return FR_INT_ERR;                    /* 内部错误 */
                    if (clst == 0xFFFFFFFF)
                        return FR_DISK_ERR;                   /* 磁盘错误 */
                    if (dir_clear(fs, clst) != FR_OK)
                        return FR_DISK_ERR;                   /* 清理扩展的表 */
                    if (FF_FS_EXFAT)
                        dp->obj.stat |= 4;                    /* exFAT：目录已被扩展 */
#else
                    if (!stretch)
                        dp->sect = 0;  /* （此行用于抑制编译器警告） */
                    dp->sect = 0;
                    return FR_NO_FILE; /* 报告 EOT */
#endif
                }
                dp->clust = clst; /* 为新簇初始化数据 */
                dp->sect = clst2sect(fs, clst);
            }
        }
    }
    dp->dptr = ofs;                   /* 当前条目 */
    dp->dir = fs->win + ofs % SS(fs); /* win[] 中指向条目的指针 */

    return FR_OK;
}

#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 目录处理 - 预留一块目录条目                                            */
/*-----------------------------------------------------------------------*/
static FRESULT dir_alloc(DIR *dp, UINT n_ent)
{
    FRESULT res;
    UINT n;
    FATFS *fs = dp->obj.fs;

    res = dir_sdi(dp, 0);
    if (res == FR_OK)
    {
        n = 0;
        do
        {
            res = move_window(fs, dp->sect);
            if (res != FR_OK)
                break;
    #if FF_FS_EXFAT
            if ((fs->fs_type == FS_EXFAT) ? (int)((dp->dir[XDIR_Type] & 0x80) == 0) : (int)(dp->dir[DIR_Name] == DDEM || dp->dir[DIR_Name] == 0))
            { /* 条目是否空闲？ */
    #else
            if (dp->dir[DIR_Name] == DDEM || dp->dir[DIR_Name] == 0)
            { /* 条目是否空闲？ */
    #endif
                if (++n == n_ent)
                    break; /* 是否找到一块连续的空闲条目？ */
            }
            else
            {
                n = 0;             /* 不是空闲条目，重新开始搜索 */
            }
            res = dir_next(dp, 1); /* 下一个条目，允许表拉伸 */
        } while (res == FR_OK);
    }

    if (res == FR_NO_FILE)
        res = FR_DENIED; /* 没有可分配的目录条目 */
    return res;
}
#endif /* !FF_FS_READONLY */

/*-----------------------------------------------------------------------*/
/* FAT：目录处理 - 加载/存储起始簇号                                      */
/*-----------------------------------------------------------------------*/
static DWORD ld_clust(FATFS *fs, const BYTE *dir)
{
    DWORD cl;

    cl = ld_word(dir + DIR_FstClusLO);
    if (fs->fs_type == FS_FAT32)
    {
        cl |= (DWORD)ld_word(dir + DIR_FstClusHI) << 16;
    }
    return cl;
}

#if !FF_FS_READONLY
static void st_clust(FATFS *fs, BYTE *dir, DWORD cl)
{
    st_word(dir + DIR_FstClusLO, (WORD)cl);
    if (fs->fs_type == FS_FAT32)
    {
        st_word(dir + DIR_FstClusHI, (WORD)(cl >> 16));
    }
}
#endif

#if FF_USE_LFN
/*--------------------------------------------------------*/
/* FAT-LFN：将文件名的一部分与 LFN 条目比较                */
/*--------------------------------------------------------*/
static int cmp_lfn(const WCHAR *lfnbuf, BYTE *dir)
{
    UINT i, s;
    WCHAR wc, uc;

    if (ld_word(dir + LDIR_FstClusLO) != 0)
        return 0;                          /* 检查 LDIR_FstClusLO */

    i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13; /* LFN 缓冲区中的偏移 */

    for (wc = 1, s = 0; s < 13; s++)
    {                                  /* 处理条目中的所有字符 */
        uc = ld_word(dir + LfnOfs[s]); /* 选取一个 LFN 字符 */
        if (wc != 0)
        {
            if (i >= FF_MAX_LFN + 1 || ff_wtoupper(uc) != ff_wtoupper(lfnbuf[i++]))
            {             /* 比较 */
                return 0; /* 不匹配 */
            }
            wc = uc;
        }
        else
        {
            if (uc != 0xFFFF)
                return 0; /* 检查填充符 */
        }
    }

    if ((dir[LDIR_Ord] & LLEF) && wc && lfnbuf[i])
        return 0; /* 最后一个段匹配但长度不同 */

    return 1;     /* LFN 部分匹配 */
}

    #if FF_FS_MINIMIZE <= 1 || FF_FS_RPATH >= 2 || FF_USE_LABEL || FF_FS_EXFAT
/*-----------------------------------------------------*/
/* FAT-LFN：从 LFN 条目中选取一部分文件名               */
/*-----------------------------------------------------*/
static int pick_lfn(WCHAR *lfnbuf, BYTE *dir)
{
    UINT i, s;
    WCHAR wc, uc;

    if (ld_word(dir + LDIR_FstClusLO) != 0)
        return 0;                           /* 检查 LDIR_FstClusLO 为 0 */

    i = ((dir[LDIR_Ord] & ~LLEF) - 1) * 13; /* LFN 缓冲区中的偏移 */

    for (wc = 1, s = 0; s < 13; s++)
    {                                  /* 处理条目中的所有字符 */
        uc = ld_word(dir + LfnOfs[s]); /* 选取一个 LFN 字符 */
        if (wc != 0)
        {
            if (i >= FF_MAX_LFN + 1)
                return 0;          /* 缓冲区溢出？ */
            lfnbuf[i++] = wc = uc; /* 存储 */
        }
        else
        {
            if (uc != 0xFFFF)
                return 0; /* 检查填充符 */
        }
    }

    if (dir[LDIR_Ord] & LLEF && wc != 0)
    {                 /* 如果是最后一个 LFN 部分且未终止，添加终止符 */
        if (i >= FF_MAX_LFN + 1)
            return 0; /* 缓冲区溢出？ */
        lfnbuf[i] = 0;
    }

    return 1; /* LFN 部分有效 */
}
    #endif

    #if !FF_FS_READONLY
/*-----------------------------------------*/
/* FAT-LFN：创建 LFN 条目                   */
/*-----------------------------------------*/
static void put_lfn(const WCHAR *lfn, BYTE *dir, BYTE ord, BYTE sum)
{
    UINT i, s;
    WCHAR wc;

    dir[LDIR_Chksum] = sum;  /* 设置校验和 */
    dir[LDIR_Attr] = AM_LFN; /* 设置属性，LFN 条目 */
    dir[LDIR_Type] = 0;
    st_word(dir + LDIR_FstClusLO, 0);

    i = (ord - 1) * 13; /* 获取 LFN 工作缓冲区中的偏移 */
    s = wc = 0;
    do
    {
        if (wc != 0xFFFF)
            wc = lfn[i++];            /* 获取一个有效字符 */
        st_word(dir + LfnOfs[s], wc); /* 放入 */
        if (wc == 0)
            wc = 0xFFFF;              /* 后续项填充字符 */
    } while (++s < 13);
    if (wc == 0xFFFF || !lfn[i])
        ord |= LLEF;     /* 最后一个 LFN 部分是 LFN 序列的开始 */
    dir[LDIR_Ord] = ord; /* 设置 LFN 顺序 */
}
    #endif               /* !FF_FS_READONLY */
#endif                   /* FF_USE_LFN */

#if FF_USE_LFN && !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT-LFN：创建带编号的 SFN                                              */
/*-----------------------------------------------------------------------*/
static void gen_numname(BYTE *dst, const BYTE *src, const WCHAR *lfn, UINT seq)
{
    BYTE ns[8], c;
    UINT i, j;
    WCHAR wc;
    DWORD sreg;

    memcpy(dst, src, 11); /* 准备要修改的 SFN */

    if (seq > 5)
    { /* 如果冲突较多，生成哈希数而不是顺序号 */
        sreg = seq;
        while (*lfn)
        { /* 创建 CRC 作为哈希值 */
            wc = *lfn++;
            for (i = 0; i < 16; i++)
            {
                sreg = (sreg << 1) + (wc & 1);
                wc >>= 1;
                if (sreg & 0x10000)
                    sreg ^= 0x11021;
            }
        }
        seq = (UINT)sreg;
    }

    /* 生成后缀（~ + 十六进制） */
    i = 7;
    do
    {
        c = (BYTE)((seq % 16) + '0');
        seq /= 16;
        if (c > '9')
            c += 7;
        ns[i--] = c;
    } while (i && seq);
    ns[i] = '~';

    /* 将后缀附加到 SFN 主体 */
    for (j = 0; j < i && dst[j] != ' '; j++)
    {     /* 查找附加偏移 */
        if (dbc_1st(dst[j]))
        { /* 避免拆分双字节字符 */
            if (j == i - 1)
                break;
            j++;
        }
    }
    do
    { /* 附加后缀 */
        dst[j++] = (i < 8) ? ns[i++] : ' ';
    } while (j < 8);
}
#endif /* FF_USE_LFN && !FF_FS_READONLY */

#if FF_USE_LFN
/*-----------------------------------------------------------------------*/
/* FAT-LFN：计算 SFN 条目的校验和                                         */
/*-----------------------------------------------------------------------*/
static BYTE sum_sfn(const BYTE *dir)
{
    BYTE sum = 0;
    UINT n = 11;

    do
    {
        sum = (sum >> 1) + (sum << 7) + *dir++;
    } while (--n);
    return sum;
}
#endif /* FF_USE_LFN */

#if FF_FS_EXFAT
/*-----------------------------------------------------------------------*/
/* exFAT：校验和                                                          */
/*-----------------------------------------------------------------------*/
static WORD xdir_sum(const BYTE *dir)
{
    UINT i, szblk;
    WORD sum;

    szblk = (dir[XDIR_NumSec] + 1) * SZDIRE; /* 条目块的字节数 */
    for (i = sum = 0; i < szblk; i++)
    {
        if (i == XDIR_SetSum)
        { /* 跳过 2 字节和字段 */
            i++;
        }
        else
        {
            sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + dir[i];
        }
    }
    return sum;
}

static WORD xname_sum(const WCHAR *name)
{
    WCHAR chr;
    WORD sum = 0;

    while ((chr = *name++) != 0)
    {
        chr = (WCHAR)ff_wtoupper(chr); /* 文件名需要转换为大写 */
        sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr & 0xFF);
        sum = ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr >> 8);
    }
    return sum;
}

    #if !FF_FS_READONLY && FF_USE_MKFS
static DWORD xsum32(BYTE dat, DWORD sum)
{
    sum = ((sum & 1) ? 0x80000000 : 0) + (sum >> 1) + dat;
    return sum;
}
    #endif

/*-----------------------------------*/
/* exFAT：加载一个目录项块             */
/*-----------------------------------*/
static FRESULT load_xdir(DIR *dp)
{
    FRESULT res;
    UINT i, sz_ent;
    BYTE *dirb = dp->obj.fs->dirbuf; /* 指向内存中的目录项块 85+C0+C1s 的指针 */

    /* 加载文件目录条目 */
    res = move_window(dp->obj.fs, dp->sect);
    if (res != FR_OK)
        return res;
    if (dp->dir[XDIR_Type] != ET_FILEDIR)
        return FR_INT_ERR; /* 无效顺序 */
    memcpy(dirb + 0 * SZDIRE, dp->dir, SZDIRE);
    sz_ent = (dirb[XDIR_NumSec] + 1) * SZDIRE;
    if (sz_ent < 3 * SZDIRE || sz_ent > 19 * SZDIRE)
        return FR_INT_ERR;

    /* 加载流扩展条目 */
    res = dir_next(dp, 0);
    if (res == FR_NO_FILE)
        res = FR_INT_ERR; /* 不可能 */
    if (res != FR_OK)
        return res;
    res = move_window(dp->obj.fs, dp->sect);
    if (res != FR_OK)
        return res;
    if (dp->dir[XDIR_Type] != ET_STREAM)
        return FR_INT_ERR; /* 无效顺序 */
    memcpy(dirb + 1 * SZDIRE, dp->dir, SZDIRE);
    if (MAXDIRB(dirb[XDIR_NumName]) > sz_ent)
        return FR_INT_ERR;

    /* 加载文件名条目 */
    i = 2 * SZDIRE; /* 要加载的名称偏移 */
    do
    {
        res = dir_next(dp, 0);
        if (res == FR_NO_FILE)
            res = FR_INT_ERR; /* 不可能 */
        if (res != FR_OK)
            return res;
        res = move_window(dp->obj.fs, dp->sect);
        if (res != FR_OK)
            return res;
        if (dp->dir[XDIR_Type] != ET_FILENAME)
            return FR_INT_ERR; /* 无效顺序 */
        if (i < MAXDIRB(FF_MAX_LFN))
            memcpy(dirb + i, dp->dir, SZDIRE);
    } while ((i += SZDIRE) < sz_ent);

    /* 完整性检查（仅对可访问对象执行） */
    if (i <= MAXDIRB(FF_MAX_LFN))
    {
        if (xdir_sum(dirb) != ld_word(dirb + XDIR_SetSum))
            return FR_INT_ERR;
    }
    return FR_OK;
}

/*------------------------------------------------------------------*/
/* exFAT：用加载的条目块初始化对象分配信息                            */
/*------------------------------------------------------------------*/
static void init_alloc_info(FATFS *fs, FFOBJID *obj)
{
    obj->sclust = ld_dword(fs->dirbuf + XDIR_FstClus);   /* 起始簇 */
    obj->objsize = ld_qword(fs->dirbuf + XDIR_FileSize); /* 大小 */
    obj->stat = fs->dirbuf[XDIR_GenFlags] & 2;           /* 分配状态 */
    obj->n_frag = 0;                                     /* 无最后一个片段信息 */
}

    #if !FF_FS_READONLY || FF_FS_RPATH != 0
/*------------------------------------------------*/
/* exFAT：加载对象的目录项块                        */
/*------------------------------------------------*/
static FRESULT load_obj_xdir(DIR *dp, const FFOBJID *obj)
{
    FRESULT res;

    /* 打开对象所在的目录 */
    dp->obj.fs = obj->fs;
    dp->obj.sclust = obj->c_scl;
    dp->obj.stat = (BYTE)obj->c_size;
    dp->obj.objsize = obj->c_size & 0xFFFFFF00;
    dp->obj.n_frag = 0;
    dp->blk_ofs = obj->c_ofs;

    res = dir_sdi(dp, dp->blk_ofs); /* 定位到对象的条目块 */
    if (res == FR_OK)
    {
        res = load_xdir(dp); /* 加载对象的条目块 */
    }
    return res;
}
    #endif

    #if !FF_FS_READONLY
/*----------------------------------------*/
/* exFAT：存储目录项块                      */
/*----------------------------------------*/
static FRESULT store_xdir(DIR *dp)
{
    FRESULT res;
    UINT nent;
    BYTE *dirb = dp->obj.fs->dirbuf; /* 指向内存中的目录项块 85+C0+C1s 的指针 */

    /* 创建集和 */
    st_word(dirb + XDIR_SetSum, xdir_sum(dirb));
    nent = dirb[XDIR_NumSec] + 1;

    /* 将目录项块存储到目录中 */
    res = dir_sdi(dp, dp->blk_ofs);
    while (res == FR_OK)
    {
        res = move_window(dp->obj.fs, dp->sect);
        if (res != FR_OK)
            break;
        memcpy(dp->dir, dirb, SZDIRE);
        dp->obj.fs->wflag = 1;
        if (--nent == 0)
            break;
        dirb += SZDIRE;
        res = dir_next(dp, 0);
    }
    return (res == FR_OK || res == FR_DISK_ERR) ? res : FR_INT_ERR;
}

/*-------------------------------------------*/
/* exFAT：创建新的目录项块                     */
/*-------------------------------------------*/
static void create_xdir(BYTE *dirb, const WCHAR *lfn)
{
    UINT i;
    BYTE nc1, nlen;
    WCHAR wc;

    /* 创建文件目录和流扩展条目 */
    memset(dirb, 0, 2 * SZDIRE);
    dirb[0 * SZDIRE + XDIR_Type] = ET_FILEDIR;
    dirb[1 * SZDIRE + XDIR_Type] = ET_STREAM;

    /* 创建文件名条目 */
    i = SZDIRE * 2; /* 文件名条目起始 */
    nlen = nc1 = 0;
    wc = 1;
    do
    {
        dirb[i++] = ET_FILENAME;
        dirb[i++] = 0;
        do
        {                          /* 填充名字字段 */
            if (wc != 0 && (wc = lfn[nlen]) != 0)
                nlen++;            /* 如果存在，获取一个字符 */
            st_word(dirb + i, wc); /* 存储 */
            i += 2;
        } while (i % SZDIRE != 0);
        nc1++;
    } while (lfn[nlen]); /* 如果有后续字符，填充下一个条目 */

    dirb[XDIR_NumName] = nlen;                     /* 设置名字长度 */
    dirb[XDIR_NumSec] = 1 + nc1;                   /* 设置二级条目计数（C0 + C1s） */
    st_word(dirb + XDIR_NameHash, xname_sum(lfn)); /* 设置名字哈希 */
}
    #endif                                         /* !FF_FS_READONLY */
#endif                                             /* FF_FS_EXFAT */

#if FF_FS_MINIMIZE <= 1 || FF_FS_RPATH >= 2 || FF_USE_LABEL || FF_FS_EXFAT
    /*-----------------------------------------------------------------------*/
    /* 从目录中读取一个对象                                                   */
    /*-----------------------------------------------------------------------*/
    #define DIR_READ_FILE(dp) dir_read(dp, 0)
    #define DIR_READ_LABEL(dp) dir_read(dp, 1)

static FRESULT dir_read(DIR *dp, int vol)
{
    FRESULT res = FR_NO_FILE;
    FATFS *fs = dp->obj.fs;
    BYTE attr, b;
    #if FF_USE_LFN
    BYTE ord = 0xFF, sum = 0xFF;
    #endif

    while (dp->sect)
    {
        res = move_window(fs, dp->sect);
        if (res != FR_OK)
            break;
        b = dp->dir[DIR_Name]; /* 测试条目类型 */
        if (b == 0)
        {
            res = FR_NO_FILE;
            break; /* 到达目录末尾 */
        }
    #if FF_FS_EXFAT
        if (fs->fs_type == FS_EXFAT)
        { /* 在 exFAT 卷上 */
            if (FF_USE_LABEL && vol)
            {
                if (b == ET_VLABEL)
                    break; /* 卷标条目？ */
            }
            else
            {
                if (b == ET_FILEDIR)
                {                           /* 文件条目块开始？ */
                    dp->blk_ofs = dp->dptr; /* 获取块的位置 */
                    res = load_xdir(dp);    /* 加载条目块 */
                    if (res == FR_OK)
                    {
                        dp->obj.attr = fs->dirbuf[XDIR_Attr] & AM_MASK; /* 获取属性 */
                    }
                    break;
                }
            }
        }
        else
    #endif
        {                                                      /* 在 FAT/FAT32 卷上 */
            dp->obj.attr = attr = dp->dir[DIR_Attr] & AM_MASK; /* 获取属性 */
    #if FF_USE_LFN                                             /* LFN 配置 */
            if (b == DDEM || b == '.' || (int)((attr & ~AM_ARC) == AM_VOL) != vol)
            {                                                  /* 无效数据的条目 */
                ord = 0xFF;
            }
            else
            {
                if (attr == AM_LFN)
                {     /* 找到 LFN 条目 */
                    if (b & LLEF)
                    { /* 是否为 LFN 序列的开始？ */
                        sum = dp->dir[LDIR_Chksum];
                        b &= (BYTE)~LLEF;
                        ord = b;
                        dp->blk_ofs = dp->dptr;
                    }
                    /* 检查 LFN 有效性并捕获它 */
                    ord = (b == ord && sum == dp->dir[LDIR_Chksum] && pick_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF;
                }
                else
                {                                 /* 找到 SFN 条目 */
                    if (ord != 0 || sum != sum_sfn(dp->dir))
                    {                             /* 是否有有效的 LFN？ */
                        dp->blk_ofs = 0xFFFFFFFF; /* 没有 LFN */
                    }
                    break;
                }
            }
    #else /* 非 LFN 配置 */
            if (b != DDEM && b != '.' && attr != AM_LFN && (int)((attr & ~AM_ARC) == AM_VOL) == vol)
            { /* 是否为有效条目？ */
                break;
            }
    #endif
        }
        res = dir_next(dp, 0); /* 下一个条目 */
        if (res != FR_OK)
            break;
    }

    if (res != FR_OK)
        dp->sect = 0; /* 出错或 EOT 时终止读操作 */
    return res;
}
#endif /* FF_FS_MINIMIZE <= 1 || FF_USE_LABEL || FF_FS_RPATH >= 2 */

/*-----------------------------------------------------------------------*/
/* 目录处理 - 在目录中查找对象                                            */
/*-----------------------------------------------------------------------*/
static FRESULT dir_find(DIR *dp)
{
    FRESULT res;
    FATFS *fs = dp->obj.fs;
    BYTE c;
#if FF_USE_LFN
    BYTE a, ord, sum;
#endif

    res = dir_sdi(dp, 0); /* 回绕目录对象 */
    if (res != FR_OK)
        return res;
#if FF_FS_EXFAT
    if (fs->fs_type == FS_EXFAT)
    { /* 在 exFAT 卷上 */
        BYTE nc;
        UINT di, ni;
        WORD hash = xname_sum(fs->lfnbuf); /* 要查找的名字的哈希值 */

        while ((res = DIR_READ_FILE(dp)) == FR_OK)
        { /* 读取一个项 */
    #if FF_MAX_LFN < 255
            if (fs->dirbuf[XDIR_NumName] > FF_MAX_LFN)
                continue; /* 如果对象名过长则跳过比较 */
    #endif
            if (ld_word(fs->dirbuf + XDIR_NameHash) != hash)
                continue; /* 如果哈希不匹配则跳过 */
            for (nc = fs->dirbuf[XDIR_NumName], di = SZDIRE * 2, ni = 0; nc; nc--, di += 2, ni++)
            {             /* 比较名字 */
                if ((di % SZDIRE) == 0)
                    di += 2;
                if (ff_wtoupper(ld_word(fs->dirbuf + di)) != ff_wtoupper(fs->lfnbuf[ni]))
                    break;
            }
            if (nc == 0 && !fs->lfnbuf[ni])
                break; /* 名字匹配？ */
        }
        return res;
    }
#endif
    /* 在 FAT/FAT32 卷上 */
#if FF_USE_LFN
    ord = sum = 0xFF;
    dp->blk_ofs = 0xFFFFFFFF; /* 重置 LFN 序列 */
#endif
    do
    {
        res = move_window(fs, dp->sect);
        if (res != FR_OK)
            break;
        c = dp->dir[DIR_Name];
        if (c == 0)
        {
            res = FR_NO_FILE;
            break;
        } /* 到达表末尾 */
#if FF_USE_LFN /* LFN 配置 */
        dp->obj.attr = a = dp->dir[DIR_Attr] & AM_MASK;
        if (c == DDEM || ((a & AM_VOL) && a != AM_LFN))
        {                             /* 无效数据的条目 */
            ord = 0xFF;
            dp->blk_ofs = 0xFFFFFFFF; /* 重置 LFN 序列 */
        }
        else
        {
            if (a == AM_LFN)
            { /* 找到 LFN 条目 */
                if (!(dp->fn[NSFLAG] & NS_NOLFN))
                {
                    if (c & LLEF)
                    { /* 是否为 LFN 序列的开始？ */
                        sum = dp->dir[LDIR_Chksum];
                        c &= (BYTE)~LLEF;
                        ord = c;                /* LFN 起始顺序 */
                        dp->blk_ofs = dp->dptr; /* LFN 起始偏移 */
                    }
                    /* 检查 LFN 条目的有效性并与给定名字比较 */
                    ord = (c == ord && sum == dp->dir[LDIR_Chksum] && cmp_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF;
                }
            }
            else
            {                             /* 找到 SFN 条目 */
                if (ord == 0 && sum == sum_sfn(dp->dir))
                    break;                /* LFN 匹配？ */
                if (!(dp->fn[NSFLAG] & NS_LOSS) && !memcmp(dp->dir, dp->fn, 11))
                    break;                /* SFN 匹配？ */
                ord = 0xFF;
                dp->blk_ofs = 0xFFFFFFFF; /* 重置 LFN 序列 */
            }
        }
#else                          /* 非 LFN 配置 */
        dp->obj.attr = dp->dir[DIR_Attr] & AM_MASK;
        if (!(dp->dir[DIR_Attr] & AM_VOL) && !memcmp(dp->dir, dp->fn, 11))
            break; /* 是否为有效条目？ */
#endif
        res = dir_next(dp, 0); /* 下一个条目 */
    } while (res == FR_OK);

    return res;
}

#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 将对象注册到目录中                                                     */
/*-----------------------------------------------------------------------*/
static FRESULT dir_register(DIR *dp)
{
    FRESULT res;
    FATFS *fs = dp->obj.fs;
    #if FF_USE_LFN /* LFN 配置 */
    UINT n, len, n_ent;
    BYTE sn[12], sum;

    if (dp->fn[NSFLAG] & (NS_DOT | NS_NONAME))
        return FR_INVALID_NAME;            /* 检查名字有效性 */
    for (len = 0; fs->lfnbuf[len]; len++); /* 获取 lfn 长度 */

        #if FF_FS_EXFAT
    if (fs->fs_type == FS_EXFAT)
    {                                /* 在 exFAT 卷上 */
        n_ent = (len + 14) / 15 + 2; /* 要分配的条目数 (85+C0+C1s) */
        res = dir_alloc(dp, n_ent);  /* 分配目录条目 */
        if (res != FR_OK)
            return res;
        dp->blk_ofs = dp->dptr - SZDIRE * (n_ent - 1); /* 设置分配的条目块偏移 */

        if (dp->obj.stat & 4)
        {                                    /* 目录是否因新分配而被扩展？ */
            dp->obj.stat &= ~4;
            res = fill_first_frag(&dp->obj); /* 如果需要，填充第一个片段 */
            if (res != FR_OK)
                return res;
            res = fill_last_frag(&dp->obj, dp->clust, 0xFFFFFFFF); /* 如果需要，填充最后一个片段 */
            if (res != FR_OK)
                return res;
            if (dp->obj.sclust != 0)
            { /* 是否为子目录？ */
                DIR dj;

                res = load_obj_xdir(&dj, &dp->obj); /* 加载对象状态 */
                if (res != FR_OK)
                    return res;
                dp->obj.objsize += (DWORD)fs->csize * SS(fs); /* 目录大小增加一个簇大小 */
                st_qword(fs->dirbuf + XDIR_FileSize, dp->obj.objsize);
                st_qword(fs->dirbuf + XDIR_ValidFileSize, dp->obj.objsize);
                fs->dirbuf[XDIR_GenFlags] = dp->obj.stat | 1; /* 更新分配状态 */
                res = store_xdir(&dj);                        /* 存储对象状态 */
                if (res != FR_OK)
                    return res;
            }
        }

        create_xdir(fs->dirbuf, fs->lfnbuf); /* 创建内存中的目录块，稍后写入 */
        return FR_OK;
    }
        #endif
    /* 在 FAT/FAT32 卷上 */
    memcpy(sn, dp->fn, 12);
    if (sn[NSFLAG] & NS_LOSS)
    {                              /* 当 LFN 不符合 8.3 格式时，生成编号名称 */
        dp->fn[NSFLAG] = NS_NOLFN; /* 仅查找 SFN */
        for (n = 1; n < 100; n++)
        {
            gen_numname(dp->fn, sn, fs->lfnbuf, n); /* 生成编号名称 */
            res = dir_find(dp);                     /* 检查名称是否与现有 SFN 冲突 */
            if (res != FR_OK)
                break;
        }
        if (n == 100)
            return FR_DENIED; /* 如果冲突过多，中止 */
        if (res != FR_NO_FILE)
            return res;       /* 如果结果不是“不冲突”，则中止 */
        dp->fn[NSFLAG] = sn[NSFLAG];
    }

    /* 创建带或不带 LFN 的 SFN */
    n_ent = (sn[NSFLAG] & NS_LFN) ? (len + 12) / 13 + 1 : 1; /* 要分配的条目数 */
    res = dir_alloc(dp, n_ent);                              /* 分配条目 */
    if (res == FR_OK && --n_ent)
    {                                                        /* 如果需要，设置 LFN 条目 */
        res = dir_sdi(dp, dp->dptr - n_ent * SZDIRE);
        if (res == FR_OK)
        {
            sum = sum_sfn(dp->fn); /* 与 LFN 关联的 SFN 的校验和值 */
            do
            {                      /* 从底部开始存储 LFN 条目 */
                res = move_window(fs, dp->sect);
                if (res != FR_OK)
                    break;
                put_lfn(fs->lfnbuf, dp->dir, (BYTE)n_ent, sum);
                fs->wflag = 1;
                res = dir_next(dp, 0); /* 下一个条目 */
            } while (res == FR_OK && --n_ent);
        }
    }

    #else /* 非 LFN 配置 */
    res = dir_alloc(dp, 1); /* 为 SFN 分配一个条目 */
    #endif

    /* 设置 SFN 条目 */
    if (res == FR_OK)
    {
        res = move_window(fs, dp->sect);
        if (res == FR_OK)
        {
            memset(dp->dir, 0, SZDIRE);                               /* 清除条目 */
            memcpy(dp->dir + DIR_Name, dp->fn, 11);                   /* 放入 SFN */
    #if FF_USE_LFN
            dp->dir[DIR_NTres] = dp->fn[NSFLAG] & (NS_BODY | NS_EXT); /* 放入 NT 标志 */
    #endif
            fs->wflag = 1;
        }
    }

    return res;
}
#endif /* !FF_FS_READONLY */

#if !FF_FS_READONLY && FF_FS_MINIMIZE == 0
/*-----------------------------------------------------------------------*/
/* 从目录中移除一个对象                                                   */
/*-----------------------------------------------------------------------*/
static FRESULT dir_remove(DIR *dp)
{
    FRESULT res;
    FATFS *fs = dp->obj.fs;
    #if FF_USE_LFN /* LFN 配置 */
    DWORD last = dp->dptr;

    res = (dp->blk_ofs == 0xFFFFFFFF) ? FR_OK : dir_sdi(dp, dp->blk_ofs); /* 如果存在 LFN，定位到条目块顶部 */
    if (res == FR_OK)
    {
        do
        {
            res = move_window(fs, dp->sect);
            if (res != FR_OK)
                break;
            if (FF_FS_EXFAT && fs->fs_type == FS_EXFAT)
            {                               /* 在 exFAT 卷上 */
                dp->dir[XDIR_Type] &= 0x7F; /* 清除条目 InUse 标志 */
            }
            else
            {                             /* 在 FAT/FAT32 卷上 */
                dp->dir[DIR_Name] = DDEM; /* 将条目标记为“已删除” */
            }
            fs->wflag = 1;
            if (dp->dptr >= last)
                break;             /* 如果到达最后一个条目，则对象的所有条目已被删除 */
            res = dir_next(dp, 0); /* 下一个条目 */
        } while (res == FR_OK);
        if (res == FR_NO_FILE)
            res = FR_INT_ERR;
    }
    #else /* 非 LFN 配置 */
    res = move_window(fs, dp->sect);
    if (res == FR_OK)
    {
        dp->dir[DIR_Name] = DDEM; /* 将条目标记为“已删除” */
        fs->wflag = 1;
    }
    #endif

    return res;
}
#endif /* !FF_FS_READONLY && FF_FS_MINIMIZE == 0 */

#if FF_FS_MINIMIZE <= 1 || FF_FS_RPATH >= 2
/*-----------------------------------------------------------------------*/
/* 从目录条目获取文件信息                                                  */
/*-----------------------------------------------------------------------*/
static void get_fileinfo(DIR *dp, FILINFO *fno)
{
    UINT si, di;
    #if FF_USE_LFN
    BYTE lcf;
    WCHAR wc, hs;
    FATFS *fs = dp->obj.fs;
    UINT nw;
    #else
    TCHAR c;
    #endif

    fno->fname[0] = 0; /* 使文件信息无效 */
    if (dp->sect == 0)
        return;        /* 如果读指针已到达目录末尾，退出 */

    #if FF_USE_LFN     /* LFN 配置 */
        #if FF_FS_EXFAT
    if (fs->fs_type == FS_EXFAT)
    { /* exFAT 卷 */
        UINT nc = 0;

        si = SZDIRE * 2;
        di = 0; /* 条目块中的第一个 C1 条目 */
        hs = 0;
        while (nc < fs->dirbuf[XDIR_NumName])
        {
            if (si >= MAXDIRB(FF_MAX_LFN))
            {
                di = 0;
                break;
            } /* 目录块被截断？ */
            if ((si % SZDIRE) == 0)
                si += 2; /* 跳过条目类型字段 */
            wc = ld_word(fs->dirbuf + si);
            si += 2;
            nc++;                                                                 /* 获取一个字符 */
            if (hs == 0 && IsSurrogate(wc))
            {                                                                     /* 是否为代理项？ */
                hs = wc;
                continue;                                                         /* 获取低代理项 */
            }
            nw = put_utf((DWORD)hs << 16 | wc, &fno->fname[di], FF_LFN_BUF - di); /* 以 API 编码存储 */
            if (nw == 0)
            {
                di = 0;
                break;
            } /* 缓冲区溢出或错误字符？ */
            di += nw;
            hs = 0;
        }
        if (hs != 0)
            di = 0;                                                                      /* 代理项对损坏？ */
        if (di == 0)
            fno->fname[di++] = '?';                                                      /* 无法访问的对象名？ */
        fno->fname[di] = 0;                                                              /* 终止名字 */
        fno->altname[0] = 0;                                                             /* exFAT 不支持 SFN */

        fno->fattrib = fs->dirbuf[XDIR_Attr] & AM_MASKX;                                 /* 属性 */
        fno->fsize = (fno->fattrib & AM_DIR) ? 0 : ld_qword(fs->dirbuf + XDIR_FileSize); /* 大小 */
        fno->ftime = ld_word(fs->dirbuf + XDIR_ModTime + 0);                             /* 时间 */
        fno->fdate = ld_word(fs->dirbuf + XDIR_ModTime + 2);                             /* 日期 */
        return;
    }
    else
        #endif
    {     /* FAT/FAT32 卷 */
        if (dp->blk_ofs != 0xFFFFFFFF)
        { /* 如果 LFN 可用，获取它 */
            si = di = 0;
            hs = 0;
            while (fs->lfnbuf[si] != 0)
            {
                wc = fs->lfnbuf[si++];                                                /* 获取一个 LFN 字符 (UTF-16) */
                if (hs == 0 && IsSurrogate(wc))
                {                                                                     /* 是否为代理项？ */
                    hs = wc;
                    continue;                                                         /* 获取低代理项 */
                }
                nw = put_utf((DWORD)hs << 16 | wc, &fno->fname[di], FF_LFN_BUF - di); /* 以 API 编码存储 */
                if (nw == 0)
                {
                    di = 0;
                    break;
                } /* 缓冲区溢出或错误字符？ */
                di += nw;
                hs = 0;
            }
            if (hs != 0)
                di = 0;         /* 代理项对损坏？ */
            fno->fname[di] = 0; /* 终止 LFN（空字符串表示 LFN 无效） */
        }
    }

    si = di = 0;
    while (si < 11)
    {                                 /* 从 SFN 条目获取 SFN */
        wc = dp->dir[si++];           /* 获取一个字符 */
        if (wc == ' ')
            continue;                 /* 跳过填充空格 */
        if (wc == RDDEM)
            wc = DDEM;                /* 恢复被替换的 DDEM 字符 */
        if (si == 9 && di < FF_SFN_BUF)
            fno->altname[di++] = '.'; /* 如果扩展名存在，插入点 */
        #if FF_LFN_UNICODE >= 1       /* Unicode 输出 */
        if (dbc_1st((BYTE)wc) && si != 8 && si != 11 && dbc_2nd(dp->dir[si]))
        {                             /* 如果需要，组成 DBCS */
            wc = wc << 8 | dp->dir[si++];
        }
        wc = ff_oem2uni(wc, CODEPAGE); /* ANSI/OEM -> Unicode */
        if (wc == 0)
        {
            di = 0;
            break;
        } /* 当前代码页中的错误字符？ */
        nw = put_utf(wc, &fno->altname[di], FF_SFN_BUF - di); /* 以 API 编码存储 */
        if (nw == 0)
        {
            di = 0;
            break;
        } /* 缓冲区溢出？ */
        di += nw;
        #else /* ANSI/OEM 输出 */
        fno->altname[di++] = (TCHAR)wc; /* 直接存储，不转换 */
        #endif
    }
    fno->altname[di] = 0; /* 终止 SFN（空字符串表示 SFN 无效） */

    if (fno->fname[0] == 0)
    {     /* 如果 LFN 无效，需要将 altname[] 复制到 fname[] */
        if (di == 0)
        { /* 如果 LFN 和 SFN 都无效，则该对象无法访问 */
            fno->fname[di++] = '?';
        }
        else
        {
            for (si = di = 0, lcf = NS_BODY; fno->altname[si]; si++, di++)
            { /* 将 altname[] 复制到 fname[]，同时处理大小写信息 */
                wc = (WCHAR)fno->altname[si];
                if (wc == '.')
                    lcf = NS_EXT;
                if (IsUpper(wc) && (dp->dir[DIR_NTres] & lcf))
                    wc += 0x20;
                fno->fname[di] = (TCHAR)wc;
            }
        }
        fno->fname[di] = 0;      /* 终止 LFN */
        if (!dp->dir[DIR_NTres])
            fno->altname[0] = 0; /* 如果没有 LFN 或大小写信息，不需要 altname */
    }

    #else /* 非 LFN 配置 */
    si = di = 0;
    while (si < 11)
    { /* 复制名字主体和扩展名 */
        c = (TCHAR)dp->dir[si++];
        if (c == ' ')
            continue;               /* 跳过填充空格 */
        if (c == RDDEM)
            c = DDEM;               /* 恢复被替换的 DDEM 字符 */
        if (si == 9)
            fno->fname[di++] = '.'; /* 如果扩展名存在，插入点 */
        fno->fname[di++] = c;
    }
    fno->fname[di] = 0; /* 终止 SFN */
    #endif

    fno->fattrib = dp->dir[DIR_Attr] & AM_MASK;      /* 属性 */
    fno->fsize = ld_dword(dp->dir + DIR_FileSize);   /* 大小 */
    fno->ftime = ld_word(dp->dir + DIR_ModTime + 0); /* 时间 */
    fno->fdate = ld_word(dp->dir + DIR_ModTime + 2); /* 日期 */
}
#endif                                               /* FF_FS_MINIMIZE <= 1 || FF_FS_RPATH >= 2 */

#if FF_USE_FIND && FF_FS_MINIMIZE <= 1
    /*-----------------------------------------------------------------------*/
    /* 模式匹配                                                              */
    /*-----------------------------------------------------------------------*/
    #define FIND_RECURS 4 /* 模式中通配符项的最大数量，用于限制递归 */

static DWORD get_achar(const TCHAR **ptr)
{
    DWORD chr;

    #if FF_USE_LFN && FF_LFN_UNICODE >= 1 /* Unicode 输入 */
    chr = tchar2uni(ptr);
    if (chr == 0xFFFFFFFF)
        chr = 0; /* 错误的 UTF 编码视为字符串结束 */
    chr = ff_wtoupper(chr);
    #else        /* ANSI/OEM 输入 */
    chr = (BYTE) * (*ptr)++; /* 获取一个字节 */
    if (IsLower(chr))
        chr -= 0x20;         /* 转换为大写 ASCII 字符 */
        #if FF_CODE_PAGE == 0
    if (ExCvt && chr >= 0x80)
        chr = ExCvt[chr - 0x80]; /* 转换为大写 SBCS 扩展字符 */
        #elif FF_CODE_PAGE < 900
    if (chr >= 0x80)
        chr = ExCvt[chr - 0x80]; /* 转换为大写 SBCS 扩展字符 */
        #endif
        #if FF_CODE_PAGE == 0 || FF_CODE_PAGE >= 900
    if (dbc_1st((BYTE)chr))
    { /* 如果需要，获取 DBCS 次字节 */
        chr = dbc_2nd((BYTE) * *ptr) ? chr << 8 | (BYTE) * (*ptr)++ : 0;
    }
        #endif
    #endif
    return chr;
}

static int pattern_match(const TCHAR *pat, const TCHAR *nam, UINT skip, UINT recur)
{
    const TCHAR *pptr, *nptr;
    DWORD pchr, nchr;
    UINT sk;

    while ((skip & 0xFF) != 0)
    {                 /* 预跳过名字字符 */
        if (!get_achar(&nam))
            return 0; /* 如果名字字符不足，分支不匹配 */
        skip--;
    }
    if (*pat == 0 && skip)
        return 1; /* 匹配？（短路） */

    do
    {
        pptr = pat;
        nptr = nam; /* 模式与名字的当前位置 */
        for (;;)
        {
            if (*pptr == '?' || *pptr == '*')
            {                 /* 通配符项？ */
                if (recur == 0)
                    return 0; /* 通配符项过多？ */
                sk = 0;
                do
                { /* 分析通配符项 */
                    if (*pptr++ == '?')
                        sk++;
                    else
                        sk |= 0x100;
                } while (*pptr == '?' || *pptr == '*');
                if (pattern_match(pptr, nptr, sk, recur - 1))
                    return 1;        /* 测试新分支（递归调用） */
                nchr = *nptr;
                break;               /* 分支不匹配 */
            }
            pchr = get_achar(&pptr); /* 获取模式字符 */
            nchr = get_achar(&nptr); /* 获取名字字符 */
            if (pchr != nchr)
                break;               /* 分支不匹配？ */
            if (pchr == 0)
                return 1;            /* 分支匹配？（两字符串均结束） */
        }
        get_achar(&nam);             /* nam++ */
    } while (skip && nchr); /* 如果指定了无限搜索，直到名字结束重试 */

    return 0;
}
#endif /* FF_USE_FIND && FF_FS_MINIMIZE <= 1 */

/*-----------------------------------------------------------------------*/
/* 选取一个顶级段并在目录格式中创建对象名称                                */
/*-----------------------------------------------------------------------*/
static FRESULT create_name(DIR *dp, const TCHAR **path)
{
#if FF_USE_LFN /* LFN 配置 */
    BYTE b, cf;
    WCHAR wc, *lfn;
    DWORD uc;
    UINT i, ni, si, di;
    const TCHAR *p;

    /* 将 LFN 创建到 LFN 工作缓冲区 */
    p = *path;
    lfn = dp->obj.fs->lfnbuf;
    di = 0;
    for (;;)
    {
        uc = tchar2uni(&p);                /* 获取一个字符 */
        if (uc == 0xFFFFFFFF)
            return FR_INVALID_NAME;        /* 无效代码或 UTF 解码错误 */
        if (uc >= 0x10000)
            lfn[di++] = (WCHAR)(uc >> 16); /* 如果需要，存储高代理项 */
        wc = (WCHAR)uc;
        if (wc < ' ' || IsSeparator(wc))
            break;                  /* 如果遇到路径结束或分隔符，停止 */
        if (wc < 0x80 && strchr("*:<>|\"\?\x7F", (int)wc))
            return FR_INVALID_NAME; /* 拒绝 LFN 的非法字符 */
        if (di >= FF_MAX_LFN)
            return FR_INVALID_NAME; /* 拒绝过长的名字 */
        lfn[di++] = wc;             /* 存储 Unicode 字符 */
    }
    if (wc < ' ')
    {                 /* 在路径结束时停止？ */
        cf = NS_LAST; /* 最后一个段 */
    }
    else
    {                                /* 在分隔符处停止 */
        while (IsSeparator(*p)) p++; /* 如果存在重复分隔符，跳过 */
        cf = 0;                      /* 可能后面有下一个段 */
        if (IsTerminator(*p))
            cf = NS_LAST;            /* 忽略终止分隔符 */
    }
    *path = p;                       /* 返回指向下一个段的指针 */

    #if FF_FS_RPATH != 0
    if ((di == 1 && lfn[di - 1] == '.') ||
        (di == 2 && lfn[di - 1] == '.' && lfn[di - 2] == '.'))
    { /* 此段是否为点名字？ */
        lfn[di] = 0;
        for (i = 0; i < 11; i++)
        { /* 为 SFN 条目创建点名字 */
            dp->fn[i] = (i < di) ? '.' : ' ';
        }
        dp->fn[i] = cf | NS_DOT; /* 这是点条目 */
        return FR_OK;
    }
    #endif
    while (di)
    { /* 如果存在，剪掉尾随的空格和点 */
        wc = lfn[di - 1];
        if (wc != ' ' && wc != '.')
            break;
        di--;
    }
    lfn[di] = 0;                /* LFN 已创建到工作缓冲区 */
    if (di == 0)
        return FR_INVALID_NAME; /* 拒绝空名字 */

    /* 以目录格式创建 SFN */
    for (si = 0; lfn[si] == ' '; si++);        /* 移除前导空格 */
    if (si > 0 || lfn[si] == '.')
        cf |= NS_LOSS | NS_LFN;                /* 是否有前导空格或点？ */
    while (di > 0 && lfn[di - 1] != '.') di--; /* 找到最后一个点（di<=si：无扩展名） */

    memset(dp->fn, ' ', 11);
    i = b = 0;
    ni = 8;
    for (;;)
    {
        wc = lfn[si++]; /* 获取一个 LFN 字符 */
        if (wc == 0)
            break;      /* 到达 LFN 末尾时停止 */
        if (wc == ' ' || (wc == '.' && si != di))
        {               /* 移除嵌入的空格和点 */
            cf |= NS_LOSS | NS_LFN;
            continue;
        }

        if (i >= ni || si == di)
        {     /* 字段结束？ */
            if (ni == 11)
            { /* 名字扩展溢出？ */
                cf |= NS_LOSS | NS_LFN;
                break;
            }
            if (si != di)
                cf |= NS_LOSS | NS_LFN; /* 名字主体溢出？ */
            if (si > di)
                break;                  /* 无名字扩展？ */
            si = di;
            i = 8;
            ni = 11;
            b <<= 2; /* 进入名字扩展 */
            continue;
        }

        if (wc >= 0x80)
        {                 /* 是否为扩展字符？ */
            cf |= NS_LFN; /* 需要创建 LFN 条目 */
    #if FF_CODE_PAGE == 0
            if (ExCvt)
            {                                  /* 在 SBCS 配置中 */
                wc = ff_uni2oem(wc, CODEPAGE); /* Unicode ==> ANSI/OEM 代码 */
                if (wc & 0x80)
                    wc = ExCvt[wc & 0x7F];     /* 将扩展字符转换为大写（SBCS） */
            }
            else
            {                                               /* 在 DBCS 配置中 */
                wc = ff_uni2oem(ff_wtoupper(wc), CODEPAGE); /* Unicode ==> 大写转换 ==> ANSI/OEM 代码 */
            }
    #elif FF_CODE_PAGE < 900                                /* 在 SBCS 配置中 */
            wc = ff_uni2oem(wc, CODEPAGE); /* Unicode ==> ANSI/OEM 代码 */
            if (wc & 0x80)
                wc = ExCvt[wc & 0x7F];     /* 将扩展字符转换为大写（SBCS） */
    #else                                                   /* 在 DBCS 配置中 */
            wc = ff_uni2oem(ff_wtoupper(wc), CODEPAGE); /* Unicode ==> 大写转换 ==> ANSI/OEM 代码 */
    #endif
        }

        if (wc >= 0x100)
        {     /* 是否为 DBCS 字符？ */
            if (i >= ni - 1)
            { /* 字段溢出？ */
                cf |= NS_LOSS | NS_LFN;
                i = ni;
                continue;                  /* 下一个字段 */
            }
            dp->fn[i++] = (BYTE)(wc >> 8); /* 放入首字节 */
        }
        else
        {                               /* SBCS 字符 */
            if (wc == 0 || strchr("+,;=[]", (int)wc))
            {                           /* 替换 SFN 的非法字符 */
                wc = '_';
                cf |= NS_LOSS | NS_LFN; /* 有损转换 */
            }
            else
            {
                if (IsUpper(wc))
                { /* ASCII 大写？ */
                    b |= 2;
                }
                if (IsLower(wc))
                { /* ASCII 小写？ */
                    b |= 1;
                    wc -= 0x20;
                }
            }
        }
        dp->fn[i++] = (BYTE)wc;
    }

    if (dp->fn[0] == DDEM)
        dp->fn[0] = RDDEM; /* 如果第一个字符与 DDEM 冲突，替换为 RDDEM */

    if (ni == 8)
        b <<= 2;           /* 如果没有扩展名，移位大写标志 */
    if ((b & 0x0C) == 0x0C || (b & 0x03) == 0x03)
        cf |= NS_LFN;      /* 如果复合大写，需要创建 LFN 条目 */
    if (!(cf & NS_LFN))
    {                      /* 当 LFN 为 8.3 格式且无扩展字符时，创建 NT 标志 */
        if (b & 0x01)
            cf |= NS_EXT;  /* NT 标志（扩展名只有小写字母） */
        if (b & 0x04)
            cf |= NS_BODY; /* NT 标志（主体只有小写字母） */
    }

    dp->fn[NSFLAG] = cf; /* SFN 已创建到 dp->fn[] */

    return FR_OK;

#else /* FF_USE_LFN：非 LFN 配置 */
    BYTE c, d, *sfn;
    UINT ni, si, i;
    const char *p;

    /* 以目录格式创建文件名 */
    p = *path;
    sfn = dp->fn;
    memset(sfn, ' ', 11);
    si = i = 0;
    ni = 8;
    #if FF_FS_RPATH != 0
    if (p[si] == '.')
    { /* 是否为点条目？ */
        for (;;)
        {
            c = (BYTE)p[si++];
            if (c != '.' || si >= 3)
                break;
            sfn[i++] = c;
        }
        if (!IsSeparator(c) && c > ' ')
            return FR_INVALID_NAME;
        *path = p + si;                                       /* 返回指向下一个段的指针 */
        sfn[NSFLAG] = (c <= ' ') ? NS_LAST | NS_DOT : NS_DOT; /* 如果路径结束，设置最后一个段标志 */
        return FR_OK;
    }
    #endif
    for (;;)
    {
        c = (BYTE)p[si++];                   /* 获取一个字节 */
        if (c <= ' ')
            break;                           /* 如果遇到路径名结束，停止 */
        if (IsSeparator(c))
        {                                    /* 如果遇到分隔符，停止 */
            while (IsSeparator(p[si])) si++; /* 如果存在重复分隔符，跳过 */
            break;
        }
        if (c == '.' || i >= ni)
        {                               /* 主体结束或字段溢出？ */
            if (ni == 11 || c != '.')
                return FR_INVALID_NAME; /* 字段溢出或无效的点？ */
            i = 8;
            ni = 11;                    /* 进入文件扩展名字段 */
            continue;
        }
    #if FF_CODE_PAGE == 0
        if (ExCvt && c >= 0x80)
        {                        /* 是否为 SBCS 扩展字符？ */
            c = ExCvt[c & 0x7F]; /* 转换为大写 SBCS 扩展字符 */
        }
    #elif FF_CODE_PAGE < 900
        if (c >= 0x80)
        {                        /* 是否为 SBCS 扩展字符？ */
            c = ExCvt[c & 0x7F]; /* 转换为大写 SBCS 扩展字符 */
        }
    #endif
        if (dbc_1st(c))
        {                               /* 检查是否为 DBCS 首字节 */
            d = (BYTE)p[si++];          /* 获取次字节 */
            if (!dbc_2nd(d) || i >= ni - 1)
                return FR_INVALID_NAME; /* 拒绝无效的 DBCS */
            sfn[i++] = c;
            sfn[i++] = d;
        }
        else
        {                               /* SBCS */
            if (strchr("*+,:;<=>[]|\"\?\x7F", (int)c))
                return FR_INVALID_NAME; /* 拒绝 SFN 的非法字符 */
            if (IsLower(c))
                c -= 0x20;              /* 转换为大写 */
            sfn[i++] = c;
        }
    }
    *path = &p[si];             /* 返回指向下一个段的指针 */
    if (i == 0)
        return FR_INVALID_NAME; /* 拒绝空字符串 */

    if (sfn[0] == DDEM)
        sfn[0] = RDDEM;                                     /* 如果第一个字符与 DDEM 冲突，替换为 RDDEM */
    sfn[NSFLAG] = (c <= ' ' || p[si] <= ' ') ? NS_LAST : 0; /* 如果路径结束，设置最后一个段标志 */

    return FR_OK;
#endif /* FF_USE_LFN */
}

/*-----------------------------------------------------------------------*/
/* 跟随文件路径                                                          */
/*-----------------------------------------------------------------------*/
static FRESULT follow_path(DIR *dp, const TCHAR *path)
{
    FRESULT res;
    BYTE ns;
    FATFS *fs = dp->obj.fs;

#if FF_FS_RPATH != 0
    if (!IsSeparator(*path) && (FF_STR_VOLUME_ID != 2 || !IsTerminator(*path)))
    {                              /* 无前导分隔符 */
        dp->obj.sclust = fs->cdir; /* 从当前目录开始 */
    }
    else
#endif
    {                                      /* 有前导分隔符 */
        while (IsSeparator(*path)) path++; /* 去除分隔符 */
        dp->obj.sclust = 0;                /* 从根目录开始 */
    }
#if FF_FS_EXFAT
    dp->obj.n_frag = 0; /* 使对象的最后一个片段计数器无效 */
    #if FF_FS_RPATH != 0
    if (fs->fs_type == FS_EXFAT && dp->obj.sclust)
    { /* exFAT：检索子目录的状态 */
        DIR dj;

        dp->obj.c_scl = fs->cdc_scl;
        dp->obj.c_size = fs->cdc_size;
        dp->obj.c_ofs = fs->cdc_ofs;
        res = load_obj_xdir(&dj, &dp->obj);
        if (res != FR_OK)
            return res;
        dp->obj.objsize = ld_dword(fs->dirbuf + XDIR_FileSize);
        dp->obj.stat = fs->dirbuf[XDIR_GenFlags] & 2;
    }
    #endif
#endif

    if ((UINT)*path < ' ')
    { /* 空路径名即当前目录本身 */
        dp->fn[NSFLAG] = NS_NONAME;
        res = dir_sdi(dp, 0);
    }
    else
    { /* 跟随路径 */
        for (;;)
        {
            res = create_name(dp, &path); /* 获取路径的一个段名 */
            if (res != FR_OK)
                break;
            res = dir_find(dp); /* 用该段名查找对象 */
            ns = dp->fn[NSFLAG];
            if (res != FR_OK)
            {                         /* 未能找到对象 */
                if (res == FR_NO_FILE)
                {                     /* 对象未找到 */
                    if (FF_FS_RPATH && (ns & NS_DOT))
                    {                 /* 如果点条目不存在，停留在那里 */
                        if (!(ns & NS_LAST))
                            continue; /* 如果不是最后一个段，继续跟随 */
                        dp->fn[NSFLAG] = NS_NONAME;
                        res = FR_OK;
                    }
                    else
                    {                         /* 无法找到对象 */
                        if (!(ns & NS_LAST))
                            res = FR_NO_PATH; /* 如果不是最后一个段，调整错误码 */
                    }
                }
                break;
            }
            if (ns & NS_LAST)
                break; /* 最后一个段匹配。函数完成 */
            /* 进入子目录 */
            if (!(dp->obj.attr & AM_DIR))
            { /* 不是子目录，无法继续 */
                res = FR_NO_PATH;
                break;
            }
#if FF_FS_EXFAT
            if (fs->fs_type == FS_EXFAT)
            { /* 保存包含目录信息以供下一个目录使用 */
                dp->obj.c_scl = dp->obj.sclust;
                dp->obj.c_size = ((DWORD)dp->obj.objsize & 0xFFFFFF00) | dp->obj.stat;
                dp->obj.c_ofs = dp->blk_ofs;
                init_alloc_info(fs, &dp->obj); /* 打开下一个目录 */
            }
            else
#endif
            {
                dp->obj.sclust = ld_clust(fs, fs->win + dp->dptr % SS(fs)); /* 打开下一个目录 */
            }
        }
    }
    return res;
}

/*-----------------------------------------------------------------------*/
/* 从路径名中获取逻辑驱动器号                                              */
/*-----------------------------------------------------------------------*/
static int get_ldnumber(const TCHAR **path)
{
    const TCHAR *tp, *tt;
    TCHAR tc;
    int i;
    int vol = -1;
#if FF_STR_VOLUME_ID /* 查找字符串卷 ID */
    const char *sp;
    char c;
#endif

    tt = tp = *path;
    if (!tp)
        return vol; /* 无效路径名？ */
    do tc = *tt++;
    while (!IsTerminator(tc) && tc != ':'); /* 在路径中查找冒号 */

    if (tc == ':')
    { /* DOS/Windows 风格卷 ID？ */
        i = FF_VOLUMES;
        if (IsDigit(*tp) && tp + 2 == tt)
        {                       /* 是否存在数字卷 ID + 冒号？ */
            i = (int)*tp - '0'; /* 获取 LD 号 */
        }
#if FF_STR_VOLUME_ID == 1       /* 启用任意字符串 */
        else
        {
            i = 0;
            do
            {
                sp = VolumeStr[i];
                tp = *path; /* 该字符串卷 ID 和路径名 */
                do
                {           /* 将卷 ID 与路径名比较 */
                    c = *sp++;
                    tc = *tp++;
                    if (IsLower(c))
                        c -= 0x20;
                    if (IsLower(tc))
                        tc -= 0x20;
                } while (c && (TCHAR)c == tc);
            } while ((c || tp != tt) && ++i < FF_VOLUMES); /* 对每个 ID 重复直到模式匹配 */
        }
#endif
        if (i < FF_VOLUMES)
        {               /* 如果找到卷 ID，获取驱动器号并去除它 */
            vol = i;    /* 驱动器号 */
            *path = tt; /* 去除驱动器前缀 */
        }
        return vol;
    }
#if FF_STR_VOLUME_ID == 2              /* Unix 风格卷 ID 启用 */
    if (*tp == '/')
    {                                  /* 是否存在卷 ID？ */
        while (*(tp + 1) == '/') tp++; /* 跳过重复分隔符 */
        i = 0;
        do
        {
            tt = tp;
            sp = VolumeStr[i]; /* 路径名和该字符串卷 ID */
            do
            {                  /* 将卷 ID 与路径名比较 */
                c = *sp++;
                tc = *(++tt);
                if (IsLower(c))
                    c -= 0x20;
                if (IsLower(tc))
                    tc -= 0x20;
            } while (c && (TCHAR)c == tc);
        } while ((c || (tc != '/' && !IsTerminator(tc))) && ++i < FF_VOLUMES); /* 对每个 ID 重复直到模式匹配 */
        if (i < FF_VOLUMES)
        {               /* 如果找到卷 ID，获取驱动器号并去除它 */
            vol = i;    /* 驱动器号 */
            *path = tt; /* 去除驱动器前缀 */
        }
        return vol;
    }
#endif
    /* 未找到驱动器前缀 */
#if FF_FS_RPATH != 0
    vol = CurrVol; /* 默认驱动器为当前驱动器 */
#else
    vol = 0; /* 默认驱动器为0 */
#endif
    return vol; /* 返回默认驱动器 */
}

/*-----------------------------------------------------------------------*/
/* GPT 支持函数                                                          */
/*-----------------------------------------------------------------------*/
#if FF_LBA64

/* 逐字节计算 CRC32 */
static DWORD crc32(DWORD crc, BYTE d)
{
    BYTE b;

    for (b = 1; b; b <<= 1)
    {
        crc ^= (d & b) ? 1 : 0;
        crc = (crc & 1) ? crc >> 1 ^ 0xEDB88320 : crc >> 1;
    }
    return crc;
}

/* 检查 GPT 头的有效性 */
static int test_gpt_header(const BYTE *gpth)
{
    UINT i;
    DWORD bcc;

    if (memcmp(gpth + GPTH_Sign, "EFI PART"
                                 "\0\0\1\0"
                                 "\x5C\0\0",
               16))
        return 0; /* 检查签名、版本 (1.0) 和长度 (92) */
    for (i = 0, bcc = 0xFFFFFFFF; i < 92; i++)
    {             /* 检查头 BCC */
        bcc = crc32(bcc, i - GPTH_Bcc < 4 ? 0 : gpth[i]);
    }
    if (~bcc != ld_dword(gpth + GPTH_Bcc))
        return 0;
    if (ld_dword(gpth + GPTH_PteSize) != SZ_GPTE)
        return 0; /* 表项大小（必须为 SZ_GPTE 字节） */
    if (ld_dword(gpth + GPTH_PtNum) > 128)
        return 0; /* 表大小（必须 <= 128 项） */

    return 1;
}

    #if !FF_FS_READONLY && FF_USE_MKFS
/* 生成随机值 */
static DWORD make_rand(DWORD seed, BYTE *buff, UINT n)
{
    UINT r;

    if (seed == 0)
        seed = 1;
    do
    {
        for (r = 0; r < 8; r++) seed = seed & 1 ? seed >> 1 ^ 0xA3000000 : seed >> 1; /* 移位 8 位 32 位 LFSR */
        *buff++ = (BYTE)seed;
    } while (--n);
    return seed;
}
    #endif
#endif

/*-----------------------------------------------------------------------*/
/* 加载一个扇区并检查是否为 FAT VBR                                        */
/*-----------------------------------------------------------------------*/

/* 检查扇区类型 */
static UINT check_fs(FATFS *fs, LBA_t sect)
{
    WORD w, sign;
    BYTE b;

    fs->wflag = 0;
    fs->winsect = (LBA_t)0 - 1; /* 使窗口无效 */
    if (move_window(fs, sect) != FR_OK)
        return 4;               /* 加载引导扇区 */
    sign = ld_word(fs->win + BS_55AA);
#if FF_FS_EXFAT
    if (sign == 0xAA55 && !memcmp(fs->win + BS_JmpBoot, "\xEB\x76\x90"
                                                        "EXFAT   ",
                                  11))
        return 1; /* 它是 exFAT VBR */
#endif
    b = fs->win[BS_JmpBoot];
    if (b == 0xEB || b == 0xE9 || b == 0xE8)
    { /* 有效的 JumpBoot 代码？（短跳转、近跳转或近调用） */
        if (sign == 0xAA55 && !memcmp(fs->win + BS_FilSysType32, "FAT32   ", 8))
        {
            return 0; /* 它是 FAT32 VBR */
        }
        /* 早期 MS-DOS 格式化的 FAT 卷缺少 BS_55AA 和 BS_FilSysType，因此需要不带它们识别 FAT VBR */
        w = ld_word(fs->win + BPB_BytsPerSec);
        b = fs->win[BPB_SecPerClus];
        if ((w & (w - 1)) == 0 && w >= FF_MIN_SS && w <= FF_MAX_SS                                     /* 扇区大小的正确性（512-4096 且为 2 的幂） */
            && b != 0 && (b & (b - 1)) == 0                                                            /* 簇大小的正确性（2 的幂） */
            && ld_word(fs->win + BPB_RsvdSecCnt) != 0                                                  /* 保留扇区的正确性（非零） */
            && (UINT)fs->win[BPB_NumFATs] - 1 <= 1                                                     /* FAT 数量的正确性（1 或 2） */
            && ld_word(fs->win + BPB_RootEntCnt) != 0                                                  /* 根目录条目数的正确性（非零） */
            && (ld_word(fs->win + BPB_TotSec16) >= 128 || ld_dword(fs->win + BPB_TotSec32) >= 0x10000) /* 卷扇区数的正确性（>=128） */
            && ld_word(fs->win + BPB_FATSz16) != 0)
        {                                                                                              /* FAT 大小的正确性（非零） */
            return 0;                                                                                  /* 可以推测为 FAT VBR */
        }
    }
    return sign == 0xAA55 ? 2 : 3; /* 不是 FAT VBR（有效或无效的 BS） */
}

/* 查找 FAT 卷 */
/* （仅支持通用分区规则：MBR、GPT 和 SFD） */
static UINT find_volume(FATFS *fs, UINT part)
{
    UINT fmt, i;
    DWORD mbr_pt[4];

    fmt = check_fs(fs, 0); /* 加载扇区 0 并检查是否为 FAT VBR（SFD 格式） */
    if (fmt != 2 && (fmt >= 3 || part == 0))
        return fmt;        /* 如果是 FAT VBR（自动扫描），非 BS 或磁盘错误，则返回 */

    /* 扇区 0 不是 FAT VBR，或者强制分区号需要分区 */

#if FF_LBA64
    if (fs->win[MBR_Table + PTE_System] == 0xEE)
    { /* GPT 保护 MBR？ */
        DWORD n_ent, v_ent, ofs;
        QWORD pt_lba;

        if (move_window(fs, 1) != FR_OK)
            return 4;                                                      /* 加载 GPT 头扇区（MBR 后一扇区） */
        if (!test_gpt_header(fs->win))
            return 3;                                                      /* 检查 GPT 头是否有效 */
        n_ent = ld_dword(fs->win + GPTH_PtNum);                            /* 条目数 */
        pt_lba = ld_qword(fs->win + GPTH_PtOfs);                           /* 表位置 */
        for (v_ent = i = 0; i < n_ent; i++)
        {                                                                  /* 查找 FAT 分区 */
            if (move_window(fs, pt_lba + i * SZ_GPTE / SS(fs)) != FR_OK)
                return 4;                                                  /* PT 扇区 */
            ofs = i * SZ_GPTE % SS(fs);                                    /* 扇区内偏移 */
            if (!memcmp(fs->win + ofs + GPTE_PtGuid, GUID_MS_Basic, 16))
            {                                                              /* MS 基本数据分区？ */
                v_ent++;
                fmt = check_fs(fs, ld_qword(fs->win + ofs + GPTE_FstLba)); /* 加载 VBR 并检查状态 */
                if (part == 0 && fmt <= 1)
                    return fmt;                                            /* 自动搜索（先找到的有效 FAT 卷） */
                if (part != 0 && v_ent == part)
                    return fmt;                                            /* 强制分区顺序（无论是否有效） */
            }
        }
        return 3; /* 未找到 */
    }
#endif
    if (FF_MULTI_PARTITION && part > 4)
        return 3; /* MBR 最多 4 个分区 */
    for (i = 0; i < 4; i++)
    {             /* 在 MBR 中加载分区偏移 */
        mbr_pt[i] = ld_dword(fs->win + MBR_Table + i * SZ_PTE + PTE_StLba);
    }
    i = part ? part - 1 : 0;                           /* 要查找的第一个表的索引 */
    do
    {                                                  /* 查找 FAT 卷 */
        fmt = mbr_pt[i] ? check_fs(fs, mbr_pt[i]) : 3; /* 检查分区是否为 FAT */
    } while (part == 0 && fmt >= 2 && ++i < 4);
    return fmt;
}

/*-----------------------------------------------------------------------*/
/* 确定逻辑驱动器号并在需要时挂载卷                                       */
/*-----------------------------------------------------------------------*/
static FRESULT mount_volume(const TCHAR **path, FATFS **rfs, BYTE mode)
{
    int vol;
    DSTATUS stat;
    LBA_t bsect;
    DWORD tsect, sysect, fasize, nclst, szbfat;
    WORD nrsv;
    FATFS *fs;
    UINT fmt;

    /* 获取逻辑驱动器号 */
    *rfs = 0;
    vol = get_ldnumber(path);
    if (vol < 0)
        return FR_INVALID_DRIVE;

    /* 检查文件系统对象是否有效 */
    fs = FatFs[vol];           /* 获取指向文件系统对象的指针 */
    if (!fs)
        return FR_NOT_ENABLED; /* 文件系统对象是否可用？ */
#if FF_FS_REENTRANT
    if (!lock_fs(fs))
        return FR_TIMEOUT;  /* 锁定卷 */
#endif
    *rfs = fs;              /* 返回指向文件系统对象的指针 */

    mode &= (BYTE)~FA_READ; /* 期望的访问模式，是否写访问 */
    if (fs->fs_type != 0)
    {                       /* 如果卷已挂载 */
        stat = disk_status(fs->pdrv);
        if (!(stat & STA_NOINIT))
        {     /* 并且物理驱动器保持初始化 */
            if (!FF_FS_READONLY && mode && (stat & STA_PROTECT))
            { /* 如果需要，检查写保护 */
                return FR_WRITE_PROTECTED;
            }
            return FR_OK; /* 文件系统对象已有效 */
        }
    }

    /* 文件系统对象无效 */
    /* 以下代码尝试挂载卷。（查找 FAT 卷，分析 BPB 并初始化文件系统对象） */

    fs->fs_type = 0;                  /* 清除文件系统对象 */
    fs->pdrv = LD2PD(vol);            /* 卷所在的物理驱动器 */
    stat = disk_initialize(fs->pdrv); /* 初始化物理驱动器 */
    if (stat & STA_NOINIT)
    {                                 /* 检查初始化是否成功 */
        return FR_NOT_READY;          /* 由于无介质或硬件错误导致初始化失败 */
    }
    if (!FF_FS_READONLY && mode && (stat & STA_PROTECT))
    { /* 如果需要，检查磁盘写保护 */
        return FR_WRITE_PROTECTED;
    }
#if FF_MAX_SS != FF_MIN_SS /* 获取扇区大小（仅多扇区大小配置） */
    if (disk_ioctl(fs->pdrv, GET_SECTOR_SIZE, &SS(fs)) != RES_OK)
        return FR_DISK_ERR;
    if (SS(fs) > FF_MAX_SS || SS(fs) < FF_MIN_SS || (SS(fs) & (SS(fs) - 1)))
        return FR_DISK_ERR;
#endif

    /* 在驱动器上查找 FAT 卷 */
    fmt = find_volume(fs, LD2PT(vol));
    if (fmt == 4)
        return FR_DISK_ERR;      /* 磁盘 I/O 层发生错误 */
    if (fmt >= 2)
        return FR_NO_FILESYSTEM; /* 未找到 FAT 卷 */
    bsect = fs->winsect;         /* 卷偏移 */

    /* 找到 FAT 卷（bsect）。以下代码初始化文件系统对象 */

#if FF_FS_EXFAT
    if (fmt == 1)
    {
        QWORD maxlba;
        DWORD so, cv, bcl, i;

        for (i = BPB_ZeroedEx; i < BPB_ZeroedEx + 53 && fs->win[i] == 0; i++); /* 检查零填充 */
        if (i < BPB_ZeroedEx + 53)
            return FR_NO_FILESYSTEM;

        if (ld_word(fs->win + BPB_FSVerEx) != 0x100)
            return FR_NO_FILESYSTEM; /* 检查 exFAT 版本（必须为 1.0） */

        if (1 << fs->win[BPB_BytsPerSecEx] != SS(fs))
        { /* (BPB_BytsPerSecEx 必须等于物理扇区大小) */
            return FR_NO_FILESYSTEM;
        }

        maxlba = ld_qword(fs->win + BPB_TotSecEx) + bsect; /* 卷的最后 LBA + 1 */
        if (!FF_LBA64 && maxlba >= 0x100000000)
            return FR_NO_FILESYSTEM;                       /* （无法在 32 位 LBA 中访问） */

        fs->fsize = ld_dword(fs->win + BPB_FatSzEx);       /* 每个 FAT 的扇区数 */

        fs->n_fats = fs->win[BPB_NumFATsEx];               /* FAT 数量 */
        if (fs->n_fats != 1)
            return FR_NO_FILESYSTEM;                       /* （仅支持 1 个 FAT） */

        fs->csize = 1 << fs->win[BPB_SecPerClusEx];        /* 簇大小 */
        if (fs->csize == 0)
            return FR_NO_FILESYSTEM;                       /* （必须为 1..32768 扇区） */

        nclst = ld_dword(fs->win + BPB_NumClusEx);         /* 簇数量 */
        if (nclst > MAX_EXFAT)
            return FR_NO_FILESYSTEM;                       /* （簇过多） */
        fs->n_fatent = nclst + 2;

        /* 边界和限制 */
        fs->volbase = bsect;
        fs->database = bsect + ld_dword(fs->win + BPB_DataOfsEx);
        fs->fatbase = bsect + ld_dword(fs->win + BPB_FatOfsEx);
        if (maxlba < (QWORD)fs->database + nclst * fs->csize)
            return FR_NO_FILESYSTEM; /* （卷大小不能小于所需大小） */
        fs->dirbase = ld_dword(fs->win + BPB_RootClusEx);

        /* 获取位图位置并检查是否连续（实现假设） */
        so = i = 0;
        for (;;)
        { /* 在根目录中查找位图条目（仅第一个簇中） */
            if (i == 0)
            {
                if (so >= fs->csize)
                    return FR_NO_FILESYSTEM; /* 未找到？ */
                if (move_window(fs, clst2sect(fs, (DWORD)fs->dirbase) + so) != FR_OK)
                    return FR_DISK_ERR;
                so++;
            }
            if (fs->win[i] == ET_BITMAP)
                break;                                      /* 是否为位图条目？ */
            i = (i + SZDIRE) % SS(fs);                      /* 下一个条目 */
        }
        bcl = ld_dword(fs->win + i + 20);                   /* 位图簇 */
        if (bcl < 2 || bcl >= fs->n_fatent)
            return FR_NO_FILESYSTEM;                        /* （错误的簇#） */
        fs->bitbase = fs->database + fs->csize * (bcl - 2); /* 位图扇区 */
        for (;;)
        {                                                   /* 检查位图是否连续 */
            if (move_window(fs, fs->fatbase + bcl / (SS(fs) / 4)) != FR_OK)
                return FR_DISK_ERR;
            cv = ld_dword(fs->win + bcl % (SS(fs) / 4) * 4);
            if (cv == 0xFFFFFFFF)
                break;                   /* 最后一个链接？ */
            if (cv != ++bcl)
                return FR_NO_FILESYSTEM; /* 碎片化？ */
        }

    #if !FF_FS_READONLY
        fs->last_clst = fs->free_clst = 0xFFFFFFFF; /* 初始化簇分配信息 */
    #endif
        fmt = FS_EXFAT;                             /* FAT 子类型 */
    }
    else
#endif /* FF_FS_EXFAT */
    {
        if (ld_word(fs->win + BPB_BytsPerSec) != SS(fs))
            return FR_NO_FILESYSTEM;             /* (BPB_BytsPerSec 必须等于物理扇区大小) */

        fasize = ld_word(fs->win + BPB_FATSz16); /* 每个 FAT 的扇区数 */
        if (fasize == 0)
            fasize = ld_dword(fs->win + BPB_FATSz32);
        fs->fsize = fasize;

        fs->n_fats = fs->win[BPB_NumFATs];                 /* FAT 数量 */
        if (fs->n_fats != 1 && fs->n_fats != 2)
            return FR_NO_FILESYSTEM;                       /* （必须为 1 或 2） */
        fasize *= fs->n_fats;                              /* FAT 区的扇区数 */

        fs->csize = fs->win[BPB_SecPerClus];               /* 簇大小 */
        if (fs->csize == 0 || (fs->csize & (fs->csize - 1)))
            return FR_NO_FILESYSTEM;                       /* （必须为 2 的幂） */

        fs->n_rootdir = ld_word(fs->win + BPB_RootEntCnt); /* 根目录条目数 */
        if (fs->n_rootdir % (SS(fs) / SZDIRE))
            return FR_NO_FILESYSTEM;                       /* （必须扇区对齐） */

        tsect = ld_word(fs->win + BPB_TotSec16);           /* 卷上的扇区数 */
        if (tsect == 0)
            tsect = ld_dword(fs->win + BPB_TotSec32);

        nrsv = ld_word(fs->win + BPB_RsvdSecCnt); /* 保留扇区数 */
        if (nrsv == 0)
            return FR_NO_FILESYSTEM;              /* （不能为 0） */

        /* 确定 FAT 子类型 */
        sysect = nrsv + fasize + fs->n_rootdir / (SS(fs) / SZDIRE); /* RSV + FAT + DIR */
        if (tsect < sysect)
            return FR_NO_FILESYSTEM;                                /* （无效卷大小） */
        nclst = (tsect - sysect) / fs->csize;                       /* 簇数 */
        if (nclst == 0)
            return FR_NO_FILESYSTEM;                                /* （无效卷大小） */
        fmt = 0;
        if (nclst <= MAX_FAT32)
            fmt = FS_FAT32;
        if (nclst <= MAX_FAT16)
            fmt = FS_FAT16;
        if (nclst <= MAX_FAT12)
            fmt = FS_FAT12;
        if (fmt == 0)
            return FR_NO_FILESYSTEM;

        /* 边界和限制 */
        fs->n_fatent = nclst + 2;      /* FAT 条目数 */
        fs->volbase = bsect;           /* 卷起始扇区 */
        fs->fatbase = bsect + nrsv;    /* FAT 起始扇区 */
        fs->database = bsect + sysect; /* 数据区起始扇区 */
        if (fmt == FS_FAT32)
        {
            if (ld_word(fs->win + BPB_FSVer32) != 0)
                return FR_NO_FILESYSTEM;                      /* （必须为 FAT32 修订版 0.0） */
            if (fs->n_rootdir != 0)
                return FR_NO_FILESYSTEM;                      /* (BPB_RootEntCnt 必须为 0) */
            fs->dirbase = ld_dword(fs->win + BPB_RootClus32); /* 根目录起始簇 */
            szbfat = fs->n_fatent * 4;                        /* （所需 FAT 大小） */
        }
        else
        {
            if (fs->n_rootdir == 0)
                return FR_NO_FILESYSTEM;        /* (BPB_RootEntCnt 不能为 0) */
            fs->dirbase = fs->fatbase + fasize; /* 根目录起始扇区 */
            szbfat = (fmt == FS_FAT16) ?        /* （所需 FAT 大小） */
                         fs->n_fatent * 2
                                       : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
        }
        if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs))
            return FR_NO_FILESYSTEM; /* (BPB_FATSz 不能小于所需大小) */

#if !FF_FS_READONLY
        /* 如果可用，获取 FSInfo */
        fs->last_clst = fs->free_clst = 0xFFFFFFFF; /* 初始化簇分配信息 */
        fs->fsi_flag = 0x80;
    #if (FF_FS_NOFSINFO & 3) != 3
        if (fmt == FS_FAT32 /* 仅当 BPB_FSInfo32 == 1 时才允许更新 FSInfo */
            && ld_word(fs->win + BPB_FSInfo32) == 1 && move_window(fs, bsect + 1) == FR_OK)
        {
            fs->fsi_flag = 0;
            if (ld_word(fs->win + BS_55AA) == 0xAA55 /* 如果可用，加载 FSInfo 数据 */
                && ld_dword(fs->win + FSI_LeadSig) == 0x41615252 && ld_dword(fs->win + FSI_StrucSig) == 0x61417272)
            {
        #if (FF_FS_NOFSINFO & 1) == 0
                fs->free_clst = ld_dword(fs->win + FSI_Free_Count);
        #endif
        #if (FF_FS_NOFSINFO & 2) == 0
                fs->last_clst = ld_dword(fs->win + FSI_Nxt_Free);
        #endif
            }
        }
    #endif /* (FF_FS_NOFSINFO & 3) != 3 */
#endif     /* !FF_FS_READONLY */
    }

    fs->fs_type = (BYTE)fmt; /* FAT 子类型 */
    fs->id = ++Fsid;         /* 卷挂载 ID */
#if FF_USE_LFN == 1
    fs->lfnbuf = LfnBuf;     /* 静态 LFN 工作缓冲区 */
    #if FF_FS_EXFAT
    fs->dirbuf = DirBuf;     /* 静态目录块暂存缓冲区 */
    #endif
#endif
#if FF_FS_RPATH != 0
    fs->cdir = 0;   /* 初始化当前目录 */
#endif
#if FF_FS_LOCK != 0 /* 清除文件锁信号量 */
    clear_lock(fs);
#endif
    return FR_OK;
}

/*-----------------------------------------------------------------------*/
/* 检查文件/目录对象是否有效                                              */
/*-----------------------------------------------------------------------*/
static FRESULT validate(FFOBJID *obj, FATFS **rfs)
{
    FRESULT res = FR_INVALID_OBJECT;

    if (obj && obj->fs && obj->fs->fs_type && obj->id == obj->fs->id)
    { /* 测试对象是否有效 */
#if FF_FS_REENTRANT
        if (lock_fs(obj->fs))
        {     /* 获取文件系统对象 */
            if (!(disk_status(obj->fs->pdrv) & STA_NOINIT))
            { /* 测试物理驱动器是否保持初始化 */
                res = FR_OK;
            }
            else
            {
                unlock_fs(obj->fs, FR_OK);
            }
        }
        else
        {
            res = FR_TIMEOUT;
        }
#else
        if (!(disk_status(obj->fs->pdrv) & STA_NOINIT))
        { /* 测试物理驱动器是否保持初始化 */
            res = FR_OK;
        }
#endif
    }
    *rfs = (res == FR_OK) ? obj->fs : 0; /* 对应的文件系统对象 */
    return res;
}

/*---------------------------------------------------------------------------

   公共函数（FatFs API）

----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* 挂载/卸载逻辑驱动器                                                    */
/*-----------------------------------------------------------------------*/
FRESULT f_mount(FATFS *fs, const TCHAR *path, BYTE opt)
{
    FATFS *cfs;
    int vol;
    FRESULT res;
    const TCHAR *rp = path;

    /* 获取逻辑驱动器号 */
    vol = get_ldnumber(&rp);
    if (vol < 0)
        return FR_INVALID_DRIVE;
    cfs = FatFs[vol]; /* 指向 fs 对象的指针 */

    if (cfs)
    {
#if FF_FS_LOCK != 0
        clear_lock(cfs);
#endif
#if FF_FS_REENTRANT /* 丢弃当前卷的同步对象 */
        if (!ff_del_syncobj(cfs->sobj))
            return FR_INT_ERR;
#endif
        cfs->fs_type = 0; /* 清除旧 fs 对象 */
    }

    if (fs)
    {
        fs->fs_type = 0; /* 清除新 fs 对象 */
#if FF_FS_REENTRANT      /* 为新卷创建同步对象 */
        if (!ff_cre_syncobj((BYTE)vol, &fs->sobj))
            return FR_INT_ERR;
#endif
    }
    FatFs[vol] = fs; /* 注册新 fs 对象 */

    if (opt == 0)
        return FR_OK;                  /* 现在不挂载，稍后挂载 */

    res = mount_volume(&path, &fs, 0); /* 强制挂载卷 */
    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 打开或创建文件                                                         */
/*-----------------------------------------------------------------------*/
FRESULT f_open(FIL *fp, const TCHAR *path, BYTE mode)
{
    FRESULT res;
    DIR dj;
    FATFS *fs;
#if !FF_FS_READONLY
    DWORD cl, bcs, clst, tm;
    LBA_t sc;
    FSIZE_t ofs;
#endif
    DEF_NAMBUF

    if (!fp)
        return FR_INVALID_OBJECT;

    /* 获取逻辑驱动器号 */
    mode &= FF_FS_READONLY ? FA_READ : FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_CREATE_NEW | FA_OPEN_ALWAYS | FA_OPEN_APPEND;
    res = mount_volume(&path, &fs, mode);
    if (res == FR_OK)
    {
        dj.obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(&dj, path); /* 跟随文件路径 */
#if !FF_FS_READONLY                   /* 读/写配置 */
        if (res == FR_OK)
        {
            if (dj.fn[NSFLAG] & NS_NONAME)
            { /* 当前目录本身？ */
                res = FR_INVALID_NAME;
            }
    #if FF_FS_LOCK != 0
            else
            {
                res = chk_lock(&dj, (mode & ~FA_READ) ? 1 : 0); /* 检查文件是否可以使用 */
            }
    #endif
        }
        /* 创建或打开文件 */
        if (mode & (FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW))
        {
            if (res != FR_OK)
            {     /* 没有文件，创建新文件 */
                if (res == FR_NO_FILE)
                { /* 没有文件可打开，创建新条目 */
    #if FF_FS_LOCK != 0
                    res = enq_lock() ? dir_register(&dj) : FR_TOO_MANY_OPEN_FILES;
    #else
                    res = dir_register(&dj);
    #endif
                }
                mode |= FA_CREATE_ALWAYS; /* 文件已创建 */
            }
            else
            {     /* 已存在同名对象 */
                if (dj.obj.attr & (AM_RDO | AM_DIR))
                { /* 无法覆盖（只读或目录） */
                    res = FR_DENIED;
                }
                else
                {
                    if (mode & FA_CREATE_NEW)
                        res = FR_EXIST; /* 无法创建为新文件 */
                }
            }
            if (res == FR_OK && (mode & FA_CREATE_ALWAYS))
            { /* 如果覆盖模式，截断文件 */
    #if FF_FS_EXFAT
                if (fs->fs_type == FS_EXFAT)
                {
                    /* 获取当前分配信息 */
                    fp->obj.fs = fs;
                    init_alloc_info(fs, &fp->obj);
                    /* 设置目录项块初始状态 */
                    memset(fs->dirbuf + 2, 0, 30);  /* 清除 85 条目，除 NumSec 外 */
                    memset(fs->dirbuf + 38, 0, 26); /* 清除 C0 条目，除 NumName 和 NameHash 外 */
                    fs->dirbuf[XDIR_Attr] = AM_ARC;
                    st_dword(fs->dirbuf + XDIR_CrtTime, GET_FATTIME());
                    fs->dirbuf[XDIR_GenFlags] = 1;
                    res = store_xdir(&dj);
                    if (res == FR_OK && fp->obj.sclust != 0)
                    {                                       /* 如果存在簇链，移除它 */
                        res = remove_chain(&fp->obj, fp->obj.sclust, 0);
                        fs->last_clst = fp->obj.sclust - 1; /* 重用簇洞 */
                    }
                }
                else
    #endif
                {
                    /* 设置目录项初始状态 */
                    tm = GET_FATTIME(); /* 设置创建时间 */
                    st_dword(dj.dir + DIR_CrtTime, tm);
                    st_dword(dj.dir + DIR_ModTime, tm);
                    cl = ld_clust(fs, dj.dir); /* 获取当前簇链 */
                    dj.dir[DIR_Attr] = AM_ARC; /* 重置属性 */
                    st_clust(fs, dj.dir, 0);   /* 重置文件分配信息 */
                    st_dword(dj.dir + DIR_FileSize, 0);
                    fs->wflag = 1;
                    if (cl != 0)
                    { /* 如果存在簇链，移除它 */
                        sc = fs->winsect;
                        res = remove_chain(&dj.obj, cl, 0);
                        if (res == FR_OK)
                        {
                            res = move_window(fs, sc);
                            fs->last_clst = cl - 1; /* 重用簇洞 */
                        }
                    }
                }
            }
        }
        else
        {         /* 打开现有文件 */
            if (res == FR_OK)
            {     /* 对象是否存在？ */
                if (dj.obj.attr & AM_DIR)
                { /* 对目录打开文件 */
                    res = FR_NO_FILE;
                }
                else
                {
                    if ((mode & FA_WRITE) && (dj.obj.attr & AM_RDO))
                    { /* 写模式打开只读文件 */
                        res = FR_DENIED;
                    }
                }
            }
        }
        if (res == FR_OK)
        {
            if (mode & FA_CREATE_ALWAYS)
                mode |= FA_MODIFIED;    /* 如果创建或覆盖，设置文件更改标志 */
            fp->dir_sect = fs->winsect; /* 指向目录条目 */
            fp->dir_ptr = dj.dir;
    #if FF_FS_LOCK != 0
            fp->obj.lockid = inc_lock(&dj, (mode & ~FA_READ) ? 1 : 0); /* 为此会话锁定文件 */
            if (fp->obj.lockid == 0)
                res = FR_INT_ERR;
    #endif
        }
#else /* 只读配置 */
        if (res == FR_OK)
        {
            if (dj.fn[NSFLAG] & NS_NONAME)
            { /* 是否为当前目录本身？ */
                res = FR_INVALID_NAME;
            }
            else
            {
                if (dj.obj.attr & AM_DIR)
                { /* 是否为目录？ */
                    res = FR_NO_FILE;
                }
            }
        }
#endif

        if (res == FR_OK)
        {
#if FF_FS_EXFAT
            if (fs->fs_type == FS_EXFAT)
            {
                fp->obj.c_scl = dj.obj.sclust; /* 获取包含目录信息 */
                fp->obj.c_size = ((DWORD)dj.obj.objsize & 0xFFFFFF00) | dj.obj.stat;
                fp->obj.c_ofs = dj.blk_ofs;
                init_alloc_info(fs, &fp->obj);
            }
            else
#endif
            {
                fp->obj.sclust = ld_clust(fs, dj.dir); /* 获取对象分配信息 */
                fp->obj.objsize = ld_dword(dj.dir + DIR_FileSize);
            }
#if FF_USE_FASTSEEK
            fp->cltbl = 0;   /* 禁用快速查找模式 */
#endif
            fp->obj.fs = fs; /* 验证文件对象 */
            fp->obj.id = fs->id;
            fp->flag = mode; /* 设置文件访问模式 */
            fp->err = 0;     /* 清除错误标志 */
            fp->sect = 0;    /* 使当前数据扇区无效 */
            fp->fptr = 0;    /* 将文件指针设置到文件顶部 */
#if !FF_FS_READONLY
    #if !FF_FS_TINY
            memset(fp->buf, 0, sizeof fp->buf); /* 清除扇区缓冲区 */
    #endif
            if ((mode & FA_SEEKEND) && fp->obj.objsize > 0)
            {                                    /* 如果指定了 FA_OPEN_APPEND，定位到文件末尾 */
                fp->fptr = fp->obj.objsize;      /* 要查找的偏移 */
                bcs = (DWORD)fs->csize * SS(fs); /* 簇大小（字节） */
                clst = fp->obj.sclust;           /* 沿着簇链 */
                for (ofs = fp->obj.objsize; res == FR_OK && ofs > bcs; ofs -= bcs)
                {
                    clst = get_fat(&fp->obj, clst);
                    if (clst <= 1)
                        res = FR_INT_ERR;
                    if (clst == 0xFFFFFFFF)
                        res = FR_DISK_ERR;
                }
                fp->clust = clst;
                if (res == FR_OK && ofs % SS(fs))
                { /* 如果不在扇区边界，填充扇区缓冲区 */
                    sc = clst2sect(fs, clst);
                    if (sc == 0)
                    {
                        res = FR_INT_ERR;
                    }
                    else
                    {
                        fp->sect = sc + (DWORD)(ofs / SS(fs));
    #if !FF_FS_TINY
                        if (disk_read(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
                            res = FR_DISK_ERR;
    #endif
                    }
                }
    #if FF_FS_LOCK != 0
                if (res != FR_OK)
                    dec_lock(fp->obj.lockid); /* 如果查找失败，减少文件打开计数器 */
    #endif
            }
#endif
        }

        FREE_NAMBUF();
    }

    if (res != FR_OK)
        fp->obj.fs = 0; /* 出错时使文件对象无效 */

    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 读取文件                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_read(FIL *fp, void *buff, UINT btr, UINT *br)
{
    FRESULT res;
    FATFS *fs;
    DWORD clst;
    LBA_t sect;
    FSIZE_t remain;
    UINT rcnt, cc, csect;
    BYTE *rbuff = (BYTE *)buff;

    *br = 0;                       /* 清除读取字节计数器 */
    res = validate(&fp->obj, &fs); /* 检查文件对象的有效性 */
    if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK)
        LEAVE_FF(fs, res);         /* 检查有效性 */
    if (!(fp->flag & FA_READ))
        LEAVE_FF(fs, FR_DENIED);   /* 检查访问模式 */
    remain = fp->obj.objsize - fp->fptr;
    if (btr > remain)
        btr = (UINT)remain; /* 按剩余字节截断 btr */

    for (; btr > 0; btr -= rcnt, *br += rcnt, rbuff += rcnt, fp->fptr += rcnt)
    {                                                            /* 重复直到读取 btr 字节 */
        if (fp->fptr % SS(fs) == 0)
        {                                                        /* 在扇区边界？ */
            csect = (UINT)(fp->fptr / SS(fs) & (fs->csize - 1)); /* 簇内扇区偏移 */
            if (csect == 0)
            {                                                    /* 在簇边界？ */
                if (fp->fptr == 0)
                {                                                /* 在文件顶部？ */
                    clst = fp->obj.sclust;                       /* 从起点沿着簇链 */
                }
                else
                { /* 文件中间或末尾 */
#if FF_USE_FASTSEEK
                    if (fp->cltbl)
                    {
                        clst = clmt_clust(fp, fp->fptr); /* 从 CLMT 获取簇# */
                    }
                    else
#endif
                    {
                        clst = get_fat(&fp->obj, fp->clust); /* 沿着 FAT 上的簇链 */
                    }
                }
                if (clst < 2)
                    ABORT(fs, FR_INT_ERR);
                if (clst == 0xFFFFFFFF)
                    ABORT(fs, FR_DISK_ERR);
                fp->clust = clst;            /* 更新当前簇 */
            }
            sect = clst2sect(fs, fp->clust); /* 获取当前扇区 */
            if (sect == 0)
                ABORT(fs, FR_INT_ERR);
            sect += csect;
            cc = btr / SS(fs); /* 当剩余字节 >= 扇区大小时， */
            if (cc > 0)
            {                  /* 直接读取最大连续扇区 */
                if (csect + cc > fs->csize)
                {              /* 在簇边界处截断 */
                    cc = fs->csize - csect;
                }
                if (disk_read(fs->pdrv, rbuff, sect, cc) != RES_OK)
                    ABORT(fs, FR_DISK_ERR);
#if !FF_FS_READONLY && FF_FS_MINIMIZE <= 2 /* 如果包含脏扇区，用缓存数据替换其中一个读取扇区 */
    #if FF_FS_TINY
                if (fs->wflag && fs->winsect - sect < cc)
                {
                    memcpy(rbuff + ((fs->winsect - sect) * SS(fs)), fs->win, SS(fs));
                }
    #else
                if ((fp->flag & FA_DIRTY) && fp->sect - sect < cc)
                {
                    memcpy(rbuff + ((fp->sect - sect) * SS(fs)), fp->buf, SS(fs));
                }
    #endif
#endif
                rcnt = SS(fs) * cc; /* 传输的字节数 */
                continue;
            }
#if !FF_FS_TINY
            if (fp->sect != sect)
            { /* 如果数据扇区不在缓存中，加载 */
    #if !FF_FS_READONLY
                if (fp->flag & FA_DIRTY)
                { /* 回写脏扇区缓存 */
                    if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
                        ABORT(fs, FR_DISK_ERR);
                    fp->flag &= (BYTE)~FA_DIRTY;
                }
    #endif
                if (disk_read(fs->pdrv, fp->buf, sect, 1) != RES_OK)
                    ABORT(fs, FR_DISK_ERR); /* 填充扇区缓存 */
            }
#endif
            fp->sect = sect;
        }
        rcnt = SS(fs) - (UINT)fp->fptr % SS(fs); /* 扇区中剩余字节数 */
        if (rcnt > btr)
            rcnt = btr;                          /* 如果需要，用 btr 截断 */
#if FF_FS_TINY
        if (move_window(fs, fp->sect) != FR_OK)
            ABORT(fs, FR_DISK_ERR);                       /* 移动扇区窗口 */
        memcpy(rbuff, fs->win + fp->fptr % SS(fs), rcnt); /* 提取部分扇区 */
#else
        memcpy(rbuff, fp->buf + fp->fptr % SS(fs), rcnt); /* 提取部分扇区 */
#endif
    }

    LEAVE_FF(fs, FR_OK);
}

#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 写入文件                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_write(FIL *fp, const void *buff, UINT btw, UINT *bw)
{
    FRESULT res;
    FATFS *fs;
    DWORD clst;
    LBA_t sect;
    UINT wcnt, cc, csect;
    const BYTE *wbuff = (const BYTE *)buff;

    *bw = 0;                       /* 清除写入字节计数器 */
    res = validate(&fp->obj, &fs); /* 检查文件对象的有效性 */
    if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK)
        LEAVE_FF(fs, res);         /* 检查有效性 */
    if (!(fp->flag & FA_WRITE))
        LEAVE_FF(fs, FR_DENIED);   /* 检查访问模式 */

    /* 检查 fptr 回绕（在 FAT 卷上文件大小不能达到 4 GiB） */
    if ((!FF_FS_EXFAT || fs->fs_type != FS_EXFAT) && (DWORD)(fp->fptr + btw) < (DWORD)fp->fptr)
    {
        btw = (UINT)(0xFFFFFFFF - (DWORD)fp->fptr);
    }

    for (; btw > 0; btw -= wcnt, *bw += wcnt, wbuff += wcnt, fp->fptr += wcnt, fp->obj.objsize = (fp->fptr > fp->obj.objsize) ? fp->fptr : fp->obj.objsize)
    {                                                            /* 重复直到所有数据写入 */
        if (fp->fptr % SS(fs) == 0)
        {                                                        /* 在扇区边界？ */
            csect = (UINT)(fp->fptr / SS(fs)) & (fs->csize - 1); /* 簇内扇区偏移 */
            if (csect == 0)
            {                                                    /* 在簇边界？ */
                if (fp->fptr == 0)
                {                                                /* 在文件顶部？ */
                    clst = fp->obj.sclust;                       /* 从起点开始 */
                    if (clst == 0)
                    {                                            /* 如果没有分配簇， */
                        clst = create_chain(&fp->obj, 0);        /* 创建新簇链 */
                    }
                }
                else
                { /* 在文件中间或末尾 */
    #if FF_USE_FASTSEEK
                    if (fp->cltbl)
                    {
                        clst = clmt_clust(fp, fp->fptr); /* 从 CLMT 获取簇# */
                    }
                    else
    #endif
                    {
                        clst = create_chain(&fp->obj, fp->clust); /* 沿着或扩展 FAT 上的簇链 */
                    }
                }
                if (clst == 0)
                    break; /* 无法分配新簇（磁盘满） */
                if (clst == 1)
                    ABORT(fs, FR_INT_ERR);
                if (clst == 0xFFFFFFFF)
                    ABORT(fs, FR_DISK_ERR);
                fp->clust = clst;          /* 更新当前簇 */
                if (fp->obj.sclust == 0)
                    fp->obj.sclust = clst; /* 如果是第一次写入，设置起始簇 */
            }
    #if FF_FS_TINY
            if (fs->winsect == fp->sect && sync_window(fs) != FR_OK)
                ABORT(fs, FR_DISK_ERR); /* 回写扇区缓存 */
    #else
            if (fp->flag & FA_DIRTY)
            { /* 回写扇区缓存 */
                if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
                    ABORT(fs, FR_DISK_ERR);
                fp->flag &= (BYTE)~FA_DIRTY;
            }
    #endif
            sect = clst2sect(fs, fp->clust); /* 获取当前扇区 */
            if (sect == 0)
                ABORT(fs, FR_INT_ERR);
            sect += csect;
            cc = btw / SS(fs); /* 当剩余字节 >= 扇区大小时， */
            if (cc > 0)
            {                  /* 直接写入最大连续扇区 */
                if (csect + cc > fs->csize)
                {              /* 在簇边界处截断 */
                    cc = fs->csize - csect;
                }
                if (disk_write(fs->pdrv, wbuff, sect, cc) != RES_OK)
                    ABORT(fs, FR_DISK_ERR);
    #if FF_FS_MINIMIZE <= 2
        #if FF_FS_TINY
                if (fs->winsect - sect < cc)
                { /* 如果直接写入使扇区缓存无效，重新填充 */
                    memcpy(fs->win, wbuff + ((fs->winsect - sect) * SS(fs)), SS(fs));
                    fs->wflag = 0;
                }
        #else
                if (fp->sect - sect < cc)
                { /* 如果直接写入使扇区缓存无效，重新填充 */
                    memcpy(fp->buf, wbuff + ((fp->sect - sect) * SS(fs)), SS(fs));
                    fp->flag &= (BYTE)~FA_DIRTY;
                }
        #endif
    #endif
                wcnt = SS(fs) * cc; /* 传输的字节数 */
                continue;
            }
    #if FF_FS_TINY
            if (fp->fptr >= fp->obj.objsize)
            { /* 在增长边缘避免无意义的缓存填充 */
                if (sync_window(fs) != FR_OK)
                    ABORT(fs, FR_DISK_ERR);
                fs->winsect = sect;
            }
    #else
            if (fp->sect != sect && /* 用文件数据填充扇区缓存 */
                fp->fptr < fp->obj.objsize &&
                disk_read(fs->pdrv, fp->buf, sect, 1) != RES_OK)
            {
                ABORT(fs, FR_DISK_ERR);
            }
    #endif
            fp->sect = sect;
        }
        wcnt = SS(fs) - (UINT)fp->fptr % SS(fs); /* 扇区中剩余字节数 */
        if (wcnt > btw)
            wcnt = btw;                          /* 如果需要，用 btw 截断 */
    #if FF_FS_TINY
        if (move_window(fs, fp->sect) != FR_OK)
            ABORT(fs, FR_DISK_ERR);                       /* 移动扇区窗口 */
        memcpy(fs->win + fp->fptr % SS(fs), wbuff, wcnt); /* 将数据放入扇区 */
        fs->wflag = 1;
    #else
        memcpy(fp->buf + fp->fptr % SS(fs), wbuff, wcnt); /* 将数据放入扇区 */
        fp->flag |= FA_DIRTY;
    #endif
    }

    fp->flag |= FA_MODIFIED; /* 设置文件更改标志 */

    LEAVE_FF(fs, FR_OK);
}

/*-----------------------------------------------------------------------*/
/* 同步文件                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_sync(FIL *fp)
{
    FRESULT res;
    FATFS *fs;
    DWORD tm;
    BYTE *dir;

    res = validate(&fp->obj, &fs); /* 检查文件对象的有效性 */
    if (res == FR_OK)
    {
        if (fp->flag & FA_MODIFIED)
        { /* 文件是否有任何更改？ */
    #if !FF_FS_TINY
            if (fp->flag & FA_DIRTY)
            { /* 如果需要，回写缓存数据 */
                if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
                    LEAVE_FF(fs, FR_DISK_ERR);
                fp->flag &= (BYTE)~FA_DIRTY;
            }
    #endif
            /* 更新目录条目 */
            tm = GET_FATTIME(); /* 修改时间 */
    #if FF_FS_EXFAT
            if (fs->fs_type == FS_EXFAT)
            {
                res = fill_first_frag(&fp->obj); /* 如果需要，在 FAT 上填充第一个片段 */
                if (res == FR_OK)
                {
                    res = fill_last_frag(&fp->obj, fp->clust, 0xFFFFFFFF); /* 如果需要，在 FAT 上填充最后一个片段 */
                }
                if (res == FR_OK)
                {
                    DIR dj;
                    DEF_NAMBUF

                    INIT_NAMBUF(fs);
                    res = load_obj_xdir(&dj, &fp->obj); /* 加载目录条目块 */
                    if (res == FR_OK)
                    {
                        fs->dirbuf[XDIR_Attr] |= AM_ARC;                            /* 设置存档属性，表示文件已被修改 */
                        fs->dirbuf[XDIR_GenFlags] = fp->obj.stat | 1;               /* 更新文件分配信息 */
                        st_dword(fs->dirbuf + XDIR_FstClus, fp->obj.sclust);        /* 更新起始簇 */
                        st_qword(fs->dirbuf + XDIR_FileSize, fp->obj.objsize);      /* 更新文件大小 */
                        st_qword(fs->dirbuf + XDIR_ValidFileSize, fp->obj.objsize); /* (FatFs 不支持有效文件大小功能) */
                        st_dword(fs->dirbuf + XDIR_ModTime, tm);                    /* 更新修改时间 */
                        fs->dirbuf[XDIR_ModTime10] = 0;
                        st_dword(fs->dirbuf + XDIR_AccTime, 0);
                        res = store_xdir(&dj); /* 将其恢复回目录 */
                        if (res == FR_OK)
                        {
                            res = sync_fs(fs);
                            fp->flag &= (BYTE)~FA_MODIFIED;
                        }
                    }
                    FREE_NAMBUF();
                }
            }
            else
    #endif
            {
                res = move_window(fs, fp->dir_sect);
                if (res == FR_OK)
                {
                    dir = fp->dir_ptr;
                    dir[DIR_Attr] |= AM_ARC;                              /* 设置存档属性，表示文件已被修改 */
                    st_clust(fp->obj.fs, dir, fp->obj.sclust);            /* 更新文件分配信息 */
                    st_dword(dir + DIR_FileSize, (DWORD)fp->obj.objsize); /* 更新文件大小 */
                    st_dword(dir + DIR_ModTime, tm);                      /* 更新修改时间 */
                    st_word(dir + DIR_LstAccDate, 0);
                    fs->wflag = 1;
                    res = sync_fs(fs); /* 将其恢复回目录 */
                    fp->flag &= (BYTE)~FA_MODIFIED;
                }
            }
        }
    }

    LEAVE_FF(fs, res);
}
#endif /* !FF_FS_READONLY */

/*-----------------------------------------------------------------------*/
/* 关闭文件                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_close(FIL *fp)
{
    FRESULT res;
    FATFS *fs;

#if !FF_FS_READONLY
    res = f_sync(fp); /* 刷新缓存数据 */
    if (res == FR_OK)
#endif
    {
        res = validate(&fp->obj, &fs); /* 锁定卷 */
        if (res == FR_OK)
        {
#if FF_FS_LOCK != 0
            res = dec_lock(fp->obj.lockid); /* 减少文件打开计数器 */
            if (res == FR_OK)
                fp->obj.fs = 0;             /* 使文件对象无效 */
#else
            fp->obj.fs = 0; /* 使文件对象无效 */
#endif
#if FF_FS_REENTRANT
            unlock_fs(fs, FR_OK); /* 解锁卷 */
#endif
        }
    }
    return res;
}

#if FF_FS_RPATH >= 1
/*-----------------------------------------------------------------------*/
/* 更改当前目录或当前驱动器，获取当前目录                                  */
/*-----------------------------------------------------------------------*/
FRESULT f_chdrive(const TCHAR *path)
{
    int vol;

    /* 获取逻辑驱动器号 */
    vol = get_ldnumber(&path);
    if (vol < 0)
        return FR_INVALID_DRIVE;
    CurrVol = (BYTE)vol; /* 将其设置为当前卷 */

    return FR_OK;
}

FRESULT f_chdir(const TCHAR *path)
{
    #if FF_STR_VOLUME_ID == 2
    UINT i;
    #endif
    FRESULT res;
    DIR dj;
    FATFS *fs;
    DEF_NAMBUF

    /* 获取逻辑驱动器 */
    res = mount_volume(&path, &fs, 0);
    if (res == FR_OK)
    {
        dj.obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(&dj, path); /* 跟随路径 */
        if (res == FR_OK)
        {                             /* 跟随完成 */
            if (dj.fn[NSFLAG] & NS_NONAME)
            {                         /* 是否为起始目录本身？ */
                fs->cdir = dj.obj.sclust;
    #if FF_FS_EXFAT
                if (fs->fs_type == FS_EXFAT)
                {
                    fs->cdc_scl = dj.obj.c_scl;
                    fs->cdc_size = dj.obj.c_size;
                    fs->cdc_ofs = dj.obj.c_ofs;
                }
    #endif
            }
            else
            {
                if (dj.obj.attr & AM_DIR)
                { /* 是否为子目录 */
    #if FF_FS_EXFAT
                    if (fs->fs_type == FS_EXFAT)
                    {
                        fs->cdir = ld_dword(fs->dirbuf + XDIR_FstClus); /* 子目录簇 */
                        fs->cdc_scl = dj.obj.sclust;                    /* 保存包含目录信息 */
                        fs->cdc_size = ((DWORD)dj.obj.objsize & 0xFFFFFF00) | dj.obj.stat;
                        fs->cdc_ofs = dj.blk_ofs;
                    }
                    else
    #endif
                    {
                        fs->cdir = ld_clust(fs, dj.dir); /* 子目录簇 */
                    }
                }
                else
                {
                    res = FR_NO_PATH; /* 到达但为文件 */
                }
            }
        }
        FREE_NAMBUF();
        if (res == FR_NO_FILE)
            res = FR_NO_PATH;
    #if FF_STR_VOLUME_ID == 2 /* 如果在 Unix 风格卷 ID 中，当前驱动器也会更改 */
        if (res == FR_OK)
        {
            for (i = FF_VOLUMES - 1; i && fs != FatFs[i]; i--); /* 设置当前驱动器 */
            CurrVol = (BYTE)i;
        }
    #endif
    }

    LEAVE_FF(fs, res);
}

    #if FF_FS_RPATH >= 2
FRESULT f_getcwd(TCHAR *buff, UINT len)
{
    FRESULT res;
    DIR dj;
    FATFS *fs;
    UINT i, n;
    DWORD ccl;
    TCHAR *tp = buff;
        #if FF_VOLUMES >= 2
    UINT vl;
            #if FF_STR_VOLUME_ID
    const char *vp;
            #endif
        #endif
    FILINFO fno;
    DEF_NAMBUF

    /* 获取逻辑驱动器 */
    buff[0] = 0;                                       /* 设置空字符串以获取当前卷 */
    res = mount_volume((const TCHAR **)&buff, &fs, 0); /* 获取当前卷 */
    if (res == FR_OK)
    {
        dj.obj.fs = fs;
        INIT_NAMBUF(fs);

        /* 跟随父目录并创建路径 */
        i = len;                                /* 缓冲区底部（目录栈底部） */
        if (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT)
        {                                       /* (在 exFAT 上无法执行 getcwd，返回根路径) */
            dj.obj.sclust = fs->cdir;           /* 从当前目录开始向上跟随父目录 */
            while ((ccl = dj.obj.sclust) != 0)
            {                                   /* 重复直到当前目录为根目录 */
                res = dir_sdi(&dj, 1 * SZDIRE); /* 获取父目录 */
                if (res != FR_OK)
                    break;
                res = move_window(fs, dj.sect);
                if (res != FR_OK)
                    break;
                dj.obj.sclust = ld_clust(fs, dj.dir); /* 转到父目录 */
                res = dir_sdi(&dj, 0);
                if (res != FR_OK)
                    break;
                do
                { /* 查找链接到子目录的条目 */
                    res = DIR_READ_FILE(&dj);
                    if (res != FR_OK)
                        break;
                    if (ccl == ld_clust(fs, dj.dir))
                        break; /* 找到条目 */
                    res = dir_next(&dj, 0);
                } while (res == FR_OK);
                if (res == FR_NO_FILE)
                    res = FR_INT_ERR; /* 不可能“未找到” */
                if (res != FR_OK)
                    break;
                get_fileinfo(&dj, &fno);        /* 获取目录名并压入缓冲区 */
                for (n = 0; fno.fname[n]; n++); /* 名字长度 */
                if (i < n + 1)
                {                               /* 存储路径名的空间不足？ */
                    res = FR_NOT_ENOUGH_CORE;
                    break;
                }
                while (n) buff[--i] = fno.fname[--n]; /* 压入名字 */
                buff[--i] = '/';
            }
        }
        if (res == FR_OK)
        {
            if (i == len)
                buff[--i] = '/';      /* 是否为根目录？ */
        #if FF_VOLUMES >= 2           /* 放入驱动器前缀 */
            vl = 0;
            #if FF_STR_VOLUME_ID >= 1 /* 字符串卷 ID */
            for (n = 0, vp = (const char *)VolumeStr[CurrVol]; vp[n]; n++);
            if (i >= n + 2)
            {
                if (FF_STR_VOLUME_ID == 2)
                    *tp++ = (TCHAR)'/';
                for (vl = 0; vl < n; *tp++ = (TCHAR)vp[vl], vl++);
                if (FF_STR_VOLUME_ID == 1)
                    *tp++ = (TCHAR)':';
                vl++;
            }
            #else /* 数字卷 ID */
            if (i >= 3)
            {
                *tp++ = (TCHAR)'0' + CurrVol;
                *tp++ = (TCHAR)':';
                vl = 2;
            }
            #endif
            if (vl == 0)
                res = FR_NOT_ENOUGH_CORE;
        #endif
            /* 添加当前目录路径 */
            if (res == FR_OK)
            {
                do *tp++ = buff[i++];
                while (i < len); /* 复制栈中的路径字符串 */
            }
        }
        FREE_NAMBUF();
    }

    *tp = 0;
    LEAVE_FF(fs, res);
}
    #endif /* FF_FS_RPATH >= 2 */
#endif     /* FF_FS_RPATH >= 1 */

#if FF_FS_MINIMIZE <= 2
/*-----------------------------------------------------------------------*/
/* 移动文件读/写指针                                                      */
/*-----------------------------------------------------------------------*/
FRESULT f_lseek(FIL *fp, FSIZE_t ofs)
{
    FRESULT res;
    FATFS *fs;
    DWORD clst, bcs;
    LBA_t nsect;
    FSIZE_t ifptr;
    #if FF_USE_FASTSEEK
    DWORD cl, pcl, ncl, tcl, tlen, ulen;
    DWORD *tbl;
    LBA_t dsc;
    #endif

    res = validate(&fp->obj, &fs); /* 检查文件对象的有效性 */
    if (res == FR_OK)
        res = (FRESULT)fp->err;
    #if FF_FS_EXFAT && !FF_FS_READONLY
    if (res == FR_OK && fs->fs_type == FS_EXFAT)
    {
        res = fill_last_frag(&fp->obj, fp->clust, 0xFFFFFFFF); /* 如果需要，在 FAT 上填充最后一个片段 */
    }
    #endif
    if (res != FR_OK)
        LEAVE_FF(fs, res);

    #if FF_USE_FASTSEEK
    if (fp->cltbl)
    {     /* 快速查找 */
        if (ofs == CREATE_LINKMAP)
        { /* 创建 CLMT */
            tbl = fp->cltbl;
            tlen = *tbl++;
            ulen = 2;            /* 给定表大小和所需表大小 */
            cl = fp->obj.sclust; /* 链的起点 */
            if (cl != 0)
            {
                do
                {
                    /* 获取一个片段 */
                    tcl = cl;
                    ncl = 0;
                    ulen += 2; /* 顶部、长度和已用项 */
                    do
                    {
                        pcl = cl;
                        ncl++;
                        cl = get_fat(&fp->obj, cl);
                        if (cl <= 1)
                            ABORT(fs, FR_INT_ERR);
                        if (cl == 0xFFFFFFFF)
                            ABORT(fs, FR_DISK_ERR);
                    } while (cl == pcl + 1);
                    if (ulen <= tlen)
                    { /* 存储片段的长度和顶部 */
                        *tbl++ = ncl;
                        *tbl++ = tcl;
                    }
                } while (cl < fs->n_fatent); /* 重复直到链结束 */
            }
            *fp->cltbl = ulen; /* 使用的项数 */
            if (ulen <= tlen)
            {
                *tbl = 0; /* 表终止 */
            }
            else
            {
                res = FR_NOT_ENOUGH_CORE; /* 给定表大小小于所需大小 */
            }
        }
        else
        {                              /* 快速查找 */
            if (ofs > fp->obj.objsize)
                ofs = fp->obj.objsize; /* 在文件大小处截断偏移 */
            fp->fptr = ofs;            /* 设置文件指针 */
            if (ofs > 0)
            {
                fp->clust = clmt_clust(fp, ofs - 1);
                dsc = clst2sect(fs, fp->clust);
                if (dsc == 0)
                    ABORT(fs, FR_INT_ERR);
                dsc += (DWORD)((ofs - 1) / SS(fs)) & (fs->csize - 1);
                if (fp->fptr % SS(fs) && dsc != fp->sect)
                { /* 如果需要，重新填充扇区缓存 */
        #if !FF_FS_TINY
            #if !FF_FS_READONLY
                    if (fp->flag & FA_DIRTY)
                    { /* 回写脏扇区缓存 */
                        if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
                            ABORT(fs, FR_DISK_ERR);
                        fp->flag &= (BYTE)~FA_DIRTY;
                    }
            #endif
                    if (disk_read(fs->pdrv, fp->buf, dsc, 1) != RES_OK)
                        ABORT(fs, FR_DISK_ERR); /* 加载当前扇区 */
        #endif
                    fp->sect = dsc;
                }
            }
        }
    }
    else
    #endif

    /* 普通查找 */
    {
    #if FF_FS_EXFAT
        if (fs->fs_type != FS_EXFAT && ofs >= 0x100000000)
            ofs = 0xFFFFFFFF; /* 如果在 FATxx，在 4 GiB - 1 处截断 */
    #endif
        if (ofs > fp->obj.objsize && (FF_FS_READONLY || !(fp->flag & FA_WRITE)))
        { /* 在只读模式下，按文件大小截断偏移 */
            ofs = fp->obj.objsize;
        }
        ifptr = fp->fptr;
        fp->fptr = nsect = 0;
        if (ofs > 0)
        {
            bcs = (DWORD)fs->csize * SS(fs); /* 簇大小（字节） */
            if (ifptr > 0 &&
                (ofs - 1) / bcs >= (ifptr - 1) / bcs)
            {                                                 /* 当查找同一或后续簇时， */
                fp->fptr = (ifptr - 1) & ~(FSIZE_t)(bcs - 1); /* 从当前簇开始 */
                ofs -= fp->fptr;
                clst = fp->clust;
            }
            else
            {                          /* 当向后查找时， */
                clst = fp->obj.sclust; /* 从第一个簇开始 */
    #if !FF_FS_READONLY
                if (clst == 0)
                { /* 如果没有簇链，创建新链 */
                    clst = create_chain(&fp->obj, 0);
                    if (clst == 1)
                        ABORT(fs, FR_INT_ERR);
                    if (clst == 0xFFFFFFFF)
                        ABORT(fs, FR_DISK_ERR);
                    fp->obj.sclust = clst;
                }
    #endif
                fp->clust = clst;
            }
            if (clst != 0)
            {
                while (ofs > bcs)
                { /* 簇跟随循环 */
                    ofs -= bcs;
                    fp->fptr += bcs;
    #if !FF_FS_READONLY
                    if (fp->flag & FA_WRITE)
                    {     /* 检查是否在写模式 */
                        if (FF_FS_EXFAT && fp->fptr > fp->obj.objsize)
                        { /* 没有 FAT 链的对象需要正确的 objsize 来生成 FAT 值 */
                            fp->obj.objsize = fp->fptr;
                            fp->flag |= FA_MODIFIED;
                        }
                        clst = create_chain(&fp->obj, clst); /* 强制拉伸沿着链 */
                        if (clst == 0)
                        {                                    /* 在磁盘满的情况下截断文件大小 */
                            ofs = 0;
                            break;
                        }
                    }
                    else
    #endif
                    {
                        clst = get_fat(&fp->obj, clst); /* 如果不在写模式，沿着簇链 */
                    }
                    if (clst == 0xFFFFFFFF)
                        ABORT(fs, FR_DISK_ERR);
                    if (clst <= 1 || clst >= fs->n_fatent)
                        ABORT(fs, FR_INT_ERR);
                    fp->clust = clst;
                }
                fp->fptr += ofs;
                if (ofs % SS(fs))
                {
                    nsect = clst2sect(fs, clst); /* 当前扇区 */
                    if (nsect == 0)
                        ABORT(fs, FR_INT_ERR);
                    nsect += (DWORD)(ofs / SS(fs));
                }
            }
        }
        if (!FF_FS_READONLY && fp->fptr > fp->obj.objsize)
        { /* 如果文件大小扩展，设置文件更改标志 */
            fp->obj.objsize = fp->fptr;
            fp->flag |= FA_MODIFIED;
        }
        if (fp->fptr % SS(fs) && nsect != fp->sect)
        { /* 如果需要，填充扇区缓存 */
    #if !FF_FS_TINY
        #if !FF_FS_READONLY
            if (fp->flag & FA_DIRTY)
            { /* 回写脏扇区缓存 */
                if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
                    ABORT(fs, FR_DISK_ERR);
                fp->flag &= (BYTE)~FA_DIRTY;
            }
        #endif
            if (disk_read(fs->pdrv, fp->buf, nsect, 1) != RES_OK)
                ABORT(fs, FR_DISK_ERR); /* 填充扇区缓存 */
    #endif
            fp->sect = nsect;
        }
    }

    LEAVE_FF(fs, res);
}

    #if FF_FS_MINIMIZE <= 1
/*-----------------------------------------------------------------------*/
/* 创建目录对象                                                           */
/*-----------------------------------------------------------------------*/
FRESULT f_opendir(DIR *dp, const TCHAR *path)
{
    FRESULT res;
    FATFS *fs;
    DEF_NAMBUF

    if (!dp)
        return FR_INVALID_OBJECT;

    /* 获取逻辑驱动器 */
    res = mount_volume(&path, &fs, 0);
    if (res == FR_OK)
    {
        dp->obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(dp, path); /* 跟随路径到目录 */
        if (res == FR_OK)
        {                            /* 跟随完成 */
            if (!(dp->fn[NSFLAG] & NS_NONAME))
            {                        /* 不是起始目录本身 */
                if (dp->obj.attr & AM_DIR)
                {                    /* 此对象是子目录 */
        #if FF_FS_EXFAT
                    if (fs->fs_type == FS_EXFAT)
                    {
                        dp->obj.c_scl = dp->obj.sclust; /* 获取包含目录信息 */
                        dp->obj.c_size = ((DWORD)dp->obj.objsize & 0xFFFFFF00) | dp->obj.stat;
                        dp->obj.c_ofs = dp->blk_ofs;
                        init_alloc_info(fs, &dp->obj); /* 获取对象分配信息 */
                    }
                    else
        #endif
                    {
                        dp->obj.sclust = ld_clust(fs, dp->dir); /* 获取对象分配信息 */
                    }
                }
                else
                { /* 此对象是文件 */
                    res = FR_NO_PATH;
                }
            }
            if (res == FR_OK)
            {
                dp->obj.id = fs->id;
                res = dir_sdi(dp, 0); /* 回绕目录 */
        #if FF_FS_LOCK != 0
                if (res == FR_OK)
                {
                    if (dp->obj.sclust != 0)
                    {
                        dp->obj.lockid = inc_lock(dp, 0); /* 锁定子目录 */
                        if (!dp->obj.lockid)
                            res = FR_TOO_MANY_OPEN_FILES;
                    }
                    else
                    {
                        dp->obj.lockid = 0; /* 根目录不需要锁定 */
                    }
                }
        #endif
            }
        }
        FREE_NAMBUF();
        if (res == FR_NO_FILE)
            res = FR_NO_PATH;
    }
    if (res != FR_OK)
        dp->obj.fs = 0; /* 如果函数失败，使目录对象无效 */

    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 关闭目录                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_closedir(DIR *dp)
{
    FRESULT res;
    FATFS *fs;

    res = validate(&dp->obj, &fs); /* 检查文件对象的有效性 */
    if (res == FR_OK)
    {
        #if FF_FS_LOCK != 0
        if (dp->obj.lockid)
            res = dec_lock(dp->obj.lockid); /* 减少子目录打开计数器 */
        if (res == FR_OK)
            dp->obj.fs = 0;                 /* 使目录对象无效 */
        #else
        dp->obj.fs = 0; /* 使目录对象无效 */
        #endif
        #if FF_FS_REENTRANT
        unlock_fs(fs, FR_OK); /* 解锁卷 */
        #endif
    }
    return res;
}

/*-----------------------------------------------------------------------*/
/* 按顺序读取目录条目                                                     */
/*-----------------------------------------------------------------------*/
FRESULT f_readdir(DIR *dp, FILINFO *fno)
{
    FRESULT res;
    FATFS *fs;
    DEF_NAMBUF

    res = validate(&dp->obj, &fs); /* 检查目录对象的有效性 */
    if (res == FR_OK)
    {
        if (!fno)
        {
            res = dir_sdi(dp, 0); /* 回绕目录对象 */
        }
        else
        {
            INIT_NAMBUF(fs);
            res = DIR_READ_FILE(dp);   /* 读取一个项 */
            if (res == FR_NO_FILE)
                res = FR_OK;           /* 忽略目录结束 */
            if (res == FR_OK)
            {                          /* 找到有效条目 */
                get_fileinfo(dp, fno); /* 获取对象信息 */
                res = dir_next(dp, 0); /* 递增索引以供下次使用 */
                if (res == FR_NO_FILE)
                    res = FR_OK;       /* 现在忽略目录结束 */
            }
            FREE_NAMBUF();
        }
    }
    LEAVE_FF(fs, res);
}

        #if FF_USE_FIND
/*-----------------------------------------------------------------------*/
/* 查找下一个文件                                                         */
/*-----------------------------------------------------------------------*/
FRESULT f_findnext(DIR *dp, FILINFO *fno)
{
    FRESULT res;

    for (;;)
    {
        res = f_readdir(dp, fno); /* 获取一个目录项 */
        if (res != FR_OK || !fno || !fno->fname[0])
            break;                /* 如果出现任何错误或目录结束，终止 */
        if (pattern_match(dp->pat, fno->fname, 0, FIND_RECURS))
            break;                /* 测试文件名 */
            #if FF_USE_LFN && FF_USE_FIND == 2
        if (pattern_match(dp->pat, fno->altname, 0, FIND_RECURS))
            break; /* 如果存在，测试备选名 */
            #endif
    }
    return res;
}

/*-----------------------------------------------------------------------*/
/* 查找第一个文件                                                         */
/*-----------------------------------------------------------------------*/
FRESULT f_findfirst(DIR *dp, FILINFO *fno, const TCHAR *path, const TCHAR *pattern)
{
    FRESULT res;

    dp->pat = pattern;         /* 保存指向模式字符串的指针 */
    res = f_opendir(dp, path); /* 打开目标目录 */
    if (res == FR_OK)
    {
        res = f_findnext(dp, fno); /* 查找第一个项 */
    }
    return res;
}
        #endif /* FF_USE_FIND */

        #if FF_FS_MINIMIZE == 0
/*-----------------------------------------------------------------------*/
/* 获取文件状态                                                          */
/*-----------------------------------------------------------------------*/
FRESULT f_stat(const TCHAR *path, FILINFO *fno)
{
    FRESULT res;
    DIR dj;
    DEF_NAMBUF

    /* 获取逻辑驱动器 */
    res = mount_volume(&path, &dj.obj.fs, 0);
    if (res == FR_OK)
    {
        INIT_NAMBUF(dj.obj.fs);
        res = follow_path(&dj, path); /* 跟随文件路径 */
        if (res == FR_OK)
        {                             /* 跟随完成 */
            if (dj.fn[NSFLAG] & NS_NONAME)
            {                         /* 是起始目录 */
                res = FR_INVALID_NAME;
            }
            else
            { /* 找到对象 */
                if (fno)
                    get_fileinfo(&dj, fno);
            }
        }
        FREE_NAMBUF();
    }

    LEAVE_FF(dj.obj.fs, res);
}

            #if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 获取空闲簇数                                                           */
/*-----------------------------------------------------------------------*/
FRESULT f_getfree(const TCHAR *path, DWORD *nclst, FATFS **fatfs)
{
    FRESULT res;
    FATFS *fs;
    DWORD nfree, clst, stat;
    LBA_t sect;
    UINT i;
    FFOBJID obj;

    /* 获取逻辑驱动器 */
    res = mount_volume(&path, &fs, 0);
    if (res == FR_OK)
    {
        *fatfs = fs; /* 返回指向 fs 对象的指针 */
        /* 如果 free_clst 有效，直接返回，无需完全 FAT 扫描 */
        if (fs->free_clst <= fs->n_fatent - 2)
        {
            *nclst = fs->free_clst;
        }
        else
        {
            /* 扫描 FAT 以获得空闲簇数 */
            nfree = 0;
            if (fs->fs_type == FS_FAT12)
            { /* FAT12：扫描位字段 FAT 条目 */
                clst = 2;
                obj.fs = fs;
                do
                {
                    stat = get_fat(&obj, clst);
                    if (stat == 0xFFFFFFFF)
                    {
                        res = FR_DISK_ERR;
                        break;
                    }
                    if (stat == 1)
                    {
                        res = FR_INT_ERR;
                        break;
                    }
                    if (stat == 0)
                        nfree++;
                } while (++clst < fs->n_fatent);
            }
            else
            {
                #if FF_FS_EXFAT
                if (fs->fs_type == FS_EXFAT)
                { /* exFAT：扫描分配位图 */
                    BYTE bm;
                    UINT b;

                    clst = fs->n_fatent - 2; /* 簇数 */
                    sect = fs->bitbase;      /* 位图扇区 */
                    i = 0;                   /* 扇区内偏移 */
                    do
                    {                        /* 计数位图中为零的位数 */
                        if (i == 0)
                        {
                            res = move_window(fs, sect++);
                            if (res != FR_OK)
                                break;
                        }
                        for (b = 8, bm = fs->win[i]; b && clst; b--, clst--)
                        {
                            if (!(bm & 1))
                                nfree++;
                            bm >>= 1;
                        }
                        i = (i + 1) % SS(fs);
                    } while (clst);
                }
                else
                #endif
                {                        /* FAT16/32：扫描 WORD/DWORD FAT 条目 */
                    clst = fs->n_fatent; /* 条目数 */
                    sect = fs->fatbase;  /* FAT 顶部 */
                    i = 0;               /* 扇区内偏移 */
                    do
                    {                    /* 计数 FAT 中为零的条目数 */
                        if (i == 0)
                        {
                            res = move_window(fs, sect++);
                            if (res != FR_OK)
                                break;
                        }
                        if (fs->fs_type == FS_FAT16)
                        {
                            if (ld_word(fs->win + i) == 0)
                                nfree++;
                            i += 2;
                        }
                        else
                        {
                            if ((ld_dword(fs->win + i) & 0x0FFFFFFF) == 0)
                                nfree++;
                            i += 4;
                        }
                        i %= SS(fs);
                    } while (--clst);
                }
            }
            if (res == FR_OK)
            {                          /* 如果成功，更新参数 */
                *nclst = nfree;        /* 返回空闲簇数 */
                fs->free_clst = nfree; /* 现在 free_clst 有效 */
                fs->fsi_flag |= 1;     /* FAT32：需要更新 FSInfo */
            }
        }
    }

    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 截断文件                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_truncate(FIL *fp)
{
    FRESULT res;
    FATFS *fs;
    DWORD ncl;

    res = validate(&fp->obj, &fs); /* 检查文件对象的有效性 */
    if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK)
        LEAVE_FF(fs, res);
    if (!(fp->flag & FA_WRITE))
        LEAVE_FF(fs, FR_DENIED); /* 检查访问模式 */

    if (fp->fptr < fp->obj.objsize)
    {     /* 当 fptr 不在文件末尾时进行处理 */
        if (fp->fptr == 0)
        { /* 当将文件大小设置为零时，移除整个簇链 */
            res = remove_chain(&fp->obj, fp->obj.sclust, 0);
            fp->obj.sclust = 0;
        }
        else
        { /* 当截断文件的一部分时，移除剩余簇 */
            ncl = get_fat(&fp->obj, fp->clust);
            res = FR_OK;
            if (ncl == 0xFFFFFFFF)
                res = FR_DISK_ERR;
            if (ncl == 1)
                res = FR_INT_ERR;
            if (res == FR_OK && ncl < fs->n_fatent)
            {
                res = remove_chain(&fp->obj, ncl, fp->clust);
            }
        }
        fp->obj.objsize = fp->fptr; /* 将文件大小设置为当前读/写点 */
        fp->flag |= FA_MODIFIED;
                #if !FF_FS_TINY
        if (res == FR_OK && (fp->flag & FA_DIRTY))
        {
            if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
            {
                res = FR_DISK_ERR;
            }
            else
            {
                fp->flag &= (BYTE)~FA_DIRTY;
            }
        }
                #endif
        if (res != FR_OK)
            ABORT(fs, res);
    }

    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 删除文件/目录                                                          */
/*-----------------------------------------------------------------------*/
FRESULT f_unlink(const TCHAR *path)
{
    FRESULT res;
    DIR dj, sdj;
    DWORD dclst = 0;
    FATFS *fs;
                #if FF_FS_EXFAT
    FFOBJID obj;
                #endif
    DEF_NAMBUF

    /* 获取逻辑驱动器 */
    res = mount_volume(&path, &fs, FA_WRITE);
    if (res == FR_OK)
    {
        dj.obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(&dj, path); /* 跟随文件路径 */
        if (FF_FS_RPATH && res == FR_OK && (dj.fn[NSFLAG] & NS_DOT))
        {
            res = FR_INVALID_NAME; /* 无法删除点条目 */
        }
                #if FF_FS_LOCK != 0
        if (res == FR_OK)
            res = chk_lock(&dj, 2); /* 检查是否为打开的对象 */
                #endif
        if (res == FR_OK)
        { /* 对象可访问 */
            if (dj.fn[NSFLAG] & NS_NONAME)
            {
                res = FR_INVALID_NAME; /* 无法删除起始目录 */
            }
            else
            {
                if (dj.obj.attr & AM_RDO)
                {
                    res = FR_DENIED; /* 无法删除只读对象 */
                }
            }
            if (res == FR_OK)
            {
                #if FF_FS_EXFAT
                obj.fs = fs;
                if (fs->fs_type == FS_EXFAT)
                {
                    init_alloc_info(fs, &obj);
                    dclst = obj.sclust;
                }
                else
                #endif
                {
                    dclst = ld_clust(fs, dj.dir);
                }
                if (dj.obj.attr & AM_DIR)
                { /* 是否为子目录？ */
                #if FF_FS_RPATH != 0
                    if (dclst == fs->cdir)
                    { /* 是否为当前目录？ */
                        res = FR_DENIED;
                    }
                    else
                #endif
                    {
                        sdj.obj.fs = fs; /* 打开子目录 */
                        sdj.obj.sclust = dclst;
                #if FF_FS_EXFAT
                        if (fs->fs_type == FS_EXFAT)
                        {
                            sdj.obj.objsize = obj.objsize;
                            sdj.obj.stat = obj.stat;
                        }
                #endif
                        res = dir_sdi(&sdj, 0);
                        if (res == FR_OK)
                        {
                            res = DIR_READ_FILE(&sdj); /* 测试目录是否为空 */
                            if (res == FR_OK)
                                res = FR_DENIED;       /* 不为空？ */
                            if (res == FR_NO_FILE)
                                res = FR_OK;           /* 为空？ */
                        }
                    }
                }
            }
            if (res == FR_OK)
            {
                res = dir_remove(&dj); /* 删除目录条目 */
                if (res == FR_OK && dclst != 0)
                {                      /* 如果存在簇链，移除它 */
                #if FF_FS_EXFAT
                    res = remove_chain(&obj, dclst, 0);
                #else
                    res = remove_chain(&dj.obj, dclst, 0);
                #endif
                }
                if (res == FR_OK)
                    res = sync_fs(fs);
            }
        }
        FREE_NAMBUF();
    }

    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 创建目录                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_mkdir(const TCHAR *path)
{
    FRESULT res;
    DIR dj;
    FFOBJID sobj;
    FATFS *fs;
    DWORD dcl, pcl, tm;
    DEF_NAMBUF

    res = mount_volume(&path, &fs, FA_WRITE); /* 获取逻辑驱动器 */
    if (res == FR_OK)
    {
        dj.obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(&dj, path); /* 跟随文件路径 */
        if (res == FR_OK)
            res = FR_EXIST;           /* 名称冲突？ */
        if (FF_FS_RPATH && res == FR_NO_FILE && (dj.fn[NSFLAG] & NS_DOT))
        {                             /* 无效名称？ */
            res = FR_INVALID_NAME;
        }
        if (res == FR_NO_FILE)
        {                                 /* 可以创建新目录 */
            sobj.fs = fs;                 /* 要创建新链的新对象 id */
            dcl = create_chain(&sobj, 0); /* 为新目录分配一个簇 */
            res = FR_OK;
            if (dcl == 0)
                res = FR_DENIED;   /* 没有空间分配新簇？ */
            if (dcl == 1)
                res = FR_INT_ERR;  /* 任何异常？ */
            if (dcl == 0xFFFFFFFF)
                res = FR_DISK_ERR; /* 磁盘错误？ */
            tm = GET_FATTIME();
            if (res == FR_OK)
            {
                res = dir_clear(fs, dcl); /* 清理新表 */
                if (res == FR_OK)
                {
                    if (!FF_FS_EXFAT || fs->fs_type != FS_EXFAT)
                    {                                        /* 创建点条目（仅 FAT） */
                        memset(fs->win + DIR_Name, ' ', 11); /* 创建 "." 条目 */
                        fs->win[DIR_Name] = '.';
                        fs->win[DIR_Attr] = AM_DIR;
                        st_dword(fs->win + DIR_ModTime, tm);
                        st_clust(fs, fs->win, dcl);
                        memcpy(fs->win + SZDIRE, fs->win, SZDIRE); /* 创建 ".." 条目 */
                        fs->win[SZDIRE + 1] = '.';
                        pcl = dj.obj.sclust;
                        st_clust(fs, fs->win + SZDIRE, pcl);
                        fs->wflag = 1;
                    }
                    res = dir_register(&dj); /* 将对象注册到父目录 */
                }
            }
            if (res == FR_OK)
            {
                #if FF_FS_EXFAT
                if (fs->fs_type == FS_EXFAT)
                {                                                                    /* 初始化目录条目块 */
                    st_dword(fs->dirbuf + XDIR_ModTime, tm);                         /* 创建时间 */
                    st_dword(fs->dirbuf + XDIR_FstClus, dcl);                        /* 表起始簇 */
                    st_dword(fs->dirbuf + XDIR_FileSize, (DWORD)fs->csize * SS(fs)); /* 目录大小必须有效 */
                    st_dword(fs->dirbuf + XDIR_ValidFileSize, (DWORD)fs->csize * SS(fs));
                    fs->dirbuf[XDIR_GenFlags] = 3;                                   /* 初始化对象标志 */
                    fs->dirbuf[XDIR_Attr] = AM_DIR;                                  /* 属性 */
                    res = store_xdir(&dj);
                }
                else
                #endif
                {
                    st_dword(dj.dir + DIR_ModTime, tm); /* 创建时间 */
                    st_clust(fs, dj.dir, dcl);          /* 表起始簇 */
                    dj.dir[DIR_Attr] = AM_DIR;          /* 属性 */
                    fs->wflag = 1;
                }
                if (res == FR_OK)
                {
                    res = sync_fs(fs);
                }
            }
            else
            {
                remove_chain(&sobj, dcl, 0); /* 无法注册，移除分配的簇 */
            }
        }
        FREE_NAMBUF();
    }

    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 重命名文件/目录                                                        */
/*-----------------------------------------------------------------------*/
FRESULT f_rename(const TCHAR *path_old, const TCHAR *path_new)
{
    FRESULT res;
    DIR djo, djn;
    FATFS *fs;
    BYTE buf[FF_FS_EXFAT ? SZDIRE * 2 : SZDIRE], *dir;
    LBA_t sect;
    DEF_NAMBUF

    get_ldnumber(&path_new);                      /* 去除新名称的驱动器号 */
    res = mount_volume(&path_old, &fs, FA_WRITE); /* 获取旧对象的逻辑驱动器 */
    if (res == FR_OK)
    {
        djo.obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(&djo, path_old); /* 检查旧对象 */
        if (res == FR_OK && (djo.fn[NSFLAG] & (NS_DOT | NS_NONAME)))
            res = FR_INVALID_NAME;         /* 检查名称有效性 */
                #if FF_FS_LOCK != 0
        if (res == FR_OK)
        {
            res = chk_lock(&djo, 2);
        }
                #endif
        if (res == FR_OK)
        { /* 找到要重命名的对象 */
                #if FF_FS_EXFAT
            if (fs->fs_type == FS_EXFAT)
            { /* 在 exFAT 卷上 */
                BYTE nf, nn;
                WORD nh;

                memcpy(buf, fs->dirbuf, SZDIRE * 2); /* 保存旧对象的 85+C0 条目 */
                memcpy(&djn, &djo, sizeof djo);
                res = follow_path(&djn, path_new);   /* 确保新对象名未被使用 */
                if (res == FR_OK)
                {                                    /* 新名称已被其他对象使用？ */
                    res = (djn.obj.sclust == djo.obj.sclust && djn.dptr == djo.dptr) ? FR_NO_FILE : FR_EXIST;
                }
                if (res == FR_NO_FILE)
                {                             /* 是有效路径且无名称冲突 */
                    res = dir_register(&djn); /* 注册新条目 */
                    if (res == FR_OK)
                    {
                        nf = fs->dirbuf[XDIR_NumSec];
                        nn = fs->dirbuf[XDIR_NumName];
                        nh = ld_word(fs->dirbuf + XDIR_NameHash);
                        memcpy(fs->dirbuf, buf, SZDIRE * 2); /* 恢复 85+C0 条目 */
                        fs->dirbuf[XDIR_NumSec] = nf;
                        fs->dirbuf[XDIR_NumName] = nn;
                        st_word(fs->dirbuf + XDIR_NameHash, nh);
                        if (!(fs->dirbuf[XDIR_Attr] & AM_DIR))
                            fs->dirbuf[XDIR_Attr] |= AM_ARC; /* 如果是文件，设置存档属性 */
                                                             /* 开始临界区，中断可能导致交叉链接 */
                        res = store_xdir(&djn);
                    }
                }
            }
            else
                #endif
            {                                      /* 在 FAT/FAT32 卷上 */
                memcpy(buf, djo.dir, SZDIRE);      /* 保存对象的目录条目 */
                memcpy(&djn, &djo, sizeof(DIR));   /* 复制目录对象 */
                res = follow_path(&djn, path_new); /* 确保新对象名未被使用 */
                if (res == FR_OK)
                {                                  /* 新名称已被其他对象使用？ */
                    res = (djn.obj.sclust == djo.obj.sclust && djn.dptr == djo.dptr) ? FR_NO_FILE : FR_EXIST;
                }
                if (res == FR_NO_FILE)
                {                             /* 是有效路径且无名称冲突 */
                    res = dir_register(&djn); /* 注册新条目 */
                    if (res == FR_OK)
                    {
                        dir = djn.dir; /* 复制对象的目录条目，除名称外 */
                        memcpy(dir + 13, buf + 13, SZDIRE - 13);
                        dir[DIR_Attr] = buf[DIR_Attr];
                        if (!(dir[DIR_Attr] & AM_DIR))
                            dir[DIR_Attr] |= AM_ARC; /* 如果是文件，设置存档属性 */
                        fs->wflag = 1;
                        if ((dir[DIR_Attr] & AM_DIR) && djo.obj.sclust != djn.obj.sclust)
                        { /* 如果需要，更新子目录中的 .. 条目 */
                            sect = clst2sect(fs, ld_clust(fs, dir));
                            if (sect == 0)
                            {
                                res = FR_INT_ERR;
                            }
                            else
                            {
                                /* 开始临界区，中断可能导致交叉链接 */
                                res = move_window(fs, sect);
                                dir = fs->win + SZDIRE * 1; /* 指向 .. 条目 */
                                if (res == FR_OK && dir[1] == '.')
                                {
                                    st_clust(fs, dir, djn.obj.sclust);
                                    fs->wflag = 1;
                                }
                            }
                        }
                    }
                }
            }
            if (res == FR_OK)
            {
                res = dir_remove(&djo); /* 删除旧条目 */
                if (res == FR_OK)
                {
                    res = sync_fs(fs);
                }
            }
            /* 临界区结束 */
        }
        FREE_NAMBUF();
    }

    LEAVE_FF(fs, res);
}
            #endif /* !FF_FS_READONLY */
        #endif     /* FF_FS_MINIMIZE == 0 */
    #endif         /* FF_FS_MINIMIZE <= 1 */
#endif             /* FF_FS_MINIMIZE <= 2 */

#if FF_USE_CHMOD && !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 更改属性                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_chmod(const TCHAR *path, BYTE attr, BYTE mask)
{
    FRESULT res;
    DIR dj;
    FATFS *fs;
    DEF_NAMBUF

    res = mount_volume(&path, &fs, FA_WRITE); /* 获取逻辑驱动器 */
    if (res == FR_OK)
    {
        dj.obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(&dj, path); /* 跟随文件路径 */
        if (res == FR_OK && (dj.fn[NSFLAG] & (NS_DOT | NS_NONAME)))
            res = FR_INVALID_NAME;    /* 检查对象有效性 */
        if (res == FR_OK)
        {
            mask &= AM_RDO | AM_HID | AM_SYS | AM_ARC; /* 有效属性掩码 */
    #if FF_FS_EXFAT
            if (fs->fs_type == FS_EXFAT)
            {
                fs->dirbuf[XDIR_Attr] = (attr & mask) | (fs->dirbuf[XDIR_Attr] & (BYTE)~mask); /* 应用属性更改 */
                res = store_xdir(&dj);
            }
            else
    #endif
            {
                dj.dir[DIR_Attr] = (attr & mask) | (dj.dir[DIR_Attr] & (BYTE)~mask); /* 应用属性更改 */
                fs->wflag = 1;
            }
            if (res == FR_OK)
            {
                res = sync_fs(fs);
            }
        }
        FREE_NAMBUF();
    }

    LEAVE_FF(fs, res);
}

/*-----------------------------------------------------------------------*/
/* 更改时间戳                                                             */
/*-----------------------------------------------------------------------*/
FRESULT f_utime(const TCHAR *path, const FILINFO *fno)
{
    FRESULT res;
    DIR dj;
    FATFS *fs;
    DEF_NAMBUF

    res = mount_volume(&path, &fs, FA_WRITE); /* 获取逻辑驱动器 */
    if (res == FR_OK)
    {
        dj.obj.fs = fs;
        INIT_NAMBUF(fs);
        res = follow_path(&dj, path); /* 跟随文件路径 */
        if (res == FR_OK && (dj.fn[NSFLAG] & (NS_DOT | NS_NONAME)))
            res = FR_INVALID_NAME;    /* 检查对象有效性 */
        if (res == FR_OK)
        {
    #if FF_FS_EXFAT
            if (fs->fs_type == FS_EXFAT)
            {
                st_dword(fs->dirbuf + XDIR_ModTime, (DWORD)fno->fdate << 16 | fno->ftime);
                res = store_xdir(&dj);
            }
            else
    #endif
            {
                st_dword(dj.dir + DIR_ModTime, (DWORD)fno->fdate << 16 | fno->ftime);
                fs->wflag = 1;
            }
            if (res == FR_OK)
            {
                res = sync_fs(fs);
            }
        }
        FREE_NAMBUF();
    }

    LEAVE_FF(fs, res);
}
#endif /* FF_USE_CHMOD && !FF_FS_READONLY */

#if FF_USE_LABEL
/*-----------------------------------------------------------------------*/
/* 获取卷标                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_getlabel(const TCHAR *path, TCHAR *label, DWORD *vsn)
{
    FRESULT res;
    DIR dj;
    FATFS *fs;
    UINT si, di;
    WCHAR wc;

    /* 获取逻辑驱动器 */
    res = mount_volume(&path, &fs, 0);

    /* 获取卷标 */
    if (res == FR_OK && label)
    {
        dj.obj.fs = fs;
        dj.obj.sclust = 0; /* 打开根目录 */
        res = dir_sdi(&dj, 0);
        if (res == FR_OK)
        {
            res = DIR_READ_LABEL(&dj); /* 查找卷标条目 */
            if (res == FR_OK)
            {
    #if FF_FS_EXFAT
                if (fs->fs_type == FS_EXFAT)
                {
                    WCHAR hs;
                    UINT nw;

                    for (si = di = hs = 0; si < dj.dir[XDIR_NumLabel]; si++)
                    { /* 从 83 条目提取卷标 */
                        wc = ld_word(dj.dir + XDIR_Label + si * 2);
                        if (hs == 0 && IsSurrogate(wc))
                        { /* 是否为代理项？ */
                            hs = wc;
                            continue;
                        }
                        nw = put_utf((DWORD)hs << 16 | wc, &label[di], 4); /* 以 API 编码存储 */
                        if (nw == 0)
                        {
                            di = 0;
                            break;
                        } /* 编码错误？ */
                        di += nw;
                        hs = 0;
                    }
                    if (hs != 0)
                        di = 0; /* 代理项对损坏？ */
                    label[di] = 0;
                }
                else
    #endif
                {
                    si = di = 0; /* 从 AM_VOL 条目提取卷标 */
                    while (si < 11)
                    {
                        wc = dj.dir[si++];
    #if FF_USE_LFN && FF_LFN_UNICODE >= 1                /* Unicode 输出 */
                        if (dbc_1st((BYTE)wc) && si < 11)
                            wc = wc << 8 | dj.dir[si++]; /* 是否为 DBCS？ */
                        wc = ff_oem2uni(wc, CODEPAGE);   /* 转换为 Unicode */
                        if (wc == 0)
                        {
                            di = 0;
                            break;
                        } /* 当前代码页中的无效字符？ */
                        di += put_utf(wc, &label[di], 4); /* 以 Unicode 存储 */
    #else                                                 /* ANSI/OEM 输出 */
                        label[di++] = (TCHAR)wc;
    #endif
                    }
                    do
                    { /* 截断尾随空格 */
                        label[di] = 0;
                        if (di == 0)
                            break;
                    } while (label[--di] == ' ');
                }
            }
        }
        if (res == FR_NO_FILE)
        { /* 无卷标条目，返回空字符串 */
            label[0] = 0;
            res = FR_OK;
        }
    }

    /* 获取卷序列号 */
    if (res == FR_OK && vsn)
    {
        res = move_window(fs, fs->volbase);
        if (res == FR_OK)
        {
            switch (fs->fs_type)
            {
                case FS_EXFAT:
                    di = BPB_VolIDEx;
                    break;
                case FS_FAT32:
                    di = BS_VolID32;
                    break;
                default:
                    di = BS_VolID;
            }
            *vsn = ld_dword(fs->win + di);
        }
    }

    LEAVE_FF(fs, res);
}

    #if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 设置卷标                                                              */
/*-----------------------------------------------------------------------*/
FRESULT f_setlabel(const TCHAR *label)
{
    FRESULT res;
    DIR dj;
    FATFS *fs;
    BYTE dirvn[22];
    UINT di;
    WCHAR wc;
    static const char badchr[18] = "+.,;=[]"
                                   "/*:<>|\\\"\?\x7F"; /* [0..16] 用于 FAT，[7..16] 用于 exFAT */
        #if FF_USE_LFN
    DWORD dc;
        #endif

    /* 获取逻辑驱动器 */
    res = mount_volume(&label, &fs, FA_WRITE);
    if (res != FR_OK)
        LEAVE_FF(fs, res);

        #if FF_FS_EXFAT
    if (fs->fs_type == FS_EXFAT)
    { /* 在 exFAT 卷上 */
        memset(dirvn, 0, 22);
        di = 0;
        while ((UINT)*label >= ' ')
        {                           /* 创建卷标 */
            dc = tchar2uni(&label); /* 获取一个 Unicode 字符 */
            if (dc >= 0x10000)
            {
                if (dc == 0xFFFFFFFF || di >= 10)
                { /* 错误的代理项或缓冲区溢出 */
                    dc = 0;
                }
                else
                {
                    st_word(dirvn + di * 2, (WCHAR)(dc >> 16));
                    di++;
                }
            }
            if (dc == 0 || strchr(&badchr[7], (int)dc) || di >= 11)
            { /* 检查卷标有效性 */
                LEAVE_FF(fs, FR_INVALID_NAME);
            }
            st_word(dirvn + di * 2, (WCHAR)dc);
            di++;
        }
    }
    else
        #endif
    { /* 在 FAT/FAT32 卷上 */
        memset(dirvn, ' ', 11);
        di = 0;
        while ((UINT)*label >= ' ')
        { /* 创建卷标 */
        #if FF_USE_LFN
            dc = tchar2uni(&label);
            wc = (dc < 0x10000) ? ff_uni2oem(ff_wtoupper(dc), CODEPAGE) : 0;
        #else /* ANSI/OEM 输入 */
            wc = (BYTE)*label++;
            if (dbc_1st((BYTE)wc))
                wc = dbc_2nd((BYTE)*label) ? wc << 8 | (BYTE)*label++ : 0;
            if (IsLower(wc))
                wc -= 0x20; /* 转换为大写 ASCII 字符 */
            #if FF_CODE_PAGE == 0
            if (ExCvt && wc >= 0x80)
                wc = ExCvt[wc - 0x80]; /* 转换为大写扩展字符（SBCS 配置） */
            #elif FF_CODE_PAGE < 900
            if (wc >= 0x80)
                wc = ExCvt[wc - 0x80]; /* 转换为大写扩展字符（SBCS 配置） */
            #endif
        #endif
            if (wc == 0 || strchr(&badchr[0], (int)wc) || di >= (UINT)((wc >= 0x100) ? 10 : 11))
            { /* 拒绝卷标的无效字符 */
                LEAVE_FF(fs, FR_INVALID_NAME);
            }
            if (wc >= 0x100)
                dirvn[di++] = (BYTE)(wc >> 8);
            dirvn[di++] = (BYTE)wc;
        }
        if (dirvn[0] == DDEM)
            LEAVE_FF(fs, FR_INVALID_NAME);       /* 拒绝非法名称（首字符为 DDEM） */
        while (di && dirvn[di - 1] == ' ') di--; /* 截断尾随空格 */
    }

    /* 设置卷标 */
    dj.obj.fs = fs;
    dj.obj.sclust = 0; /* 打开根目录 */
    res = dir_sdi(&dj, 0);
    if (res == FR_OK)
    {
        res = DIR_READ_LABEL(&dj); /* 获取卷标条目 */
        if (res == FR_OK)
        {
            if (FF_FS_EXFAT && fs->fs_type == FS_EXFAT)
            {
                dj.dir[XDIR_NumLabel] = (BYTE)di; /* 更改卷标 */
                memcpy(dj.dir + XDIR_Label, dirvn, 22);
            }
            else
            {
                if (di != 0)
                {
                    memcpy(dj.dir, dirvn, 11); /* 更改卷标 */
                }
                else
                {
                    dj.dir[DIR_Name] = DDEM; /* 移除卷标 */
                }
            }
            fs->wflag = 1;
            res = sync_fs(fs);
        }
        else
        { /* 无卷标条目或错误 */
            if (res == FR_NO_FILE)
            {
                res = FR_OK;
                if (di != 0)
                {                            /* 创建卷标条目 */
                    res = dir_alloc(&dj, 1); /* 分配一个条目 */
                    if (res == FR_OK)
                    {
                        memset(dj.dir, 0, SZDIRE); /* 清除条目 */
                        if (FF_FS_EXFAT && fs->fs_type == FS_EXFAT)
                        {
                            dj.dir[XDIR_Type] = ET_VLABEL; /* 创建卷标条目 */
                            dj.dir[XDIR_NumLabel] = (BYTE)di;
                            memcpy(dj.dir + XDIR_Label, dirvn, 22);
                        }
                        else
                        {
                            dj.dir[DIR_Attr] = AM_VOL; /* 创建卷标条目 */
                            memcpy(dj.dir, dirvn, 11);
                        }
                        fs->wflag = 1;
                        res = sync_fs(fs);
                    }
                }
            }
        }
    }

    LEAVE_FF(fs, res);
}
    #endif /* !FF_FS_READONLY */
#endif     /* FF_USE_LABEL */

#if FF_USE_EXPAND && !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* 为文件分配连续块                                                       */
/*-----------------------------------------------------------------------*/
FRESULT f_expand(FIL *fp, FSIZE_t fsz, BYTE opt)
{
    FRESULT res;
    FATFS *fs;
    DWORD n, clst, stcl, scl, ncl, tcl, lclst;

    res = validate(&fp->obj, &fs); /* 检查文件对象的有效性 */
    if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK)
        LEAVE_FF(fs, res);
    if (fsz == 0 || fp->obj.objsize != 0 || !(fp->flag & FA_WRITE))
        LEAVE_FF(fs, FR_DENIED);
    #if FF_FS_EXFAT
    if (fs->fs_type != FS_EXFAT && fsz >= 0x100000000)
        LEAVE_FF(fs, FR_DENIED);                        /* 检查大小限制 */
    #endif
    n = (DWORD)fs->csize * SS(fs);                      /* 簇大小 */
    tcl = (DWORD)(fsz / n) + ((fsz & (n - 1)) ? 1 : 0); /* 所需簇数 */
    stcl = fs->last_clst;
    lclst = 0;
    if (stcl < 2 || stcl >= fs->n_fatent)
        stcl = 2;

    #if FF_FS_EXFAT
    if (fs->fs_type == FS_EXFAT)
    {
        scl = find_bitmap(fs, stcl, tcl); /* 查找一个连续簇块 */
        if (scl == 0)
            res = FR_DENIED;              /* 未找到连续簇块 */
        if (scl == 0xFFFFFFFF)
            res = FR_DISK_ERR;
        if (res == FR_OK)
        {                                             /* 找到连续空闲区 */
            if (opt)
            {                                         /* 立即分配 */
                res = change_bitmap(fs, scl, tcl, 1); /* 将簇块标记为“已用” */
                lclst = scl + tcl - 1;
            }
            else
            { /* 将其设置为下次分配的建议点 */
                lclst = scl - 1;
            }
        }
    }
    else
    #endif
    {
        scl = clst = stcl;
        ncl = 0;
        for (;;)
        { /* 查找一个连续簇块 */
            n = get_fat(&fp->obj, clst);
            if (++clst >= fs->n_fatent)
                clst = 2;
            if (n == 1)
            {
                res = FR_INT_ERR;
                break;
            }
            if (n == 0xFFFFFFFF)
            {
                res = FR_DISK_ERR;
                break;
            }
            if (n == 0)
            {              /* 是否为空闲簇？ */
                if (++ncl == tcl)
                    break; /* 如果找到连续簇块，退出 */
            }
            else
            {
                scl = clst;
                ncl = 0; /* 不是空闲簇 */
            }
            if (clst == stcl)
            {
                res = FR_DENIED;
                break;
            } /* 无连续簇？ */
        }
        if (res == FR_OK)
        {         /* 找到连续空闲区 */
            if (opt)
            {     /* 立即分配 */
                for (clst = scl, n = tcl; n; clst++, n--)
                { /* 在 FAT 上创建簇链 */
                    res = put_fat(fs, clst, (n == 1) ? 0xFFFFFFFF : clst + 1);
                    if (res != FR_OK)
                        break;
                    lclst = clst;
                }
            }
            else
            { /* 将其设置为下次分配的建议点 */
                lclst = scl - 1;
            }
        }
    }

    if (res == FR_OK)
    {
        fs->last_clst = lclst;    /* 设置建议起始簇供下次分配 */
        if (opt)
        {                         /* 是否已分配？ */
            fp->obj.sclust = scl; /* 更新对象分配信息 */
            fp->obj.objsize = fsz;
            if (FF_FS_EXFAT)
                fp->obj.stat = 2; /* 设置状态为“连续链” */
            fp->flag |= FA_MODIFIED;
            if (fs->free_clst <= fs->n_fatent - 2)
            { /* 更新 FSINFO */
                fs->free_clst -= tcl;
                fs->fsi_flag |= 1;
            }
        }
    }

    LEAVE_FF(fs, res);
}
#endif /* FF_USE_EXPAND && !FF_FS_READONLY */

#if FF_USE_FORWARD
/*-----------------------------------------------------------------------*/
/* 将数据直接转发到流                                                     */
/*-----------------------------------------------------------------------*/
FRESULT f_forward(FIL *fp, UINT (*func)(const BYTE *, UINT), UINT btf, UINT *bf)
{
    FRESULT res;
    FATFS *fs;
    DWORD clst;
    LBA_t sect;
    FSIZE_t remain;
    UINT rcnt, csect;
    BYTE *dbuf;

    *bf = 0;                       /* 清除传输字节计数器 */
    res = validate(&fp->obj, &fs); /* 检查文件对象的有效性 */
    if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK)
        LEAVE_FF(fs, res);
    if (!(fp->flag & FA_READ))
        LEAVE_FF(fs, FR_DENIED); /* 检查访问模式 */

    remain = fp->obj.objsize - fp->fptr;
    if (btf > remain)
        btf = (UINT)remain; /* 按剩余字节截断 btf */

    for (; btf > 0 && (*func)(0, 0); fp->fptr += rcnt, *bf += rcnt, btf -= rcnt)
    {                                                        /* 重复直到所有数据传输或流忙 */
        csect = (UINT)(fp->fptr / SS(fs) & (fs->csize - 1)); /* 簇内扇区偏移 */
        if (fp->fptr % SS(fs) == 0)
        {                                                    /* 在扇区边界？ */
            if (csect == 0)
            {                                                /* 在簇边界？ */
                clst = (fp->fptr == 0) ?                     /* 在文件顶部？ */
                           fp->obj.sclust
                                       : get_fat(&fp->obj, fp->clust);
                if (clst <= 1)
                    ABORT(fs, FR_INT_ERR);
                if (clst == 0xFFFFFFFF)
                    ABORT(fs, FR_DISK_ERR);
                fp->clust = clst; /* 更新当前簇 */
            }
        }
        sect = clst2sect(fs, fp->clust); /* 获取当前数据扇区 */
        if (sect == 0)
            ABORT(fs, FR_INT_ERR);
        sect += csect;
    #if FF_FS_TINY
        if (move_window(fs, sect) != FR_OK)
            ABORT(fs, FR_DISK_ERR); /* 将扇区窗口移动到文件数据 */
        dbuf = fs->win;
    #else
        if (fp->sect != sect)
        { /* 用文件数据填充扇区缓存 */
        #if !FF_FS_READONLY
            if (fp->flag & FA_DIRTY)
            { /* 回写脏扇区缓存 */
                if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) != RES_OK)
                    ABORT(fs, FR_DISK_ERR);
                fp->flag &= (BYTE)~FA_DIRTY;
            }
        #endif
            if (disk_read(fs->pdrv, fp->buf, sect, 1) != RES_OK)
                ABORT(fs, FR_DISK_ERR);
        }
        dbuf = fp->buf;
    #endif
        fp->sect = sect;
        rcnt = SS(fs) - (UINT)fp->fptr % SS(fs);                /* 扇区中剩余字节数 */
        if (rcnt > btf)
            rcnt = btf;                                         /* 如果需要，用 btf 截断 */
        rcnt = (*func)(dbuf + ((UINT)fp->fptr % SS(fs)), rcnt); /* 转发文件数据 */
        if (rcnt == 0)
            ABORT(fs, FR_INT_ERR);
    }

    LEAVE_FF(fs, FR_OK);
}
#endif /* FF_USE_FORWARD */

#if !FF_FS_READONLY && FF_USE_MKFS
/*-----------------------------------------------------------------------*/
/* 创建 FAT/exFAT 卷（带子函数）                                          */
/*-----------------------------------------------------------------------*/

    #define N_SEC_TRACK 63     /* 确定驱动器 CHS 的每磁道扇区数 */
    #define GPT_ALIGN 0x100000 /* GPT 中分区对齐 [字节] (>=128KB) */
    #define GPT_ITEMS 128      /* GPT 表大小（>=128，扇区对齐） */

/* 在 MBR 或 GPT 格式的物理驱动器上创建分区 */
static FRESULT create_partition(BYTE drv, const LBA_t plst[], BYTE sys, BYTE *buf)
{
    UINT i, cy;
    LBA_t sz_drv;
    DWORD sz_drv32, nxt_alloc32, sz_part32;
    BYTE *pte;
    BYTE hd, n_hd, sc, n_sc;

    /* 获取物理驱动器大小 */
    if (disk_ioctl(drv, GET_SECTOR_COUNT, &sz_drv) != RES_OK)
        return FR_DISK_ERR;

    #if FF_LBA64
    if (sz_drv >= FF_MIN_GPT)
    { /* 以 GPT 格式创建分区 */
        WORD ss;
        UINT sz_ptbl, pi, si, ofs;
        DWORD bcc, rnd, align;
        QWORD nxt_alloc, sz_part, sz_pool, top_bpt;
        static const BYTE gpt_mbr[16] = {0x00, 0x00, 0x02, 0x00, 0xEE, 0xFE, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};

        #if FF_MAX_SS != FF_MIN_SS
        if (disk_ioctl(drv, GET_SECTOR_SIZE, &ss) != RES_OK)
            return FR_DISK_ERR; /* 获取扇区大小 */
        if (ss > FF_MAX_SS || ss < FF_MIN_SS || (ss & (ss - 1)))
            return FR_DISK_ERR;
        #else
        ss = FF_MAX_SS;
        #endif
        rnd = (DWORD)sz_drv + GET_FATTIME(); /* 随机种子 */
        align = GPT_ALIGN / ss;              /* GPT 的分区对齐 [扇区] */
        sz_ptbl = GPT_ITEMS * SZ_GPTE / ss;  /* 分区表大小 [扇区] */
        top_bpt = sz_drv - sz_ptbl - 1;      /* 备份分区表起始扇区 */
        nxt_alloc = 2 + sz_ptbl;             /* 第一个可分配扇区 */
        sz_pool = top_bpt - nxt_alloc;       /* 可分配区大小 */
        bcc = 0xFFFFFFFF;
        sz_part = 1;
        pi = si = 0; /* 分区表索引，大小表索引 */
        do
        {
            if (pi * SZ_GPTE % ss == 0)
                memset(buf, 0, ss);                                       /* 如果需要，清除缓冲区 */
            if (sz_part != 0)
            {                                                             /* 大小表未终止？ */
                nxt_alloc = (nxt_alloc + align - 1) & ((QWORD)0 - align); /* 对齐分区起始 */
                sz_part = plst[si++];                                     /* 获取分区大小 */
                if (sz_part <= 100)
                {                                                         /* 大小是否为百分比？ */
                    sz_part = sz_pool * sz_part / 100;
                    sz_part = (sz_part + align - 1) & ((QWORD)0 - align); /* 对齐分区结束（仅当为百分比时） */
                }
                if (nxt_alloc + sz_part > top_bpt)
                { /* 在池尾处截断大小 */
                    sz_part = (nxt_alloc < top_bpt) ? top_bpt - nxt_alloc : 0;
                }
            }
            if (sz_part != 0)
            {                                                               /* 添加一个分区？ */
                ofs = pi * SZ_GPTE % ss;
                memcpy(buf + ofs + GPTE_PtGuid, GUID_MS_Basic, 16);         /* 设置分区 GUID（Microsoft 基本数据） */
                rnd = make_rand(rnd, buf + ofs + GPTE_UpGuid, 16);          /* 设置唯一分区 GUID */
                st_qword(buf + ofs + GPTE_FstLba, nxt_alloc);               /* 设置分区起始扇区 */
                st_qword(buf + ofs + GPTE_LstLba, nxt_alloc + sz_part - 1); /* 设置分区结束扇区 */
                nxt_alloc += sz_part;                                       /* 下一个可分配扇区 */
            }
            if ((pi + 1) * SZ_GPTE % ss == 0)
            {                                                    /* 如果缓冲区已满，写入 */
                for (i = 0; i < ss; bcc = crc32(bcc, buf[i++])); /* 计算表校验和 */
                if (disk_write(drv, buf, 2 + pi * SZ_GPTE / ss, 1) != RES_OK)
                    return FR_DISK_ERR;                          /* 写入主表 */
                if (disk_write(drv, buf, top_bpt + pi * SZ_GPTE / ss, 1) != RES_OK)
                    return FR_DISK_ERR;                          /* 写入辅表 */
            }
        } while (++pi < GPT_ITEMS);

        /* 创建主 GPT 头 */
        memset(buf, 0, ss);
        memcpy(buf + GPTH_Sign, "EFI PART"
                                "\0\0\1\0"
                                "\x5C\0\0",
               16);                                                        /* 签名，版本 (1.0) 和大小 (92) */
        st_dword(buf + GPTH_PtBcc, ~bcc);                                  /* 表校验和 */
        st_qword(buf + GPTH_CurLba, 1);                                    /* 此头的 LBA */
        st_qword(buf + GPTH_BakLba, sz_drv - 1);                           /* 备份头的 LBA */
        st_qword(buf + GPTH_FstLba, 2 + sz_ptbl);                          /* 第一个可分配扇区的 LBA */
        st_qword(buf + GPTH_LstLba, top_bpt - 1);                          /* 最后一个可分配扇区的 LBA */
        st_dword(buf + GPTH_PteSize, SZ_GPTE);                             /* 表项大小 */
        st_dword(buf + GPTH_PtNum, GPT_ITEMS);                             /* 表项数 */
        st_dword(buf + GPTH_PtOfs, 2);                                     /* 此表的 LBA */
        rnd = make_rand(rnd, buf + GPTH_DskGuid, 16);                      /* 磁盘 GUID */
        for (i = 0, bcc = 0xFFFFFFFF; i < 92; bcc = crc32(bcc, buf[i++])); /* 计算头校验和 */
        st_dword(buf + GPTH_Bcc, ~bcc);                                    /* 头校验和 */
        if (disk_write(drv, buf, 1, 1) != RES_OK)
            return FR_DISK_ERR;

        /* 创建辅 GPT 头 */
        st_qword(buf + GPTH_CurLba, sz_drv - 1);                           /* 此头的 LBA */
        st_qword(buf + GPTH_BakLba, 1);                                    /* 主头的 LBA */
        st_qword(buf + GPTH_PtOfs, top_bpt);                               /* 此表的 LBA */
        st_dword(buf + GPTH_Bcc, 0);
        for (i = 0, bcc = 0xFFFFFFFF; i < 92; bcc = crc32(bcc, buf[i++])); /* 计算头校验和 */
        st_dword(buf + GPTH_Bcc, ~bcc);                                    /* 头校验和 */
        if (disk_write(drv, buf, sz_drv - 1, 1) != RES_OK)
            return FR_DISK_ERR;

        /* 创建保护 MBR */
        memset(buf, 0, ss);
        memcpy(buf + MBR_Table, gpt_mbr, 16); /* 创建 GPT 分区 */
        st_word(buf + BS_55AA, 0xAA55);
        if (disk_write(drv, buf, 0, 1) != RES_OK)
            return FR_DISK_ERR;
    }
    else
    #endif
    {                       /* 以 MBR 格式创建分区 */
        sz_drv32 = (DWORD)sz_drv;
        n_sc = N_SEC_TRACK; /* 确定驱动器 CHS，不考虑驱动器几何 */
        for (n_hd = 8; n_hd != 0 && sz_drv32 / n_hd / n_sc > 1024; n_hd *= 2);
        if (n_hd == 0)
            n_hd = 255;            /* 磁头数需 <256 */

        memset(buf, 0, FF_MAX_SS); /* 清除 MBR */
        pte = buf + MBR_Table;     /* MBR 中的分区表 */
        for (i = 0, nxt_alloc32 = n_sc; i < 4 && nxt_alloc32 != 0 && nxt_alloc32 < sz_drv32; i++, nxt_alloc32 += sz_part32)
        {
            sz_part32 = (DWORD)plst[i];                                                 /* 获取分区大小 */
            if (sz_part32 <= 100)
                sz_part32 = (sz_part32 == 100) ? sz_drv32 : sz_drv32 / 100 * sz_part32; /* 百分比大小？ */
            if (nxt_alloc32 + sz_part32 > sz_drv32 || nxt_alloc32 + sz_part32 < nxt_alloc32)
                sz_part32 = sz_drv32 - nxt_alloc32;                                     /* 在驱动器大小处截断 */
            if (sz_part32 == 0)
                break;                                                                  /* 表结束或没有扇区可分配？ */

            st_dword(pte + PTE_StLba, nxt_alloc32);                                     /* 起始 LBA */
            st_dword(pte + PTE_SizLba, sz_part32);                                      /* 扇区数 */
            pte[PTE_System] = sys;                                                      /* 系统类型 */

            cy = (UINT)(nxt_alloc32 / n_sc / n_hd);                                     /* 起始柱面 */
            hd = (BYTE)(nxt_alloc32 / n_sc % n_hd);                                     /* 起始磁头 */
            sc = (BYTE)(nxt_alloc32 % n_sc + 1);                                        /* 起始扇区 */
            pte[PTE_StHead] = hd;
            pte[PTE_StSec] = (BYTE)((cy >> 2 & 0xC0) | sc);
            pte[PTE_StCyl] = (BYTE)cy;

            cy = (UINT)((nxt_alloc32 + sz_part32 - 1) / n_sc / n_hd); /* 结束柱面 */
            hd = (BYTE)((nxt_alloc32 + sz_part32 - 1) / n_sc % n_hd); /* 结束磁头 */
            sc = (BYTE)((nxt_alloc32 + sz_part32 - 1) % n_sc + 1);    /* 结束扇区 */
            pte[PTE_EdHead] = hd;
            pte[PTE_EdSec] = (BYTE)((cy >> 2 & 0xC0) | sc);
            pte[PTE_EdCyl] = (BYTE)cy;

            pte += SZ_PTE; /* 下一个条目 */
        }

        st_word(buf + BS_55AA, 0xAA55); /* MBR 签名 */
        if (disk_write(drv, buf, 0, 1) != RES_OK)
            return FR_DISK_ERR;         /* 写入 MBR */
    }

    return FR_OK;
}

FRESULT f_mkfs(const TCHAR *path, const MKFS_PARM *opt, void *work, UINT len)
{
    static const WORD cst[] = {1, 4, 16, 64, 256, 512, 0}; /* FAT 卷的簇大小边界（4KB 单位） */
    static const WORD cst32[] = {1, 2, 4, 8, 16, 32, 0};   /* FAT32 卷的簇大小边界（128KB 单位） */
    static const MKFS_PARM defopt = {FM_ANY, 0, 0, 0, 0};  /* 默认参数 */
    BYTE fsopt, fsty, sys, *buf, *pte, pdrv, ipart;
    WORD ss;                                               /* 扇区大小 */
    DWORD sz_buf, sz_blk, n_clst, pau, nsect, n, vsn;
    LBA_t sz_vol, b_vol, b_fat, b_data;                    /* 卷大小、卷基 LBA、FAT 基、数据基 */
    LBA_t sect, lba[2];
    DWORD sz_rsv, sz_fat, sz_dir, sz_au;                   /* 保留区大小、FAT 大小、目录大小、数据区大小、簇大小 */
    UINT n_fat, n_root, i;                                 /* 索引、FAT 数和根目录条目数 */
    int vol;
    DSTATUS ds;
    FRESULT fr;

    /* 检查已挂载的驱动器并清除工作区 */
    vol = get_ldnumber(&path); /* 获取目标逻辑驱动器 */
    if (vol < 0)
        return FR_INVALID_DRIVE;
    if (FatFs[vol])
        FatFs[vol]->fs_type = 0; /* 如果已挂载，清除 fs 对象 */
    pdrv = LD2PD(vol);           /* 物理驱动器 */
    ipart = LD2PT(vol);          /* 分区（0：作为新创建，1..：从分区表获取） */
    if (!opt)
        opt = &defopt;           /* 如果未给定，使用默认参数 */

    /* 获取物理驱动器状态（sz_drv, sz_blk, ss） */
    ds = disk_initialize(pdrv);
    if (ds & STA_NOINIT)
        return FR_NOT_READY;
    if (ds & STA_PROTECT)
        return FR_WRITE_PROTECTED;
    sz_blk = opt->align;
    if (sz_blk == 0 && disk_ioctl(pdrv, GET_BLOCK_SIZE, &sz_blk) != RES_OK)
        sz_blk = 1;
    if (sz_blk == 0 || sz_blk > 0x8000 || (sz_blk & (sz_blk - 1)))
        sz_blk = 1;
    #if FF_MAX_SS != FF_MIN_SS
    if (disk_ioctl(pdrv, GET_SECTOR_SIZE, &ss) != RES_OK)
        return FR_DISK_ERR;
    if (ss > FF_MAX_SS || ss < FF_MIN_SS || (ss & (ss - 1)))
        return FR_DISK_ERR;
    #else
    ss = FF_MAX_SS;
    #endif
    /* FAT 子类型和 FAT 参数的选项 */
    fsopt = opt->fmt & (FM_ANY | FM_SFD);
    n_fat = (opt->n_fat >= 1 && opt->n_fat <= 2) ? opt->n_fat : 1;
    n_root = (opt->n_root >= 1 && opt->n_root <= 32768 && (opt->n_root % (ss / SZDIRE)) == 0) ? opt->n_root : 512;
    sz_au = (opt->au_size <= 0x1000000 && (opt->au_size & (opt->au_size - 1)) == 0) ? opt->au_size : 0;
    sz_au /= ss; /* 字节 -> 扇区 */

    /* 获取工作缓冲区 */
    sz_buf = len / ss; /* 工作缓冲区大小 [扇区] */
    if (sz_buf == 0)
        return FR_NOT_ENOUGH_CORE;
    buf = (BYTE *)work; /* 工作缓冲区 */
    #if FF_USE_LFN == 3
    if (!buf)
        buf = ff_memalloc(sz_buf * ss); /* 使用堆内存作为工作缓冲区 */
    #endif
    if (!buf)
        return FR_NOT_ENOUGH_CORE;

    /* 确定卷的位置 (b_vol, sz_vol) */
    b_vol = sz_vol = 0;
    if (FF_MULTI_PARTITION && ipart != 0)
    { /* 卷是否与任何特定分区关联？ */
        /* 从现有分区表获取分区位置 */
        if (disk_read(pdrv, buf, 0, 1) != RES_OK)
            LEAVE_MKFS(FR_DISK_ERR);     /* 加载 MBR */
        if (ld_word(buf + BS_55AA) != 0xAA55)
            LEAVE_MKFS(FR_MKFS_ABORTED); /* 检查 MBR 是否有效 */
    #if FF_LBA64
        if (buf[MBR_Table + PTE_System] == 0xEE)
        { /* GPT 保护 MBR？ */
            DWORD n_ent, ofs;
            QWORD pt_lba;

            /* 从 GPT 获取分区位置 */
            if (disk_read(pdrv, buf, 1, 1) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);         /* 加载 GPT 头扇区（MBR 后一扇区） */
            if (!test_gpt_header(buf))
                LEAVE_MKFS(FR_MKFS_ABORTED);     /* 检查 GPT 头是否有效 */
            n_ent = ld_dword(buf + GPTH_PtNum);  /* 条目数 */
            pt_lba = ld_qword(buf + GPTH_PtOfs); /* 表起始扇区 */
            ofs = i = 0;
            while (n_ent)
            {                                /* 按 ipart 顺序查找 MS 基本分区 */
                if (ofs == 0 && disk_read(pdrv, buf, pt_lba++, 1) != RES_OK)
                    LEAVE_MKFS(FR_DISK_ERR); /* 获取 PT 扇区 */
                if (!memcmp(buf + ofs + GPTE_PtGuid, GUID_MS_Basic, 16) && ++i == ipart)
                {                            /* MS 基本数据分区？ */
                    b_vol = ld_qword(buf + ofs + GPTE_FstLba);
                    sz_vol = ld_qword(buf + ofs + GPTE_LstLba) - b_vol + 1;
                    break;
                }
                n_ent--;
                ofs = (ofs + SZ_GPTE) % ss; /* 下一个条目 */
            }
            if (n_ent == 0)
                LEAVE_MKFS(FR_MKFS_ABORTED); /* 未找到分区 */
            fsopt |= 0x80;                   /* 分区格式为 GPT */
        }
        else
    #endif
        { /* 从 MBR 分区表获取分区位置 */
            pte = buf + (MBR_Table + (ipart - 1) * SZ_PTE);
            if (ipart > 4 || pte[PTE_System] == 0)
                LEAVE_MKFS(FR_MKFS_ABORTED);     /* 无分区？ */
            b_vol = ld_dword(pte + PTE_StLba);   /* 获取卷起始扇区 */
            sz_vol = ld_dword(pte + PTE_SizLba); /* 获取卷大小 */
        }
    }
    else
    { /* 卷与物理驱动器关联 */
        if (disk_ioctl(pdrv, GET_SECTOR_COUNT, &sz_vol) != RES_OK)
            LEAVE_MKFS(FR_DISK_ERR);
        if (!(fsopt & FM_SFD))
        {   /* 将要分区？ */
            /* 在此函数中在驱动器上创建单个分区 */
    #if FF_LBA64
            if (sz_vol >= FF_MIN_GPT)
            {                                                   /* 要创建的分区类型，MBR 还是 GPT？ */
                fsopt |= 0x80;                                  /* 分区格式为 GPT */
                b_vol = GPT_ALIGN / ss;
                sz_vol -= b_vol + GPT_ITEMS * SZ_GPTE / ss + 1; /* 估计分区偏移和大小 */
            }
            else
    #endif
            { /* 分区格式为 MBR */
                if (sz_vol > N_SEC_TRACK)
                {
                    b_vol = N_SEC_TRACK;
                    sz_vol -= b_vol; /* 估计分区偏移和大小 */
                }
            }
        }
    }
    if (sz_vol < 128)
        LEAVE_MKFS(FR_MKFS_ABORTED); /* 检查卷大小是否 >=128 扇区 */

    /* 现在开始在 b_vol 和 sz_vol 处创建 FAT 卷 */

    do
    {         /* 预先确定 FAT 类型 */
        if (FF_FS_EXFAT && (fsopt & FM_EXFAT))
        {     /* 可能为 exFAT？ */
            if ((fsopt & FM_ANY) == FM_EXFAT || sz_vol >= 0x4000000 || sz_au > 128)
            { /* 仅 exFAT，卷 >= 64MS 或 sz_au > 128S？ */
                fsty = FS_EXFAT;
                break;
            }
        }
    #if FF_LBA64
        if (sz_vol >= 0x100000000)
            LEAVE_MKFS(FR_MKFS_ABORTED); /* 对于 FAT/FAT32 来说卷太大 */
    #endif
        if (sz_au > 128)
            sz_au = 128; /* 对于 FAT/FAT32 无效的 AU？ */
        if (fsopt & FM_FAT32)
        {                /* 可能为 FAT32？ */
            if (!(fsopt & FM_FAT))
            {            /* 非 FAT？ */
                fsty = FS_FAT32;
                break;
            }
        }
        if (!(fsopt & FM_FAT))
            LEAVE_MKFS(FR_INVALID_PARAMETER); /* 非 FAT？ */
        fsty = FS_FAT16;
    } while (0);

    vsn = (DWORD)sz_vol + GET_FATTIME(); /* VSN 由当前时间和分区大小生成 */

    #if FF_FS_EXFAT
    if (fsty == FS_EXFAT)
    { /* 创建 exFAT 卷 */
        DWORD szb_bit, szb_case, sum, nbit, clu, clen[3];
        WCHAR ch, si;
        UINT j, st;

        if (sz_vol < 0x1000)
            LEAVE_MKFS(FR_MKFS_ABORTED); /* 对于 exFAT 来说卷太小？ */
        #if FF_USE_TRIM
        lba[0] = b_vol;
        lba[1] = b_vol + sz_vol - 1; /* 通知存储设备卷区可能被擦除 */
        disk_ioctl(pdrv, CTRL_TRIM, lba);
        #endif
        /* 确定 FAT 位置、数据位置和簇数 */
        if (sz_au == 0)
        { /* AU 自动选择 */
            sz_au = 8;
            if (sz_vol >= 0x80000)
                sz_au = 64;                                            /* >= 512Ks */
            if (sz_vol >= 0x4000000)
                sz_au = 256;                                           /* >= 64Ms */
        }
        b_fat = b_vol + 32;                                            /* FAT 起始于偏移 32 */
        sz_fat = (DWORD)((sz_vol / sz_au + 2) * 4 + ss - 1) / ss;      /* FAT 扇区数 */
        b_data = (b_fat + sz_fat + sz_blk - 1) & ~((LBA_t)sz_blk - 1); /* 将数据区对齐到擦除块边界 */
        if (b_data - b_vol >= sz_vol / 2)
            LEAVE_MKFS(FR_MKFS_ABORTED);                               /* 卷太小？ */
        n_clst = (DWORD)(sz_vol - (b_data - b_vol)) / sz_au;           /* 簇数 */
        if (n_clst < 16)
            LEAVE_MKFS(FR_MKFS_ABORTED);                               /* 簇太少？ */
        if (n_clst > MAX_EXFAT)
            LEAVE_MKFS(FR_MKFS_ABORTED);                               /* 簇太多？ */

        szb_bit = (n_clst + 7) / 8;                                    /* 分配位图大小 */
        clen[0] = (szb_bit + sz_au * ss - 1) / (sz_au * ss);           /* 分配位图簇数 */

        /* 创建压缩的大写表 */
        sect = b_data + sz_au * clen[0]; /* 表起始扇区 */
        sum = 0;                         /* 要存储在 82 条目中的表校验和 */
        st = 0;
        si = 0;
        i = 0;
        j = 0;
        szb_case = 0;
        do
        {
            switch (st)
            {
                case 0:
                    ch = (WCHAR)ff_wtoupper(si); /* 获取一个大写字符 */
                    if (ch != si)
                    {
                        si++;
                        break;                                                                            /* 如果存在大写字符，存储 */
                    }
                    for (j = 1; (WCHAR)(si + j) && (WCHAR)(si + j) == ff_wtoupper((WCHAR)(si + j)); j++); /* 获取无大小写块的运行长度 */
                    if (j >= 128)
                    {
                        ch = 0xFFFF;
                        st = 2;
                        break; /* 如果运行 >= 128 字符，压缩无大小写块 */
                    }
                    st = 1;    /* 不压缩短运行 */
                    /* FALLTHROUGH */
                case 1:
                    ch = si++; /* 填充短运行 */
                    if (--j == 0)
                        st = 0;
                    break;

                default:
                    ch = (WCHAR)j;
                    si += (WCHAR)j; /* 要跳过的字符数 */
                    st = 0;
            }
            sum = xsum32(buf[i + 0] = (BYTE)ch, sum); /* 放入写入缓冲区 */
            sum = xsum32(buf[i + 1] = (BYTE)(ch >> 8), sum);
            i += 2;
            szb_case += 2;
            if (si == 0 || i == sz_buf * ss)
            { /* 当缓冲区满或过程结束时，写入缓冲数据 */
                n = (i + ss - 1) / ss;
                if (disk_write(pdrv, buf, sect, n) != RES_OK)
                    LEAVE_MKFS(FR_DISK_ERR);
                sect += n;
                i = 0;
            }
        } while (si);
        clen[1] = (szb_case + sz_au * ss - 1) / (sz_au * ss); /* 大写表簇数 */
        clen[2] = 1;                                          /* 根目录簇数 */

        /* 初始化分配位图 */
        sect = b_data;
        nsect = (szb_bit + ss - 1) / ss;    /* 位图起始和位图扇区数 */
        nbit = clen[0] + clen[1] + clen[2]; /* 系统使用的簇数（位图、大写表和根目录） */
        do
        {
            memset(buf, 0, sz_buf * ss);                                                            /* 初始化位图缓冲区 */
            for (i = 0; nbit != 0 && i / 8 < sz_buf * ss; buf[i / 8] |= 1 << (i % 8), i++, nbit--); /* 标记已用簇 */
            n = (nsect > sz_buf) ? sz_buf : nsect;                                                  /* 写入缓冲数据 */
            if (disk_write(pdrv, buf, sect, n) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);
            sect += n;
            nsect -= n;
        } while (nsect);

        /* 初始化 FAT */
        sect = b_fat;
        nsect = sz_fat; /* FAT 起始和 FAT 扇区数 */
        j = nbit = clu = 0;
        do
        {
            memset(buf, 0, sz_buf * ss);
            i = 0; /* 清除工作区并重置写入偏移 */
            if (clu == 0)
            {      /* 初始化 FAT[0] 和 FAT[1] */
                st_dword(buf + i, 0xFFFFFFF8);
                i += 4;
                clu++;
                st_dword(buf + i, 0xFFFFFFFF);
                i += 4;
                clu++;
            }
            do
            {     /* 创建位图、大写表和根目录的链 */
                while (nbit != 0 && i < sz_buf * ss)
                { /* 创建一个链 */
                    st_dword(buf + i, (nbit > 1) ? clu + 1 : 0xFFFFFFFF);
                    i += 4;
                    clu++;
                    nbit--;
                }
                if (nbit == 0 && j < 3)
                    nbit = clen[j++];              /* 获取下一个链长度 */
            } while (nbit != 0 && i < sz_buf * ss);
            n = (nsect > sz_buf) ? sz_buf : nsect; /* 写入缓冲数据 */
            if (disk_write(pdrv, buf, sect, n) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);
            sect += n;
            nsect -= n;
        } while (nsect);

        /* 初始化根目录 */
        memset(buf, 0, sz_buf * ss);
        buf[SZDIRE * 0 + 0] = ET_VLABEL;              /* 卷标条目（无标号） */
        buf[SZDIRE * 1 + 0] = ET_BITMAP;              /* 位图条目 */
        st_dword(buf + SZDIRE * 1 + 20, 2);           /*  簇 */
        st_dword(buf + SZDIRE * 1 + 24, szb_bit);     /*  大小 */
        buf[SZDIRE * 2 + 0] = ET_UPCASE;              /* 大写表条目 */
        st_dword(buf + SZDIRE * 2 + 4, sum);          /*  校验和 */
        st_dword(buf + SZDIRE * 2 + 20, 2 + clen[0]); /*  簇 */
        st_dword(buf + SZDIRE * 2 + 24, szb_case);    /*  大小 */
        sect = b_data + sz_au * (clen[0] + clen[1]);
        nsect = sz_au;                                /* 根目录起始和扇区数 */
        do
        {                                             /* 填充根目录扇区 */
            n = (nsect > sz_buf) ? sz_buf : nsect;
            if (disk_write(pdrv, buf, sect, n) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);
            memset(buf, 0, ss); /* 剩余条目填充为零 */
            sect += n;
            nsect -= n;
        } while (nsect);

        /* 创建两套 exFAT VBR 块 */
        sect = b_vol;
        for (n = 0; n < 2; n++)
        {
            /* 主记录 (+0) */
            memset(buf, 0, ss);
            memcpy(buf + BS_JmpBoot, "\xEB\x76\x90"
                                     "EXFAT   ",
                   11);                                                                   /* 启动跳转代码 (x86), OEM 名称 */
            st_qword(buf + BPB_VolOfsEx, b_vol);                                          /* 物理驱动器中的卷偏移 [扇区] */
            st_qword(buf + BPB_TotSecEx, sz_vol);                                         /* 卷大小 [扇区] */
            st_dword(buf + BPB_FatOfsEx, (DWORD)(b_fat - b_vol));                         /* FAT 偏移 [扇区] */
            st_dword(buf + BPB_FatSzEx, sz_fat);                                          /* FAT 大小 [扇区] */
            st_dword(buf + BPB_DataOfsEx, (DWORD)(b_data - b_vol));                       /* 数据偏移 [扇区] */
            st_dword(buf + BPB_NumClusEx, n_clst);                                        /* 簇数 */
            st_dword(buf + BPB_RootClusEx, 2 + clen[0] + clen[1]);                        /* 根目录簇# */
            st_dword(buf + BPB_VolIDEx, vsn);                                             /* VSN */
            st_word(buf + BPB_FSVerEx, 0x100);                                            /* 文件系统版本 (1.00) */
            for (buf[BPB_BytsPerSecEx] = 0, i = ss; i >>= 1; buf[BPB_BytsPerSecEx]++);    /* 扇区大小对数 [字节] */
            for (buf[BPB_SecPerClusEx] = 0, i = sz_au; i >>= 1; buf[BPB_SecPerClusEx]++); /* 簇大小对数 [扇区] */
            buf[BPB_NumFATsEx] = 1;                                                       /* FAT 数 */
            buf[BPB_DrvNumEx] = 0x80;                                                     /* 驱动器号 (用于 int13) */
            st_word(buf + BS_BootCodeEx, 0xFEEB);                                         /* 启动代码 (x86) */
            st_word(buf + BS_55AA, 0xAA55);                                               /* 签名（无论扇区大小，均放在此处） */
            for (i = sum = 0; i < ss; i++)
            {                                                                             /* VBR 校验和 */
                if (i != BPB_VolFlagEx && i != BPB_VolFlagEx + 1 && i != BPB_PercInUseEx)
                    sum = xsum32(buf[i], sum);
            }
            if (disk_write(pdrv, buf, sect++, 1) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);
            /* 扩展引导记录 (+1..+8) */
            memset(buf, 0, ss);
            st_word(buf + ss - 2, 0xAA55); /* 签名（放在扇区末尾） */
            for (j = 1; j < 9; j++)
            {
                for (i = 0; i < ss; sum = xsum32(buf[i++], sum)); /* VBR 校验和 */
                if (disk_write(pdrv, buf, sect++, 1) != RES_OK)
                    LEAVE_MKFS(FR_DISK_ERR);
            }
            /* OEM/保留记录 (+9..+10) */
            memset(buf, 0, ss);
            for (; j < 11; j++)
            {
                for (i = 0; i < ss; sum = xsum32(buf[i++], sum)); /* VBR 校验和 */
                if (disk_write(pdrv, buf, sect++, 1) != RES_OK)
                    LEAVE_MKFS(FR_DISK_ERR);
            }
            /* 和记录 (+11) */
            for (i = 0; i < ss; i += 4) st_dword(buf + i, sum); /* 用校验和值填充 */
            if (disk_write(pdrv, buf, sect++, 1) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);
        }
    }
    else
    #endif /* FF_FS_EXFAT */
    {      /* 创建 FAT/FAT32 卷 */
        do
        {
            pau = sz_au;
            /* 预先确定簇数和 FAT 子类型 */
            if (fsty == FS_FAT32)
            {                                                                        /* FAT32 卷 */
                if (pau == 0)
                {                                                                    /* AU 自动选择 */
                    n = (DWORD)sz_vol / 0x20000;                                     /* 以 128KB 为单位的卷大小 */
                    for (i = 0, pau = 1; cst32[i] && cst32[i] <= n; i++, pau <<= 1); /* 从表中获取 */
                }
                n_clst = (DWORD)sz_vol / pau;                                        /* 簇数 */
                sz_fat = (n_clst * 4 + 8 + ss - 1) / ss;                             /* FAT 大小 [扇区] */
                sz_rsv = 32;                                                         /* 保留扇区数 */
                sz_dir = 0;                                                          /* 无静态目录 */
                if (n_clst <= MAX_FAT16 || n_clst > MAX_FAT32)
                    LEAVE_MKFS(FR_MKFS_ABORTED);
            }
            else
            {                                                                    /* FAT 卷 */
                if (pau == 0)
                {                                                                /* au 自动选择 */
                    n = (DWORD)sz_vol / 0x1000;                                  /* 以 4KB 为单位的卷大小 */
                    for (i = 0, pau = 1; cst[i] && cst[i] <= n; i++, pau <<= 1); /* 从表中获取 */
                }
                n_clst = (DWORD)sz_vol / pau;
                if (n_clst > MAX_FAT12)
                {
                    n = n_clst * 2 + 4; /* FAT 大小 [字节] */
                }
                else
                {
                    fsty = FS_FAT12;
                    n = (n_clst * 3 + 1) / 2 + 3;     /* FAT 大小 [字节] */
                }
                sz_fat = (n + ss - 1) / ss;           /* FAT 大小 [扇区] */
                sz_rsv = 1;                           /* 保留扇区数 */
                sz_dir = (DWORD)n_root * SZDIRE / ss; /* 根目录大小 [扇区] */
            }
            b_fat = b_vol + sz_rsv;                   /* FAT 基 */
            b_data = b_fat + sz_fat * n_fat + sz_dir; /* 数据基 */

            /* 将数据区对齐到擦除块边界（用于闪存介质） */
            n = (DWORD)(((b_data + sz_blk - 1) & ~(sz_blk - 1)) - b_data); /* 从当前数据基到下一个最近的扇区数 */
            if (fsty == FS_FAT32)
            {                                                              /* FAT32：移动 FAT */
                sz_rsv += n;
                b_fat += n;
            }
            else
            {     /* FAT：扩展 FAT */
                if (n % n_fat)
                { /* 如果需要，调整分数误差 */
                    n--;
                    sz_rsv++;
                    b_fat++;
                }
                sz_fat += n / n_fat;
            }

            /* 确定簇数并最终检查 FAT 子类型的有效性 */
            if (sz_vol < b_data + pau * 16 - b_vol)
                LEAVE_MKFS(FR_MKFS_ABORTED); /* 卷太小？ */
            n_clst = ((DWORD)sz_vol - sz_rsv - sz_fat * n_fat - sz_dir) / pau;
            if (fsty == FS_FAT32)
            {
                if (n_clst <= MAX_FAT16)
                {                 /* FAT32 的簇太少？ */
                    if (sz_au == 0 && (sz_au = pau / 2) != 0)
                        continue; /* 调整簇大小并重试 */
                    LEAVE_MKFS(FR_MKFS_ABORTED);
                }
            }
            if (fsty == FS_FAT16)
            {
                if (n_clst > MAX_FAT16)
                { /* FAT16 的簇太多 */
                    if (sz_au == 0 && (pau * 2) <= 64)
                    {
                        sz_au = pau * 2;
                        continue; /* 调整簇大小并重试 */
                    }
                    if ((fsopt & FM_FAT32))
                    {
                        fsty = FS_FAT32;
                        continue; /* 切换到 FAT32 并重试 */
                    }
                    if (sz_au == 0 && (sz_au = pau * 2) <= 128)
                        continue; /* 调整簇大小并重试 */
                    LEAVE_MKFS(FR_MKFS_ABORTED);
                }
                if (n_clst <= MAX_FAT12)
                {                 /* FAT16 的簇太少 */
                    if (sz_au == 0 && (sz_au = pau * 2) <= 128)
                        continue; /* 调整簇大小并重试 */
                    LEAVE_MKFS(FR_MKFS_ABORTED);
                }
            }
            if (fsty == FS_FAT12 && n_clst > MAX_FAT12)
                LEAVE_MKFS(FR_MKFS_ABORTED); /* FAT12 的簇太多 */

            /* 好的，这是有效的簇配置 */
            break;
        } while (1);

    #if FF_USE_TRIM
        lba[0] = b_vol;
        lba[1] = b_vol + sz_vol - 1; /* 通知存储设备卷区可能被擦除 */
        disk_ioctl(pdrv, CTRL_TRIM, lba);
    #endif
        /* 创建 FAT VBR */
        memset(buf, 0, ss);
        memcpy(buf + BS_JmpBoot, "\xEB\xFE\x90"
                                 "MSDOS5.0",
               11);                                                             /* 启动跳转代码 (x86), OEM 名称 */
        st_word(buf + BPB_BytsPerSec, ss);                                      /* 扇区大小 [字节] */
        buf[BPB_SecPerClus] = (BYTE)pau;                                        /* 簇大小 [扇区] */
        st_word(buf + BPB_RsvdSecCnt, (WORD)sz_rsv);                            /* 保留区大小 */
        buf[BPB_NumFATs] = (BYTE)n_fat;                                         /* FAT 数 */
        st_word(buf + BPB_RootEntCnt, (WORD)((fsty == FS_FAT32) ? 0 : n_root)); /* 根目录条目数 */
        if (sz_vol < 0x10000)
        {
            st_word(buf + BPB_TotSec16, (WORD)sz_vol); /* 16 位 LBA 卷大小 */
        }
        else
        {
            st_dword(buf + BPB_TotSec32, (DWORD)sz_vol); /* 32 位 LBA 卷大小 */
        }
        buf[BPB_Media] = 0xF8;                           /* 介质描述字节 */
        st_word(buf + BPB_SecPerTrk, 63);                /* 每磁道扇区数（用于 int13） */
        st_word(buf + BPB_NumHeads, 255);                /* 磁头数（用于 int13） */
        st_dword(buf + BPB_HiddSec, (DWORD)b_vol);       /* 物理驱动器中的卷偏移 [扇区] */
        if (fsty == FS_FAT32)
        {
            st_dword(buf + BS_VolID32, vsn);     /* VSN */
            st_dword(buf + BPB_FATSz32, sz_fat); /* FAT 大小 [扇区] */
            st_dword(buf + BPB_RootClus32, 2);   /* 根目录簇# (2) */
            st_word(buf + BPB_FSInfo32, 1);      /* FSINFO 扇区偏移 (VBR + 1) */
            st_word(buf + BPB_BkBootSec32, 6);   /* 备份 VBR 偏移 (VBR + 6) */
            buf[BS_DrvNum32] = 0x80;             /* 驱动器号（用于 int13） */
            buf[BS_BootSig32] = 0x29;            /* 扩展引导签名 */
            memcpy(buf + BS_VolLab32, "NO NAME    "
                                      "FAT32   ",
                   19); /* 卷标, FAT 签名 */
        }
        else
        {
            st_dword(buf + BS_VolID, vsn);            /* VSN */
            st_word(buf + BPB_FATSz16, (WORD)sz_fat); /* FAT 大小 [扇区] */
            buf[BS_DrvNum] = 0x80;                    /* 驱动器号（用于 int13） */
            buf[BS_BootSig] = 0x29;                   /* 扩展引导签名 */
            memcpy(buf + BS_VolLab, "NO NAME    "
                                    "FAT     ",
                   19);                 /* 卷标, FAT 签名 */
        }
        st_word(buf + BS_55AA, 0xAA55); /* 签名（无论扇区大小，固定偏移） */
        if (disk_write(pdrv, buf, b_vol, 1) != RES_OK)
            LEAVE_MKFS(FR_DISK_ERR);    /* 写入 VBR 扇区 */

        /* 如果需要，创建 FSINFO 记录 */
        if (fsty == FS_FAT32)
        {
            disk_write(pdrv, buf, b_vol + 6, 1); /* 写入备份 VBR (VBR + 6) */
            memset(buf, 0, ss);
            st_dword(buf + FSI_LeadSig, 0x41615252);
            st_dword(buf + FSI_StrucSig, 0x61417272);
            st_dword(buf + FSI_Free_Count, n_clst - 1); /* 空闲簇数 */
            st_dword(buf + FSI_Nxt_Free, 2);            /* 最后分配的簇# */
            st_word(buf + BS_55AA, 0xAA55);
            disk_write(pdrv, buf, b_vol + 7, 1);        /* 写入备份 FSINFO (VBR + 7) */
            disk_write(pdrv, buf, b_vol + 1, 1);        /* 写入原始 FSINFO (VBR + 1) */
        }

        /* 初始化 FAT 区 */
        memset(buf, 0, sz_buf * ss);
        sect = b_fat; /* FAT 起始扇区 */
        for (i = 0; i < n_fat; i++)
        {             /* 初始化每个 FAT */
            if (fsty == FS_FAT32)
            {
                st_dword(buf + 0, 0xFFFFFFF8); /* FAT[0] */
                st_dword(buf + 4, 0xFFFFFFFF); /* FAT[1] */
                st_dword(buf + 8, 0x0FFFFFFF); /* FAT[2] (根目录) */
            }
            else
            {
                st_dword(buf + 0, (fsty == FS_FAT12) ? 0xFFFFF8 : 0xFFFFFFF8); /* FAT[0] 和 FAT[1] */
            }
            nsect = sz_fat;                                                    /* FAT 扇区数 */
            do
            {                                                                  /* 填充 FAT 扇区 */
                n = (nsect > sz_buf) ? sz_buf : nsect;
                if (disk_write(pdrv, buf, sect, (UINT)n) != RES_OK)
                    LEAVE_MKFS(FR_DISK_ERR);
                memset(buf, 0, ss); /* 其余 FAT 全部清除 */
                sect += n;
                nsect -= n;
            } while (nsect);
        }

        /* 初始化根目录（用零填充） */
        nsect = (fsty == FS_FAT32) ? pau : sz_dir; /* 根目录扇区数 */
        do
        {
            n = (nsect > sz_buf) ? sz_buf : nsect;
            if (disk_write(pdrv, buf, sect, (UINT)n) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);
            sect += n;
            nsect -= n;
        } while (nsect);
    }

    /* 此处已创建 FAT 卷 */

    /* 确定 MBR 分区表中的系统 ID */
    if (FF_FS_EXFAT && fsty == FS_EXFAT)
    {
        sys = 0x07; /* exFAT */
    }
    else
    {
        if (fsty == FS_FAT32)
        {
            sys = 0x0C; /* FAT32X */
        }
        else
        {
            if (sz_vol >= 0x10000)
            {
                sys = 0x06; /* FAT12/16 (large) */
            }
            else
            {
                sys = (fsty == FS_FAT16) ? 0x04 : 0x01; /* FAT16 : FAT12 */
            }
        }
    }

    /* 更新分区信息 */
    if (FF_MULTI_PARTITION && ipart != 0)
    { /* 卷位于现有分区中 */
        if (!FF_LBA64 || !(fsopt & 0x80))
        {
            /* 更新分区表中的系统 ID */
            if (disk_read(pdrv, buf, 0, 1) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);                              /* 读取 MBR */
            buf[MBR_Table + (ipart - 1) * SZ_PTE + PTE_System] = sys; /* 设置系统 ID */
            if (disk_write(pdrv, buf, 0, 1) != RES_OK)
                LEAVE_MKFS(FR_DISK_ERR);                              /* 写回 MBR */
        }
    }
    else
    {     /* 卷作为新单个分区 */
        if (!(fsopt & FM_SFD))
        { /* 如果不是 SFD，创建分区表 */
            lba[0] = sz_vol;
            lba[1] = 0;
            fr = create_partition(pdrv, lba, sys, buf);
            if (fr != FR_OK)
                LEAVE_MKFS(fr);
        }
    }

    if (disk_ioctl(pdrv, CTRL_SYNC, 0) != RES_OK)
        LEAVE_MKFS(FR_DISK_ERR);

    LEAVE_MKFS(FR_OK);
}

    #if FF_MULTI_PARTITION
/*-----------------------------------------------------------------------*/
/* 在物理驱动器上创建分区表                                                */
/*-----------------------------------------------------------------------*/
FRESULT f_fdisk(BYTE pdrv, const LBA_t ptbl[], void *work)
{
    BYTE *buf = (BYTE *)work;
    DSTATUS stat;

    stat = disk_initialize(pdrv);
    if (stat & STA_NOINIT)
        return FR_NOT_READY;
    if (stat & STA_PROTECT)
        return FR_WRITE_PROTECTED;
        #if FF_USE_LFN == 3
    if (!buf)
        buf = ff_memalloc(FF_MAX_SS); /* 使用堆内存作为工作缓冲区 */
        #endif
    if (!buf)
        return FR_NOT_ENOUGH_CORE;

    LEAVE_MKFS(create_partition(pdrv, ptbl, 0x07, buf));
}
    #endif /* FF_MULTI_PARTITION */
#endif     /* !FF_FS_READONLY && FF_USE_MKFS */

#if FF_USE_STRFUNC
    #if FF_USE_LFN && FF_LFN_UNICODE && (FF_STRF_ENCODE < 0 || FF_STRF_ENCODE > 3)
        #error 错误的 FF_STRF_ENCODE 设置
    #endif
/*-----------------------------------------------------------------------*/
/* 从文件获取字符串                                                       */
/*-----------------------------------------------------------------------*/
TCHAR *f_gets(TCHAR *buff, int len, FIL *fp)
{
    int nc = 0;
    TCHAR *p = buff;
    BYTE s[4];
    UINT rc;
    DWORD dc;
    #if FF_USE_LFN && FF_LFN_UNICODE && FF_STRF_ENCODE <= 2
    WCHAR wc;
    #endif
    #if FF_USE_LFN && FF_LFN_UNICODE && FF_STRF_ENCODE == 3
    UINT ct;
    #endif

    #if FF_USE_LFN && FF_LFN_UNICODE /* 带代码转换（Unicode API） */
    /* 为字符和终止符留出空间 */
    if (FF_LFN_UNICODE == 1)
        len -= (FF_STRF_ENCODE == 0) ? 1 : 2;
    if (FF_LFN_UNICODE == 2)
        len -= (FF_STRF_ENCODE == 0) ? 3 : 4;
    if (FF_LFN_UNICODE == 3)
        len -= 1;
    while (nc < len)
    {
        #if FF_STRF_ENCODE == 0 /* 以 ANSI/OEM 读取一个字符 */
        f_read(fp, s, 1, &rc);  /* 获取一个编码单元 */
        if (rc != 1)
            break;              /* EOF？ */
        wc = s[0];
        if (dbc_1st((BYTE)wc))
        {                          /* DBCS 首字节？ */
            f_read(fp, s, 1, &rc); /* 获取次字节 */
            if (rc != 1 || !dbc_2nd(s[0]))
                continue;          /* 错误代码？ */
            wc = wc << 8 | s[0];
        }
        dc = ff_oem2uni(wc, CODEPAGE);                   /* 将 ANSI/OEM 转换为 Unicode */
        if (dc == 0)
            continue;                                    /* 转换错误？ */
        #elif FF_STRF_ENCODE == 1 || FF_STRF_ENCODE == 2 /* 以 UTF-16LE/BE 读取一个字符 */
        f_read(fp, s, 2, &rc); /* 获取一个编码单元 */
        if (rc != 2)
            break;             /* EOF？ */
        dc = (FF_STRF_ENCODE == 1) ? ld_word(s) : s[0] << 8 | s[1];
        if (IsSurrogateL(dc))
            continue;              /* 代理项对损坏？ */
        if (IsSurrogateH(dc))
        {                          /* 高代理项？ */
            f_read(fp, s, 2, &rc); /* 获取低代理项 */
            if (rc != 2)
                break;             /* EOF？ */
            wc = (FF_STRF_ENCODE == 1) ? ld_word(s) : s[0] << 8 | s[1];
            if (!IsSurrogateL(wc))
                continue;                                    /* 代理项对损坏？ */
            dc = ((dc & 0x3FF) + 0x40) << 10 | (wc & 0x3FF); /* 合并代理项对 */
        }
        #else                                            /* 以 UTF-8 读取一个字符 */
        f_read(fp, s, 1, &rc); /* 获取一个编码单元 */
        if (rc != 1)
            break;             /* EOF？ */
        dc = s[0];
        if (dc >= 0x80)
        { /* 多字节序列？ */
            ct = 0;
            if ((dc & 0xE0) == 0xC0)
            {
                dc &= 0x1F;
                ct = 1;
            } /* 2字节序列？ */
            if ((dc & 0xF0) == 0xE0)
            {
                dc &= 0x0F;
                ct = 2;
            } /* 3字节序列？ */
            if ((dc & 0xF8) == 0xF0)
            {
                dc &= 0x07;
                ct = 3;
            } /* 4字节序列？ */
            if (ct == 0)
                continue;
            f_read(fp, s, ct, &rc); /* 获取后续字节 */
            if (rc != ct)
                break;
            rc = 0;
            do
            { /* 合并字节序列 */
                if ((s[rc] & 0xC0) != 0x80)
                    break;
                dc = dc << 6 | (s[rc] & 0x3F);
            } while (++rc < ct);
            if (rc != ct || dc < 0x80 || IsSurrogate(dc) || dc >= 0x110000)
                continue; /* 错误编码？ */
        }
        #endif
        /* 此时 dc 包含要输出的代码点 */

        if (FF_USE_STRFUNC == 2 && dc == '\r')
            continue;                                  /* 如果需要，去掉 \r */
        #if FF_LFN_UNICODE == 1 || FF_LFN_UNICODE == 3 /* 以 UTF-16/32 编码输出 */
        if (FF_LFN_UNICODE == 1 && dc >= 0x10000)
        {                                              /* 在 UTF-16 中超出 BMP？ */
            *p++ = (TCHAR)(0xD800 | ((dc >> 10) - 0x40));
            nc++;                                      /* 生成并输出高代理项 */
            dc = 0xDC00 | (dc & 0x3FF);                /* 生成低代理项 */
        }
        *p++ = (TCHAR)dc;
        nc++;
        if (dc == '\n')
            break;                /* 行结束？ */
        #elif FF_LFN_UNICODE == 2 /* 以 UTF-8 编码输出 */
        if (dc < 0x80)
        { /* 单字节？ */
            *p++ = (TCHAR)dc;
            nc++;
            if (dc == '\n')
                break; /* 行结束？ */
        }
        else
        {
            if (dc < 0x800)
            { /* 2字节序列？ */
                *p++ = (TCHAR)(0xC0 | (dc >> 6 & 0x1F));
                *p++ = (TCHAR)(0x80 | (dc >> 0 & 0x3F));
                nc += 2;
            }
            else
            {
                if (dc < 0x10000)
                { /* 3字节序列？ */
                    *p++ = (TCHAR)(0xE0 | (dc >> 12 & 0x0F));
                    *p++ = (TCHAR)(0x80 | (dc >> 6 & 0x3F));
                    *p++ = (TCHAR)(0x80 | (dc >> 0 & 0x3F));
                    nc += 3;
                }
                else
                { /* 4字节序列？ */
                    *p++ = (TCHAR)(0xF0 | (dc >> 18 & 0x07));
                    *p++ = (TCHAR)(0x80 | (dc >> 12 & 0x3F));
                    *p++ = (TCHAR)(0x80 | (dc >> 6 & 0x3F));
                    *p++ = (TCHAR)(0x80 | (dc >> 0 & 0x3F));
                    nc += 4;
                }
            }
        }
        #endif
    }

    #else /* 逐字节读取，无转换（ANSI/OEM API） */
    len -= 1; /* 为终止符留出空间 */
    while (nc < len)
    {
        f_read(fp, s, 1, &rc); /* 获取一个字节 */
        if (rc != 1)
            break;             /* EOF？ */
        dc = s[0];
        if (FF_USE_STRFUNC == 2 && dc == '\r')
            continue;
        *p++ = (TCHAR)dc;
        nc++;
        if (dc == '\n')
            break;
    }
    #endif

    *p = 0;               /* 终止字符串 */
    return nc ? buff : 0; /* 由于 EOF 或错误未读取数据时，返回错误 */
}

    #if !FF_FS_READONLY
        #include <stdarg.h>
        #define SZ_PUTC_BUF 64
        #define SZ_NUM_BUF 32

/*-----------------------------------------------------------------------*/
/* 向文件输出一个字符（带子函数）                                          */
/*-----------------------------------------------------------------------*/

/* 输出缓冲区和工作区 */
typedef struct
{
    FIL *fp;       /* 指向写入文件的指针 */
    int idx, nchr; /* buf[] 的写入索引（-1：错误），写入的编码单元数 */
        #if FF_USE_LFN && FF_LFN_UNICODE == 1
    WCHAR hs;
        #elif FF_USE_LFN && FF_LFN_UNICODE == 2
    BYTE bs[4];
    UINT wi, ct;
        #endif
    BYTE buf[SZ_PUTC_BUF]; /* 写入缓冲区 */
} putbuff;

/* 带代码转换的缓冲文件写入 */
static void putc_bfd(putbuff *pb, TCHAR c)
{
    UINT n;
    int i, nc;
        #if FF_USE_LFN && FF_LFN_UNICODE
    WCHAR hs, wc;
            #if FF_LFN_UNICODE == 2
    DWORD dc;
    const TCHAR *tp;
            #endif
        #endif

    if (FF_USE_STRFUNC == 2 && c == '\n')
    { /* LF -> CRLF 转换 */
        putc_bfd(pb, '\r');
    }

    i = pb->idx;   /* pb->buf[] 的写入索引 */
    if (i < 0)
        return;    /* 写入错误？ */
    nc = pb->nchr; /* 写入单元计数器 */

        #if FF_USE_LFN && FF_LFN_UNICODE
            #if FF_LFN_UNICODE == 1 /* UTF-16 输入 */
    if (IsSurrogateH(c))
    {                               /* 高代理项？ */
        pb->hs = c;
        return;                     /* 保存供下次使用 */
    }
    hs = pb->hs;
    pb->hs = 0;
    if (hs != 0)
    {               /* 存在前导高代理项 */
        if (!IsSurrogateL(c))
            hs = 0; /* 如果不是代理项对，丢弃高代理项 */
    }
    else
    {
        if (IsSurrogateL(c))
            return; /* 丢弃孤立的低代理项 */
    }
    wc = c;
            #elif FF_LFN_UNICODE == 2 /* UTF-8 输入 */
    for (;;)
    {
        if (pb->ct == 0)
        {                                 /* 多字节序列外？ */
            pb->bs[pb->wi = 0] = (BYTE)c; /* 保存首字节 */
            if ((BYTE)c < 0x80)
                break;                    /* 单字节？ */
            if (((BYTE)c & 0xE0) == 0xC0)
                pb->ct = 1;               /* 2字节序列？ */
            if (((BYTE)c & 0xF0) == 0xE0)
                pb->ct = 2;               /* 3字节序列？ */
            if (((BYTE)c & 0xF1) == 0xF0)
                pb->ct = 3;               /* 4字节序列？ */
            return;
        }
        else
        {     /* 在多字节序列中 */
            if (((BYTE)c & 0xC0) != 0x80)
            { /* 序列损坏？ */
                pb->ct = 0;
                continue;
            }
            pb->bs[++pb->wi] = (BYTE)c; /* 保存后续字节 */
            if (--pb->ct == 0)
                break;                  /* 多字节序列结束？ */
            return;
        }
    }
    tp = (const TCHAR *)pb->bs;
    dc = tchar2uni(&tp); /* UTF-8 ==> UTF-16 */
    if (dc == 0xFFFFFFFF)
        return;          /* 错误代码？ */
    wc = (WCHAR)dc;
    hs = (WCHAR)(dc >> 16);
            #elif FF_LFN_UNICODE == 3 /* UTF-32 输入 */
    if (IsSurrogate(c) || c >= 0x110000)
        return;                                    /* 丢弃无效代码 */
    if (c >= 0x10000)
    {                                              /* 超出 BMP？ */
        hs = (WCHAR)(0xD800 | ((c >> 10) - 0x40)); /* 生成高代理项 */
        wc = 0xDC00 | (c & 0x3FF);                 /* 生成低代理项 */
    }
    else
    {
        hs = 0;
        wc = (WCHAR)c;
    }
            #endif
            /* 此时 UTF-16 格式的代码点存储在 hs 和 wc 中 */

            #if FF_STRF_ENCODE == 1 /* 以 UTF-16LE 写入代码点 */
    if (hs != 0)
    {                               /* 代理项对？ */
        st_word(&pb->buf[i], hs);
        i += 2;
        nc++;
    }
    st_word(&pb->buf[i], wc);
    i += 2;
            #elif FF_STRF_ENCODE == 2 /* 以 UTF-16BE 写入代码点 */
    if (hs != 0)
    { /* 代理项对？ */
        pb->buf[i++] = (BYTE)(hs >> 8);
        pb->buf[i++] = (BYTE)hs;
        nc++;
    }
    pb->buf[i++] = (BYTE)(wc >> 8);
    pb->buf[i++] = (BYTE)wc;
            #elif FF_STRF_ENCODE == 3 /* 以 UTF-8 写入代码点 */
    if (hs != 0)
    { /* 4字节序列？ */
        nc += 3;
        hs = (hs & 0x3FF) + 0x40;
        pb->buf[i++] = (BYTE)(0xF0 | hs >> 8);
        pb->buf[i++] = (BYTE)(0x80 | (hs >> 2 & 0x3F));
        pb->buf[i++] = (BYTE)(0x80 | (hs & 3) << 4 | (wc >> 6 & 0x0F));
        pb->buf[i++] = (BYTE)(0x80 | (wc & 0x3F));
    }
    else
    {
        if (wc < 0x80)
        { /* 单字节？ */
            pb->buf[i++] = (BYTE)wc;
        }
        else
        {
            if (wc < 0x800)
            { /* 2字节序列？ */
                nc += 1;
                pb->buf[i++] = (BYTE)(0xC0 | wc >> 6);
            }
            else
            { /* 3字节序列 */
                nc += 2;
                pb->buf[i++] = (BYTE)(0xE0 | wc >> 12);
                pb->buf[i++] = (BYTE)(0x80 | (wc >> 6 & 0x3F));
            }
            pb->buf[i++] = (BYTE)(0x80 | (wc & 0x3F));
        }
    }
            #else                     /* 以 ANSI/OEM 写入代码点 */
    if (hs != 0)
        return;
    wc = ff_uni2oem(wc, CODEPAGE); /* UTF-16 ==> ANSI/OEM */
    if (wc == 0)
        return;
    if (wc >= 0x100)
    {
        pb->buf[i++] = (BYTE)(wc >> 8);
        nc++;
    }
    pb->buf[i++] = (BYTE)wc;
            #endif

        #else /* ANSI/OEM 输入（无重新编码） */
    pb->buf[i++] = (BYTE)c;
        #endif

    if (i >= (int)(sizeof pb->buf) - 4)
    { /* 将缓冲的字符写入文件 */
        f_write(pb->fp, pb->buf, (UINT)i, &n);
        i = (n == (UINT)i) ? 0 : -1;
    }
    pb->idx = i;
    pb->nchr = nc + 1;
}

/* 刷新缓冲区中剩余的字符 */
static int putc_flush(putbuff *pb)
{
    UINT nw;

    if (pb->idx >= 0 /* 将缓冲的字符刷新到文件 */
        && f_write(pb->fp, pb->buf, (UINT)pb->idx, &nw) == FR_OK && (UINT)pb->idx == nw)
        return pb->nchr;
    return -1;
}

/* 初始化写入缓冲区 */
static void putc_init(putbuff *pb, FIL *fp)
{
    memset(pb, 0, sizeof(putbuff));
    pb->fp = fp;
}

int f_putc(TCHAR c, FIL *fp)
{
    putbuff pb;

    putc_init(&pb, fp);
    putc_bfd(&pb, c); /* 输出字符 */
    return putc_flush(&pb);
}

/*-----------------------------------------------------------------------*/
/* 向文件输出字符串                                                       */
/*-----------------------------------------------------------------------*/
int f_puts(const TCHAR *str, FIL *fp)
{
    putbuff pb;

    putc_init(&pb, fp);
    while (*str) putc_bfd(&pb, *str++); /* 输出字符串 */
    return putc_flush(&pb);
}

        /*-----------------------------------------------------------------------*/
        /* 向文件输出格式化字符串（带子函数）                                      */
        /*-----------------------------------------------------------------------*/
        #if FF_PRINT_FLOAT && FF_INTDEF == 2
            #include <math.h>

static int ilog10(double n) /* 以整数输出计算 log10(n) */
{
    int rv = 0;

    while (n >= 10)
    { /* 右移降位 */
        if (n >= 100000)
        {
            n /= 100000;
            rv += 5;
        }
        else
        {
            n /= 10;
            rv++;
        }
    }
    while (n < 1)
    { /* 左移升位 */
        if (n < 0.00001)
        {
            n *= 100000;
            rv -= 5;
        }
        else
        {
            n *= 10;
            rv--;
        }
    }
    return rv;
}

static double i10x(int n) /* 计算 10^n，输入为整数 */
{
    double rv = 1;

    while (n > 0)
    { /* 左移 */
        if (n >= 5)
        {
            rv *= 100000;
            n -= 5;
        }
        else
        {
            rv *= 10;
            n--;
        }
    }
    while (n < 0)
    { /* 右移 */
        if (n <= -5)
        {
            rv /= 100000;
            n += 5;
        }
        else
        {
            rv /= 10;
            n++;
        }
    }
    return rv;
}

static void ftoa(char *buf, double val, int prec, TCHAR fmt)
{
    int d;
    int e = 0, m = 0;
    char sign = 0;
    double w;
    const char *er = 0;
    const char ds = FF_PRINT_FLOAT == 2 ? ',' : '.';

    if (isnan(val))
    { /* 不是数字？ */
        er = "NaN";
    }
    else
    {
        if (prec < 0)
            prec = 6; /* 默认精度？ (6 位小数) */
        if (val < 0)
        {             /* 负数？ */
            val = 0 - val;
            sign = '-';
        }
        else
        {
            sign = '+';
        }
        if (isinf(val))
        { /* 无穷？ */
            er = "INF";
        }
        else
        {
            if (fmt == 'f')
            {                              /* 十进制记数法？ */
                val += i10x(0 - prec) / 2; /* 四舍五入 (最接近) */
                m = ilog10(val);
                if (m < 0)
                    m = 0;
                if (m + prec + 3 >= SZ_NUM_BUF)
                    er = "OV"; /* 缓冲区溢出？ */
            }
            else
            {                                            /* E 记数法 */
                if (val != 0)
                {                                        /* 不为真零？ */
                    val += i10x(ilog10(val) - prec) / 2; /* 四舍五入 (最接近) */
                    e = ilog10(val);
                    if (e > 99 || prec + 7 >= SZ_NUM_BUF)
                    { /* 缓冲区溢出或 E > +99？ */
                        er = "OV";
                    }
                    else
                    {
                        if (e < -99)
                            e = -99;
                        val /= i10x(e); /* 归一化 */
                    }
                }
            }
        }
    }
    if (!er)
    {                        /* 无错误条件 */
        if (sign == '-')
            *buf++ = sign;   /* 如果为负值，添加 - */
        do
        {                    /* 输出十进制数 */
            if (m == -1)
                *buf++ = ds; /* 进入小数部分时插入小数点 */
            w = i10x(m);     /* 切出最高位 d */
            d = (int)(val / w);
            val -= d * w;
            *buf++ = (char)('0' + d); /* 输出数字 */
        } while (--m >= -prec); /* 输出由 prec 指定的所有数字 */
        if (fmt != 'f')
        { /* 如果需要，添加指数 */
            *buf++ = (char)fmt;
            if (e < 0)
            {
                e = 0 - e;
                *buf++ = '-';
            }
            else
            {
                *buf++ = '+';
            }
            *buf++ = (char)('0' + e / 10);
            *buf++ = (char)('0' + e % 10);
        }
    }
    if (er)
    {                      /* 错误条件 */
        if (sign)
            *buf++ = sign; /* 如果需要，添加符号 */
        do *buf++ = *er++;
        while (*er); /* 输出错误符号 */
    }
    *buf = 0;  /* 终止 */
}
        #endif /* FF_PRINT_FLOAT && FF_INTDEF == 2 */

int f_printf(FIL *fp, const TCHAR *fmt, ...)
{
    va_list arp;
    putbuff pb;
    UINT i, j, w, f, r;
    int prec;
        #if FF_PRINT_LLI && FF_INTDEF == 2
    QWORD v;
        #else
    DWORD v;
        #endif
    TCHAR tc, pad, *tp;
    TCHAR nul = 0;
    char d, str[SZ_NUM_BUF];

    putc_init(&pb, fp);

    va_start(arp, fmt);

    for (;;)
    {
        tc = *fmt++;
        if (tc == 0)
            break; /* 格式字符串结束 */
        if (tc != '%')
        {          /* 不是转义字符（直接通过） */
            putc_bfd(&pb, tc);
            continue;
        }
        f = w = 0;
        pad = ' ';
        prec = -1; /* 初始化参数 */
        tc = *fmt++;
        if (tc == '0')
        { /* 标志：'0' 填充 */
            pad = '0';
            tc = *fmt++;
        }
        else if (tc == '-')
        { /* 标志：左对齐 */
            f = 2;
            tc = *fmt++;
        }
        if (tc == '*')
        { /* 最小宽度来自参数 */
            w = va_arg(arp, int);
            tc = *fmt++;
        }
        else
        {
            while (IsDigit(tc))
            { /* 最小宽度 */
                w = w * 10 + tc - '0';
                tc = *fmt++;
            }
        }
        if (tc == '.')
        { /* 精度 */
            tc = *fmt++;
            if (tc == '*')
            { /* 精度来自参数 */
                prec = va_arg(arp, int);
                tc = *fmt++;
            }
            else
            {
                prec = 0;
                while (IsDigit(tc))
                { /* 精度 */
                    prec = prec * 10 + tc - '0';
                    tc = *fmt++;
                }
            }
        }
        if (tc == 'l')
        { /* 大小：长整型 */
            f |= 4;
            tc = *fmt++;
        #if FF_PRINT_LLI && FF_INTDEF == 2
            if (tc == 'l')
            { /* 大小：长长整型 */
                f |= 8;
                tc = *fmt++;
            }
        #endif
        }
        if (tc == 0)
            break;    /* 格式字符串结束 */
        switch (tc)
        {             /* 参数类型为... */
            case 'b': /* 无符号二进制 */
                r = 2;
                break;
            case 'o': /* 无符号八进制 */
                r = 8;
                break;
            case 'd': /* 有符号十进制 */
            case 'u': /* 无符号十进制 */
                r = 10;
                break;
            case 'x': /* 无符号十六进制（小写） */
            case 'X': /* 无符号十六进制（大写） */
                r = 16;
                break;
            case 'c': /* 字符 */
                putc_bfd(&pb, (TCHAR)va_arg(arp, int));
                continue;
            case 's':                                              /* 字符串 */
                tp = va_arg(arp, TCHAR *);                         /* 获取指针参数 */
                if (!tp)
                    tp = &nul;                                     /* 空指针生成空字符串 */
                for (j = 0; tp[j]; j++);                           /* j = tcslen(tp) */
                if (prec >= 0 && j > (UINT)prec)
                    j = prec;                                      /* 限制字符串主体长度 */
                for (; !(f & 2) && j < w; j++) putc_bfd(&pb, pad); /* 左填充 */
                while (*tp && prec--) putc_bfd(&pb, *tp++);        /* 主体 */
                while (j++ < w) putc_bfd(&pb, ' ');                /* 右填充 */
                continue;
        #if FF_PRINT_FLOAT && FF_INTDEF == 2
            case 'f':                                                             /* 浮点数（十进制） */
            case 'e':                                                             /* 浮点数（e） */
            case 'E':                                                             /* 浮点数（E） */
                ftoa(str, va_arg(arp, double), prec, tc);                         /* 生成浮点数字符串 */
                for (j = strlen(str); !(f & 2) && j < w; j++) putc_bfd(&pb, pad); /* 左填充 */
                for (i = 0; str[i]; putc_bfd(&pb, str[i++]));                     /* 主体 */
                while (j++ < w) putc_bfd(&pb, ' ');                               /* 右填充 */
                continue;
        #endif
            default: /* 未知类型（直接通过） */
                putc_bfd(&pb, tc);
                continue;
        }

                /* 获取一个整数参数并将其转换为数字形式 */
        #if FF_PRINT_LLI && FF_INTDEF == 2
        if (f & 8)
        { /* long long 参数？ */
            v = (QWORD)va_arg(arp, LONGLONG);
        }
        else
        {
            if (f & 4)
            { /* long 参数？ */
                v = (tc == 'd') ? (QWORD)(LONGLONG)va_arg(arp, long) : (QWORD)va_arg(arp, unsigned long);
            }
            else
            { /* int/short/char 参数 */
                v = (tc == 'd') ? (QWORD)(LONGLONG)va_arg(arp, int) : (QWORD)va_arg(arp, unsigned int);
            }
        }
        if (tc == 'd' && (v & 0x8000000000000000))
        { /* 负值？ */
            v = 0 - v;
            f |= 1;
        }
        #else
        if (f & 4)
        { /* long 参数？ */
            v = (DWORD)va_arg(arp, long);
        }
        else
        { /* int/short/char 参数 */
            v = (tc == 'd') ? (DWORD)(long)va_arg(arp, int) : (DWORD)va_arg(arp, unsigned int);
        }
        if (tc == 'd' && (v & 0x80000000))
        { /* 负值？ */
            v = 0 - v;
            f |= 1;
        }
        #endif
        i = 0;
        do
        { /* 生成整数字符串 */
            d = (char)(v % r);
            v /= r;
            if (d > 9)
                d += (tc == 'x') ? 0x27 : 0x07;
            str[i++] = d + '0';
        } while (v && i < SZ_NUM_BUF);
        if (f & 1)
            str[i++] = '-'; /* 符号 */
        /* 写入 */
        for (j = i; !(f & 2) && j < w; j++) putc_bfd(&pb, pad); /* 左填充 */
        do putc_bfd(&pb, (TCHAR)str[--i]);
        while (i); /* 主体 */
        while (j++ < w) putc_bfd(&pb, ' '); /* 右填充 */
    }

    va_end(arp);

    return putc_flush(&pb);
}
    #endif /* !FF_FS_READONLY */
#endif     /* FF_USE_STRFUNC */

#if FF_CODE_PAGE == 0
/*-----------------------------------------------------------------------*/
/* 为路径名设置活动代码页                                                 */
/*-----------------------------------------------------------------------*/
FRESULT f_setcp(WORD cp)
{
    static const WORD validcp[22] = {437, 720, 737, 771, 775, 850, 852, 855, 857, 860, 861, 862, 863, 864, 865, 866, 869, 932, 936, 949, 950, 0};
    static const BYTE *const tables[22] = {Ct437, Ct720, Ct737, Ct771, Ct775, Ct850, Ct852, Ct855, Ct857, Ct860, Ct861, Ct862, Ct863, Ct864, Ct865, Ct866, Ct869, Dc932, Dc936, Dc949, Dc950, 0};
    UINT i;

    for (i = 0; validcp[i] != 0 && validcp[i] != cp; i++); /* 查找代码页 */
    if (validcp[i] != cp)
        return FR_INVALID_PARAMETER;                       /* 未找到？ */

    CodePage = cp;
    if (cp >= 900)
    { /* DBCS */
        ExCvt = 0;
        DbcTbl = tables[i];
    }
    else
    { /* SBCS */
        ExCvt = tables[i];
        DbcTbl = 0;
    }
    return FR_OK;
}
#endif /* FF_CODE_PAGE == 0 */