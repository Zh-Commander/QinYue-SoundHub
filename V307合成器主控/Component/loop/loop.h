/**
 * @file    loop.h
 * @brief   录制回放事件模块
 */

#ifndef __LOOP_H
#define __LOOP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define LOOP_EVENT_DIR_PATH         "1:/loops"
#define LOOP_EVENT_FILE_PATH        "1:/loops/loop1.mid"
#define LOOP_FILE_MAGIC             0x504F4F4CUL
#define LOOP_FILE_VERSION           1
#define LOOP_SLOT_COUNT             8U

typedef enum {
    LOOP_EVENT_NOTE_ON = 1,
    LOOP_EVENT_NOTE_OFF = 2,
} loop_event_type_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint32_t loop_length_ticks;
    uint32_t event_count;
} loop_file_header_t;

typedef struct {
    uint32_t tick;
    uint8_t type;
    uint8_t key;
    uint8_t velocity;
    uint8_t reserved;
} loop_event_t;

typedef struct {
    uint8_t selected_slot;
    uint8_t recording;
    uint8_t playing;
    uint8_t record_slot;
    uint8_t play_slot;
    uint8_t slot_has_file[LOOP_SLOT_COUNT];
    uint32_t slot_event_count[LOOP_SLOT_COUNT];
    uint32_t slot_length_ticks[LOOP_SLOT_COUNT];
} loop_ui_state_t;

int loop_init(void);
void loop_deinit(void);
void loop_on_live_note_on(uint8_t key_index, uint8_t velocity);
void loop_on_live_note_off(uint8_t key_index);

int loop_select_slot(uint8_t slot_index);
int loop_ui_start_record(void);
int loop_ui_stop_record(void);
int loop_ui_start_playback(void);
int loop_ui_stop_playback(void);
int loop_ui_clear_selected_slot(void);
void loop_get_ui_state(loop_ui_state_t *state);
uint8_t loop_is_running(void);

#ifdef __cplusplus
}
#endif

#endif /* __LOOP_H */
