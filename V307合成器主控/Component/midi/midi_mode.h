#ifndef __APP_MIDI_MODE_H__
#define __APP_MIDI_MODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    APP_MODE_FREE_PLAY = 0,
    APP_MODE_MIDI_KEYBOARD,
    APP_MODE_SWITCHING
} app_mode_t;

typedef struct {
    app_mode_t current_mode;
    uint8_t midi_ready;
    uint8_t switching;
    uint8_t last_note;
    uint8_t last_velocity;
    uint32_t note_on_count;
    uint32_t note_off_count;
    uint32_t send_error_count;
    char last_error[64];
} app_mode_status_t;

int app_mode_init(void);
app_mode_t app_mode_get(void);
const char *app_mode_get_name(app_mode_t mode);
const char *app_mode_get_last_error(void);
int app_mode_switch(app_mode_t target_mode);
uint8_t app_mode_is_loop_enabled(void);
uint8_t app_mode_is_midi_uart_running(void);
void app_mode_get_status(app_mode_status_t *status);

void MidiMode_Uart7RxIsr(void);
void MidiMode_Uart7DmaRxIsr(void);

#ifdef __cplusplus
}
#endif

#endif
