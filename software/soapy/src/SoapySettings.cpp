#include "SoapyIcyRadio.hpp"

SoapyIcyRadio::SoapyIcyRadio(const SoapySDR::Kwargs &args)
{
    this->fd = -1;

    // Memory maps
    this->mm_axi_flash = nullptr;
    this->mm_axi_bram = nullptr;
    this->mm_axi_dna = nullptr;
    this->mm_axi_ddr = nullptr;
    this->mm_axi_periph = nullptr;
    this->mm_dma_buffer = nullptr;

    // AXI Peripherals
    this->axi_dna = nullptr;
    for(uint8_t i = 0; i < AXI_DMAC_NUM_INSTANCES; i++)
        this->axi_dmac[i] = nullptr;
    for(uint8_t i = 0; i < AXI_IIC_NUM_INSTANCES; i++)
        this->axi_iic[i] = nullptr;
    for(uint8_t i = 0; i < AXI_SPI_NUM_INSTANCES; i++)
        this->axi_spi[i] = nullptr;
    for(uint8_t i = 0; i < AXI_GPIO_NUM_INSTANCES; i++)
        this->axi_gpio[i] = nullptr;
    this->axi_pcie = nullptr;
    this->axi_rf_tstamp = nullptr;
    this->axi_irq_ctrl = nullptr;
    this->axi_i2s = nullptr;
    this->axi_xadc = nullptr;
    this->axi_ad9361 = nullptr;

    // Peripherals
    this->pmc = nullptr;
    this->vin_reg = nullptr;
    this->spi_flash = nullptr;
    this->clk_mngr = nullptr;
    this->mmw_synth = nullptr;
    this->rf_phy = nullptr;
    this->exp_card = nullptr;

    if(args.count("label") != 0)
        DLOGF(SOAPY_SDR_INFO, "Opening %s", args.at("label").c_str());

    if(args.count("path") == 0)
        throw std::runtime_error("No device path specified");

    this->fd = open(args.at("path").c_str(), O_RDWR);

    if(this->fd < 0)
        throw std::runtime_error("Failed to open device (" + std::string(std::strerror(errno)) + ")");

    this->parseConfig(args);

    try
    {
        this->setupMemoryMaps();
        this->resetSystem();
    }
    catch(...)
    {
        this->freeMemoryMaps();

        close(this->fd);

        this->fd = -1;

        throw;
    }

    try
    {
        this->initPeripheralsPreClocks();
    }
    catch(...)
    {
        this->deinitPeripheralsPreClocks();

        this->freeMemoryMaps();

        close(this->fd);

        this->fd = -1;

        throw;
    }

    try
    {
        this->initClocks();
    }
    catch(...)
    {
        this->deinitClocks();
        this->deinitPeripheralsPreClocks();

        this->freeMemoryMaps();

        close(this->fd);

        this->fd = -1;

        throw;
    }

    try
    {
        this->initPeripheralsPostClocks();
    }
    catch(...)
    {
        this->deinitPeripheralsPostClocks();
        this->deinitClocks();
        this->deinitPeripheralsPreClocks();

        this->freeMemoryMaps();

        close(this->fd);

        this->fd = -1;

        throw;
    }
}
SoapyIcyRadio::~SoapyIcyRadio()
{
    this->deinitPeripheralsPostClocks();
    this->deinitClocks();
    this->deinitPeripheralsPreClocks();

    this->freeMemoryMaps();

    if(this->fd >= 0)
        close(this->fd);
}

std::string SoapyIcyRadio::getDriverKey() const
{
	return "IcyRadio";
}
std::string SoapyIcyRadio::getHardwareKey() const
{
	return "IcyRadio v2";
}
SoapySDR::Kwargs SoapyIcyRadio::getHardwareInfo() const
{
    SoapySDR::Kwargs info;

    // FPGA DNA (aka serial)
    while(!this->axi_dna->isSerialNumberReady()) // Serial is essential, so wait for it to be ready
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    char buf[64] = {0};

    snprintf(buf, sizeof(buf), "%015lX", this->axi_dna->getSerialNumber());
    info["serial"] = buf;

    // PMC info
    info["pmc_serial"] = this->pmc->getUniqueID();
    info["pmc_fw_version"] = "v" + std::to_string(this->pmc->getFirmwareVersion());

    // VIN Regulator info
    info["vin_reg_pn"] = this->vin_reg->readManufacturerID() + " " + this->vin_reg->readManufacturerModel();
    info["vin_reg_serial"] = this->vin_reg->readManufacturerSerial();
    info["vin_reg_rev_id"] = std::to_string(this->vin_reg->readManufacturerRevision());

    // SPI Flash info
    info["spi_flash_pn"] = this->spi_flash->getDeviceName();
    snprintf(buf, sizeof(buf), "%06X", this->spi_flash->readJEDECID());
    info["spi_flash_jedec_id"] = buf;
    snprintf(buf, sizeof(buf), "%04X", this->spi_flash->readMFDeviceID());
    info["spi_flash_mf_device_id"] = buf;
    snprintf(buf, sizeof(buf), "%016lX", this->spi_flash->readUniqueID());
    info["spi_flash_serial"] = buf;

    // Clock manager info
    info["clk_mngr_rev_id"] = std::to_string(this->clk_mngr->getRevisionID());

    // mmWave Synth info
    info["mmw_synth_rev_id"] = std::to_string(this->mmw_synth->getChipVersion());
    info["mmw_synth_chip_opt"] = std::to_string(this->mmw_synth->getChipOption());

    // RF Phy info
    info["rf_phy_rev_id"] = std::to_string(this->rf_phy->getChipRevision());

    return info;
}

void SoapyIcyRadio::setFrontendMapping(const int direction, const std::string &mapping)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setFrontendMapping: Unknown direction");
}
std::string SoapyIcyRadio::getFrontendMapping(const int direction) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getFrontendMapping: Unknown direction");

    return "";
}
size_t SoapyIcyRadio::getNumChannels(const int direction) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getNumChannels: Unknown direction");

    return 2;
}
SoapySDR::Kwargs SoapyIcyRadio::getChannelInfo(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getChannelInfo: Unknown direction");

    SoapySDR::Kwargs info;

    return info;
}
bool SoapyIcyRadio::getFullDuplex(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getFullDuplex: Unknown direction");

    return true;
}

std::vector<std::string> SoapyIcyRadio::listAntennas(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("listAntennas: Unknown direction");

    std::vector<std::string> antennas;

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            antennas.push_back("RX" + std::to_string(BIT(channel)) + "A");
            // RXnB is not routed
            antennas.push_back("RX" + std::to_string(BIT(channel)) + "C");
        }
        else
        {
            antennas.push_back("TX" + std::to_string(BIT(channel)) + "A");
            antennas.push_back("TX" + std::to_string(BIT(channel)) + "B");
        }
    }

    return antennas;
}
void SoapyIcyRadio::setAntenna(const int direction, const size_t channel, const std::string &name)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setAntenna: Unknown direction");

    bool toggle_pa = true; // TODO: Allow disabling auto PA setting when TXnB selected

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            toggle_pa = false;

            if(name == ("RX" + std::to_string(BIT(channel)) + "A"))
            {
                this->rf_phy->pdata->rf_rx_input_sel = 0;
            }
            else if(name == ("RX" + std::to_string(BIT(channel)) + "C"))
            {
                this->rf_phy->pdata->rf_rx_input_sel = 2;
            }
            else
            {
                throw std::runtime_error("setAntenna: Unknown name");
            }
        }
        else
        {
            if(name == ("TX" + std::to_string(BIT(channel)) + "A"))
            {
                this->rf_phy->pdata->rf_tx_output_sel = 0;
            }
            else if(name == ("TX" + std::to_string(BIT(channel)) + "B"))
            {
                this->rf_phy->pdata->rf_tx_output_sel = 1;
            }
            else
            {
                throw std::runtime_error("setAntenna: Unknown name");
            }
        }

        this->rf_phy->setupRFPort(false, this->rf_phy->pdata->rf_rx_input_sel, this->rf_phy->pdata->rf_tx_output_sel);

        if(toggle_pa)
            this->rf_phy->setGPOValue(2 + channel, this->rf_phy->pdata->rf_tx_output_sel == 1);
    }

}
std::string SoapyIcyRadio::getAntenna(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getAntenna: Unknown direction");

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            if(this->rf_phy->pdata->rf_rx_input_sel == 0)
            {
                return "RX" + std::to_string(BIT(channel)) + "A";
            }
            else if(this->rf_phy->pdata->rf_rx_input_sel == 2)
            {
                return "RX" + std::to_string(BIT(channel)) + "C";
            }
            else
            {
                return "Unknown";
            }
        }
        else
        {
            if(this->rf_phy->pdata->rf_tx_output_sel == 0)
            {
                return "TX" + std::to_string(BIT(channel)) + "A";
            }
            else if(this->rf_phy->pdata->rf_tx_output_sel == 1)
            {
                return "TX" + std::to_string(BIT(channel)) + "B";
            }
            else
            {
                return "Unknown";
            }
        }
    }

    return "Unknown";
}


std::vector<std::string> SoapyIcyRadio::listGains(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("listGains: Unknown direction");

    std::vector<std::string> gains;

    if(direction == SOAPY_SDR_RX)
    {
        gains.push_back("RX_RF");
    }
    else
    {
        gains.push_back("TX_RF");
    }

    return gains;
}
bool SoapyIcyRadio::hasGainMode(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("hasGainMode: Unknown direction");

    return direction == SOAPY_SDR_RX;
}
void SoapyIcyRadio::setGainMode(const int direction, const size_t channel, const bool automatic)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setGainMode: Unknown direction");

    if(direction != SOAPY_SDR_RX)
        throw std::runtime_error("setGainMode: AGC is not supported for TX");

    this->rf_phy->setRXGainMode(BIT(channel), automatic ? AD9361::RFGainCtrlMode::RF_GAIN_SLOWATTACK_AGC : AD9361::RFGainCtrlMode::RF_GAIN_MGC);
}
bool SoapyIcyRadio::getGainMode(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getGainMode: Unknown direction");

    if(direction != SOAPY_SDR_RX)
        return false;

    return false;
}
void SoapyIcyRadio::setGain(const int direction, const size_t channel, const double value)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setGain: Unknown direction");

    this->setGain(direction, channel, direction == SOAPY_SDR_RX ? "RX_RF" : "TX_RF", value);
}
void SoapyIcyRadio::setGain(const int direction, const size_t channel, const std::string &name, const double value)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setGain: Unknown direction");

    if(direction == SOAPY_SDR_RX)
    {
        if(name == "RX_RF")
        {
            // SoapySDR::Range r = this->getGainRange(direction, channel, name);

            // if(value < r.minimum() || value > r.maximum())
            //     throw std::runtime_error("setGain: Gain out of range");

            AD9361::RFRXGain g;

            g.gain_db = value;

            this->rf_phy->setRXGain(BIT(channel), &g);
        }
        else
        {
            throw std::runtime_error("setGain: Unknown name");
        }
    }
    else
    {
        if(name == "TX_RF")
        {
            if(value > 0 || value < -89.75)
                throw std::runtime_error("setGain: Gain out of range");

            this->rf_phy->setTXAttenuation(value * -1000, channel == 0, channel == 1, true);
        }
        else
        {
            throw std::runtime_error("setGain: Unknown name");
        }
    }
}
double SoapyIcyRadio::getGain(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getGain: Unknown direction");

    return this->getGain(direction, channel, direction == SOAPY_SDR_RX ? "RX_RF" : "TX_RF");
}
double SoapyIcyRadio::getGain(const int direction, const size_t channel, const std::string &name) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getGain: Unknown direction");

    if(direction == SOAPY_SDR_RX)
    {
        if(name == "RX_RF")
        {
            AD9361::RFRXGain g;

            this->rf_phy->getRXGain(BIT(channel), &g);

            return g.gain_db;
        }
        else
        {
            throw std::runtime_error("getGain: Unknown name");
        }
    }
    else
    {
        if(name == "TX_RF")
        {
            return (double)this->rf_phy->getTXAttenuation(BIT(channel)) / -1000.0;
        }
        else
        {
            throw std::runtime_error("getGain: Unknown name");
        }
    }
}
SoapySDR::Range SoapyIcyRadio::getGainRange(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getGainRange: Unknown direction");

    return this->getGainRange(direction, channel, direction == SOAPY_SDR_RX ? "RX_RF" : "TX_RF");
}
SoapySDR::Range SoapyIcyRadio::getGainRange(const int direction, const size_t channel, const std::string &name) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getGainRange: Unknown direction");

    if(direction == SOAPY_SDR_RX)
    {
        if(name == "RX_RF")
        {
            int8_t min = this->rf_phy->gt_info[this->rf_phy->getCurrentGainTable()].abs_gain_tbl[0];
            int8_t max = this->rf_phy->gt_info[this->rf_phy->getCurrentGainTable()].abs_gain_tbl[this->rf_phy->gt_info[this->rf_phy->getCurrentGainTable()].max_index - 1];

            return SoapySDR::Range(min, max, 1.0);
        }
        else
        {
            throw std::runtime_error("getGainRange: Unknown name");
        }
    }
    else
    {
        if(name == "TX_RF")
        {
            return SoapySDR::Range(-89.75, 0.0, 0.25);
        }
        else
        {
            throw std::runtime_error("getGainRange: Unknown name");
        }
    }
}

void SoapyIcyRadio::setFrequency(const int direction, const size_t channel, const double frequency, const SoapySDR::Kwargs &args)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setFrequency: Unknown direction");

    this->setFrequency(direction, channel, direction == SOAPY_SDR_RX ? "RX_LO" : "TX_LO", frequency, args);
}
void SoapyIcyRadio::setFrequency(const int direction, const size_t channel, const std::string &name, const double frequency, const SoapySDR::Kwargs &args)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setFrequency: Unknown direction");

    if(direction == SOAPY_SDR_RX)
    {
        if(name == "RX_LO")
        {
            if(frequency < 70e6 || frequency > 6e9)
                throw std::runtime_error("setFrequency: Frequency out of range");

            this->rf_phy->setClockRate(this->rf_phy->ref_clk_scale[AD9361::ClockIndex::RX_RFPLL], frequency);
        }
        else
        {
            throw std::runtime_error("setFrequency: Unknown name");
        }
    }
    else
    {
        if(name == "TX_LO")
        {
            if(frequency < 47e6 || frequency > 6e9)
                throw std::runtime_error("setFrequency: Frequency out of range");

            this->rf_phy->setClockRate(this->rf_phy->ref_clk_scale[AD9361::ClockIndex::TX_RFPLL], frequency);
        }
        else
        {
            throw std::runtime_error("setFrequency: Unknown name");
        }
    }
}
double SoapyIcyRadio::getFrequency(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getFrequency: Unknown direction");

    return this->getFrequency(direction, channel, direction == SOAPY_SDR_RX ? "RX_LO" : "TX_LO");
}
double SoapyIcyRadio::getFrequency(const int direction, const size_t channel, const std::string &name) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getFrequency: Unknown direction");

    if(direction == SOAPY_SDR_RX)
    {
        if(name == "RX_LO")
        {
            return this->rf_phy->getClockRate(this->rf_phy->ref_clk_scale[AD9361::ClockIndex::RX_RFPLL]);
        }
        else
        {
            throw std::runtime_error("getFrequency: Unknown name");
        }
    }
    else
    {
        if(name == "TX_LO")
        {
            return this->rf_phy->getClockRate(this->rf_phy->ref_clk_scale[AD9361::ClockIndex::TX_RFPLL]);
        }
        else
        {
            throw std::runtime_error("getFrequency: Unknown name");
        }
    }
}
std::vector<std::string> SoapyIcyRadio::listFrequencies(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("listFrequencies: Unknown direction");

    std::vector<std::string> names;

    if(direction == SOAPY_SDR_RX)
    {
        names.push_back("RX_LO");
    }
    else
    {
        names.push_back("TX_LO");
    }

    return names;
}
SoapySDR::RangeList SoapyIcyRadio::getFrequencyRange(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getFrequencyRange: Unknown direction");

    return this->getFrequencyRange(direction, channel, direction == SOAPY_SDR_RX ? "RX_LO" : "TX_LO");
}
SoapySDR::RangeList SoapyIcyRadio::getFrequencyRange(const int direction, const size_t channel, const std::string &name) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getFrequencyRange: Unknown direction");

    SoapySDR::RangeList ranges;

    if(direction == SOAPY_SDR_RX)
    {
        if(name == "RX_LO")
        {
            ranges.emplace_back(70e6, 6e9);
        }
        else
        {
            throw std::runtime_error("getFrequencyRange: Unknown name");
        }
    }
    else
    {
        if(name == "TX_LO")
        {
            ranges.emplace_back(47e6, 6e9);
        }
        else
        {
            throw std::runtime_error("getFrequencyRange: Unknown name");
        }
    }

    return ranges;
}
SoapySDR::ArgInfoList SoapyIcyRadio::getFrequencyArgsInfo(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getFrequencyArgsInfo: Unknown direction");

    SoapySDR::ArgInfoList args;

    return args;
}

void SoapyIcyRadio::setSampleRate(const int direction, const size_t channel, const double rate)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setSampleRate: Unknown direction");

    std::lock_guard<std::mutex> lock(this->streams_mutex);

    // Check if datapath needs reconfiguration and if we can do it
    auto rx_chs = this->getBusyChannels(SOAPY_SDR_RX);
    auto tx_chs = this->getBusyChannels(SOAPY_SDR_TX);
    size_t num_chans = MAX(rx_chs.size(), tx_chs.size());

    this->validateSampleRateAndChannelCombination(rate, MAX(num_chans, 1));

    if(this->requiresDataPathReconfiguration(rate))
    {
        if(this->getStreams(true).size() > 0)
            throw std::runtime_error("setSampleRate: Sample rate change requires datapath reconfiguration, but there are active streams");

        DLOGF(SOAPY_SDR_DEBUG, "setSampleRate: Reconfiguring data path for new sample rate %u", (size_t)rate);

        if(num_chans > 1)
            this->reconfigureDataPath(true);
        else
            this->reconfigureDataPath(false, rx_chs.size() > 0 ? rx_chs[0] : 0, tx_chs.size() > 0 ? tx_chs[0] : 0);

        auto streams = this->getStreams();

        for(auto &s : streams)
            this->reinitStreamChannels(s);
    }

    uint8_t int_dec = 0;
    uint8_t ntaps = 0;

    if(rate <= MAX_DAC_CLK / 16.0) // TODO: Check why libad9361-iio uses this value as limit for int/dec 4
    {
        int_dec = 4;
        ntaps = 128;
    }
    else if(rate <= MAX_DAC_CLK / 8.0) // The TX FIR can process 128 (16 * 8) taps up to this rate
    {
        int_dec = 2;
        ntaps = 128;
    }
    else if(rate <= MAX_DAC_CLK / 6.0) // The TX FIR can process 96 (16 * 6) taps up to this rate
    {
        int_dec = 2;
        ntaps = 96;
    }
    else if(rate <= MAX_DAC_CLK / 4.0) // The TX FIR can process 64 (16 * 4) taps up to this rate
    {
        int_dec = 2;
        ntaps = 64;
    }

    if(!int_dec || !ntaps)
    {
        if(!this->rf_phy->bypass_rx_fir || !this->rf_phy->bypass_tx_fir)
        {
            this->rf_phy->bypass_rx_fir = true;
            this->rf_phy->bypass_tx_fir = true;
        }
    }
    else
    {
        this->loadRFPhyFIR(ntaps, int_dec, false); // Will do nothing if the correct FIR is already loaded

        if(this->rf_phy->bypass_rx_fir || this->rf_phy->bypass_tx_fir)
        {
            this->rf_phy->bypass_rx_fir = false;
            this->rf_phy->bypass_tx_fir = false;

            // No need to call this since changes will apply when we set the clocks below
            // This also validates some things that "can" be invalid, but should we really care?
            // this->rf_phy->validateAndEnableFIR();
        }
    }

    this->rf_phy->setClockChainFreq(rate);
    this->rf_phy->setRFBandwidth(this->rf_phy->current_rx_bw_Hz, this->rf_phy->current_tx_bw_Hz);

    this->axi_rf_tstamp->triggerClockResync(true, 100); // Wait for sync, 100 ms timeout
}
double SoapyIcyRadio::getSampleRate(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getSampleRate: Unknown direction");

    return this->rf_phy->current_rx_path_clks[AD9361::RXClockIndex::RX_SAMPL_FREQ];
}
SoapySDR::RangeList SoapyIcyRadio::getSampleRateRange(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getSampleRateRange: Unknown direction");

    SoapySDR::RangeList ranges;

    ranges.emplace_back(MIN_BASEBAND_RATE, MAX_BASEBAND_RATE); // Actually supported up to 122.88 MS/s, but off-spec

    return ranges;
}

void SoapyIcyRadio::setBandwidth(const int direction, const size_t channel, const double bw)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setBandwidth: Unknown direction");

    double _bw = bw;

    if(_bw == 0.0)
    {
        _bw = this->getSampleRate(direction, channel) * 0.9;

        if(_bw > 56e6)
            _bw = 56e6;

        SoapySDR_logf(SOAPY_SDR_DEBUG, "setBandwidth: Auto-setting bandwidth to 90%% of sample rate (%.0f Hz)", _bw);
    }

    if(_bw < 200e3 || _bw > 56e6)
    {
        throw std::runtime_error("setBandwidth: Bandwidth out of range");
    }

    this->rf_phy->setRFBandwidth(direction == SOAPY_SDR_RX ? _bw : this->rf_phy->current_rx_bw_Hz, direction == SOAPY_SDR_TX ? _bw : this->rf_phy->current_tx_bw_Hz);
}
double SoapyIcyRadio::getBandwidth(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getBandwidth: Unknown direction");

    return direction == SOAPY_SDR_RX ? this->rf_phy->current_rx_bw_Hz : this->rf_phy->current_tx_bw_Hz;
}
SoapySDR::RangeList SoapyIcyRadio::getBandwidthRange(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getBandwidthRange: Unknown direction");

    SoapySDR::RangeList ranges;

    ranges.emplace_back(200000UL, 56000000UL);

    return ranges;
}


std::vector<std::string> SoapyIcyRadio::listTimeSources() const
{
    std::vector<std::string> sources;

    sources.push_back("rf_sample_clock");

    return sources;
}
void SoapyIcyRadio::setTimeSource(const std::string &source)
{
    if(source != "rf_sample_clock")
        throw std::runtime_error("setTimeSource: Unknown time source");
}
std::string SoapyIcyRadio::getTimeSource() const
{
    return "rf_sample_clock";
}
bool SoapyIcyRadio::hasHardwareTime(const std::string &what) const
{
    if(what != "" && what != "rf_timestamp")
        throw std::runtime_error("setHardwareTime: Unknown time source");

    return true;
}
long long SoapyIcyRadio::getHardwareTime(const std::string &what) const
{
    if(what != "" && what != "rf_timestamp")
        throw std::runtime_error("setHardwareTime: Unknown time source");

    return SoapySDR::ticksToTimeNs(this->axi_rf_tstamp->getCounter(), this->getSampleRate(SOAPY_SDR_RX, 0));
}
void SoapyIcyRadio::setHardwareTime(const long long timeNs, const std::string &what)
{
    if(what != "" && what != "rf_timestamp")
        throw std::runtime_error("setHardwareTime: Unknown time source");

    this->axi_rf_tstamp->setCounter(SoapySDR::timeNsToTicks(timeNs, this->getSampleRate(SOAPY_SDR_RX, 0)));
}

std::vector<std::string> SoapyIcyRadio::listSensors() const
{
    std::vector<std::string> sensors;

    // XADC sensors (FPGA)
    sensors.push_back("xadc_temp");
    sensors.push_back("xadc_temp_min");
    sensors.push_back("xadc_temp_max");
    sensors.push_back("xadc_vccint");
    sensors.push_back("xadc_vccint_min");
    sensors.push_back("xadc_vccint_max");
    sensors.push_back("xadc_vccaux");
    sensors.push_back("xadc_vccaux_min");
    sensors.push_back("xadc_vccaux_max");
    sensors.push_back("xadc_vccbram");
    sensors.push_back("xadc_vccbram_min");
    sensors.push_back("xadc_vccbram_max");

    // PMC sensors
    sensors.push_back("pmc_temp");
    sensors.push_back("pmc_vin");
    sensors.push_back("pmc_vext");
    sensors.push_back("pmc_12v0");
    sensors.push_back("pmc_vbus");
    sensors.push_back("pmc_iovdd");
    sensors.push_back("pmc_core_vdd");

    // LT7182S sensors (VIN Regulator)
    sensors.push_back("vin_reg_temp");
    sensors.push_back("vin_reg_temp_max");
    sensors.push_back("vin_reg_ext_vcc");
    sensors.push_back("vin_reg_5v0_vin");
    sensors.push_back("vin_reg_5v0_vin_peak");
    sensors.push_back("vin_reg_5v0_iin");
    sensors.push_back("vin_reg_5v0_vout");
    sensors.push_back("vin_reg_5v0_vout_peak");
    sensors.push_back("vin_reg_5v0_iout");
    sensors.push_back("vin_reg_5v0_iout_peak");
    sensors.push_back("vin_reg_5v0_pout");
    sensors.push_back("vin_reg_5v0_sw_freq");
    sensors.push_back("vin_reg_5v0_vith");
    sensors.push_back("vin_reg_1v0_vin");
    sensors.push_back("vin_reg_1v0_vin_peak");
    sensors.push_back("vin_reg_1v0_iin");
    sensors.push_back("vin_reg_1v0_vout");
    sensors.push_back("vin_reg_1v0_vout_peak");
    sensors.push_back("vin_reg_1v0_iout");
    sensors.push_back("vin_reg_1v0_iout_peak");
    sensors.push_back("vin_reg_1v0_pout");
    sensors.push_back("vin_reg_1v0_sw_freq");
    sensors.push_back("vin_reg_1v0_vith");

    // mmWave Synthesizer sensors
    // sensors.push_back("mmw_synth_temp");
    sensors.push_back("mmw_synth_locked");
    sensors.push_back("mmw_synth_vco_id");
    sensors.push_back("mmw_synth_vco_dig_band_id");

    // RF Phy sensors
    sensors.push_back("rf_phy_temp");

    return sensors;
}
SoapySDR::ArgInfo SoapyIcyRadio::getSensorInfo(const std::string &key) const
{
    SoapySDR::ArgInfo info;

    // XADC sensors (FPGA)
    if(key == "xadc_temp")
    {
        info.key = "xadc_temp";
        info.value = "0.000";
        info.name = "FPGA Temperature";
        info.description = "FPGA die temperature as measured by the Xilinx XADC";
        info.units = "°C";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(-40.0, 125.0);
    }
    else if(key == "xadc_temp_min")
    {
        info.key = "xadc_temp_min";
        info.value = "0.000";
        info.name = "FPGA Min. Temperature";
        info.description = "Minnimum FPGA die temperature as measured by the Xilinx XADC";
        info.units = "°C";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(-40.0, 125.0);
    }
    else if(key == "xadc_temp_max")
    {
        info.key = "xadc_temp_max";
        info.value = "0.000";
        info.name = "FPGA Max. Temperature";
        info.description = "Maximum FPGA die temperature as measured by the Xilinx XADC";
        info.units = "°C";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(-40.0, 125.0);
    }
    else if(key == "xadc_vccint")
    {
        info.key = "xadc_vccint";
        info.value = "0.000";
        info.name = "FPGA VCCINT Voltage";
        info.description = "FPGA VCCINT voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccint_min")
    {
        info.key = "xadc_vccint_min";
        info.value = "0.000";
        info.name = "FPGA Min. VCCINT Voltage";
        info.description = "Minimum FPGA VCCINT voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccint_max")
    {
        info.key = "xadc_vccint_max";
        info.value = "0.000";
        info.name = "FPGA Max. VCCINT Voltage";
        info.description = "Maximum FPGA VCCINT voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccaux")
    {
        info.key = "xadc_vccaux";
        info.value = "0.000";
        info.name = "FPGA VCCAUX Voltage";
        info.description = "FPGA VCCAUX voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccaux_min")
    {
        info.key = "xadc_vccaux_min";
        info.value = "0.000";
        info.name = "FPGA Min. VCCAUX Voltage";
        info.description = "Minimum FPGA VCCAUX voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccaux_max")
    {
        info.key = "xadc_vccaux_max";
        info.value = "0.000";
        info.name = "FPGA Max. VCCAUX Voltage";
        info.description = "Maximum FPGA VCCAUX voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccbram")
    {
        info.key = "xadc_vccbram";
        info.value = "0.000";
        info.name = "FPGA VCCBRAM Voltage";
        info.description = "FPGA VCCBRAM voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccbram_min")
    {
        info.key = "xadc_vccbram_min";
        info.value = "0.000";
        info.name = "FPGA Min. VCCBRAM Voltage";
        info.description = "Minimum FPGA VCCBRAM voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    else if(key == "xadc_vccbram_max")
    {
        info.key = "xadc_vccbram_max";
        info.value = "0.000";
        info.name = "FPGA Max. VCCBRAM Voltage";
        info.description = "Maximum FPGA VCCBRAM voltage as measured by the Xilinx XADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 5.0);
    }
    // PMC sensors
    else if(key == "pmc_temp")
    {
        info.key = "pmc_temp";
        info.value = "0.000";
        info.name = "Power Management Controller Temperature";
        info.description = "PMC die temperature as measured by the internal ADC semsor";
        info.units = "°C";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(-40.0, 125.0);
    }
    else if(key == "pmc_vin")
    {
        info.key = "pmc_vin";
        info.value = "0.000";
        info.name = "VIN Voltage";
        info.description = "Main regulator input voltage as measured by the PMC ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 24.0);
    }
    else if(key == "pmc_vext")
    {
        info.key = "pmc_vext";
        info.value = "0.000";
        info.name = "VEXT Voltage";
        info.description = "Voltage at the 2EDG connector as measured by the PMC ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 24.0);
    }
    else if(key == "pmc_12v0")
    {
        info.key = "pmc_12v0";
        info.value = "0.000";
        info.name = "12V0 Voltage";
        info.description = "PCIe edge connector 12V0 rail voltage as measured by the PMC ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 24.0);
    }
    else if(key == "pmc_vbus")
    {
        info.key = "pmc_vbus";
        info.value = "0.000";
        info.name = "VBUS Voltage";
        info.description = "USB Bus voltage as measured by the PMC ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.0, 24.0);
    }
    else if(key == "pmc_iovdd")
    {
        info.key = "pmc_iovdd";
        info.value = "0.000";
        info.name = "Power Management Controller IOVDD Voltage";
        info.description = "PMC IOVDD voltage (3V3_PM) as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(2.5, 3.6);
    }
    else if(key == "pmc_core_vdd")
    {
        info.key = "pmc_core_vdd";
        info.value = "0.000";
        info.name = "Power Management Controller Core Voltage";
        info.description = "PMC Core voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(1.1, 1.3);
    }
    // LT7182S sensors (VIN Regulator)
    else if(key == "vin_reg_temp")
    {
        info.key = "vin_reg_temp";
        info.value = "0.000";
        info.name = "Main Regulator Temperature";
        info.description = "Main regulator die temperature";
        info.units = "°C";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(-60.0, 160.0);
    }
    else if(key == "vin_reg_temp_max")
    {
        info.key = "vin_reg_temp_max";
        info.value = "0.000";
        info.name = "Main Regulator Max. Temperature";
        info.description = "Maximum main regulator die temperature";
        info.units = "°C";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(-60.0, 160.0);
    }
    else if(key == "vin_reg_ext_vcc")
    {
        info.key = "vin_reg_ext_vcc";
        info.value = "0.000";
        info.name = "Main Regulator External VCC Voltage";
        info.description = "Main regulator external VCC voltage (3V3_PM) as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(2.5, 3.6);
    }
    else if(key == "vin_reg_5v0_vin")
    {
        info.key = "vin_reg_5v0_vin";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Input Voltage";
        info.description = "Main regulator 5V0 channel (CH 0) input voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 20);
    }
    else if(key == "vin_reg_5v0_vin_peak")
    {
        info.key = "vin_reg_5v0_vin_peak";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Input Peak Voltage";
        info.description = "Main regulator 5V0 channel (CH 0) peak input voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 20);
    }
    else if(key == "vin_reg_5v0_iin")
    {
        info.key = "vin_reg_5v0_iin";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Input Current";
        info.description = "Main regulator 5V0 channel (CH 0) input current as measured by the internal ADC";
        info.units = "A";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 8);
    }
    else if(key == "vin_reg_5v0_vout")
    {
        info.key = "vin_reg_5v0_vout";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Output Voltage";
        info.description = "Main regulator 5V0 channel (CH 0) output voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(4.75, 5.35);
    }
    else if(key == "vin_reg_5v0_vout_peak")
    {
        info.key = "vin_reg_5v0_vout_peak";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Output Peak Voltage";
        info.description = "Main regulator 5V0 channel (CH 0) peak output voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(4.75, 5.35);
    }
    else if(key == "vin_reg_5v0_iout")
    {
        info.key = "vin_reg_5v0_iout";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Output Current";
        info.description = "Main regulator 5V0 channel (CH 0) output current as measured by the internal ADC";
        info.units = "A";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 8);
    }
    else if(key == "vin_reg_5v0_iout_peak")
    {
        info.key = "vin_reg_5v0_iout_peak";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Output Peak Current";
        info.description = "Main regulator 5V0 channel (CH 0) peak output current as measured by the internal ADC";
        info.units = "A";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 8);
    }
    else if(key == "vin_reg_5v0_pout")
    {
        info.key = "vin_reg_5v0_pout";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Output Power";
        info.description = "Main regulator 5V0 channel (CH 0) internally calculated output power";
        info.units = "W";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 50);
    }
    else if(key == "vin_reg_5v0_sw_freq")
    {
        info.key = "vin_reg_5v0_sw_freq";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel Switching Frequency";
        info.description = "Main regulator 5V0 channel (CH 0) switching frequency as measured internally";
        info.units = "kHz";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 4000);
    }
    else if(key == "vin_reg_5v0_vith")
    {
        info.key = "vin_reg_5v0_vith";
        info.value = "0.000";
        info.name = "Main Regulator 5V0 Channel ITH Voltage";
        info.description = "Main regulator 5V0 channel (CH 0) error amplifier output voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 20);
    }
    else if(key == "vin_reg_1v0_vin")
    {
        info.key = "vin_reg_1v0_vin";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Input Voltage";
        info.description = "Main regulator 1V0 channel (CH 1) input voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 20);
    }
    else if(key == "vin_reg_1v0_vin_peak")
    {
        info.key = "vin_reg_1v0_vin_peak";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Input Peak Voltage";
        info.description = "Main regulator 1V0 channel (CH 1) peak input voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 20);
    }
    else if(key == "vin_reg_1v0_iin")
    {
        info.key = "vin_reg_1v0_iin";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Input Current";
        info.description = "Main regulator 1V0 channel (CH 1) input current as measured by the internal ADC";
        info.units = "A";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 8);
    }
    else if(key == "vin_reg_1v0_vout")
    {
        info.key = "vin_reg_1v0_vout";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Output Voltage";
        info.description = "Main regulator 1V0 channel (CH 1) output voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.95, 1.05);
    }
    else if(key == "vin_reg_1v0_vout_peak")
    {
        info.key = "vin_reg_1v0_vout_peak";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Output Peak Voltage";
        info.description = "Main regulator 1V0 channel (CH 1) peak output voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0.95, 1.05);
    }
    else if(key == "vin_reg_1v0_iout")
    {
        info.key = "vin_reg_1v0_iout";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Output Current";
        info.description = "Main regulator 1V0 channel (CH 1) output current as measured by the internal ADC";
        info.units = "A";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 8);
    }
    else if(key == "vin_reg_1v0_iout_peak")
    {
        info.key = "vin_reg_1v0_iout_peak";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Output Peak Current";
        info.description = "Main regulator 1V0 channel (CH 1) peak output current as measured by the internal ADC";
        info.units = "A";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 8);
    }
    else if(key == "vin_reg_1v0_pout")
    {
        info.key = "vin_reg_1v0_pout";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Output Power";
        info.description = "Main regulator 1V0 channel (CH 1) internally calculated output power";
        info.units = "W";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 6);
    }
    else if(key == "vin_reg_1v0_sw_freq")
    {
        info.key = "vin_reg_1v0_sw_freq";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel Switching Frequency";
        info.description = "Main regulator 1V0 channel (CH 1) switching frequency as measured internally";
        info.units = "kHz";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 4000);
    }
    else if(key == "vin_reg_1v0_vith")
    {
        info.key = "vin_reg_1v0_vith";
        info.value = "0.000";
        info.name = "Main Regulator 1V0 Channel ITH Voltage";
        info.description = "Main regulator 1V0 channel (CH 1) error amplifier output voltage as measured by the internal ADC";
        info.units = "V";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(0, 20);
    }
    // mmWave Synthesizer sensors
    // else if(key == "mmw_synth_temp")
    // {
    //     info.key = "mmw_synth_temp";
    //     info.value = "0.000";
    //     info.name = "mmWave Synthesizer Temperature";
    //     info.description = "mmWave synthesizer die temperature";
    //     info.units = "°C";
    //     info.type = SoapySDR::ArgInfo::FLOAT;
    //     info.range = SoapySDR::Range(-40.0, 125.0);
    // }
    else if(key == "mmw_synth_locked")
    {
        info.key = "mmw_synth_locked";
        info.value = "false";
        info.name = "mmWave Synthesizer PLL Locked";
        info.description = "mmWave synthesizer PLL lock status";
        info.type = SoapySDR::ArgInfo::BOOL;
    }
    else if(key == "mmw_synth_vco_id")
    {
        info.key = "mmw_synth_vco_id";
        info.value = "0";
        info.name = "mmWave Synthesizer VCO ID";
        info.description = "mmWave synthesizer current VCO ID";
        info.units = "";
        info.type = SoapySDR::ArgInfo::INT;
        info.range = SoapySDR::Range(0, 7);
    }
    else if(key == "mmw_synth_vco_dig_band_id")
    {
        info.key = "mmw_synth_vco_band_id";
        info.value = "0";
        info.name = "mmWave Synthesizer VCO Band ID";
        info.description = "mmWave synthesizer digital VCO band ID";
        info.units = "";
        info.type = SoapySDR::ArgInfo::INT;
        info.range = SoapySDR::Range(0, 127);
    }
    // RF Phy sensors
    else if(key == "rf_phy_temp")
    {
        info.key = "rf_phy_temp";
        info.value = "0.000";
        info.name = "RF Phy Temperature";
        info.description = "RF Phy (transceiver) die temperature";
        info.units = "°C";
        info.type = SoapySDR::ArgInfo::FLOAT;
        info.range = SoapySDR::Range(-40.0, 125.0);
    }

    return info;
}
std::string SoapyIcyRadio::readSensor(const std::string &key) const
{
    char buf[16] = {0};

    // XADC sensors (FPGA)
    if(key == "xadc_temp")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readTemperature());
    else if(key == "xadc_temp_min")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMinTemperature());
    else if(key == "xadc_temp_max")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMaxTemperature());
    else if(key == "xadc_vccint")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readVCCINT());
    else if(key == "xadc_vccint_min")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMinVCCINT());
    else if(key == "xadc_vccint_max")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMaxVCCINT());
    else if(key == "xadc_vccaux")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readVCCAUX());
    else if(key == "xadc_vccaux_min")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMinVCCAUX());
    else if(key == "xadc_vccaux_max")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMaxVCCAUX());
    else if(key == "xadc_vccbram")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readVBRAM());
    else if(key == "xadc_vccbram_min")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMinVBRAM());
    else if(key == "xadc_vccbram_max")
        snprintf(buf, sizeof(buf), "%.3f", this->axi_xadc->readMaxVBRAM());
    // PMC sensors
    else if(key == "pmc_temp")
        snprintf(buf, sizeof(buf), "%.3f", this->pmc->getTemperature());
    else if(key == "pmc_vin")
        snprintf(buf, sizeof(buf), "%.3f", this->pmc->getVINVoltage());
    else if(key == "pmc_vext")
        snprintf(buf, sizeof(buf), "%.3f", this->pmc->getVEXTVoltage());
    else if(key == "pmc_12v0")
        snprintf(buf, sizeof(buf), "%.3f", this->pmc->get12V0Voltage());
    else if(key == "pmc_vbus")
        snprintf(buf, sizeof(buf), "%.3f", this->pmc->getVBUSVoltage());
    else if(key == "pmc_iovdd")
        snprintf(buf, sizeof(buf), "%.3f", this->pmc->getIOVDDVoltage());
    else if(key == "pmc_core_vdd")
        snprintf(buf, sizeof(buf), "%.3f", this->pmc->getCoreVoltage());
    // LT7182S sensors (VIN Regulator)
    else if(key == "vin_reg_temp")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readTemperature());
    else if(key == "vin_reg_temp_max")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readPeakTemperature());
    else if(key == "vin_reg_ext_vcc")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readExternalVCCVoltage());
    else if(key == "vin_reg_5v0_vin")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelInputVoltage(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_vin_peak")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelPeakInputVoltage(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_iin")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelInputCurrent(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_vout")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelOutputVoltage(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_vout_peak")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelPeakOutputVoltage(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_iout")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelOutputCurrent(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_iout_peak")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelPeakOutputCurrent(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_pout")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelOutputPower(LT7182S::Chan::CH0));
    else if(key == "vin_reg_5v0_sw_freq")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelFrequency(LT7182S::Chan::CH0) / 1000.f);
    else if(key == "vin_reg_5v0_vith")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelITHVoltage(LT7182S::Chan::CH0));
    else if(key == "vin_reg_1v0_vin")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelInputVoltage(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_vin_peak")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelPeakInputVoltage(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_iin")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelInputCurrent(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_vout")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelOutputVoltage(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_vout_peak")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelPeakOutputVoltage(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_iout")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelOutputCurrent(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_iout_peak")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelPeakOutputCurrent(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_pout")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelOutputPower(LT7182S::Chan::CH1));
    else if(key == "vin_reg_1v0_sw_freq")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelFrequency(LT7182S::Chan::CH1) / 1000.f);
    else if(key == "vin_reg_1v0_vith")
        snprintf(buf, sizeof(buf), "%.3f", this->vin_reg->readChannelITHVoltage(LT7182S::Chan::CH1));
    // mmWave Synthesizer sensors
    // else if(key == "mmw_synth_temp")
    //     snprintf(buf, sizeof(buf), "%.3f", this->mmw_synth->getTemperature());
    else if(key == "mmw_synth_locked")
        snprintf(buf, sizeof(buf), "%s", this->mmw_synth->isPLLLocked() ? "true" : "false");
    else if(key == "mmw_synth_vco_id")
        snprintf(buf, sizeof(buf), "%hhu", this->mmw_synth->getCurrentVCO());
    else if(key == "mmw_synth_vco_dig_band_id")
        snprintf(buf, sizeof(buf), "%hhu", this->mmw_synth->getCurrentDigitalBand());
    // RF Phy sensors
    else if(key == "rf_phy_temp")
        snprintf(buf, sizeof(buf), "%.3f", this->rf_phy->getTemperature());

    return buf;
}
std::vector<std::string> SoapyIcyRadio::listSensors(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("listSensors: Unknown direction");

    std::vector<std::string> sensors;

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            sensors.push_back("preamble_rssi");
            sensors.push_back("symbol_rssi");
        }
        else
        {
            // No sensors
        }
    }

    return sensors;
}
SoapySDR::ArgInfo SoapyIcyRadio::getSensorInfo(const int direction, const size_t channel, const std::string &key) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("getSensorInfo: Unknown direction");

    SoapySDR::ArgInfo info;

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            if(key == "preamble_rssi")
            {
                info.key = "preamble_rssi";
                info.value = "0.00";
                info.name = "RX Channel " + std::to_string(channel) + " preamble phase RSSI";
                info.description = "RSSI as reported by the RF PHY for the preamble phase of RX channel " + std::to_string(channel);
                info.units = "dB";
                info.type = SoapySDR::ArgInfo::FLOAT;
                info.range = SoapySDR::Range(-128.0, 0.0, 0.25);
            }
            else if(key == "symbol_rssi")
            {
                info.key = "symbol_rssi";
                info.value = "0.00";
                info.name = "RX Channel " + std::to_string(channel) + " symbol phase RSSI";
                info.description = "RSSI as reported by the RF PHY for the symbol phase of RX channel " + std::to_string(channel);
                info.units = "dB";
                info.type = SoapySDR::ArgInfo::FLOAT;
                info.range = SoapySDR::Range(-128.0, 0.0, 0.25);
            }
        }
        else
        {
            // No sensors
        }
    }

    return info;
}
std::string SoapyIcyRadio::readSensor(const int direction, const size_t channel, const std::string &key) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("readSensor: Unknown direction");

    char buf[16] = {0};

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            if(key == "preamble_rssi")
                snprintf(buf, sizeof(buf), "%.2f", this->rf_phy->readPreambleRSSI(BIT(channel)));
            else if(key == "symbol_rssi")
                snprintf(buf, sizeof(buf), "%.2f", this->rf_phy->readSymbolRSSI(BIT(channel)));
        }
        else
        {
            // No sensors
        }
    }

    return buf;
}


SoapySDR::ArgInfoList SoapyIcyRadio::getSettingInfo() const
{
    SoapySDR::ArgInfoList settings;

    return settings;
}
void SoapyIcyRadio::writeSetting(const std::string &key, const std::string &value)
{
    // No settings to write
}
std::string SoapyIcyRadio::readSetting(const std::string &key) const
{
    // No settings to read
    return "";
}
SoapySDR::ArgInfoList SoapyIcyRadio::getSettingInfo(const int direction, const size_t channel) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("setAntenna: Unknown direction");

    SoapySDR::ArgInfoList settings;

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            SoapySDR::ArgInfo s;

            s.key = "bias_t";
            s.value = "false";
            s.name = "Bias-Tee";
            s.description = "Enable or disable the Bias-Tee power on RX" + std::to_string(BIT(channel)) + "C";
            s.type = SoapySDR::ArgInfo::Type::BOOL;

            settings.push_back(s);
        }
        else
        {
            SoapySDR::ArgInfo s;

            s.key = "pa";
            s.value = "false";
            s.name = "Power Amplifier";
            s.description = "Enable or disable the power amplifier on TX" + std::to_string(BIT(channel)) + "B";
            s.type = SoapySDR::ArgInfo::Type::BOOL;

            settings.push_back(s);
        }
    }

    return settings;
}
void SoapyIcyRadio::writeSetting(const int direction, const size_t channel, const std::string &key, const std::string &value)
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("writeSetting: Unknown direction");

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            if(key == "bias_t")
            {
                // GPO 0 for channel 0, GPO 1 for channel 1
                if(value == "true")
                    this->rf_phy->setGPOValue(channel, true);
                else
                    this->rf_phy->setGPOValue(channel, false);
            }
        }
        else
        {
            if(key == "pa")
            {
                // GPO 2 for channel 0, GPO 3 for channel 1
                if(value == "true")
                    this->rf_phy->setGPOValue(channel + 2, true);
                else
                    this->rf_phy->setGPOValue(channel + 2, false);
            }
        }
    }
}
std::string SoapyIcyRadio::readSetting(const int direction, const size_t channel, const std::string &key) const
{
    if(direction != SOAPY_SDR_RX && direction != SOAPY_SDR_TX)
        throw std::runtime_error("readSetting: Unknown direction");

    if(channel <= 1)
    {
        // RF Channels
        if(direction == SOAPY_SDR_RX)
        {
            if(key == "bias_t")
            {
                // GPO 0 for channel 0, GPO 1 for channel 1
                return this->rf_phy->getGPOValue(channel) ? "true" : "false";
            }
        }
        else
        {
            if(key == "pa")
            {
                // GPO 2 for channel 0, GPO 3 for channel 1
                return this->rf_phy->getGPOValue(channel + 2) ? "true" : "false";
            }
        }
    }

    return "";
}