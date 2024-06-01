#include "lt7182s.h"

#ifndef LT7182S_DISABLE_FLOAT
static float lt7182s_pow2f(int8_t bExp)
{
    switch(bExp)
    {
        case -16:
            return 0.0000152587890625f;
        case -15:
            return 0.000030517578125f;
        case -14:
            return 0.00006103515625f;
        case -13:
            return 0.0001220703125f;
        case -12:
            return 0.000244140625f;
        case -11:
            return 0.00048828125f;
        case -10:
            return 0.0009765625f;
        case -9:
            return 0.001953125f;
        case -8:
            return 0.00390625f;
        case -7:
            return 0.0078125f;
        case -6:
            return 0.015625f;
        case -5:
            return 0.03125f;
        case -4:
            return 0.0625f;
        case -3:
            return 0.125f;
        case -2:
            return 0.25f;
        case -1:
            return 0.5f;
        case 0:
            return 1.f;
        case 1:
            return 2.f;
        case 2:
            return 4.f;
        case 3:
            return 8.f;
        case 4:
            return 16.f;
        case 5:
            return 32.f;
        case 6:
            return 64.f;
        case 7:
            return 128.f;
        case 8:
            return 256.f;
        case 9:
            return 512.f;
        case 10:
            return 1024.f;
        case 11:
            return 2048.f;
        case 12:
            return 4096.f;
        case 13:
            return 8192.f;
        case 14:
            return 16384.f;
        case 15:
            return 32768.f;
    }

    return 0.f;
}
static float lt7182s_from_l11f(uint16_t usData)
{
    int8_t bExp = (usData >> 11) & 0xF;

    if(usData & BIT(15))
        bExp = -1 * (bExp ^ 0xF) - 1;

    int16_t sMant = usData & 0x3FF;

    if(usData & BIT(10))
        sMant = -1 * (sMant ^ 0x3FF) - 1;

    return (float)sMant * lt7182s_pow2f(bExp);
}
#endif
static int64_t lt7182s_from_l11ll1e9(uint16_t usData)
{
    uint8_t ubExp = (usData >> 11) & 0xF;
    uint16_t usMant = usData & 0x3FF;
    int8_t bSign;
    uint64_t ullVal;

    if(usData & BIT(10))
    {
        bSign = -1;
        ullVal = (uint64_t)((usMant ^ 0x3FF) + 1) * 1000000000ULL;
    }
    else
    {
        bSign = 1;
        ullVal = (uint64_t)usMant * 1000000000ULL;
    }

    if(usData & BIT(15))
    {
        ubExp = (ubExp ^ 0xF) + 1;

        if(ubExp > 1)
            ullVal >>= ubExp - 1; // Divide by two n-1 times

        if(ubExp)
        {
            ullVal++; // Round on the last division
            ullVal >>= 1; // Do the last division
        }
    }
    else
    {
        ullVal <<= ubExp;
    }

    return (int64_t)ullVal * bSign;
}
#ifndef LT7182S_DISABLE_FLOAT
static uint16_t lt7182s_to_l11f(float fData)
{
    int8_t bExp = -16;
    int32_t lMant = (int32_t)(fData / lt7182s_pow2f(bExp));

    // Search for an exponent that produces valid 11-bit mantissa
    do
    {
        if(lMant >= -1024 && lMant <= 1023)
            break;

        lMant = (int32_t)(fData / lt7182s_pow2f(++bExp));
    } while(bExp < 15);

    if(lMant < -1024 || lMant > 1023)
        return 0;

    return ((uint16_t)bExp << 11) | (lMant & 0x07FF);
}
#endif
static uint16_t lt7182s_to_l11ll1e9(int64_t llData)
{
    uint64_t ullData = llData < 0 ? (uint64_t)(-1 * llData) : (uint64_t)llData;
    int8_t bExp = 15;
    uint16_t usMant = DIV_ROUND(ullData >> 15, 1000000000ULL);

    // Search for an exponent that produces valid 11-bit mantissa
    do
    {
        if(usMant >= 512 && usMant < 1024)
            break;

        bExp--;

        if(bExp < 0)
            usMant = DIV_ROUND(ullData << -bExp, 1000000000ULL);
        else
            usMant = DIV_ROUND(ullData >> bExp, 1000000000ULL);
    } while(bExp >= -16);

    int16_t sMant = llData < 0 ? (-1 * (int16_t)usMant) : (int16_t)usMant;

    if(sMant < -1024 || sMant > 1023)
        return 0;

    return ((uint16_t)bExp << 11) | (sMant & 0x07FF);
}

#ifndef LT7182S_DISABLE_FLOAT
static float lt7182s_from_ul16f(uint16_t usData)
{
    return (float)usData / 4096.f;
}
#endif
static uint64_t lt7182s_from_ul16ull1e12(uint16_t usData)
{
    return (uint64_t)usData * 1000000000000ULL / 4096ULL;
}
#ifndef LT7182S_DISABLE_FLOAT
static uint16_t lt7182s_to_ul16f(float fData)
{
    if(fData < 0.f || fData > 16.f)
        return 0;

    return (uint16_t)(fData * 4096.f);
}
#endif
static uint16_t lt7182s_to_ul16ull1e12(uint64_t ullData)
{
    return (uint16_t)(ullData * 4096ULL / 1000000000000ULL);
}

static uint8_t lt7182s_pmbus_send_command(uint8_t ubCommand)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write_byte(LT7182S_I2C_ADDR, ubCommand, SERCOM_I2C_STOP))
            return 0;
    }

    return 1;
}
static uint8_t lt7182s_pmbus_write_byte(uint8_t ubCommand, uint8_t ubData)
{
    uint8_t ubTemp[2];

    ubTemp[0] = ubCommand;
    ubTemp[1] = ubData & 0xFF;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write(LT7182S_I2C_ADDR, ubTemp, 2, SERCOM_I2C_STOP))
            return 0;
    }

    return 1;
}
static uint8_t lt7182s_pmbus_write_word(uint8_t ubCommand, uint16_t usData)
{
    uint8_t ubTemp[3];

    ubTemp[0] = ubCommand;
    ubTemp[1] = usData & 0xFF;
    ubTemp[2] = usData >> 8;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write(LT7182S_I2C_ADDR, ubTemp, 3, SERCOM_I2C_STOP))
            return 0;
    }

    return 1;
}
static uint8_t lt7182s_pmbus_write_dword(uint8_t ubCommand, uint32_t ulData)
{
    uint8_t ubTemp[5];

    ubTemp[0] = ubCommand;
    ubTemp[1] = ulData & 0xFF;
    ubTemp[2] = (ulData >> 8) & 0xFF;
    ubTemp[3] = (ulData >> 16) & 0xFF;
    ubTemp[4] = ulData >> 24;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write(LT7182S_I2C_ADDR, ubTemp, 5, SERCOM_I2C_STOP))
            return 0;
    }

    return 1;
}
#ifndef LT7182S_DISABLE_FLOAT
static uint8_t lt7182s_pmbus_write_l11f(uint8_t ubCommand, float fData)
{
    return lt7182s_pmbus_write_word(ubCommand, lt7182s_to_l11f(fData));
}
static uint8_t lt7182s_pmbus_write_ul16f(uint8_t ubCommand, float fData)
{
    return lt7182s_pmbus_write_word(ubCommand, lt7182s_to_ul16f(fData));
}
#endif
static uint8_t lt7182s_pmbus_write_l11(uint8_t ubCommand, int64_t llData)
{
    return lt7182s_pmbus_write_word(ubCommand, lt7182s_to_l11ll1e9(llData));
}
static uint8_t lt7182s_pmbus_write_ul16(uint8_t ubCommand, uint64_t ullData)
{
    return lt7182s_pmbus_write_word(ubCommand, lt7182s_to_ul16ull1e12(ullData));
}
static uint8_t lt7182s_pmbus_read_byte(uint8_t ubCommand)
{
    uint8_t ubTemp;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write_byte(LT7182S_I2C_ADDR, ubCommand, SERCOM_I2C_RESTART))
            return 0;

        if(!sercom1_i2c_master_read(LT7182S_I2C_ADDR, &ubTemp, 1, SERCOM_I2C_STOP))
            return 0;
    }

    return ubTemp;
}
static uint16_t lt7182s_pmbus_read_word(uint8_t ubCommand)
{
    uint8_t ubTemp[2];

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write_byte(LT7182S_I2C_ADDR, ubCommand, SERCOM_I2C_RESTART))
            return 0;

        if(!sercom1_i2c_master_read(LT7182S_I2C_ADDR, ubTemp, 2, SERCOM_I2C_STOP))
            return 0;
    }

    return (((uint16_t)ubTemp[1]) << 8) | ubTemp[0];
}
static uint32_t lt7182s_pmbus_read_dword(uint8_t ubCommand)
{
    uint8_t ubTemp[4];

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write_byte(LT7182S_I2C_ADDR, ubCommand, SERCOM_I2C_RESTART))
            return 0;

        if(!sercom1_i2c_master_read(LT7182S_I2C_ADDR, ubTemp, 4, SERCOM_I2C_STOP))
            return 0;
    }

    return (((uint32_t)ubTemp[3]) << 24) | (((uint32_t)ubTemp[2]) << 16) | (((uint32_t)ubTemp[1]) << 8) | ubTemp[0];
}
#ifndef LT7182S_DISABLE_FLOAT
static float lt7182s_pmbus_read_l11f(uint8_t ubCommand)
{
    return lt7182s_from_l11f(lt7182s_pmbus_read_word(ubCommand));
}
static float lt7182s_pmbus_read_ul16f(uint8_t ubCommand)
{
    return lt7182s_from_ul16f(lt7182s_pmbus_read_word(ubCommand));
}
#endif
static int64_t lt7182s_pmbus_read_l11(uint8_t ubCommand)
{
    return lt7182s_from_l11ll1e9(lt7182s_pmbus_read_word(ubCommand));
}
static uint64_t lt7182s_pmbus_read_ul16(uint8_t ubCommand)
{
    return lt7182s_from_ul16ull1e12(lt7182s_pmbus_read_word(ubCommand));
}
static uint8_t lt7182s_pmbus_read_block(uint8_t ubCommand, uint8_t *pubBuffer, uint8_t ubBufferMaxSize)
{
    uint8_t ubTemp[256]; // Max PMBus block read size

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if(!sercom1_i2c_master_write_byte(LT7182S_I2C_ADDR, ubCommand, SERCOM_I2C_RESTART))
            return 0;

        if(!sercom1_i2c_master_read(LT7182S_I2C_ADDR, ubTemp, ubBufferMaxSize + 1, SERCOM_I2C_STOP))
            return 0;
    }

    uint8_t ubSize = ubTemp[0];

    if(ubSize > ubBufferMaxSize)
        ubSize = ubBufferMaxSize;

    memcpy(pubBuffer, ubTemp + 1, ubSize);

    return ubSize;
}
static uint8_t lt7182s_pmbus_read_string(uint8_t ubCommand, char *pszBuffer, uint8_t ubBufferMaxSize)
{
    uint8_t ubSize = lt7182s_pmbus_read_block(ubCommand, (uint8_t *)pszBuffer, ubBufferMaxSize - 1);

    pszBuffer[ubSize] = 0x00;

    return 1;
}

static uint8_t lt7182s_select_page(uint8_t ubPage)
{
    if(ubPage > 1)
        return 0;

    return lt7182s_pmbus_write_byte(0x00, ubPage);
}

uint8_t lt7182s_init()
{
    if(!sercom1_i2c_master_write(LT7182S_I2C_ADDR, NULL, 0, SERCOM_I2C_STOP)) // Check ACK from the expected address
        return 0;

    if(!lt7182s_pmbus_send_command(0xFD)) // Reset
        return 0;

    delay_ms(50);

    if(lt7182s_read_mfr_special_id() != 0x1C1D)
        return 0;

    if(lt7182s_get_status_mfr_common() & BIT(0)) // Device is Write Protected by hardware pin
        return 0;

    if(!lt7182s_pmbus_write_word(0xD1, BIT(7) | BIT(1))) // Disable IEEE half-precision floats (use L11 and UL16 instead). Enable clock stretching.
        return 0;

    if(!lt7182s_pmbus_write_byte(0xD8, BIT(0))) // Enable debug telemetry
        return 0;

    return 1;
}

uint8_t lt7182s_read_mfr_id(char *pszBuffer, uint8_t ubBufferMaxSize)
{
    return lt7182s_pmbus_read_string(0x99, pszBuffer, ubBufferMaxSize);
}
uint8_t lt7182s_read_mfr_model(char *pszBuffer, uint8_t ubBufferMaxSize)
{
    return lt7182s_pmbus_read_string(0x9A, pszBuffer, ubBufferMaxSize);
}
uint8_t lt7182s_read_mfr_revision(uint8_t *pubBuffer, uint8_t ubBufferMaxSize)
{
    return lt7182s_pmbus_read_block(0x9B, pubBuffer, ubBufferMaxSize);
}
uint8_t lt7182s_read_mfr_serial(uint8_t *pubBuffer, uint8_t ubBufferMaxSize)
{
    return lt7182s_pmbus_read_block(0x9E, pubBuffer, ubBufferMaxSize);
}
uint16_t lt7182s_read_mfr_special_id()
{
    return lt7182s_pmbus_read_word(0xE7);
}

uint64_t lt7182s_mfr_fault_log_get_timestamp()
{
    uint64_t ullTime = lt7182s_pmbus_read_dword(0xE8);

    ullTime = (ullTime << 32) | lt7182s_pmbus_read_dword(0xE9);

    return ullTime;
}
uint8_t lt7182s_mfr_fault_log_set_timestamp(uint64_t ullTime)
{
    if(!lt7182s_pmbus_write_dword(0xE8, ullTime >> 32))
        return 0;

    return lt7182s_pmbus_write_dword(0xE9, ullTime & 0xFFFFFFFF);
}
uint8_t lt7182s_mfr_fault_store()
{
    return lt7182s_pmbus_send_command(0xEA);
}
uint8_t lt7182s_mfr_fault_clear()
{
    return lt7182s_pmbus_send_command(0xEC);
}
uint8_t lt7182s_mfr_fault_log_read(/* ... */)
{
    // TODO: Implement
}

uint8_t lt7182s_get_status_byte(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0x78);
}
uint8_t lt7182s_clear_status_byte(uint8_t ubChannel, uint8_t ubMask)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_byte(0x78, ubMask);
}
uint16_t lt7182s_get_status_word(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_word(0x79);
}
uint8_t lt7182s_clear_status_word(uint8_t ubChannel, uint16_t usMask)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_word(0x79, usMask);
}
uint8_t lt7182s_get_status_vout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0x7A);
}
uint8_t lt7182s_clear_status_vout(uint8_t ubChannel, uint8_t ubMask)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_byte(0x7A, ubMask);
}
uint8_t lt7182s_get_status_iout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0x7B);
}
uint8_t lt7182s_clear_status_iout(uint8_t ubChannel, uint8_t ubMask)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_byte(0x7B, ubMask);
}
uint8_t lt7182s_get_status_input(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0x7C);
}
uint8_t lt7182s_clear_status_input(uint8_t ubChannel, uint8_t ubMask)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_byte(0x7C, ubMask);
}
uint8_t lt7182s_get_status_temperature()
{
    return lt7182s_pmbus_read_byte(0x7D);
}
uint8_t lt7182s_clear_status_temperature(uint8_t ubMask)
{
    return lt7182s_pmbus_write_byte(0x7D, ubMask);
}
uint8_t lt7182s_get_status_cml()
{
    return lt7182s_pmbus_read_byte(0x7E);
}
uint8_t lt7182s_clear_status_cml(uint8_t ubMask)
{
    return lt7182s_pmbus_write_byte(0x7E, ubMask);
}
uint8_t lt7182s_get_status_mfr_specific(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0x80);
}
uint8_t lt7182s_clear_status_mfr_specific(uint8_t ubChannel, uint8_t ubMask)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_byte(0x80, ubMask);
}
uint8_t lt7182s_get_status_mfr_common()
{
    return lt7182s_pmbus_read_byte(0xEF);
}
uint16_t lt7182s_get_status_mfr_pads()
{
    return lt7182s_pmbus_read_word(0xE5);
}
uint8_t lt7182s_get_status_mfr_pin_config()
{
    return lt7182s_pmbus_read_byte(0xF7);
}

uint8_t lt7182s_cleak_faults()
{
    return lt7182s_pmbus_send_command(0x03);
}
uint8_t lt7182s_cleak_peaks()
{
    return lt7182s_pmbus_send_command(0xE3);
}

uint8_t lt7182s_get_state(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0xF1);
}
uint8_t lt7182s_get_operation(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0x01);
}
uint8_t lt7182s_set_operation(uint8_t ubChannel, uint8_t ubOperation)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_byte(0x01, ubOperation);
}
uint8_t lt7182s_get_on_off_config(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_byte(0x02) & 0x0D;
}
uint8_t lt7182s_set_on_off_config(uint8_t ubChannel, uint8_t ubConfig)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_byte(0x02, (ubConfig & 0x0D) | 0x12);
}
uint16_t lt7182s_get_pwm_config(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_word(0xD4);
}
uint8_t lt7182s_set_pwm_config(uint8_t ubChannel, uint16_t usConfig)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_word(0xD4, usConfig & 0x3FFF);
}
#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_getf_pwm_phase(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_read_l11f(0xF5);
}
uint8_t lt7182s_setf_pwm_phase(uint8_t ubChannel, float fPhase)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    while(fPhase >= 360)
        fPhase -= 360;

    return lt7182s_pmbus_write_l11f(0xF5, fPhase);
}
#endif
uint32_t lt7182s_get_pwm_phase(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0xF5), 1000000ULL); // m deg
}
uint8_t lt7182s_set_pwm_phase(uint8_t ubChannel, uint32_t ulPhase)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    while(ulPhase >= 360000)
        ulPhase -= 360000;

    return lt7182s_pmbus_write_l11(0xF5, (uint64_t)ulPhase * 1000000ULL); // m deg
}

#ifndef LT7182S_DISABLE_FLOAT
float l17182s_pwm_config_parsef_gmea(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x3800)
    {
        case 0x0000:
            return (usPWMConfig & BIT(1)) ? 150.f : 37.5f;
        case 0x0800:
            return (usPWMConfig & BIT(1)) ? 300.f : 75.f;
        case 0x1000:
            return (usPWMConfig & BIT(1)) ? 450.f : 112.5f;
        case 0x1800:
            return (usPWMConfig & BIT(1)) ? 600.f : 150.f;
        case 0x2000:
            return (usPWMConfig & BIT(1)) ? 750.f : 187.5f;
        case 0x2800:
            return (usPWMConfig & BIT(1)) ? 900.f : 225.f;
        case 0x3000:
            return (usPWMConfig & BIT(1)) ? 1050.f : 262.5f;
        case 0x3800:
            return (usPWMConfig & BIT(1)) ? 1200.f : 300.f;
    }
}
float l17182s_pwm_config_parsef_pos_ilim(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x0600)
    {
        case 0x0000:
            return 3.f;
        case 0x0200:
            return 4.5f;
        case 0x0400:
            return 6.5f;
        case 0x0600:
            return 8.5f;
    }
}
float l17182s_pwm_config_parsef_neg_ilim(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x0600)
    {
        case 0x0000:
            return -2.3f;
        case 0x0200:
            return -3.4f;
        case 0x0400:
            return -4.f;
        case 0x0600:
            return -4.25f;
    }
}
float l17182s_pwm_config_parsef_cith(uint16_t usPWMConfig)
{
    return (((usPWMConfig & 0x01C0) >> 6) + 1) * 10.f;
}
float l17182s_pwm_config_parsef_rith(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x0038)
    {
        case 0x0000:
            return 5.f;
        case 0x0008:
            return 10.f;
        case 0x0010:
            return 20.f;
        case 0x0018:
            return 40.f;
        case 0x0020:
            return 60.f;
        case 0x0028:
            return 80.f;
        case 0x0030:
            return 100.f;
        case 0x0038:
            return 120.f;
    }
}
#endif
uint32_t l17182s_pwm_config_parse_gmea(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x3800)
    {
        case 0x0000:
            return (usPWMConfig & BIT(1)) ? 150000 : 37500;
        case 0x0800:
            return (usPWMConfig & BIT(1)) ? 300000 : 75000;
        case 0x1000:
            return (usPWMConfig & BIT(1)) ? 450000 : 112500;
        case 0x1800:
            return (usPWMConfig & BIT(1)) ? 600000 : 150000;
        case 0x2000:
            return (usPWMConfig & BIT(1)) ? 750000 : 187500;
        case 0x2800:
            return (usPWMConfig & BIT(1)) ? 900000 : 225000;
        case 0x3000:
            return (usPWMConfig & BIT(1)) ? 1050000 : 262500;
        case 0x3800:
            return (usPWMConfig & BIT(1)) ? 1200000 : 300000;
    }
}
uint16_t l17182s_pwm_config_parse_pos_ilim(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x0600)
    {
        case 0x0000:
            return 3000;
        case 0x0200:
            return 4500;
        case 0x0400:
            return 6500;
        case 0x0600:
            return 8500;
    }
}
int16_t l17182s_pwm_config_parse_neg_ilim(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x0600)
    {
        case 0x0000:
            return -2300;
        case 0x0200:
            return -3400;
        case 0x0400:
            return -4000;
        case 0x0600:
            return -4250;
    }
}
uint8_t l17182s_pwm_config_parse_cith(uint16_t usPWMConfig)
{
    return (((usPWMConfig & 0x01C0) >> 6) + 1) * 10;
}
uint8_t l17182s_pwm_config_parse_rith(uint16_t usPWMConfig)
{
    switch(usPWMConfig & 0x0038)
    {
        case 0x0000:
            return 5;
        case 0x0008:
            return 10;
        case 0x0010:
            return 20;
        case 0x0018:
            return 40;
        case 0x0020:
            return 60;
        case 0x0028:
            return 80;
        case 0x0030:
            return 100;
        case 0x0038:
            return 120;
    }
}
uint8_t l17182s_pwm_config_parse_pulse_skipping(uint16_t usPWMConfig)
{
    return !!(usPWMConfig & BIT(0));
}
uint8_t l17182s_pwm_config_parse_fcm_at_toff(uint16_t usPWMConfig)
{
    return !!(usPWMConfig & BIT(2));
}
uint8_t l17182s_pwm_config_parse_low_vout_mode(uint16_t usPWMConfig)
{
    return !!(usPWMConfig & BIT(1));
}

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_vin(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x88);
}
float lt7182s_readf_vin_peak(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0xDE);
}
float lt7182s_getf_vin_on(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x35);
}
uint8_t lt7182s_setf_vin_on(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11f(0x35, fVoltage);
}
float lt7182s_getf_vin_off(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x36);
}
uint8_t lt7182s_setf_vin_off(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11f(0x36, fVoltage);
}
float lt7182s_getf_vin_uv_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x58);
}
uint8_t lt7182s_setf_vin_uv_warn(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11f(0x58, fVoltage);
}
#endif
uint16_t lt7182s_read_vin(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x88), 1000000ULL); // mV
}
uint16_t lt7182s_read_vin_peak(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0xDE), 1000000ULL); // mV
}
uint16_t lt7182s_get_vin_on(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x35), 1000000ULL); // mV
}
uint8_t lt7182s_set_vin_on(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11(0x35, (uint64_t)usVoltage * 1000000ULL); // mV
}
uint16_t lt7182s_get_vin_off(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x36), 1000000ULL); // mV
}
uint8_t lt7182s_set_vin_off(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11(0x36, (uint64_t)usVoltage * 1000000ULL); // mV
}
int16_t lt7182s_get_vin_uv_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x58), 1000000LL); // mV
}
uint8_t lt7182s_set_vin_uv_warn(uint8_t ubChannel, int16_t sVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11(0x58, (int64_t)sVoltage * 1000000LL); // mV
}

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_iin(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x89);
}
float lt7182s_getf_iin_oc_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x5D);
}
uint8_t lt7182s_setf_iin_oc_warn(uint8_t ubChannel, float fCurrent)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11f(0x5D, fCurrent);
}
#endif
uint16_t lt7182s_read_iin(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x89), 1000000ULL); // mA
}
uint16_t lt7182s_get_iin_oc_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x5D), 1000000ULL); // mA
}
uint8_t lt7182s_set_iin_oc_warn(uint8_t ubChannel, uint16_t usCurrent)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11(0x5D, (uint64_t)usCurrent * 1000000ULL); // mA
}

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_vout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x8B);
}
float lt7182s_readf_vout_peak(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0xDD);
}
float lt7182s_getf_vout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x21);
}
uint8_t lt7182s_setf_vout(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x21, fVoltage);
}
float lt7182s_getf_vout_max(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x24);
}
uint8_t lt7182s_setf_vout_max(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x24, fVoltage);
}
float lt7182s_getf_vout_margin_high(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x25);
}
uint8_t lt7182s_setf_vout_margin_high(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x25, fVoltage);
}
float lt7182s_getf_vout_margin_low(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x26);
}
uint8_t lt7182s_setf_vout_margin_low(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x26, fVoltage);
}
float lt7182s_getf_vout_transition_rate(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x27);
}
uint8_t lt7182s_setf_vout_transition_rate(uint8_t ubChannel, float fRate)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11f(0x27, fRate);
}
float lt7182s_getf_vout_ov_fault(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x40);
}
uint8_t lt7182s_setf_vout_ov_fault(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x40, fVoltage);
}
float lt7182s_getf_vout_ov_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x42);
}
uint8_t lt7182s_setf_vout_ov_warn(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x42, fVoltage);
}
float lt7182s_getf_vout_uv_fault(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x44);
}
uint8_t lt7182s_setf_vout_uv_fault(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x44, fVoltage);
}
float lt7182s_getf_vout_uv_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_ul16f(0x43);
}
uint8_t lt7182s_setf_vout_uv_warn(uint8_t ubChannel, float fVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16f(0x43, fVoltage);
}
#endif
uint16_t lt7182s_read_vout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x8B), 1000000000ULL); // mV
}
uint16_t lt7182s_read_vout_peak(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0xDD), 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x21), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x21, (uint64_t)usVoltage * 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout_max(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x24), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout_max(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x24, (uint64_t)usVoltage * 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout_margin_high(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x25), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout_margin_high(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x25, (uint64_t)usVoltage * 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout_margin_low(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x26), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout_margin_low(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x26, (uint64_t)usVoltage * 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout_transition_rate(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x27), 1000000ULL); // mV/ms
}
uint8_t lt7182s_set_vout_transition_rate(uint8_t ubChannel, uint16_t usRate)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11(0x27, (uint64_t)usRate * 1000000ULL); // mV/ms
}
uint16_t lt7182s_get_vout_ov_fault(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x40), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout_ov_fault(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x40, (uint64_t)usVoltage * 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout_ov_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x42), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout_ov_warn(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x42, (uint64_t)usVoltage * 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout_uv_fault(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x44), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout_uv_fault(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x44, (uint64_t)usVoltage * 1000000000ULL); // mV
}
uint16_t lt7182s_get_vout_uv_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_ul16(0x43), 1000000000ULL); // mV
}
uint8_t lt7182s_set_vout_uv_warn(uint8_t ubChannel, uint16_t usVoltage)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_ul16(0x43, (uint64_t)usVoltage * 1000000000ULL); // mV
}

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_iout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x8C);
}
float lt7182s_readf_iout_peak(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0xD7);
}
float lt7182s_getf_iout_oc_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x4A);
}
uint8_t lt7182s_setf_iout_oc_warn(uint8_t ubChannel, float fCurrent)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11f(0x4A, fCurrent);
}
#endif
uint16_t lt7182s_read_iout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x8C), 1000000ULL); // mA
}
uint16_t lt7182s_read_iout_peak(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0xD7), 1000000ULL); // mA
}
uint16_t lt7182s_get_iout_oc_warn(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x4A), 1000000ULL); // mA
}
uint8_t lt7182s_set_iout_oc_warn(uint8_t ubChannel, uint16_t usCurrent)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return lt7182s_pmbus_write_l11(0x4A, (uint64_t)usCurrent * 1000000ULL); // mA
}

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_freq(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x95);
}
float lt7182s_readf_pout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0x96);
}
float lt7182s_readf_ith(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0.f;

    return lt7182s_pmbus_read_l11f(0xCE);
}
#endif
uint32_t lt7182s_read_freq(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x95), 1000000ULL); // Hz
}
uint16_t lt7182s_read_pout(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0x96), 1000000ULL); // mW
}
uint16_t lt7182s_read_ith(uint8_t ubChannel)
{
    if(!lt7182s_select_page(ubChannel))
        return 0;

    return DIV_ROUND(lt7182s_pmbus_read_l11(0xCE), 1000000ULL); // mV
}

#ifndef LT7182S_DISABLE_FLOAT
float lt7182s_readf_extvcc()
{
    return lt7182s_pmbus_read_l11f(0xCD);
}
float lt7182s_readf_temperature()
{
    return lt7182s_pmbus_read_l11f(0x8D);
}
float lt7182s_readf_temperature_peak()
{
    return lt7182s_pmbus_read_l11f(0xDF);
}
float lt7182s_getf_ot_fault()
{
    return lt7182s_pmbus_read_l11f(0x4F);
}
uint8_t lt7182s_setf_ot_fault(float fTemperature)
{
    return lt7182s_pmbus_write_l11f(0x4F, fTemperature);
}
float lt7182s_getf_ot_warn()
{
    return lt7182s_pmbus_read_l11f(0x51);
}
uint8_t lt7182s_setf_ot_warn(float fTemperature)
{
    return lt7182s_pmbus_write_l11f(0x51, fTemperature);
}
float lt7182s_getf_freq()
{
    return lt7182s_pmbus_read_l11f(0x33);
}
uint8_t lt7182s_setf_freq(float fFrequency)
{
    return lt7182s_pmbus_write_l11f(0x33, fFrequency);
}
#endif
uint16_t lt7182s_read_extvcc()
{
    return DIV_ROUND(lt7182s_pmbus_read_l11(0xCD), 1000000ULL); // mV
}
int32_t lt7182s_read_temperature()
{
    return DIV_ROUND(lt7182s_pmbus_read_l11(0x8D), 1000000LL); // m degC
}
int32_t lt7182s_read_temperature_peak()
{
    return DIV_ROUND(lt7182s_pmbus_read_l11(0xDF), 1000000LL); // m degC
}
int32_t lt7182s_get_ot_fault()
{
    return DIV_ROUND(lt7182s_pmbus_read_l11(0x4F), 1000000LL); // m degC
}
uint8_t lt7182s_set_ot_fault(int32_t lTemperature)
{
    return lt7182s_pmbus_write_l11(0x4F, (uint64_t)lTemperature * 1000000LL); // m degC
}
int32_t lt7182s_get_ot_warn()
{
    return DIV_ROUND(lt7182s_pmbus_read_l11(0x51), 1000000LL); // m degC
}
uint8_t lt7182s_set_ot_warn(int32_t lTemperature)
{
    return lt7182s_pmbus_write_l11(0x51, (uint64_t)lTemperature * 1000000LL); // m degC
}
uint32_t lt7182s_get_freq()
{
    return DIV_ROUND(lt7182s_pmbus_read_l11(0x33), 1000000ULL); // Hz
}
uint8_t lt7182s_set_freq(uint32_t ulFrequency)
{
    return lt7182s_pmbus_write_l11(0x33, (uint64_t)ulFrequency * 1000000ULL); // Hz
}