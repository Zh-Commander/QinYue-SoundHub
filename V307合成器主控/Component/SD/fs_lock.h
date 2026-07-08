/**
 * @file    fs_lock.h
 * @brief   FatFS 共享互斥锁接口
 */

#ifndef __FS_LOCK_H
#define __FS_LOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "semphr.h"

void fs_lock_init(void);
BaseType_t fs_lock_take(TickType_t timeout_ticks);
void fs_lock_give(void);

#ifdef __cplusplus
}
#endif

#endif /* __FS_LOCK_H */
