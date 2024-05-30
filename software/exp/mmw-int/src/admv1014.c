#include "admv1014.h"

static const uint8_t pubIFAmpCoarseLUT[] = {15, 7, 3, 1, 0};
static const uint8_t pubMixerVGateLUT[] = {106, 107, 108, 110, 111, 112, 113, 114, 117, 118, 119, 120, 122, 123, 44, 45};

static uint16_t admv1014_read_register(uint8_t ubRegister)
{
    uint8_t ubData[3];

    ubData[0] = BIT(7) | ((ubRegister & 0x3F) << 1);
    ubData[1] = 0x00;
    ubData[2] = 0x00;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        RX_SELECT();

        sercom0_spi_transfer(ubData, 3, ubData);

        RX_UNSELECT();
    }

    return ((uint16_t)ubData[2] >> 1) | ((uint16_t)ubData[1] << 7) | ((uint16_t)ubData[0] << 15);
}
static void admv1014_write_register(uint8_t ubRegister, uint16_t usValue)
{
    uint8_t ubData[3];

    ubData[0] = ((ubRegister & 0x3F) << 1) | ((usValue & 0x8000) >> 15);
    ubData[1] = (usValue & 0x7F80) >> 7;
    ubData[2] = (usValue & 0x007F) << 1;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        RX_SELECT();

        sercom0_spi_write(ubData, 3, 1);

        RX_UNSELECT();
    }
}
static void admv1014_rmw_register(uint8_t ubRegister, uint16_t usMask, uint16_t usValue)
{
    admv1014_write_register(ubRegister, (admv1014_read_register(ubRegister) & usMask) | usValue);
}

uint8_t admv1014_init()
{
    admv1014_reset();

    if(admv1014_get_chip_id() != 0x09)
        return 0;

    admv1014_write_register(ADMV1014_REG_VVA_TEMP_COMP, 0x727C); // DS recommended value
    admv1014_rmw_register(ADMV1014_REG_QUAD, ~0x03C0, ADMV1014_REG_QUAD_QUAD_SE_MODE_SE_NEG_DIS); // Single-ended positive LO
    admv1014_rmw_register(ADMV1014_REG_ENABLE, ~(ADMV1014_REG_ENABLE_P1DB_COMPENSATION | ADMV1014_REG_ENABLE_IF_AMP_PD | ADMV1014_REG_ENABLE_BB_AMP_PD | ADMV1014_REG_ENABLE_DET_EN), ADMV1014_REG_ENABLE_P1DB_COMPENSATION | ADMV1014_REG_ENABLE_BB_AMP_PD | ADMV1014_REG_ENABLE_DET_EN); // Enable P1dB Comp., enable IF amp, disable BB amp, enable detector

    return 1;
}
void admv1014_power_down()
{
    uint16_t usValue = admv1014_read_register(ADMV1014_REG_ENABLE);

    usValue |= ADMV1014_REG_ENABLE_IBIAS_PD;
    // usValue |= ADMV1014_REG_ENABLE_IF_AMP_PD;
    usValue |= ADMV1014_REG_ENABLE_QUAD_BG_PD;
    // usValue |= ADMV1014_REG_ENABLE_BB_AMP_PD;
    usValue |= ADMV1014_REG_ENABLE_QUAD_IBIAS_PD;
    usValue |= ADMV1014_REG_ENABLE_BG_PD;

    admv1014_write_register(ADMV1014_REG_ENABLE, usValue);
}
void admv1014_power_up()
{
    uint16_t usValue = admv1014_read_register(ADMV1014_REG_ENABLE);

    usValue &= ~ADMV1014_REG_ENABLE_IBIAS_PD;
    // usValue &= ~ADMV1014_REG_ENABLE_IF_AMP_PD;
    usValue &= ~ADMV1014_REG_ENABLE_QUAD_BG_PD;
    // usValue &= ~ADMV1014_REG_ENABLE_BB_AMP_PD;
    usValue &= ~ADMV1014_REG_ENABLE_QUAD_IBIAS_PD;
    usValue &= ~ADMV1014_REG_ENABLE_BG_PD;

    admv1014_write_register(ADMV1014_REG_ENABLE, usValue);
}
void admv1014_reset()
{
    // Hardware reset
    RX_RESET();
    delay_ms(10);
    RX_UNRESET();
    delay_ms(10);

    // Software reset
    // admv1014_rmw_register(ADMV1014_REG_SPI_CONTROL, (uint16_t)~ADMV1014_REG_SPI_CONTROL_SPI_SOFT_RESET, ADMV1014_REG_SPI_CONTROL_SPI_SOFT_RESET);
    // admv1014_rmw_register(ADMV1014_REG_SPI_CONTROL, (uint16_t)~ADMV1014_REG_SPI_CONTROL_SPI_SOFT_RESET, 0);
}
uint8_t admv1014_get_chip_id()
{
    return (admv1014_read_register(ADMV1014_REG_SPI_CONTROL) >> 4) & 0xFF;
}
uint8_t admv1014_get_revision()
{
    return admv1014_read_register(ADMV1014_REG_SPI_CONTROL) & 0x0F;
}
void admv1014_update_lo_filters(uint64_t ullFreq)
{
    if(ullFreq < 5400000000ULL || ullFreq > 10250000000ULL)
        return;

    uint16_t usValue;

    if(ullFreq >= 5400000000ULL && ullFreq <= 7000000000ULL)
        usValue = ADMV1014_REG_QUAD_QUAD_FILTERS_5G4_7G0;
    else if(ullFreq >= 7000000000ULL && ullFreq <= 8000000000ULL)
        usValue = ADMV1014_REG_QUAD_QUAD_FILTERS_5G4_8G0;
    else if(ullFreq >= 8000000000ULL && ullFreq <= 9200000000ULL)
        usValue = ADMV1014_REG_QUAD_QUAD_FILTERS_6G6_9G2;
    else
        usValue = ADMV1014_REG_QUAD_QUAD_FILTERS_8G625_10G25;

    admv1014_rmw_register(ADMV1014_REG_QUAD, ~0x000F, usValue);
}
void admv1014_set_det_prog(uint8_t ubValue)
{
    if(ubValue > 7)
        ubValue = 7;

    ubValue = !ubValue ? 0 : BIT(ubValue - 1);

    admv1014_rmw_register(ADMV1014_REG_MIXER, ~0x007F, ubValue);
}
uint8_t admv1014_get_det_prog()
{
    uint8_t ubValue = admv1014_read_register(ADMV1014_REG_MIXER) & 0x7F;

    if(!ubValue)
        return 0;

    for(uint8_t i = 0; i < 7; i++)
    {
        if(ubValue & BIT(i))
            return i + 1;
    }

    return 0; // Invalid value ?
}
int32_t admv1014_get_det_prog_range_min(uint8_t ubValue)
{
    if(ubValue > 7)
        ubValue = 7;

    switch(ubValue)
    {
        case 0:
            return -12000;
        case 1:
            return -13000;
        case 2:
            return -14000;
        case 3:
            return -15000;
        case 4:
            return -15500;
        case 5:
            return -16250;
        case 6:
            return -17000;
        case 7:
            return -18000;
        default:
            return 0;
    }
}
int32_t admv1014_get_det_prog_range_max(uint8_t ubValue)
{
    if(ubValue > 7)
        ubValue = 7;

    switch(ubValue)
    {
        case 0:
            return 4000;
        case 1:
            return 3000;
        case 2:
            return 2000;
        case 3:
            return 1000;
        case 4:
            return 500;
        case 5:
            return -250;
        case 6:
            return -1000;
        case 7:
            return -2000;
        default:
            return 0;
    }
}
void admv1014_set_i_phase(uint8_t ubValue)
{
    admv1014_rmw_register(ADMV1014_REG_LO_AMP_PHASE_ADJUST1, (uint16_t)~0xFE00, ((uint16_t)ubValue << 9) & 0xFE00);
}
uint8_t admv1014_get_i_phase()
{
    return (admv1014_read_register(ADMV1014_REG_LO_AMP_PHASE_ADJUST1) >> 9) & 0x7F;
}
void admv1014_set_q_phase(uint8_t ubValue)
{
    admv1014_rmw_register(ADMV1014_REG_LO_AMP_PHASE_ADJUST1, ~0x01FC, ((uint16_t)ubValue << 2) & 0x01FC);
}
uint8_t admv1014_get_q_phase()
{
    return (admv1014_read_register(ADMV1014_REG_LO_AMP_PHASE_ADJUST1) >> 2) & 0x7F;
}
void admv1014_set_if_i_gain(uint8_t ubValue)
{
    if(ubValue > 54)
        ubValue = 54;

    // Map values from 0-54 to 5 coarse steps, each with 11 fine steps
    uint8_t ubCoarse = pubIFAmpCoarseLUT[ubValue / 11];
    uint8_t ubFine = 10 - (ubValue % 11);

    admv1014_rmw_register(ADMV1014_REG_IF_AMP, ~0x0F0F, ((uint16_t)ubCoarse << 8) | ubFine);
}
uint8_t admv1014_get_if_i_gain()
{
    uint16_t usReg = admv1014_read_register(ADMV1014_REG_IF_AMP);

    uint8_t ubFine = usReg & 0x0F;

    if(ubFine > 10)
        ubFine = 10;

    for(uint8_t i = 0; i < 5; i++)
    {
        if(pubIFAmpCoarseLUT[i] == ((usReg >> 8) & 0x0F))
            return i * 11 + (10 - ubFine);
    }

    return 0; // Invalid coarse value
}
void admv1014_set_if_q_gain(uint8_t ubValue)
{
    if(ubValue > 54)
        ubValue = 54;

    // Map values from 0-54 to 5 coarse steps, each with 11 fine steps
    uint8_t ubCoarse = pubIFAmpCoarseLUT[ubValue / 11];
    uint8_t ubFine = 10 - (ubValue % 11);

    admv1014_rmw_register(ADMV1014_REG_IF_AMP, ~0x00F0, ubFine << 4);
    admv1014_rmw_register(ADMV1014_REG_IF_AMP_BB_AMP, (uint16_t)~0xF000, (uint16_t)ubCoarse << 12);
}
uint8_t admv1014_get_if_q_gain()
{
    uint16_t usReg = admv1014_read_register(ADMV1014_REG_IF_AMP);
    uint16_t usReg2 = admv1014_read_register(ADMV1014_REG_IF_AMP_BB_AMP);

    uint8_t ubFine = (usReg >> 4) & 0x0F;

    if(ubFine > 10)
        ubFine = 10;

    for(uint8_t i = 0; i < 5; i++)
    {
        if(pubIFAmpCoarseLUT[i] == ((usReg2 >> 12) & 0x0F))
            return i * 11 + (10 - ubFine);
    }

    return 0; // Invalid coarse value
}
void admv1014_set_bb_vcm(uint16_t usValueMv)
{
    // TODO:
}
uint16_t admv1014_get_bb_vcm()
{
    // TODO:
    return 0;
}
void admv1014_set_bb_i_offset(int8_t bValue)
{
    // TODO:
}
int8_t admv1014_get_bb_i_offset()
{
    // TODO:
    return 0;
}
void admv1014_set_bb_q_offset(int8_t bValue)
{
    // TODO:
}
int8_t admv1014_get_bb_q_offset()
{
    // TODO:
    return 0;
}
void admv1014_set_bb_gain(uint8_t ubValue)
{
    // TODO:
}
uint8_t admv1014_get_bb_gain()
{
    // TODO:
    return 0;
}