#include "AuxMCU.hpp"

uint8_t AuxMCU::calcChecksum(uint8_t *data, uint8_t count)
{
    uint8_t cs = this->iic.addr;

    for(uint16_t i = 0; i < count; i++)
        cs += data[i];

    return (0xFF - cs) + 1;
}

void AuxMCU::readMem(bool rom, uint8_t addr, uint8_t *dst, uint8_t count, bool check)
{
    if(this->iic.controller == nullptr)
        throw std::runtime_error("AuxMCU: IIC not initialized");

    if(!count)
        return;

    if(dst == nullptr)
        throw std::invalid_argument("AuxMCU: Invalid destination buffer");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t buf[256];

    buf[0] = addr;
    buf[1] = rom ? 0xFE : 0xFF;
    buf[2] = count;
    buf[3] = this->calcChecksum(buf, 3);

    this->iic.controller->startAtomicTransaction(); // Lock the I2C bus so the next two transactions are not interrupted

    this->iic.controller->write(this->iic.addr, buf, 4, AXIIIC::Stop::RESTART);
    this->iic.controller->read(this->iic.addr, buf, count + 1, AXIIIC::Stop::STOP);

    this->iic.controller->endAtomicTransaction(); // Unlock the I2C bus

    if(check && !this->checkChecksum(buf, count + 1))
        throw std::runtime_error("AuxMCU: Checksum mismatch");

    for(uint8_t i = 0; i < count; i++)
        dst[i] = buf[i];
}
void AuxMCU::writeReg(uint8_t reg, uint8_t *src, uint8_t count)
{
    if(this->iic.controller == nullptr)
        throw std::runtime_error("AuxMCU: IIC not initialized");

    if(!count)
        return;

    if(src == nullptr)
        throw std::invalid_argument("AuxMCU: Invalid source buffer");

    if(count > 256 - 3)
        throw std::invalid_argument("AuxMCU: Data too long");

    std::lock_guard<std::recursive_mutex> lock(this->mutex);

    uint8_t buf[256];

    buf[0] = reg;
    buf[1] = count;

    for(uint8_t i = 0; i < count; i++)
        buf[i + 2] = src[i];

    buf[count + 2] = this->calcChecksum(buf, count + 2);

    // No need to lock the I2C bus here, since I2C transactions are guaranteed to be atomic
    this->iic.controller->write(this->iic.addr, buf, count + 3, AXIIIC::Stop::STOP);
}

AuxMCU::AuxMCU(AuxMCU::IICConfig iic)
{
    this->iic = iic;

    if(this->iic.controller == nullptr)
        throw std::runtime_error("AuxMCU: IIC not initialized");

    bool found = false;
    uint32_t timeout = 1000UL;

    while(--timeout)
    {
        if(this->iic.controller->scan(this->iic.addr))
        {
            found = true;

            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    if(!found)
        throw std::runtime_error("AuxMCU: Device not found on IIC bus");
}
AuxMCU::~AuxMCU()
{
    // Nothing to do here
}