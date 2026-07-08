/*----------------------------------------------------------------------------/
/  FatFs - 通用 FAT 文件系统模块  R0.14b                                      /
/-----------------------------------------------------------------------------/
/
/ 版权所有 (C) 2021, ChaN, 保留所有权利。
/
/ FatFs 模块是一个开源软件。在满足以下条件的前提下，允许以源代码和二进制形式
/ 对 FatFs 进行再分发和使用，无论是否经过修改：

/ 1. 再分发的源代码必须保留上述版权声明、
/    此条件和以下免责声明。
/
/ 本软件由版权持有人和贡献者按"原样"提供，
/ 并且与本软件相关的任何保证均被否认。
/ 版权所有者或贡献者对因使用本软件而造成的任何损害不承担责任。
/
/----------------------------------------------------------------------------*/


#ifndef FF_DEFINED
    #define FF_DEFINED 86631 /* 修订版本 ID */

    #ifdef __cplusplus
extern "C"
{
    #endif

    #include "ffconf.h" /* FatFs 配置文件，包含配置选项 */

    #if FF_DEFINED != FFCONF_DEF
        #error 配置文件错误 (ffconf.h).
    #endif


    /* FatFs API 使用的整数类型 */

    #if defined(_WIN32) /* Windows VC++ (仅用于开发) */
        #define FF_INTDEF 2
        #include <windows.h>
    typedef unsigned __int64 QWORD;
        #include <float.h>
        #define isnan(v) _isnan(v)
        #define isinf(v) (!_finite(v))

    #elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__cplusplus) /* C99 或更高版本 */
        #define FF_INTDEF 2
        #include <stdint.h>
typedef unsigned int UINT;  /* int 必须是 16 位或 32 位 */
typedef unsigned char BYTE; /* char 必须是 8 位 */
typedef uint16_t WORD;      /* 16 位无符号整数 */
typedef uint32_t DWORD;     /* 32 位无符号整数 */
typedef uint64_t QWORD;     /* 64 位无符号整数 */
typedef WORD WCHAR;         /* UTF-16 字符类型 */

    #else /* 早于 C99 的版本 */
        #define FF_INTDEF 1
typedef unsigned int UINT;   /* int 必须是 16 位或 32 位 */
typedef unsigned char BYTE;  /* char 必须是 8 位 */
typedef unsigned short WORD; /* 16 位无符号整数 */
typedef unsigned long DWORD; /* 32 位无符号整数 */
typedef WORD WCHAR;          /* UTF-16 字符类型 */
    #endif


    /* 文件大小和 LBA 变量的类型 */

    #if FF_FS_EXFAT
        #if FF_INTDEF != 2
            #error exFAT 特性需要 C99 或更高版本
        #endif
    typedef QWORD FSIZE_t; /* 文件大小类型 */
        #if FF_LBA64
    typedef QWORD LBA_t;   /* 逻辑块地址类型，64位 */
        #else
    typedef DWORD LBA_t; /* 逻辑块地址类型，32位 */
        #endif
    #else
        #if FF_LBA64
            #error 启用 64 位 LBA 时必须启用 exFAT
        #endif
typedef DWORD FSIZE_t; /* 文件大小类型 */
typedef DWORD LBA_t;   /* 逻辑块地址类型 */
    #endif


    /* FatFs API 上路径名字符串的类型 (TCHAR) */

    #if FF_USE_LFN && FF_LFN_UNICODE == 1   /* UTF-16 编码的 Unicode */
    typedef WCHAR TCHAR;                    /* TCHAR 定义为 WCHAR */
        #define _T(x) L##x                  /* 宽字符串字面量前缀 */
        #define _TEXT(x) L##x               /* 宽字符串字面量前缀 */
    #elif FF_USE_LFN && FF_LFN_UNICODE == 2 /* UTF-8 编码的 Unicode */
typedef char TCHAR; /* TCHAR 定义为 char */
        #define _T(x) u8##x                 /* UTF-8 字符串字面量前缀 */
        #define _TEXT(x) u8##x              /* UTF-8 字符串字面量前缀 */
    #elif FF_USE_LFN && FF_LFN_UNICODE == 3 /* UTF-32 编码的 Unicode */
typedef DWORD TCHAR; /* TCHAR 定义为 DWORD */
        #define _T(x) U##x                  /* UTF-32 字符串字面量前缀 */
        #define _TEXT(x) U##x               /* UTF-32 字符串字面量前缀 */
    #elif FF_USE_LFN && (FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3)
        #error FF_LFN_UNICODE 设置错误
    #else                  /* SBCS/DBCS 中的 ANSI/OEM 编码 */
typedef char TCHAR; /* TCHAR 定义为 char */
        #define _T(x) x    /* 普通字符串字面量，无前缀 */
        #define _TEXT(x) x /* 普通字符串字面量，无前缀 */
    #endif


    /* 卷管理定义 */

    #if FF_MULTI_PARTITION /* 多分区配置 */
    typedef struct
    {
        BYTE pd; /* 物理驱动器编号 */
        BYTE pt; /* 分区: 0:自动检测, 1-4:强制分区 */
    } PARTITION;

    extern PARTITION VolToPart[]; /* 卷到分区的映射表 */
    #endif

    #if FF_STR_VOLUME_ID
        #ifndef FF_VOLUME_STRS
    extern const char *VolumeStr[FF_VOLUMES]; /* 用户定义的卷标识符 */
        #endif
    #endif


    /* 文件系统对象结构体 (FATFS) */

    typedef struct
    {
        BYTE fs_type;   /* 文件系统类型 (0:未挂载) */
        BYTE pdrv;      /* 关联的物理驱动器 */
        BYTE n_fats;    /* FAT 表数量 (1 或 2) */
        BYTE wflag;     /* win[] 标志位 (b0:脏位，表示窗口数据被修改) */
        BYTE fsi_flag;  /* FSINFO 标志位 (b7:禁用, b0:脏位) */
        WORD id;        /* 卷挂载 ID */
        WORD n_rootdir; /* 根目录条目数 (FAT12/16) */
        WORD csize;     /* 簇大小 [扇区数] */
    #if FF_MAX_SS != FF_MIN_SS
        WORD ssize;     /* 扇区大小 (512, 1024, 2048 或 4096) */
    #endif
    #if FF_USE_LFN
        WCHAR *lfnbuf; /* 长文件名工作缓冲区 */
    #endif
    #if FF_FS_EXFAT
        BYTE *dirbuf; /* exFAT 目录条目块临时缓冲区 */
    #endif
    #if FF_FS_REENTRANT
        FF_SYNC_t sobj; /* 同步对象标识符 */
    #endif
    #if !FF_FS_READONLY
        DWORD last_clst; /* 上次分配的簇 */
        DWORD free_clst; /* 空闲簇数量 */
    #endif
    #if FF_FS_RPATH
        DWORD cdir;     /* 当前目录起始簇 (0:根目录) */
        #if FF_FS_EXFAT
        DWORD cdc_scl;  /* 包含目录的起始簇 (当 cdir 为 0 时无效) */
        DWORD cdc_size; /* b31-b8:包含目录的大小, b7-b0:链状态 */
        DWORD cdc_ofs;  /* 在包含目录中的偏移量 (当 cdir 为 0 时无效) */
        #endif
    #endif
        DWORD n_fatent;      /* FAT 条目数 (簇数 + 2) */
        DWORD fsize;         /* 一个 FAT 表的大小 [扇区数] */
        LBA_t volbase;       /* 卷基址扇区 */
        LBA_t fatbase;       /* FAT 基址扇区 */
        LBA_t dirbase;       /* 根目录基址扇区/簇 */
        LBA_t database;      /* 数据区基址扇区 */
    #if FF_FS_EXFAT
        LBA_t bitbase;       /* 分配位图基址扇区 */
    #endif
        LBA_t winsect;       /* 当前在 win[] 中显示的扇区 */
        BYTE win[FF_MAX_SS]; /* 磁盘访问窗口，用于目录、FAT（以及在小配置下的文件数据） */
    } FATFS;

    /* 对象 ID 和分配信息 (FFOBJID) */

    typedef struct
    {
        FATFS *fs;       /* 指向该对象所属卷的指针 */
        WORD id;         /* 所属卷的挂载 ID */
        BYTE attr;       /* 对象属性 */
        BYTE stat;       /* 对象链状态 (b1-0: =0:非连续, =2:连续, =3:本次会话中碎片化, b2:子目录被拉伸) */
        DWORD sclust;    /* 对象数据起始簇 (0:无簇或根目录) */
        FSIZE_t objsize; /* 对象大小 (当 sclust != 0 时有效) */
    #if FF_FS_EXFAT
        DWORD n_cont;    /* 第一段碎片的大小 - 1 (当 stat == 3 时有效) */
        DWORD n_frag;    /* 需要写入 FAT 的最后一段碎片的大小 (非零时有效) */
        DWORD c_scl;     /* 包含目录的起始簇 (当 sclust != 0 时有效) */
        DWORD c_size;    /* b31-b8:包含目录的大小, b7-b0: 链状态 (当 c_scl != 0 时有效) */
        DWORD c_ofs;     /* 在包含目录中的偏移量 (当文件对象且 sclust != 0 时有效) */
    #endif
    #if FF_FS_LOCK
        UINT lockid; /* 文件锁定 ID，从 1 开始 (文件信号量表 Files[] 的索引) */
    #endif
    } FFOBJID;

    /* 文件对象结构体 (FIL) */

    typedef struct
    {
        FFOBJID obj;    /* 对象标识符 (必须是第一个成员，用于检测无效的对象指针) */
        BYTE flag;      /* 文件状态标志 */
        BYTE err;       /* 中止标志 (错误代码) */
        FSIZE_t fptr;   /* 文件读写指针 (文件打开时清零) */
        DWORD clust;    /* fptr 的当前簇 (当 fptr 为 0 时无效) */
        LBA_t sect;     /* 在 buf[] 中显示的扇区号 (0:无效) */
    #if !FF_FS_READONLY
        LBA_t dir_sect; /* 包含目录项的扇区号 (exFAT 中未使用) */
        BYTE *dir_ptr;  /* 指向 win[] 中目录项的指针 (exFAT 中未使用) */
    #endif
    #if FF_USE_FASTSEEK
        DWORD *cltbl; /* 指向簇链接映射表的指针 (打开时置空，由应用程序设置) */
    #endif
    #if !FF_FS_TINY
        BYTE buf[FF_MAX_SS]; /* 文件私有数据读写窗口 */
    #endif
    } FIL;

    /* 目录对象结构体 (DIR) */

    typedef struct
    {
        FFOBJID obj;   /* 对象标识符 */
        DWORD dptr;    /* 当前读写偏移量 */
        DWORD clust;   /* 当前簇 */
        LBA_t sect;    /* 当前扇区 (0:读操作已终止) */
        BYTE *dir;     /* 指向 win[] 中目录项的指针 */
        BYTE fn[12];   /* 短文件名 (输入/输出) {主体[8],扩展名[3],状态[1]} */
    #if FF_USE_LFN
        DWORD blk_ofs; /* 正在处理的当前条目块的偏移量 (0xFFFFFFFF:无效) */
    #endif
    #if FF_USE_FIND
        const TCHAR *pat; /* 指向名称匹配模式的指针 */
    #endif
    } DIR;

    /* 文件信息结构体 (FILINFO) */

    typedef struct
    {
        FSIZE_t fsize;                 /* 文件大小 */
        WORD fdate;                    /* 修改日期 */
        WORD ftime;                    /* 修改时间 */
        BYTE fattrib;                  /* 文件属性 */
    #if FF_USE_LFN
        TCHAR altname[FF_SFN_BUF + 1]; /* 备用文件名 */
        TCHAR fname[FF_LFN_BUF + 1];   /* 主文件名 */
    #else
    TCHAR fname[12 + 1]; /* 文件名 */
    #endif
    } FILINFO;

    /* 格式化参数结构体 (MKFS_PARM) */

    typedef struct
    {
        BYTE fmt;      /* 格式选项 (FM_FAT, FM_FAT32, FM_EXFAT 和 FM_SFD) */
        BYTE n_fat;    /* FAT 表数量 */
        UINT align;    /* 数据区对齐 (扇区) */
        UINT n_root;   /* 根目录条目数 */
        DWORD au_size; /* 簇大小 (字节) */
    } MKFS_PARM;

    /* 文件函数返回代码 (FRESULT) */

    typedef enum
    {
        FR_OK = 0,              /* (0) 成功 */
        FR_DISK_ERR,            /* (1) 底层磁盘 I/O 层发生硬件错误 */
        FR_INT_ERR,             /* (2) 断言失败 */
        FR_NOT_READY,           /* (3) 物理驱动器无法工作 */
        FR_NO_FILE,             /* (4) 找不到文件 */
        FR_NO_PATH,             /* (5) 找不到路径 */
        FR_INVALID_NAME,        /* (6) 路径名格式无效 */
        FR_DENIED,              /* (7) 由于访问被禁止或目录已满导致访问被拒绝 */
        FR_EXIST,               /* (8) 由于访问被禁止导致访问被拒绝 */
        FR_INVALID_OBJECT,      /* (9) 文件/目录对象无效 */
        FR_WRITE_PROTECTED,     /* (10) 物理驱动器写保护 */
        FR_INVALID_DRIVE,       /* (11) 逻辑驱动器号无效 */
        FR_NOT_ENABLED,         /* (12) 卷没有工作区 */
        FR_NO_FILESYSTEM,       /* (13) 没有有效的 FAT 卷 */
        FR_MKFS_ABORTED,        /* (14) f_mkfs() 因任何问题而中止 */
        FR_TIMEOUT,             /* (15) 在定义的时间内无法获得访问卷的授权 */
        FR_LOCKED,              /* (16) 根据文件共享策略，操作被拒绝 */
        FR_NOT_ENOUGH_CORE,     /* (17) 无法分配长文件名工作缓冲区 */
        FR_TOO_MANY_OPEN_FILES, /* (18) 打开文件数 > FF_FS_LOCK */
        FR_INVALID_PARAMETER    /* (19) 给定的参数无效 */
    } FRESULT;

    /*--------------------------------------------------------------*/
    /* FatFs 模块应用程序接口                                        */

    FRESULT f_open(FIL *fp, const TCHAR *path, BYTE mode);                               /* 打开或创建一个文件 */
    FRESULT f_close(FIL *fp);                                                            /* 关闭一个打开的文件对象 */
    FRESULT f_read(FIL *fp, void *buff, UINT btr, UINT *br);                             /* 从文件中读取数据 */
    FRESULT f_write(FIL *fp, const void *buff, UINT btw, UINT *bw);                      /* 向文件写入数据 */
    FRESULT f_lseek(FIL *fp, FSIZE_t ofs);                                               /* 移动文件的文件指针 */
    FRESULT f_truncate(FIL *fp);                                                         /* 截断文件 */
    FRESULT f_sync(FIL *fp);                                                             /* 刷新写入文件的缓存数据 */
    FRESULT f_opendir(DIR *dp, const TCHAR *path);                                       /* 打开一个目录 */
    FRESULT f_closedir(DIR *dp);                                                         /* 关闭一个打开的目录 */
    FRESULT f_readdir(DIR *dp, FILINFO *fno);                                            /* 读取一个目录项 */
    FRESULT f_findfirst(DIR *dp, FILINFO *fno, const TCHAR *path, const TCHAR *pattern); /* 查找第一个文件 */
    FRESULT f_findnext(DIR *dp, FILINFO *fno);                                           /* 查找下一个文件 */
    FRESULT f_mkdir(const TCHAR *path);                                                  /* 创建一个子目录 */
    FRESULT f_unlink(const TCHAR *path);                                                 /* 删除一个现有的文件或目录 */
    FRESULT f_rename(const TCHAR *path_old, const TCHAR *path_new);                      /* 重命名/移动一个文件或目录 */
    FRESULT f_stat(const TCHAR *path, FILINFO *fno);                                     /* 获取文件状态 */
    FRESULT f_chmod(const TCHAR *path, BYTE attr, BYTE mask);                            /* 更改文件/目录的属性 */
    FRESULT f_utime(const TCHAR *path, const FILINFO *fno);                              /* 更改文件/目录的时间戳 */
    FRESULT f_chdir(const TCHAR *path);                                                  /* 更改当前目录 */
    FRESULT f_chdrive(const TCHAR *path);                                                /* 更改当前驱动器 */
    FRESULT f_getcwd(TCHAR *buff, UINT len);                                             /* 获取当前目录 */
    FRESULT f_getfree(const TCHAR *path, DWORD *nclst, FATFS **fatfs);                   /* 获取驱动器上的空闲簇数量 */
    FRESULT f_getlabel(const TCHAR *path, TCHAR *label, DWORD *vsn);                     /* 获取卷标 */
    FRESULT f_setlabel(const TCHAR *label);                                              /* 设置卷标 */
    FRESULT f_forward(FIL *fp, UINT (*func)(const BYTE *, UINT), UINT btf, UINT *bf);    /* 将数据转发到流 */
    FRESULT f_expand(FIL *fp, FSIZE_t fsz, BYTE opt);                                    /* 为文件分配一个连续块 */
    FRESULT f_mount(FATFS *fs, const TCHAR *path, BYTE opt);                             /* 挂载/卸载一个逻辑驱动器 */
    FRESULT f_mkfs(const TCHAR *path, const MKFS_PARM *opt, void *work, UINT len);       /* 创建一个 FAT 卷 */
    FRESULT f_fdisk(BYTE pdrv, const LBA_t ptbl[], void *work);                          /* 将一个物理驱动器划分为多个分区 */
    FRESULT f_setcp(WORD cp);                                                            /* 设置当前代码页 */
    int f_putc(TCHAR c, FIL *fp);                                                        /* 向文件写入一个字符 */
    int f_puts(const TCHAR *str, FIL *cp);                                               /* 向文件写入一个字符串 */
    int f_printf(FIL *fp, const TCHAR *str, ...);                                        /* 向文件写入一个格式化字符串 */
    TCHAR *f_gets(TCHAR *buff, int len, FIL *fp);                                        /* 从文件读取一个字符串 */

    #define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))                           /* 检查是否到达文件末尾 */
    #define f_error(fp) ((fp)->err)                                                      /* 获取文件错误状态 */
    #define f_tell(fp) ((fp)->fptr)                                                      /* 获取当前文件指针位置 */
    #define f_size(fp) ((fp)->obj.objsize)                                               /* 获取文件大小 */
    #define f_rewind(fp) f_lseek((fp), 0)                                                /* 将文件指针重置到文件开头 */
    #define f_rewinddir(dp) f_readdir((dp), 0)                                           /* 重置目录读取位置到开始 */
    #define f_rmdir(path) f_unlink(path)                                                 /* 删除一个目录 */
    #define f_unmount(path) f_mount(0, path, 0)                                          /* 卸载一个卷 */


    /*--------------------------------------------------------------*/
    /* 额外的用户定义函数                                            */

    /* RTC 函数 */
    #if !FF_FS_READONLY && !FF_FS_NORTC
    DWORD get_fattime(void); /* 获取当前时间，用于文件时间戳 */
    #endif

    /* 长文件名支持函数 */
    #if FF_USE_LFN >= 1                   /* 代码转换 (在 unicode.c 中定义) */
    WCHAR ff_oem2uni(WCHAR oem, WORD cp); /* OEM 代码到 Unicode 转换 */
    WCHAR ff_uni2oem(DWORD uni, WORD cp); /* Unicode 到 OEM 代码转换 */
    DWORD ff_wtoupper(DWORD uni);         /* Unicode 大写转换 */
    #endif
    #if FF_USE_LFN == 3                   /* 动态内存分配 */
    void *ff_memalloc(UINT msize);        /* 分配内存块 */
    void ff_memfree(void *mblock);        /* 释放内存块 */
    #endif

    /* 同步函数 */
    #if FF_FS_REENTRANT
    int ff_cre_syncobj(BYTE vol, FF_SYNC_t *sobj); /* 创建一个同步对象 */
    int ff_req_grant(FF_SYNC_t sobj);              /* 锁定同步对象 */
    void ff_rel_grant(FF_SYNC_t sobj);             /* 解锁同步对象 */
    int ff_del_syncobj(FF_SYNC_t sobj);            /* 删除一个同步对象 */
    #endif


    /*--------------------------------------------------------------*/
    /* 标志和偏移地址                                                */


    /* 文件访问模式和打开方法标志 (f_open 的第 3 个参数) */
    #define FA_READ 0x01          /* 读访问 */
    #define FA_WRITE 0x02         /* 写访问 */
    #define FA_OPEN_EXISTING 0x00 /* 仅打开现有文件 */
    #define FA_CREATE_NEW 0x04    /* 创建新文件，如果文件已存在则失败 */
    #define FA_CREATE_ALWAYS 0x08 /* 总是创建新文件，覆盖现有文件 */
    #define FA_OPEN_ALWAYS 0x10   /* 如果文件存在则打开，否则创建 */
    #define FA_OPEN_APPEND 0x30   /* 以追加模式打开，如果文件不存在则创建 */

    /* 快速查找控制 (f_lseek 的第 2 个参数) */
    #define CREATE_LINKMAP ((FSIZE_t)0 - 1) /* 创建簇链接映射表 */

    /* 格式化选项 (f_mkfs 的第 2 个参数) */
    #define FM_FAT 0x01   /* 创建 FAT12/16 卷 */
    #define FM_FAT32 0x02 /* 创建 FAT32 卷 */
    #define FM_EXFAT 0x04 /* 创建 exFAT 卷 */
    #define FM_ANY 0x07   /* 任何 FAT 类型 */
    #define FM_SFD 0x08   /* 创建 SFD 格式（无分区） */

    /* 文件系统类型 (FATFS.fs_type) */
    #define FS_FAT12 1 /* FAT12 */
    #define FS_FAT16 2 /* FAT16 */
    #define FS_FAT32 3 /* FAT32 */
    #define FS_EXFAT 4 /* exFAT */

    /* 目录条目的文件属性位 (FILINFO.fattrib) */
    #define AM_RDO 0x01 /* 只读 */
    #define AM_HID 0x02 /* 隐藏 */
    #define AM_SYS 0x04 /* 系统 */
    #define AM_DIR 0x10 /* 目录 */
    #define AM_ARC 0x20 /* 存档 */


    #ifdef __cplusplus
}
    #endif

#endif /* FF_DEFINED */
