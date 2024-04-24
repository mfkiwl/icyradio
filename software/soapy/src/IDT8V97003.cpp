#include "IDT8V97003.hpp"

static const double IDT8V97003_kVCO[] = {100e6, 120e6, 140e6, 160e6, 210e6, 165e6, 155e6, 170e6};

void IDT8V97003::ValidateLoopFilter(IDT8V97003::LoopFilter filter)
{
    if(filter.rs <= 0.0)
        throw std::invalid_argument("8V97003: Invalid loop filter Rs value");

    if(filter.cs <= 0.0)
        throw std::invalid_argument("8V97003: Invalid loop filter Cs value");

    if(filter.cp <= 0.0)
        throw std::invalid_argument("8V97003: Invalid loop filter Cp value");

    if(filter.c3 > 0.0 && filter.r3 <= 0.0)
        throw std::invalid_argument("8V97003: Invalid loop filter R3 value");

    if(filter.r3 > 0.0 && filter.c3 <= 0.0)
        throw std::invalid_argument("8V97003: Invalid loop filter C3 value");
}

void IDT8V97003::readReg(uint8_t reg, uint8_t *dst, uint8_t count)
{
    if(this->spi.controller == nullptr)
        throw std::runtime_error("8V97003: SPI not initialized");

    if(reg + count > 0x49)
        throw std::invalid_argument("8V97003: Invalid register address");

    if(!count)
        return;

    if(dst == nullptr)
        throw std::invalid_argument("8V97003: Invalid destination buffer");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->write(BIT(7), false);
    this->spi.controller->write(reg, true);
    this->spi.controller->read(dst, count);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}
void IDT8V97003::writeReg(uint8_t reg, uint8_t *src, uint8_t count)
{
    if(this->spi.controller == nullptr)
        throw std::runtime_error("8V97003: SPI not initialized");

    if(reg + count > 0x49)
        throw std::invalid_argument("8V97003: Invalid register address");

    if(!count)
        return;

    if(src == nullptr)
        throw std::invalid_argument("8V97003: Invalid source buffer");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->write(0x00, false);
    this->spi.controller->write(reg, false);
    this->spi.controller->write(src, count, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}

IDT8V97003::IDT8V97003(IDT8V97003::SPIConfig spi, IDT8V97003::GPIOConfig ce_gpio, IDT8V97003::GPIOConfig mute_gpio, IDT8V97003::GPIOConfig sync_gpio, IDT8V97003::GPIOConfig ld_gpio, IDT8V97003::GPIOConfig reset_gpio)
{
    this->spi = spi;
    this->ce_gpio = ce_gpio;
    this->mute_gpio = mute_gpio;
    this->sync_gpio = sync_gpio;
    this->ld_gpio = ld_gpio;
    this->reset_gpio = reset_gpio;

    this->ref_freq = 0;
    this->cached_rfout_pwr[IDT8V97003::RFOutput::RFOUT_A] = 0;
    this->cached_rfout_pwr[IDT8V97003::RFOutput::RFOUT_B] = 0;
    this->loop_filter.rs = 0.0;
    this->loop_filter.cs = 0.0;
    this->loop_filter.cp = 0.0;
    this->loop_filter.r3 = 0.0;
    this->loop_filter.c3 = 0.0;
    this->target_loop_bw = 0.0;

    if(this->spi.controller == nullptr)
        throw std::runtime_error("8V97003: SPI not initialized");

    this->reset();

    this->writeReg(IDT8V97003_REG_INTF_CONFIG, IDT8V97003_REG_INTF_CONFIG_ADDR_ASC | IDT8V97003_REG_INTF_CONFIG_SDO_ACTIVE);

    uint8_t chip_type = this->readReg(IDT8V97003_REG_CHIP_TYPE);

    if(chip_type != 0x06)
        throw std::runtime_error("8V97003: Invalid chip type (" + std::to_string(chip_type) + " != " + std::to_string(0x06) + ")");

    uint16_t chip_id = this->readReg16(IDT8V97003_REG_CHIP_ID_LOW);

    if(chip_id != 0x0001)
        throw std::runtime_error("8V97003: Invalid chip ID (" + std::to_string(chip_id) + " != " + std::to_string(0x0001) + ")");

    uint16_t vendor_id = this->readReg16(IDT8V97003_REG_VENDOR_ID_LOW);

    if(vendor_id != 0x0426)
        throw std::runtime_error("8V97003: Invalid vendor ID (" + std::to_string(vendor_id) + " != " + std::to_string(0x0426) + ")");
}
IDT8V97003::~IDT8V97003()
{
    this->mute();
    this->powerDown(IDT8V97003::PowerFlags::PWR_ALL);
}

void IDT8V97003::init()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->reset();

    this->writeReg(IDT8V97003_REG_INTF_CONFIG, IDT8V97003_REG_INTF_CONFIG_ADDR_ASC | IDT8V97003_REG_INTF_CONFIG_SDO_ACTIVE);
    this->writeReg(IDT8V97003_REG_BUF_READ, 0x00); // Reads target the active register, not the buffer
    this->writeReg(IDT8V97003_REG_DSM_CTL, IDT8V97003_REG_DSM_CTL_SHAPE_DITHER_EN);
    this->writeReg(IDT8V97003_REG_MANUAL_VCO, 0x00);
    this->writeReg(IDT8V97003_REG_MANUAL_DIGITAL, 0x00);
    this->writeReg(IDT8V97003_REG_MANUAL_CTL, IDT8V97003_REG_MANUAL_CTL_BAND_SEL_ACC_8X);
    this->writeReg(0x24, 0x80);
    this->writeReg(0x25, 0x00);
    this->writeReg(IDT8V97003_REG_MULT_CTL0, 0x00);
    this->writeReg(IDT8V97003_REG_MULT_CTL1, IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCLOW);
    this->writeReg(IDT8V97003_REG_ICP_BLEEDER, 0x00);
    this->writeReg(IDT8V97003_REG_PFD_PULSE_WIDTH, IDT8V97003_REG_PFD_PULSE_WIDTH_VCO_BUF_LOW_V);
    this->writeReg(IDT8V97003_REG_RESYNC_TIME_LOW, 0x88); // resync_time = 5000
    this->writeReg(IDT8V97003_REG_RESYNC_TIME_HIGH, 0x13);
    this->writeReg(IDT8V97003_REG_RFOUTA_PWR, 0x00);
    this->writeReg(IDT8V97003_REG_RFOUTA_ENA, IDT8V97003_REG_RFOUTA_ENA_REF_VOLTAGE | IDT8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD | IDT8V97003_REG_RFOUTA_ENA_DAC_BIAS(14));
    this->writeReg(IDT8V97003_REG_RFOUTB_PWR, 0x00);
    this->writeReg(IDT8V97003_REG_RFOUTB_ENA, IDT8V97003_REG_RFOUTB_ENA_REF_VOLTAGE | IDT8V97003_REG_RFOUTB_ENA_DAC_BIAS(14));
    this->writeReg(IDT8V97003_REG_VCO_CAL_VOLTAGE, IDT8V97003_REG_VCO_CAL_VOLTAGE_0V82);
    this->writeReg(IDT8V97003_REG_PTAT_PWR_CONFIG, IDT8V97003_REG_PTAT_PWR_CONFIG_PTAT_MODE_NORMAL);
    this->writeReg(IDT8V97003_REG_CURRENT_CONFIG1, 0x00);
    this->writeReg(IDT8V97003_REG_CURRENT_CONFIG2, 0x00);
    this->writeReg(IDT8V97003_REG_TRIM_CONFIG1, IDT8V97003_REG_TRIM_CONFIG1_VCO_REG_TRIM_2V09 | IDT8V97003_REG_TRIM_CONFIG1_OUT_REG_TRIM_2V62);
    this->writeReg(IDT8V97003_REG_TRIM_CONFIG2, IDT8V97003_REG_TRIM_CONFIG2_PDCP_REG_TRIM_2V62 | IDT8V97003_REG_TRIM_CONFIG2_FB_REG_TRIM_2V62);
    this->writeReg(IDT8V97003_REG_TRIM_CONFIG3, IDT8V97003_REG_TRIM_CONFIG3_DIG_REG_TRIM_1V5 | IDT8V97003_REG_TRIM_CONFIG3_REF_REG_TRIM_2V62);

    this->transferDoubleBuffer();
}

void IDT8V97003::reset()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->reset_gpio.controller != nullptr) // Hardware reset
    {
        this->reset_gpio.controller->setValue(this->reset_gpio.gpio, this->reset_gpio.invert ? true : false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        this->reset_gpio.controller->setValue(this->reset_gpio.gpio, this->reset_gpio.invert ? false : true);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        return;
    }

    this->writeReg(IDT8V97003_REG_INTF_CONFIG, IDT8V97003_REG_INTF_CONFIG_SOFT_RESET);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void IDT8V97003::powerUp(IDT8V97003::PowerFlags flags)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if((flags & IDT8V97003::PowerFlags::PWR_CHIP_ENABLE) && this->ce_gpio.controller != nullptr)
        this->ce_gpio.controller->setValue(this->ce_gpio.gpio, this->ce_gpio.invert ? false : true);

    uint8_t reg = this->readReg(IDT8V97003_REG_PWR_CTL);
    bool reset_mult = false;

    if((reg & IDT8V97003_REG_PWR_CTL_REF_VREG_PDOWN) && (flags & IDT8V97003::PowerFlags::PWR_REF_VREG))
        reset_mult = true;

    reg &= (flags & IDT8V97003::PowerFlags::PWR_REF_VREG) ? ~IDT8V97003_REG_PWR_CTL_REF_VREG_PDOWN : 0xFF;
    reg &= (flags & IDT8V97003::PowerFlags::PWR_PDCP_VREG) ? ~IDT8V97003_REG_PWR_CTL_PDCP_VREG_PDOWN : 0xFF;
    reg &= (flags & IDT8V97003::PowerFlags::PWR_FB_VREG) ? ~IDT8V97003_REG_PWR_CTL_FB_VREG_PDOWN : 0xFF;
    reg &= (flags & IDT8V97003::PowerFlags::PWR_OUTA_VREG) ? ~IDT8V97003_REG_PWR_CTL_OUTA_VREG_PDOWN : 0xFF;
    reg &= (flags & IDT8V97003::PowerFlags::PWR_OUTB_BUF_VREG) ? ~IDT8V97003_REG_PWR_CTL_OUTB_BUF_VREG_PDOWN : 0xFF;
    reg &= (flags & IDT8V97003::PowerFlags::PWR_ANA_VREG) ? ~IDT8V97003_REG_PWR_CTL_ANA_PDOWN : 0xFF;
    reg |= (flags & IDT8V97003::PowerFlags::PWR_VCO) ? IDT8V97003_REG_PWR_CTL_VCO_EN : 0; // VCO is inverted

    this->writeReg(IDT8V97003_REG_PWR_CTL, reg);

    // Reset multiplier if enabled and reference path was just powered up
    if(!reset_mult)
        return;

    reg = this->readReg(IDT8V97003_REG_MULT_CTL0);

    if(!(reg & IDT8V97003_REG_MULT_CTL0_MULT_EN))
        return;

    this->writeReg(IDT8V97003_REG_MULT_CTL0, reg | IDT8V97003_REG_MULT_CTL0_MULT_RESET);
    this->transferDoubleBuffer();
    this->writeReg(IDT8V97003_REG_MULT_CTL0, reg & ~IDT8V97003_REG_MULT_CTL0_MULT_RESET);
    this->transferDoubleBuffer();
}
void IDT8V97003::powerDown(IDT8V97003::PowerFlags flags)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t reg = this->readReg(IDT8V97003_REG_PWR_CTL);

    reg |= (flags & IDT8V97003::PowerFlags::PWR_REF_VREG) ? IDT8V97003_REG_PWR_CTL_REF_VREG_PDOWN : 0;
    reg |= (flags & IDT8V97003::PowerFlags::PWR_PDCP_VREG) ? IDT8V97003_REG_PWR_CTL_PDCP_VREG_PDOWN : 0;
    reg |= (flags & IDT8V97003::PowerFlags::PWR_FB_VREG) ? IDT8V97003_REG_PWR_CTL_FB_VREG_PDOWN : 0;
    reg |= (flags & IDT8V97003::PowerFlags::PWR_OUTA_VREG) ? IDT8V97003_REG_PWR_CTL_OUTA_VREG_PDOWN : 0;
    reg |= (flags & IDT8V97003::PowerFlags::PWR_OUTB_BUF_VREG) ? IDT8V97003_REG_PWR_CTL_OUTB_BUF_VREG_PDOWN : 0;
    reg |= (flags & IDT8V97003::PowerFlags::PWR_ANA_VREG) ? IDT8V97003_REG_PWR_CTL_ANA_PDOWN : 0;
    reg &= (flags & IDT8V97003::PowerFlags::PWR_VCO) ? ~IDT8V97003_REG_PWR_CTL_VCO_EN : 0xFF; // VCO is inverted

    this->writeReg(IDT8V97003_REG_PWR_CTL, reg);

    if((flags & IDT8V97003::PowerFlags::PWR_CHIP_ENABLE) && this->ce_gpio.controller != nullptr)
        this->ce_gpio.controller->setValue(this->ce_gpio.gpio, this->ce_gpio.invert ? true : false);
}

bool IDT8V97003::isMuted()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->mute_gpio.controller != nullptr)
    {
        bool muted = this->mute_gpio.controller->getValue(this->mute_gpio.gpio);

        if(this->mute_gpio.invert)
            muted = !muted;

        if(muted)
            return true;
    }

    return this->isMuted(IDT8V97003::RFOutput::RFOUT_A) && this->isMuted(IDT8V97003::RFOutput::RFOUT_B);
}
void IDT8V97003::mute(bool mute)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->mute_gpio.controller != nullptr)
    {
        if(this->mute_gpio.invert)
            mute = !mute;

        this->mute_gpio.controller->setValue(this->mute_gpio.gpio, mute);

        return;
    }

    this->mute(IDT8V97003::RFOutput::RFOUT_A, mute);
    this->mute(IDT8V97003::RFOutput::RFOUT_B, mute);
}
bool IDT8V97003::isMuted(IDT8V97003::RFOutput output)
{
    uint8_t reg;

    switch(output)
    {
        case IDT8V97003::RFOutput::RFOUT_A:
            reg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case IDT8V97003::RFOutput::RFOUT_B:
            reg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid RF output");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->mute_gpio.controller != nullptr)
    {
        bool muted = this->mute_gpio.controller->getValue(this->mute_gpio.gpio);

        if(this->mute_gpio.invert)
            muted = !muted;

        if(muted)
            return true;
    }

    uint8_t val = this->readReg(reg) & 0x0F;

    if(!val)
        return true;

    return false;
}
void IDT8V97003::mute(IDT8V97003::RFOutput output, bool mute)
{
    uint8_t reg;

    switch(output)
    {
        case IDT8V97003::RFOutput::RFOUT_A:
            reg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case IDT8V97003::RFOutput::RFOUT_B:
            reg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid RF output");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(mute && this->cached_rfout_pwr[output])
        return; // Already muted

    if(!mute && !this->cached_rfout_pwr[output])
        return; // Already unmuted

    uint8_t val = this->readReg(reg);
    uint8_t pwr = val & 0x0F;

    if(mute)
    {
        if(!pwr)
            return; // Already muted

        this->cached_rfout_pwr[output] = pwr;

        pwr = 0;
    }
    else
    {
        if(pwr)
            return; // Already unmuted

        pwr = this->cached_rfout_pwr[output];

        this->cached_rfout_pwr[output] = 0;
    }

    this->writeReg(reg, (val & 0xF0) | pwr);
}
void IDT8V97003::setMuteUntilLocked(bool mute)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->rmwReg(IDT8V97003_REG_RFOUTA_ENA, IDT8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD, mute ? IDT8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD : 0);
}

void IDT8V97003::enableRFOutput(IDT8V97003::RFOutput output, bool enable)
{
    uint8_t reg;

    switch(output)
    {
        case IDT8V97003::RFOutput::RFOUT_A:
            reg = IDT8V97003_REG_RFOUTA_ENA;
        break;
        case IDT8V97003::RFOutput::RFOUT_B:
            reg = IDT8V97003_REG_RFOUTB_ENA;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid RF output");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->rmwReg(reg, IDT8V97003_REG_RFOUTA_ENA_RFOUTA_ENA, enable ? IDT8V97003_REG_RFOUTA_ENA_RFOUTA_ENA : 0);
}
void IDT8V97003::setRFOutputPower(IDT8V97003::RFOutput output, uint8_t power)
{
    if(power > 12)
        throw std::invalid_argument("8V97003: Invalid RF output power (Valid: 0-12)");

    uint8_t reg;

    switch(output)
    {
        case IDT8V97003::RFOutput::RFOUT_A:
            reg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case IDT8V97003::RFOutput::RFOUT_B:
            reg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid RF output");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->writeReg(reg, power & 0x0F);

    this->cached_rfout_pwr[output] = 0;
}
uint8_t IDT8V97003::getRFOutputPower(IDT8V97003::RFOutput output)
{
    uint8_t reg;

    switch(output)
    {
        case IDT8V97003::RFOutput::RFOUT_A:
            reg = IDT8V97003_REG_RFOUTA_PWR;
        break;
        case IDT8V97003::RFOutput::RFOUT_B:
            reg = IDT8V97003_REG_RFOUTB_PWR;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid RF output");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t pwr = this->readReg(reg) & 0x0F;

    if(!pwr)
        return this->cached_rfout_pwr[output];

    return pwr;
}

double IDT8V97003::getTemperature()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    // TODO
    return 0.0;

    // static bool once = false;

    // if(!once)
    // {
    //     this->init();
    //     this->powerUp();

    //     this->configReferenceInput(25000000UL);
    //     this->configPFD(250000000UL);
    //     this->setLockDetectPrecision(IDT8V97003::LDPrecision::LD_PREC_6p4ns);
    //     this->enableAutoRecal();
    //     this->enableLockDetect();
    //     this->setChargePumpPositiveCurrent(9);
    //     this->setChargePumpNegativeCurrent(9);
    //     this->setFrequency(1000000000ULL);
    // }

    // once = true;

    // this->writeReg(IDT8V97003_REG_PTAT_PWR_CONFIG, IDT8V97003_REG_PTAT_PWR_CONFIG_SENSOR_POWER | BIT(0));
    // uint16_t temp = this->readReg(IDT8V97003_REG_TEMP_HIGH);

    // uint32_t to = 10000;
    // while(--to && !(temp & 0x20))
    // {
    //     usleep(1000);

    //     temp = this->readReg(IDT8V97003_REG_TEMP_HIGH);
    // }

    // temp = this->readReg16(IDT8V97003_REG_TEMP_LOW);

    // this->writeReg(IDT8V97003_REG_PTAT_PWR_CONFIG, IDT8V97003_REG_PTAT_PWR_CONFIG_SENSOR_POWER);
    // this->writeReg(IDT8V97003_REG_PTAT_PWR_CONFIG, IDT8V97003_REG_PTAT_PWR_CONFIG_SENSOR_POWER | BIT(0));

    // if(!to)
    //     return -1;

    // // static bool once = false;

    // // if(once)
    // //     this->powerUp();
    // // else
    // //     this->powerDown();

    // // once = !once;

    // // temp &= 0xFFF;

    // // if(temp & 0x08000)
    // //     temp |= 0xF000;

    // // int16_t stemp;
    // // *((uint16_t *)&stemp) = temp;

    // return (double)(temp & 0x3FF); // TODO: Calibrate
}

IDT8V97003::VCOBand IDT8V97003::getCurrentVCOBand()
{
    uint8_t buf[2];

    this->readReg(IDT8V97003_REG_LD_CAL_VCO_STATUS, buf, 2);

    return {
        .vco = (uint8_t)(buf[0] & IDT8V97003_REG_LD_CAL_VCO_STATUS_VCO_STS),
        .dig_band = (uint8_t)(buf[1] & 0x7F)
    };
}
void IDT8V97003::forceVCOBand(bool force, IDT8V97003::VCOBand band)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    // TODO
}

void IDT8V97003::configReferenceInput(double freq, bool diff)
{
    if(freq < 10e6)
        throw std::runtime_error("8V97003: Reference frequency too low (Valid: >= 10 MHz)");

    if(freq > 1.6e9)
        throw std::runtime_error("8V97003: Reference frequency too high (Valid: <= 1600 MHz)");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->rmwReg(IDT8V97003_REG_RDIV_HIGH, (uint8_t)~(IDT8V97003_REG_RDIV_HIGH_REF_DBL_DELAY | IDT8V97003_REG_RDIV_HIGH_INPUT_TYPE), ((freq < 50000000UL) ? IDT8V97003_REG_RDIV_HIGH_REF_DBL_DELAY : 0) | (diff ? IDT8V97003_REG_RDIV_HIGH_INPUT_TYPE : 0));

    this->ref_freq = freq;
}
void IDT8V97003::configPFD(double freq, IDT8V97003::PFDPulseWidth pw)
{
    if(freq > 500e6)
        throw std::invalid_argument("8V97003: PFD frequency too high (Valid: <= 500 MHz)");

    if(this->ref_freq < 10e6)
        throw std::runtime_error("8V97003: Reference frequency too low (Valid: >= 10 MHz)");

    uint8_t found = 0;
    IDT8V97003::RefPathConfig ref_cfg;
    double min_diff = INFINITY;

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    for(uint8_t dbl = 2; dbl > 0; dbl--) // First try with the doubler enabled
    {
        for(uint8_t mult = 1; mult < 64; mult++)
        {
            if(mult == 2)
                continue;

            double mult_out_freq = this->ref_freq * dbl * mult;

            if(mult > 1 && (mult_out_freq < 160e6 || mult_out_freq > 250e6))
                continue;

            for(uint16_t r_div = 1; r_div < 1024; r_div++)
            {
                double pfd_freq = mult_out_freq / r_div;
                double diff = ABS(pfd_freq - freq);

                if(diff < min_diff)
                {
                    ref_cfg.doubler_en = (dbl == 2);
                    ref_cfg.mult = mult;
                    ref_cfg.r_div = r_div;

                    min_diff = diff;
                    found = 1;

                    if(diff == 0)
                    {
                        found = 2; // Found exact match

                        break;
                    }
                }
            }

            if(found > 1)
                break;
        }

        if(found > 1)
            break;
    }

    if(found == 0)
        throw std::runtime_error("8V97003: Could not find any valid PFD configuration");

    this->configPFD(ref_cfg, pw);
}
void IDT8V97003::configPFD(IDT8V97003::RefPathConfig ref_cfg, IDT8V97003::PFDPulseWidth pw)
{
    uint8_t pw_val;

    switch(pw)
    {
        case IDT8V97003::PFDPulseWidth::PFD_PW_260ps:
            pw_val = IDT8V97003_REG_PFD_PULSE_WIDTH_PW_260ps;
        break;
        case IDT8V97003::PFDPulseWidth::PFD_PW_348ps:
            pw_val = IDT8V97003_REG_PFD_PULSE_WIDTH_PW_348ps;
        break;
        case IDT8V97003::PFDPulseWidth::PFD_PW_487ps:
            pw_val = IDT8V97003_REG_PFD_PULSE_WIDTH_PW_487ps;
        break;
        case IDT8V97003::PFDPulseWidth::PFD_PW_583ps:
            pw_val = IDT8V97003_REG_PFD_PULSE_WIDTH_PW_583ps;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid PFD pulse width");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->ref_freq < 10e6)
        throw std::runtime_error("8V97003: Reference frequency too low (Valid: >= 10 MHz)");

    if(ref_cfg.mult < 1 || ref_cfg.mult == 2 || ref_cfg.mult > 63)
        throw std::invalid_argument("8V97003: Invalid multiplier (Valid: 1, 3-63)");

    if(ref_cfg.r_div < 1 || ref_cfg.r_div > 1023)
        throw std::invalid_argument("8V97003: Invalid R divider (Valid: 1-1023)");

    if(this->ref_freq > (ref_cfg.doubler_en ? 250e6 : 1.6e9))
        throw std::runtime_error("8V97003: Reference frequency too high (Valid: <= " + std::to_string(ref_cfg.doubler_en ? 250 : 1600) + " MHz)");

    double mult_out_freq = this->ref_freq * (ref_cfg.doubler_en ? 2 : 1) * ref_cfg.mult;

    if(ref_cfg.mult > 1 && (mult_out_freq < 160e6 || mult_out_freq > 250e6))
            throw std::runtime_error("8V97003: Multiplier output frequency out of range (Valid: 160-250 MHz)");

    double pfd_freq = mult_out_freq / ref_cfg.r_div;

    if(pfd_freq > 500e6)
        throw std::runtime_error("8V97003: PFD frequency too high (Valid: <= 500 MHz)");

    // Calculate band select clock divider
    double band_sel_clock = pfd_freq;
    uint16_t band_sel_div = 1;

    while(band_sel_div < 8192)
    {
        if(band_sel_clock >= 50e3 && band_sel_clock <= 100e3)
            break;

        band_sel_div++;
        band_sel_clock = pfd_freq / band_sel_div;
    }

    if(band_sel_div >= 8192)
        throw std::runtime_error("8V97003: Could not find a suitable band select clock divider");

    if(ref_cfg.mult > 1)
    {
        // Multiplier enabled, enable and reset
        this->rmwReg(IDT8V97003_REG_MULT_CTL1, 0x07, IDT8V97003_REG_MULT_CTL1_MULT_MUX_ENA | IDT8V97003_REG_MULT_CTL1_MULT_D2S_ENA | IDT8V97003_REG_MULT_CTL1_MULT_CP_ENA);
        this->writeReg(IDT8V97003_REG_MULT_CTL0, IDT8V97003_REG_MULT_CTL0_MULT_EN | IDT8V97003_REG_MULT_CTL0_MULT_RESET);

        this->transferDoubleBuffer(); // Ensure it is enabled and properly reset

        // Program the multiplication value (does not apply until the next buffer transfer)
        this->writeReg(IDT8V97003_REG_MULT_CTL0, IDT8V97003_REG_MULT_CTL0_MULT_EN | (ref_cfg.mult & 0x3F));
    }
    else
    {
        // Multiplier disabled, disable and force low control voltage (does not apply until the next buffer transfer)
        this->writeReg(IDT8V97003_REG_MULT_CTL0, 0);
        this->rmwReg(IDT8V97003_REG_MULT_CTL1, 0x07, IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCLOW);
    }

    this->writeReg(IDT8V97003_REG_RDIV_LOW, ref_cfg.r_div & 0xFF);
    this->rmwReg(IDT8V97003_REG_RDIV_HIGH, IDT8V97003_REG_RDIV_HIGH_REF_DBL_DELAY | IDT8V97003_REG_RDIV_HIGH_INPUT_TYPE, (ref_cfg.doubler_en ? IDT8V97003_REG_RDIV_HIGH_REF_DBL_EN : 0) | ((ref_cfg.r_div >> 8) & 0x03));

    this->writeReg16(IDT8V97003_REG_BAND_SEL_DIV_LOW, band_sel_div & 0x1FFF);

    this->transferDoubleBuffer();

    this->rmwReg(IDT8V97003_REG_PFD_PULSE_WIDTH, ~0x0C, (pw_val & 0x0C));
}
double IDT8V97003::getReferenceDoublerInputFrequency()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t reg = this->readReg(IDT8V97003_REG_RDIV_HIGH);

    if(!(reg & IDT8V97003_REG_RDIV_HIGH_REF_DBL_EN))
        return 0.0;

    return this->ref_freq;
}
double IDT8V97003::getReferenceMultiplierInputFrequency()
{
    uint8_t reg[2];

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->readReg(IDT8V97003_REG_MULT_CTL0, reg, 2);

    if(!(reg[1] & IDT8V97003_REG_MULT_CTL1_MULT_MUX_ENA))
        return 0.0;

    if(!(reg[1] & IDT8V97003_REG_MULT_CTL1_MULT_D2S_ENA))
        return 0.0;

    if(!(reg[1] & IDT8V97003_REG_MULT_CTL1_MULT_CP_ENA))
        return 0.0;

    if(reg[1] & IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCLOW)
        return 0.0;

    if(reg[1] & IDT8V97003_REG_MULT_CTL1_MULT_FORCE_VCHI)
        return 0.0;

    if(!(reg[0] & IDT8V97003_REG_MULT_CTL0_MULT_EN))
        return 0.0;

    if(reg[0] & IDT8V97003_REG_MULT_CTL0_MULT_RESET)
        return 0.0;

    double mult_in = this->getReferenceDoublerOutputFrequency();

    if(mult_in == 0.0)
        mult_in = this->ref_freq;

    return mult_in;
}
double IDT8V97003::getReferenceMultiplierOutputFrequency()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t mult = this->readReg(IDT8V97003_REG_MULT_CTL0) & 0x3F;

    if(!mult)
        return 0.0;

    return this->getReferenceMultiplierInputFrequency() * mult;
}
double IDT8V97003::getReferenceDividerInputFrequency()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    double ref_div_in = this->getReferenceMultiplierOutputFrequency();

    if(ref_div_in == 0.0)
        ref_div_in = this->getReferenceDoublerOutputFrequency();

    if(ref_div_in == 0.0)
        ref_div_in = this->ref_freq;

    return ref_div_in;
}
double IDT8V97003::getReferenceDividerOutputFrequency()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint16_t r_div = this->readReg16(IDT8V97003_REG_RDIV_LOW) & 0x3FF;

    if(!r_div)
        r_div = 1;

    return this->getReferenceDividerInputFrequency() / r_div;
}

IDT8V97003::LoopFilter IDT8V97003::getLoopFilter()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    try
    {
        IDT8V97003::ValidateLoopFilter(this->loop_filter);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error("8V97003: Current loop filter is not valid");
    }

    return this->loop_filter;
}
void IDT8V97003::setLoopFilter(IDT8V97003::LoopFilter filter)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->loop_filter = filter;
}
IDT8V97003::LoopFrequencyResponse IDT8V97003::getLoopFrequencyResponse()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    IDT8V97003::LoopFilter lf = this->getLoopFilter();
    IDT8V97003::ChargePumpConfig cp = this->getChargePumpConfig();
    double icp = (cp.icp_pmos + cp.icp_nmos) / 2.0;
    uint8_t vco = this->getCurrentVCO();
    double n = this->getFeedbackDivider();

    IDT8V97003::LoopFrequencyResponse resp;

    resp.fz = 1.0 / (2.0 * M_PI * lf.rs * lf.cs);
    resp.fc = lf.rs * icp * IDT8V97003_kVCO[vco] / (2.0 * M_PI * n);
    resp.fp = 1.0 / (2.0 * M_PI * lf.rs * lf.cp);

    if(lf.r3 > 0.0 && lf.c3 > 0.0)
        resp.fp2 = 1.0 / (2.0 * M_PI * lf.r3 * lf.c3);
    else
        resp.fp2 = 0.0;

    double b = 1.0 + lf.cs / lf.cp;

    resp.phase_margin = std::atan(b - 1.0 / (2.0 * std::sqrt(b))) * 180.0 / M_PI;

    return resp;
}
double IDT8V97003::getLoopBandwidth()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    IDT8V97003::LoopFilter lf = this->getLoopFilter();
    IDT8V97003::ChargePumpConfig cp = this->getChargePumpConfig();
    double icp = (cp.icp_pmos + cp.icp_nmos) / 2.0;
    uint8_t vco = this->getCurrentVCO();
    double n = this->getFeedbackDivider();

    return lf.rs * icp * IDT8V97003_kVCO[vco] / (2.0 * M_PI * n);
}
void IDT8V97003::setLoopBandwidth(double bw)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(bw <= 0.0)
    {
        try
        {
            bw = this->getTargetLoopBandwidth();
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("8V97003: Loop bandwidth is not valid and target loop bandwidth is not set");
        }
    }

    IDT8V97003::LoopFilter lf = this->getLoopFilter();
    uint8_t vco = this->getCurrentVCO();
    double n = this->getFeedbackDivider();

    double icp = bw * 2.0 * M_PI * n / (lf.rs * IDT8V97003_kVCO[vco]);

    this->setChargePumpConfig(
        {
            .icp_pmos = icp,
            .icp_nmos = icp,
            .icp_bleeder = -1.0 // Do not touch the bleeder current
        }
    );
}
double IDT8V97003::getTargetLoopBandwidth()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->target_loop_bw <= 0.0)
        throw std::runtime_error("8V97003: Target loop bandwidth is not valid");

    return this->target_loop_bw;
}
void IDT8V97003::setTargetLoopBandwidth(double bw)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->target_loop_bw = bw;
}

IDT8V97003::ChargePumpConfig IDT8V97003::getChargePumpConfig()
{
    uint8_t buf[3];

    this->readReg(IDT8V97003_REG_ICP_PMOS, buf, 3);

    return {
        .icp_pmos = (double)((buf[0] & 0x3F) + 1) / 6.0 * 1e-3,
        .icp_nmos = (double)((buf[1] & 0x3F) + 1) / 6.0 * 1e-3,
        .icp_bleeder = (double)(buf[2] & 0x7F) * 20e-6
    };
}
void IDT8V97003::setChargePumpConfig(IDT8V97003::ChargePumpConfig cfg)
{
    cfg.icp_pmos = std::round(cfg.icp_pmos * 6.0 / 1e-3) - 1;
    cfg.icp_pmos = CLAMP(cfg.icp_pmos, 0, 63);

    cfg.icp_nmos = std::round(cfg.icp_nmos * 6.0 / 1e-3) - 1;
    cfg.icp_nmos = CLAMP(cfg.icp_nmos, 0, 63);

    bool write_bleeder = cfg.icp_bleeder >= 0.0;
    cfg.icp_bleeder = std::round(cfg.icp_bleeder / 20e-6);
    cfg.icp_bleeder = CLAMP(cfg.icp_bleeder, 0, 127);

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t buf[3];

    buf[0] = (uint8_t)cfg.icp_pmos & 0x3F;
    buf[1] = (uint8_t)cfg.icp_nmos & 0x3F;
    buf[2] = (uint8_t)cfg.icp_bleeder & 0x7F;

    if(write_bleeder)
        buf[2] |= this->readReg(IDT8V97003_REG_ICP_BLEEDER) & IDT8V97003_REG_ICP_BLEEDER_CP_HIZ;

    this->writeReg(IDT8V97003_REG_ICP_PMOS, buf, write_bleeder ? 3 : 2);
}
double IDT8V97003::getChargePumpPositiveCurrent()
{
    uint8_t reg = this->readReg(IDT8V97003_REG_ICP_PMOS);

    return (double)((reg & 0x3F) + 1) / 6.0 * 1e-3;
}
void IDT8V97003::setChargePumpPositiveCurrent(double current)
{
    current = std::round(current * 6.0 / 1e-3) - 1;
    current = CLAMP(current, 0, 63);

    this->writeReg(IDT8V97003_REG_ICP_PMOS, (uint8_t)current & 0x3F);
}
double IDT8V97003::getChargePumpNegativeCurrent()
{
    uint8_t reg = this->readReg(IDT8V97003_REG_ICP_NMOS);

    return (double)((reg & 0x3F) + 1) / 6.0 * 1e-3;
}
void IDT8V97003::setChargePumpNegativeCurrent(double current)
{
    current = std::round(current * 6.0 / 1e-3) - 1;
    current = CLAMP(current, 0, 63);

    this->writeReg(IDT8V97003_REG_ICP_NMOS, (uint8_t)current & 0x3F);
}
double IDT8V97003::getChargePumpBleederCurrent()
{
    uint8_t reg = this->readReg(IDT8V97003_REG_ICP_BLEEDER);

    return (double)(reg & 0x7F) * 20e-6;
}
void IDT8V97003::setChargePumpBleederCurrent(double current)
{
    current = std::round(current / 20e-6);
    current = CLAMP(current, 0, 127);

    this->writeReg(IDT8V97003_REG_ICP_BLEEDER, (uint8_t)current & 0x7F);
}
void IDT8V97003::enableChargePump(bool enable)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->rmwReg(IDT8V97003_REG_ICP_BLEEDER, (uint8_t)~IDT8V97003_REG_ICP_BLEEDER_CP_HIZ, enable ? 0 : IDT8V97003_REG_ICP_BLEEDER_CP_HIZ);
}
bool IDT8V97003::isChargePumpEnabled()
{
    uint8_t reg = this->readReg(IDT8V97003_REG_ICP_BLEEDER);

    return !(reg & IDT8V97003_REG_ICP_BLEEDER_CP_HIZ);
}

void IDT8V97003::enableLockDetect(bool enable)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    // Yes, it looks like it is inverted (DISABLE = 1 is actually enabled...)
    this->rmwReg(IDT8V97003_REG_LD_CTL0, (uint8_t)~IDT8V97003_REG_LD_CTL0_LD_DISABLE, enable ? IDT8V97003_REG_LD_CTL0_LD_DISABLE : 0);
}
void IDT8V97003::enableAutoRecal(bool enable)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->rmwReg(IDT8V97003_REG_LD_CTL0, (uint8_t)~IDT8V97003_REG_LD_CTL0_AUTO_RECAL_EN, enable ? IDT8V97003_REG_LD_CTL0_AUTO_RECAL_EN : 0);
}
IDT8V97003::LDPrecision IDT8V97003::getLockDetectPrecision()
{
    uint8_t reg = this->readReg(IDT8V97003_REG_LD_CTL1) & 0x07;

    switch(reg)
    {
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_0p5ns:
            return IDT8V97003::LDPrecision::LD_PREC_0p5ns;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p0ns:
            return IDT8V97003::LDPrecision::LD_PREC_1p0ns;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p8ns:
            return IDT8V97003::LDPrecision::LD_PREC_1p8ns;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_3p0ns:
            return IDT8V97003::LDPrecision::LD_PREC_3p0ns;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_6p4ns:
        case 0x05: // Repeated setting
            return IDT8V97003::LDPrecision::LD_PREC_6p4ns;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PRECISION_10p4ns:
        case 0x07: // Repeated setting
            return IDT8V97003::LDPrecision::LD_PREC_10p4ns;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid lock detect precision");
    }
}
void IDT8V97003::setLockDetectPrecision(IDT8V97003::LDPrecision prec)
{
    uint8_t reg;

    switch(prec)
    {
        case IDT8V97003::LDPrecision::LD_PREC_0p5ns:
            reg = IDT8V97003_REG_LD_CTL1_LD_PRECISION_0p5ns;
        break;
        case IDT8V97003::LDPrecision::LD_PREC_1p0ns:
            reg = IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p0ns;
        break;
        case IDT8V97003::LDPrecision::LD_PREC_1p8ns:
            reg = IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p8ns;
        break;
        case IDT8V97003::LDPrecision::LD_PREC_3p0ns:
            reg = IDT8V97003_REG_LD_CTL1_LD_PRECISION_3p0ns;
        break;
        case IDT8V97003::LDPrecision::LD_PREC_6p4ns:
            reg = IDT8V97003_REG_LD_CTL1_LD_PRECISION_6p4ns;
        break;
        case IDT8V97003::LDPrecision::LD_PREC_10p4ns:
            reg = IDT8V97003_REG_LD_CTL1_LD_PRECISION_10p4ns;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid lock detect precision");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->rmwReg(IDT8V97003_REG_LD_CTL1, ~0x07, reg);
}
IDT8V97003::LDPinMode IDT8V97003::getLockDetectPinMode()
{
    uint8_t reg = this->readReg(IDT8V97003_REG_LD_CTL1) & 0x30;

    switch(reg)
    {
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LD:
            return IDT8V97003::LDPinMode::LD_MODE_LD;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_CAL_DONE:
            return IDT8V97003::LDPinMode::LD_MODE_CAL_DONE;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LOW:
            return IDT8V97003::LDPinMode::LD_MODE_LOW;
        break;
        case IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_HIGH:
            return IDT8V97003::LDPinMode::LD_MODE_HIGH;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid lock detect pin mode");
    }
}
void IDT8V97003::setLockDetectPinMode(IDT8V97003::LDPinMode mode)
{
    uint8_t reg;

    switch(mode)
    {
        case IDT8V97003::LDPinMode::LD_MODE_LD:
            reg = IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LD;
        break;
        case IDT8V97003::LDPinMode::LD_MODE_CAL_DONE:
            reg = IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_CAL_DONE;
        break;
        case IDT8V97003::LDPinMode::LD_MODE_LOW:
            reg = IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LOW;
        break;
        case IDT8V97003::LDPinMode::LD_MODE_HIGH:
            reg = IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_HIGH;
        break;
        default:
            throw std::runtime_error("8V97003: Invalid lock detect pin mode");
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->rmwReg(IDT8V97003_REG_LD_CTL1, ~0x30, reg);
}

double IDT8V97003::getFeedbackDivider()
{
    uint8_t buf[10];

    this->readReg(IDT8V97003_REG_NINT_LOW, buf, 10);

    uint16_t a = buf[0] | ((uint16_t)buf[1] << 8);
    uint32_t b = buf[2] | ((uint32_t)buf[3] << 8) | ((uint32_t)buf[4] << 16) | ((uint32_t)buf[5] << 24);
    uint32_t c = buf[6] | ((uint32_t)buf[7] << 8) | ((uint32_t)buf[8] << 16) | ((uint32_t)buf[9] << 24);

    if(!b || !c || b >= c)
        return (double)a;

    return (double)a + (double)b / (double)c;
}
bool IDT8V97003::isFeedbackDividerFractional(double &dist)
{
    uint8_t buf[10];

    this->readReg(IDT8V97003_REG_NFRAC_LOW, buf, 8);

    uint32_t b = buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);
    uint32_t c = buf[4] | ((uint32_t)buf[5] << 8) | ((uint32_t)buf[6] << 16) | ((uint32_t)buf[7] << 24);

    if(!b || !c || b >= c)
        return false;

    double frac = (double)b / (double)c;

    if(frac < 0.5)
        dist = frac;
    else
        dist = 1.0 - frac;

    return true;
}

double IDT8V97003::getVCOFrequency()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    double pfd_freq = this->getPFDFrequency();

    if(pfd_freq == 0.0)
        return 0.0;

    return pfd_freq * this->getFeedbackDivider();
}
double IDT8V97003::getFrequency()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    double vco_freq = this->getVCOFrequency();

    if(vco_freq == 0.0)
        return 0.0;

    uint8_t reg = this->readReg(IDT8V97003_REG_OUT_DIV_DBL);

    if(reg & IDT8V97003_REG_OUT_DIV_DBL_OUT_DBL_ENA)
        return vco_freq * 2.0;

    if(reg & IDT8V97003_REG_OUT_DIV_DBL_OUT_DIV_ENA)
    {
        uint8_t div = reg & 0x07;

        if(div == 0 || div > 5)
            throw std::runtime_error("8V97003: Invalid output divider");

        return vco_freq / BIT(div);
    }

    return vco_freq;
}
void IDT8V97003::setFrequency(double freq, bool set_loop_bw, int32_t cal_timeout, int32_t lock_timeout)
{
    if(freq < 171.875e6)
        throw std::invalid_argument("8V97003: Frequency too low (Valid: >= 171.875 MHz)");

    if(freq > 18e9)
        throw std::invalid_argument("8V97003: Frequency too high (Valid: <= 18 GHz)");

    // VCO works from 5.5 GHz to 11 GHz
    // For frequencies below 5.5 GHz, the VCO divider must be used
    // For frequencies above 11 GHz, the VCO doubler must be used
    uint8_t m0_div = 0;
    bool out_dbl_en = false;
    double vco_freq;

    if(freq < 5.5e9)
    {
        for(uint8_t i = 1; i < 6; i++)
        {
            vco_freq = freq * BIT(i);

            if(vco_freq >= 5.5e9 && vco_freq <= 11e9) // VCO operating range
            {
                m0_div = i;

                break;
            }
        }

        if(!m0_div)
            throw std::runtime_error("8V97003: Could not find a suitable VCO output divider");
    }
    else if(freq > 11e9)
    {
        vco_freq = freq / 2;
        out_dbl_en = true;
    }
    else
    {
        vco_freq = freq;
    }

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    double pfd_freq = this->getPFDFrequency();

    if(pfd_freq == 0.0)
        throw std::runtime_error("8V97003: Invalid PFD frequency (Did you configure the PFD first?)");

    double b = vco_freq;
    double c = pfd_freq;
    double a = Utils::GetMixedNumber(b, c);

    while(c >= BIT(32))
    {
        b /= 2.0;
        c /= 2.0;
    }

    if(b > 0.0 && pfd_freq > 250e6)
        throw std::runtime_error("8V97003: Fractional mode not supported with PFD above 250 MHz");

    if(b >= c) // Sanity check
        throw std::runtime_error("8V97003: Fractional part out of range (" + std::to_string(b) + " / " + std::to_string(c) + ", Valid: < 1)");

    if(a > (BIT(16) - 1) || a < 12) // Integer limits (min 12, 16-bit wide)
        throw std::runtime_error("8V97003: Integer part out of range (" + std::to_string(a) + ", Valid: 12-65535)");

    if(b > 0.0)
    {
        while(c < BIT(31)) // Maximize MOD and FRAC to improve phase noise
        {
            c *= 2.0;
            b *= 2.0;
        }
    }
    else
    {
        c = 2.0;
    }

    a = std::round(a);
    b = std::round(b);
    c = std::round(c);

    uint8_t man_ctl_reg = IDT8V97003_REG_MANUAL_CTL_FORCE_RELOCK | IDT8V97003_REG_MANUAL_CTL_MANUAL_RESYNC;

    uint8_t buf[10];

    buf[0] = ((uint16_t)a >> 0) & 0xFF;
    buf[1] = ((uint16_t)a >> 8) & 0xFF;
    buf[2] = ((uint32_t)b >> 0) & 0xFF;
    buf[3] = ((uint32_t)b >> 8) & 0xFF;
    buf[4] = ((uint32_t)b >> 16) & 0xFF;
    buf[5] = ((uint32_t)b >> 24) & 0xFF;
    buf[6] = ((uint32_t)c >> 0) & 0xFF;
    buf[7] = ((uint32_t)c >> 8) & 0xFF;
    buf[8] = ((uint32_t)c >> 16) & 0xFF;
    buf[9] = ((uint32_t)c >> 24) & 0xFF;

    this->writeReg(IDT8V97003_REG_NINT_LOW, buf, 10);

    if(b > 0)
    {
        // Re-calculate the phase word since it depends and is limited by MOD
        uint32_t prev_c = this->readReg32(IDT8V97003_REG_NMOD_LOW); // Its ok to read after write because the register is buffered
        uint64_t phase = this->readReg32(IDT8V97003_REG_PHASE_LOW);

        phase = ((phase * (uint64_t)c) / prev_c);

        if(phase >= c)
            phase = c - 1;

        this->writeReg32(IDT8V97003_REG_PHASE_LOW, phase & 0xFFFFFFFF);

        man_ctl_reg |= IDT8V97003_REG_MANUAL_CTL_PH_ADJ;
    }

    this->transferDoubleBuffer();

    this->rmwReg(IDT8V97003_REG_DSM_CTL, 0x0F, (b > 0) ? IDT8V97003_REG_DSM_CTL_DSM_ORDER_3RD : IDT8V97003_REG_DSM_CTL_DSM_ORDER_OFF);

    uint8_t out_reg = 0;

    if(out_dbl_en)
    {
        out_reg |= IDT8V97003_REG_OUT_DIV_DBL_OUT_DBL_ENA;

        if(vco_freq < 7e9)
            out_reg |= IDT8V97003_REG_OUT_DIV_DBL_OUT_DBL_FREQ;
    }

    if(m0_div > 0)
        out_reg |= IDT8V97003_REG_OUT_DIV_DBL_OUT_DIV_ENA | (m0_div & 0x07);

    this->writeReg(IDT8V97003_REG_OUT_DIV_DBL, out_reg);
    this->rmwReg(IDT8V97003_REG_MANUAL_CTL, 0x0F, man_ctl_reg);

    // Force waiting for calibration completion when auto loop bandwidth setting is requested
    if(set_loop_bw && cal_timeout < 0)
        cal_timeout = 5000; // 5 ms

    // Check if the calibration (band selection) completes
    if(cal_timeout >= 0)
    {
        uint8_t status = this->readReg(IDT8V97003_REG_LD_CAL_VCO_STATUS);

        while(cal_timeout > 0 && !(status & IDT8V97003_REG_LD_CAL_VCO_STATUS_BAND_SEL_DONE))
        {
            std::this_thread::sleep_for(std::chrono::microseconds(10));

            cal_timeout -= 10;
            status = this->readReg(IDT8V97003_REG_LD_CAL_VCO_STATUS);
        }

        if(!(status & IDT8V97003_REG_LD_CAL_VCO_STATUS_BAND_SEL_DONE))
            throw std::runtime_error("8V97003: Calibration timeout (Status: " + std::to_string(status) + ")");

        if(set_loop_bw)
            this->setLoopBandwidth();
    }

    // Check if the PLL locks
    if(this->readReg(IDT8V97003_REG_LD_CTL0) & IDT8V97003_REG_LD_CTL0_LD_DISABLE)
    {
        if(lock_timeout >= 0)
        {
            uint8_t status = this->readReg(IDT8V97003_REG_LD_CAL_VCO_STATUS);

            while(lock_timeout > 0 && !(status & IDT8V97003_REG_LD_CAL_VCO_STATUS_DIG_LOCK))
            {
                std::this_thread::sleep_for(std::chrono::microseconds(10));

                lock_timeout -= 10;
                status = this->readReg(IDT8V97003_REG_LD_CAL_VCO_STATUS);
            }

            if(!(status & IDT8V97003_REG_LD_CAL_VCO_STATUS_DIG_LOCK))
                throw std::runtime_error("8V97003: Lock timeout (Status: " + std::to_string(status) + ")");
        }
    }
    else
    {
        throw std::runtime_error("8V97003: Lock detect circuit disabled, PLL may not be locked");
    }
}

double IDT8V97003::getPhase()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint32_t b = this->readReg32(IDT8V97003_REG_NFRAC_LOW);

    if(!b)
        return 0; // Phase adjustment not supported in integer mode (FRAC = 0)

    uint8_t buf[8];

    this->readReg(IDT8V97003_REG_NMOD_LOW, buf, 8);

    uint32_t c = buf[0] | ((uint32_t)buf[1] << 8) | ((uint32_t)buf[2] << 16) | ((uint32_t)buf[3] << 24);

    if(c < 2)
        throw std::runtime_error("8V97003: Invalid modulus");

    uint32_t phase = buf[4] | ((uint32_t)buf[5] << 8) | ((uint32_t)buf[6] << 16) | ((uint32_t)buf[7] << 24);

    if(phase > c)
        throw std::runtime_error("8V97003: Phase out of range (" + std::to_string(phase) + " > " + std::to_string(c) + ")");

    return (double)phase / (double)c * 360.0;
}
void IDT8V97003::setPhase(double phase)
{
    while(phase >= 360.0)
        phase -= 360.0;

    while(phase < 0.0)
        phase += 360.0;

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint32_t b = this->readReg32(IDT8V97003_REG_NFRAC_LOW);

    if(!b)
        throw std::runtime_error("8V97003: Phase adjustment not supported in integer mode");

    uint32_t c = this->readReg32(IDT8V97003_REG_NMOD_LOW);

    if(c < 2)
        throw std::runtime_error("8V97003: Invalid modulus");

    uint32_t phase_word = (uint32_t)std::round((double)c * phase / 360.0);

    this->writeReg32(IDT8V97003_REG_PHASE_LOW, phase_word);
    this->transferDoubleBuffer();

    this->rmwReg(IDT8V97003_REG_MANUAL_CTL, (uint8_t)~IDT8V97003_REG_MANUAL_CTL_PH_ADJ, IDT8V97003_REG_MANUAL_CTL_PH_ADJ);
}