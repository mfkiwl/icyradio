#pragma once

#include <cstdint>
#include <mutex>
#include <unistd.h>
#include "AXIPeripheral.hpp"
#include "Utils.hpp"

#define AXI_QUAD_SPI_NUM_INSTANCES 3

#define AXI_QUAD_SPI_REG_SRR     0x40
#define AXI_QUAD_SPI_REG_SPICR   0x60
#define AXI_QUAD_SPI_REG_SPISR   0x64
#define AXI_QUAD_SPI_REG_SPI_DTR 0x68
#define AXI_QUAD_SPI_REG_SPI_DRR 0x6C
#define AXI_QUAD_SPI_REG_SPISSR  0x70
#define AXI_QUAD_SPI_REG_SPITFOR 0x74
#define AXI_QUAD_SPI_REG_SPIRFOR 0x78
#define AXI_QUAD_SPI_REG_DGIER   0x1C
#define AXI_QUAD_SPI_REG_IPISR   0x20
#define AXI_QUAD_SPI_REG_IPIER   0x28

#define AXI_QUAD_SPI_REG_SRR_RESET_VALUE 0x0A

#define AXI_QUAD_SPI_REG_SPICR_LOOP           BIT(0)
#define AXI_QUAD_SPI_REG_SPICR_SPE            BIT(1)
#define AXI_QUAD_SPI_REG_SPICR_MASTER         BIT(2)
#define AXI_QUAD_SPI_REG_SPICR_CPOL           BIT(3)
#define AXI_QUAD_SPI_REG_SPICR_CPHA           BIT(4)
#define AXI_QUAD_SPI_REG_SPICR_TXFIFO_RESET   BIT(5)
#define AXI_QUAD_SPI_REG_SPICR_RXFIFO_RESET   BIT(6)
#define AXI_QUAD_SPI_REG_SPICR_MANUAL_SS      BIT(7)
#define AXI_QUAD_SPI_REG_SPICR_MASTER_INHIBIT BIT(8)
#define AXI_QUAD_SPI_REG_SPICR_LSB_FIRST      BIT(9)

#define AXI_QUAD_SPI_REG_SPISR_RX_EMPTY       BIT(0)
#define AXI_QUAD_SPI_REG_SPISR_RX_FULL        BIT(1)
#define AXI_QUAD_SPI_REG_SPISR_TX_EMPTY       BIT(2)
#define AXI_QUAD_SPI_REG_SPISR_TX_FULL        BIT(3)
#define AXI_QUAD_SPI_REG_SPISR_MODE_FAULT     BIT(4)
#define AXI_QUAD_SPI_REG_SPISR_SLAVE_MODE     BIT(5)
#define AXI_QUAD_SPI_REG_SPISR_CPOL_CPHA_ERR  BIT(6)
#define AXI_QUAD_SPI_REG_SPISR_SLAVE_MODE_ERR BIT(7)
#define AXI_QUAD_SPI_REG_SPISR_MSB_ERR        BIT(8)
#define AXI_QUAD_SPI_REG_SPISR_LOOPBACK_ERR   BIT(9)
#define AXI_QUAD_SPI_REG_SPISR_CMD_ERR        BIT(10)

#define AXI_QUAD_SPI_REG_DGIER_GIE            BIT(31)

#define AXI_QUAD_SPI_REG_IPIxR_MODE_FAULT         BIT(0)
#define AXI_QUAD_SPI_REG_IPIxR_SLAVE_MODE_FAULT   BIT(1)
#define AXI_QUAD_SPI_REG_IPIxR_DTR_EMPTY          BIT(2)
#define AXI_QUAD_SPI_REG_IPIxR_DTR_UNDERRUN       BIT(3)
#define AXI_QUAD_SPI_REG_IPIxR_DRR_FULL           BIT(4)
#define AXI_QUAD_SPI_REG_IPIxR_DRR_OVERRUN        BIT(5)
#define AXI_QUAD_SPI_REG_IPIxR_TX_FIFO_HALF_EMPTY BIT(6)
#define AXI_QUAD_SPI_REG_IPIxR_SS_MODE            BIT(7)
#define AXI_QUAD_SPI_REG_IPIxR_DRR_NOT_EMPTY      BIT(8)
#define AXI_QUAD_SPI_REG_IPIxR_CPOL_CPHA_ERR      BIT(9)
#define AXI_QUAD_SPI_REG_IPIxR_SLAVE_MODE_ERR     BIT(10)
#define AXI_QUAD_SPI_REG_IPIxR_MSB_ERR            BIT(11)
#define AXI_QUAD_SPI_REG_IPIxR_LOOPBACK_ERR       BIT(12)
#define AXI_QUAD_SPI_REG_IPIxR_CMD_ERR            BIT(13)

#define AXI_QUAD_SPI_LSB_FIRST 0
#define AXI_QUAD_SPI_MSB_FIRST 1

#define AXI_QUAD_SPI_MODE_0 0
#define AXI_QUAD_SPI_MODE_1 1
#define AXI_QUAD_SPI_MODE_2 2
#define AXI_QUAD_SPI_MODE_3 3

class AXIQuadSPI: public AXIPeripheral
{
public:
    enum BitOrder
    {
        LSB_FIRST = 0,
        MSB_FIRST = 1
    };
    enum Mode
    {
        MODE_0 = 0,
        MODE_1 = 1,
        MODE_2 = 2,
        MODE_3 = 3
    };
    enum Select
    {
        DESELECT = 0,
        SELECT = 1,
    };
    enum Wait
    {
        NO_WAIT = 0,
        WAIT = 1,
    };

    AXIQuadSPI(void *base_address);
    AXIQuadSPI(void *base_address, AXIQuadSPI::Mode mode, AXIQuadSPI::BitOrder bit_order = AXIQuadSPI::MSB_FIRST);

    void init(AXIQuadSPI::Mode mode = AXIQuadSPI::MODE_0, AXIQuadSPI::BitOrder bit_order = AXIQuadSPI::MSB_FIRST);

    void slaveSelect(uint32_t mask, AXIQuadSPI::Select select);

    uint8_t transferByte(const uint8_t data);
    void writeByte(const uint8_t data, const AXIQuadSPI::Wait wait = AXIQuadSPI::WAIT);
    inline uint8_t readByte()
    {
        return this->transferByte(0x00);
    }
    inline void transfer(const uint8_t *src, uint32_t size, uint8_t *dst)
    {
        if(src == nullptr)
            throw std::invalid_argument("AXI Quad SPI: Source buffer cannot be null, use read() if not sending data");

        if(dst == nullptr)
            throw std::invalid_argument("AXI Quad SPI: Destination buffer cannot be null, use write() if not receiving data");

        while(size--)
            *dst++ = this->transferByte(*src++);
    }
    inline void write(const uint8_t *src, uint32_t size, const AXIQuadSPI::Wait wait = AXIQuadSPI::WAIT)
    {
        if(src == nullptr)
            throw std::invalid_argument("AXI Quad SPI: Source buffer cannot be null");

        while(size--)
            this->writeByte(*src++, wait == AXIQuadSPI::WAIT && !size ? AXIQuadSPI::WAIT : AXIQuadSPI::NO_WAIT);
    }
    inline void read(uint8_t *dst, uint32_t size, uint8_t send_data = 0x00)
    {
        if(dst == nullptr)
            throw std::invalid_argument("AXI Quad SPI: Destination buffer cannot be null");

        while(size--)
            *dst++ = this->transferByte(send_data);
    }

private:
    std::mutex mutex;
};

// Instance 0 - axi_quad_spi_0 - QSPI Flash
#define AXI_QUAD_SPI_FLASH_INST 0

#define AXI_QUAD_SPI_FLASH_SS BIT(0)

// Instance 1 - axi_quad_spi_1 - Transceiver
#define AXI_QUAD_SPI_TRX_INST 1

#define AXI_QUAD_SPI_TRX_SS BIT(0)

// Instance 2 - axi_quad_spi_2 - mmWave Synthesizer
#define AXI_QUAD_SPI_SYNTH_INST 2

#define AXI_QUAD_SPI_SYNTH_SS BIT(0)