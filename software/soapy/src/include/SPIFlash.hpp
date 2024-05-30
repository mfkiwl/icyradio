#pragma once

#include <cstdint>
#include <string>
#include <cstring>
#include <fstream>
#include <thread>
#include <chrono>
#include <mutex>
#include "AXISPI.hpp"
#include "Utils.hpp"

// Configuration commands
#define SPI_FLASH_CMD_NOP                   0x00
#define SPI_FLASH_CMD_RESET_ENABLE          0x66
#define SPI_FLASH_CMD_RESET                 0x99
#define SPI_FLASH_CMD_RESET_CONTINUOUS      0xFF
#define SPI_FLASH_CMD_READ_STATUS           0x05
#define SPI_FLASH_CMD_WRITE_STATUS          0x01
#define SPI_FLASH_CMD_READ_STATUS_2         0x35
#define SPI_FLASH_CMD_WRITE_STATUS_2        0x31
#define SPI_FLASH_CMD_READ_STATUS_3         0x15
#define SPI_FLASH_CMD_WRITE_STATUS_3        0x11

// Read commands
#define SPI_FLASH_CMD_READ                  0x03
#define SPI_FLASH_CMD_READ_FAST             0x0B
#define SPI_FLASH_CMD_READ_FAST_QOUT        0x6B
#define SPI_FLASH_CMD_READ_FAST_QIO         0xEB
#define SPI_FLASH_CMD_READ_FAST_DOUT        0x3B
#define SPI_FLASH_CMD_READ_FAST_DIO         0xBB

// Identification commands
#define SPI_FLASH_CMD_READ_ID               0xAB
#define SPI_FLASH_CMD_READ_MF_DEV_ID        0x90
#define SPI_FLASH_CMD_READ_MF_DEV_ID_DIO    0x92
#define SPI_FLASH_CMD_READ_MF_DEV_ID_QIO    0x94
#define SPI_FLASH_CMD_READ_UID              0x4B
#define SPI_FLASH_CMD_JEDEC_READ_ID         0x9F
#define SPI_FLASH_CMD_SFDP                  0x5A

// Write commands
#define SPI_FLASH_CMD_WRITE_ENABLE          0x06
#define SPI_FLASH_CMD_WRITE_DISABLE         0x04
#define SPI_FLASH_CMD_SECTOR_ERASE_4K       0x20
#define SPI_FLASH_CMD_BLOCK_ERASE_32K       0x52
#define SPI_FLASH_CMD_BLOCK_ERASE_64K       0xD8
#define SPI_FLASH_CMD_CHIP_ERASE            0xC7
#define SPI_FLASH_CMD_WRITE                 0x02
#define SPI_FLASH_CMD_WRITE_QIO             0x32
#define SPI_FLASH_CMD_SUSPEND               0x75
#define SPI_FLASH_CMD_RESUME                0x7A

// Protection commands
#define SPI_FLASH_CMD_GLOBAL_UNPROTECT      0x98
#define SPI_FLASH_CMD_GLOBAL_PROTECT        0x7E
#define SPI_FLASH_CMD_QUERY_PROTECT         0x3D
#define SPI_FLASH_CMD_UNPROTECT             0x39
#define SPI_FLASH_CMD_PROTECT               0x36

// Security area commands
#define SPI_FLASH_CMD_ERASE_SECURITY_REG    0x44
#define SPI_FLASH_CMD_READ_SECURITY_REG     0x48
#define SPI_FLASH_CMD_WRITE_SECURITY_REG    0x42

// Power saving commands
#define SPI_FLASH_CMD_POWER_DOWN            0xB9
#define SPI_FLASH_CMD_RELEASE_POWER_DOWN    0xAB

// Flash constants
#define SPI_FLASH_PAGE_SIZE                 256
#define SPI_FLASH_SECTOR_SIZE               4096
#define SPI_FLASH_BLOCK_SIZE_32K            32768
#define SPI_FLASH_BLOCK_SIZE_64K            65536

// Known IDs
// 0xEF4016 - W25Q32JV-IQ/JQ
// 0xEF7016 - W25Q32JV-IM/JM
// 0xEF4017 - W25Q64JV-IQ/JQ
// 0xEF4017 - W25Q64BV-IQ/JQ
// 0xEF4017 - W25Q64FV-IQ/JQ
// 0xEF4018 - W25Q128JV-IQ/JQ
// 0xEF4018 - W25Q129FV-IQ/JQ
// 0xBF2641 - SST26VF016B
// 0xBF2643 - SST26VF064B

class SPIFlash
{
public:
    enum DeviceID : size_t
    {
        W25Q32JV,
        W25Q64JV,
        W25Q64BV,
        W25Q64FV,
        W25Q128JV,
        W25Q128FV,
        SST26VF016B,
        SST26VF064B,

        UNKNOWN
    };

    struct SPIConfig
    {
        AXISPI *controller;
        uint32_t ss_mask;
    };

private:
    void detectDevice();

    void resetContinuousRead();

public:
    SPIFlash(SPIFlash::SPIConfig spi);

    std::string getDeviceName();

    bool busy();
    void waitNotBusy(uint32_t timeout_ms = 10000);

    void writeEnable();
    void writeDisable();

    uint32_t readJEDECID();
    uint16_t readMFDeviceID();
    uint16_t readMFDeviceIDDual();
    uint16_t readMFDeviceIDQuad();

    uint64_t readUniqueID();

    uint8_t readStatus();
    void writeStatus(uint8_t status);
    uint8_t readStatus2();
    void writeStatus2(uint8_t status);
    uint8_t readStatus3();
    void writeStatus3(uint8_t status);

    void read(uint32_t addr, uint8_t *dst, uint32_t size);
    uint8_t read(uint32_t addr)
    {
        uint8_t data;

        this->read(addr, &data, 1);

        return data;
    }
    void read(uint32_t addr, std::ofstream &dst, uint32_t size);
    void readDualIO(uint32_t addr, uint8_t *dst, uint32_t size, bool cont = false, bool set_cont = false);
    uint8_t readDualIO(uint32_t addr, bool cont = false, bool set_cont = false)
    {
        uint8_t data;

        this->readDualIO(addr, &data, 1, cont, set_cont);

        return data;
    }
    void readDualIO(uint32_t addr, std::ofstream &dst, uint32_t size, bool cont = false, bool set_cont = false);
    void readQuadIO(uint32_t addr, uint8_t *dst, uint32_t size, bool cont = false, bool set_cont = false);
    uint8_t readQuadIO(uint32_t addr, bool cont = false, bool set_cont = false)
    {
        uint8_t data;

        this->readQuadIO(addr, &data, 1, cont, set_cont);

        return data;
    }
    void readQuadIO(uint32_t addr, std::ofstream &dst, uint32_t size, bool cont = false, bool set_cont = false);

    void verify(uint32_t addr, uint8_t *src, uint32_t size, uint32_t chunk_size = SPI_FLASH_SECTOR_SIZE);
    void verify(uint32_t addr, std::ifstream &src, uint32_t size, uint32_t chunk_size = SPI_FLASH_SECTOR_SIZE);
    void verifyDualIO(uint32_t addr, uint8_t *src, uint32_t size, uint32_t chunk_size = SPI_FLASH_SECTOR_SIZE);
    void verifyDualIO(uint32_t addr, std::ifstream &src, uint32_t size, uint32_t chunk_size = SPI_FLASH_SECTOR_SIZE);
    void verifyQuadIO(uint32_t addr, uint8_t *src, uint32_t size, uint32_t chunk_size = SPI_FLASH_SECTOR_SIZE);
    void verifyQuadIO(uint32_t addr, std::ifstream &src, uint32_t size, uint32_t chunk_size = SPI_FLASH_SECTOR_SIZE);

    void eraseChip();
    void eraseSector(uint32_t addr);
    bool isSectorErased(uint32_t addr);
    void eraseBlock32K(uint32_t addr);
    bool isBlock32KErased(uint32_t addr);
    void eraseBlock64K(uint32_t addr);
    bool isBlock64KErased(uint32_t addr);

    void writePage(uint32_t addr, uint8_t *src, uint32_t size);
    void writePageQuadIO(uint32_t addr, uint8_t *src, uint32_t size);

    void write(uint32_t addr, uint8_t *src, uint32_t size, bool erase = true, bool quad = false);
    void write(uint32_t addr, std::ifstream &src, uint32_t size, bool erase = true, bool quad = false);
    inline void writeQuadIO(uint32_t addr, uint8_t *src, uint32_t size, bool erase = true)
    {
        this->write(addr, src, size, erase, true);
    }
    inline void writeQuadIO(uint32_t addr, std::ifstream &src, uint32_t size, bool erase = true)
    {
        this->write(addr, src, size, erase, true);
    }

private:
    SPIFlash::SPIConfig spi;

    SPIFlash::DeviceID dev;
    uint32_t dev_size;

    std::recursive_mutex mutex;
};
