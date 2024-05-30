#ifndef __ADC_H__
#define __ADC_H__

#include <sam.h>
#include "pm.h"

#define ADC_TX_PWR_VDET_DIV_RF  (4700UL) // Feedback resistor value
#define ADC_TX_PWR_VDET_DIV_RI  (1000UL) // Input resistor value
#define ADC_TX_PWR_VDET_DIV_AV  ((float)ADC_TX_PWR_VDET_DIV_RF / (float)ADC_TX_PWR_VDET_DIV_RI)
#define ADC_RX_PWR_VDET_DIV_RT  (2000UL) // Top resistor value
#define ADC_RX_PWR_VDET_DIV_RB  (1000UL) // Bottom resistor value
#define ADC_RX_PWR_VDET_DIV     ((float)ADC_RX_PWR_VDET_DIV_RB / (float)(ADC_RX_PWR_VDET_DIV_RT + ADC_RX_PWR_VDET_DIV_RB))

#define ADC_TX_PWR_VDET_DIV_CHAN    0 // PA02
#define ADC_RX_PWR_VDET_DIV_CHAN    1 // PA03

#define ADC_DISABLE_FLOAT 1

void adc_init();

uint32_t adc_get_iovdd();
uint32_t adc_get_corevdd();

int32_t adc_get_temperature();

uint32_t adc_get_tx_pwr_vdet(); // uV
uint32_t adc_get_rx_pwr_vdet(); // uV

#ifndef ADC_DISABLE_FLOAT
float adc_getf_iovdd();
float adc_getf_corevdd();

float adc_getf_temperature();

float adc_getf_tx_pwr_vdet();
float adc_getf_rx_pwr_vdet();
#endif

#endif // __ADC_H__