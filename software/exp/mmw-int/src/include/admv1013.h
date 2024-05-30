#ifndef __ADMV1013_H__
#define __ADMV1013_H__

#include <sam.h>
#include "port.h"
#include "sercom.h"
#include "atomic.h"
#include "systick.h"


#define ADMV1013_REG_SPI_CONTROL        0x00
#define ADMV1013_REG_ALARM              0x01
#define ADMV1013_REG_ALARM_MASKS        0x02
#define ADMV1013_REG_ENABLE             0x03
#define ADMV1013_REG_LO_AMP_I           0x05
#define ADMV1013_REG_LO_AMP_Q           0x06
#define ADMV1013_REG_OFFSET_ADJUST_I    0x07
#define ADMV1013_REG_OFFSET_ADJUST_Q    0x08
#define ADMV1013_REG_QUAD               0x09
#define ADMV1013_REG_VVA_TEMP_COMP      0x0A

// ADMV1013_REG_SPI_CONTROL
#define ADMV1013_REG_SPI_CONTROL_PARITY_EN          BIT(15)
#define ADMV1013_REG_SPI_CONTROL_SPI_SOFT_RESET     BIT(14)

// ADMV1013_REG_ALARM
#define ADMV1013_REG_ALARM_PARITY_ERROR             BIT(15)
#define ADMV1013_REG_ALARM_TOO_FEW_ERRORS           BIT(14)
#define ADMV1013_REG_ALARM_TOO_MANY_ERRORS          BIT(13)
#define ADMV1013_REG_ALARM_ADDRESS_RANGE_ERROR      BIT(12)

// ADMV1013_REG_ENABLE
#define ADMV1013_REG_ENABLE_VGA_PD              BIT(15)
#define ADMV1013_REG_ENABLE_MIXER_PD            BIT(14)
#define ADMV1013_REG_ENABLE_QUAD_PD             (BIT(13) | BIT(12) | BIT(11))
#define ADMV1013_REG_ENABLE_BG_PD               BIT(10)
#define ADMV1013_REG_ENABLE_MIXER_IF_EN         BIT(7)
#define ADMV1013_REG_ENABLE_DET_EN              BIT(5)

// ADMV1013_REG_QUAD
#define ADMV1013_REG_QUAD_QUAD_SE_MODE_SE_NEG_DIS  0x0180
#define ADMV1013_REG_QUAD_QUAD_SE_MODE_SE_POS_DIS  0x0240
#define ADMV1013_REG_QUAD_QUAD_SE_MODE_DIFF_DIS    0x0300
#define ADMV1013_REG_QUAD_QUAD_FILTERS_8G62_10G25  0x0000
#define ADMV1013_REG_QUAD_QUAD_FILTERS_6G6_9G2     0x0005
#define ADMV1013_REG_QUAD_QUAD_FILTERS_5G4_8G0     0x000A
#define ADMV1013_REG_QUAD_QUAD_FILTERS_5G4_7G0     0x000F


uint8_t admv1013_init();
void admv1013_power_down();
void admv1013_power_up();
void admv1013_reset();
uint8_t admv1013_get_chip_id();
uint8_t admv1013_get_revision();
void admv1013_update_lo_filters(uint64_t ullFreq);
void admv1013_set_mixer_vgate(uint32_t ulValueUv);
void admv1013_set_i_phase(uint8_t ubValue);
uint8_t admv1013_get_i_phase();
void admv1013_set_q_phase(uint8_t ubValue);
uint8_t admv1013_get_q_phase();
void admv1013_set_i_offset(uint8_t ubPosValue, uint8_t ubNegValue);
void admv1013_get_i_offset(uint8_t *pubPosValue, uint8_t *pubNegValue);
void admv1013_set_q_offset(uint8_t ubPosValue, uint8_t ubNegValue);
void admv1013_get_q_offset(uint8_t *pubPosValue, uint8_t *pubNegValue);

#endif  // __ADMV1013_H__