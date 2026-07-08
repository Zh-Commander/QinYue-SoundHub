/**
 * @file    loop.c
 * @brief   基于 SD 事件文件的录制回放模块
 */

#include "loop.h"
#include "debug.h"
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "..\\dynamic\\dynamic_audio.h"
#include "..\\SD\\FATFS\\ff.h"
#include "..\\SD\\fs_lock.h"

#define LOOP_TASK_PRIO               6
#define LOOP_TASK_STACK_SIZE         1024
#define LOOP_RECORD_CACHE_EVENTS     16
#define LOOP_CMD_BUFFER_SIZE         32
#define LOOP_FS_TIMEOUT_MS           200
#define LOOP_STARTUP_DIR_DELAY_MS    3000
#define LOOP_DIR_RETRY_INTERVAL_MS   1000
#define LOOP_TASK_WAIT_MS            20
#define LOOP_TASK_STOP_WAIT_MS       300
#define LOOP_INVALID_SLOT            0xFFU
#define LOOP_NOTIFY_PLAYBACK         (1UL << 0)
#define LOOP_NOTIFY_COMMAND          (1UL << 1)
#define LOOP_NOTIFY_RECORD_EVENT     (1UL << 2)
#define LOOP_NOTIFY_ALL              (LOOP_NOTIFY_PLAYBACK | LOOP_NOTIFY_COMMAND | LOOP_NOTIFY_RECORD_EVENT)
#define LOOP_RECORD_TEMP_FILE_PATH   "1:/loops/LPREC.TMP"
#define LOOP_PLAY_TEMP_FILE_PATH     "1:/loops/LPPLAY.TMP"
#define LOOP_PROBE_TEMP_FILE_PATH    "1:/loops/LPPROBE.TMP"
#define LOOP_MIDI_DIVISION           480U
#define LOOP_MIDI_TEMPO_US           960000UL
#define LOOP_MIDI_BASE_NOTE          60U
#define LOOP_MIDI_MELODY_CHANNEL     0U
#define LOOP_MIDI_DRUM_CHANNEL       9U
#define LOOP_MIDI_MAX_TRACKS         16U
#define LOOP_RTOS_TICK_US            (1000000UL / configTICK_RATE_HZ)
#define LOOP_MIDI_TRACK_LENGTH_OFFSET 18UL

typedef enum {
    LOOP_RECORD_EVENT_NOTE_ON = 1,
    LOOP_RECORD_EVENT_NOTE_OFF = 2,
} loop_record_event_type_t;

typedef struct {
    uint8_t type;
    uint8_t key;
    uint8_t velocity;
    uint8_t reserved;
} loop_record_event_msg_t;

typedef enum {
    LOOP_MIDI_EVT_NONE = 0,
    LOOP_MIDI_EVT_NOTE_ON,
    LOOP_MIDI_EVT_NOTE_OFF,
    LOOP_MIDI_EVT_TEMPO,
    LOOP_MIDI_EVT_END,
} loop_midi_event_kind_t;

typedef struct {
    uint32_t track_start_offset;
    uint32_t track_end_offset;
    uint32_t current_offset;
    uint32_t abs_midi_tick;
    uint32_t next_midi_tick;
    uint32_t tempo_us_per_qn;
    uint8_t running_status;
    uint8_t kind;
    uint8_t note;
    uint8_t velocity;
    uint8_t active;
} loop_midi_track_state_t;

typedef struct {
    SemaphoreHandle_t mutex;
    TaskHandle_t task_handle;
    QueueHandle_t record_event_queue;
    volatile uint8_t running;
    uint8_t recording;
    uint8_t playing;
    uint8_t record_file_open;
    uint8_t play_file_open;
    uint8_t has_next_event;
    uint8_t selected_slot;
    uint8_t record_slot;
    uint8_t play_slot;
    TickType_t record_start_tick;
    TickType_t playback_start_tick;
    FIL record_file;
    FIL play_file;
    loop_file_header_t header;
    loop_event_t next_event;
    loop_event_t record_cache[LOOP_RECORD_CACHE_EVENTS];
    uint8_t record_cache_count;
    uint32_t recorded_event_count;
    uint32_t record_track_length_bytes;
    uint32_t record_last_event_tick;
    uint32_t last_loop_length_ticks;
    uint8_t slot_has_file[LOOP_SLOT_COUNT];
    uint32_t slot_event_count[LOOP_SLOT_COUNT];
    uint32_t slot_length_ticks[LOOP_SLOT_COUNT];
    char cmd_buffer[LOOP_CMD_BUFFER_SIZE];
    uint8_t cmd_length;
} loop_context_t;

static loop_context_t g_loop = {0};
static loop_midi_track_state_t g_loop_midi_tracks[LOOP_MIDI_MAX_TRACKS] = {0};
static volatile uint32_t g_loop_runtime_generation = 0U;

static uint8_t loop_task_is_active(uint32_t task_generation)
{
    return (uint8_t)((g_loop.running != 0U) && (g_loop_runtime_generation == task_generation));
}

static const char *loop_fresult_to_string(FRESULT res);
static uint32_t loop_get_elapsed_ticks(TickType_t start_tick);
static void loop_build_slot_path(uint8_t slot_index, char *path, uint32_t path_size);
static FRESULT loop_probe_slot_file(uint8_t slot_index, uint8_t *has_file, uint32_t *event_count, uint32_t *loop_length_ticks);
static void loop_refresh_slot_cache(void);
static FRESULT loop_flush_record_cache_locked(void);
static FRESULT loop_store_record_msg_locked(const loop_record_event_msg_t *msg);
static FRESULT loop_drain_record_queue_locked(void);
static int loop_start_recording(void);
static int loop_stop_recording(void);
static int loop_start_playback(void);
static int loop_stop_playback(void);
static void loop_finish_recording_locked(void);
static FRESULT loop_file_read_exact(FIL *file, void *buffer, UINT size);
static FRESULT loop_file_write_exact(FIL *file, const void *buffer, UINT size);
static FRESULT loop_file_skip(FIL *file, uint32_t size);
static FRESULT loop_read_be16(FIL *file, uint16_t *value);
static FRESULT loop_read_be32(FIL *file, uint32_t *value);
static FRESULT loop_write_be16(FIL *file, uint16_t value);
static FRESULT loop_write_be32(FIL *file, uint32_t value);
static FRESULT loop_read_vlq(FIL *file, uint32_t *value);
static FRESULT loop_write_vlq(FIL *file, uint32_t value, uint32_t *written_bytes);
static uint8_t loop_key_to_midi_note(uint8_t key_index);
static int loop_midi_note_to_key(uint8_t midi_note);
static uint8_t loop_midi_event_priority(uint8_t kind);
static int loop_midi_find_next_track_index(uint16_t track_count);
static uint32_t loop_midi_us_to_rtos_ticks(uint64_t total_us);
static uint32_t loop_rtos_ticks_to_midi_ticks(uint32_t ticks);
static FRESULT loop_midi_load_next_relevant_event(FIL *file, loop_midi_track_state_t *track);
static FRESULT loop_midi_convert_file(FIL *src_file, FIL *dst_file, uint8_t *valid_file, loop_file_header_t *header);
static FRESULT loop_prepare_playback_temp_from_slot(uint8_t slot_index, loop_file_header_t *header);
static FRESULT loop_probe_midi_slot_file(uint8_t slot_index, uint32_t *event_count, uint32_t *loop_length_ticks);
static FRESULT loop_export_record_temp_to_slot(uint8_t slot_index, uint32_t loop_length_ticks);
static FRESULT loop_write_midi_file_header(FIL *file);
static FRESULT loop_write_midi_track_preamble(FIL *file, uint32_t *track_length);
static void loop_reset_runtime_preserve_primitives(void);

static void loop_reset_runtime_preserve_primitives(void)
{
    SemaphoreHandle_t mutex = g_loop.mutex;
    QueueHandle_t record_event_queue = g_loop.record_event_queue;

    memset(&g_loop, 0, sizeof(g_loop));
    g_loop.mutex = mutex;
    g_loop.record_event_queue = record_event_queue;
    g_loop.record_slot = LOOP_INVALID_SLOT;
    g_loop.play_slot = LOOP_INVALID_SLOT;
}

static void loop_notify(uint32_t bits)
{
    if (g_loop.task_handle != NULL) {
        xTaskNotify(g_loop.task_handle, bits, eSetBits);
    }
}

static void loop_process_record_events(void)
{
    loop_record_event_msg_t msg;
    FRESULT res;

    if (g_loop.record_event_queue == NULL) {
        return;
    }

    while (xQueueReceive(g_loop.record_event_queue, &msg, 0) == pdTRUE) {
        if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(5)) != pdTRUE) {
            break;
        }

        if (!g_loop.recording || !g_loop.record_file_open) {
            xSemaphoreGive(g_loop.mutex);
            continue;
        }

        res = loop_store_record_msg_locked(&msg);
        if (res != FR_OK) {
            Debug_Printf("[Loop] Record store failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
            loop_finish_recording_locked();
        }

        xSemaphoreGive(g_loop.mutex);
    }
}

static const char *loop_fresult_to_string(FRESULT res)
{
    switch (res) {
    case FR_OK:
        return "FR_OK";
    case FR_DISK_ERR:
        return "FR_DISK_ERR";
    case FR_INT_ERR:
        return "FR_INT_ERR";
    case FR_NOT_READY:
        return "FR_NOT_READY";
    case FR_NO_FILE:
        return "FR_NO_FILE";
    case FR_NO_PATH:
        return "FR_NO_PATH";
    case FR_INVALID_NAME:
        return "FR_INVALID_NAME";
    case FR_DENIED:
        return "FR_DENIED";
    case FR_EXIST:
        return "FR_EXIST";
    case FR_INVALID_OBJECT:
        return "FR_INVALID_OBJECT";
    case FR_WRITE_PROTECTED:
        return "FR_WRITE_PROTECTED";
    case FR_INVALID_DRIVE:
        return "FR_INVALID_DRIVE";
    case FR_NOT_ENABLED:
        return "FR_NOT_ENABLED";
    case FR_NO_FILESYSTEM:
        return "FR_NO_FILESYSTEM";
    case FR_MKFS_ABORTED:
        return "FR_MKFS_ABORTED";
    case FR_TIMEOUT:
        return "FR_TIMEOUT";
    case FR_LOCKED:
        return "FR_LOCKED";
    case FR_NOT_ENOUGH_CORE:
        return "FR_NOT_ENOUGH_CORE";
    case FR_TOO_MANY_OPEN_FILES:
        return "FR_TOO_MANY_OPEN_FILES";
    case FR_INVALID_PARAMETER:
        return "FR_INVALID_PARAMETER";
    default:
        return "FR_UNKNOWN";
    }
}

static uint32_t loop_get_elapsed_ticks(TickType_t start_tick)
{
    return (uint32_t)(xTaskGetTickCount() - start_tick);
}

static FRESULT loop_file_read_exact(FIL *file, void *buffer, UINT size)
{
    UINT bytes_read = 0;
    FRESULT res;

    res = f_read(file, buffer, size, &bytes_read);
    if ((res != FR_OK) || (bytes_read != size)) {
        return (res == FR_OK) ? FR_INT_ERR : res;
    }

    return FR_OK;
}

static FRESULT loop_file_write_exact(FIL *file, const void *buffer, UINT size)
{
    UINT bytes_written = 0;
    FRESULT res;

    res = f_write(file, buffer, size, &bytes_written);
    if ((res != FR_OK) || (bytes_written != size)) {
        return (res == FR_OK) ? FR_DISK_ERR : res;
    }

    return FR_OK;
}

static FRESULT loop_file_skip(FIL *file, uint32_t size)
{
    return f_lseek(file, f_tell(file) + size);
}

static FRESULT loop_read_be16(FIL *file, uint16_t *value)
{
    uint8_t data[2];
    FRESULT res;

    if (value == NULL) {
        return FR_INVALID_PARAMETER;
    }

    res = loop_file_read_exact(file, data, (UINT)sizeof(data));
    if (res != FR_OK) {
        return res;
    }

    *value = (uint16_t)(((uint16_t)data[0] << 8) | data[1]);
    return FR_OK;
}

static FRESULT loop_read_be32(FIL *file, uint32_t *value)
{
    uint8_t data[4];
    FRESULT res;

    if (value == NULL) {
        return FR_INVALID_PARAMETER;
    }

    res = loop_file_read_exact(file, data, (UINT)sizeof(data));
    if (res != FR_OK) {
        return res;
    }

    *value = ((uint32_t)data[0] << 24) |
             ((uint32_t)data[1] << 16) |
             ((uint32_t)data[2] << 8) |
             (uint32_t)data[3];
    return FR_OK;
}

static FRESULT loop_write_be16(FIL *file, uint16_t value)
{
    uint8_t data[2];

    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)(value & 0xFFU);
    return loop_file_write_exact(file, data, (UINT)sizeof(data));
}

static FRESULT loop_write_be32(FIL *file, uint32_t value)
{
    uint8_t data[4];

    data[0] = (uint8_t)(value >> 24);
    data[1] = (uint8_t)((value >> 16) & 0xFFU);
    data[2] = (uint8_t)((value >> 8) & 0xFFU);
    data[3] = (uint8_t)(value & 0xFFU);
    return loop_file_write_exact(file, data, (UINT)sizeof(data));
}

static FRESULT loop_read_vlq(FIL *file, uint32_t *value)
{
    uint8_t byte = 0;
    uint32_t result = 0;
    uint8_t i;
    FRESULT res;

    if (value == NULL) {
        return FR_INVALID_PARAMETER;
    }

    for (i = 0U; i < 4U; i++) {
        res = loop_file_read_exact(file, &byte, 1U);
        if (res != FR_OK) {
            return res;
        }

        result = (result << 7) | (uint32_t)(byte & 0x7FU);
        if ((byte & 0x80U) == 0U) {
            *value = result;
            return FR_OK;
        }
    }

    return FR_INT_ERR;
}

static FRESULT loop_write_vlq(FIL *file, uint32_t value, uint32_t *written_bytes)
{
    uint8_t data[4];
    uint8_t count = 1U;
    uint8_t i;

    data[0] = (uint8_t)(value & 0x7FU);
    while ((value >>= 7) != 0U) {
        data[count++] = (uint8_t)((value & 0x7FU) | 0x80U);
    }

    for (i = 0U; i < count / 2U; i++) {
        uint8_t temp = data[i];
        data[i] = data[count - 1U - i];
        data[count - 1U - i] = temp;
    }

    if (written_bytes != NULL) {
        *written_bytes = count;
    }

    return loop_file_write_exact(file, data, count);
}

static uint8_t loop_key_to_midi_note(uint8_t key_index)
{
    static const uint8_t drum_notes[DYNAMIC_DRUM_KEY_COUNT] = {36U, 38U, 42U, 46U, 45U, 49U};

    if ((key_index >= DYNAMIC_DRUM_KEY_BASE) &&
        (key_index < (DYNAMIC_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT))) {
        return drum_notes[key_index - DYNAMIC_DRUM_KEY_BASE];
    }

    return (uint8_t)(LOOP_MIDI_BASE_NOTE + key_index - 1U);
}

static uint8_t loop_key_to_midi_channel(uint8_t key_index)
{
    if ((key_index >= DYNAMIC_DRUM_KEY_BASE) &&
        (key_index < (DYNAMIC_DRUM_KEY_BASE + DYNAMIC_DRUM_KEY_COUNT))) {
        return LOOP_MIDI_DRUM_CHANNEL;
    }

    return LOOP_MIDI_MELODY_CHANNEL;
}

static int loop_midi_note_to_key(uint8_t midi_note)
{
    uint8_t live_max_note = (uint8_t)(LOOP_MIDI_BASE_NOTE + DYNAMIC_LIVE_KEY_COUNT - 1U);
    static const uint8_t drum_notes[DYNAMIC_DRUM_KEY_COUNT] = {36U, 38U, 42U, 46U, 45U, 49U};
    uint8_t i;

    if ((midi_note >= LOOP_MIDI_BASE_NOTE) && (midi_note <= live_max_note)) {
        return (int)(midi_note - LOOP_MIDI_BASE_NOTE + 1U);
    }

    for (i = 0U; i < DYNAMIC_DRUM_KEY_COUNT; i++) {
        if (midi_note == drum_notes[i]) {
            return (int)(DYNAMIC_DRUM_KEY_BASE + i);
        }
    }

    return -1;
}

static uint32_t loop_midi_us_to_rtos_ticks(uint64_t total_us)
{
    return (uint32_t)((total_us + (LOOP_RTOS_TICK_US / 2U)) / LOOP_RTOS_TICK_US);
}

static uint32_t loop_rtos_ticks_to_midi_ticks(uint32_t ticks)
{
    return (uint32_t)((((uint64_t)ticks * LOOP_RTOS_TICK_US * LOOP_MIDI_DIVISION) + (LOOP_MIDI_TEMPO_US / 2U)) / LOOP_MIDI_TEMPO_US);
}

static uint8_t loop_midi_event_priority(uint8_t kind)
{
    if (kind == LOOP_MIDI_EVT_TEMPO) {
        return 0U;
    }
    if (kind == LOOP_MIDI_EVT_NOTE_OFF) {
        return 1U;
    }
    if (kind == LOOP_MIDI_EVT_NOTE_ON) {
        return 2U;
    }
    if (kind == LOOP_MIDI_EVT_END) {
        return 3U;
    }
    return 4U;
}

static int loop_midi_find_next_track_index(uint16_t track_count)
{
    uint16_t i;
    int best_index = -1;
    uint32_t best_tick = 0U;
    uint8_t best_priority = 0xFFU;

    for (i = 0U; i < track_count; i++) {
        if (g_loop_midi_tracks[i].active == 0U) {
            continue;
        }

        if ((best_index < 0) ||
            (g_loop_midi_tracks[i].next_midi_tick < best_tick) ||
            ((g_loop_midi_tracks[i].next_midi_tick == best_tick) &&
             (loop_midi_event_priority(g_loop_midi_tracks[i].kind) < best_priority))) {
            best_index = (int)i;
            best_tick = g_loop_midi_tracks[i].next_midi_tick;
            best_priority = loop_midi_event_priority(g_loop_midi_tracks[i].kind);
        }
    }

    return best_index;
}

static FRESULT loop_write_midi_file_header(FIL *file)
{
    FRESULT res;

    res = loop_file_write_exact(file, "MThd", 4U);
    if (res != FR_OK) {
        return res;
    }

    res = loop_write_be32(file, 6U);
    if (res != FR_OK) {
        return res;
    }

    res = loop_write_be16(file, 0U);
    if (res != FR_OK) {
        return res;
    }

    res = loop_write_be16(file, 1U);
    if (res != FR_OK) {
        return res;
    }

    return loop_write_be16(file, LOOP_MIDI_DIVISION);
}

static FRESULT loop_write_midi_track_preamble(FIL *file, uint32_t *track_length)
{
    static const uint8_t tempo_event[] = {0x00U, 0xFFU, 0x51U, 0x03U,
                                          (uint8_t)(LOOP_MIDI_TEMPO_US >> 16),
                                          (uint8_t)(LOOP_MIDI_TEMPO_US >> 8),
                                          (uint8_t)(LOOP_MIDI_TEMPO_US)};
    FRESULT res;

    res = loop_file_write_exact(file, "MTrk", 4U);
    if (res != FR_OK) {
        return res;
    }

    res = loop_write_be32(file, 0U);
    if (res != FR_OK) {
        return res;
    }

    res = loop_file_write_exact(file, tempo_event, (UINT)sizeof(tempo_event));
    if (res != FR_OK) {
        return res;
    }

    if (track_length != NULL) {
        *track_length = (uint32_t)sizeof(tempo_event);
    }

    return FR_OK;
}

static FRESULT loop_midi_load_next_relevant_event(FIL *file, loop_midi_track_state_t *track)
{
    FRESULT res;

    if ((file == NULL) || (track == NULL)) {
        return FR_INVALID_PARAMETER;
    }

    while (track->current_offset < track->track_end_offset) {
        uint32_t delta_tick = 0U;
        uint8_t status = 0U;
        uint8_t data0 = 0U;
        uint8_t data1 = 0U;
        uint8_t meta_type = 0U;
        uint32_t meta_len = 0U;
        uint8_t has_inline_data0 = 0U;

        res = f_lseek(file, track->current_offset);
        if (res != FR_OK) {
            return res;
        }

        res = loop_read_vlq(file, &delta_tick);
        if (res != FR_OK) {
            Debug_Printf("[Loop] MIDI read delta failed, offset=%lu error=%d(%s)\r\n",
                         track->current_offset,
                         res,
                         loop_fresult_to_string(res));
            return res;
        }
        track->abs_midi_tick += delta_tick;

        res = loop_file_read_exact(file, &status, 1U);
        if (res != FR_OK) {
            Debug_Printf("[Loop] MIDI read status failed, offset=%lu error=%d(%s)\r\n",
                         track->current_offset,
                         res,
                         loop_fresult_to_string(res));
            return res;
        }

        if (status < 0x80U) {
            if (track->running_status == 0U) {
                Debug_Printf("[Loop] MIDI parse missing running status, offset=%lu raw=%02X track_end=%lu\r\n",
                             track->current_offset,
                             status,
                             track->track_end_offset);
                return FR_INT_ERR;
            }
            data0 = status;
            status = track->running_status;
            has_inline_data0 = 1U;
        } else if ((status >= 0x80U) && (status < 0xF0U)) {
            track->running_status = status;
        }

        if ((status >= 0x80U) && (status <= 0x8FU)) {
            if (has_inline_data0 == 0U) {
                res = loop_file_read_exact(file, &data0, 1U);
                if (res != FR_OK) {
                    return res;
                }
            }
            res = loop_file_read_exact(file, &data1, 1U);
            if (res != FR_OK) {
                return res;
            }
            track->current_offset = f_tell(file);
            track->next_midi_tick = track->abs_midi_tick;
            track->kind = LOOP_MIDI_EVT_NOTE_OFF;
            track->note = data0;
            track->velocity = data1;
            return FR_OK;
        }

        if ((status >= 0x90U) && (status <= 0x9FU)) {
            if (has_inline_data0 == 0U) {
                res = loop_file_read_exact(file, &data0, 1U);
                if (res != FR_OK) {
                    return res;
                }
            }
            res = loop_file_read_exact(file, &data1, 1U);
            if (res != FR_OK) {
                return res;
            }
            track->current_offset = f_tell(file);
            track->next_midi_tick = track->abs_midi_tick;
            track->kind = (data1 == 0U) ? LOOP_MIDI_EVT_NOTE_OFF : LOOP_MIDI_EVT_NOTE_ON;
            track->note = data0;
            track->velocity = data1;
            return FR_OK;
        }

        if (((status >= 0xA0U) && (status <= 0xBFU)) || ((status >= 0xE0U) && (status <= 0xEFU))) {
            if (has_inline_data0 == 0U) {
                res = loop_file_skip(file, 2U);
            } else {
                res = loop_file_skip(file, 1U);
            }
            if (res != FR_OK) {
                return res;
            }
            track->current_offset = f_tell(file);
            continue;
        }

        if (((status >= 0xC0U) && (status <= 0xDFU))) {
            if (has_inline_data0 == 0U) {
                res = loop_file_skip(file, 1U);
                if (res != FR_OK) {
                    return res;
                }
            }
            track->current_offset = f_tell(file);
            continue;
        }

        if (status == 0xFFU) {
            res = loop_file_read_exact(file, &meta_type, 1U);
            if (res != FR_OK) {
                return res;
            }
            res = loop_read_vlq(file, &meta_len);
            if (res != FR_OK) {
                return res;
            }

            if ((meta_type == 0x2FU) && (meta_len == 0U)) {
                track->current_offset = f_tell(file);
                track->active = 0U;
                track->kind = LOOP_MIDI_EVT_NONE;
                return FR_OK;
            }

            if ((meta_type == 0x51U) && (meta_len == 3U)) {
                uint8_t tempo_data[3];
                res = loop_file_read_exact(file, tempo_data, 3U);
                if (res != FR_OK) {
                    return res;
                }
                track->tempo_us_per_qn = ((uint32_t)tempo_data[0] << 16) |
                                         ((uint32_t)tempo_data[1] << 8) |
                                         (uint32_t)tempo_data[2];
                track->current_offset = f_tell(file);
                track->next_midi_tick = track->abs_midi_tick;
                track->kind = LOOP_MIDI_EVT_TEMPO;
                return FR_OK;
            }

            res = loop_file_skip(file, meta_len);
            if (res != FR_OK) {
                return res;
            }
            track->current_offset = f_tell(file);
            continue;
        }

        if ((status == 0xF0U) || (status == 0xF7U)) {
            res = loop_read_vlq(file, &meta_len);
            if (res != FR_OK) {
                return res;
            }
            res = loop_file_skip(file, meta_len);
            if (res != FR_OK) {
                return res;
            }
            track->current_offset = f_tell(file);
            continue;
        }

        Debug_Printf("[Loop] MIDI parse unsupported status, offset=%lu status=%02X running=%02X track_end=%lu\r\n",
                     track->current_offset,
                     status,
                     track->running_status,
                     track->track_end_offset);
        return FR_INT_ERR;
    }

    track->active = 0U;
    track->kind = LOOP_MIDI_EVT_NONE;
    return FR_OK;
}

static FRESULT loop_midi_convert_file(FIL *src_file, FIL *dst_file, uint8_t *valid_file, loop_file_header_t *header)
{
    uint8_t chunk_id[4];
    uint32_t chunk_size = 0U;
    uint16_t format = 0U;
    uint16_t track_count = 0U;
    uint16_t division = 0U;
    uint16_t parsed_tracks = 0U;
    int track_index;
    uint64_t elapsed_us = 0U;
    uint32_t last_global_tick = 0U;
    uint32_t current_tempo = 500000UL;
    uint32_t event_count = 0U;
    uint32_t loop_length_ticks = 0U;
    FRESULT res;
    loop_file_header_t temp_header;

    if ((src_file == NULL) || (header == NULL)) {
        return FR_INVALID_PARAMETER;
    }

    if (valid_file != NULL) {
        *valid_file = 0U;
    }

    memset(&temp_header, 0, sizeof(temp_header));
    temp_header.magic = LOOP_FILE_MAGIC;
    temp_header.version = LOOP_FILE_VERSION;

    memset(g_loop_midi_tracks, 0, sizeof(g_loop_midi_tracks));

    res = loop_file_read_exact(src_file, chunk_id, 4U);
    if (res != FR_OK) {
        Debug_Printf("[Loop] MIDI read header failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
        return res;
    }

    if (memcmp(chunk_id, "MThd", 4) != 0) {
        Debug_Printf("[Loop] MIDI header mismatch, got=%02X %02X %02X %02X\r\n",
                     chunk_id[0], chunk_id[1], chunk_id[2], chunk_id[3]);
        return FR_INT_ERR;
    }

    res = loop_read_be32(src_file, &chunk_size);
    if (res != FR_OK) {
        return res;
    }

    res = loop_read_be16(src_file, &format);
    if (res != FR_OK) {
        return res;
    }

    res = loop_read_be16(src_file, &track_count);
    if (res != FR_OK) {
        return res;
    }

    res = loop_read_be16(src_file, &division);
    if (res != FR_OK) {
        return res;
    }

    if ((division & 0x8000U) != 0U) {
        Debug_Printf("[Loop] MIDI SMPTE division unsupported, division=%u\r\n", division);
        return FR_INT_ERR;
    }

    if (chunk_size > 6U) {
        res = loop_file_skip(src_file, chunk_size - 6U);
        if (res != FR_OK) {
            return res;
        }
    }

    if ((format > 1U) || (track_count == 0U)) {
        Debug_Printf("[Loop] MIDI format unsupported, format=%u tracks=%u\r\n",
                     format,
                     track_count);
        return FR_INT_ERR;
    }

    while ((parsed_tracks < LOOP_MIDI_MAX_TRACKS) && (parsed_tracks < track_count)) {
        res = loop_file_read_exact(src_file, chunk_id, 4U);
        if (res != FR_OK) {
            Debug_Printf("[Loop] MIDI read track chunk id failed, index=%u error=%d(%s)\r\n",
                         parsed_tracks,
                         res,
                         loop_fresult_to_string(res));
            return res;
        }

        res = loop_read_be32(src_file, &chunk_size);
        if (res != FR_OK) {
            Debug_Printf("[Loop] MIDI read track chunk size failed, index=%u error=%d(%s)\r\n",
                         parsed_tracks,
                         res,
                         loop_fresult_to_string(res));
            return res;
        }

        if (memcmp(chunk_id, "MTrk", 4) != 0) {
            res = loop_file_skip(src_file, chunk_size);
            if (res != FR_OK) {
                return res;
            }
            continue;
        }

        g_loop_midi_tracks[parsed_tracks].track_start_offset = f_tell(src_file);
        g_loop_midi_tracks[parsed_tracks].track_end_offset = f_tell(src_file) + chunk_size;
        g_loop_midi_tracks[parsed_tracks].current_offset = f_tell(src_file);
        g_loop_midi_tracks[parsed_tracks].tempo_us_per_qn = current_tempo;
        g_loop_midi_tracks[parsed_tracks].active = 1U;
        g_loop_midi_tracks[parsed_tracks].kind = LOOP_MIDI_EVT_NONE;
        res = loop_midi_load_next_relevant_event(src_file, &g_loop_midi_tracks[parsed_tracks]);
        if (res != FR_OK) {
            Debug_Printf("[Loop] MIDI initial track parse failed, index=%u error=%d(%s)\r\n",
                         parsed_tracks,
                         res,
                         loop_fresult_to_string(res));
            return res;
        }

        parsed_tracks++;
        res = f_lseek(src_file, g_loop_midi_tracks[parsed_tracks - 1U].track_end_offset);
        if (res != FR_OK) {
            return res;
        }
    }

    if (dst_file != NULL) {
        res = loop_file_write_exact(dst_file, &temp_header, (UINT)sizeof(temp_header));
        if (res != FR_OK) {
            return res;
        }
    }

    while ((track_index = loop_midi_find_next_track_index(parsed_tracks)) >= 0) {
        loop_midi_track_state_t *track = &g_loop_midi_tracks[(uint16_t)track_index];
        if (track->next_midi_tick > last_global_tick) {
            elapsed_us += ((uint64_t)(track->next_midi_tick - last_global_tick) * current_tempo) / division;
            last_global_tick = track->next_midi_tick;
        }

        if (track->kind == LOOP_MIDI_EVT_TEMPO) {
            current_tempo = track->tempo_us_per_qn;
        } else if ((track->kind == LOOP_MIDI_EVT_NOTE_ON) || (track->kind == LOOP_MIDI_EVT_NOTE_OFF)) {
            int key_index = loop_midi_note_to_key(track->note);
            if (key_index > 0) {
                loop_event_t event;
                event.tick = loop_midi_us_to_rtos_ticks(elapsed_us);
                event.type = (track->kind == LOOP_MIDI_EVT_NOTE_ON) ? LOOP_EVENT_NOTE_ON : LOOP_EVENT_NOTE_OFF;
                event.key = (uint8_t)key_index;
                event.velocity = track->velocity;
                event.reserved = 0U;
                if (dst_file != NULL) {
                    res = loop_file_write_exact(dst_file, &event, (UINT)sizeof(event));
                    if (res != FR_OK) {
                        return res;
                    }
                }
                event_count++;
                loop_length_ticks = event.tick;
            }
        }

        res = loop_midi_load_next_relevant_event(src_file, track);
        if (res != FR_OK) {
            Debug_Printf("[Loop] MIDI next event parse failed, error=%d(%s) tick=%lu current=%lu end=%lu\r\n",
                         res,
                         loop_fresult_to_string(res),
                         track->next_midi_tick,
                         track->current_offset,
                         track->track_end_offset);
            return res;
        }
    }

    temp_header.event_count = event_count;
    temp_header.loop_length_ticks = loop_length_ticks;
    if (dst_file != NULL) {
        res = f_lseek(dst_file, 0U);
        if (res != FR_OK) {
            return res;
        }

        res = loop_file_write_exact(dst_file, &temp_header, (UINT)sizeof(temp_header));
        if (res != FR_OK) {
            return res;
        }
    }

    *header = temp_header;
    if (valid_file != NULL) {
        *valid_file = (event_count > 0U) ? 1U : 0U;
    }

    return FR_OK;
}

static FRESULT loop_prepare_playback_temp_from_slot(uint8_t slot_index, loop_file_header_t *header)
{
    FIL src_file;
    FIL dst_file;
    FRESULT res;
    char path[32];
    uint8_t valid_file = 0U;

    if (header == NULL) {
        return FR_INVALID_PARAMETER;
    }

    loop_build_slot_path(slot_index, path, (uint32_t)sizeof(path));

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        return FR_INT_ERR;
    }

    res = f_open(&src_file, path, FA_READ);
    if (res != FR_OK) {
        fs_lock_give();
        return res;
    }

    (void)f_unlink(LOOP_PLAY_TEMP_FILE_PATH);
    res = f_open(&dst_file, LOOP_PLAY_TEMP_FILE_PATH, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if (res != FR_OK) {
        (void)f_close(&src_file);
        fs_lock_give();
        return res;
    }

    res = loop_midi_convert_file(&src_file, &dst_file, &valid_file, header);
    (void)f_close(&dst_file);
    (void)f_close(&src_file);
    fs_lock_give();

    if ((res == FR_OK) && (valid_file == 0U)) {
        return FR_NO_FILE;
    }

    return res;
}

static FRESULT loop_probe_midi_slot_file(uint8_t slot_index, uint32_t *event_count, uint32_t *loop_length_ticks)
{
    FIL src_file;
    FRESULT res;
    char path[32];
    uint8_t valid_file = 0U;
    loop_file_header_t temp_header;

    if ((event_count == NULL) || (loop_length_ticks == NULL)) {
        return FR_INVALID_PARAMETER;
    }

    *event_count = 0U;
    *loop_length_ticks = 0U;
    memset(&temp_header, 0, sizeof(temp_header));
    loop_build_slot_path(slot_index, path, (uint32_t)sizeof(path));

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        return FR_INT_ERR;
    }

    res = f_open(&src_file, path, FA_READ);
    if (res != FR_OK) {
        Debug_Printf("[Loop] Probe MIDI open failed, slot=%u path=%s error=%d(%s)\r\n",
                     (unsigned)(slot_index + 1U),
                     path,
                     res,
                     loop_fresult_to_string(res));
        fs_lock_give();
        return res;
    }

    res = loop_midi_convert_file(&src_file, NULL, &valid_file, &temp_header);
    (void)f_close(&src_file);
    fs_lock_give();

    if (res != FR_OK) {
        Debug_Printf("[Loop] Probe MIDI convert failed, slot=%u path=%s error=%d(%s)\r\n",
                     (unsigned)(slot_index + 1U),
                     path,
                     res,
                     loop_fresult_to_string(res));
        return res;
    }

    if (valid_file != 0U) {
        *event_count = temp_header.event_count;
        *loop_length_ticks = temp_header.loop_length_ticks;
        Debug_Printf("[Loop] Probe MIDI ok, slot=%u path=%s events=%lu length=%lu\r\n",
                     (unsigned)(slot_index + 1U),
                     path,
                     temp_header.event_count,
                     temp_header.loop_length_ticks);
    } else {
        Debug_Printf("[Loop] Probe MIDI no playable events, slot=%u path=%s\r\n",
                     (unsigned)(slot_index + 1U),
                     path);
    }

    return FR_OK;
}

static FRESULT loop_export_record_temp_to_slot(uint8_t slot_index, uint32_t loop_length_ticks)
{
    FIL src_file;
    FIL dst_file;
    FRESULT res;
    char path[32];
    loop_file_header_t raw_header;
    uint32_t i;
    uint32_t prev_midi_tick = 0U;
    uint32_t track_length = 0U;

    loop_build_slot_path(slot_index, path, (uint32_t)sizeof(path));

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        return FR_INT_ERR;
    }

    if (g_loop.recorded_event_count == 0U) {
        (void)f_unlink(path);
        fs_lock_give();
        return FR_OK;
    }

    res = f_open(&src_file, LOOP_RECORD_TEMP_FILE_PATH, FA_READ);
    if (res != FR_OK) {
        fs_lock_give();
        return res;
    }

    res = f_open(&dst_file, path, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if (res != FR_OK) {
        (void)f_close(&src_file);
        fs_lock_give();
        return res;
    }

    res = loop_file_read_exact(&src_file, &raw_header, (UINT)sizeof(raw_header));
    if (res == FR_OK) {
        res = loop_write_midi_file_header(&dst_file);
    }
    if (res == FR_OK) {
        res = loop_write_midi_track_preamble(&dst_file, &track_length);
    }

    for (i = 0U; (res == FR_OK) && (i < raw_header.event_count); i++) {
        loop_event_t event;
        uint32_t abs_midi_tick;
        uint32_t delta_midi_tick;
        uint32_t vlq_len = 0U;
        uint8_t msg[3];

        res = loop_file_read_exact(&src_file, &event, (UINT)sizeof(event));
        if (res != FR_OK) {
            break;
        }

        abs_midi_tick = loop_rtos_ticks_to_midi_ticks(event.tick);
        delta_midi_tick = abs_midi_tick - prev_midi_tick;
        prev_midi_tick = abs_midi_tick;

        res = loop_write_vlq(&dst_file, delta_midi_tick, &vlq_len);
        if (res != FR_OK) {
            break;
        }
        track_length += vlq_len;

        msg[0] = (uint8_t)(((event.type == LOOP_EVENT_NOTE_ON) ? 0x90U : 0x80U) |
                            (loop_key_to_midi_channel(event.key) & 0x0FU));
        msg[1] = loop_key_to_midi_note(event.key);
        msg[2] = (event.type == LOOP_EVENT_NOTE_ON) ? event.velocity : 0U;
        res = loop_file_write_exact(&dst_file, msg, (UINT)sizeof(msg));
        if (res != FR_OK) {
            break;
        }
        track_length += (uint32_t)sizeof(msg);
    }

    if (res == FR_OK) {
        uint32_t end_tick = loop_rtos_ticks_to_midi_ticks(loop_length_ticks);
        uint32_t end_delta = (end_tick > prev_midi_tick) ? (end_tick - prev_midi_tick) : 0U;
        uint32_t vlq_len = 0U;
        static const uint8_t eot[3] = {0xFFU, 0x2FU, 0x00U};

        res = loop_write_vlq(&dst_file, end_delta, &vlq_len);
        if (res == FR_OK) {
            track_length += vlq_len;
            res = loop_file_write_exact(&dst_file, eot, (UINT)sizeof(eot));
            if (res == FR_OK) {
                track_length += (uint32_t)sizeof(eot);
            }
        }
    }

    if (res == FR_OK) {
        res = f_lseek(&dst_file, LOOP_MIDI_TRACK_LENGTH_OFFSET);
    }
    if (res == FR_OK) {
        res = loop_write_be32(&dst_file, track_length);
    }
    if (res == FR_OK) {
        res = f_sync(&dst_file);
    }

    (void)f_close(&dst_file);
    (void)f_close(&src_file);
    if (res != FR_OK) {
        (void)f_unlink(path);
    }
    fs_lock_give();
    return res;
}

static void loop_build_slot_path(uint8_t slot_index, char *path, uint32_t path_size)
{
    if((path == NULL) || (path_size == 0U))
    {
        return;
    }

    if(slot_index >= LOOP_SLOT_COUNT)
    {
        path[0] = '\0';
        return;
    }

    (void)snprintf(path, (size_t)path_size, "1:/loops/loop%u.mid", (unsigned)(slot_index + 1U));
}

static FRESULT loop_probe_slot_file(uint8_t slot_index, uint8_t *has_file, uint32_t *event_count, uint32_t *loop_length_ticks)
{
    FIL file;
    UINT bytes_read = 0;
    FRESULT res;
    char path[32];
    uint32_t file_size = 0U;
    uint8_t header_id[16] = {0};

    if((has_file == NULL) || (event_count == NULL) || (loop_length_ticks == NULL))
    {
        return FR_INVALID_PARAMETER;
    }

    *has_file = 0U;
    *event_count = 0U;
    *loop_length_ticks = 0U;
    loop_build_slot_path(slot_index, path, (uint32_t)sizeof(path));

    if(fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE)
    {
        return FR_INT_ERR;
    }

    res = f_open(&file, path, FA_READ);
    if(res == FR_NO_FILE)
    {
        Debug_Printf("[Loop] Probe slot empty, slot=%u path=%s\r\n",
                     (unsigned)(slot_index + 1U),
                     path);
        fs_lock_give();
        return FR_OK;
    }

    if(res != FR_OK)
    {
        Debug_Printf("[Loop] Probe slot open failed, slot=%u path=%s error=%d(%s)\r\n",
                     (unsigned)(slot_index + 1U),
                     path,
                     res,
                     loop_fresult_to_string(res));
        fs_lock_give();
        return res;
    }

    file_size = (uint32_t)f_size(&file);
    res = f_read(&file, header_id, sizeof(header_id), &bytes_read);
    (void)f_close(&file);
    fs_lock_give();

    if(res != FR_OK)
    {
        return res;
    }

    if(bytes_read < 4U)
    {
        Debug_Printf("[Loop] Probe slot read short, slot=%u path=%s size=%lu bytes=%u\r\n",
                     (unsigned)(slot_index + 1U),
                     path,
                     file_size,
                     (unsigned)bytes_read);
        return FR_INT_ERR;
    }

    if(memcmp(header_id, "MThd", 4) == 0)
    {
        res = loop_probe_midi_slot_file(slot_index, event_count, loop_length_ticks);
        if (res != FR_OK) {
            Debug_Printf("[Loop] Probe slot MIDI parse failed, slot=%u path=%s error=%d(%s)\r\n",
                         (unsigned)(slot_index + 1U),
                         path,
                         res,
                         loop_fresult_to_string(res));
            return res;
        }
        *has_file = 1U;
        Debug_Printf("[Loop] Probe slot ready, slot=%u path=%s size=%lu events=%lu length=%lu head=%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                     (unsigned)(slot_index + 1U),
                     path,
                     file_size,
                     *event_count,
                     *loop_length_ticks,
                     header_id[0], header_id[1], header_id[2], header_id[3],
                     header_id[4], header_id[5], header_id[6], header_id[7],
                     header_id[8], header_id[9], header_id[10], header_id[11],
                     header_id[12], header_id[13], header_id[14], header_id[15]);
    }
    else
    {
        Debug_Printf("[Loop] Probe slot header mismatch, slot=%u path=%s size=%lu head=%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
                     (unsigned)(slot_index + 1U),
                     path,
                     file_size,
                     header_id[0], header_id[1], header_id[2], header_id[3],
                     header_id[4], header_id[5], header_id[6], header_id[7],
                     header_id[8], header_id[9], header_id[10], header_id[11],
                     header_id[12], header_id[13], header_id[14], header_id[15]);
    }

    return FR_OK;
}

static void loop_refresh_slot_cache(void)
{
    uint8_t i;

    for(i = 0U; i < LOOP_SLOT_COUNT; i++)
    {
        uint8_t has_file = 0U;
        uint32_t event_count = 0U;
        uint32_t loop_length_ticks = 0U;

        if (!g_loop.running) {
            break;
        }

        if(loop_probe_slot_file(i, &has_file, &event_count, &loop_length_ticks) != FR_OK)
        {
            Debug_Printf("[Loop] Refresh slot cache failed, slot=%u\r\n", (unsigned)(i + 1U));
            has_file = 0U;
            event_count = 0U;
            loop_length_ticks = 0U;
        }

        if (!g_loop.running) {
            break;
        }

        if(g_loop.mutex != NULL)
        {
            if(xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(20)) == pdTRUE)
            {
                g_loop.slot_has_file[i] = has_file;
                g_loop.slot_event_count[i] = event_count;
                g_loop.slot_length_ticks[i] = loop_length_ticks;
                xSemaphoreGive(g_loop.mutex);
            }
        }
        else
        {
            g_loop.slot_has_file[i] = has_file;
            g_loop.slot_event_count[i] = event_count;
            g_loop.slot_length_ticks[i] = loop_length_ticks;
        }

        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
    }
}

static void loop_print_help(void)
{
    Debug_Printf("[Loop] Commands: rec start | rec stop | play start | play stop | loop clear | loop status | help\r\n");
}

static FRESULT loop_ensure_directory_exists(void)
{
    FRESULT res;
    FILINFO info;

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        return FR_INT_ERR;
    }

    memset(&info, 0, sizeof(info));
    res = f_stat(LOOP_EVENT_DIR_PATH, &info);
    if (res == FR_OK) {
        fs_lock_give();
        return FR_OK;
    }

    if (res != FR_NO_PATH && res != FR_NO_FILE) {
        fs_lock_give();
        return res;
    }

    res = f_mkdir(LOOP_EVENT_DIR_PATH);
    fs_lock_give();

    if (res == FR_EXIST) {
        return FR_OK;
    }

    return res;
}

static FRESULT loop_flush_record_cache_locked(void)
{
    UINT bytes_written;
    UINT expected_bytes;
    FRESULT res;

    if (!g_loop.record_file_open || g_loop.record_cache_count == 0) {
        return FR_OK;
    }

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        return FR_INT_ERR;
    }

    expected_bytes = (UINT)(g_loop.record_cache_count * sizeof(loop_event_t));
    res = f_write(&g_loop.record_file, g_loop.record_cache, expected_bytes, &bytes_written);
    fs_lock_give();

    if (res != FR_OK || bytes_written != expected_bytes) {
        return (res == FR_OK) ? FR_DISK_ERR : res;
    }

    if (g_loop.record_cache_count > 0U) {
        g_loop.record_last_event_tick = g_loop.record_cache[g_loop.record_cache_count - 1U].tick;
    }
    g_loop.recorded_event_count += g_loop.record_cache_count;
    g_loop.record_cache_count = 0;
    return FR_OK;
}

static FRESULT loop_store_record_msg_locked(const loop_record_event_msg_t *msg)
{
    loop_event_t *event;
    FRESULT res;

    if ((msg == NULL) || !g_loop.recording || !g_loop.record_file_open) {
        return FR_OK;
    }

    if (g_loop.record_cache_count >= LOOP_RECORD_CACHE_EVENTS) {
        res = loop_flush_record_cache_locked();
        if (res != FR_OK) {
            return res;
        }
    }

    event = &g_loop.record_cache[g_loop.record_cache_count++];
    event->tick = loop_get_elapsed_ticks(g_loop.record_start_tick);
    event->type = (msg->type == LOOP_RECORD_EVENT_NOTE_ON) ? LOOP_EVENT_NOTE_ON : LOOP_EVENT_NOTE_OFF;
    event->key = msg->key;
    event->velocity = msg->velocity;
    event->reserved = 0;

    Debug_Printf("[Loop] Captured evt type=%u key=%u vel=%u tick=%lu\r\n",
                 (unsigned)event->type,
                 (unsigned)event->key,
                 (unsigned)event->velocity,
                 event->tick);

    return FR_OK;
}

static FRESULT loop_drain_record_queue_locked(void)
{
    loop_record_event_msg_t msg;
    FRESULT res;

    if (g_loop.record_event_queue == NULL) {
        return FR_OK;
    }

    while (xQueueReceive(g_loop.record_event_queue, &msg, 0) == pdTRUE) {
        res = loop_store_record_msg_locked(&msg);
        if (res != FR_OK) {
            return res;
        }
    }

    return FR_OK;
}


static FRESULT loop_read_next_event_locked(loop_event_t *event, uint8_t *has_event)
{
    UINT bytes_read;
    FRESULT res;

    *has_event = 0;

    if (!g_loop.play_file_open) {
        return FR_INVALID_OBJECT;
    }

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        return FR_INT_ERR;
    }

    res = f_read(&g_loop.play_file, event, sizeof(loop_event_t), &bytes_read);
    fs_lock_give();

    if (res != FR_OK) {
        return res;
    }

    if (bytes_read == 0) {
        return FR_OK;
    }

    if (bytes_read != sizeof(loop_event_t)) {
        return FR_INT_ERR;
    }

    *has_event = 1;
    return FR_OK;
}

static void loop_stop_playback_locked(void)
{
    if (g_loop.play_file_open) {
        if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
            return;
        }

        f_close(&g_loop.play_file);
        fs_lock_give();
    }

    g_loop.playing = 0;
    g_loop.play_file_open = 0;
    g_loop.has_next_event = 0;
    g_loop.play_slot = LOOP_INVALID_SLOT;
    dynamic_seq_release_all();
}

static void loop_restart_playback_locked(void)
{
    FRESULT res;
    uint8_t has_event = 0;

    if (!g_loop.play_file_open) {
        return;
    }

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        loop_stop_playback_locked();
        return;
    }

    res = f_lseek(&g_loop.play_file, sizeof(loop_file_header_t));
    fs_lock_give();
    if (res != FR_OK) {
        loop_stop_playback_locked();
        return;
    }

    dynamic_seq_release_all();
    res = loop_read_next_event_locked(&g_loop.next_event, &has_event);
    if (res != FR_OK || !has_event) {
        loop_stop_playback_locked();
        return;
    }

    g_loop.has_next_event = 1;
    g_loop.playback_start_tick = xTaskGetTickCount();
}


static void loop_finish_recording_locked(void)
{
    FRESULT res = FR_OK;
    uint8_t record_slot;

    if (!g_loop.recording) {
        return;
    }

    g_loop.last_loop_length_ticks = loop_get_elapsed_ticks(g_loop.record_start_tick);
    g_loop.header.loop_length_ticks = g_loop.last_loop_length_ticks;
    g_loop.header.event_count = g_loop.recorded_event_count + g_loop.record_cache_count;
    res = loop_flush_record_cache_locked();
    record_slot = g_loop.record_slot;

    if ((res == FR_OK) && g_loop.record_file_open) {
        if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) == pdTRUE) {
            res = f_lseek(&g_loop.record_file, 0U);
            if (res == FR_OK) {
                res = loop_file_write_exact(&g_loop.record_file, &g_loop.header, (UINT)sizeof(g_loop.header));
            }
            if (res == FR_OK) {
                res = f_sync(&g_loop.record_file);
            }
            (void)f_close(&g_loop.record_file);
            fs_lock_give();
        } else {
            res = FR_INT_ERR;
        }
    } else if (g_loop.record_file_open) {
        if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) == pdTRUE) {
            (void)f_close(&g_loop.record_file);
            fs_lock_give();
        }
    }

    g_loop.recording = 0;
    g_loop.record_file_open = 0;
    g_loop.record_cache_count = 0;

    if ((res == FR_OK) && (record_slot < LOOP_SLOT_COUNT)) {
        Debug_Printf("[Loop] Export MIDI slot=%u events=%lu length=%lu\r\n",
                     (unsigned)(record_slot + 1U),
                     g_loop.header.event_count,
                     g_loop.last_loop_length_ticks);
        res = loop_export_record_temp_to_slot(record_slot, g_loop.last_loop_length_ticks);
    }

    if ((res == FR_OK) && (record_slot < LOOP_SLOT_COUNT)) {
        uint8_t has_file = 0U;
        uint32_t event_count = 0U;
        uint32_t loop_length_ticks = 0U;

        res = loop_probe_slot_file(record_slot, &has_file, &event_count, &loop_length_ticks);
        if (res == FR_OK) {
            g_loop.slot_has_file[record_slot] = has_file;
            g_loop.slot_event_count[record_slot] = event_count;
            g_loop.slot_length_ticks[record_slot] = loop_length_ticks;
        }
    }

    g_loop.record_slot = LOOP_INVALID_SLOT;

    if (res == FR_OK) {
        Debug_Printf("[Loop] Record stopped, events=%lu, length=%lu ticks\r\n",
                     g_loop.recorded_event_count,
                     g_loop.last_loop_length_ticks);
    } else {
        Debug_Printf("[Loop] Record stop failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
    }
}

static int loop_start_recording(void)
{
    FRESULT res;
    char path[32];

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return -1;
    }

    if (g_loop.recording) {
        Debug_Printf("[Loop] Record is already running\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    if (g_loop.playing) {
        Debug_Printf("[Loop] Stop playback before recording\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    res = loop_ensure_directory_exists();
    if (res != FR_OK) {
        Debug_Printf("[Loop] Create directory failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    memset(&g_loop.header, 0, sizeof(g_loop.header));
    g_loop.header.magic = LOOP_FILE_MAGIC;
    g_loop.header.version = LOOP_FILE_VERSION;
    g_loop.recorded_event_count = 0;
    g_loop.record_track_length_bytes = 0U;
    g_loop.record_last_event_tick = 0U;
    g_loop.last_loop_length_ticks = 0;
    g_loop.record_cache_count = 0;
    g_loop.record_slot = g_loop.selected_slot;
    loop_build_slot_path(g_loop.record_slot, path, (uint32_t)sizeof(path));

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        g_loop.record_slot = LOOP_INVALID_SLOT;
        Debug_Printf("[Loop] FS lock timeout on record start\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    (void)f_unlink(LOOP_RECORD_TEMP_FILE_PATH);
    res = f_open(&g_loop.record_file, LOOP_RECORD_TEMP_FILE_PATH, FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
    if (res == FR_OK) {
        res = loop_file_write_exact(&g_loop.record_file, &g_loop.header, (UINT)sizeof(loop_file_header_t));
        if (res == FR_OK) {
            g_loop.record_file_open = 1;
        } else {
            f_close(&g_loop.record_file);
        }
    }
    fs_lock_give();

    if (res != FR_OK || !g_loop.record_file_open) {
        (void)f_unlink(LOOP_RECORD_TEMP_FILE_PATH);
        g_loop.record_slot = LOOP_INVALID_SLOT;
        Debug_Printf("[Loop] Record start failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    g_loop.record_start_tick = xTaskGetTickCount();
    g_loop.recording = 1;
    Debug_Printf("[Loop] Record started -> slot=%u (%s)\r\n", (unsigned)(g_loop.record_slot + 1U), path);
    xSemaphoreGive(g_loop.mutex);
    return 0;
}

static int loop_stop_recording(void)
{
    FRESULT res;

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return -1;
    }

    if (!g_loop.recording) {
        Debug_Printf("[Loop] Record is not running\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    res = loop_drain_record_queue_locked();
    if (res != FR_OK) {
        Debug_Printf("[Loop] Record drain failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
    }

    loop_finish_recording_locked();
    xSemaphoreGive(g_loop.mutex);
    return (res == FR_OK) ? 0 : -1;
}

static int loop_start_playback(void)
{
    FRESULT res;
    uint8_t has_event = 0;
    char path[32];

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return -1;
    }

    if (g_loop.playing) {
        Debug_Printf("[Loop] Playback is already running\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    if (g_loop.recording) {
        Debug_Printf("[Loop] Stop recording before playback\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    g_loop.play_slot = g_loop.selected_slot;
    loop_build_slot_path(g_loop.play_slot, path, (uint32_t)sizeof(path));

    res = loop_prepare_playback_temp_from_slot(g_loop.play_slot, &g_loop.header);
    if (res != FR_OK) {
        g_loop.play_slot = LOOP_INVALID_SLOT;
        Debug_Printf("[Loop] Playback prepare failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        g_loop.play_slot = LOOP_INVALID_SLOT;
        Debug_Printf("[Loop] FS lock timeout on playback temp open\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    res = f_open(&g_loop.play_file, LOOP_PLAY_TEMP_FILE_PATH, FA_READ);
    if (res == FR_OK) {
        g_loop.play_file_open = 1;
    }
    fs_lock_give();

    if (res != FR_OK || !g_loop.play_file_open) {
        g_loop.play_slot = LOOP_INVALID_SLOT;
        Debug_Printf("[Loop] Playback temp open failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    if (g_loop.header.event_count == 0) {
        loop_stop_playback_locked();
        Debug_Printf("[Loop] MIDI file has no playable events\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        loop_stop_playback_locked();
        Debug_Printf("[Loop] FS lock timeout on playback seek\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    res = f_lseek(&g_loop.play_file, sizeof(loop_file_header_t));
    fs_lock_give();
    if (res != FR_OK) {
        loop_stop_playback_locked();
        Debug_Printf("[Loop] Playback seek failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    dynamic_seq_release_all();
    res = loop_read_next_event_locked(&g_loop.next_event, &has_event);
    if (res != FR_OK || !has_event) {
        loop_stop_playback_locked();
        Debug_Printf("[Loop] Read first event failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    g_loop.has_next_event = 1;
    g_loop.playback_start_tick = xTaskGetTickCount();
    g_loop.playing = 1;
    Debug_Printf("[Loop] Playback started slot=%u, events=%lu, length=%lu ticks\r\n",
                 (unsigned)(g_loop.play_slot + 1U),
                 g_loop.header.event_count,
                 g_loop.header.loop_length_ticks);
    xSemaphoreGive(g_loop.mutex);
    loop_notify(LOOP_NOTIFY_PLAYBACK);
    return 0;
}

static int loop_stop_playback(void)
{
    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return -1;
    }

    if (!g_loop.playing) {
        Debug_Printf("[Loop] Playback is not running\r\n");
        xSemaphoreGive(g_loop.mutex);
        return -1;
    }

    loop_stop_playback_locked();
    Debug_Printf("[Loop] Playback stopped\r\n");
    xSemaphoreGive(g_loop.mutex);
    return 0;
}

static void loop_clear_file(void)
{
    FRESULT res;
    char path[32];

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return;
    }

    if (g_loop.recording || g_loop.playing) {
        Debug_Printf("[Loop] Stop record/playback before clearing file\r\n");
        xSemaphoreGive(g_loop.mutex);
        return;
    }

    loop_build_slot_path(g_loop.selected_slot, path, (uint32_t)sizeof(path));

    if (fs_lock_take(pdMS_TO_TICKS(LOOP_FS_TIMEOUT_MS)) != pdTRUE) {
        Debug_Printf("[Loop] FS lock timeout on clear\r\n");
        xSemaphoreGive(g_loop.mutex);
        return;
    }

    Debug_Printf("[Loop][FS] Unlink slot file: %s\r\n", path);
    res = f_unlink(path);
    fs_lock_give();

    if (res == FR_OK || res == FR_NO_FILE) {
        if (g_loop.selected_slot < LOOP_SLOT_COUNT) {
            g_loop.slot_has_file[g_loop.selected_slot] = 0U;
            g_loop.slot_event_count[g_loop.selected_slot] = 0U;
            g_loop.slot_length_ticks[g_loop.selected_slot] = 0U;
        }
        Debug_Printf("[Loop] Cleared %s\r\n", path);
    } else {
        Debug_Printf("[Loop] Clear file failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
    }

    xSemaphoreGive(g_loop.mutex);
}

static void loop_print_status(void)
{
    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(100)) != pdTRUE) {
        return;
    }

    Debug_Printf("[Loop] Status: recording=%d, playing=%d, cached=%d, recorded=%lu, last_length=%lu ticks\r\n",
                 g_loop.recording,
                 g_loop.playing,
                 g_loop.record_cache_count,
                 g_loop.recorded_event_count,
                 g_loop.last_loop_length_ticks);
    xSemaphoreGive(g_loop.mutex);
}

static uint8_t loop_is_whitespace_char(char ch)
{
    return (uint8_t)(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n');
}

static void loop_trim_command(char *cmd)
{
    size_t start = 0;
    size_t end;
    size_t len;

    if (cmd == NULL) {
        return;
    }

    len = strlen(cmd);
    end = len;

    while (start < len && loop_is_whitespace_char(cmd[start])) {
        start++;
    }

    while (end > start && loop_is_whitespace_char(cmd[end - 1])) {
        end--;
    }

    if (start > 0 && end > start) {
        memmove(cmd, cmd + start, end - start);
    }

    cmd[end - start] = '\0';
}

static uint8_t loop_is_complete_command(const char *cmd)
{
    if (cmd == NULL || cmd[0] == '\0') {
        return 0;
    }

    return (uint8_t)(strcmp(cmd, "rec start") == 0 ||
                     strcmp(cmd, "rec stop") == 0 ||
                     strcmp(cmd, "play start") == 0 ||
                     strcmp(cmd, "play stop") == 0 ||
                     strcmp(cmd, "loop clear") == 0 ||
                     strcmp(cmd, "loop status") == 0 ||
                     strcmp(cmd, "help") == 0);
}

static void loop_handle_command(const char *cmd)
{
    char local_cmd[LOOP_CMD_BUFFER_SIZE];
    size_t cmd_len;

    if (cmd == NULL) {
        return;
    }

    cmd_len = strlen(cmd);
    if (cmd_len >= sizeof(local_cmd)) {
        cmd_len = sizeof(local_cmd) - 1;
    }

    memcpy(local_cmd, cmd, cmd_len);
    local_cmd[cmd_len] = '\0';
    loop_trim_command(local_cmd);

    if (local_cmd[0] == '\0') {
        return;
    }

    Debug_Printf("[Loop] Command received: %s\r\n", local_cmd);

    if (strcmp(local_cmd, "rec start") == 0) {
        loop_start_recording();
    } else if (strcmp(local_cmd, "rec stop") == 0) {
        loop_stop_recording();
    } else if (strcmp(local_cmd, "play start") == 0) {
        loop_start_playback();
    } else if (strcmp(local_cmd, "play stop") == 0) {
        loop_stop_playback();
    } else if (strcmp(local_cmd, "loop clear") == 0) {
        loop_clear_file();
    } else if (strcmp(local_cmd, "loop status") == 0) {
        loop_print_status();
    } else if (strcmp(local_cmd, "help") == 0) {
        loop_print_help();
    } else {
        Debug_Printf("[Loop] Unknown command: %s\r\n", local_cmd);
        loop_print_help();
    }
}

static void loop_poll_debug_commands(void)
{
    uint8_t ch;

    while (Debug_ReadCharBlocking(&ch, 0) == pdTRUE) {
        if (ch == '\r' || ch == '\n') {
            if (g_loop.cmd_length > 0) {
                g_loop.cmd_buffer[g_loop.cmd_length] = '\0';
                loop_handle_command(g_loop.cmd_buffer);
                g_loop.cmd_length = 0;
            }
            continue;
        }

        if (ch == '\b' || ch == 0x7F) {
            if (g_loop.cmd_length > 0) {
                g_loop.cmd_length--;
                g_loop.cmd_buffer[g_loop.cmd_length] = '\0';
            }
            continue;
        }

        if (ch >= 'A' && ch <= 'Z') {
            ch = (uint8_t)(ch - 'A' + 'a');
        }

        if (g_loop.cmd_length < (LOOP_CMD_BUFFER_SIZE - 1)) {
            g_loop.cmd_buffer[g_loop.cmd_length++] = (char)ch;
            g_loop.cmd_buffer[g_loop.cmd_length] = '\0';

            if (loop_is_complete_command(g_loop.cmd_buffer)) {
                Debug_Printf("[Loop] Auto submit command without CRLF: %s\r\n", g_loop.cmd_buffer);
                loop_handle_command(g_loop.cmd_buffer);
                g_loop.cmd_length = 0;
                g_loop.cmd_buffer[0] = '\0';
            }
        } else {
            Debug_Printf("[Loop] Command buffer overflow, reset\r\n");
            g_loop.cmd_length = 0;
            g_loop.cmd_buffer[0] = '\0';
        }
    }
}

static void loop_process_playback(void)
{
    uint8_t has_event = 0;
    uint8_t playback_finished = 0;
    FRESULT res = FR_OK;
    uint32_t elapsed_ticks;

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(5)) != pdTRUE) {
        return;
    }

    while (g_loop.playing) {
        elapsed_ticks = loop_get_elapsed_ticks(g_loop.playback_start_tick);

        if (!g_loop.has_next_event) {
            if (elapsed_ticks < g_loop.header.loop_length_ticks) {
                break;
            }

            loop_restart_playback_locked();
            if (!g_loop.playing || !g_loop.has_next_event) {
                playback_finished = 1;
                break;
            }
            continue;
        }

        if (elapsed_ticks < g_loop.next_event.tick) {
            break;
        }

        if (g_loop.next_event.type == LOOP_EVENT_NOTE_ON) {
            dynamic_seq_note_on(g_loop.next_event.key, g_loop.next_event.velocity);
        } else if (g_loop.next_event.type == LOOP_EVENT_NOTE_OFF) {
            dynamic_seq_note_off(g_loop.next_event.key);
        }

        res = loop_read_next_event_locked(&g_loop.next_event, &has_event);
        if (res != FR_OK) {
            loop_stop_playback_locked();
            playback_finished = 1;
            break;
        }

        g_loop.has_next_event = has_event;
    }

    xSemaphoreGive(g_loop.mutex);

    if (playback_finished && res != FR_OK) {
        Debug_Printf("[Loop] Playback aborted, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
    }
}


static void loop_task(void *pvParameters)
{
    uint32_t task_generation = (uint32_t)pvParameters;
    FRESULT res;
    uint8_t dir_checked = 0;
    TickType_t last_dir_check_tick = 0;
    TickType_t wait_ticks = pdMS_TO_TICKS(LOOP_TASK_WAIT_MS);

    (void)pvParameters;
    Debug_Printf("[Loop] task entered\r\n");

    while (loop_task_is_active(task_generation) != 0U) {
        uint32_t notify_value = 0;
        TickType_t now = xTaskGetTickCount();
        uint8_t should_process_playback = 0;

        if (!dir_checked && now >= pdMS_TO_TICKS(LOOP_STARTUP_DIR_DELAY_MS) &&
            (last_dir_check_tick == 0 || (now - last_dir_check_tick) >= pdMS_TO_TICKS(LOOP_DIR_RETRY_INTERVAL_MS))) {
            last_dir_check_tick = now;
            res = loop_ensure_directory_exists();
            if (!g_loop.running) {
                break;
            }
            if (res == FR_OK) {
                Debug_Printf("[Loop] Directory ready: %s\r\n", LOOP_EVENT_DIR_PATH);
                loop_refresh_slot_cache();
                if (!g_loop.running) {
                    break;
                }
                dir_checked = 1;
            } else {
                Debug_Printf("[Loop] Directory prepare failed, error=%d(%s)\r\n", res, loop_fresult_to_string(res));
            }
        }

        if (g_loop.playing) {
            should_process_playback = 1;
            if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(5)) == pdTRUE) {
                if (g_loop.has_next_event) {
                    uint32_t elapsed_ticks = loop_get_elapsed_ticks(g_loop.playback_start_tick);
                    uint32_t remain_ticks = (g_loop.next_event.tick > elapsed_ticks) ? (g_loop.next_event.tick - elapsed_ticks) : 0;
                    wait_ticks = pdMS_TO_TICKS(LOOP_TASK_WAIT_MS);
                    if (remain_ticks < wait_ticks) {
                        wait_ticks = (TickType_t)remain_ticks;
                    }
                } else {
                    uint32_t elapsed_ticks = loop_get_elapsed_ticks(g_loop.playback_start_tick);
                    uint32_t remain_ticks = (g_loop.header.loop_length_ticks > elapsed_ticks) ? (g_loop.header.loop_length_ticks - elapsed_ticks) : 0;
                    wait_ticks = pdMS_TO_TICKS(LOOP_TASK_WAIT_MS);
                    if (remain_ticks < wait_ticks) {
                        wait_ticks = (TickType_t)remain_ticks;
                    }
                }
                xSemaphoreGive(g_loop.mutex);
            }
        } else {
            wait_ticks = pdMS_TO_TICKS(LOOP_TASK_WAIT_MS);
        }

        xTaskNotifyWait(0, LOOP_NOTIFY_ALL, &notify_value, wait_ticks);

        loop_poll_debug_commands();
        loop_process_record_events();

        if (should_process_playback || (notify_value & LOOP_NOTIFY_PLAYBACK) != 0U) {
            loop_process_playback();
        }
    }

    if (g_loop_runtime_generation == task_generation) {
        g_loop.task_handle = NULL;
    }
    vTaskDelete(NULL);
}

int loop_init(void)
{
    BaseType_t task_res;

    if (g_loop.running != 0U) {
        return 0;
    }

    loop_reset_runtime_preserve_primitives();
    g_loop_runtime_generation++;
    if (g_loop_runtime_generation == 0U) {
        g_loop_runtime_generation = 1U;
    }
    fs_lock_init();
    if (g_loop.mutex == NULL) {
        g_loop.mutex = xSemaphoreCreateMutex();
    }
    if (g_loop.mutex == NULL) {
        Debug_Printf("[Loop] Create mutex failed\r\n");
        g_loop.running = 0;
        return -1;
    }

    if (g_loop.record_event_queue == NULL) {
        g_loop.record_event_queue = xQueueCreate(LOOP_RECORD_CACHE_EVENTS * 2, sizeof(loop_record_event_msg_t));
    } else {
        (void)xQueueReset(g_loop.record_event_queue);
    }
    if (g_loop.record_event_queue == NULL) {
        Debug_Printf("[Loop] Create record event queue failed\r\n");
        g_loop.running = 0;
        return -1;
    }

    g_loop.selected_slot = 0U;
    g_loop.record_slot = LOOP_INVALID_SLOT;
    g_loop.play_slot = LOOP_INVALID_SLOT;

    g_loop.running = 1;

    task_res = xTaskCreate((TaskFunction_t)loop_task,
                           (const char *)"loop_task",
                           (uint16_t)LOOP_TASK_STACK_SIZE,
                           (void *)g_loop_runtime_generation,
                           (UBaseType_t)LOOP_TASK_PRIO,
                           (TaskHandle_t *)&g_loop.task_handle);
    if (task_res != pdPASS) {
        Debug_Printf("[Loop] Create task failed\r\n");
        g_loop.running = 0;
        return -1;
    }

    loop_print_help();
    return 0;
}

void loop_deinit(void)
{
    uint32_t stop_generation;

    if ((g_loop.mutex == NULL) || (g_loop.record_event_queue == NULL)) {
        return;
    }

    stop_generation = g_loop_runtime_generation;
    g_loop.running = 0;
    loop_notify(LOOP_NOTIFY_ALL);

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        if (g_loop.recording) {
            loop_finish_recording_locked();
        }
        if (g_loop.playing) {
            loop_stop_playback_locked();
        }
        xSemaphoreGive(g_loop.mutex);
    }

    for (int wait = 0; wait < LOOP_TASK_STOP_WAIT_MS; wait++) {
        if ((g_loop_runtime_generation != stop_generation) || (g_loop.task_handle == NULL)) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    g_loop.running = 0U;
    g_loop.recording = 0U;
    g_loop.playing = 0U;
    g_loop.record_file_open = 0U;
    g_loop.play_file_open = 0U;
    g_loop.has_next_event = 0U;
    g_loop.record_slot = LOOP_INVALID_SLOT;
    g_loop.play_slot = LOOP_INVALID_SLOT;
    g_loop.cmd_length = 0U;
    g_loop.cmd_buffer[0] = '\0';
    (void)xQueueReset(g_loop.record_event_queue);
}

void loop_on_live_note_on(uint8_t key_index, uint8_t velocity)
{
    loop_record_event_msg_t msg;

    if (g_loop.record_event_queue == NULL) {
        return;
    }

    msg.type = LOOP_RECORD_EVENT_NOTE_ON;
    msg.key = key_index;
    msg.velocity = velocity;
    msg.reserved = 0;

    if (xQueueSend(g_loop.record_event_queue, &msg, 0) == pdTRUE) {
        loop_notify(LOOP_NOTIFY_RECORD_EVENT);
    }
}

void loop_on_live_note_off(uint8_t key_index)
{
    loop_record_event_msg_t msg;

    if (g_loop.record_event_queue == NULL) {
        return;
    }

    msg.type = LOOP_RECORD_EVENT_NOTE_OFF;
    msg.key = key_index;
    msg.velocity = 0;
    msg.reserved = 0;

    if (xQueueSend(g_loop.record_event_queue, &msg, 0) == pdTRUE) {
        loop_notify(LOOP_NOTIFY_RECORD_EVENT);
    }
}

int loop_select_slot(uint8_t slot_index)
{
    if (slot_index >= LOOP_SLOT_COUNT) {
        return -1;
    }

    if (g_loop.mutex == NULL) {
        return -1;
    }

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return -1;
    }

    g_loop.selected_slot = slot_index;
    xSemaphoreGive(g_loop.mutex);
    return 0;
}

int loop_ui_start_record(void)
{
    if (g_loop.mutex == NULL) {
        return -1;
    }

    return loop_start_recording();
}

int loop_ui_stop_record(void)
{
    if (g_loop.mutex == NULL) {
        return -1;
    }

    return loop_stop_recording();
}

int loop_ui_start_playback(void)
{
    if (g_loop.mutex == NULL) {
        return -1;
    }

    return loop_start_playback();
}

int loop_ui_stop_playback(void)
{
    if (g_loop.mutex == NULL) {
        return -1;
    }

    return loop_stop_playback();
}

int loop_ui_clear_selected_slot(void)
{
    uint8_t selected_slot = LOOP_INVALID_SLOT;
    uint8_t has_file_after = 0U;
    uint32_t event_count = 0U;
    uint32_t loop_length_ticks = 0U;

    if (g_loop.mutex == NULL) {
        return -1;
    }

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(50)) == pdTRUE) {
        selected_slot = g_loop.selected_slot;
        xSemaphoreGive(g_loop.mutex);
    }

    if (selected_slot >= LOOP_SLOT_COUNT) {
        return -1;
    }

    loop_clear_file();

    if (loop_probe_slot_file(selected_slot, &has_file_after, &event_count, &loop_length_ticks) != FR_OK) {
        return -1;
    }

    return (has_file_after == 0U) ? 0 : -1;
}

uint8_t loop_is_running(void)
{
    return g_loop.running;
}

void loop_get_ui_state(loop_ui_state_t *state)
{
    if (state == NULL) {
        return;
    }

    memset(state, 0, sizeof(*state));

    if (g_loop.mutex == NULL) {
        return;
    }

    if (xSemaphoreTake(g_loop.mutex, pdMS_TO_TICKS(50)) != pdTRUE) {
        return;
    }

    state->selected_slot = g_loop.selected_slot;
    state->recording = g_loop.recording;
    state->playing = g_loop.playing;
    state->record_slot = g_loop.record_slot;
    state->play_slot = g_loop.play_slot;
    memcpy(state->slot_has_file, g_loop.slot_has_file, sizeof(state->slot_has_file));
    memcpy(state->slot_event_count, g_loop.slot_event_count, sizeof(state->slot_event_count));
    memcpy(state->slot_length_ticks, g_loop.slot_length_ticks, sizeof(state->slot_length_ticks));

    xSemaphoreGive(g_loop.mutex);
}
