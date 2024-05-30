#ifndef __ADC_H__
#define __ADC_H__

#include <sam.h>
#include "pm.h"

#define ADC_DISABLE_FLOAT 1

void adc_init();

uint32_t adc_get_iovdd();
uint32_t adc_get_corevdd();

int32_t adc_get_temperature();

#ifndef ADC_DISABLE_FLOAT
float adc_getf_iovdd();
float adc_getf_corevdd();

float adc_getf_temperature();
#endif

#endif // __ADC_H__