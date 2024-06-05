#include "admv1013.h"

static uint16_t admv1013_read_register(uint8_t ubRegister)
{
    uint8_t ubData[3];

    ubData[0] = BIT(7) | ((ubRegister & 0x3F) << 1);
    ubData[1] = 0x00;
    ubData[2] = 0x00;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TX_SELECT();

        sercom0_spi_transfer(ubData, 3, ubData);

        TX_UNSELECT();
    }

    return ((uint16_t)ubData[2] >> 1) | ((uint16_t)ubData[1] << 7) | ((uint16_t)ubData[0] << 15);
}
static void admv1013_write_register(uint8_t ubRegister, uint16_t usValue)
{
    uint8_t ubData[3];

    ubData[0] = ((ubRegister & 0x3F) << 1) | ((usValue & 0x8000) >> 15);
    ubData[1] = (usValue & 0x7F80) >> 7;
    ubData[2] = (usValue & 0x007F) << 1;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TX_SELECT();

        sercom0_spi_write(ubData, 3, 1);

        TX_UNSELECT();
    }
}
static void admv1013_rmw_register(uint8_t ubRegister, uint16_t usMask, uint16_t usValue)
{
    admv1013_write_register(ubRegister, (admv1013_read_register(ubRegister) & usMask) | usValue);
}

uint8_t admv1013_init()
{
    admv1013_reset();

    if(admv1013_get_chip_id() != 0x0A)
        return 0;

    // Test SPI write and reset
    uint16_t usValue = admv1013_read_register(ADMV1013_REG_VVA_TEMP_COMP);

    admv1013_write_register(ADMV1013_REG_VVA_TEMP_COMP, 0xE700);

    if(admv1013_read_register(ADMV1013_REG_VVA_TEMP_COMP) != 0xE700)
        return 0;

    admv1013_reset();

    if(admv1013_read_register(ADMV1013_REG_VVA_TEMP_COMP) != usValue)
        return 0;

    admv1013_write_register(ADMV1013_REG_VVA_TEMP_COMP, 0xE700); // DS recommended value
    admv1013_rmw_register(ADMV1013_REG_QUAD, ~0x03C0, ADMV1013_REG_QUAD_QUAD_SE_MODE_SE_NEG_DIS); // Single-ended positive LO
    admv1013_rmw_register(ADMV1013_REG_ENABLE, ~(ADMV1013_REG_ENABLE_MIXER_IF_EN | ADMV1013_REG_ENABLE_DET_EN), ADMV1013_REG_ENABLE_MIXER_IF_EN | ADMV1013_REG_ENABLE_DET_EN); // Enable mixer IF and detector

    return 1;
}
void admv1013_power_down()
{
    admv1013_rmw_register(ADMV1013_REG_ENABLE, (uint16_t)~ADMV1013_REG_ENABLE_BG_PD, ADMV1013_REG_ENABLE_BG_PD);
}
void admv1013_power_up()
{
    admv1013_rmw_register(ADMV1013_REG_ENABLE, (uint16_t)~ADMV1013_REG_ENABLE_BG_PD, 0);
}
void admv1013_reset()
{
    // Hardware reset
    TX_RESET();
    delay_ms(10);
    TX_UNRESET();
    delay_ms(10);

    // Software reset
    // admv1013_rmw_register(ADMV1013_REG_SPI_CONTROL, (uint16_t)~ADMV1013_REG_SPI_CONTROL_SPI_SOFT_RESET, ADMV1013_REG_SPI_CONTROL_SPI_SOFT_RESET);
    // admv1013_rmw_register(ADMV1013_REG_SPI_CONTROL, (uint16_t)~ADMV1013_REG_SPI_CONTROL_SPI_SOFT_RESET, 0);
}
uint8_t admv1013_get_chip_id()
{
    return (admv1013_read_register(ADMV1013_REG_SPI_CONTROL) >> 4) & 0xFF;
}
uint8_t admv1013_get_revision()
{
    return admv1013_read_register(ADMV1013_REG_SPI_CONTROL) & 0x0F;
}
void admv1013_update_lo_filters(uint64_t ullFreq)
{
    if(ullFreq < 5400000000ULL || ullFreq > 10250000000ULL)
        return;

    uint16_t usValue;

    if(ullFreq >= 5400000000ULL && ullFreq <= 7000000000ULL)
        usValue = ADMV1013_REG_QUAD_QUAD_FILTERS_5G4_7G0;
    else if(ullFreq >= 5400000000ULL && ullFreq <= 8000000000ULL)
        usValue = ADMV1013_REG_QUAD_QUAD_FILTERS_5G4_8G0;
    else if(ullFreq >= 6600000000ULL && ullFreq <= 9200000000ULL)
        usValue = ADMV1013_REG_QUAD_QUAD_FILTERS_6G6_9G2;
    else
        usValue = ADMV1013_REG_QUAD_QUAD_FILTERS_8G62_10G25;

    admv1013_rmw_register(ADMV1013_REG_QUAD, ~0x000F, usValue);
}
void admv1013_set_mixer_vgate(uint32_t ulValueUv)
{
    uint16_t usValue;

    if(ulValueUv <= 1800000)
        usValue = (2389ULL * (uint64_t)ulValueUv / 1000000ULL + 8100) / 100;
    else if(ulValueUv > 1800000 && ulValueUv <= 2600000)
        usValue = (2375ULL * (uint64_t)ulValueUv / 1000000ULL + 125) / 100;
    else
        return;

    admv1013_rmw_register(ADMV1013_REG_LO_AMP_I, ~0x007F, usValue & 0x007F);
}
void admv1013_set_i_phase(uint8_t ubValue)
{
    admv1013_rmw_register(ADMV1013_REG_LO_AMP_I, ~0x3F80, ((uint16_t)ubValue << 7) & 0x3F80);
}
uint8_t admv1013_get_i_phase()
{
    return (admv1013_read_register(ADMV1013_REG_LO_AMP_I) >> 7) & 0x7F;
}
void admv1013_set_q_phase(uint8_t ubValue)
{
    admv1013_rmw_register(ADMV1013_REG_LO_AMP_Q, ~0x3F80, ((uint16_t)ubValue << 7) & 0x3F80);
}
uint8_t admv1013_get_q_phase()
{
    return (admv1013_read_register(ADMV1013_REG_LO_AMP_Q) >> 7) & 0x7F;
}
void admv1013_set_i_offset(uint8_t ubPosValue, uint8_t ubNegValue)
{
    uint16_t usValue = ((uint16_t)(ubPosValue & 0x7F) << 9) | ((uint16_t)(ubNegValue & 0x7F) << 2);

    admv1013_rmw_register(ADMV1013_REG_OFFSET_ADJUST_I, (uint16_t)~0xFFFC, usValue);
}
void admv1013_get_i_offset(uint8_t *pubPosValue, uint8_t *pubNegValue)
{
    if(!pubPosValue && !pubNegValue)
        return;

    uint16_t usValue = admv1013_read_register(ADMV1013_REG_OFFSET_ADJUST_I);

    if(pubPosValue)
        *pubPosValue = (usValue >> 9) & 0x7F;

    if(pubNegValue)
        *pubNegValue = (usValue >> 2) & 0x7F;
}
void admv1013_set_q_offset(uint8_t ubPosValue, uint8_t ubNegValue)
{
    uint16_t usValue = ((uint16_t)(ubPosValue & 0x7F) << 9) | ((uint16_t)(ubNegValue & 0x7F) << 2);

    admv1013_rmw_register(ADMV1013_REG_OFFSET_ADJUST_Q, (uint16_t)~0xFFFC, usValue);
}
void admv1013_get_q_offset(uint8_t *pubPosValue, uint8_t *pubNegValue)
{
    if(!pubPosValue && !pubNegValue)
        return;

    uint16_t usValue = admv1013_read_register(ADMV1013_REG_OFFSET_ADJUST_Q);

    if(pubPosValue)
        *pubPosValue = (usValue >> 9) & 0x7F;

    if(pubNegValue)
        *pubNegValue = (usValue >> 2) & 0x7F;
}