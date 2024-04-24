#include "SPIFlash.hpp"

SPIFlash::SPIFlash(SPIFlash::SPIConfig spi)
{
    this->spi = spi;

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->dev = SPIFlash::DeviceID::UNKNOWN;
    this->dev_size = 0;

    this->resetContinuousRead();

    this->writeEnable();
    this->writeStatus2(BIT(1)); // Set the QE bit
    this->writeEnable();
    this->writeStatus3(0x00); // Set maximum drive strength
    this->writeDisable(); // Needed in case device does not support status 3 register, otherwise the WE bit may be left set

    this->detectDevice();
}

void SPIFlash::detectDevice()
{
    uint32_t jedec = this->readJEDECID();

    switch(jedec)
    {
        case 0xEF4017:
        {
            uint16_t mf_dev = this->readMFDeviceID();

            if(mf_dev != 0xEF16)
                break;

            this->dev_size = 8 * 1024 * 1024;

            // Distinguish between BV and JV/FV variants
            uint16_t mf_dev_dual = this->readMFDeviceIDDual();
            uint16_t mf_dev_quad = this->readMFDeviceIDQuad();

            if(mf_dev_dual != 0xEF16 && mf_dev_quad != 0xEF16)
            {
                this->dev = SPIFlash::DeviceID::W25Q64BV; // BV does not have dual/quad I/O ID read support
            }
            else
            {
                // Distinguish between JV and FV variants
                uint8_t data = this->read(0x00000024);
                uint8_t data_dual = this->readDualIO(0x00000024, false, true);
                uint8_t data_dual_cont = this->readDualIO(0x00000024, true, false);

                if(data == data_dual)
                {
                    if(data_dual != data_dual_cont)
                        this->dev = SPIFlash::DeviceID::W25Q64JV; // JV does not support continuous read
                    else
                        this->dev = SPIFlash::DeviceID::W25Q64FV; // FV supports continuous read
                }
            }
        }
        break;
    }
}

void SPIFlash::resetContinuousRead()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(0xFF, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}

std::string SPIFlash::getDeviceName()
{
    switch(this->dev)
    {
        case SPIFlash::DeviceID::W25Q32JV:
            return "W25Q32JV";
        case SPIFlash::DeviceID::W25Q64JV:
            return "W25Q64JV";
        case SPIFlash::DeviceID::W25Q64BV:
            return "W25Q64BV";
        case SPIFlash::DeviceID::W25Q64FV:
            return "W25Q64FV";
        case SPIFlash::DeviceID::SST26VF016B:
            return "SST26VF016B";
        case SPIFlash::DeviceID::SST26VF064B:
            return "SST26VF064B";
        default:
            return "Unknown";
    }
}

bool SPIFlash::busy()
{
    return !!(this->readStatus() & BIT(0));
}
void SPIFlash::waitNotBusy(uint32_t timeout_ms)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t status;

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_STATUS, true);
    this->spi.controller->read(&status, 1);

    uint64_t timeout = (uint64_t)timeout_ms * 100ULL;

    while(--timeout && (status & BIT(0)))
    {
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        this->spi.controller->read(&status, 1);
    }

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    if(status & BIT(0))
        throw std::runtime_error("SPI Flash: Timed out waiting for flash not busy");
}

void SPIFlash::writeEnable()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_WRITE_ENABLE, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}
void SPIFlash::writeDisable()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_WRITE_DISABLE, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}

uint32_t SPIFlash::readJEDECID()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t buf[3];

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_JEDEC_READ_ID, true);
    this->spi.controller->read(buf, 3);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return ((uint32_t)buf[0] << 16) | ((uint32_t)buf[1] << 8) | (uint32_t)buf[2];
}
uint16_t SPIFlash::readMFDeviceID()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t buf[2];

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_MF_DEV_ID, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, true);
    this->spi.controller->read(buf, 2);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
}
uint16_t SPIFlash::readMFDeviceIDDual()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t buf[2];

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_MF_DEV_ID_DIO, true);

    this->spi.controller->setIOMode(AXISPI::IOMode::DUAL);

    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0xF0, true);
    this->spi.controller->read(buf, 2);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return ((uint16_t)buf[0] << 8) | (uint16_t)buf[1];
}
uint16_t SPIFlash::readMFDeviceIDQuad()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t buf[4];

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_MF_DEV_ID_QIO, false);

    this->spi.controller->setIOMode(AXISPI::IOMode::QUAD);

    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0xF0, true);
    this->spi.controller->read(buf, 4);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return ((uint16_t)buf[2] << 8) | (uint16_t)buf[3];
}

uint64_t SPIFlash::readUniqueID()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    switch(this->dev)
    {
        case SPIFlash::DeviceID::W25Q32JV:
        case SPIFlash::DeviceID::W25Q64JV:
        case SPIFlash::DeviceID::W25Q64BV:
        case SPIFlash::DeviceID::W25Q64FV:
        break;
        default:
            throw std::runtime_error("SPI Flash: Device does not support unique ID read");
    }

    uint8_t buf[12];

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_UID, true);
    this->spi.controller->read(buf, 12);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return ((uint64_t)buf[4] << 56) | ((uint64_t)buf[5] << 48) | ((uint64_t)buf[6] << 40) | ((uint64_t)buf[7] << 32) |
           ((uint64_t)buf[8] << 24) | ((uint64_t)buf[9] << 16) | ((uint64_t)buf[10] << 8) | (uint64_t)buf[11];

    // TODO: SST26xx have the UID in the security region
}

uint8_t SPIFlash::readStatus()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t status;

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_STATUS, true);
    this->spi.controller->read(&status, 1);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return status;
}
void SPIFlash::writeStatus(uint8_t status)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->writeEnable();

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_WRITE_STATUS, false);
    this->spi.controller->write(status, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}
uint8_t SPIFlash::readStatus2()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t status;

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_STATUS_2, true);
    this->spi.controller->read(&status, 1);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return status;
}
void SPIFlash::writeStatus2(uint8_t status)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->writeEnable();

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_WRITE_STATUS_2, false);
    this->spi.controller->write(status, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}
uint8_t SPIFlash::readStatus3()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    uint8_t status;

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_STATUS_3, true);
    this->spi.controller->read(&status, 1);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    return status;
}
void SPIFlash::writeStatus3(uint8_t status)
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->writeEnable();

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_WRITE_STATUS_3, false);
    this->spi.controller->write(status, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}

void SPIFlash::read(uint32_t addr, uint8_t *dst, uint32_t size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!dst)
        throw std::invalid_argument("SPI Flash: Destination buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_FAST, false);
    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);
    this->spi.controller->write(0x00, true);
    this->spi.controller->read(dst, size);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}
void SPIFlash::read(uint32_t addr, std::ofstream &dst, uint32_t size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!dst)
        throw std::invalid_argument("SPI Flash: Destination buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_READ_FAST, false);
    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);
    this->spi.controller->write(0x00, true);

    while(size--)
    {
        uint8_t data = this->spi.controller->read();

        dst.write((char *)&data, 1);
    }

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}
void SPIFlash::readDualIO(uint32_t addr, uint8_t *dst, uint32_t size, bool cont, bool set_cont)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!dst)
        throw std::invalid_argument("SPI Flash: Destination buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    if(!cont)
    {
        this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

        this->spi.controller->write(SPI_FLASH_CMD_READ_FAST_DIO, false);
    }

    this->spi.controller->setIOMode(AXISPI::IOMode::DUAL);

    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);
    this->spi.controller->write(set_cont ? 0xA0 : 0xF0, true);
    this->spi.controller->read(dst, size);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}
void SPIFlash::readDualIO(uint32_t addr, std::ofstream &dst, uint32_t size, bool cont, bool set_cont)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!dst)
        throw std::invalid_argument("SPI Flash: Destination buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    if(!cont)
    {
        this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

        this->spi.controller->write(SPI_FLASH_CMD_READ_FAST_DIO, false);
    }

    this->spi.controller->setIOMode(AXISPI::IOMode::DUAL);

    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);
    this->spi.controller->write(set_cont ? 0xA0 : 0xF0, true);

    while(size--)
    {
        uint8_t data = this->spi.controller->read();

        dst.write((char *)&data, 1);
    }

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}
void SPIFlash::readQuadIO(uint32_t addr, uint8_t *dst, uint32_t size, bool cont, bool set_cont)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!dst)
        throw std::invalid_argument("SPI Flash: Destination buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    if(!cont)
    {
        this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

        this->spi.controller->write(SPI_FLASH_CMD_READ_FAST_QIO, false);
    }

    this->spi.controller->setIOMode(AXISPI::IOMode::QUAD);

    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);
    this->spi.controller->write(set_cont ? 0xA0 : 0xF0, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, true);
    this->spi.controller->read(dst, size);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}
void SPIFlash::readQuadIO(uint32_t addr, std::ofstream &dst, uint32_t size, bool cont, bool set_cont)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!dst)
        throw std::invalid_argument("SPI Flash: Destination buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    if(this->spi.controller == nullptr)
        throw std::runtime_error("SPI Flash: SPI not initialized");

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    if(!cont)
    {
        this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

        this->spi.controller->write(SPI_FLASH_CMD_READ_FAST_QIO, false);
    }

    this->spi.controller->setIOMode(AXISPI::IOMode::QUAD);

    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);
    this->spi.controller->write(set_cont ? 0xA0 : 0xF0, false);
    this->spi.controller->write(0x00, false);
    this->spi.controller->write(0x00, true);

    while(size--)
    {
        uint8_t data = this->spi.controller->read();

        dst.write((char *)&data, 1);
    }

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex
}

void SPIFlash::verify(uint32_t addr, uint8_t *src, uint32_t size, uint32_t chunk_size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    if(!chunk_size)
        throw std::invalid_argument("SPI Flash: Chunk size cannot be zero");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    while(size)
    {
        uint32_t chunk = std::min(size, chunk_size);

        uint8_t buf[chunk];

        this->read(addr, buf, chunk);

        if(std::memcmp(src, buf, chunk))
            throw std::runtime_error("SPI Flash: Verification failed");

        addr += chunk;
        src += chunk;
        size -= chunk;
    }
}
void SPIFlash::verify(uint32_t addr, std::ifstream &src, uint32_t size, uint32_t chunk_size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    if(!chunk_size)
        throw std::invalid_argument("SPI Flash: Chunk size cannot be zero");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    while(size)
    {
        uint32_t chunk = std::min(size, chunk_size);

        uint8_t buf[chunk];

        this->read(addr, buf, chunk);

        uint8_t src_buf[chunk];

        src.read((char *)src_buf, chunk);

        if(std::memcmp(src_buf, buf, chunk))
            throw std::runtime_error("SPI Flash: Verification failed");

        addr += chunk;
        size -= chunk;
    }
}
void SPIFlash::verifyDualIO(uint32_t addr, uint8_t *src, uint32_t size, uint32_t chunk_size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    if(!chunk_size)
        throw std::invalid_argument("SPI Flash: Chunk size cannot be zero");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    while(size)
    {
        uint32_t chunk = std::min(size, chunk_size);

        uint8_t buf[chunk];

        this->readDualIO(addr, buf, chunk, false, true);

        if(std::memcmp(src, buf, chunk))
            throw std::runtime_error("SPI Flash: Verification failed");

        addr += chunk;
        src += chunk;
        size -= chunk;
    }
}
void SPIFlash::verifyDualIO(uint32_t addr, std::ifstream &src, uint32_t size, uint32_t chunk_size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    if(!chunk_size)
        throw std::invalid_argument("SPI Flash: Chunk size cannot be zero");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    while(size)
    {
        uint32_t chunk = std::min(size, chunk_size);

        uint8_t buf[chunk];

        this->readDualIO(addr, buf, chunk, false, true);

        uint8_t src_buf[chunk];

        src.read((char *)src_buf, chunk);

        if(std::memcmp(src_buf, buf, chunk))
            throw std::runtime_error("SPI Flash: Verification failed");

        addr += chunk;
        size -= chunk;
    }
}
void SPIFlash::verifyQuadIO(uint32_t addr, uint8_t *src, uint32_t size, uint32_t chunk_size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    if(!chunk_size)
        throw std::invalid_argument("SPI Flash: Chunk size cannot be zero");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    while(size)
    {
        uint32_t chunk = std::min(size, chunk_size);

        uint8_t buf[chunk];

        this->readQuadIO(addr, buf, chunk, false, true);

        if(std::memcmp(src, buf, chunk))
            throw std::runtime_error("SPI Flash: Verification failed");

        addr += chunk;
        src += chunk;
        size -= chunk;
    }
}
void SPIFlash::verifyQuadIO(uint32_t addr, std::ifstream &src, uint32_t size, uint32_t chunk_size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    if(!chunk_size)
        throw std::invalid_argument("SPI Flash: Chunk size cannot be zero");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    while(size)
    {
        uint32_t chunk = std::min(size, chunk_size);

        uint8_t buf[chunk];

        this->readQuadIO(addr, buf, chunk, false, true);

        uint8_t src_buf[chunk];

        src.read((char *)src_buf, chunk);

        if(std::memcmp(src_buf, buf, chunk))
            throw std::runtime_error("SPI Flash: Verification failed");

        addr += chunk;
        size -= chunk;
    }
}

void SPIFlash::eraseChip()
{
    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_CHIP_ERASE, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}
void SPIFlash::eraseSector(uint32_t addr)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(addr & (SPI_FLASH_SECTOR_SIZE - 1))
        throw std::invalid_argument("SPI Flash: Address not sector aligned");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_SECTOR_ERASE_4K, false);
    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}
bool SPIFlash::isSectorErased(uint32_t addr)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(addr & (SPI_FLASH_SECTOR_SIZE - 1))
        throw std::invalid_argument("SPI Flash: Address not sector aligned");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t buf[SPI_FLASH_SECTOR_SIZE];

    this->read(addr, buf, SPI_FLASH_SECTOR_SIZE);

    for(size_t i = 0; i < SPI_FLASH_SECTOR_SIZE; i++)
    {
        if(buf[i] != 0xFF)
            return false;
    }

    return true;
}
void SPIFlash::eraseBlock32K(uint32_t addr)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(addr & (SPI_FLASH_BLOCK_SIZE_32K - 1))
        throw std::invalid_argument("SPI Flash: Address not 32K block aligned");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_BLOCK_ERASE_32K, false);
    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}
bool SPIFlash::isBlock32KErased(uint32_t addr)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(addr & (SPI_FLASH_BLOCK_SIZE_32K - 1))
        throw std::invalid_argument("SPI Flash: Address not 32K block aligned");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    for(size_t i = 0; i < SPI_FLASH_BLOCK_SIZE_32K; i += SPI_FLASH_SECTOR_SIZE)
    {
        if(!this->isSectorErased(addr + i))
            return false;
    }

    return true;
}
void SPIFlash::eraseBlock64K(uint32_t addr)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(addr & (SPI_FLASH_BLOCK_SIZE_64K - 1))
        throw std::invalid_argument("SPI Flash: Address not 64K block aligned");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_BLOCK_ERASE_64K, false);
    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}
bool SPIFlash::isBlock64KErased(uint32_t addr)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(addr & (SPI_FLASH_BLOCK_SIZE_64K - 1))
        throw std::invalid_argument("SPI Flash: Address not 64K block aligned");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    for(size_t i = 0; i < SPI_FLASH_BLOCK_SIZE_64K; i += SPI_FLASH_SECTOR_SIZE)
    {
        if(!this->isSectorErased(addr + i))
            return false;
    }

    return true;
}

void SPIFlash::writePage(uint32_t addr, uint8_t *src, uint32_t size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    uint32_t max_size = SPI_FLASH_PAGE_SIZE - (addr & (SPI_FLASH_PAGE_SIZE - 1));

    if(size > max_size)
        throw std::invalid_argument("SPI Flash: Write size crosses page boundary");

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_WRITE, false);
    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);
    this->spi.controller->write(src, size, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}
void SPIFlash::writePageQuadIO(uint32_t addr, uint8_t *src, uint32_t size)
{
    if(addr >= this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    uint32_t max_size = SPI_FLASH_PAGE_SIZE - (addr & (SPI_FLASH_PAGE_SIZE - 1));

    if(size > max_size)
        throw std::invalid_argument("SPI Flash: Write size crosses page boundary");

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    this->spi.controller->selectSlave(this->spi.ss_mask, true); // This locks mutex

    this->spi.controller->setIOMode(AXISPI::IOMode::SINGLE);

    this->spi.controller->write(SPI_FLASH_CMD_WRITE_QIO, false);
    this->spi.controller->write((addr >> 16) & 0xFF, false);
    this->spi.controller->write((addr >> 8) & 0xFF, false);
    this->spi.controller->write((addr >> 0) & 0xFF, false);

    this->spi.controller->setIOMode(AXISPI::IOMode::QUAD);

    this->spi.controller->write(src, size, true);

    this->spi.controller->selectSlave(this->spi.ss_mask, false); // This unlocks mutex

    this->waitNotBusy();
}

void SPIFlash::write(uint32_t addr, uint8_t *src, uint32_t size, bool erase, bool quad)
{
    if(addr + size > this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    if(!size)
        return;

    if(!src)
        throw std::invalid_argument("SPI Flash: Source buffer cannot be null");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint32_t _addr = addr & ~(SPI_FLASH_SECTOR_SIZE - 1); // Align to sector boundary
    uint32_t _offset = addr & (SPI_FLASH_SECTOR_SIZE - 1);

    if(_offset)
    {
        // Write the first sector, then the address is aligned and we can write the rest
        uint8_t buf[SPI_FLASH_SECTOR_SIZE];

        if(quad)
            this->readQuadIO(_addr, buf, SPI_FLASH_SECTOR_SIZE);
        else
            this->read(_addr, buf, SPI_FLASH_SECTOR_SIZE);

        uint32_t _size = MIN(size, SPI_FLASH_SECTOR_SIZE - _offset);

        std::memcpy(buf + _offset, src, _size);

        if(erase)
        {
            this->writeEnable();
            this->eraseSector(_addr);

            if(!this->isSectorErased(_addr))
                throw std::runtime_error("SPI Flash: Sector erase failed, flash state is unknown");
        }

        for(size_t i = 0; i < SPI_FLASH_SECTOR_SIZE; i += SPI_FLASH_PAGE_SIZE)
        {
            this->writeEnable();

            if(quad)
                this->writePageQuadIO(_addr, buf + i, SPI_FLASH_PAGE_SIZE);
            else
                this->writePage(_addr, buf + i, SPI_FLASH_PAGE_SIZE);

            _addr += SPI_FLASH_PAGE_SIZE;
        }

        src += _size;
        size -= _size;
    }

    // Write in sector-sized chunks
    while(size >= SPI_FLASH_SECTOR_SIZE)
    {
        uint32_t _size = MIN(size, SPI_FLASH_SECTOR_SIZE - _offset);

        if(erase)
        {
            this->writeEnable();
            this->eraseSector(_addr);

            if(!this->isSectorErased(_addr))
                throw std::runtime_error("SPI Flash: Sector erase failed, flash state is unknown");
        }

        for(size_t i = 0; i < _size; i += SPI_FLASH_PAGE_SIZE)
        {
            this->writeEnable();

            if(quad)
                this->writePageQuadIO(_addr, src, SPI_FLASH_PAGE_SIZE);
            else
                this->writePage(_addr, src, SPI_FLASH_PAGE_SIZE);

            _addr += SPI_FLASH_PAGE_SIZE;
            src += SPI_FLASH_PAGE_SIZE;
        }

        size -= _size;
    }

    if(size)
    {
        // Write the last sector
        uint8_t buf[SPI_FLASH_SECTOR_SIZE];

        if(quad)
            this->readQuadIO(_addr, buf, SPI_FLASH_SECTOR_SIZE);
        else
            this->read(_addr, buf, SPI_FLASH_SECTOR_SIZE);

        std::memcpy(buf, src, size);

        if(erase)
        {
            this->writeEnable();
            this->eraseSector(_addr);

            if(!this->isSectorErased(_addr))
                throw std::runtime_error("SPI Flash: Sector erase failed, flash state is unknown");
        }

        for(size_t i = 0; i < SPI_FLASH_SECTOR_SIZE; i += SPI_FLASH_PAGE_SIZE)
        {
            this->writeEnable();

            if(quad)
                this->writePageQuadIO(_addr, buf + i, SPI_FLASH_PAGE_SIZE);
            else
                this->writePage(_addr, buf + i, SPI_FLASH_PAGE_SIZE);

            _addr += SPI_FLASH_PAGE_SIZE;
        }

        src += size;
        size = 0;
    }
}
void SPIFlash::write(uint32_t addr, std::ifstream &src, uint32_t size, bool erase, bool quad)
{
    std::streampos cur_pos = src.tellg();
    src.seekg(0, std::ios::end);
    std::streampos end_pos = src.tellg();
    src.seekg(cur_pos);

    if(size > (end_pos - cur_pos))
        throw std::out_of_range("SPI Flash: Input stream does not contain enough data");

    size = size == 0 ? (end_pos - cur_pos) : size;

    if(addr + size > this->dev_size)
        throw std::out_of_range("SPI Flash: Address out of range");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint32_t _addr = addr & ~(SPI_FLASH_SECTOR_SIZE - 1); // Align to sector boundary
    uint32_t _offset = addr & (SPI_FLASH_SECTOR_SIZE - 1);

    if(_offset)
    {
        // Write the first sector, then the address is aligned and we can write the rest
        uint8_t buf[SPI_FLASH_SECTOR_SIZE];

        if(quad)
            this->readQuadIO(_addr, buf, SPI_FLASH_SECTOR_SIZE);
        else
            this->read(_addr, buf, SPI_FLASH_SECTOR_SIZE);

        uint32_t _size = MIN(size, SPI_FLASH_SECTOR_SIZE - _offset);

        src.read((char *)(buf + _offset), _size);

        if(erase)
        {
            this->writeEnable();
            this->eraseSector(_addr);

            if(!this->isSectorErased(_addr))
                throw std::runtime_error("SPI Flash: Sector erase failed, flash state is unknown");
        }

        for(size_t i = 0; i < SPI_FLASH_SECTOR_SIZE; i += SPI_FLASH_PAGE_SIZE)
        {
            this->writeEnable();

            if(quad)
                this->writePageQuadIO(_addr, buf + i, SPI_FLASH_PAGE_SIZE);
            else
                this->writePage(_addr, buf + i, SPI_FLASH_PAGE_SIZE);

            _addr += SPI_FLASH_PAGE_SIZE;
        }

        size -= _size;
    }

    // Write in sector-sized chunks
    while(size >= SPI_FLASH_SECTOR_SIZE)
    {
        uint32_t _size = MIN(size, SPI_FLASH_SECTOR_SIZE - _offset);

        if(erase)
        {
            this->writeEnable();
            this->eraseSector(_addr);

            if(!this->isSectorErased(_addr))
                throw std::runtime_error("SPI Flash: Sector erase failed, flash state is unknown");
        }

        for(size_t i = 0; i < _size; i += SPI_FLASH_PAGE_SIZE)
        {
            uint8_t buf[SPI_FLASH_PAGE_SIZE];

            src.read((char *)buf, SPI_FLASH_PAGE_SIZE);

            this->writeEnable();

            if(quad)
                this->writePageQuadIO(_addr, buf, SPI_FLASH_PAGE_SIZE);
            else
                this->writePage(_addr, buf, SPI_FLASH_PAGE_SIZE);

            _addr += SPI_FLASH_PAGE_SIZE;
        }

        size -= _size;
    }

    if(size)
    {
        // Write the last sector
        uint8_t buf[SPI_FLASH_SECTOR_SIZE];

        if(quad)
            this->readQuadIO(_addr, buf, SPI_FLASH_SECTOR_SIZE);
        else
            this->read(_addr, buf, SPI_FLASH_SECTOR_SIZE);

        src.read((char *)buf, size);

        if(erase)
        {
            this->writeEnable();
            this->eraseSector(_addr);

            if(!this->isSectorErased(_addr))
                throw std::runtime_error("SPI Flash: Sector erase failed, flash state is unknown");
        }

        for(size_t i = 0; i < SPI_FLASH_SECTOR_SIZE; i += SPI_FLASH_PAGE_SIZE)
        {
            this->writeEnable();

            if(quad)
                this->writePageQuadIO(_addr, buf + i, SPI_FLASH_PAGE_SIZE);
            else
                this->writePage(_addr, buf + i, SPI_FLASH_PAGE_SIZE);

            _addr += SPI_FLASH_PAGE_SIZE;
        }
    }
}