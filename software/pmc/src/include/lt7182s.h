#ifndef __LT7182S_H__
#define __LT7182S_H__

#include <sam.h>
#include <math.h>
#include "utils.h"
#include "systick.h"
#include "sercom.h"

#define LT7182S_I2C_ADDR 0x40

#define LT7182S_OPERATION_TURN_OFF_IMMED    0x00
#define LT7182S_OPERATION_ON                0x80
#define LT7182S_OPERATION_MARGIN_LOW        0x98
#define LT7182S_OPERATION_MARGIN_HIGH       0xA8
#define LT7182S_OPERATION_SEQUENCE_OFF      0x40

#define LT7182S_DISABLE_FLOAT 1

uint8_t lt7182s_init();

uint8_t lt7182s_read_mfr_id(char *pszBuffer, uint8_t ubBufferMaxSize);
uint8_t lt7182s_read_mfr_model(char *pszBuffer, uint8_t ubBufferMaxSize);
uint8_t lt7182s_read_mfr_revision(uint8_t *pubBuffer, uint8_t ubBufferMaxSize);
uint8_t lt7182s_read_mfr_serial(uint8_t *pubBuffer, uint8_t ubBufferMaxSize);
uint16_t lt7182s_read_mfr_special_id();

uint64_t lt7182s_mfr_fault_log_get_timestamp();
uint8_t lt7182s_mfr_fault_log_set_timestamp(uint64_t ullTime);
uint8_t lt7182s_mfr_fault_store();
uint8_t lt7182s_mfr_fault_clear();
uint8_t lt7182s_mfr_fault_log_read(/* ... */);

uint8_t lt7182s_get_status_byte(uint8_t ubChannel);
uint8_t lt7182s_clear_status_byte(uint8_t ubChannel, uint8_t ubMask);
uint16_t lt7182s_get_status_word(uint8_t ubChannel);
uint8_t lt7182s_clear_status_word(uint8_t ubChannel, uint16_t usMask);
uint8_t lt7182s_get_status_vout(uint8_t ubChannel);
uint8_t lt7182s_clear_status_vout(uint8_t ubChannel, uint8_t ubMask);
uint8_t lt7182s_get_status_iout(uint8_t ubChannel);
uint8_t lt7182s_clear_status_iout(uint8_t ubChannel, uint8_t ubMask);
uint8_t lt7182s_get_status_input(uint8_t ubChannel);
uint8_t lt7182s_clear_status_input(uint8_t ubChannel, uint8_t ubMask);
uint8_t lt7182s_get_status_temperature();
uint8_t lt7182s_clear_status_temperature(uint8_t ubMask);
uint8_t lt7182s_get_status_cml();
uint8_t lt7182s_clear_status_cml(uint8_t ubMask);
uint8_t lt7182s_get_status_mfr_specific(uint8_t ubChannel);
uint8_t lt7182s_clear_status_mfr_specific(uint8_t ubChannel, uint8_t ubMask);
uint8_t lt7182s_get_status_mfr_common();
uint16_t lt7182s_get_status_mfr_pads();
uint8_t lt7182s_get_status_mfr_pin_config();

uint8_t lt7182s_cleak_faults();
uint8_t lt7182s_cleak_peaks();

uint8_t lt7182s_get_state(uint8_t ubChannel);
uint8_t lt7182s_get_operation(uint8_t ubChannel);
uint8_t lt7182s_set_operation(uint8_t ubChannel, uint8_t ubOperation);
uint8_t lt7182s_get_on_off_config(uint8_t ubChannel);
uint8_t lt7182s_set_on_off_config(uint8_t ubChannel, uint8_t ubConfig);
uint16_t lt7182s_get_pwm_config(uint8_t ubChannel);
uint8_t lt7182s_set_pwm_config(uint8_t ubChannel, uint16_t usConfig);
#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_getf_pwm_phase(uint8_t ubChannel); // deg
uint8_t lt7182s_setf_pwm_phase(uint8_t ubChannel, float fPhase); // deg
#endif
uint32_t lt7182s_get_pwm_phase(uint8_t ubChannel); // m deg
uint8_t lt7182s_set_pwm_phase(uint8_t ubChannel, uint32_t ulPhase); // m deg

#ifndef LT7182S_DISABLE_FLOAT
float l17182s_pwm_config_parsef_gmea(uint16_t usPWMConfig); // uS
float l17182s_pwm_config_parsef_pos_ilim(uint16_t usPWMConfig); // A
float l17182s_pwm_config_parsef_neg_ilim(uint16_t usPWMConfig); // A
float l17182s_pwm_config_parsef_cith(uint16_t usPWMConfig); // pF
float l17182s_pwm_config_parsef_rith(uint16_t usPWMConfig); // kOhm
#endif
uint32_t l17182s_pwm_config_parse_gmea(uint16_t usPWMConfig); // nS
uint16_t l17182s_pwm_config_parse_pos_ilim(uint16_t usPWMConfig); // mA
int16_t l17182s_pwm_config_parse_neg_ilim(uint16_t usPWMConfig); // mA
uint8_t l17182s_pwm_config_parse_cith(uint16_t usPWMConfig); // pF
uint8_t l17182s_pwm_config_parse_rith(uint16_t usPWMConfig); // kOhm
uint8_t l17182s_pwm_config_parse_pulse_skipping(uint16_t usPWMConfig);
uint8_t l17182s_pwm_config_parse_fcm_at_toff(uint16_t usPWMConfig);
uint8_t l17182s_pwm_config_parse_low_vout_mode(uint16_t usPWMConfig);

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_vin(uint8_t ubChannel); // V
float lt7182s_readf_vin_peak(uint8_t ubChannel); // V
float lt7182s_getf_vin_on(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vin_on(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vin_off(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vin_off(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vin_uv_warn(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vin_uv_warn(uint8_t ubChannel, float fVoltage); // V
#endif
uint16_t lt7182s_read_vin(uint8_t ubChannel); // mV
uint16_t lt7182s_read_vin_peak(uint8_t ubChannel); // mV
uint16_t lt7182s_get_vin_on(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vin_on(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vin_off(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vin_off(uint8_t ubChannel, uint16_t usVoltage); // mV
int16_t lt7182s_get_vin_uv_warn(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vin_uv_warn(uint8_t ubChannel, int16_t sVoltage); // mV

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_iin(uint8_t ubChannel); // A
float lt7182s_getf_iin_oc_warn(uint8_t ubChannel); // A
uint8_t lt7182s_setf_iin_oc_warn(uint8_t ubChannel, float fCurrent); // A
#endif
uint16_t lt7182s_read_iin(uint8_t ubChannel); // mA
uint16_t lt7182s_get_iin_oc_warn(uint8_t ubChannel); // mA
uint8_t lt7182s_set_iin_oc_warn(uint8_t ubChannel, uint16_t usCurrent); // mA

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_vout(uint8_t ubChannel); // V
float lt7182s_readf_vout_peak(uint8_t ubChannel); // V
float lt7182s_getf_vout(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vout_max(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout_max(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vout_margin_high(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout_margin_high(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vout_margin_low(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout_margin_low(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vout_transition_rate(uint8_t ubChannel); // V/ms
uint8_t lt7182s_setf_vout_transition_rate(uint8_t ubChannel, float fRate); // V/ms
float lt7182s_getf_vout_ov_fault(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout_ov_fault(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vout_ov_warn(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout_ov_warn(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vout_uv_fault(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout_uv_fault(uint8_t ubChannel, float fVoltage); // V
float lt7182s_getf_vout_uv_warn(uint8_t ubChannel); // V
uint8_t lt7182s_setf_vout_uv_warn(uint8_t ubChannel, float fVoltage); // V
#endif
uint16_t lt7182s_read_vout(uint8_t ubChannel); // mV
uint16_t lt7182s_read_vout_peak(uint8_t ubChannel); // mV
uint16_t lt7182s_get_vout(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vout_max(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout_max(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vout_margin_high(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout_margin_high(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vout_margin_low(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout_margin_low(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vout_transition_rate(uint8_t ubChannel); // mV/ms
uint8_t lt7182s_set_vout_transition_rate(uint8_t ubChannel, uint16_t usRate); // mV/ms
uint16_t lt7182s_get_vout_ov_fault(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout_ov_fault(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vout_ov_warn(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout_ov_warn(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vout_uv_fault(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout_uv_fault(uint8_t ubChannel, uint16_t usVoltage); // mV
uint16_t lt7182s_get_vout_uv_warn(uint8_t ubChannel); // mV
uint8_t lt7182s_set_vout_uv_warn(uint8_t ubChannel, uint16_t usVoltage); // mV

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_iout(uint8_t ubChannel); // A
float lt7182s_readf_iout_peak(uint8_t ubChannel); // A
float lt7182s_getf_iout_oc_warn(uint8_t ubChannel); // A
uint8_t lt7182s_setf_iout_oc_warn(uint8_t ubChannel, float fCurrent); // A
#endif
uint16_t lt7182s_read_iout(uint8_t ubChannel); // mA
uint16_t lt7182s_read_iout_peak(uint8_t ubChannel); // mA
uint16_t lt7182s_get_iout_oc_warn(uint8_t ubChannel); // mA
uint8_t lt7182s_set_iout_oc_warn(uint8_t ubChannel, uint16_t usCurrent); // mA

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_freq(uint8_t ubChannel); // kHz
float lt7182s_readf_pout(uint8_t ubChannel); // W
float lt7182s_readf_ith(uint8_t ubChannel); // V
#endif
uint32_t lt7182s_read_freq(uint8_t ubChannel); // Hz
uint16_t lt7182s_read_pout(uint8_t ubChannel); // mW
uint16_t lt7182s_read_ith(uint8_t ubChannel); // mV

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_extvcc(); // V
float lt7182s_readf_temperature(); // degC
float lt7182s_readf_temperature_peak(); // degC
float lt7182s_getf_ot_fault(); // degC
uint8_t lt7182s_setf_ot_fault(float fTemperature); // degC
float lt7182s_getf_ot_warn(); // degC
uint8_t lt7182s_setf_ot_warn(float fTemperature); // degC
float lt7182s_getf_freq(); // kHz
uint8_t lt7182s_setf_freq(float fFrequency); // kHz
#endif
uint16_t lt7182s_read_extvcc(); // mV
int32_t lt7182s_read_temperature(); // m degC
int32_t lt7182s_read_temperature_peak(); // m degC
int32_t lt7182s_get_ot_fault(); // m degC
uint8_t lt7182s_set_ot_fault(int32_t lTemperature); // m degC
int32_t lt7182s_get_ot_warn(); // m degC
uint8_t lt7182s_set_ot_warn(int32_t lTemperature); // m degC
uint32_t lt7182s_get_freq(); // Hz
uint8_t lt7182s_set_freq(uint32_t ulFrequency); // Hz

#endif  // __LT7182S_H__