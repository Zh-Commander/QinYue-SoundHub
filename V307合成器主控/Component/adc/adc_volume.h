#ifndef __ADC_VOLUME_H
#define __ADC_VOLUME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ADC_VOLUME_TASK_PRIO       7
#define ADC_VOLUME_TASK_STACK      384

int adc_volume_init(void);
void adc_volume_deinit(void);
uint8_t adc_volume_is_running(void);
uint16_t adc_volume_get_q8(void);
uint16_t adc_volume_get_raw(uint8_t channel_index);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_VOLUME_H */
