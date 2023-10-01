#include "axi_adc_core.h"

/**
 * @brief AXI ADC Data read.
 * @param adc - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Read data value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_read(struct axi_adc* adc, uint32_t reg_addr, uint32_t* reg_data)
{
    if(adc == NULL)
        return -1;

    if(reg_data == NULL)
        return -1;

    *reg_data = (uint32_t)*(volatile uint32_t *)((uintptr_t)adc->base + reg_addr);

    return 0;
}

/**
 * @brief AXI ADC Data Write.
 * @param adc - The device structure.
 * @param reg_addr - The register address.
 * @param reg_data - Data value to be written.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_write(struct axi_adc* adc, uint32_t reg_addr, uint32_t reg_data)
{
    if(adc == NULL)
        return -1;

    *(volatile uint32_t *)((uintptr_t)adc->base + reg_addr) = reg_data;

    return 0;
}

/**
 * @brief Set AXI ADC PN sequence.
 * @param adc - The device structure.
 * @param chan - The AXI ADC channel.
 * @param sel - PN sequence.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_pnsel(struct axi_adc* adc, uint32_t chan, enum axi_adc_pn_sel sel)
{
    uint32_t reg_data;

    axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_3(chan), &reg_data);
    reg_data &= ~AXI_ADC_ADC_PN_SEL(~0);
    reg_data |= AXI_ADC_ADC_PN_SEL(sel);
    axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_3(chan), reg_data);

    return 0;
}

/**
 * @brief Monitor the AXI ADC PN Sequence.
 * @param adc - The device structure.
 * @param sel - PN sequence.
 * @param delay_ms - Delay in ms.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_pn_mon(struct axi_adc* adc, enum axi_adc_pn_sel sel, uint32_t delay_ms)
{
    uint8_t	ch;
    uint32_t reg_data;

    for(ch = 0; ch < adc->num_channels; ch++)
    {
        axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL(ch), &reg_data);
        reg_data |= AXI_ADC_ENABLE;
        axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL(ch), reg_data);
        axi_adc_set_pnsel(adc, ch, sel);
    }

    usleep(1000);

    for(ch = 0; ch < adc->num_channels; ch++)
        axi_adc_write(adc, AXI_ADC_REG_CHAN_STATUS(ch), 0xFF);

    usleep(delay_ms * 1000);

    for(ch = 0; ch < adc->num_channels; ch++)
    {
        axi_adc_read(adc, AXI_ADC_REG_CHAN_STATUS(ch), &reg_data);

        if(reg_data != 0)
            return -1;
    }

    return 0;
}

/**
 * @brief Get the AXI ADC Sampling Frequency
 * @param adc - The device structure.
 * @param chan - The AXI ADC channel.
 * @param sampling_freq - Sampling frequency value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_sampling_freq(struct axi_adc* adc, uint32_t chan, uint64_t* sampling_freq)
{
    uint32_t freq = 0;
    uint32_t ratio = 0;

    axi_adc_read(adc, AXI_ADC_REG_CLK_FREQ, &freq);
    axi_adc_read(adc, AXI_ADC_REG_CLK_RATIO, &ratio);

    *sampling_freq = freq * ratio;
    *sampling_freq = ((*sampling_freq) * 125000000UL) >> 16;

    return 0;
}

/**
 * @brief Set input/output delay primitive for specific interface line
 * @param adc - The device structure.
 * @param lane - The AXI ADC interface line.
 * @param val - Delay value.
 */
void axi_adc_idelay_set(struct axi_adc* adc, uint32_t lane, uint32_t val)
{
    axi_adc_write(adc, AXI_ADC_REG_DELAY(lane), val);
}

/**
 * @brief Set input/output delay primitive for number of lines
 * @param adc - The device structure.
 * @param no_of_lanes - The AXI ADC number of lanes.
 * @param delay - Delay value.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_delay_set(struct axi_adc* adc, uint32_t no_of_lanes, uint32_t delay)
{
    uint32_t i;
    uint32_t rdata = 0;
    uint32_t pcore_version = 0;

    axi_adc_read(adc, 0x0, &pcore_version);

    pcore_version = AXI_CORE_VERSION_MAJOR(pcore_version);

    if(pcore_version < 9)
    {
        DBGPRINTLN_CTX("pcore_version is : %d", (int)pcore_version);
        DBGPRINTLN_CTX("DRIVER DOES NOT SUPPORT PCORE VERSIONS OLDER THAN 10 !");

        return -1;
    }
    else
    {
        for(i = 0; i < no_of_lanes; i++)
        {
            axi_adc_idelay_set(adc, i, delay);
            axi_adc_read(adc, AXI_ADC_REG_DELAY(i), &rdata);

            if(rdata != delay)
            {
                DBGPRINTLN_CTX("adc_delay_1: sel(%2d), rcv(%04x), exp(%04x)", (int)i, (int)rdata, (int)delay);
            }
        }
    }

    return 0;
}

/**
 * @brief Calibrate Delay using specific PN sequence.
 * @param adc - The device structure.
 * @param no_of_lanes - The AXI ADC number of lanes.
 * @param sel - PN sequence.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_delay_calibrate(struct axi_adc* adc, uint32_t no_of_lanes, enum axi_adc_pn_sel sel)
{
    uint8_t err_field[32] = { 0 };
    uint16_t valid_range[5] = { 0 };
    uint16_t invalid_range[5] = { 0 };
    uint16_t delay = 0;
    uint16_t start_valid_delay = 32;
    uint16_t start_invalid_delay = 32;
    uint8_t interval = 0;
    uint8_t max_interval = 0;
    uint8_t max_valid_range = 0;
    uint8_t cnt_valid[5] = { 0 };
    uint8_t cnt_invalid = 0;
    uint8_t val = 0;
    uint8_t max_val = 32;

    for(delay = 0; delay < 32; delay++)
    {
        axi_adc_delay_set(adc, no_of_lanes, delay);

        usleep(20000);

        if(axi_adc_pn_mon(adc, sel, 100) == 0)
        {
            err_field[delay] = 0;
            start_valid_delay = start_valid_delay == 32 ? delay : start_valid_delay;
        }
        else
        {
            err_field[delay] = 1;
        }
    }

    if(start_valid_delay > 31)
    {
        DBGPRINTLN_CTX("FAILED");

        axi_adc_delay_set(adc, no_of_lanes, 0);

        return -1;
    }

    start_valid_delay = 32;
    start_invalid_delay = 32;

    for(val = 0; val < max_val; val++)
    {
        if(err_field[val] == 0)
        {
            if(start_valid_delay == 32)
                start_valid_delay = val;

            if(start_valid_delay != 32 && start_invalid_delay != 32)
            {
                start_valid_delay = 32;
                start_invalid_delay = 32;
            }

            cnt_valid[interval]++;
        }

        if((err_field[val] == 1) || (val == max_val - 1))
        {
            if(start_invalid_delay == 32)
                start_invalid_delay = val;

            if(start_valid_delay != 32 && start_invalid_delay != 32)
            {
                valid_range[interval] = start_valid_delay;
                invalid_range[interval] = start_invalid_delay;
                start_valid_delay = 32;
                start_invalid_delay = 32;
                interval++;
            }

            cnt_invalid++;
        }
    }

    for(val = 0; val < 5; val++)
    {
        if(cnt_valid[val] > max_valid_range)
        {
            max_valid_range = cnt_valid[val];
            max_interval = val;
        }
    }

    delay = (valid_range[max_interval] + invalid_range[max_interval] - 1) / 2;

    DBGPRINTLN_CTX("adc_delay: setting zero error delay (%d)", delay);

    axi_adc_delay_set(adc, no_of_lanes, delay);

    return 0;
}

/**
 * @brief Calibrate phase for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param phase - The AXI ADC phase value.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_phase_scale(struct axi_adc* adc, uint32_t phase, uint32_t chan, int32_t val, int32_t val2)
{
    uint32_t fract;
    uint64_t llval;
    uint32_t tmp = 0;

    switch(val)
    {
        case 1:
            fract = 0x4000;
        break;
        case -1:
            fract = 0xC000;
        break;
        case 0:
            fract = 0;

            if(val2 < 0)
            {
                fract = 0x8000;
                val2 *= -1;
            }

            break;
        default:
            return -1;
        break;
    }

    llval = (uint64_t)val2 * 0x4000UL + (1000000UL / 2);
    utils_do_div(&llval, 1000000UL);
    fract |= llval;

    axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), &tmp);

    if(!((chan + phase) % 2))
    {
        tmp &= ~AXI_ADC_IQCOR_COEFF_1(~0);
        tmp |= AXI_ADC_IQCOR_COEFF_1(fract);
    }
    else
    {
        tmp &= ~AXI_ADC_IQCOR_COEFF_2(~0);
        tmp |= AXI_ADC_IQCOR_COEFF_2(fract);
    }

    axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), tmp);

    return 0;
}

/**
 * @brief Get the phase calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param phase - The AXI ADC phase value.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_phase_scale(struct axi_adc* adc, uint32_t phase, uint32_t chan, int32_t* val, int32_t* val2)
{
    uint32_t tmp = 0;
    int32_t sign;
    uint64_t llval;

    axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_2(chan), &tmp);

    if(!((phase + chan) % 2))
        tmp = AXI_ADC_TO_IQCOR_COEFF_1(tmp);
    else
        tmp = AXI_ADC_TO_IQCOR_COEFF_2(tmp);

    if(tmp & 0x8000)
        sign = -1;
    else
        sign = 1;

    if(tmp & 0x4000)
        *val = 1 * sign;
    else
        *val = 0;

    tmp &= ~0xC000;

    llval = tmp * 1000000ULL + (0x4000 / 2);
    utils_do_div(&llval, 0x4000);
    if(*val == 0)
        *val2 = llval * sign;
    else
        *val2 = llval;

    return 0;
}

/**
 * @brief Calibrate phase for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_phase(struct axi_adc* adc, uint32_t chan, int32_t val, int32_t val2)
{
    return axi_adc_set_calib_phase_scale(adc, 1, chan, val, val2);
}

/**
 * @brief Get the phase calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_phase(struct axi_adc* adc, uint32_t chan, int32_t* val, int32_t* val2)
{
    return axi_adc_get_calib_phase_scale(adc, 1, chan, val, val2);
}

/**
 * @brief Calibrate scale for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_scale(struct axi_adc* adc, uint32_t chan, int32_t val, int32_t val2)
{
    return axi_adc_set_calib_phase_scale(adc, 0, chan, val, val2);
}

/**
 * @brief Get the scale calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_scale(struct axi_adc* adc, uint32_t chan, int32_t* val, int32_t* val2)
{
    return axi_adc_get_calib_phase_scale(adc, 0, chan, val, val2);
}

/**
 * @brief Calibrate bias for specific AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_set_calib_bias(struct axi_adc* adc, uint32_t chan, int32_t val, int32_t val2)
{
    uint32_t tmp;

    axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), &tmp);

    tmp &= ~AXI_ADC_DCFILT_OFFSET(~0);
    tmp |= AXI_ADC_DCFILT_OFFSET((uint16_t)val);

    axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), tmp);

    return 0;
}

/**
 * @brief Get the scale calibration values for AXI ADC channel.
 * @param adc - The device structure.
 * @param chan - AXI ADC channel.
 * @param val - Integer value
 * @param val2 - Fractional value
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_get_calib_bias(struct axi_adc* adc, uint32_t chan, int32_t* val, int32_t* val2)
{
    uint32_t tmp = 0;

    axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL_1(chan), &tmp);

    *val = (uint16_t)AXI_ADC_TO_DCFILT_OFFSET(tmp);

    return 0;
}
/**
 * @brief Update active AXI ADC channels.
 * @param adc - The device structure.
 * @param mask - Channel mask.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_update_active_channels(struct axi_adc* adc, uint32_t mask)
{
    int32_t ret;
    uint32_t ch;
    uint32_t val;
    uint32_t new_val;

    if(mask == adc->mask)
        return 0;

    adc->mask = mask;

    for(ch = 0; ch < adc->num_channels; ch++)
    {
        ret = axi_adc_read(adc, AXI_ADC_REG_CHAN_CNTRL(ch), &val);

        if(ret)
            return ret;

        new_val = val & (~AXI_ADC_ENABLE);

        if(mask & 1)
            new_val = val | AXI_ADC_ENABLE;

        if(new_val != val)
        {
            ret = axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL(ch), new_val);

            if(ret)
                return ret;
        }

        mask >>= 1;
    }

    return 0;
}

/**
 * @brief Begin AXI ADC Initialization.
 * @param adc_core - The device structure.
 * @param init - Initialization parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_init_begin(struct axi_adc** adc_core, const struct axi_adc_init* init)
{
    struct axi_adc* adc;

    adc = (struct axi_adc*)malloc(sizeof(*adc));

    if(!adc)
        return -1;

    adc->name = init->name;
    adc->base = init->base;
    adc->num_channels = init->num_channels;

    *adc_core = adc;

    return 0;
};

/**
 * @brief Begin AXI ADC Initialization.
 * @param adc - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_init_finish(struct axi_adc* adc)
{
    uint32_t reg_data = 0;
    uint32_t freq = 0;
    uint32_t ratio = 0;

    axi_adc_read(adc, AXI_ADC_REG_STATUS, &reg_data);

    if(reg_data == 0x0)
    {
        DBGPRINTLN_CTX("%s: Status errors", adc->name);

        return -1;
    }

    axi_adc_read(adc, AXI_ADC_REG_CLK_FREQ, &freq);
    axi_adc_read(adc, AXI_ADC_REG_CLK_RATIO, &ratio);

    adc->clock_hz = freq * ratio;
    adc->clock_hz = (adc->clock_hz * 125000000UL) >> 16;

    DBGPRINTLN_CTX("%s: Successfully initialized (%"PRIu64" Hz)", adc->name, adc->clock_hz);

    return 0;
}

/**
 * @brief AXI ADC Main Initialization.
 * @param adc_core - The device structure.
 * @param init - Initialization parameters.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_init(struct axi_adc** adc_core, const struct axi_adc_init* init)
{
    struct axi_adc* adc;
    int32_t ret;
    uint8_t ch;

    ret = axi_adc_init_begin(&adc, init);

    if(ret)
        return ret;

    axi_adc_write(adc, AXI_ADC_REG_RSTN, 0);
    axi_adc_write(adc, AXI_ADC_REG_RSTN, AXI_ADC_MMCM_RSTN | AXI_ADC_RSTN);

    for(ch = 0; ch < adc->num_channels; ch++)
        axi_adc_write(adc, AXI_ADC_REG_CHAN_CNTRL(ch), AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE | AXI_ADC_ENABLE);

    usleep(100000);

    ret = axi_adc_init_finish(adc);

    if(ret)
    {
        free(adc);

        return -1;
    }

    *adc_core = adc;

    return 0;
}

/**
 * @brief AXI ADC Resources deallocation.
 * @param adc - The device structure.
 * @return Returns 0 in case of success or negative error code otherwise.
 */
int32_t axi_adc_remove(struct axi_adc* adc)
{
    free(adc);

    return 0;
}
