#include "SoapyIcyRadio.hpp"

// Default FIR coefficients for decimation/interpolation when sample rates below 2.0833... MHz are needed
// Gain is 6 dB, set AD9361 gain to -6 dB to compensate
static const int16_t rf_phy_fir_128_dec4[] = {
	   -15,   -27,   -23,    -6,    17,    33,    31,     9,   -23,   -47,   -45,   -13,    34,    69,    67,    21,
       -49,  -102,   -99,   -32,    69,   146,   143,    48,   -96,  -204,  -200,   -69,   129,   278,   275,    97,
      -170,  -372,  -371,  -135,   222,   494,   497,   187,  -288,  -654,  -665,  -258,   376,   875,   902,   363,
      -500, -1201, -1265,  -530,   699,  1748,  1906,   845, -1089, -2922, -3424, -1697,  2326,  7714, 12821, 15921,
     15921, 12821,  7714,  2326, -1697, -3424, -2922, -1089,   845,  1906,  1748,   699,  -530, -1265, -1201,  -500,
       363,   902,   875,   376,  -258,  -665,  -654,  -288,   187,   497,   494,   222,  -135,  -371,  -372,  -170,
        97,   275,   278,   129,   -69,  -200,  -204,   -96,    48,   143,   146,    69,   -32,   -99,  -102,   -49,
        21,    67,    69,    34,   -13,   -45,   -47,   -23,     9,    31,    33,    17,    -6,   -23,   -27,   -15
};
static const int16_t rf_phy_fir_128_dec2[] = {
        0,     0,     1,     0,    -2,     0,     3,     0,    -5,     0,     8,     0,   -11,     0,    17,     0,
      -24,     0,    33,     0,   -45,     0,    61,     0,   -80,     0,   104,     0,  -134,     0,   169,     0,
     -213,     0,   264,     0,  -327,     0,   401,     0,  -489,     0,   595,     0,  -724,     0,   880,     0,
    -1075,     0,  1323,     0, -1652,     0,  2114,     0, -2819,     0,  4056,     0, -6883,     0, 20837, 32767,
    20837,     0, -6883,     0,  4056,     0, -2819,     0,  2114,     0, -1652,     0,  1323,     0, -1075,     0,
      880,     0,  -724,     0,   595,     0,  -489,     0,   401,     0,  -327,     0,   264,     0,  -213,     0,
      169,     0,  -134,     0,   104,     0,   -80,     0,    61,     0,   -45,     0,    33,     0,   -24,     0,
       17,     0,   -11,     0,     8,     0,    -5,     0,     3,     0,    -2,     0,     1,     0,    -0,     0
};

void SoapyIcyRadio::parseConfig(const SoapySDR::Kwargs &args)
{
    SoapySDR::Kwargs c_args = args;

    // Remove identification arguments
    c_args.erase("driver");
    c_args.erase("path");
    c_args.erase("device_id");
    c_args.erase("serial");
    c_args.erase("label");

    // Parse configuration
    DLOGF(SOAPY_SDR_DEBUG, "Parsing configuration string \"%s\"", SoapySDR::KwargsToString(c_args).c_str());

    // use_ext_clk_in
    if(c_args.count("use_ext_clk_in") > 0)
    {
        this->config.use_clkin = true;

        DLOGF(SOAPY_SDR_INFO, "Using external clock input as clock source");
    }
    else
    {
        this->config.use_clkin = false;
    }

    // ext_clk_in_freq
    if(args.count("ext_clk_in_freq") > 0)
    {
        double f = std::stod(c_args.at("ext_clk_in_freq"));

        if(f < 0 || f > UINT32_MAX)
        {
            this->config.clkin_freq = 10e6;

            DLOGF(SOAPY_SDR_WARNING, "Invalid external clock input frequency provided (%.6f), using default: 10 MHz", f);
        }
        else
        {
            this->config.clkin_freq = f;

            DLOGF(SOAPY_SDR_INFO, "External clock input frequency is %.6f Hz", this->config.clkin_freq);
        }
    }
    else
    {
        if(this->config.use_clkin)
            DLOGF(SOAPY_SDR_WARNING, "External clock input usage requested, but no frequency provided, using default: 10 MHz");

        this->config.clkin_freq = 10e6;
    }

    // en_ext_clk_out
    if(c_args.count("en_ext_clk_out") > 0)
    {
        this->config.enable_clkout = true;

        DLOGF(SOAPY_SDR_INFO, "Enabling external clock output");
    }
    else
    {
        this->config.enable_clkout = false;
    }

    // ext_clk_out_freq
    if(args.count("ext_clk_out_freq") > 0)
    {
        double f = std::stod(c_args.at("ext_clk_out_freq"));

        if(f < 0 || f > UINT32_MAX)
        {
            this->config.clkout_freq = 10e6;

            DLOGF(SOAPY_SDR_WARNING, "Invalid external clock output frequency provided (%.6f), using default: 10 MHz", f);
        }
        else
        {
            this->config.clkout_freq = f;

            DLOGF(SOAPY_SDR_INFO, "Requested external clock output frequency is %.6f Hz", this->config.clkout_freq);
        }
    }
    else
    {
        if(this->config.enable_clkout)
            DLOGF(SOAPY_SDR_WARNING, "External clock output enabled, but no frequency provided, using default: 10 MHz");

        this->config.clkout_freq = 10e6;
    }
}

void SoapyIcyRadio::setupMemoryMaps()
{
    if(this->fd < 0)
        throw std::runtime_error("Invalid file descriptor");

    // Map AXI address space
    this->mm_axi_flash = new MappedRegion(this->fd, AXI_FLASH_XIP_BASE, AXI_FLASH_XIP_SIZE);
    this->mm_axi_bram = new MappedRegion(this->fd, AXI_BRAM_BASE, AXI_BRAM_SIZE);
    this->mm_axi_dna = new MappedRegion(this->fd, AXI_DNA_BASE, AXI_DNA_SIZE);
    this->mm_axi_ddr = new MappedRegion(this->fd, AXI_MIG_DDR3_BASE, AXI_MIG_DDR3_SIZE);
    this->mm_axi_periph = new MappedRegion(this->fd, AXI_PERIPH_BASE, AXI_PERIPH_SIZE);

    DLOGF(SOAPY_SDR_TRACE, "AXI Flash: Phys = %016llX, Virt = %016llX, Size = %016llX", this->mm_axi_flash->getPhys(), this->mm_axi_flash->getVirt(), this->mm_axi_flash->getSize());
    DLOGF(SOAPY_SDR_TRACE, "AXI BRAM: Phys = %016llX, Virt = %016llX, Size = %016llX", this->mm_axi_bram->getPhys(), this->mm_axi_bram->getVirt(), this->mm_axi_bram->getSize());
    DLOGF(SOAPY_SDR_TRACE, "AXI DNA: Phys = %016llX, Virt = %016llX, Size = %016llX", this->mm_axi_dna->getPhys(), this->mm_axi_dna->getVirt(), this->mm_axi_dna->getSize());
    DLOGF(SOAPY_SDR_TRACE, "AXI DDR: Phys = %016llX, Virt = %016llX, Size = %016llX", this->mm_axi_ddr->getPhys(), this->mm_axi_ddr->getVirt(), this->mm_axi_ddr->getSize());
    DLOGF(SOAPY_SDR_TRACE, "AXI Peripherals: Phys = %016llX, Virt = %016llX, Size = %016llX", this->mm_axi_periph->getPhys(), this->mm_axi_periph->getVirt(), this->mm_axi_periph->getSize());

    // Setup DNA (ROM)
    this->axi_dna = new AXIDNA(this->mm_axi_dna->getVirt(AXI_DNA_BASE));

    // Setup mapped peripherals
    this->axi_dmac[AXI_DMAC_RF_TX0_INST] = new AXIDMAC(this->mm_axi_periph->getVirt(AXI_DMAC_RF_TX0_BASE));
    this->axi_dmac[AXI_DMAC_RF_TX1_INST] = new AXIDMAC(this->mm_axi_periph->getVirt(AXI_DMAC_RF_TX1_BASE));
    this->axi_dmac[AXI_DMAC_RF_RX0_INST] = new AXIDMAC(this->mm_axi_periph->getVirt(AXI_DMAC_RF_RX0_BASE));
    this->axi_dmac[AXI_DMAC_RF_RX1_INST] = new AXIDMAC(this->mm_axi_periph->getVirt(AXI_DMAC_RF_RX1_BASE));
    this->axi_dmac[AXI_DMAC_I2S_TX_INST] = new AXIDMAC(this->mm_axi_periph->getVirt(AXI_DMAC_I2S_TX_BASE));
    this->axi_dmac[AXI_DMAC_I2S_RX_INST] = new AXIDMAC(this->mm_axi_periph->getVirt(AXI_DMAC_I2S_RX_BASE));
    this->axi_iic[AXI_IIC_CODEC_INST] = new AXIIIC(this->mm_axi_periph->getVirt(AXI_IIC0_BASE));
    this->axi_iic[AXI_IIC_SYS_INST] = new AXIIIC(this->mm_axi_periph->getVirt(AXI_IIC1_BASE));
    this->axi_iic[AXI_IIC_EXP_INST] = new AXIIIC(this->mm_axi_periph->getVirt(AXI_IIC2_BASE));
    this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST] = new AXISPI(this->mm_axi_periph->getVirt(AXI_QUAD_SPI_MM0_BASE));
    this->axi_spi[AXI_SPI_TRX_INST] = new AXISPI(this->mm_axi_periph->getVirt(AXI_SPI0_BASE));
    this->axi_spi[AXI_SPI_SYNTH_INST] = new AXISPI(this->mm_axi_periph->getVirt(AXI_SPI1_BASE));
    this->axi_gpio[AXI_GPIO_TRX_INST] = new AXIGPIO(this->mm_axi_periph->getVirt(AXI_GPIO0_BASE));
    this->axi_gpio[AXI_GPIO_SYNTH_INST] = new AXIGPIO(this->mm_axi_periph->getVirt(AXI_GPIO1_BASE));
    this->axi_gpio[AXI_GPIO_SYS_INST] = new AXIGPIO(this->mm_axi_periph->getVirt(AXI_GPIO2_BASE));
    this->axi_pcie = new AXIPCIe(this->mm_axi_periph->getVirt(AXI_PCIE0_BASE));
    this->axi_rf_tstamp = new AXIRFTStamp(this->mm_axi_periph->getVirt(AXI_RF_TSTAMP_BASE));
    this->axi_irq_ctrl = new AXIIRQCtrl(this->mm_axi_periph->getVirt(AXI_IRQ_CTRL_BASE));
    this->axi_i2s = new AXII2S(this->mm_axi_periph->getVirt(AXI_I2S_BASE));
    this->axi_xadc = new AXIXADC(this->mm_axi_periph->getVirt(AXI_XADC_WIZ_BASE));
    // RSVD
    this->axi_ad9361 = new AXIAD9361(this->mm_axi_periph->getVirt(AXI_AD9361_BASE));

    // Allocate DMA buffer memory
    uint32_t dma_sz = ICYRADIO_DEFAULT_TOTAL_DMA_POOL_SIZE_BYTES; // TODO: make this configurable

    DLOGF(SOAPY_SDR_DEBUG, "Allocating DMA buffer pool of %u bytes", dma_sz);

    uint64_t arg = dma_sz;

    ioctl(this->fd, ICYRADIO_IOCTL_DMA_FREE); // TODO: Implement getting an existing buffer

    if(ioctl(this->fd, ICYRADIO_IOCTL_DMA_ALLOC, &arg) < 0)
        throw std::runtime_error("Could not allocate DMA buffer memory (" + std::string(std::strerror(errno)) + ")");

    // Map DMA buffer memory
    this->mm_dma_buffer = new MappedRegion(this->fd, arg | BIT(48), dma_sz);

    DLOGF(SOAPY_SDR_TRACE, "DMA Buffer: Phys = %016llX, Virt = %016llX, Size = %016llX", this->mm_dma_buffer->getPhys() & (BIT(48) - 1), this->mm_dma_buffer->getVirt(), this->mm_dma_buffer->getSize());
}
void SoapyIcyRadio::freeMemoryMaps()
{
    // AXI Peripherals
    if(this->axi_dna != nullptr)
    {
        delete this->axi_dna;

        this->axi_dna = nullptr;
    }

    for(uint8_t i = 0; i < AXI_DMAC_NUM_INSTANCES; i++)
    {
        if(this->axi_dmac[i] != nullptr)
        {
            delete this->axi_dmac[i];

            this->axi_dmac[i] = nullptr;
        }
    }

    for(uint8_t i = 0; i < AXI_IIC_NUM_INSTANCES; i++)
    {
        if(this->axi_iic[i] != nullptr)
        {
            delete this->axi_iic[i];

            this->axi_iic[i] = nullptr;
        }
    }

    for(uint8_t i = 0; i < AXI_SPI_NUM_INSTANCES; i++)
    {
        if(this->axi_spi[i] != nullptr)
        {
            delete this->axi_spi[i];

            this->axi_spi[i] = nullptr;
        }
    }

    for(uint8_t i = 0; i < AXI_GPIO_NUM_INSTANCES; i++)
    {
        if(this->axi_gpio[i] != nullptr)
        {
            delete this->axi_gpio[i];

            this->axi_gpio[i] = nullptr;
        }
    }

    if(this->axi_pcie != nullptr)
    {
        delete this->axi_pcie;

        this->axi_pcie = nullptr;
    }

    if(this->axi_rf_tstamp != nullptr)
    {
        delete this->axi_rf_tstamp;

        this->axi_rf_tstamp = nullptr;
    }

    if(this->axi_irq_ctrl != nullptr)
    {
        delete this->axi_irq_ctrl;

        this->axi_irq_ctrl = nullptr;
    }

    if(this->axi_i2s != nullptr)
    {
        delete this->axi_i2s;

        this->axi_i2s = nullptr;
    }

    if(this->axi_xadc != nullptr)
    {
        delete this->axi_xadc;

        this->axi_xadc = nullptr;
    }

    if(this->axi_ad9361 != nullptr)
    {
        delete this->axi_ad9361;

        this->axi_ad9361 = nullptr;
    }

    // Memory maps
    if(this->mm_axi_flash != nullptr)
    {
        delete this->mm_axi_flash;

        this->mm_axi_flash = nullptr;
    }

    if(this->mm_axi_bram != nullptr)
    {
        delete this->mm_axi_bram;

        this->mm_axi_bram = nullptr;
    }

    if(this->mm_axi_dna != nullptr)
    {
        delete this->mm_axi_dna;

        this->mm_axi_dna = nullptr;
    }

    if(this->mm_axi_ddr != nullptr)
    {
        delete this->mm_axi_ddr;

        this->mm_axi_ddr = nullptr;
    }

    if(this->mm_axi_periph != nullptr)
    {
        delete this->mm_axi_periph;

        this->mm_axi_periph = nullptr;
    }

    if(this->mm_dma_buffer != nullptr)
    {
        delete this->mm_dma_buffer;

        this->mm_dma_buffer = nullptr;
    }

    ioctl(this->fd, ICYRADIO_IOCTL_DMA_FREE);
}

void SoapyIcyRadio::initPeripheralsPreClocks()
{
    // PCIe
    uint8_t num_bars = this->axi_pcie->getNumBARs();

    DLOGF(SOAPY_SDR_DEBUG, "Detected %hhu AXI -> PCIe BARs", num_bars);

    if(num_bars < 6)
        throw std::runtime_error("AXI PCIe: Not enough BARs detected");

    this->axi_pcie->setBARAXIAddress(0, AXI_PCIE_BAR0_MM_BASE, AXI_PCIE_BAR0_MM_SIZE);
    this->axi_pcie->setBARAXIAddress(1, AXI_PCIE_BAR1_MM_BASE, AXI_PCIE_BAR1_MM_SIZE);
    this->axi_pcie->setBARAXIAddress(2, AXI_PCIE_BAR2_MM_BASE, AXI_PCIE_BAR2_MM_SIZE);
    this->axi_pcie->setBARAXIAddress(3, AXI_PCIE_BAR3_MM_BASE, AXI_PCIE_BAR3_MM_SIZE);
    this->axi_pcie->setBARAXIAddress(4, AXI_PCIE_BAR4_MM_BASE, AXI_PCIE_BAR4_MM_SIZE);
    this->axi_pcie->setBARAXIAddress(5, AXI_PCIE_BAR5_MM_BASE, AXI_PCIE_BAR5_MM_SIZE);

    bool pcie_done = false;
    uintptr_t pcie_start = this->mm_dma_buffer->getPhys() & (BIT(48) - 1);
    size_t pcie_rem = this->mm_dma_buffer->getSize();

    for(uint8_t i = 0; i < num_bars; i++)
    {
        if(!pcie_done)
        {
            this->axi_pcie->setBARPCIeAddress(i, pcie_start, pcie_rem);

            try
            {
                uint64_t _pcie_start = this->axi_pcie->getBARPCIeAddress(i, this->axi_pcie->getBARAXIAddress(i) + this->axi_pcie->getBARAXISize(i) - 1) + 1;

                if(_pcie_start > pcie_start)
                {
                    pcie_rem -= _pcie_start - pcie_start;
                    pcie_start = _pcie_start;
                }
                else
                {
                    pcie_done = true;
                }
            }
            catch(const std::runtime_error &e)
            {
                pcie_done = true;
            }
        }
        else
        {
            this->axi_pcie->setBARPCIeAddress(i, 0, 0);
        }

        DLOGF(SOAPY_SDR_TRACE, "  BAR #%hhu (%s):", i, this->axi_pcie->isBAR64Bit(i) ? "64-bit" : "32-bit");
        DLOGF(SOAPY_SDR_TRACE, "    AXI Base: 0x%08lX", this->axi_pcie->getBARAXIAddress(i));
        DLOGF(SOAPY_SDR_TRACE, "    AXI Size: 0x%08lX", this->axi_pcie->getBARAXISize(i));
        DLOGF(SOAPY_SDR_TRACE, "    PCIe Base: 0x%016llX", this->axi_pcie->getBARPCIeAddress(i));
        DLOGF(SOAPY_SDR_TRACE, "    PCIe Size: 0x%08lX", this->axi_pcie->getBARPCIeSize(i));

        try
        {
            DLOGF(SOAPY_SDR_TRACE, "    AXI Valid Start: 0x%08lX", this->axi_pcie->getBARAXIAddress(i, this->axi_pcie->getBARPCIeAddress(i)));

            if(pcie_done)
                DLOGF(SOAPY_SDR_TRACE, "    AXI Valid End: 0x%08lX", this->axi_pcie->getBARAXIAddress(i, this->axi_pcie->getBARPCIeAddress(i)) + this->axi_pcie->getBARPCIeSize(i) - 1);
            else
                DLOGF(SOAPY_SDR_TRACE, "    AXI Valid End: 0x%08lX", this->axi_pcie->getBARAXIAddress(i) + this->axi_pcie->getBARAXISize(i) - 1);
        }
        catch(const std::runtime_error &e)
        {
            DLOGF(SOAPY_SDR_TRACE, "    AXI Valid Start: N/A");
            DLOGF(SOAPY_SDR_TRACE, "    AXI Valid End: N/A");
        }
    }

    if(!pcie_done)
        throw std::runtime_error("AXI PCIe: Not enough AXI address space to map the DMA buffer, please reduce the buffer size");

    // IRQ Controller
    this->axi_irq_ctrl->init(this->fd);

    {
        uint32_t ver = this->axi_irq_ctrl->getIPVersion();

        DLOGF(SOAPY_SDR_TRACE, "AXI IRQ Controller:");
        DLOGF(SOAPY_SDR_TRACE, "  IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
    }

    this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_TX0, AXIIRQCtrl::IRQMode::LEVEL_HIGH, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_TX1, AXIIRQCtrl::IRQMode::LEVEL_HIGH, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_RX0, AXIIRQCtrl::IRQMode::LEVEL_HIGH, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_RX1, AXIIRQCtrl::IRQMode::LEVEL_HIGH, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::AXI_DMAC_I2S_TX, AXIIRQCtrl::IRQMode::LEVEL_HIGH, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::AXI_DMAC_I2S_RX, AXIIRQCtrl::IRQMode::LEVEL_HIGH, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::AXI_RF_TSTAMP, AXIIRQCtrl::IRQMode::LEVEL_HIGH, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    // this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::CLK_MNGR_IRQn, AXIIRQCtrl::IRQMode::EDGE_FALLING, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);
    // this->axi_irq_ctrl->configIRQ(AXIIRQCtrl::IRQNumber::VIN_REG_ALERTn, AXIIRQCtrl::IRQMode::EDGE_FALLING, AXI_IRQ_CTRL_REG_IRQ_CONFIG_IRQ_DEST_PCIE_MSI(0), false);

    // DMA Controllers
    for(uint8_t i = 0; i < AXI_DMAC_NUM_INSTANCES; i++)
    {
        uint32_t ver = this->axi_dmac[i]->getIPVersion();
        uint32_t id = this->axi_dmac[i]->getPeripheralID();
        AXIDMAC::Capabilities caps = this->axi_dmac[i]->getCapabilities();

        DLOGF(SOAPY_SDR_TRACE, "DMA Controller #%hhu:", i);
        DLOGF(SOAPY_SDR_TRACE, "  IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
        DLOGF(SOAPY_SDR_TRACE, "  Hardware ID: %u", id);
        DLOGF(SOAPY_SDR_TRACE, "  Configuration / Capabilities:");
        DLOGF(SOAPY_SDR_TRACE, "    Destination data bus width: %u bits", caps.dest_data_width);
        DLOGF(SOAPY_SDR_TRACE, "    Destination interface: %s", AXIDMAC::InterfaceTypeToString(caps.dest_interface).c_str());
        DLOGF(SOAPY_SDR_TRACE, "    Source data bus width: %u bits", caps.src_data_width);
        DLOGF(SOAPY_SDR_TRACE, "    Source interface: %s", AXIDMAC::InterfaceTypeToString(caps.src_interface).c_str());
        DLOGF(SOAPY_SDR_TRACE, "    Bytes per burst: %u", caps.bytes_per_burst);
        DLOGF(SOAPY_SDR_TRACE, "    Cyclic transfers: %s", caps.cyclic_support ? "Supported" : "Not supported");
        DLOGF(SOAPY_SDR_TRACE, "    Maximum transfer length: %u bytes", caps.max_transfer_size);
        DLOGF(SOAPY_SDR_TRACE, "    Destination address mask: 0x%08X", caps.dest_addr_mask);
        DLOGF(SOAPY_SDR_TRACE, "    Source address mask: 0x%08X", caps.src_addr_mask);
    }

    this->axi_dmac[AXI_DMAC_RF_TX0_INST]->init(
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_TX0,
        }
    );
    this->axi_dmac[AXI_DMAC_RF_TX1_INST]->init(
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_TX1,
        }
    );
    this->axi_dmac[AXI_DMAC_RF_RX0_INST]->init(
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_RX0,
        }
    );
    this->axi_dmac[AXI_DMAC_RF_RX1_INST]->init(
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::AXI_DMAC_RF_RX1,
        }
    );
    this->axi_dmac[AXI_DMAC_I2S_TX_INST]->init(
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::AXI_DMAC_I2S_TX,
        }
    );
    this->axi_dmac[AXI_DMAC_I2S_RX_INST]->init(
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::AXI_DMAC_I2S_RX,
        }
    );

    // GPIO
    for(uint8_t i = 0; i < AXI_GPIO_NUM_INSTANCES; i++)
    {
        uint32_t ver = this->axi_gpio[i]->getIPVersion();

        DLOGF(SOAPY_SDR_TRACE, "GPIO Controller #%hhu:", i);
        DLOGF(SOAPY_SDR_TRACE, "  IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
    }

    // I2C
    this->axi_iic[AXI_IIC_CODEC_INST]->init(AXI_ACLK_FREQ, AXIIIC::Speed::FAST);
    this->axi_iic[AXI_IIC_SYS_INST]->init(AXI_ACLK_FREQ, AXIIIC::Speed::FAST);
    this->axi_iic[AXI_IIC_EXP_INST]->init(AXI_ACLK_FREQ, AXIIIC::Speed::FAST);

    // I2C Scan
    #ifdef TRACE_I2C
    {
        std::vector<uint8_t> addrs;

        auto addrs_to_str = [](std::vector<uint8_t> addrs) -> std::string
        {
            std::stringstream s;

            s << "  Found " << addrs.size() << " devices";

            if(addrs.size() > 0)
            {
                s << " (";

                for(size_t i = 0; i < addrs.size(); i++)
                {
                    s << "0x" << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << (size_t)addrs[i];

                    if(i < addrs.size() - 1)
                        s << ", ";
                }

                s << ")";
            }

            return s.str();
        };

        //// CODEC I2C
        DLOGF(SOAPY_SDR_TRACE, "Scanning CODEC I2C bus...");

        this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_CODEC_RSTn_BIT, AXIGPIO::Value::HIGH);
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        addrs = this->axi_iic[AXI_IIC_CODEC_INST]->scan();
        this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_CODEC_RSTn_BIT, AXIGPIO::Value::LOW);

        DLOGF(SOAPY_SDR_TRACE, "%s", addrs_to_str(addrs).c_str());

        //// SYS I2C
        DLOGF(SOAPY_SDR_TRACE, "Scanning SYS I2C bus...");

        this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_PM_I2C_EN_BIT, AXIGPIO::Value::HIGH);
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        addrs = this->axi_iic[AXI_IIC_SYS_INST]->scan();
        this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_PM_I2C_EN_BIT, AXIGPIO::Value::LOW);

        DLOGF(SOAPY_SDR_TRACE, "%s", addrs_to_str(addrs).c_str());

        //// EXP I2C
        DLOGF(SOAPY_SDR_TRACE, "Scanning EXP I2C bus...");

        addrs = this->axi_iic[AXI_IIC_EXP_INST]->scan();

        DLOGF(SOAPY_SDR_TRACE, "%s", addrs_to_str(addrs).c_str());

        ///////////////// EXP TEST
        if(addrs.size() > 0 && addrs.at(0) == 0x22)
        {
            AuxMCU *exp = new AuxMCU(
                {
                    .controller = this->axi_iic[AXI_IIC_EXP_INST],
                    .addr = 0x22,
                }
            );

            DLOGF(SOAPY_SDR_DEBUG, "reg0 before %02X", exp->readReg(0x00));
            exp->writeReg(0x00, 0x01);
            DLOGF(SOAPY_SDR_DEBUG, "reg0 after %02X", exp->readReg(0x00));
            DLOGF(SOAPY_SDR_DEBUG, "reg1 before %02X", exp->readReg(0x01));
            exp->writeReg(0x01, 0x01);
            DLOGF(SOAPY_SDR_DEBUG, "reg1 after %02X", exp->readReg(0x01));

            DLOGF(SOAPY_SDR_DEBUG, "rom0 %02X", exp->readROM(0x00));
            DLOGF(SOAPY_SDR_DEBUG, "rom1 %02X", exp->readROM(0x01));

            delete exp;
        }
        ///////////////// EXP TEST
    }
    #endif

    // SPI
    for(uint8_t i = 0; i < AXI_SPI_NUM_INSTANCES; i++)
    {
        uint32_t ver = this->axi_spi[i]->getIPVersion();
        AXISPI::Capabilities caps = this->axi_spi[i]->getCapabilities();

        DLOGF(SOAPY_SDR_TRACE, "(Q)SPI Controller #%hhu:", i);
        DLOGF(SOAPY_SDR_TRACE, "  IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
        DLOGF(SOAPY_SDR_TRACE, "  Configuration / Capabilities:");
        DLOGF(SOAPY_SDR_TRACE, "    Dual IO: %s", caps.dual_io_supported ? "Supported" : "Not supported");
        DLOGF(SOAPY_SDR_TRACE, "    Quad IO: %s", caps.quad_io_supported ? "Supported" : "Not supported");
        DLOGF(SOAPY_SDR_TRACE, "    Memory-mapped (XIP) accesses: %s", caps.mmio_supported ? "Supported" : "Not supported");

        AXISPI::Mode mode;
        AXISPI::BitOrder bit_order;
        AXISPI::IOMode io_mode;
        uint64_t input_freq;
        uint64_t sck_freq;

        switch(i)
        {
            case AXI_QUAD_SPI_MM0_FLASH_INST:
            {
                mode = AXISPI::Mode::MODE_0;
                bit_order = AXISPI::BitOrder::MSB_FIRST;
                io_mode = AXISPI::IOMode::SINGLE;
                input_freq = AXI_ACLK_FREQ;
                sck_freq = 20000000UL;
            }
            break;
            case AXI_SPI_TRX_INST:
            {
                mode = AXISPI::Mode::MODE_1;
                bit_order = AXISPI::BitOrder::MSB_FIRST;
                io_mode = AXISPI::IOMode::SINGLE;
                input_freq = AXI_ACLK_FREQ;
                sck_freq = 20000000UL;
            }
            break;
            case AXI_SPI_SYNTH_INST:
            {
                mode = AXISPI::Mode::MODE_0;
                bit_order = AXISPI::BitOrder::MSB_FIRST;
                io_mode = AXISPI::IOMode::SINGLE;
                input_freq = AXI_ACLK_FREQ;
                sck_freq = 12500000UL;
            }
            break;
            default:
            {
                DLOGF(SOAPY_SDR_TRACE, "  Unknown controller, leaving unconfigured");

                continue;
            }
        }

        this->axi_spi[i]->setMode(mode);
        this->axi_spi[i]->setBitOrder(bit_order);
        this->axi_spi[i]->setIOMode(io_mode);
        this->axi_spi[i]->setClockFrequency(input_freq, sck_freq);
        this->axi_spi[i]->enableClock();
        this->axi_spi[i]->enable();

        DLOGF(SOAPY_SDR_TRACE, "  SCK Divider input frequency: %llu Hz", input_freq);
        DLOGF(SOAPY_SDR_TRACE, "  Requested SCK frequency: %llu Hz", sck_freq);
        DLOGF(SOAPY_SDR_TRACE, "  Achieved SCK frequency: %llu Hz", this->axi_spi[i]->getClockFrequency(input_freq));
    }

    // Peripherals
    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_PM_I2C_EN_BIT, AXIGPIO::Value::HIGH);
    std::this_thread::sleep_for(std::chrono::microseconds(500));

    this->pmc = new PMC(
        {
            .controller = this->axi_iic[AXI_IIC_SYS_INST],
            .addr = PMC_I2C_ADDR,
        }
    );

    DLOGF(SOAPY_SDR_DEBUG, "PMC Unique ID: %s", this->pmc->getUniqueID().c_str());
    DLOGF(SOAPY_SDR_DEBUG, "PMC Firmware Version: v%hu", this->pmc->getFirmwareVersion());

    this->vin_reg = new LT7182S(
        {
            .controller = this->axi_iic[AXI_IIC_SYS_INST],
            .addr = LT7182S_I2C_ADDR,
        }/*,
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::VIN_REG_ALERTn,
        }*/
    );

    this->vin_reg->init();

    DLOGF(SOAPY_SDR_DEBUG, "VIN Regulator P/N: %s %s", this->vin_reg->readManufacturerID().c_str(), this->vin_reg->readManufacturerModel().c_str());
    DLOGF(SOAPY_SDR_DEBUG, "VIN Regulator Revision: %hhu", this->vin_reg->readManufacturerRevision());
    DLOGF(SOAPY_SDR_DEBUG, "VIN Regulator Serial: %s", this->vin_reg->readManufacturerSerial().c_str());

    this->spi_flash = new SPIFlash(
        {
            .controller = this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST],
            .ss_mask = AXI_QUAD_SPI_MM0_FLASH_SS,
        }
    );

    DLOGF(SOAPY_SDR_DEBUG, "SPI Flash JEDEC ID: 0x%06X", this->spi_flash->readJEDECID());
    DLOGF(SOAPY_SDR_DEBUG, "SPI Flash Device Name: %s", this->spi_flash->getDeviceName().c_str());
    DLOGF(SOAPY_SDR_DEBUG, "SPI Flash Unique ID: %016lX", this->spi_flash->readUniqueID());
    DLOGF(SOAPY_SDR_TRACE, "SPI Flash Status registers (0, 1, 2): 0x%02X, 0x%02X, 0x%02X", this->spi_flash->readStatus(), this->spi_flash->readStatus2(), this->spi_flash->readStatus3());

    //////////////////////// SPI FLASH TEST
    // {
    //     uint8_t buf[16 * 4];
    //     AXISPI::MMIOConfig mmio;
    //     AXISPI::MMIOStats stats;
    //     std::stringstream s;

    //     // Software read
    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->disableMMIO();
    //     this->spi_flash->read(0x000000, buf, sizeof(buf));

    //     s.str("");
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash software read:");
    //     for(size_t i = 0; i < sizeof(buf); i++)
    //     {
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)buf[i] << " ";

    //         if(i % 16 == 15)
    //         {
    //             DLOGF(SOAPY_SDR_DEBUG, "%s", s.str().c_str());
    //             s.str("");
    //         }
    //     }

    //     // MMIO QIO read
    //     mmio.rd_instr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.rd_instr = SPI_FLASH_CMD_READ_FAST_QIO;
    //     mmio.addr_io_mode = AXISPI::IOMode::QUAD;
    //     mmio.addr_bytes = 3;
    //     mmio.mode_bits = 0xF0;
    //     mmio.mode_bits_en = true;
    //     mmio.cont_read_en = false;
    //     mmio.dummy_io_mode = AXISPI::IOMode::QUAD;
    //     mmio.dummy_bytes = 2;
    //     mmio.data_io_mode = AXISPI::IOMode::QUAD;
    //     mmio.cs_high_wait = 0;
    //     mmio.cs_low_wait = 0;
    //     mmio.cs_mask = AXI_QUAD_SPI_MM0_FLASH_SS;

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->configMMIOMode(mmio);
    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->enableMMIO();

    //     s.str("");
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO QIO read:");
    //     for(size_t i = 0; i < sizeof(buf); i += 4)
    //     {
    //         uint32_t word = *reinterpret_cast<uint32_t*>(this->mm_axi_flash->getVirt(i));

    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 0) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 8) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 16) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 24) & 0xFF) << " ";

    //         if(i % 16 == 12)
    //         {
    //             DLOGF(SOAPY_SDR_DEBUG, "%s", s.str().c_str());
    //             s.str("");
    //         }
    //     }

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->disableMMIO();

    //     stats = this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->getMMIOStats();

    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO read request count: %lu", stats.rd_req_cnt);
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO continuous read request count: %lu (%.3f %%)", stats.cont_rd_req_cnt, (double)stats.cont_rd_req_cnt / (double)stats.rd_req_cnt * 100.0);

    //     // MMIO QOUT read
    //     mmio.rd_instr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.rd_instr = SPI_FLASH_CMD_READ_FAST_QOUT;
    //     mmio.addr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.addr_bytes = 3;
    //     mmio.mode_bits = 0xF0;
    //     mmio.mode_bits_en = false;
    //     mmio.cont_read_en = false;
    //     mmio.dummy_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.dummy_bytes = 1;
    //     mmio.data_io_mode = AXISPI::IOMode::QUAD;
    //     mmio.cs_high_wait = 0;
    //     mmio.cs_low_wait = 0;
    //     mmio.cs_mask = AXI_QUAD_SPI_MM0_FLASH_SS;

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->configMMIOMode(mmio);
    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->enableMMIO();

    //     s.str("");
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO QOUT read:");
    //     for(size_t i = 0; i < sizeof(buf); i += 4)
    //     {
    //         uint32_t word = *reinterpret_cast<uint32_t*>(this->mm_axi_flash->getVirt(i));

    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 0) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 8) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 16) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 24) & 0xFF) << " ";

    //         if(i % 16 == 12)
    //         {
    //             DLOGF(SOAPY_SDR_DEBUG, "%s", s.str().c_str());
    //             s.str("");
    //         }
    //     }

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->disableMMIO();

    //     stats = this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->getMMIOStats();

    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO read request count: %lu", stats.rd_req_cnt);
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO continuous read request count: %lu (%.3f %%)", stats.cont_rd_req_cnt, (double)stats.cont_rd_req_cnt / (double)stats.rd_req_cnt * 100.0);

    //     // MMIO DIO read
    //     mmio.rd_instr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.rd_instr = SPI_FLASH_CMD_READ_FAST_DIO;
    //     mmio.addr_io_mode = AXISPI::IOMode::DUAL;
    //     mmio.addr_bytes = 3;
    //     mmio.mode_bits = 0xF0;
    //     mmio.mode_bits_en = true;
    //     mmio.cont_read_en = false;
    //     mmio.dummy_io_mode = AXISPI::IOMode::QUAD;
    //     mmio.dummy_bytes = 0;
    //     mmio.data_io_mode = AXISPI::IOMode::DUAL;
    //     mmio.cs_high_wait = 0;
    //     mmio.cs_low_wait = 0;
    //     mmio.cs_mask = AXI_QUAD_SPI_MM0_FLASH_SS;

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->configMMIOMode(mmio);
    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->enableMMIO();

    //     s.str("");
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO DIO read:");
    //     for(size_t i = 0; i < sizeof(buf); i += 4)
    //     {
    //         uint32_t word = *reinterpret_cast<uint32_t*>(this->mm_axi_flash->getVirt(i));

    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 0) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 8) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 16) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 24) & 0xFF) << " ";

    //         if(i % 16 == 12)
    //         {
    //             DLOGF(SOAPY_SDR_DEBUG, "%s", s.str().c_str());
    //             s.str("");
    //         }
    //     }

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->disableMMIO();

    //     stats = this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->getMMIOStats();

    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO read request count: %lu", stats.rd_req_cnt);
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO continuous read request count: %lu (%.3f %%)", stats.cont_rd_req_cnt, (double)stats.cont_rd_req_cnt / (double)stats.rd_req_cnt * 100.0);

    //     // MMIO DOUT read
    //     mmio.rd_instr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.rd_instr = SPI_FLASH_CMD_READ_FAST_DOUT;
    //     mmio.addr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.addr_bytes = 3;
    //     mmio.mode_bits = 0xF0;
    //     mmio.mode_bits_en = false;
    //     mmio.cont_read_en = false;
    //     mmio.dummy_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.dummy_bytes = 1;
    //     mmio.data_io_mode = AXISPI::IOMode::DUAL;
    //     mmio.cs_high_wait = 0;
    //     mmio.cs_low_wait = 0;
    //     mmio.cs_mask = AXI_QUAD_SPI_MM0_FLASH_SS;

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->configMMIOMode(mmio);
    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->enableMMIO();

    //     s.str("");
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO DOUT read:");
    //     for(size_t i = 0; i < sizeof(buf); i += 4)
    //     {
    //         uint32_t word = *reinterpret_cast<uint32_t*>(this->mm_axi_flash->getVirt(i));

    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 0) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 8) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 16) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 24) & 0xFF) << " ";

    //         if(i % 16 == 12)
    //         {
    //             DLOGF(SOAPY_SDR_DEBUG, "%s", s.str().c_str());
    //             s.str("");
    //         }
    //     }

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->disableMMIO();

    //     stats = this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->getMMIOStats();

    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO read request count: %lu", stats.rd_req_cnt);
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO continuous read request count: %lu (%.3f %%)", stats.cont_rd_req_cnt, (double)stats.cont_rd_req_cnt / (double)stats.rd_req_cnt * 100.0);

    //     // MMIO read
    //     mmio.rd_instr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.rd_instr = SPI_FLASH_CMD_READ_FAST;
    //     mmio.addr_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.addr_bytes = 3;
    //     mmio.mode_bits = 0xF0;
    //     mmio.mode_bits_en = false;
    //     mmio.cont_read_en = false;
    //     mmio.dummy_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.dummy_bytes = 1;
    //     mmio.data_io_mode = AXISPI::IOMode::SINGLE;
    //     mmio.cs_high_wait = 0;
    //     mmio.cs_low_wait = 0;
    //     mmio.cs_mask = AXI_QUAD_SPI_MM0_FLASH_SS;

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->configMMIOMode(mmio);
    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->enableMMIO();

    //     s.str("");
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO read:");
    //     for(size_t i = 0; i < sizeof(buf); i += 4)
    //     {
    //         uint32_t word = *reinterpret_cast<uint32_t*>(this->mm_axi_flash->getVirt(i));

    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 0) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 8) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 16) & 0xFF) << " ";
    //         s << std::hex << std::setw(2) << std::setfill('0') << (size_t)((word >> 24) & 0xFF) << " ";

    //         if(i % 16 == 12)
    //         {
    //             DLOGF(SOAPY_SDR_DEBUG, "%s", s.str().c_str());
    //             s.str("");
    //         }
    //     }

    //     this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->disableMMIO();

    //     stats = this->axi_spi[AXI_QUAD_SPI_MM0_FLASH_INST]->getMMIOStats();

    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO read request count: %lu", stats.rd_req_cnt);
    //     DLOGF(SOAPY_SDR_DEBUG, "SPI Flash MMIO continuous read request count: %lu (%.3f %%)", stats.cont_rd_req_cnt, (double)stats.cont_rd_req_cnt / (double)stats.rd_req_cnt * 100.0);
    // }
    //////////////////////// SPI FLASH TEST

    this->clk_mngr = new Si5351(
        {
            .controller = this->axi_iic[AXI_IIC_SYS_INST],
            .addr = SI5351_I2C_ADDR,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_SYS_INST],
            .gpio = AXI_GPIO_CLK_MNGR_OEn_BIT,
            .invert = false,
        }/*,
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::CLK_MNGR_IRQn,
        }*/
    );

    DLOGF(SOAPY_SDR_DEBUG, "Si5351 Revision: %hhu", this->clk_mngr->getRevisionID());

    this->mmw_synth = new IDT8V97003(
        {
            .controller = this->axi_spi[AXI_SPI_SYNTH_INST],
            .ss_mask = AXI_SPI1_SYNTH_SS,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_SYNTH_INST],
            .gpio = AXI_GPIO_SYNTH_CE_BIT,
            .invert = false,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_SYNTH_INST],
            .gpio = AXI_GPIO_SYNTH_MUTE_BIT,
            .invert = false,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_SYNTH_INST],
            .gpio = AXI_GPIO_SYNTH_SYNC_BIT,
            .invert = false,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_SYNTH_INST],
            .gpio = AXI_GPIO_SYNTH_LD_BIT,
            .invert = false,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_SYNTH_INST],
            .gpio = AXI_GPIO_SYNTH_RESETn_BIT,
            .invert = false,
        }
    );

    this->mmw_synth->powerDown();

    DLOGF(SOAPY_SDR_DEBUG, "8V97003 Revision: %hhu", this->mmw_synth->getChipVersion());
    DLOGF(SOAPY_SDR_DEBUG, "8V97003 Option: %hhu", this->mmw_synth->getChipOption());

    this->rf_phy = new AD9361(
        {
            .controller = this->axi_spi[AXI_SPI_TRX_INST],
            .ss_mask = AXI_SPI0_TRX_SS,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_TRX_INST],
            .gpio = AXI_GPIO_TRX_RESETn_BIT,
            .invert = false,
        },
        {
            .controller = this->axi_gpio[AXI_GPIO_TRX_INST],
            .gpio = AXI_GPIO_TRX_SYNC_IN_BIT,
            .invert = false,
        }
    );

    DLOGF(SOAPY_SDR_DEBUG, "AD9361 Revision: %hhu", this->rf_phy->getChipRevision());
}
void SoapyIcyRadio::deinitPeripheralsPreClocks()
{
    if(this->rf_phy != nullptr)
    {
        delete this->rf_phy;

        this->rf_phy = nullptr;
    }

    if(this->mmw_synth != nullptr)
    {
        delete this->mmw_synth;

        this->mmw_synth = nullptr;
    }

    if(this->clk_mngr != nullptr)
    {
        delete this->clk_mngr;

        this->clk_mngr = nullptr;
    }

    if(this->spi_flash != nullptr)
    {
        delete this->spi_flash;

        this->spi_flash = nullptr;
    }

    if(this->vin_reg != nullptr)
    {
        delete this->vin_reg;

        this->vin_reg = nullptr;
    }

    if(this->pmc != nullptr)
    {
        delete this->pmc;

        this->pmc = nullptr;
    }

    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_PM_I2C_EN_BIT, AXIGPIO::Value::LOW);
}

void SoapyIcyRadio::initPeripheralsPostClocks()
{
    // Init I2S

    // Init RF Phy
    this->rf_phy->init();
    this->rf_phy->loadFIRCoefficients(AD9361::FIRDest::FIR_RX1_RX2, rf_phy_fir_128_dec4, ARRAY_SIZE(rf_phy_fir_128_dec4), 4, -6);
    this->rf_phy->loadFIRCoefficients(AD9361::FIRDest::FIR_TX1_TX2, rf_phy_fir_128_dec4, ARRAY_SIZE(rf_phy_fir_128_dec4), 4, 0);

    // Init RF Phy HDL block
    this->axi_ad9361->init(this->rf_phy);

    {
        uint32_t ver = this->axi_ad9361->getIPVersion();

        DLOGF(SOAPY_SDR_TRACE, "AXI AD9361:");
        DLOGF(SOAPY_SDR_TRACE, "  IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
        DLOGF(SOAPY_SDR_TRACE, "  Channel count: %u", this->axi_ad9361->getChannelCount());

        ver = this->axi_ad9361->adc->getIPVersion();

        DLOGF(SOAPY_SDR_TRACE, "  ADC:");
        DLOGF(SOAPY_SDR_TRACE, "    IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
        DLOGF(SOAPY_SDR_TRACE, "    Interface clock: %llu Hz", this->axi_ad9361->adc->getInterfaceClockFrequency(AXI_ACLK_FREQ));
        DLOGF(SOAPY_SDR_TRACE, "    Sampling frequency: %llu Hz", this->axi_ad9361->adc->getSamplingFrequency(AXI_ACLK_FREQ));

        ver = this->axi_ad9361->dac->getIPVersion();

        DLOGF(SOAPY_SDR_TRACE, "  DAC:");
        DLOGF(SOAPY_SDR_TRACE, "    IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
        DLOGF(SOAPY_SDR_TRACE, "    Interface clock: %llu Hz", this->axi_ad9361->dac->getInterfaceClockFrequency(AXI_ACLK_FREQ));
        DLOGF(SOAPY_SDR_TRACE, "    Sampling frequency: %llu Hz", this->axi_ad9361->dac->getSamplingFrequency(AXI_ACLK_FREQ));
    }

    this->axi_ad9361->adc->setDataSource(AXIAD9361::ADC::Channel::RX1_I, AXIAD9361::ADC::DataSource::DATA_SRC_INPUT_DATA);
    this->axi_ad9361->adc->setDataSource(AXIAD9361::ADC::Channel::RX1_Q, AXIAD9361::ADC::DataSource::DATA_SRC_INPUT_DATA);
    this->axi_ad9361->adc->setDataSource(AXIAD9361::ADC::Channel::RX2_I, AXIAD9361::ADC::DataSource::DATA_SRC_INPUT_DATA);
    this->axi_ad9361->adc->setDataSource(AXIAD9361::ADC::Channel::RX2_Q, AXIAD9361::ADC::DataSource::DATA_SRC_INPUT_DATA);

    this->axi_ad9361->dac->setDataSource(AXIAD9361::DAC::Channel::TX1_I, AXIAD9361::DAC::DataSource::DATA_SRC_DMA);
    this->axi_ad9361->dac->setDataSource(AXIAD9361::DAC::Channel::TX1_Q, AXIAD9361::DAC::DataSource::DATA_SRC_DMA);
    this->axi_ad9361->dac->setDataSource(AXIAD9361::DAC::Channel::TX2_I, AXIAD9361::DAC::DataSource::DATA_SRC_DMA);
    this->axi_ad9361->dac->setDataSource(AXIAD9361::DAC::Channel::TX2_Q, AXIAD9361::DAC::DataSource::DATA_SRC_DMA);

    // DLOGF(SOAPY_SDR_DEBUG, "Doing RF Phy digital interface tuning...");

    // auto intf_delays = this->axi_ad9361->tuneInterfaceTiming(AXIAD9361::InterfaceTuneFlags::DO_CHIP_RX | AXIAD9361::InterfaceTuneFlags::DO_CHIP_TX);

    // DLOGF(SOAPY_SDR_DEBUG, "RF Phy interface delays:");

    // for(auto d : intf_delays)
    //     DLOGF(SOAPY_SDR_DEBUG, "  %s = %d", d.first.c_str(), d.second);

    // Init RF Timestamping
    this->axi_rf_tstamp->init(
        {
            .controller = this->axi_irq_ctrl,
            .irq = AXIIRQCtrl::IRQNumber::AXI_RF_TSTAMP,
        }
    );

    {
        uint32_t ver = this->axi_rf_tstamp->getIPVersion();

        DLOGF(SOAPY_SDR_TRACE, "AXI RF Timestamping:");
        DLOGF(SOAPY_SDR_TRACE, "  IP Version: v%u.%u.%u", AXI_CORE_VERSION_MAJOR(ver), AXI_CORE_VERSION_MINOR(ver), AXI_CORE_VERSION_PATCH(ver));
    }

    this->axi_rf_tstamp->disableClockSyncBypass(); // Disable bypass since we are using both channels
    this->axi_rf_tstamp->triggerClockResync(true, 100); // Wait for sync both channels, 100 ms timeout

    this->axi_rf_tstamp->enableCounter();

    // Probe expansion cards
    // Enable mmWave Synth if present
    this->mmw_synth->powerUp();
    this->mmw_synth->init();
    this->mmw_synth->mute();

    this->mmw_synth->enableLockDetect();
    this->mmw_synth->enableAutoRecal();
    this->mmw_synth->setLockDetectPinMode(IDT8V97003::LDPinMode::LD_MODE_LD);
    this->mmw_synth->setLockDetectPrecision(IDT8V97003::LDPrecision::LD_PREC_1p0ns);

    this->mmw_synth->configReferenceInput(this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_SYNTH_REF_CLK), false);
    this->mmw_synth->configPFD(250000000UL, IDT8V97003::PFDPulseWidth::PFD_PW_260ps);

    this->mmw_synth->setLoopFilter(
        {
            .rs = 20,
            .cs = 1e-6,
            .cp = 10e-9,
            .r3 = 62,
            .c3 = 2.2e-9
        }
    );
    this->mmw_synth->setTargetLoopBandwidth(100e3); // 100 kHz

    this->mmw_synth->setChargePumpPositiveCurrent(5e-3); // 5.0 mA
    this->mmw_synth->setChargePumpNegativeCurrent(5e-3); // 5.0 mA
    this->mmw_synth->setChargePumpBleederCurrent(0.0); // 0.0 mA

    this->mmw_synth->setRFOutputPower(IDT8V97003::RFOutput::RFOUT_A, 12);
    this->mmw_synth->setRFOutputPower(IDT8V97003::RFOutput::RFOUT_B, 12);

    // this->mmw_synth->setFrequency(6000000000ULL); // 8 GHz

    {
        DLOGF(SOAPY_SDR_DEBUG, "mmWave Synth:");
        DLOGF(SOAPY_SDR_TRACE, "  Reference Input: %.6f MHz", this->mmw_synth->getReferenceFrequency() * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "  Reference Doubler Input: %.6f MHz", this->mmw_synth->getReferenceDoublerInputFrequency() * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "  Reference Doubler Output: %.6f MHz", this->mmw_synth->getReferenceDoublerOutputFrequency() * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "  Reference Multiplier Input: %.6f MHz", this->mmw_synth->getReferenceMultiplierInputFrequency() * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "  Reference Multiplier Output: %.6f MHz", this->mmw_synth->getReferenceMultiplierOutputFrequency() * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "  Reference R-Divider Input: %.6f MHz", this->mmw_synth->getReferenceDividerInputFrequency() * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "  Reference R-Divider Output: %.6f MHz", this->mmw_synth->getReferenceDividerOutputFrequency() * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "  PFD Frequency: %.6f MHz", this->mmw_synth->getPFDFrequency() * 1e-6);

        IDT8V97003::LoopFilter lf = this->mmw_synth->getLoopFilter();

        DLOGF(SOAPY_SDR_TRACE, "  Loop Filter:");
        DLOGF(SOAPY_SDR_TRACE, "    Rs: %.3e Ohm", lf.rs);
        DLOGF(SOAPY_SDR_TRACE, "    Cs: %.3e F", lf.cs);
        DLOGF(SOAPY_SDR_TRACE, "    Cp: %.3e F", lf.cp);
        DLOGF(SOAPY_SDR_TRACE, "    R3: %.3e Ohm", lf.r3);
        DLOGF(SOAPY_SDR_TRACE, "    C3: %.3e F", lf.c3);

        DLOGF(SOAPY_SDR_DEBUG, "  Charge Pump PMOS Current: %.3f mA", this->mmw_synth->getChargePumpPositiveCurrent() * 1e3);
        DLOGF(SOAPY_SDR_DEBUG, "  Charge Pump NMOS Current: %.3f mA", this->mmw_synth->getChargePumpNegativeCurrent() * 1e3);
        DLOGF(SOAPY_SDR_DEBUG, "  Charge Pump Bleeder Current: %.3f uA", this->mmw_synth->getChargePumpBleederCurrent() * 1e6);

        IDT8V97003::LoopFrequencyResponse lfr = this->mmw_synth->getLoopFrequencyResponse();

        DLOGF(SOAPY_SDR_TRACE, "  Loop Frequency Response:");
        DLOGF(SOAPY_SDR_TRACE, "    Zero: %.3f kHz", lfr.fz * 1e-3);
        DLOGF(SOAPY_SDR_TRACE, "    Bandwidth: %.3f kHz", lfr.fc * 1e-3);
        DLOGF(SOAPY_SDR_TRACE, "    1st Pole: %.3f kHz", lfr.fp * 1e-3);
        DLOGF(SOAPY_SDR_TRACE, "    2nd Pole: %.3f kHz", lfr.fp2 * 1e-3);
        DLOGF(SOAPY_SDR_TRACE, "    Phase Margin: %.3f deg", lfr.phase_margin);
        DLOGF(SOAPY_SDR_TRACE, "  Target Loop Bandwidth: %.3f kHz", this->mmw_synth->getTargetLoopBandwidth() * 1e-3);

        // DLOGF(SOAPY_SDR_DEBUG, "  VCO Frequency: %.6f MHz", this->mmw_synth->getVCOFrequency() * 1e-6);
        // DLOGF(SOAPY_SDR_DEBUG, "  Output Frequency: %.6f MHz", this->mmw_synth->getFrequency() * 1e-6);

        // double dist = 0;
        // bool frac = this->mmw_synth->isFeedbackDividerFractional(dist);

        // if(frac)
        //     DLOGF(SOAPY_SDR_DEBUG, "  Distance to integer boundary: %.6f %%", dist * 100);
        // else
        //     DLOGF(SOAPY_SDR_DEBUG, "  PLL in integer mode");
    }

    this->mmw_synth->powerDown();
}
void SoapyIcyRadio::deinitPeripheralsPostClocks()
{
    this->mmw_synth->mute();
    this->mmw_synth->powerDown();

    if(this->exp_card != nullptr)
    {
        delete this->exp_card;

        this->exp_card = nullptr;
    }

    this->axi_ad9361->deinit();

    // Wait for the RF logic reset to assert, it is commanded when the AXI core de-initializes
    uint32_t timeout = 2000; // 2 seconds
    while(--timeout && this->axi_gpio[AXI_GPIO_TRX_INST]->getValue(AXI_GPIO_RST_AD9361_61M44_PERI_ARESETn_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    this->rf_phy->setENSMState(ENSM_STATE_SLEEP, false); //TODO: this->rf_phy->deinit();
}

void SoapyIcyRadio::initClocks()
{
    DLOGF(SOAPY_SDR_DEBUG, "Setup clock tree...");

    this->clk_mngr->init();

    // Inputs
    DLOGF(SOAPY_SDR_DEBUG, "Clock Inputs:");

    this->clk_mngr->configXTAL(26000000UL, Si5351::XTALCapacitance::C_10pF);

    uint32_t timeout = 500;
    while(--timeout && !this->clk_mngr->isXTALDetected())
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->clk_mngr->isXTALDetected())
    {
        if(!this->config.use_clkin)
            throw std::runtime_error("Could not detect a valid clock at the XTAL input, aborting!");
        else
            DLOGF(SOAPY_SDR_WARNING, "  Could not detect a valid clock at the XTAL input");
    }

    DLOGF(SOAPY_SDR_DEBUG, "  XTAL Clock: %.6f MHz", this->clk_mngr->getXTALFrequency() * 1e-6);

    if(this->config.use_clkin)
    {
        this->clk_mngr->configCLKIN(this->config.clkin_freq);

        timeout = 500;
        while(--timeout && !this->clk_mngr->isCLKINDetected())
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        if(!this->clk_mngr->isCLKINDetected())
        {
            this->config.use_clkin = false;

            if(!this->clk_mngr->isXTALDetected())
                throw std::runtime_error("Could not detect any valid clock at the CLKIN or XTAL input, aborting!");
            else
                DLOGF(SOAPY_SDR_WARNING, "  Could not detect a valid clock at the CLKIN input, falling back to XTAL");
        }
        else
        {
            DLOGF(SOAPY_SDR_DEBUG, "  CLKIN Clock: %.6f MHz", this->clk_mngr->getCLKINFrequency() * 1e-6);
            DLOGF(SOAPY_SDR_DEBUG, "  CLKIN Divided Clock: %.6f MHz", this->clk_mngr->getDividedCLKINFrequency() * 1e-6);
        }
    }

    // PLLs
    DLOGF(SOAPY_SDR_DEBUG, "Clock PLLs:");

    //// PLLA
    DLOGF(SOAPY_SDR_DEBUG, "  PLL A:");

    this->clk_mngr->configPLL(Si5351::PLL::PLLA, 650000000UL, this->config.use_clkin ? Si5351::PLLSource::PLL_SRC_CLKIN : Si5351::PLLSource::PLL_SRC_XTAL, false);

    timeout = 500;
    while(--timeout && !this->clk_mngr->isPLLLocked(Si5351::PLL::PLLA))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->clk_mngr->isPLLLocked(Si5351::PLL::PLLA))
        throw std::runtime_error("PLLA did not achieve lock, aborting!");

    {
        DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getPLLSourceFrequency(Si5351::PLL::PLLA) * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getPLLFrequency(Si5351::PLL::PLLA) * 1e-6);

        double dist = 0;
        bool frac = this->clk_mngr->isPLLDividerFractional(Si5351::PLL::PLLA, dist);

        if(frac)
            DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
        else
            DLOGF(SOAPY_SDR_DEBUG, "    PLL in integer mode");
    }

    //// PLLB
    DLOGF(SOAPY_SDR_DEBUG, "  PLL B:");

    this->clk_mngr->configPLL(Si5351::PLL::PLLB, 780000000UL, this->config.use_clkin ? Si5351::PLLSource::PLL_SRC_CLKIN : Si5351::PLLSource::PLL_SRC_XTAL, false);

    timeout = 500;
    while(--timeout && !this->clk_mngr->isPLLLocked(Si5351::PLL::PLLB))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->clk_mngr->isPLLLocked(Si5351::PLL::PLLB))
        throw std::runtime_error("PLLB did not achieve lock, aborting!");

    {
        DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getPLLSourceFrequency(Si5351::PLL::PLLB) * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getPLLFrequency(Si5351::PLL::PLLB) * 1e-6);

        double dist = 0;
        bool frac = this->clk_mngr->isPLLDividerFractional(Si5351::PLL::PLLB, dist);

        if(frac)
            DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
        else
            DLOGF(SOAPY_SDR_DEBUG, "    PLL in integer mode");
    }

    // Clocks
    DLOGF(SOAPY_SDR_DEBUG, "Clock Outputs:");
    //// FPGA Clock #0
    DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (FPGA_CLK0):", Si5351::ClockOutput::CLK_FPGA_CLK0);

    this->clk_mngr->configClock(Si5351::ClockOutput::CLK_FPGA_CLK0, 50000000UL, 0.f, Si5351::PLL::PLLA, false);
    this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_FPGA_CLK0, Si5351::ClockOutputDisableState::LOW);
    this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_FPGA_CLK0, Si5351::ClockOutputDriveCurrent::I_4mA);
    this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_FPGA_CLK0, true); // Controlled by OE pin

    this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_FPGA_CLK0);
    this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_FPGA_CLK0);

    {
        DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_FPGA_CLK0) * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_FPGA_CLK0) * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_FPGA_CLK0) * 1e-6);

        double dist = 0;
        bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_FPGA_CLK0, dist);

        if(frac)
            DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
        else
            DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
    }

    //// FPGA Clock #1
    DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (FPGA_CLK1):", Si5351::ClockOutput::CLK_FPGA_CLK1);

    this->clk_mngr->configClock(Si5351::ClockOutput::CLK_FPGA_CLK1, 49152000UL, 0.f, Si5351::PLL::PLLA, false);
    this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_FPGA_CLK1, Si5351::ClockOutputDisableState::LOW);
    this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_FPGA_CLK1, Si5351::ClockOutputDriveCurrent::I_4mA);
    this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_FPGA_CLK1, true); // Controlled by OE pin

    this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_FPGA_CLK1);
    this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_FPGA_CLK1);

    {
        DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_FPGA_CLK1) * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_FPGA_CLK1) * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_FPGA_CLK1) * 1e-6);

        double dist = 0;
        bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_FPGA_CLK1, dist);

        if(frac)
            DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
        else
            DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
    }

    //// FPGA Clock #2
    // DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (FPGA_CLK2):", Si5351::ClockOutput::CLK_FPGA_CLK2);

    // this->clk_mngr->configClock(Si5351::ClockOutput::CLK_FPGA_CLK2, FREQ, 0.f, Si5351::PLL::PLLA, false);
    // this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_FPGA_CLK2, Si5351::ClockOutputDisableState::LOW);
    // this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_FPGA_CLK2, Si5351::ClockOutputDriveCurrent::I_4mA);
    // this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_FPGA_CLK2, true); // Controlled by OE pin

    // this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_FPGA_CLK2);
    // this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_FPGA_CLK2);

    // {
    //     DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_FPGA_CLK2) * 1e-6);
    //     DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_FPGA_CLK2) * 1e-6);
    //     DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_FPGA_CLK2) * 1e-6);

    //     double dist = 0;
    //     bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_FPGA_CLK2, dist);

    //     if(frac)
    //         DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
    //     else
    //         DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
    // }

    //// FPGA Clock #3
    // DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (FPGA_CLK3):", Si5351::ClockOutput::CLK_FPGA_CLK3);

    // this->clk_mngr->configClock(Si5351::ClockOutput::CLK_FPGA_CLK3, FREQ, 0.f, Si5351::PLL::PLLA, false);
    // this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_FPGA_CLK3, Si5351::ClockOutputDisableState::LOW);
    // this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_FPGA_CLK3, Si5351::ClockOutputDriveCurrent::I_4mA);
    // this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_FPGA_CLK3, true); // Controlled by OE pin

    // this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_FPGA_CLK3);
    // this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_FPGA_CLK3);

    // {
    //     DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_FPGA_CLK3) * 1e-6);
    //     DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_FPGA_CLK3) * 1e-6);
    //     DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_FPGA_CLK3) * 1e-6);

    //     double dist = 0;
    //     bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_FPGA_CLK3, dist);

    //     if(frac)
    //         DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
    //     else
    //         DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
    // }

    //// Transceiver Reference clock
    DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (TRX_REF_CLK):", Si5351::ClockOutput::CLK_TRX_REF_CLK);

    this->clk_mngr->configClock(Si5351::ClockOutput::CLK_TRX_REF_CLK, 39000000UL, 0.f, Si5351::PLL::PLLB, false);
    this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_TRX_REF_CLK, Si5351::ClockOutputDisableState::LOW);
    this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_TRX_REF_CLK, Si5351::ClockOutputDriveCurrent::I_4mA);
    this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_TRX_REF_CLK, true); // Controlled by OE pin

    this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_TRX_REF_CLK);
    this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_TRX_REF_CLK);

    {
        DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_TRX_REF_CLK) * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_TRX_REF_CLK) * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_TRX_REF_CLK) * 1e-6);

        double dist = 0;
        bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_TRX_REF_CLK, dist);

        if(frac)
            DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
        else
            DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
    }

    //// mmWave Synthesizer Reference clock
    DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (SYNTH_REF_CLK):", Si5351::ClockOutput::CLK_SYNTH_REF_CLK);

    this->clk_mngr->configClock(Si5351::ClockOutput::CLK_SYNTH_REF_CLK, 25000000UL, 0.f, Si5351::PLL::PLLA, false);
    this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_SYNTH_REF_CLK, Si5351::ClockOutputDisableState::LOW);
    this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_SYNTH_REF_CLK, Si5351::ClockOutputDriveCurrent::I_8mA);
    this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_SYNTH_REF_CLK, true); // Controlled by OE pin

    this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_SYNTH_REF_CLK);
    this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_SYNTH_REF_CLK);

    {
        DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_SYNTH_REF_CLK) * 1e-6);
        DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_SYNTH_REF_CLK) * 1e-6);
        DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_SYNTH_REF_CLK) * 1e-6);

        double dist = 0;
        bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_SYNTH_REF_CLK, dist);

        if(frac)
            DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
        else
            DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
    }

    //// External clock output (on frontend interface pin 2_3)
    // DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (EXT_CLK_2_3):", Si5351::ClockOutput::CLK_EXT_CLK_2_3);

    // this->clk_mngr->configClock(Si5351::ClockOutput::CLK_EXT_CLK_2_3, 10000000UL, 0.f, Si5351::PLL::PLL_AUTO, false);
    // this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_EXT_CLK_2_3, Si5351::ClockOutputDisableState::LOW);
    // this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_EXT_CLK_2_3, Si5351::ClockOutputDriveCurrent::I_8mA);
    // this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_EXT_CLK_2_3, true); // Controlled by OE pin

    // this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_EXT_CLK_2_3);
    // this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_EXT_CLK_2_3);

    // {
    //     DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_EXT_CLK_2_3) * 1e-6);
    //     DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_EXT_CLK_2_3) * 1e-6);
    //     DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_EXT_CLK_2_3) * 1e-6);

    //     double dist = 0;
    //     bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_EXT_CLK_2_3, dist);

    //     if(frac)
    //         DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
    //     else
    //         DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
    // }

    //// External clock output (on u.FL connector)
    if(this->config.enable_clkout)
    {
        DLOGF(SOAPY_SDR_DEBUG, "  Clock #%hhu (EXT_CLK_OUT):", Si5351::ClockOutput::CLK_EXT_CLK_OUT);

        this->clk_mngr->configClock(Si5351::ClockOutput::CLK_EXT_CLK_OUT, this->config.clkout_freq, 0.f, Si5351::PLL::PLL_AUTO, true);
        this->clk_mngr->setClockDisableState(Si5351::ClockOutput::CLK_EXT_CLK_OUT, Si5351::ClockOutputDisableState::LOW);
        this->clk_mngr->setClockDriveCurrent(Si5351::ClockOutput::CLK_EXT_CLK_OUT, Si5351::ClockOutputDriveCurrent::I_8mA);
        this->clk_mngr->setClockOutputEnableMode(Si5351::ClockOutput::CLK_EXT_CLK_OUT, false); // Controlled via software enable only

        this->clk_mngr->powerUpClock(Si5351::ClockOutput::CLK_EXT_CLK_OUT);
        this->clk_mngr->enableClock(Si5351::ClockOutput::CLK_EXT_CLK_OUT);

        {
            DLOGF(SOAPY_SDR_TRACE, "    Source Clock: %.6f MHz", this->clk_mngr->getMultiSynthSourceFrequency(Si5351::MultiSynth::MS_EXT_CLK_OUT) * 1e-6);
            DLOGF(SOAPY_SDR_TRACE, "    MS Output Clock: %.6f MHz", this->clk_mngr->getMultiSynthFrequency(Si5351::MultiSynth::MS_EXT_CLK_OUT) * 1e-6);
            DLOGF(SOAPY_SDR_DEBUG, "    Output Clock: %.6f MHz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_EXT_CLK_OUT) * 1e-6);

            double dist = 0;
            bool frac = this->clk_mngr->isMultiSynthDividerFractional(Si5351::MultiSynth::MS_EXT_CLK_OUT, dist);

            if(frac)
                DLOGF(SOAPY_SDR_DEBUG, "    Distance to integer boundary: %.6f %%", dist * 100);
            else
                DLOGF(SOAPY_SDR_DEBUG, "    MS in integer mode");
        }

        DLOGF(SOAPY_SDR_INFO, "Achieved external clock output frequency is %.6f Hz", this->clk_mngr->getClockFrequency(Si5351::ClockOutput::CLK_EXT_CLK_OUT));
    }

    // Wait and global enable
    DLOGF(SOAPY_SDR_DEBUG, "Waiting for all clocks to stabilize...");
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    this->clk_mngr->globalOutputEnable();

    DLOGF(SOAPY_SDR_DEBUG, "Clock manager global output enabled: %s", this->clk_mngr->isGlobalOutputEnabled() ? "yes" : "no");

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Check clk_wiz_0 lock
    timeout = 500;
    while(--timeout && !this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_CLK_WIZ0_LOCKED_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_CLK_WIZ0_LOCKED_BIT))
        throw std::runtime_error("FPGA clk_wiz_0 MMCM did not achieve lock (possible issue with FPGA_CLK0), aborting!");
    else
        DLOGF(SOAPY_SDR_DEBUG, "FPGA clk_wiz_0 MMCM locked!");

    // De-assert DDR3 core reset
    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_RST_CLK_WIZ0_250M_AUX_RESET_IN_BIT, AXIGPIO::Value::LOW);

    // Check DDR3 controller MMCM lock
    timeout = 500;
    while(--timeout && !this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_MIG_MMCM_LOCKED_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_MIG_MMCM_LOCKED_BIT))
        throw std::runtime_error("FPGA DDR3 MMCM (mig_7series_0) did not achieve lock, aborting!");
    else
        DLOGF(SOAPY_SDR_DEBUG, "FPGA DDR3 MMCM locked!");

    // De-assert DDR3 AXI interface reset
    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_RST_MIG_166M_AUX_RESET_IN_BIT, AXIGPIO::Value::LOW);

    // Check DDR3 AXI interface reset release
    timeout = 500;
    while(--timeout && !this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_MIG_166M_PERI_ARESETn_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_MIG_166M_PERI_ARESETn_BIT))
        throw std::runtime_error("FPGA DDR3 AXI interface did not come out of reset, aborting!");
    else
        DLOGF(SOAPY_SDR_DEBUG, "FPGA DDR3 AXI interface reset released!");

    // De-assert I2S Core reset
    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_RST_FPGA_CLK1_49M152_AUX_RESET_IN_BIT, AXIGPIO::Value::LOW);

    // Check I2S core reset release
    timeout = 500;
    while(--timeout && !this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_FPGA_CLK1_49M152_PERI_ARESETn_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_FPGA_CLK1_49M152_PERI_ARESETn_BIT))
        throw std::runtime_error("FPGA I2S Core did not come out of reset (possible issue with FPGA_CLK1), aborting!");
    else
        DLOGF(SOAPY_SDR_DEBUG, "FPGA I2S Core reset released!");

    // De-assert RF section logic reset (we do not check if it comes out of reset because there are other conditions that need to be met)
    this->axi_gpio[AXI_GPIO_TRX_INST]->setValue(AXI_GPIO_RST_AD9361_61M44_AUX_RESET_IN_BIT, AXIGPIO::Value::LOW);

    // Check PCIe MMCM lock
    timeout = 500;
    while(--timeout && !this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_PCIE_MMCM_LOCKED_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(!this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_PCIE_MMCM_LOCKED_BIT))
        throw std::runtime_error("FPGA PCIe MMCM (axi_pcie_0) did not achieve lock (how did we get here?), aborting!");
    else
        DLOGF(SOAPY_SDR_DEBUG, "FPGA PCIe MMCM locked!");
}
void SoapyIcyRadio::deinitClocks()
{
    // RF section clock domain reset procedure (this only works if the xcvr is supplying the clock to the RF section)
    // When this function is called in the cleanup, the RF section is already reset
    // When it is called during the init procedure, when an already initialized device is detected, it works because the xcvr is supplying the clock)
    DLOGF(SOAPY_SDR_DEBUG, "Resetting RF section...");

    this->axi_gpio[AXI_GPIO_TRX_INST]->setValue(AXI_GPIO_RST_AD9361_61M44_AUX_RESET_IN_BIT, AXIGPIO::Value::HIGH);

    uint32_t timeout = 2000; // 2 seconds
    while(--timeout && this->axi_gpio[AXI_GPIO_TRX_INST]->getValue(AXI_GPIO_RST_AD9361_61M44_PERI_ARESETn_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(this->axi_gpio[AXI_GPIO_TRX_INST]->getValue(AXI_GPIO_RST_AD9361_61M44_PERI_ARESETn_BIT))
        throw std::runtime_error("Could not reset RF section, aborting!");

    DLOGF(SOAPY_SDR_DEBUG, "RF section logic is reset!");

    // I2S clock domain reset procedure
    DLOGF(SOAPY_SDR_DEBUG, "Resetting I2S core...");

    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_RST_FPGA_CLK1_49M152_AUX_RESET_IN_BIT, AXIGPIO::Value::HIGH);

    timeout = 2000; // 2 seconds
    while(--timeout && this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_FPGA_CLK1_49M152_PERI_ARESETn_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_FPGA_CLK1_49M152_PERI_ARESETn_BIT))
        throw std::runtime_error("Could not reset I2S core, aborting!");

    DLOGF(SOAPY_SDR_DEBUG, "I2S core reset!");

    // DDR3 clock domain(s) reset procedure
    // First, reset the DDR3 AXI interface
    DLOGF(SOAPY_SDR_DEBUG, "Resetting DDR3 AXI interface...");

    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_RST_MIG_166M_AUX_RESET_IN_BIT, AXIGPIO::Value::HIGH);

    timeout = 2000; // 2 seconds
    while(--timeout && this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_MIG_166M_PERI_ARESETn_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_MIG_166M_PERI_ARESETn_BIT))
        throw std::runtime_error("Could not reset DDR3 AXI interface, aborting!");

    DLOGF(SOAPY_SDR_DEBUG, "DDR3 AXI interface reset!");

    // Then reset the DDR3 core
    DLOGF(SOAPY_SDR_DEBUG, "Resetting DDR3 core...");

    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_RST_CLK_WIZ0_250M_AUX_RESET_IN_BIT, AXIGPIO::Value::HIGH);

    timeout = 2000; // 2 seconds
    while(--timeout && this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_CLK_WIZ0_250M_PERI_ARESETn_BIT))
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

    if(this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_CLK_WIZ0_250M_PERI_ARESETn_BIT))
        throw std::runtime_error("Could not reset DDR3 core, aborting!");

    DLOGF(SOAPY_SDR_DEBUG, "DDR3 core reset, DDR3 MMCM is %s!", this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_MIG_MMCM_LOCKED_BIT) ? "locked (how?)" : "unlocked");

    // Disable clock manager output
    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_CLK_MNGR_OEn_BIT, AXIGPIO::Value::HIGH);

    DLOGF(SOAPY_SDR_DEBUG, "Clock manager global output disabled, clk_wiz_0 MMCM is %s!", this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_CLK_WIZ0_LOCKED_BIT) ? "locked (how?)" : "unlocked");
}

void SoapyIcyRadio::resetSystem()
{
    bool clk_mngr_oen = this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_CLK_MNGR_OEn_BIT);
    bool clk_wiz_0_locked = this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_CLK_WIZ0_LOCKED_BIT);
    bool ddr_resetn = this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_CLK_WIZ0_250M_PERI_ARESETn_BIT);
    bool mig_mmcm_locked = this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_MIG_MMCM_LOCKED_BIT);
    bool ddr_axi_resetn = this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_RST_MIG_166M_PERI_ARESETn_BIT);

    if(!clk_mngr_oen) // Clock manager OE enabled
    {
        DLOGF(SOAPY_SDR_TRACE, "Clock manager global output is enabled");

        if(clk_wiz_0_locked && ddr_resetn && mig_mmcm_locked && ddr_axi_resetn)
        {
            DLOGF(SOAPY_SDR_TRACE, "DDR3 is properly out of reset, looks like the system was left initialized");

            this->deinitClocks(); // Will throw if it fails
        }
        else if(!mig_mmcm_locked && ddr_axi_resetn) // AXI reset is de-asserted, but clocks are not locked, may cause lockups
        {
            throw std::runtime_error(std::string("DDR3 is not correctly out of reset, ") +
                                     std::string("looks like the system was left in an invalid state ") +
                                     std::string("(clk_mngr_oe: ") + std::string(!clk_mngr_oen ? "enabled" : "disabled") +
                                     std::string(", clk_wiz_0: ") + std::string(clk_wiz_0_locked ? "locked" : "unlocked") +
                                     std::string(", rst_clk_wiz_0_250M: ") + std::string(ddr_resetn ? "de-asserted" : "asserted") +
                                     std::string(", mig_mmcm: ") + std::string(mig_mmcm_locked ? "locked" : "unlocked") +
                                     std::string(", rst_mig_166M: ") + std::string(ddr_axi_resetn ? "de-asserted" : "asserted") +
                                     std::string("). Please perform a power cycle to properly reset the system"));
        }
    }
    else // Clock manager OE disabled
    {
        DLOGF(SOAPY_SDR_TRACE, "Clock manager global output is disabled");

        if(!clk_wiz_0_locked && !ddr_resetn && !mig_mmcm_locked && !ddr_axi_resetn)
        {
            DLOGF(SOAPY_SDR_TRACE, "DDR3 is properly reset, looks like the system was left uninitialized");
        }
        else if(!mig_mmcm_locked && ddr_axi_resetn) // AXI reset is de-asserted, but clocks are not locked, may cause lockups
        {
            throw std::runtime_error(std::string("DDR3 clocks are not locked and resets are not properly asserted, ") +
                                     std::string("looks like the system was left in an invalid state ") +
                                     std::string("(clk_mngr_oe: ") + std::string(!clk_mngr_oen ? "enabled" : "disabled") +
                                     std::string(", clk_wiz_0: ") + std::string(clk_wiz_0_locked ? "locked" : "unlocked") +
                                     std::string(", rst_clk_wiz_0_250M: ") + std::string(ddr_resetn ? "de-asserted" : "asserted") +
                                     std::string(", mig_mmcm: ") + std::string(mig_mmcm_locked ? "locked" : "unlocked") +
                                     std::string(", rst_mig_166M: ") + std::string(ddr_axi_resetn ? "de-asserted" : "asserted") +
                                     std::string("). Please perform a power cycle to properly reset the system"));
        }
    }

    DLOGF(SOAPY_SDR_DEBUG, "Initiating system reset...");

    // This will clear the two DDR3 reset bits set previously!
    this->axi_gpio[AXI_GPIO_SYS_INST]->setValue(AXI_GPIO_SYS_AUX_RESET_BIT, AXIGPIO::Value::HIGH);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Do not access anything while it's resetting, otherwise the ENTIRE SYSTEM will hang! TODO: mutex

    if(this->axi_gpio[AXI_GPIO_SYS_INST]->getValue(AXI_GPIO_SYS_AUX_RESET_BIT))
        throw std::runtime_error("System reset bit did not de-assert (how?), system reset failed");
}

void SoapyIcyRadio::DMAHandler(void *arg)
{
    if(arg == nullptr)
        return;

    SoapyIcyRadio::Stream::Channel::DMABuffer *buf = static_cast<SoapyIcyRadio::Stream::Channel::DMABuffer *>(arg);

    if(buf->device == nullptr)
    {
        DLOGF(SOAPY_SDR_WARNING, "DMAHandler: Called with unrelated transfer");

        return;
    }

    buf->device->handleDMAData(buf); // Call the internal handler
}
void SoapyIcyRadio::handleDMAData(SoapyIcyRadio::Stream::Channel::DMABuffer *buf)
{
    // Pointer has already been validated
    if(!buf->parent || !buf->parent->parent)
        return;

    std::lock_guard<std::mutex> lock(buf->parent->mutex);

    buf->idle = true;

    DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: Controller %u, DMA buffer %u, transfer %u done", buf->parent->dma->getPeripheralID(), buf->index, buf->xfer.id);

    if(buf->parent->parent->direction == SOAPY_SDR_RX)
    {
        AXIRFTStamp::Channel ts_chan = buf->parent->ts_chan;

        AXIRFTStamp::CounterLatchStatus hw_time_status = this->axi_rf_tstamp->getCounterLatchStatus(ts_chan);
        uint64_t hw_time = this->axi_rf_tstamp->getCounterLatch(ts_chan);
        uint64_t buf_time = 0;
        bool buf_time_valid = false;

        if(hw_time_status == AXIRFTStamp::CounterLatchStatus::LATCH_VALID)
        {
            #ifdef TRACE_STREAM
            uint64_t hw_time_now = this->axi_rf_tstamp->getCounter(); // Hardware time now
            #endif
            uint64_t sw_time_now = hw_time + buf->xfer.size / ICYRADIO_SAMPLE_SIZE_BYTES; // Software time now, is the start time of this buffer plus this buffer's size

            if(buf->parent->next_dma_user_buf_time_valid)
                DLOGF(SOAPY_SDR_WARNING, "handleDMAData: Channel %u RX buffer hardware timestamp %llu is valid, was %llu (delta %lld)", buf->parent->num, hw_time, buf->parent->next_dma_user_buf_time, hw_time - buf->parent->next_dma_user_buf_time);
            else
                DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: Channel %u RX buffer hardware timestamp %llu is valid, next is %llu, next hardware is %llu (delta %lld)", buf->parent->num, hw_time, sw_time_now, hw_time_now, hw_time_now - sw_time_now);

            buf_time = hw_time;
            buf_time_valid = true;

            buf->parent->next_dma_user_buf_time = sw_time_now;
            buf->parent->next_dma_user_buf_time_valid = true;
        }
        else if(buf->parent->next_dma_user_buf_time_valid)
        {
            #ifdef TRACE_STREAM
            uint64_t hw_time_now = this->axi_rf_tstamp->getCounter(); // Hardware time now
            #endif
            uint64_t sw_time_now = buf->parent->next_dma_user_buf_time + buf->xfer.size / ICYRADIO_SAMPLE_SIZE_BYTES; // Software time now, is the start time of this buffer plus this buffer's size

            DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: Channel %u RX buffer software timestamp %llu is valid, next is %llu, next hardware is %llu (delta %lld)", buf->parent->num, buf->parent->next_dma_user_buf_time, sw_time_now, hw_time_now, hw_time_now - sw_time_now);

            buf_time = buf->parent->next_dma_user_buf_time;
            buf_time_valid = true;

            buf->parent->next_dma_user_buf_time = sw_time_now;
        }
        else
        {
            DLOGF(SOAPY_SDR_WARNING, "handleDMAData: Channel %u RX buffer timestamp missing (hw status is %u, hw time is %llu)", buf->parent->num, hw_time_status, hw_time);

            buf_time = 0;
            buf_time_valid = false;
        }

        auto user_buf = buf->parent->buffers[buf->parent->next_dma_user_buf];

        if(user_buf->valid_size > 0)
        {
            DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer queue head is %u, buffer still has valid data, new data discarded", buf->parent->next_dma_user_buf);

            DLOGF(SOAPY_SDR_SSI, "O");
        }
        else if(user_buf->acquired)
        {
            DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer queue head is %u, buffer is acquired by user, new data discarded", buf->parent->next_dma_user_buf);

            DLOGF(SOAPY_SDR_SSI, "O");
        }
        else
        {
            DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer queue head is %u, buffer is free, copying data", buf->parent->next_dma_user_buf);

            std::memcpy(user_buf->addr, buf->virt, buf->xfer.size);

            user_buf->valid_size = buf->xfer.size;
            user_buf->time = buf_time;
            user_buf->time_valid = buf_time_valid;

            buf->parent->next_dma_user_buf = (buf->parent->next_dma_user_buf + 1) % buf->parent->buffers.size();
        }

        if(buf->parent->dma->idle())
        {
            // This should never happen. Even when user cannot read fast enough, the DMA controller should be able to keep up
            // The data is just discarded and never copied to the user buffer, but the DMA still runs
            // If this happens, it means the system is not able to keep up with the data rate/interrupt rate
            DLOGF(SOAPY_SDR_WARNING, "handleDMAData: DMA controller is idle, system cannot keep up!");
        }
        else
        {
            try
            {
                buf->parent->dma->submitTransfer(buf->xfer);

                buf->idle = false;

                DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: Re-submitted transfer, new ID %u", buf->xfer.id);
            }
            catch(const std::exception &e)
            {
                DLOGF(SOAPY_SDR_ERROR, "handleDMAData: Failed to re-submit transfer: %s", e.what());
            }
        }
    }
    else if(buf->parent->parent->direction == SOAPY_SDR_TX)
    {
        AXIRFTStamp::Channel ts_chan = buf->parent->ts_chan;

        auto dma_buf = buf;

        while(dma_buf)
        {
            bool submit_xfer = true;
            bool cur = (dma_buf == buf);
            auto user_buf = dma_buf->parent->buffers[dma_buf->parent->next_dma_user_buf];

            if(!user_buf->valid_size)
            {
                submit_xfer = false;

                DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer queue head is %u, buffer has no valid data, not submitting new transfer", dma_buf->parent->next_dma_user_buf);

                DLOGF(SOAPY_SDR_SSI, "U");

                if(dma_buf->parent->dma->idle())
                {
                    DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: DMA controller is idle, disabling");

                    this->axi_rf_tstamp->disableTXCounter(ts_chan);
                    this->axi_rf_tstamp->disableTX(ts_chan);

                    dma_buf->parent->dma->disable();

                    dma_buf->parent->underflow = true;
                }
            }
            else if(user_buf->acquired)
            {
                submit_xfer = false;

                DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer queue head is %u, buffer is acquired by user, not submitting new transfer", dma_buf->parent->next_dma_user_buf);

                DLOGF(SOAPY_SDR_SSI, "U");

                if(dma_buf->parent->dma->idle())
                {
                    DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: DMA controller is idle, disabling");

                    this->axi_rf_tstamp->disableTXCounter(ts_chan);
                    this->axi_rf_tstamp->disableTX(ts_chan);

                    dma_buf->parent->dma->disable();

                    dma_buf->parent->underflow = true;
                }
            }
            else if(user_buf->time_valid && (!cur || !dma_buf->parent->dma->idle()))
            {
                // If the next buffer has a time, but the DMA is not idle, we cannot submit a new transfer
                // because this would mess the timestamping up, since the DMA is working on another buffer, we cannot disable TX
                // Instead, we skip this buffer and wait for the next interrput (the one corresponding to the buffer the DMA is currently working on)
                // and then, it will be idle, and we can safely submit the new transfer
                submit_xfer = false;

                DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer queue head is %u, buffer has time but DMA is not idle, not submitting new transfer", dma_buf->parent->next_dma_user_buf);
            }

            if(submit_xfer)
            {
                DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer queue head is %u, buffer has valid data, copying data", dma_buf->parent->next_dma_user_buf);

                std::memcpy(dma_buf->virt, user_buf->addr, user_buf->valid_size);

                dma_buf->xfer.size = user_buf->valid_size;

                if(user_buf->time_valid)
                {
                    uint64_t hw_time_now = this->axi_rf_tstamp->getCounter(); // Hardware time now

                    if(user_buf->time > hw_time_now)
                    {
                        DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer time %llu is valid and not late (%llu ticks in the future), disabling TX and arming trigger", user_buf->time, user_buf->time - hw_time_now);

                        this->axi_rf_tstamp->disableTXCounter(ts_chan);
                        this->axi_rf_tstamp->waitTXCounterDisabled(ts_chan, 100);
                        this->axi_rf_tstamp->disableTX(ts_chan);
                        this->axi_rf_tstamp->setTXCounter(ts_chan, user_buf->time); // Safe because we already made sure the counter is disabled
                        this->axi_rf_tstamp->waitTXDisabled(ts_chan, 100);
                        this->axi_rf_tstamp->enableTXCounter(ts_chan);
                    }
                    else
                    {
                        DLOGF(SOAPY_SDR_WARNING, "handleDMAData: User buffer time %llu is late (%llu ticks late), not arming trigger, enabling TX now", user_buf->time, hw_time_now - user_buf->time);

                        DLOGF(SOAPY_SDR_SSI, "L");

                        dma_buf->parent->late = true;

                        this->axi_rf_tstamp->enableTX(ts_chan);
                    }
                }
                else
                {
                    DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: User buffer time is not valid");
                }

                user_buf->valid_size = 0;
                user_buf->time_valid = false;

                try
                {
                    dma_buf->parent->dma->submitTransfer(dma_buf->xfer);

                    dma_buf->idle = false;

                    if(cur)
                        DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: Re-submitted transfer, new ID %u", dma_buf->xfer.id);
                    else
                        DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: Submitted transfer ID %u", dma_buf->xfer.id);
                }
                catch(const std::exception &e)
                {
                    if(cur)
                        DLOGF(SOAPY_SDR_ERROR, "handleDMAData: Failed to re-submit transfer: %s", e.what());
                    else
                        DLOGF(SOAPY_SDR_ERROR, "handleDMAData: Failed to submit transfer: %s", e.what());
                }

                dma_buf->parent->next_dma_user_buf = (dma_buf->parent->next_dma_user_buf + 1) % dma_buf->parent->buffers.size();

                // Find a possible free DMA buffer
                SoapyIcyRadio::Stream::Channel::DMABuffer *next_dma_buf = nullptr;

                size_t i = (dma_buf->index + 1) % dma_buf->parent->dma_buffers.size();

                for(size_t n = 0; n < dma_buf->parent->dma_buffers.size(); n++)
                {
                    auto _buf = dma_buf->parent->dma_buffers[i];

                    if(_buf->idle)
                    {
                        next_dma_buf = _buf;

                        break;
                    }

                    i = (i + 1) % dma_buf->parent->dma_buffers.size();
                }

                if(next_dma_buf)
                    DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: DMA buffer %u is idle, checking if it can be submitted", next_dma_buf->index);
                else
                    DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: No idle DMA buffer found, everything done");

                dma_buf = next_dma_buf;
            }
            else
            {
                DLOGF_S(SOAPY_SDR_TRACE, "handleDMAData: Not submitting transfer, everything done");

                dma_buf = nullptr;
            }
        }
    }
}

bool SoapyIcyRadio::requiresDataPathReconfiguration(const std::vector<size_t> &rx_channels, const std::vector<size_t> &tx_channels) const
{
    uint8_t phy_channels = this->rf_phy->getChannelCount();

    DLOGF(SOAPY_SDR_DEBUG, "requiresDataPathReconfiguration: Phy channels: %u, RX channels %u, TX channels %u", phy_channels, rx_channels.size(), tx_channels.size());

    // If the Phy is configured for dual channel, no reconfiguration is needed
    if(phy_channels > 1)
        return false;

    size_t channel_count = MAX(rx_channels.size(), tx_channels.size());

    // If the Phy is configured for less channels than requested, we need to reconfigure
    if(channel_count > 1)
        return true;

    // If in single channel mode...
    if(channel_count <= 1)
    {
        DLOGF(SOAPY_SDR_DEBUG, "requiresDataPathReconfiguration: Single channel mode, current RX %u, wanted RX %u, current TX %u, wanted TX %u", this->rf_phy->pdata->rx1tx1_mode_use_rx_num - 1, rx_channels.size() > 0 ? rx_channels[0] : 0, this->rf_phy->pdata->rx1tx1_mode_use_tx_num - 1, tx_channels.size() > 0 ? tx_channels[0] : 0);

        // ...and the selected RX channel is not the one currently in use, we need to reconfigure
        if(rx_channels.size() > 0 && BIT(rx_channels[0]) != this->rf_phy->pdata->rx1tx1_mode_use_rx_num)
            return true;

        // ...and the selected TX channel is not the one currently in use, we need to reconfigure
        if(tx_channels.size() > 0 && BIT(tx_channels[0]) != this->rf_phy->pdata->rx1tx1_mode_use_tx_num)
            return true;
    }

    return false;
}
bool SoapyIcyRadio::requiresDataPathReconfiguration(const double new_rate) const
{
    uint8_t phy_channels = this->rf_phy->getChannelCount();

    DLOGF(SOAPY_SDR_DEBUG, "requiresDataPathReconfiguration: Phy channels: %u, new rate %u Sps", phy_channels, (size_t)new_rate);

    // If the Phy is configured for single channel, we allow all sample rates without reconfiguration
    // i.e., the datapath will be forced to reconfigure if we later request the second channel
    if(phy_channels <= 1)
        return false;

    // If the Phy is configured for dual channel, and we request up to half the max baseband rate, we don't need to reconfigure
    if(new_rate <= MAX_BASEBAND_RATE / 2)
        return false;

    // If the Phy is configured for dual channel, and we request more than half the max baseband rate, we need to reconfigure
    return true;
}
void SoapyIcyRadio::reconfigureDataPath(bool rx2tx2, size_t rx_ch, size_t tx_ch)
{
    DLOGF(SOAPY_SDR_DEBUG, "reconfigureDataPath: %s channel mode, use RX%u, use TX%u", rx2tx2 ? "Dual" : "Single", rx_ch, tx_ch);

    this->rf_phy->pdata->rx2tx2 = rx2tx2;
    this->rf_phy->pdata->rx1tx1_mode_use_rx_num = BIT(rx_ch);
    this->rf_phy->pdata->rx1tx1_mode_use_tx_num = BIT(tx_ch);

    this->rf_phy->reset();
    this->rf_phy->clear();

    this->rf_phy->unregisterClocks();
    this->rf_phy->registerClocks();

    this->rf_phy->setup();

    this->rf_phy->loadFIRCoefficients(AD9361::FIRDest::FIR_RX1_RX2, rf_phy_fir_128_dec4, ARRAY_SIZE(rf_phy_fir_128_dec4), 4, -6);
    this->rf_phy->loadFIRCoefficients(AD9361::FIRDest::FIR_TX1_TX2, rf_phy_fir_128_dec4, ARRAY_SIZE(rf_phy_fir_128_dec4), 4, 0);
    this->rf_phy->validateAndEnableFIR();

    this->axi_ad9361->updateActiveChannels();

    this->axi_rf_tstamp->enableClockSyncBypass(!rx2tx2);
    this->axi_rf_tstamp->triggerClockResync(true, 100); // Wait for sync channels, 100 ms timeout
}
void SoapyIcyRadio::validateSampleRateAndChannelCombination(const double rate, const size_t channel_count) const
{
    DLOGF(SOAPY_SDR_DEBUG, "validateSampleRateAndChannelCombination: Rate %u Sps, channel count %u", (size_t)rate, channel_count);

    if(rate < MIN_BASEBAND_RATE)
        throw std::runtime_error("validateSampleRateAndChannelCombination: Rate too low, minimum is " + std::to_string(MIN_BASEBAND_RATE) + " Sps");

    if(channel_count > 1 && rate > MAX_BASEBAND_RATE / 2)
        throw std::runtime_error("validateSampleRateAndChannelCombination: Rate too high for multiple channels, maximum is " + std::to_string(MAX_BASEBAND_RATE / 2) + " Sps");

    if(rate > (2 * MAX_BASEBAND_RATE)) // 122.88 MSPS overclock
        throw std::runtime_error("validateSampleRateAndChannelCombination: Rate too high for single channel, maximum is " + std::to_string(2 * MAX_BASEBAND_RATE) + " Sps");
}

size_t SoapyIcyRadio::getDMAControllerIndex(const int direction, const size_t channel) const
{
    if(direction == SOAPY_SDR_RX)
    {
        // When the Phy is in single channel mode, we always use the first DMA controller
        if(this->rf_phy->getChannelCount() <= 1)
            return AXI_DMAC_RF_RX0_INST;

        if(channel == 0)
            return AXI_DMAC_RF_RX0_INST;
        else if(channel == 1)
            return AXI_DMAC_RF_RX1_INST;
    }
    else if(direction == SOAPY_SDR_TX)
    {
        // When the Phy is in single channel mode, we always use the first DMA controller
        if(this->rf_phy->getChannelCount() <= 1)
            return AXI_DMAC_RF_TX0_INST;

        if(channel == 0)
            return AXI_DMAC_RF_TX0_INST;
        else if(channel == 1)
            return AXI_DMAC_RF_TX1_INST;
    }

    return AXI_DMAC_NUM_INSTANCES;
}
AXIDMAC *SoapyIcyRadio::getDMAController(const int direction, const size_t channel) const
{
    size_t idx = this->getDMAControllerIndex(direction, channel);

    if(idx < AXI_DMAC_NUM_INSTANCES)
        return this->axi_dmac[idx];

    return nullptr;
}
void *SoapyIcyRadio::getDMAPoolStartVirt(const size_t index) const
{
    if(index >= AXI_DMAC_NUM_INSTANCES)
        return nullptr;

    void *total_pool_start = this->mm_dma_buffer->getVirt();
    size_t offset = 0;

    for(size_t i = 0; i < index; i++)
        offset += this->getDMAPoolSizeBytes(i);

    return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(total_pool_start) + offset);
}
void *SoapyIcyRadio::getDMAPoolStartVirt(const int direction, const size_t channel) const
{
    size_t idx = this->getDMAControllerIndex(direction, channel);

    return this->getDMAPoolStartVirt(idx);
}
size_t SoapyIcyRadio::getDMAPoolSizeBytes(const size_t index) const
{
    (void)index; // Suppress unused warning

    // TODO: Make configurable
    return ICYRADIO_DEFAULT_DMA_BUFFER_SIZE_BYTES * ICYRADIO_DEFAULT_DMA_NUM_BUFFERS;
}
size_t SoapyIcyRadio::getDMAPoolSizeBytes(const int direction, size_t channel) const
{
    size_t idx = this->getDMAControllerIndex(direction, channel);

    return this->getDMAPoolSizeBytes(idx);
}

AXIRFTStamp::Channel SoapyIcyRadio::getTimestampingChannel(const int direction, const size_t channel) const
{
    if(direction == SOAPY_SDR_RX)
    {
        // When the Phy is in single channel mode, we always use the first timestamping channel
        if(this->rf_phy->getChannelCount() <= 1)
            return AXIRFTStamp::Channel::RX0;

        if(channel == 0)
            return AXIRFTStamp::Channel::RX0;
        else if(channel == 1)
            return AXIRFTStamp::Channel::RX1;
    }
    else if(direction == SOAPY_SDR_TX)
    {
        // When the Phy is in single channel mode, we always use the first timestamping channel
        if(this->rf_phy->getChannelCount() <= 1)
            return AXIRFTStamp::Channel::TX0;

        if(channel == 0)
            return AXIRFTStamp::Channel::TX0;
        else if(channel == 1)
            return AXIRFTStamp::Channel::TX1;
    }

    return AXIRFTStamp::Channel::CH_MAX;
}

void SoapyIcyRadio::initStreamChannels(SoapyIcyRadio::Stream *stream, const std::vector<size_t> &channels)
{
    for(size_t c : channels)
    {
        std::unique_ptr<SoapyIcyRadio::Stream::Channel> chan = std::make_unique<SoapyIcyRadio::Stream::Channel>();

        DLOGF(SOAPY_SDR_DEBUG, "initStreamChannels: Setup %s%u", (stream->direction == SOAPY_SDR_RX) ? "RX" : "TX", c);

        chan->parent = stream;
        chan->num = c;

        // DMA controllers
        AXIDMAC *dma = this->getDMAController(stream->direction, c);

        if(!dma)
            throw std::runtime_error("initStreamChannels: No DMA controller found for channel " + std::to_string(c));

        if(!dma->idle())
            throw std::runtime_error("initStreamChannels: DMA controller for channel " + std::to_string(c) + " is not idle");

        DLOGF(SOAPY_SDR_DEBUG, "initStreamChannels: Using DMA controller %u", dma->getPeripheralID());

        chan->dma = dma;

        // DMA buffers
        chan->dma_buffers = std::vector<SoapyIcyRadio::Stream::Channel::DMABuffer *>();

        void *buf_start = this->getDMAPoolStartVirt(stream->direction, c);
        size_t buf_free_size = this->getDMAPoolSizeBytes(stream->direction, c);

        DLOGF(SOAPY_SDR_TRACE, "initStreamChannels: Using DMA buffer pool at 0x%016llX, available size %u", buf_start, buf_free_size);

        for(size_t b = 0; b < ICYRADIO_DEFAULT_DMA_NUM_BUFFERS; b++)
        {
            std::unique_ptr<SoapyIcyRadio::Stream::Channel::DMABuffer> buf = std::make_unique<SoapyIcyRadio::Stream::Channel::DMABuffer>();

            DLOGF(SOAPY_SDR_DEBUG, "initStreamChannels: Setup DMA buffer %u", b);

            buf->index = b;
            buf->device = this;
            buf->parent = chan.get();
            buf->virt = buf_start;
            buf->size = ICYRADIO_DEFAULT_DMA_BUFFER_SIZE_BYTES; // TODO: Make configurable

            if(buf_free_size < buf->size)
                throw std::runtime_error("initStreamChannels: Not enough free DMA buffer space");

            uintptr_t buf_phys = this->mm_dma_buffer->getPhys(buf_start) & (BIT(48) - 1); // Apply the mask since we use bit #48 to signal a DMA buffer to the kernel driver
            uint32_t buf_phys_axi = 0x00000000;

            for(size_t i = 0; i < this->axi_pcie->getNumBARs(); i++)
            {
                try
                {
                    buf_phys_axi = this->axi_pcie->getBARAXIAddress(i, buf_phys);

                    DLOGF(SOAPY_SDR_TRACE, "initStreamChannels: Size: 0x%08lX, Virt: 0x%016llX, Phys: 0x%016llX, AXI: 0x%08lX @ PCIe BAR: %u", buf->size, (uintptr_t)buf_start, buf_phys, buf_phys_axi, i);

                    break;
                }
                catch(const std::runtime_error &e)
                {
                    // Cannot use this BAR to access this buffer
                    continue;
                }
            }

            if(buf_phys_axi == 0x00000000)
                throw std::runtime_error("initStreamChannels: Failed to determine buffer AXI address");

            buf->xfer.size = buf->size; // Size of the buffer (also the size of this DMA transfer)
            buf->xfer.flags = AXIDMAC::Transfer::Flags::NONE;

            if(stream->direction == SOAPY_SDR_RX)
                buf->xfer.dest_addr = buf_phys_axi; // AXI pointer to the buffer, the one that the DMA controller will use
            else
                buf->xfer.src_addr = buf_phys_axi;

            buf->xfer.cb = SoapyIcyRadio::DMAHandler;
            buf->xfer.cb_arg = reinterpret_cast<void *>(buf.get());

            buf->idle = true;

            // Update DMA buffer pool
            buf_start = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(buf_start) + buf->size);
            buf_free_size -= buf->size;

            SoapyIcyRadio::Stream::Channel::DMABuffer *_buf = buf.release();

            chan->dma_buffers.push_back(_buf);
        }

        // User buffers
        chan->buffers = std::vector<SoapyIcyRadio::Stream::Channel::Buffer *>();

        DLOGF(SOAPY_SDR_TRACE, "initStreamChannels: Allocating %u user buffers", ICYRADIO_DEFAULT_NUM_BUFFERS);

        for(size_t b = 0; b < ICYRADIO_DEFAULT_NUM_BUFFERS; b++)
        {
            std::unique_ptr<SoapyIcyRadio::Stream::Channel::Buffer> buf = std::make_unique<SoapyIcyRadio::Stream::Channel::Buffer>();

            DLOGF(SOAPY_SDR_DEBUG, "initStreamChannels: Setup user buffer %u", b);

            buf->size = ICYRADIO_DEFAULT_DMA_BUFFER_SIZE_BYTES; // TODO: Make configurable
            buf->addr = std::malloc(buf->size);

            if(!buf->addr)
                throw std::runtime_error("initStreamChannels: Failed to allocate user buffer");

            DLOGF(SOAPY_SDR_TRACE, "initStreamChannels: Size: 0x%08lX, Virt: 0x%016llX", buf->size, (uintptr_t)buf->addr);

            buf->valid_size = 0;
            buf->time = 0;
            buf->time_valid = false;
            buf->acquired = false;

            SoapyIcyRadio::Stream::Channel::Buffer *_buf = buf.release();

            chan->buffers.push_back(_buf);
        }

        chan->next_user_buf = 0;
        chan->next_user_buf_time = 0;
        chan->next_user_buf_time_valid = false;
        chan->next_dma_user_buf = 0;
        chan->next_dma_user_buf_time = 0;
        chan->next_dma_user_buf_time_valid = false;
        chan->underflow = false;
        chan->late = false;

        // Timestamping
        AXIRFTStamp::Channel ts_chan = this->getTimestampingChannel(stream->direction, c);

        if(ts_chan >= AXIRFTStamp::Channel::CH_MAX)
            throw std::runtime_error("initStreamChannels: No timestamping channel found for channel " + std::to_string(c));

        chan->ts_chan = ts_chan;

        if(stream->direction == SOAPY_SDR_RX)
        {
            DLOGF(SOAPY_SDR_TRACE, "initStreamChannels: Using timestamping channel RX%u", ts_chan);

            this->axi_rf_tstamp->disarmCounterLatch(ts_chan);
            this->axi_rf_tstamp->getCounterLatch(ts_chan); // Trigger read to clear valid status

            this->axi_rf_tstamp->disableRXCounter(ts_chan);
            this->axi_rf_tstamp->disableRX(ts_chan);
            this->axi_rf_tstamp->waitRXCounterDisabled(ts_chan, 100);
            this->axi_rf_tstamp->waitRXDisabled(ts_chan, 100);
        }
        else
        {
            DLOGF(SOAPY_SDR_TRACE, "initStreamChannels: Using timestamping channel TX%u", ts_chan);

            this->axi_rf_tstamp->disableTXCounter(ts_chan);
            this->axi_rf_tstamp->disableTX(ts_chan);
            this->axi_rf_tstamp->waitTXCounterDisabled(ts_chan, 100);
            this->axi_rf_tstamp->waitTXDisabled(ts_chan, 100);
        }

        SoapyIcyRadio::Stream::Channel *_chan = chan.release();

        stream->channels.push_back(_chan);
    }
}
void SoapyIcyRadio::reinitStreamChannels(SoapyIcyRadio::Stream *stream)
{
    if(stream->active)
        throw std::runtime_error("reinitStreamChannels: Stream is active");

    std::vector<size_t> channels;

    for(auto &c : stream->channels)
    {
        if(!c->dma)
            continue;

        if(!c->dma->idle())
            throw std::runtime_error("reinitStreamChannels: DMA controller for channel " + std::to_string(c->num) + " is not idle");

        if(c->dma->enabled())
            c->dma->disable();

        channels.push_back(c->num);

        delete c;
    }

    stream->channels.clear();

    this->initStreamChannels(stream, channels);
}

std::vector<SoapyIcyRadio::Stream *> SoapyIcyRadio::getStreams(bool active_only) const
{
    std::vector<SoapyIcyRadio::Stream *> ret;

    for(SoapyIcyRadio::Stream *s : this->streams)
    {
        if(!active_only || s->active)
            ret.push_back(s);
    }

    return ret;
}
std::vector<SoapyIcyRadio::Stream *> SoapyIcyRadio::getStreams(const int direction, bool active_only) const
{
    std::vector<SoapyIcyRadio::Stream *> ret;

    for(SoapyIcyRadio::Stream *s : this->streams)
    {
        if(s->direction == direction && (!active_only || s->active))
            ret.push_back(s);
    }

    return ret;
}
SoapyIcyRadio::Stream *SoapyIcyRadio::_findStream(SoapyIcyRadio::Stream *stream) const
{
    for(SoapyIcyRadio::Stream *s : this->streams)
    {
        if(s == stream)
            return s;
    }

    return nullptr;
}

bool SoapyIcyRadio::isChannelVectorValid(const int direction, const std::vector<size_t> &channels) const
{
    size_t dev_ch_cnt = this->getNumChannels(direction);

    if(channels.size() > dev_ch_cnt)
        return false;

    for(size_t c : channels)
    {
        if(c >= dev_ch_cnt)
            return false;
    }

    return true;
}
bool SoapyIcyRadio::isAnyChannelBusy(const int direction, const std::vector<size_t> &channels) const
{
    for(SoapyIcyRadio::Stream *s : this->streams)
    {
        if(s->direction != direction)
            continue;

        for(auto &c : s->channels)
            if(std::find_if(channels.begin(), channels.end(), [&c](size_t x) { return c != nullptr && x == c->num; }) != channels.end())
                return true;
    }

    return false;
}
std::vector<size_t> SoapyIcyRadio::getBusyChannels(const int direction) const
{
    std::vector<size_t> ret;

    for(SoapyIcyRadio::Stream *s : this->streams)
    {
        if(s->direction != direction)
            continue;

        for(auto &c : s->channels)
            ret.push_back(c->num);
    }

    return ret;
}