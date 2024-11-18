#ifndef __ADC_H__
#define __ADC_H__

#include <em_device.h>
#include "cmu.h"

#define ADC_RX_PWR_VDET_DIV     1.f // Voltage divider ratio and/or Amp gain
#define ADC_TX_PWR_VDET_DIV     0.1f // Voltage divider ratio and/or Amp gain
#define ADC_RX_LO_VPWR_DIV      (1.f / 3.f) // Voltage divider ratio and/or Amp gain
#define ADC_TX_LO_VPWR_DIV      (1.f / 3.f) // Voltage divider ratio and/or Amp gain

#define ADC_RX_PWR_VDET_CHAN    ADC_SINGLECTRL_POSSEL_APORT0XCH0
#define ADC_TX_PWR_VDET_CHAN    ADC_SINGLECTRL_POSSEL_APORT0XCH1
#define ADC_RX_LO_VPWR_CHAN     ADC_SINGLECTRL_POSSEL_APORT0XCH2
#define ADC_TX_LO_VPWR_CHAN     ADC_SINGLECTRL_POSSEL_APORT0XCH3

void adc_init();

float adc_get_avdd();
float adc_get_dvdd();
float adc_get_iovdd();
float adc_get_corevdd();

float adc_get_temperature();

float adc_get_rx_pwr_vdet();
float adc_get_tx_pwr_vdet();
float adc_get_rx_lo_vpwr();
float adc_get_tx_lo_vpwr();

#endif  // __ADC_H__
