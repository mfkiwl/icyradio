#ifndef __ADMV1014_H__
#define __ADMV1014_H__

#include <sam.h>
#include "port.h"
#include "sercom.h"
#include "atomic.h"
#include "systick.h"


#define ADMV1014_REG_SPI_CONTROL            0x00
#define ADMV1014_REG_ALARM                  0x01
#define ADMV1014_REG_ALARM_MASKS            0x02
#define ADMV1014_REG_ENABLE                 0x03
#define ADMV1014_REG_QUAD                   0x04
#define ADMV1014_REG_LO_AMP_PHASE_ADJUST1   0x05
#define ADMV1014_REG_MIXER                  0x07
#define ADMV1014_REG_IF_AMP                 0x08
#define ADMV1014_REG_IF_AMP_BB_AMP          0x09
#define ADMV1014_REG_BB_AMP_AGC             0x0A
#define ADMV1014_REG_VVA_TEMP_COMP          0x0B

// ADMV1014_REG_SPI_CONTROL
#define ADMV1014_REG_SPI_CONTROL_PARITY_EN          BIT(15)
#define ADMV1014_REG_SPI_CONTROL_SPI_SOFT_RESET     BIT(14)

// ADMV1014_REG_ALARM
#define ADMV1014_REG_ALARM_PARITY_ERROR             BIT(15)
#define ADMV1014_REG_ALARM_TOO_FEW_ERRORS           BIT(14)
#define ADMV1014_REG_ALARM_TOO_MANY_ERRORS          BIT(13)
#define ADMV1014_REG_ALARM_ADDRESS_RANGE_ERROR      BIT(12)

// ADMV1014_REG_ENABLE
#define ADMV1014_REG_ENABLE_IBIAS_PD               BIT(14)
#define ADMV1014_REG_ENABLE_P1DB_COMPENSATION      (BIT(13) | BIT(12))
#define ADMV1014_REG_ENABLE_IF_AMP_PD              BIT(11)
#define ADMV1014_REG_ENABLE_QUAD_BG_PD             BIT(9)
#define ADMV1014_REG_ENABLE_BB_AMP_PD              BIT(8)
#define ADMV1014_REG_ENABLE_QUAD_IBIAS_PD          BIT(7)
#define ADMV1014_REG_ENABLE_DET_EN                 BIT(6)
#define ADMV1014_REG_ENABLE_BG_PD                  BIT(5)

// ADMV1014_REG_QUAD
#define ADMV1014_REG_QUAD_QUAD_SE_MODE_SE_NEG_DIS  0x0180
#define ADMV1014_REG_QUAD_QUAD_SE_MODE_SE_POS_DIS  0x0240
#define ADMV1014_REG_QUAD_QUAD_SE_MODE_DIFF_DIS    0x0300
#define ADMV1014_REG_QUAD_QUAD_FILTERS_8G625_10G25 0x0000
#define ADMV1014_REG_QUAD_QUAD_FILTERS_6G6_9G2     0x0005
#define ADMV1014_REG_QUAD_QUAD_FILTERS_5G4_8G0     0x000A
#define ADMV1014_REG_QUAD_QUAD_FILTERS_5G4_7G0     0x000F

// ADMV1014_REG_MIXER
#define ADMV1014_REG_MIXER_DET_PROG_m12_p4         0x0000
#define ADMV1014_REG_MIXER_DET_PROG_m13_p3         BIT(0)
#define ADMV1014_REG_MIXER_DET_PROG_m14_p2         BIT(1)
#define ADMV1014_REG_MIXER_DET_PROG_m15_p1         BIT(2)
#define ADMV1014_REG_MIXER_DET_PROG_m15p5_p0p5     BIT(3)
#define ADMV1014_REG_MIXER_DET_PROG_m16p25_m0p25   BIT(4)
#define ADMV1014_REG_MIXER_DET_PROG_m17_m1         BIT(5)
#define ADMV1014_REG_MIXER_DET_PROG_m18_m2         BIT(6)


uint8_t admv1014_init();
void admv1014_power_down();
void admv1014_power_up();
void admv1014_reset();
uint8_t admv1014_get_chip_id();
uint8_t admv1014_get_revision();
void admv1014_update_lo_filters(uint64_t ullFreq);
void admv1014_set_det_prog(uint8_t ubValue);
uint8_t admv1014_get_det_prog();
int32_t admv1014_get_det_prog_range_min(uint8_t ubValue);
int32_t admv1014_get_det_prog_range_max(uint8_t ubValue);
void admv1014_set_i_phase(uint8_t ubValue);
uint8_t admv1014_get_i_phase();
void admv1014_set_q_phase(uint8_t ubValue);
uint8_t admv1014_get_q_phase();
void admv1014_set_if_i_gain(uint8_t ubValue);
uint8_t admv1014_get_if_i_gain();
void admv1014_set_if_q_gain(uint8_t ubValue);
uint8_t admv1014_get_if_q_gain();
void admv1014_set_bb_vcm(uint16_t usValueMv);
uint16_t admv1014_get_bb_vcm();
void admv1014_set_bb_i_offset(int8_t bValue);
int8_t admv1014_get_bb_i_offset();
void admv1014_set_bb_q_offset(int8_t bValue);
int8_t admv1014_get_bb_q_offset();
void admv1014_set_bb_gain(uint8_t ubValue);
uint8_t admv1014_get_bb_gain();

#endif  // __ADMV1014_H__