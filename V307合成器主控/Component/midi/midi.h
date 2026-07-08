#ifndef __APP_MIDI_H__
#define __APP_MIDI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MIDI_CHANNEL_1                  0U
#define MIDI_CHANNEL_10                 9U
#define MIDI_PROGRAM_ACOUSTIC_PIANO     0U
#define MIDI_PROGRAM_CHURCH_ORGAN       19U

int midi_init(void);
void midi_deinit(void);
uint8_t midi_is_ready(void);
uint8_t midi_map_velocity_127(uint8_t velocity_255);
int midi_send_program_change(uint8_t channel, uint8_t program);
int midi_send_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
int midi_send_note_off(uint8_t channel, uint8_t note);
int midi_send_all_notes_off(uint8_t channel);

typedef struct {
    uint8_t ready;
    uint8_t last_note;
    uint8_t last_velocity;
    uint32_t note_on_count;
    uint32_t note_off_count;
    uint32_t send_error_count;
} midi_runtime_state_t;

void midi_get_runtime_state(midi_runtime_state_t *state);

#ifdef __cplusplus
}
#endif

#endif
