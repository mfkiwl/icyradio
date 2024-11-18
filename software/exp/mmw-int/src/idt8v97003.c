#include "idt8v97003.h"

static const float kVCO[] = { 100e6, 120e6, 140e6, 160e6, 210e6, 165e6, 155e6, 170e6 };
static float fRefFreq;
static uint8_t ubCachedRFOutPwr[2];
static idt8v97003_loop_filter_t sLoopFilter;
static float fTargetLoopBw;

float idt8v97003_get_gcd(float a, float b)
{
    if((a == 0.0) || (b == 0.0))
        return MAX(a, b);

    while(b != 0.0)
    {
        float _a = a;

        a = b;
        b = fmodf(_a, b);
    }

    return a;
}
float idt8v97003_get_mixed_number(float *a, float *b)
{
    float gcd = idt8v97003_get_gcd(*a, *b);
    float _a = *a / gcd;
    float _b = *b / gcd;

    *a = fmodf(_a, _b);
    *b = _b;

    return floorf(_a / _b);
}

static int8_t idt8v97003_validate_loop_filter(idt8v97003_loop_filter_t *pLoopFilter)
{
    if(pLoopFilter->fRs <= 0.0)
        return -1;

    if(pLoopFilter->fCs <= 0.0)
        return -2;

    if(pLoopFilter->fCp <= 0.0)
        return -3;

    if(pLoopFilter->fC3 > 0.0 && pLoopFilter->fR3 <= 0.0)
        return -4;

    if(pLoopFilter->fR3 > 0.0 && pLoopFilter->fC3 <= 0.0)
        return -5;

    return 0;
}

static void idt8v97003_read_register(uint8_t ubRegister, uint8_t *pubData, uint8_t ubCount)
{
    if(ubRegister + ubCount > 0x49)
        return;

    if(!ubCount)
        return;

    if(!pubData)
        return;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SYNTH_SELECT();

        usart3_spi_write_byte(BIT(7), 0);
        usart3_spi_write_byte(ubRegister, 1);
        usart3_spi_read(pubData, ubCount, 0x00);

        SYNTH_UNSELECT();
    }
}
static uint8_t idt8v97003_read_register8(uint8_t ubRegister)
{
    uint8_t ubData;

    idt8v97003_read_register(ubRegister, &ubData, 1);

    return ubData;
}
static uint16_t idt8v97003_read_register16(uint8_t ubRegister)
{
    uint8_t ubData[2];

    idt8v97003_read_register(ubRegister, ubData, 2);

    return ((uint16_t)ubData[1] << 8) | ubData[0];
}
static uint32_t idt8v97003_read_register32(uint8_t ubRegister)
{
    uint8_t ubData[4];

    idt8v97003_read_register(ubRegister, ubData, 4);

    return ((uint32_t)ubData[3] << 24) | ((uint32_t)ubData[2] << 16) | ((uint32_t)ubData[1] << 8) | ubData[0];
}
static void idt8v97003_write_register(uint8_t ubRegister, uint8_t *pubData, uint8_t ubCount)
{
    if(ubRegister + ubCount > 0x49)
        return;

    if(!ubCount)
        return;

    if(!pubData)
        return;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        SYNTH_SELECT();

        usart3_spi_write_byte(0x00, 0);
        usart3_spi_write_byte(ubRegister, 0);
        usart3_spi_write(pubData, ubCount, 1);

        SYNTH_UNSELECT();
    }
}
static void idt8v97003_write_register8(uint8_t ubRegister, uint8_t ubData)
{
    idt8v97003_write_register(ubRegister, &ubData, 1);
}
static void idt8v97003_write_register16(uint8_t ubRegister, uint16_t usData)
{
    uint8_t ubData[2];

    ubData[0] = usData & 0xFF;
    ubData[1] = (usData >> 8) & 0xFF;

    idt8v97003_write_register(ubRegister, ubData, 2);
}
static void idt8v97003_write_register32(uint8_t ubRegister, uint32_t ulData)
{
    uint8_t ubData[4];

    ubData[0] = ulData & 0xFF;
    ubData[1] = (ulData >> 8) & 0xFF;
    ubData[2] = (ulData >> 16) & 0xFF;
    ubData[3] = (ulData >> 24) & 0xFF;

    idt8v97003_write_register(ubRegister, ubData, 4);
}
static void idt8v97003_rmw_register8(uint8_t ubRegister, uint8_t ubMask, uint8_t ubData)
{
    idt8v97003_write_register8(ubRegister, (idt8v97003_read_register8(ubRegister) & ubMask) | ubData);
}

static void idt8v97003_transfer_double_buffer()
{
    idt8v97003_write_register8(IDT8V97003_REG_BUF_TRANSFER, IDT8V97003_REG_BUF_TRANSFER_TRANSFER_ON);
}

uint8_t idt8v97003_init()
{
    fRefFreq = 0;
    ubCachedRFOutPwr[0] = 0;
    ubCachedRFOutPwr[1] = 0;
    sLoopFilter.fRs = 0.0;
    sLoopFilter.fCs = 0.0;
    sLoopFilter.fCp = 0.0;
    sLoopFilter.fR3 = 0.0;
    sLoopFilter.fC3 = 0.0;
    fTargetLoopBw = 0.0;

    SYNTH_UNSELECT();
    idt8v97003_reset();

    idt8v97003_write_register8(IDT8V97003_REG_INTF_CONFIG, IDT8V97003_REG_INTF_CONFIG_ADDR_ASC);

    uint8_t ubChipType = idt8v97003_read_register8(IDT8V97003_REG_CHIP_TYPE);

    if(ubChipType != 0x06)
        return 0;

    uint16_t usChipID = idt8v97003_read_register16(IDT8V97003_REG_CHIP_ID_LOW);

    if(usChipID != 0x0001)
        return 0;

    uint16_t usVendorID = idt8v97003_read_register16(IDT8V97003_REG_VENDOR_ID_LOW);

    if(usVendorID != 0x0426)
        return 0;

    idt8v97003_write_register8(IDT8V97003_REG_BUF_READ, 0x00); // Reads target the active register, not the buffer
    idt8v97003_write_register8(IDT8V97003_REG_DSM_CTL, IDT8V97003_REG_DSM_CTL_SHAPE_DITHER_EN);
    idt8v97003_write_register8(IDT8V97003_REG_MANUAL_VCO, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_MANUAL_DIGITAL, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_MANUAL_CTL, IDT8V97003_REG_MANUAL_CTL_BAND_SEL_ACC_8X);
    idt8v97003_write_register8(0x24, 0x80);
    idt8v97003_write_register8(0x25, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_MULT_CTL0, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_MULT_CTL1, IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCLOW);
    idt8v97003_write_register8(IDT8V97003_REG_ICP_BLEEDER, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_PFD_PULSE_WIDTH, IDT8V97003_REG_PFD_PULSE_WIDTH_VCO_BUF_LOW_V);
    idt8v97003_write_register8(IDT8V97003_REG_RESYNC_TIME_LOW, 0x88); // resync_time = 5000
    idt8v97003_write_register8(IDT8V97003_REG_RESYNC_TIME_HIGH, 0x13);
    idt8v97003_write_register8(IDT8V97003_REG_RFOUTA_PWR, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_RFOUTA_ENA, IDT8V97003_REG_RFOUTA_ENA_REF_VOLTAGE | IDT8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD | IDT8V97003_REG_RFOUTA_ENA_DAC_BIAS(14));
    idt8v97003_write_register8(IDT8V97003_REG_RFOUTB_PWR, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_RFOUTB_ENA, IDT8V97003_REG_RFOUTB_ENA_REF_VOLTAGE | IDT8V97003_REG_RFOUTB_ENA_DAC_BIAS(14));
    idt8v97003_write_register8(IDT8V97003_REG_VCO_CAL_VOLTAGE, IDT8V97003_REG_VCO_CAL_VOLTAGE_0V82);
    idt8v97003_write_register8(IDT8V97003_REG_PTAT_PWR_CONFIG, IDT8V97003_REG_PTAT_PWR_CONFIG_PTAT_MODE_NORMAL);
    idt8v97003_write_register8(IDT8V97003_REG_CURRENT_CONFIG1, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_CURRENT_CONFIG2, 0x00);
    idt8v97003_write_register8(IDT8V97003_REG_TRIM_CONFIG1, IDT8V97003_REG_TRIM_CONFIG1_VCO_REG_TRIM_2V09 | IDT8V97003_REG_TRIM_CONFIG1_OUT_REG_TRIM_2V62);
    idt8v97003_write_register8(IDT8V97003_REG_TRIM_CONFIG2, IDT8V97003_REG_TRIM_CONFIG2_PDCP_REG_TRIM_2V62 | IDT8V97003_REG_TRIM_CONFIG2_FB_REG_TRIM_2V62);
    idt8v97003_write_register8(IDT8V97003_REG_TRIM_CONFIG3, IDT8V97003_REG_TRIM_CONFIG3_DIG_REG_TRIM_1V5 | IDT8V97003_REG_TRIM_CONFIG3_REF_REG_TRIM_2V62);

    idt8v97003_transfer_double_buffer();

    return 1;
}

uint8_t idt8v97003_get_chip_version()
{
    return idt8v97003_read_register8(IDT8V97003_REG_CHIP_VERSION);
}
uint8_t idt8v97003_get_chip_option()
{
    return idt8v97003_read_register8(IDT8V97003_REG_CHIP_OPTION);
}

void idt8v97003_reset()
{
#ifdef SYNTH_RESET
    SYNTH_RESET();
    delay_ms(10);
    SYNTH_UNRESET();
    delay_ms(10);
#else
    idt8v97003_write_register8(IDT8V97003_REG_INTF_CONFIG, IDT8V97003_REG_INTF_CONFIG_SOFT_RESET);
    delay_ms(1);
#endif
}

void idt8v97003_power_up(uint8_t ubFlags)
{
#ifdef SYNTH_ENABLE
    if(ubFlags & IDT8V97003_PWR_CHIP_ENABLE)
    {
        SYNTH_ENABLE();
    }
#endif

    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_PWR_CTL);
    uint8_t ubResetMult = 0;

    if((ubReg & IDT8V97003_REG_PWR_CTL_REF_VREG_PDOWN) && (ubFlags & IDT8V97003_PWR_REF_VREG))
        ubResetMult = 1;

    ubReg &= (ubFlags & IDT8V97003_PWR_REF_VREG) ? ~IDT8V97003_REG_PWR_CTL_REF_VREG_PDOWN : 0xFF;
    ubReg &= (ubFlags & IDT8V97003_PWR_PDCP_VREG) ? ~IDT8V97003_REG_PWR_CTL_PDCP_VREG_PDOWN : 0xFF;
    ubReg &= (ubFlags & IDT8V97003_PWR_FB_VREG) ? ~IDT8V97003_REG_PWR_CTL_FB_VREG_PDOWN : 0xFF;
    ubReg &= (ubFlags & IDT8V97003_PWR_OUTA_VREG) ? ~IDT8V97003_REG_PWR_CTL_OUTA_VREG_PDOWN : 0xFF;
    ubReg &= (ubFlags & IDT8V97003_PWR_OUTB_BUF_VREG) ? ~IDT8V97003_REG_PWR_CTL_OUTB_BUF_VREG_PDOWN : 0xFF;
    ubReg &= (ubFlags & IDT8V97003_PWR_ANA_VREG) ? ~IDT8V97003_REG_PWR_CTL_ANA_PDOWN : 0xFF;
    ubReg |= (ubFlags & IDT8V97003_PWR_VCO) ? IDT8V97003_REG_PWR_CTL_VCO_EN : 0; // VCO is inverted

    idt8v97003_write_register8(IDT8V97003_REG_PWR_CTL, ubReg);

    // Reset multiplier if enabled and reference path was just powered up
    if(!ubResetMult)
        return;

    ubReg = idt8v97003_read_register8(IDT8V97003_REG_MULT_CTL0);

    if(!(ubReg & IDT8V97003_REG_MULT_CTL0_MULT_EN))
        return;

    idt8v97003_write_register8(IDT8V97003_REG_MULT_CTL0, ubReg | IDT8V97003_REG_MULT_CTL0_MULT_RESET);
    idt8v97003_transfer_double_buffer();
    idt8v97003_write_register8(IDT8V97003_REG_MULT_CTL0, ubReg & ~IDT8V97003_REG_MULT_CTL0_MULT_RESET);
    idt8v97003_transfer_double_buffer();
}
void idt8v97003_power_down(uint8_t ubFlags)
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_PWR_CTL);

    ubReg |= (ubFlags & IDT8V97003_PWR_REF_VREG) ? IDT8V97003_REG_PWR_CTL_REF_VREG_PDOWN : 0;
    ubReg |= (ubFlags & IDT8V97003_PWR_PDCP_VREG) ? IDT8V97003_REG_PWR_CTL_PDCP_VREG_PDOWN : 0;
    ubReg |= (ubFlags & IDT8V97003_PWR_FB_VREG) ? IDT8V97003_REG_PWR_CTL_FB_VREG_PDOWN : 0;
    ubReg |= (ubFlags & IDT8V97003_PWR_OUTA_VREG) ? IDT8V97003_REG_PWR_CTL_OUTA_VREG_PDOWN : 0;
    ubReg |= (ubFlags & IDT8V97003_PWR_OUTB_BUF_VREG) ? IDT8V97003_REG_PWR_CTL_OUTB_BUF_VREG_PDOWN : 0;
    ubReg |= (ubFlags & IDT8V97003_PWR_ANA_VREG) ? IDT8V97003_REG_PWR_CTL_ANA_PDOWN : 0;
    ubReg &= (ubFlags & IDT8V97003_PWR_VCO) ? ~IDT8V97003_REG_PWR_CTL_VCO_EN : 0xFF; // VCO is inverted

    idt8v97003_write_register8(IDT8V97003_REG_PWR_CTL, ubReg);

#ifdef SYNTH_ENABLE
    if(ubFlags & IDT8V97003_PWR_CHIP_ENABLE)
    {
        SYNTH_DISABLE();
    }
#endif
}

uint8_t idt8v97003_is_muted(int8_t bChannel)
{
#ifdef SYNTH_MUTED
    if(SYNTH_MUTED())
        return 1;
#endif

    if(bChannel < 0)
    {
#ifndef SYNTH_MUTED
        return idt8v97003_is_muted(0) && idt8v97003_is_muted(1);
#else
        return 0;
#endif
    }

    uint8_t ubReg;

    switch(bChannel)
    {
        case 0:
            ubReg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case 1:
            ubReg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            return 0;
    }

    uint8_t ubVal = idt8v97003_read_register8(ubReg) & 0x0F;

    if(!ubVal)
        return 1;

    return 0;
}
void idt8v97003_mute(int8_t bChannel, uint8_t ubMute)
{
    if(bChannel < 0)
    {
#ifdef SYNTH_MUTE
        if(ubMute)
            SYNTH_MUTE();
        else
            SYNTH_UNMUTE();
#else
        idt8v97003_mute(0, ubMute);
        idt8v97003_mute(1, ubMute);
#endif

        return;
    }

    uint8_t ubReg;

    switch(bChannel)
    {
        case 0:
            ubReg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case 1:
            ubReg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            return;
    }

    if(ubMute && ubCachedRFOutPwr[bChannel])
        return; // Already muted

    if(!ubMute && !ubCachedRFOutPwr[bChannel])
        return; // Already unmuted

    uint8_t ubVal = idt8v97003_read_register8(ubReg);
    uint8_t ubPwr = ubVal & 0x0F;

    if(ubMute)
    {
        if(!ubPwr)
            return; // Already muted

        ubCachedRFOutPwr[bChannel] = ubPwr;

        ubPwr = 0;
    }
    else
    {
        if(ubPwr)
            return; // Already unmuted

        ubPwr = ubCachedRFOutPwr[bChannel];

        ubCachedRFOutPwr[bChannel] = 0;
    }

    idt8v97003_write_register8(ubReg, (ubVal & 0xF0) | ubPwr);
}
uint8_t idt8v97003_get_mute_until_locked()
{
    return !!(idt8v97003_read_register8(IDT8V97003_REG_RFOUTA_ENA) & IDT8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD);
}
void idt8v97003_set_mute_until_locked(uint8_t ubMute)
{
    idt8v97003_rmw_register8(IDT8V97003_REG_RFOUTA_ENA, (uint8_t)~IDT8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD, ubMute ? IDT8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD : 0);
}

void idt8v97003_enable_rf_output(uint8_t ubChannel, uint8_t ubEnable)
{
    uint8_t ubReg;

    switch(ubChannel)
    {
        case 0:
            ubReg = IDT8V97003_REG_RFOUTA_ENA;
        break;
        case 1:
            ubReg = IDT8V97003_REG_RFOUTB_ENA;
        break;
        default:
            return;
    }

    idt8v97003_rmw_register8(ubReg, (uint8_t)~IDT8V97003_REG_RFOUTA_ENA_RFOUTA_ENA, ubEnable ? IDT8V97003_REG_RFOUTA_ENA_RFOUTA_ENA : 0);
}
uint8_t idt8v97003_is_rf_output_enabled(uint8_t ubChannel)
{
    uint8_t ubReg;

    switch(ubChannel)
    {
        case 0:
            ubReg = IDT8V97003_REG_RFOUTA_ENA;
        break;
        case 1:
            ubReg = IDT8V97003_REG_RFOUTB_ENA;
        break;
        default:
            return 0;
    }

    return !!(idt8v97003_read_register8(ubReg) & IDT8V97003_REG_RFOUTA_ENA_RFOUTA_ENA);
}
void idt8v97003_set_rf_output_power(uint8_t ubChannel, uint8_t ubPower)
{
    if(ubPower > 12)
        return;

    uint8_t ubReg;

    switch(ubChannel)
    {
        case 0:
            ubReg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case 1:
            ubReg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            return;
    }

    idt8v97003_write_register8(ubReg, ubPower & 0x0F);

    ubCachedRFOutPwr[ubChannel] = 0;
}
uint8_t idt8v97003_get_rf_output_power(uint8_t ubChannel)
{
    uint8_t ubReg;

    switch(ubChannel)
    {
        case 0:
            ubReg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case 1:
            ubReg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            return 0;
    }

    uint8_t ubPwr = idt8v97003_read_register8(ubReg) & 0x0F;

    if(!ubPwr)
        return ubCachedRFOutPwr[ubChannel];

    return ubPwr;
}

float idt8v97003_get_temperature()
{
    // TODO: Check IDT8V97003.cpp on Soapy driver
    return 0.0;
}

uint8_t idt8v97003_get_current_vco()
{
    return idt8v97003_read_register8(IDT8V97003_REG_LD_CAL_VCO_STATUS) & IDT8V97003_REG_LD_CAL_VCO_STATUS_VCO_STS;
}
uint8_t idt8v97003_get_current_digital_band()
{
    return idt8v97003_read_register8(IDT8V97003_REG_DIG_BAND_STATUS) & 0x7F;
}
void idt8v97003_get_current_vco_band(idt8v97003_vco_band_t *pVCOBand)
{
    if(!pVCOBand)
        return;

    uint8_t ubBuf[2];

    idt8v97003_read_register(IDT8V97003_REG_LD_CAL_VCO_STATUS, ubBuf, 2);

    pVCOBand->ubVCO = ubBuf[0] & IDT8V97003_REG_LD_CAL_VCO_STATUS_VCO_STS;
    pVCOBand->ubDigitalBand = ubBuf[1] & 0x7F;
}
void idt8v97003_force_vco_band(uint8_t ubForce, idt8v97003_vco_band_t *pVCOBand)
{
    if(!pVCOBand)
        return;

    // TODO: Check IDT8V97003.cpp on Soapy driver
}

uint8_t idt8v97003_is_pll_locked()
{
    return !!(idt8v97003_read_register8(IDT8V97003_REG_LD_CAL_VCO_STATUS) & IDT8V97003_REG_LD_CAL_VCO_STATUS_DIG_LOCK);
}
uint8_t idt8v97003_is_band_select_done()
{
    return !!(idt8v97003_read_register8(IDT8V97003_REG_LD_CAL_VCO_STATUS) & IDT8V97003_REG_LD_CAL_VCO_STATUS_BAND_SEL_DONE);
}

void idt8v97003_config_reference_input(float fFreq, uint8_t ubDiff)
{
    if(fFreq < 10e6)
        return;

    if(fFreq > 1.6e9)
        return;

    idt8v97003_rmw_register8(IDT8V97003_REG_RDIV_HIGH, (uint8_t)~(IDT8V97003_REG_RDIV_HIGH_REF_DBL_DELAY | IDT8V97003_REG_RDIV_HIGH_INPUT_TYPE), ((fFreq < 50e6) ? IDT8V97003_REG_RDIV_HIGH_REF_DBL_DELAY : 0) | (ubDiff ? IDT8V97003_REG_RDIV_HIGH_INPUT_TYPE : 0));

    fRefFreq = fFreq;
}
void idt8v97003_config_pfd_freq(float fFreq, uint8_t ubPulseWidth)
{
    if(fFreq > 500e6)
        return;

    if(fRefFreq < 10e6)
        return;

    uint8_t ubFound = 0;
    idt8v97003_ref_path_config_t sRefCfg;
    float fMinDiff = INFINITY;

    for(uint8_t ubDoubler = 2; ubDoubler > 0; ubDoubler--) // First try with the doubler enabled
    {
        for(uint8_t ubMult = 1; ubMult < 64; ubMult++)
        {
            if(ubMult == 2)
                continue;

            float fMultOutFreq = fRefFreq * ubDoubler * ubMult;

            if(ubMult > 1 && (fMultOutFreq < 160e6 || fMultOutFreq > 250e6))
                continue;

            for(uint16_t usRDiv = 1; usRDiv < 1024; usRDiv++)
            {
                float fPFDFreq = fMultOutFreq / usRDiv;
                float fDiff = ABS(fPFDFreq - fFreq);

                if(fDiff < fMinDiff)
                {
                    sRefCfg.ubEnableDoubler = (ubDoubler == 2);
                    sRefCfg.ubMult = ubMult;
                    sRefCfg.usRDiv = usRDiv;

                    fMinDiff = fDiff;
                    ubFound = 1;

                    if(fDiff == 0)
                    {
                        ubFound = 2; // Found exact match

                        break;
                    }
                }
            }

            if(ubFound > 1)
                break;
        }

        if(ubFound > 1)
            break;
    }

    if(ubFound == 0)
        return;

    idt8v97003_config_pfd(&sRefCfg, ubPulseWidth);
}
void idt8v97003_config_pfd(idt8v97003_ref_path_config_t *pRefCfg, uint8_t ubPulseWidth)
{
    if(!pRefCfg)
        return;

    switch(ubPulseWidth)
    {
        case IDT8V97003_REG_PFD_PULSE_WIDTH_PW_260ps:
            break;
        case IDT8V97003_REG_PFD_PULSE_WIDTH_PW_348ps:
            break;
        case IDT8V97003_REG_PFD_PULSE_WIDTH_PW_487ps:
            break;
        case IDT8V97003_REG_PFD_PULSE_WIDTH_PW_583ps:
            break;
        default:
            return;
    }

    if(fRefFreq < 10e6)
        return;

    if(pRefCfg->ubMult < 1 || pRefCfg->ubMult == 2 || pRefCfg->ubMult > 63)
        return;

    if(pRefCfg->usRDiv < 1 || pRefCfg->usRDiv > 1023)
        return;

    if(fRefFreq > (pRefCfg->ubEnableDoubler ? 250e6 : 1.6e9))
        return;

    float fMultOutFreq = fRefFreq * (pRefCfg->ubEnableDoubler ? 2 : 1) * pRefCfg->ubMult;

    if(pRefCfg->ubMult > 1 && (fMultOutFreq < 160e6 || fMultOutFreq > 250e6))
        return;

    float fPFDFreq = fMultOutFreq / pRefCfg->usRDiv;

    if(fPFDFreq > 500e6)
        return;

    // Calculate band select clock divider
    float fBandSelClock = fPFDFreq;
    uint16_t usBandSelDiv = 1;

    while(usBandSelDiv < 8192)
    {
        if(fBandSelClock >= 50e3 && fBandSelClock <= 100e3)
            break;

        usBandSelDiv++;
        fBandSelClock = fPFDFreq / usBandSelDiv;
    }

    if(usBandSelDiv >= 8192)
        return;

    if(pRefCfg->ubMult > 1)
    {
        // Multiplier enabled, enable and reset
        idt8v97003_rmw_register8(IDT8V97003_REG_MULT_CTL1, 0x07, IDT8V97003_REG_MULT_CTL1_MULT_MUX_ENA | IDT8V97003_REG_MULT_CTL1_MULT_D2S_ENA | IDT8V97003_REG_MULT_CTL1_MULT_CP_ENA);
        idt8v97003_write_register8(IDT8V97003_REG_MULT_CTL0, IDT8V97003_REG_MULT_CTL0_MULT_EN | IDT8V97003_REG_MULT_CTL0_MULT_RESET);

        idt8v97003_transfer_double_buffer(); // Ensure it is enabled and properly reset

        // Program the multiplication value (does not apply until the next buffer transfer)
        idt8v97003_write_register8(IDT8V97003_REG_MULT_CTL0, IDT8V97003_REG_MULT_CTL0_MULT_EN | (pRefCfg->ubMult & 0x3F));
    }
    else
    {
        // Multiplier disabled, disable and force low control voltage (does not apply until the next buffer transfer)
        idt8v97003_write_register8(IDT8V97003_REG_MULT_CTL0, 0);
        idt8v97003_rmw_register8(IDT8V97003_REG_MULT_CTL1, 0x07, IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCLOW);
    }

    idt8v97003_write_register8(IDT8V97003_REG_RDIV_LOW, pRefCfg->usRDiv & 0xFF);
    idt8v97003_rmw_register8(IDT8V97003_REG_RDIV_HIGH, IDT8V97003_REG_RDIV_HIGH_REF_DBL_DELAY | IDT8V97003_REG_RDIV_HIGH_INPUT_TYPE, (pRefCfg->ubEnableDoubler ? IDT8V97003_REG_RDIV_HIGH_REF_DBL_EN : 0) | ((pRefCfg->usRDiv >> 8) & 0x03));

    idt8v97003_write_register16(IDT8V97003_REG_BAND_SEL_DIV_LOW, usBandSelDiv & 0x1FFF);

    idt8v97003_transfer_double_buffer();

    idt8v97003_rmw_register8(IDT8V97003_REG_PFD_PULSE_WIDTH, ~0x0C, (ubPulseWidth & 0x0C));
}

float idt8v97003_get_reference_doubler_input_frequency()
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_RDIV_HIGH);

    if(!(ubReg & IDT8V97003_REG_RDIV_HIGH_REF_DBL_EN))
        return 0.0;

    return fRefFreq;
}
float idt8v97003_get_reference_doubler_output_frequency()
{
    return idt8v97003_get_reference_doubler_input_frequency() * 2;
}
float idt8v97003_get_reference_multiplier_input_frequency()
{
    uint8_t ubReg[2];

    idt8v97003_read_register(IDT8V97003_REG_MULT_CTL0, ubReg, 2);

    if(!(ubReg[1] & IDT8V97003_REG_MULT_CTL1_MULT_MUX_ENA))
        return 0.0;

    if(!(ubReg[1] & IDT8V97003_REG_MULT_CTL1_MULT_D2S_ENA))
        return 0.0;

    if(!(ubReg[1] & IDT8V97003_REG_MULT_CTL1_MULT_CP_ENA))
        return 0.0;

    if(ubReg[1] & IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCLOW)
        return 0.0;

    if(ubReg[1] & IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCHI)
        return 0.0;

    if(!(ubReg[0] & IDT8V97003_REG_MULT_CTL0_MULT_EN))
        return 0.0;

    if(ubReg[0] & IDT8V97003_REG_MULT_CTL0_MULT_RESET)
        return 0.0;

    float fMultIn = idt8v97003_get_reference_doubler_output_frequency();

    if(fMultIn == 0.0)
        fMultIn = fRefFreq;

    return fMultIn;
}
float idt8v97003_get_reference_multiplier_output_frequency()
{
    uint8_t ubMult = idt8v97003_read_register8(IDT8V97003_REG_MULT_CTL0) & 0x3F;

    if(!ubMult)
        return 0.0;

    return idt8v97003_get_reference_multiplier_input_frequency() * ubMult;
}
float idt8v97003_get_reference_divider_input_frequency()
{
    float fRefDivIn = idt8v97003_get_reference_multiplier_output_frequency();

    if(fRefDivIn == 0.0)
        fRefDivIn = idt8v97003_get_reference_doubler_output_frequency();

    if(fRefDivIn == 0.0)
        fRefDivIn = fRefFreq;

    return fRefDivIn;
}
float idt8v97003_get_reference_divider_output_frequency()
{
    uint16_t usRDiv = idt8v97003_read_register16(IDT8V97003_REG_RDIV_LOW) & 0x3FF;

    if(!usRDiv)
        return 0.0;

    return idt8v97003_get_reference_divider_input_frequency() / usRDiv;
}

float idt8v97003_get_reference_frequency()
{
    return fRefFreq;
}
float idt8v97003_get_pfd_frequency()
{
    return idt8v97003_get_reference_divider_output_frequency();
}

void idt8v97003_get_loop_filter(idt8v97003_loop_filter_t *pLoopFilter)
{
    if(!pLoopFilter)
        return;

    if(idt8v97003_validate_loop_filter(&sLoopFilter))
        return;

    pLoopFilter->fRs = sLoopFilter.fRs;
    pLoopFilter->fCs = sLoopFilter.fCs;
    pLoopFilter->fCp = sLoopFilter.fCp;
    pLoopFilter->fR3 = sLoopFilter.fR3;
    pLoopFilter->fC3 = sLoopFilter.fC3;
}
void idt8v97003_set_loop_filter(idt8v97003_loop_filter_t *pLoopFilter)
{
    if(!pLoopFilter)
        return;

    sLoopFilter.fRs = pLoopFilter->fRs;
    sLoopFilter.fCs = pLoopFilter->fCs;
    sLoopFilter.fCp = pLoopFilter->fCp;
    sLoopFilter.fR3 = pLoopFilter->fR3;
    sLoopFilter.fC3 = pLoopFilter->fC3;
}
void idt8v97003_get_loop_frequency_response(idt8v97003_loop_freq_response_t *pLoopFreqResp)
{
    if(!pLoopFreqResp)
        return;

    idt8v97003_loop_filter_t sLF;

    idt8v97003_get_loop_filter(&sLF);

    idt8v97003_charge_pump_config_t sCP;

    idt8v97003_get_charge_pump_config(&sCP);

    float fIcp = (sCP.fPositiveCurrent + sCP.fNegativeCurrent) / 2.0;
    uint8_t ubVCO = idt8v97003_get_current_vco();
    float fN = idt8v97003_get_feedback_divider();

    pLoopFreqResp->fFz = 1.0 / (2.0 * M_PI * sLF.fRs * sLF.fCs);
    pLoopFreqResp->fFc = sLF.fRs * fIcp * kVCO[ubVCO] / (2.0 * M_PI * fN);
    pLoopFreqResp->fFp = 1.0 / (2.0 * M_PI * sLF.fRs * sLF.fCp);

    if(sLF.fR3 > 0.0 && sLF.fC3 > 0.0)
        pLoopFreqResp->fFp2 = 1.0 / (2.0 * M_PI * sLF.fR3 * sLF.fC3);
    else
        pLoopFreqResp->fFp2 = 0.0;

    float fB = 1.0 + sLF.fCs / sLF.fCp;

    pLoopFreqResp->fPhaseMargin = atanf(fB - 1.0 / (2.0 * sqrtf(fB))) * 180.0 / M_PI;
}
float idt8v97003_get_loop_bandwidth()
{
    idt8v97003_loop_filter_t sLF;

    idt8v97003_get_loop_filter(&sLF);

    idt8v97003_charge_pump_config_t sCP;

    idt8v97003_get_charge_pump_config(&sCP);

    float fIcp = (sCP.fPositiveCurrent + sCP.fNegativeCurrent) / 2.0;
    uint8_t ubVCO = idt8v97003_get_current_vco();
    float fN = idt8v97003_get_feedback_divider();

    return sLF.fRs * fIcp * kVCO[ubVCO] / (2.0 * M_PI * fN);
}
void idt8v97003_set_loop_bandwidth(float fBandwidth)
{
    if(fBandwidth <= 0.0)
    {
        fBandwidth = idt8v97003_get_target_loop_bandwidth();

        if(fBandwidth <= 0.0)
            return;
    }

    idt8v97003_loop_filter_t sLF;

    idt8v97003_get_loop_filter(&sLF);

    uint8_t ubVCO = idt8v97003_get_current_vco();
    float fN = idt8v97003_get_feedback_divider();

    float fIcp = fBandwidth * 2.0 * M_PI * fN / (sLF.fRs * kVCO[ubVCO]);

    idt8v97003_charge_pump_config_t sCP;

    sCP.fPositiveCurrent = fIcp;
    sCP.fNegativeCurrent = fIcp;
    sCP.fBleederCurrent = -1.0;

    idt8v97003_set_charge_pump_config(&sCP);
}
float idt8v97003_get_target_loop_bandwidth()
{
    return fTargetLoopBw;
}
void idt8v97003_set_target_loop_bandwidth(float fBandwidth)
{
    fTargetLoopBw = fBandwidth;
}

void idt8v97003_get_charge_pump_config(idt8v97003_charge_pump_config_t *pChargePumpCfg)
{
    if(!pChargePumpCfg)
        return;

    uint8_t ubReg[3];

    idt8v97003_read_register(IDT8V97003_REG_ICP_PMOS, ubReg, 3);

    pChargePumpCfg->fPositiveCurrent = (float)((ubReg[0] & 0x3F) + 1) / 6.0 * 1e-3;
    pChargePumpCfg->fNegativeCurrent = (float)((ubReg[1] & 0x3F) + 1) / 6.0 * 1e-3;
    pChargePumpCfg->fBleederCurrent = (float)(ubReg[2] & 0x7F) * 20e-6;
}
void idt8v97003_set_charge_pump_config(idt8v97003_charge_pump_config_t *pChargePumpCfg)
{
    if(!pChargePumpCfg)
        return;

    float fPos = roundf(pChargePumpCfg->fPositiveCurrent * 6.0 / 1e-3) - 1;
    fPos = CLAMP(fPos, 0, 63);

    float fNeg = roundf(pChargePumpCfg->fNegativeCurrent * 6.0 / 1e-3) - 1;
    fNeg = CLAMP(fNeg, 0, 63);

    uint8_t ubWriteBleeder = pChargePumpCfg->fBleederCurrent >= 0.0;
    float fBleeder = roundf(pChargePumpCfg->fBleederCurrent / 20e-6);
    fBleeder = CLAMP(fBleeder, 0, 127);

    uint8_t ubReg[3];

    ubReg[0] = (uint8_t)fPos & 0x3F;
    ubReg[1] = (uint8_t)fNeg & 0x3F;
    ubReg[2] = (uint8_t)fBleeder & 0x7F;

    if(ubWriteBleeder)
        ubReg[2] |= idt8v97003_read_register8(IDT8V97003_REG_ICP_BLEEDER) & IDT8V97003_REG_ICP_BLEEDER_CP_HIZ;

    idt8v97003_write_register(IDT8V97003_REG_ICP_PMOS, ubReg, ubWriteBleeder ? 3 : 2);
}
float idt8v97003_get_charge_pump_positive_current()
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_ICP_PMOS);

    return (float)((ubReg & 0x3F) + 1) / 6.0 * 1e-3;
}
void idt8v97003_set_charge_pump_positive_current(float fCurrent)
{
    fCurrent = roundf(fCurrent * 6.0 / 1e-3) - 1;
    fCurrent = CLAMP(fCurrent, 0, 63);

    idt8v97003_write_register8(IDT8V97003_REG_ICP_PMOS, (uint8_t)fCurrent & 0x3F);
}
float idt8v97003_get_charge_pump_negative_current()
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_ICP_NMOS);

    return (float)((ubReg & 0x3F) + 1) / 6.0 * 1e-3;
}
void idt8v97003_set_charge_pump_negative_current(float fCurrent)
{
    fCurrent = roundf(fCurrent * 6.0 / 1e-3) - 1;
    fCurrent = CLAMP(fCurrent, 0, 63);

    idt8v97003_write_register8(IDT8V97003_REG_ICP_NMOS, (uint8_t)fCurrent & 0x3F);
}
float idt8v97003_get_charge_pump_bleeder_current()
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_ICP_BLEEDER);

    return (float)(ubReg & 0x7F) * 20e-6;
}
void idt8v97003_set_charge_pump_bleeder_current(float fCurrent)
{
    fCurrent = roundf(fCurrent / 20e-6);
    fCurrent = CLAMP(fCurrent, 0, 127);

    idt8v97003_rmw_register8(IDT8V97003_REG_ICP_BLEEDER, IDT8V97003_REG_ICP_BLEEDER_CP_HIZ, (uint8_t)fCurrent & 0x7F);
}
void idt8v97003_enable_charge_pump(uint8_t ubEnable)
{
    idt8v97003_rmw_register8(IDT8V97003_REG_ICP_BLEEDER, (uint8_t)~IDT8V97003_REG_ICP_BLEEDER_CP_HIZ, ubEnable ? 0 : IDT8V97003_REG_ICP_BLEEDER_CP_HIZ);
}
uint8_t idt8v97003_is_charge_pump_enabled()
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_ICP_BLEEDER);

    return !(ubReg & IDT8V97003_REG_ICP_BLEEDER_CP_HIZ);
}

void idt8v97003_enable_lock_detect(uint8_t ubEnable)
{
    // Yes, it looks like it is inverted (DISABLE = 1 is actually enabled...)
    idt8v97003_rmw_register8(IDT8V97003_REG_LD_CTL0, (uint8_t)~IDT8V97003_REG_LD_CTL0_LD_DISABLE, ubEnable ? IDT8V97003_REG_LD_CTL0_LD_DISABLE : 0);
}
void idt8v97003_enable_auto_recal(uint8_t ubEnable)
{
    idt8v97003_rmw_register8(IDT8V97003_REG_LD_CTL0, (uint8_t)~IDT8V97003_REG_LD_CTL0_AUTO_RECAL_EN, ubEnable ? IDT8V97003_REG_LD_CTL0_AUTO_RECAL_EN : 0);
}
uint8_t idt8v97003_get_lock_detect_precision()
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_LD_CTL1) & 0x07;

    switch(ubReg)
    {
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_0p5ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p0ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p8ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_3p0ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_6p4ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_10p4ns:
            return ubReg;
        case 0x05: // Repeated setting
            return IDT8V97003_REG_LD_CTL1_LD_PRECISION_6p4ns;
        case 0x07: // Repeated setting
            return IDT8V97003_REG_LD_CTL1_LD_PRECISION_10p4ns;
        default:
            return 0;
    }
}
void idt8v97003_set_lock_detect_precision(uint8_t ubPrecision)
{
    switch(ubPrecision)
    {
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_0p5ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p0ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p8ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_3p0ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_6p4ns:
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_10p4ns:
            break;
        default:
            return;
    }

    idt8v97003_rmw_register8(IDT8V97003_REG_LD_CTL1, ~0x07, ubPrecision);
}
uint8_t idt8v97003_get_lock_detect_pin_mode()
{
    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_LD_CTL1) & 0x30;

    switch(ubReg)
    {
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LD:
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_CAL_DONE:
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LOW:
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_HIGH:
            return ubReg;
        default:
            return 0;
    }
}
void idt8v97003_set_lock_detect_pin_mode(uint8_t ubMode)
{
    switch(ubMode)
    {
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LD:
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_CAL_DONE:
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LOW:
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_HIGH:
            break;
        default:
            return;
    }

    idt8v97003_rmw_register8(IDT8V97003_REG_LD_CTL1, ~0x30, ubMode);
}

float idt8v97003_get_feedback_divider()
{
    uint8_t ubReg[10];

    idt8v97003_read_register(IDT8V97003_REG_NINT_LOW, ubReg, 10);

    uint16_t a = ubReg[0] | ((uint16_t)ubReg[1] << 8);
    uint32_t b = ubReg[2] | ((uint32_t)ubReg[3] << 8) | ((uint32_t)ubReg[4] << 16) | ((uint32_t)ubReg[5] << 24);
    uint32_t c = ubReg[6] | ((uint32_t)ubReg[7] << 8) | ((uint32_t)ubReg[8] << 16) | ((uint32_t)ubReg[9] << 24);

    if(!b || !c || b >= c)
        return (float)a;

    return (float)a + (float)b / (float)c;
}
uint8_t idt8v97003_is_feedback_divider_fractional(float *pfDist)
{
    uint8_t ubReg[8];

    idt8v97003_read_register(IDT8V97003_REG_NFRAC_LOW, ubReg, 8);

    uint32_t b = ubReg[0] | ((uint32_t)ubReg[1] << 8) | ((uint32_t)ubReg[2] << 16) | ((uint32_t)ubReg[3] << 24);
    uint32_t c = ubReg[4] | ((uint32_t)ubReg[5] << 8) | ((uint32_t)ubReg[6] << 16) | ((uint32_t)ubReg[7] << 24);

    if(!b || !c || b >= c)
        return 0;

    float fFrac = (float)b / (float)c;

    if(!pfDist)
        return 1;

    if(fFrac < 0.5)
        *pfDist = fFrac;
    else
        *pfDist = 1.0 - fFrac;

    return 1;
}

float idt8v97003_get_vco_frequency()
{
    float fPFDFreq = idt8v97003_get_pfd_frequency();

    if(fPFDFreq == 0.0)
        return 0.0;

    return fPFDFreq * idt8v97003_get_feedback_divider();
}
float idt8v97003_get_frequency()
{
    float fVCOFreq = idt8v97003_get_vco_frequency();

    if(fVCOFreq == 0.0)
        return 0.0;

    uint8_t ubReg = idt8v97003_read_register8(IDT8V97003_REG_OUT_DIV_DBL);

    if(ubReg & IDT8V97003_REG_OUT_DIV_DBL_OUT_DBL_ENA)
        return fVCOFreq * 2.0;

    if(ubReg & IDT8V97003_REG_OUT_DIV_DBL_OUT_DIV_ENA)
    {
        uint8_t ubDiv = ubReg & 0x07;

        if(ubDiv == 0 || ubDiv > 5)
            return 0.0;

        return fVCOFreq / BIT(ubDiv);
    }

    return fVCOFreq;
}
void idt8v97003_set_frequency(float fFreq, uint8_t ubSetLoopBW, int32_t lCalTimeout, int32_t lLockTimeout)
{
    if(fFreq < 171.875e6)
        return;

    if(fFreq > 18e9)
        return;

    // VCO works from 5.5 GHz to 11 GHz
    // For frequencies below 5.5 GHz, the VCO divider must be used
    // For frequencies above 11 GHz, the VCO doubler must be used
    uint8_t ubM0Div = 0;
    uint8_t ubOutDoublerEn = 0;
    float fVCOFreq;

    if(fFreq < 5.5e9)
    {
        for(uint8_t i = 1; i < 6; i++)
        {
            fVCOFreq = fFreq * BIT(i);

            if(fVCOFreq >= 5.5e9 && fVCOFreq <= 11e9) // VCO operating range
            {
                ubM0Div = i;

                break;
            }
        }

        if(!ubM0Div)
            return;
    }
    else if(fFreq > 11e9)
    {
        fVCOFreq = fFreq / 2;
        ubOutDoublerEn = 1;
    }
    else
    {
        fVCOFreq = fFreq;
    }

    float fPFDFreq = idt8v97003_get_pfd_frequency();

    if(fPFDFreq == 0.0)
        return;

    float b = fVCOFreq;
    float c = fPFDFreq;
    float a = idt8v97003_get_mixed_number(&b, &c);

    while(c >= 2e32)
    {
        b /= 2.0;
        c /= 2.0;
    }

    a = roundf(a);
    b = roundf(b);
    c = roundf(c);

    if(b == c)
    {
        a++;

        b = 0.0;
        c = 2.0;
    }

    if(b > 0.0 && fPFDFreq > 250e6)
        return;

    if(b >= c) // Sanity check
        return;

    if(a > (BIT(16) - 1) || a < 12) // Integer limits (min 12, 16-bit wide)
        return;

    if(b > 0.0)
    {
        while(c < BIT(31)) // Maximize MOD and FRAC to improve fPhase noise
        {
            c *= 2.0;
            b *= 2.0;
        }
    }
    else
    {
        c = 2.0;
    }

    uint8_t ubManCtlReg = IDT8V97003_REG_MANUAL_CTL_FORCE_RELOCK | IDT8V97003_REG_MANUAL_CTL_MANUAL_RESYNC;

    uint8_t ubReg[10];

    ubReg[0] = ((uint16_t)a >> 0) & 0xFF;
    ubReg[1] = ((uint16_t)a >> 8) & 0xFF;
    ubReg[2] = ((uint32_t)b >> 0) & 0xFF;
    ubReg[3] = ((uint32_t)b >> 8) & 0xFF;
    ubReg[4] = ((uint32_t)b >> 16) & 0xFF;
    ubReg[5] = ((uint32_t)b >> 24) & 0xFF;
    ubReg[6] = ((uint32_t)c >> 0) & 0xFF;
    ubReg[7] = ((uint32_t)c >> 8) & 0xFF;
    ubReg[8] = ((uint32_t)c >> 16) & 0xFF;
    ubReg[9] = ((uint32_t)c >> 24) & 0xFF;

    idt8v97003_write_register(IDT8V97003_REG_NINT_LOW, ubReg, 10);

    if(b > 0)
    {
        // Re-calculate the fPhase word since it depends and is limited by MOD
        uint32_t ulPrevMod = idt8v97003_read_register32(IDT8V97003_REG_NMOD_LOW); // Its ok to read after write because the register is buffered
        uint64_t fPhase = idt8v97003_read_register32(IDT8V97003_REG_PHASE_LOW);

        fPhase = ((fPhase * (uint64_t)c) / ulPrevMod);

        if(fPhase >= c)
            fPhase = c - 1;

        idt8v97003_write_register32(IDT8V97003_REG_PHASE_LOW, fPhase & 0xFFFFFFFF);

        ubManCtlReg |= IDT8V97003_REG_MANUAL_CTL_PH_ADJ;
    }

    idt8v97003_transfer_double_buffer();

    idt8v97003_rmw_register8(IDT8V97003_REG_DSM_CTL, 0x0F, (b > 0) ? IDT8V97003_REG_DSM_CTL_DSM_ORDER_3RD : IDT8V97003_REG_DSM_CTL_DSM_ORDER_OFF);

    uint8_t ubOutReg = 0;

    if(ubOutDoublerEn)
    {
        ubOutReg |= IDT8V97003_REG_OUT_DIV_DBL_OUT_DBL_ENA;

        if(fVCOFreq < 7e9)
            ubOutReg |= IDT8V97003_REG_OUT_DIV_DBL_OUT_DBL_FREQ;
    }

    if(ubM0Div > 0)
        ubOutReg |= IDT8V97003_REG_OUT_DIV_DBL_OUT_DIV_ENA | (ubM0Div & 0x07);

    idt8v97003_write_register8(IDT8V97003_REG_OUT_DIV_DBL, ubOutReg);
    idt8v97003_rmw_register8(IDT8V97003_REG_MANUAL_CTL, 0x0F, ubManCtlReg);

    // Force waiting for calibration completion when auto loop bandwidth setting is requested
    if(ubSetLoopBW && lCalTimeout < 0)
        lCalTimeout = 5; // 5 ms

    // Check if the calibration (band selection) completes
    if(lCalTimeout >= 0)
    {
        uint8_t ubStatus = idt8v97003_read_register8(IDT8V97003_REG_LD_CAL_VCO_STATUS);

        while(lCalTimeout > 0 && !(ubStatus & IDT8V97003_REG_LD_CAL_VCO_STATUS_BAND_SEL_DONE))
        {
            delay_ms(1);

            lCalTimeout -= 1;
            ubStatus = idt8v97003_read_register8(IDT8V97003_REG_LD_CAL_VCO_STATUS);
        }

        if(!(ubStatus & IDT8V97003_REG_LD_CAL_VCO_STATUS_BAND_SEL_DONE))
            return;

        if(ubSetLoopBW)
            idt8v97003_set_loop_bandwidth(0.0);
    }

    // Check if the PLL locks
    if(idt8v97003_read_register8(IDT8V97003_REG_LD_CTL0) & IDT8V97003_REG_LD_CTL0_LD_DISABLE)
    {
        if(lLockTimeout >= 0)
        {
            uint8_t ubStatus = idt8v97003_read_register8(IDT8V97003_REG_LD_CAL_VCO_STATUS);

            while(lLockTimeout > 0 && !(ubStatus & IDT8V97003_REG_LD_CAL_VCO_STATUS_DIG_LOCK))
            {
                delay_ms(1);

                lLockTimeout -= 1;
                ubStatus = idt8v97003_read_register8(IDT8V97003_REG_LD_CAL_VCO_STATUS);
            }

            if(!(ubStatus & IDT8V97003_REG_LD_CAL_VCO_STATUS_DIG_LOCK))
                return;
        }
    }
}

float idt8v97003_get_phase()
{
    uint32_t b = idt8v97003_read_register32(IDT8V97003_REG_NFRAC_LOW);

    if(!b)
        return 0.0; // fPhase adjustment not supported in integer mode (FRAC = 0)

    uint8_t ubReg[8];

    idt8v97003_read_register(IDT8V97003_REG_NMOD_LOW, ubReg, 8);

    uint32_t c = ubReg[0] | ((uint32_t)ubReg[1] << 8) | ((uint32_t)ubReg[2] << 16) | ((uint32_t)ubReg[3] << 24);

    if(c < 2)
        return 0.0;

    uint32_t ulPhase = ubReg[4] | ((uint32_t)ubReg[5] << 8) | ((uint32_t)ubReg[6] << 16) | ((uint32_t)ubReg[7] << 24);

    if(ulPhase > c)
        return 0.0;

    return (float)ulPhase / (float)c * 360.0;
}
void idt8v97003_set_phase(float fPhase)
{
    while(fPhase >= 360.0)
        fPhase -= 360.0;

    while(fPhase < 0.0)
        fPhase += 360.0;

    uint32_t b = idt8v97003_read_register32(IDT8V97003_REG_NFRAC_LOW);

    if(!b)
        return; // fPhase adjustment not supported in integer mode (FRAC = 0)

    uint32_t c = idt8v97003_read_register32(IDT8V97003_REG_NMOD_LOW);

    if(c < 2)
        return;

    uint32_t ulPhase = (uint32_t)roundf((float)c * fPhase / 360.0);

    idt8v97003_write_register32(IDT8V97003_REG_PHASE_LOW, ulPhase);
    idt8v97003_transfer_double_buffer();

    idt8v97003_rmw_register8(IDT8V97003_REG_MANUAL_CTL, (uint8_t)~IDT8V97003_REG_MANUAL_CTL_PH_ADJ, IDT8V97003_REG_MANUAL_CTL_PH_ADJ);
}