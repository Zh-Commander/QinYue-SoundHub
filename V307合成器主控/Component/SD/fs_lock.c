/**
 * @file    fs_lock.c
 * @brief   FatFS 共享互斥锁实现
 */

#include "fs_lock.h"

static SemaphoreHandle_t g_fs_mutex = NULL;

void fs_lock_init(void)
{
    if (g_fs_mutex == NULL) {
        g_fs_mutex = xSemaphoreCreateMutex();
    }
}

BaseType_t fs_lock_take(TickType_t timeout_ticks)
{
    if (g_fs_mutex == NULL) {
        fs_lock_init();
    }

    if (g_fs_mutex == NULL) {
        return pdFALSE;
    }

    return xSemaphoreTake(g_fs_mutex, timeout_ticks);
}

void fs_lock_give(void)
{
    if (g_fs_mutex != NULL) {
        xSemaphoreGive(g_fs_mutex);
    }
}
