#ifndef ADC1_H
#define ADC1_H
#include "adc_cfg.h"
#define ADC1_INSTANCE_ID ADC_ADC1_INSTANCE_ID
#include <stdint.h>
// module init
void adc1_init(void);

void adc1_deinit(void);

uint32_t adc1_execute_convention(void);
#endif