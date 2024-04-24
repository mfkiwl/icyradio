#include "sercom.h"

void _putchar(char ch)
{
    sercom3_uart_write_byte((uint8_t)ch);
}


#ifdef SERCOM0_MODE_I2C_MASTER
void sercom0_i2c_master_init(uint32_t ulSCLFreq, uint32_t ulTRise)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM0_Msk, 1);

    while(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_SWRST_Msk;

    while(SERCOM0_REGS->I2CM.SERCOM_CTRLA & SERCOM_I2CM_CTRLA_SWRST_Msk);

    SERCOM0_REGS->I2CM.SERCOM_CTRLA = SERCOM_I2CM_CTRLA_LOWTOUT_Msk | SERCOM_I2CM_CTRLA_INACTOUT_55US | SERCOM_I2CM_CTRLA_SDAHOLD_450NS | SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;

    uint32_t ulSourceFreq = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM0_CORE_Val];
    int32_t lBaud = (10U * ulSourceFreq / ulSCLFreq) - 100;

    if(ulTRise)
        lBaud -= ulSourceFreq / (100000000U / ulTRise);

    lBaud = DIV_ROUND(lBaud, 20);

    if(lBaud > 255)
        lBaud = 255;
    else if(lBaud <= 1)
        lBaud = 1;

    SERCOM0_REGS->I2CM.SERCOM_BAUD = (uint8_t)lBaud;

    while(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE_Msk;

    while(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->I2CM.SERCOM_STATUS = SERCOM_I2CM_STATUS_LOWTOUT_Msk | (0x01 << SERCOM_I2CM_STATUS_BUSSTATE_Pos) | SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk;

    while(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom0_i2c_master_transmit(uint8_t ubAddress, uint8_t *pubSrc, uint32_t ulCount, uint8_t ubStop)
{
    if((ubAddress & 1) && !ulCount)
        return 0;

    if((SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_BUSSTATE_Msk) == SERCOM_I2CM_STATUS_BUSSTATE(3))
        return 0;

    SERCOM0_REGS->I2CM.SERCOM_ADDR = ubAddress;

    if(!ulCount)
    {
        while(!(SERCOM0_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

        if(SERCOM0_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        if(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
        {
            SERCOM0_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

            while(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

            return 0;
        }
    }
    else
    {
        do
        {
            if(ubAddress & 1) // Read
            {
                while(!(SERCOM0_REGS->I2CM.SERCOM_INTFLAG & (SERCOM_I2CM_INTFLAG_MB_Msk | SERCOM_I2CM_INTFLAG_SB_Msk)));

                if(SERCOM0_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM0_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_SB_Msk)
                {
                    *pubSrc++ = SERCOM0_REGS->I2CM.SERCOM_DATA;

                    if(ulCount > 1)
                        SERCOM0_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(2);
                }
            }
            else // Write
            {
                while(!(SERCOM0_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

                if(SERCOM0_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
                {
                    SERCOM0_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

                    while(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

                    return 0;
                }

                SERCOM0_REGS->I2CM.SERCOM_DATA = *pubSrc++;

                while(!(SERCOM0_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));
            }
        } while(--ulCount);
    }

    if(ubStop)
    {
        if(SERCOM0_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        SERCOM0_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_ACKACT_Msk | SERCOM_I2CM_CTRLB_CMD(3);

        while(SERCOM0_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
    }

    return 1;
}
#endif // SERCOM0_MODE_I2C_MASTER
#ifdef SERCOM0_MODE_I2C_SLAVE
static sercom_i2c_slave_addr_isr_t pfSERCOM0I2CSlaveAddrISR = NULL;
static sercom_i2c_slave_tx_data_isr_t pfSERCOM0I2CSlaveTXDataISR = NULL;
static sercom_i2c_slave_rx_data_isr_t pfSERCOM0I2CSlaveRXDataISR = NULL;

void _sercom0_i2c_slave_isr()
{
    static uint8_t ubLastWasAddress = 0;
    uint32_t ulFlags = SERCOM0_REGS->I2CS.SERCOM_INTFLAG;

    if(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk)
    {
        volatile uint8_t ubAddress = SERCOM0_REGS->I2CS.SERCOM_DATA;

        if(pfSERCOM0I2CSlaveAddrISR && pfSERCOM0I2CSlaveAddrISR(ubAddress))
            SERCOM0_REGS->I2CS.SERCOM_CTRLB &= ~SERCOM_I2CS_CTRLB_ACKACT_Msk;
        else
            SERCOM0_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk;

        SERCOM0_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_AMATCH_Msk;

        ubLastWasAddress = 1;
    }

    if(ulFlags & SERCOM_I2CS_INTFLAG_DRDY_Msk)
    {
        if(SERCOM0_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_DIR_Msk) // Master is trying to read from us
        {
            if(!ubLastWasAddress && (SERCOM0_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_RXNACK_Msk))
                SERCOM0_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(2);
            else
                SERCOM0_REGS->I2CS.SERCOM_DATA = pfSERCOM0I2CSlaveTXDataISR ? pfSERCOM0I2CSlaveTXDataISR() : 0xFF;
        }
        else // Master is trying to write to us, there is data available
        {
            volatile uint8_t ubData = SERCOM0_REGS->I2CS.SERCOM_DATA;

            if(pfSERCOM0I2CSlaveRXDataISR && pfSERCOM0I2CSlaveRXDataISR(ubData))
                SERCOM0_REGS->I2CS.SERCOM_CTRLB = (SERCOM0_REGS->I2CS.SERCOM_CTRLB & ~SERCOM_I2CS_CTRLB_ACKACT_Msk) | SERCOM_I2CS_CTRLB_CMD(3);
            else
                SERCOM0_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk | SERCOM_I2CS_CTRLB_CMD(2);
        }
    }

    if(!(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk) && ubLastWasAddress)
        ubLastWasAddress = 0;
}

void sercom0_i2c_slave_init(uint8_t ubAddress)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM0_Msk, 1);

    while(SERCOM0_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_SWRST_Msk;

    while(SERCOM0_REGS->I2CS.SERCOM_CTRLA & SERCOM_I2CS_CTRLA_SWRST_Msk);

    SERCOM0_REGS->I2CS.SERCOM_CTRLA = SERCOM_I2CS_CTRLA_SDAHOLD_450NS | SERCOM_I2CS_CTRLA_RUNSTDBY_Msk | SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE;
    SERCOM0_REGS->I2CS.SERCOM_CTRLB = (0 << SERCOM_I2CS_CTRLB_AMODE_Pos);
    SERCOM0_REGS->I2CS.SERCOM_ADDR = SERCOM_I2CS_ADDR_ADDR(ubAddress);

    SERCOM0_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_Msk; // Clear all flags
    IRQ_CLEAR(SERCOM0_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM0_IRQn, 2, 0); // Set priority 2,0
    IRQ_ENABLE(SERCOM0_IRQn); // Enable vector
    SERCOM0_REGS->I2CS.SERCOM_INTENSET = SERCOM_I2CS_INTFLAG_DRDY_Msk | SERCOM_I2CS_INTFLAG_AMATCH_Msk; // Enable DRDY and AMATCH flags

    SERCOM0_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE_Msk;

    while(SERCOM0_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->I2CS.SERCOM_STATUS = SERCOM_I2CS_STATUS_Msk;

    while(SERCOM0_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);
}
void sercom0_i2c_slave_set_addr_isr(sercom_i2c_slave_addr_isr_t pfISR)
{
    pfSERCOM0I2CSlaveAddrISR = pfISR;
}
void sercom0_i2c_slave_set_tx_data_isr(sercom_i2c_slave_tx_data_isr_t pfISR)
{
    pfSERCOM0I2CSlaveTXDataISR = pfISR;
}
void sercom0_i2c_slave_set_rx_data_isr(sercom_i2c_slave_rx_data_isr_t pfISR)
{
    pfSERCOM0I2CSlaveRXDataISR = pfISR;
}
#endif // SERCOM0_MODE_I2C_SLAVE
#ifdef SERCOM0_MODE_SPI
void sercom0_spi_init(uint32_t ulBaud, uint8_t ubMode, uint8_t ubBitMode, uint8_t ubDIPO, uint8_t ubDOPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM0_Msk, 1);

    while(SERCOM0_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_SWRST_Msk;

    while(SERCOM0_REGS->SPIM.SERCOM_CTRLA & SERCOM_SPIM_CTRLA_SWRST_Msk);

    SERCOM0_REGS->SPIM.SERCOM_CTRLA = (ubBitMode == SERCOM_SPI_MSB_FIRST ? 0 : SERCOM_SPIM_CTRLA_DORD_Msk) | (ubMode & 2 ? SERCOM_SPIM_CTRLA_CPOL_Msk : 0) | (ubMode & 1 ? SERCOM_SPIM_CTRLA_CPHA_Msk : 0) | SERCOM_SPIM_CTRLA_DIPO(ubDIPO) | SERCOM_SPIM_CTRLA_DOPO(ubDOPO) | SERCOM_SPIM_CTRLA_MODE_SPI_MASTER;

    while(SERCOM0_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;
    SERCOM0_REGS->SPIM.SERCOM_BAUD = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM0_CORE_Val] / (ulBaud << 1) - 1;

    while(SERCOM0_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM0_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE_Msk;

    while(SERCOM0_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom0_spi_transfer_byte(const uint8_t ubData)
{
    while(!(SERCOM0_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(!(SERCOM0_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk))
    {
        SERCOM0_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN_Msk;

        while(SERCOM0_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
    }

    SERCOM0_REGS->SPIM.SERCOM_DATA = ubData;

    while(!(SERCOM0_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk));

    return SERCOM0_REGS->SPIM.SERCOM_DATA;
}
void sercom0_spi_write_byte(const uint8_t ubData, const uint8_t ubWait)
{
    while(!(SERCOM0_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(SERCOM0_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk)
        SERCOM0_REGS->SPIM.SERCOM_CTRLB &= ~SERCOM_SPIM_CTRLB_RXEN_Msk;

    SERCOM0_REGS->SPIM.SERCOM_DATA = ubData;

    while(ubWait && !(SERCOM0_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_TXC_Msk));
}
#endif // SERCOM0_MODE_SPI
#ifdef SERCOM0_MODE_UART
static volatile uint8_t *pubSERCOM0UARTFIFO = NULL;
static volatile uint16_t usSERCOM0UARTFIFOWritePos, usSERCOM0UARTFIFOReadPos;

void _sercom0_uart_isr()
{
    while(SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk)
    {
        pubSERCOM0UARTFIFO[usSERCOM0UARTFIFOWritePos++] = SERCOM0_REGS->USART_INT.SERCOM_DATA;

        if(usSERCOM0UARTFIFOWritePos >= SERCOM0_FIFO_SIZE)
            usSERCOM0UARTFIFOWritePos = 0;
    }
}

void sercom0_uart_init(uint32_t ulBaud, uint32_t ulFrameSettings, uint8_t ubRXPO, uint8_t ubTXPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM0_Msk, 1);

    while(SERCOM0_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    free((uint8_t *)pubSERCOM0UARTFIFO);

    pubSERCOM0UARTFIFO = (volatile uint8_t *)malloc(SERCOM0_FIFO_SIZE);

    if(!pubSERCOM0UARTFIFO)
        return;

    memset((uint8_t *)pubSERCOM0UARTFIFO, 0, SERCOM0_FIFO_SIZE);

    usSERCOM0UARTFIFOWritePos = 0;
    usSERCOM0UARTFIFOReadPos = 0;

    SERCOM0_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SWRST_Msk;

    while(SERCOM0_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_SWRST_Msk);

    SERCOM0_REGS->USART_INT.SERCOM_CTRLA = (ulFrameSettings & SERCOM_USART_INT_CTRLA_DORD_Msk) | SERCOM_USART_INT_CTRLA_CMODE_ASYNC | (ulFrameSettings & SERCOM_USART_INT_CTRLA_FORM_Msk) | SERCOM_USART_INT_CTRLA_RXPO(ubRXPO) | SERCOM_USART_INT_CTRLA_TXPO(ubTXPO) | SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK;
    SERCOM0_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_RXEN_Msk | SERCOM_USART_INT_CTRLB_TXEN_Msk | (ulFrameSettings & SERCOM_USART_INT_CTRLB_PMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_SBMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_CHSIZE_Msk);

    uint64_t ullBaud = 65536U * 100U - 65536ULL * 16ULL * 100ULL * (uint64_t)ulBaud / (uint64_t)GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM0_CORE_Val];
    ullBaud = DIV_ROUND(ullBaud, 100);

    if(ullBaud > 65535)
        ullBaud = 65535;

    SERCOM0_REGS->USART_INT.SERCOM_BAUD = (uint16_t)ullBaud;

    while(SERCOM0_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    IRQ_CLEAR(SERCOM0_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM0_IRQn, 2, 1); // Set priority 2,1
    IRQ_ENABLE(SERCOM0_IRQn); // Enable vector
    SERCOM0_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC_Msk; // Enable RXC flag

    SERCOM0_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

    while(SERCOM0_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);
}
void sercom0_uart_write_byte(const uint8_t ubData)
{
    while(!(SERCOM0_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk));

    SERCOM0_REGS->USART_INT.SERCOM_DATA = ubData;
}
uint8_t sercom0_uart_read_byte()
{
    if(!sercom0_uart_available())
        return 0;

    uint8_t ubData;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ubData = pubSERCOM0UARTFIFO[usSERCOM0UARTFIFOReadPos++];

        if(usSERCOM0UARTFIFOReadPos >= SERCOM0_FIFO_SIZE)
            usSERCOM0UARTFIFOReadPos = 0;
    }

    return ubData;
}
uint32_t sercom0_uart_available()
{
    return (SERCOM0_FIFO_SIZE + usSERCOM0UARTFIFOWritePos - usSERCOM0UARTFIFOReadPos) % SERCOM0_FIFO_SIZE;
}
void sercom0_uart_flush()
{
    usSERCOM0UARTFIFOReadPos = usSERCOM0UARTFIFOWritePos = 0;
}
#endif // SERCOM0_MODE_UART

void _sercom0_isr()
{
    #if defined(SERCOM0_MODE_I2C_SLAVE) && !defined(SERCOM0_MODE_UART)
        _sercom0_i2c_slave_isr();
    #elif !defined(SERCOM0_MODE_I2C_SLAVE) && defined(SERCOM0_MODE_UART)
        _sercom0_uart_isr();
    #elif defined(SERCOM0_MODE_I2C_SLAVE) && defined(SERCOM0_MODE_UART)
        switch(SERCOM0_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_MODE_Msk)
        {
            case SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK:
                _sercom0_uart_isr();
            break;
            case SERCOM_USART_INT_CTRLA_MODE_I2C_SLAVE:
                _sercom0_i2c_slave_isr();
            break;
        }
    #endif
}


#ifdef SERCOM1_MODE_I2C_MASTER
void sercom1_i2c_master_init(uint32_t ulSCLFreq, uint32_t ulTRise)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM1_Msk, 1);

    while(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_SWRST_Msk;

    while(SERCOM1_REGS->I2CM.SERCOM_CTRLA & SERCOM_I2CM_CTRLA_SWRST_Msk);

    SERCOM1_REGS->I2CM.SERCOM_CTRLA = SERCOM_I2CM_CTRLA_LOWTOUT_Msk | SERCOM_I2CM_CTRLA_INACTOUT_55US | SERCOM_I2CM_CTRLA_SDAHOLD_450NS | SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;

    uint32_t ulSourceFreq = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM1_CORE_Val];
    int32_t lBaud = (10U * ulSourceFreq / ulSCLFreq) - 100;

    if(ulTRise)
        lBaud -= ulSourceFreq / (100000000U / ulTRise);

    lBaud = DIV_ROUND(lBaud, 20);

    if(lBaud > 255)
        lBaud = 255;
    else if(lBaud <= 1)
        lBaud = 1;

    SERCOM1_REGS->I2CM.SERCOM_BAUD = (uint8_t)lBaud;

    while(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE_Msk;

    while(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->I2CM.SERCOM_STATUS = SERCOM_I2CM_STATUS_LOWTOUT_Msk | (0x01 << SERCOM_I2CM_STATUS_BUSSTATE_Pos) | SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk;

    while(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom1_i2c_master_transmit(uint8_t ubAddress, uint8_t *pubSrc, uint32_t ulCount, uint8_t ubStop)
{
    if((ubAddress & 1) && !ulCount)
        return 0;

    if((SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_BUSSTATE_Msk) == SERCOM_I2CM_STATUS_BUSSTATE(3))
        return 0;

    SERCOM1_REGS->I2CM.SERCOM_ADDR = ubAddress;

    if(!ulCount)
    {
        while(!(SERCOM1_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

        if(SERCOM1_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        if(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
        {
            SERCOM1_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

            while(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

            return 0;
        }
    }
    else
    {
        do
        {
            if(ubAddress & 1) // Read
            {
                while(!(SERCOM1_REGS->I2CM.SERCOM_INTFLAG & (SERCOM_I2CM_INTFLAG_MB_Msk | SERCOM_I2CM_INTFLAG_SB_Msk)));

                if(SERCOM1_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM1_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_SB_Msk)
                {
                    *pubSrc++ = SERCOM1_REGS->I2CM.SERCOM_DATA;

                    if(ulCount > 1)
                        SERCOM1_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(2);
                }
            }
            else // Write
            {
                while(!(SERCOM1_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

                if(SERCOM1_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
                {
                    SERCOM1_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

                    while(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

                    return 0;
                }

                SERCOM1_REGS->I2CM.SERCOM_DATA = *pubSrc++;

                while(!(SERCOM1_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));
            }
        } while(--ulCount);
    }

    if(ubStop)
    {
        if(SERCOM1_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        SERCOM1_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_ACKACT_Msk | SERCOM_I2CM_CTRLB_CMD(3);

        while(SERCOM1_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
    }

    return 1;
}
#endif // SERCOM1_MODE_I2C_MASTER
#ifdef SERCOM1_MODE_I2C_SLAVE
static sercom_i2c_slave_addr_isr_t pfSERCOM1I2CSlaveAddrISR = NULL;
static sercom_i2c_slave_tx_data_isr_t pfSERCOM1I2CSlaveTXDataISR = NULL;
static sercom_i2c_slave_rx_data_isr_t pfSERCOM1I2CSlaveRXDataISR = NULL;

void _sercom1_i2c_slave_isr()
{
    static uint8_t ubLastWasAddress = 0;
    uint32_t ulFlags = SERCOM1_REGS->I2CS.SERCOM_INTFLAG;

    if(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk)
    {
        volatile uint8_t ubAddress = SERCOM1_REGS->I2CS.SERCOM_DATA;

        if(pfSERCOM1I2CSlaveAddrISR && pfSERCOM1I2CSlaveAddrISR(ubAddress))
            SERCOM1_REGS->I2CS.SERCOM_CTRLB &= ~SERCOM_I2CS_CTRLB_ACKACT_Msk;
        else
            SERCOM1_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk;

        SERCOM1_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_AMATCH_Msk;

        ubLastWasAddress = 1;
    }

    if(ulFlags & SERCOM_I2CS_INTFLAG_DRDY_Msk)
    {
        if(SERCOM1_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_DIR_Msk) // Master is trying to read from us
        {
            if(!ubLastWasAddress && (SERCOM1_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_RXNACK_Msk))
                SERCOM1_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(2);
            else
                SERCOM1_REGS->I2CS.SERCOM_DATA = pfSERCOM1I2CSlaveTXDataISR ? pfSERCOM1I2CSlaveTXDataISR() : 0xFF;
        }
        else // Master is trying to write to us, there is data available
        {
            volatile uint8_t ubData = SERCOM1_REGS->I2CS.SERCOM_DATA;

            if(pfSERCOM1I2CSlaveRXDataISR && pfSERCOM1I2CSlaveRXDataISR(ubData))
                SERCOM1_REGS->I2CS.SERCOM_CTRLB = (SERCOM1_REGS->I2CS.SERCOM_CTRLB & ~SERCOM_I2CS_CTRLB_ACKACT_Msk) | SERCOM_I2CS_CTRLB_CMD(3);
            else
                SERCOM1_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk | SERCOM_I2CS_CTRLB_CMD(2);
        }
    }

    if(!(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk) && ubLastWasAddress)
        ubLastWasAddress = 0;
}

void sercom1_i2c_slave_init(uint8_t ubAddress)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM1_Msk, 1);

    while(SERCOM1_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_SWRST_Msk;

    while(SERCOM1_REGS->I2CS.SERCOM_CTRLA & SERCOM_I2CS_CTRLA_SWRST_Msk);

    SERCOM1_REGS->I2CS.SERCOM_CTRLA = SERCOM_I2CS_CTRLA_SDAHOLD_450NS | SERCOM_I2CS_CTRLA_RUNSTDBY_Msk | SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE;
    SERCOM1_REGS->I2CS.SERCOM_CTRLB = (0 << SERCOM_I2CS_CTRLB_AMODE_Pos);
    SERCOM1_REGS->I2CS.SERCOM_ADDR = SERCOM_I2CS_ADDR_ADDR(ubAddress);

    SERCOM1_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_Msk; // Clear all flags
    IRQ_CLEAR(SERCOM1_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM1_IRQn, 2, 0); // Set priority 2,0
    IRQ_ENABLE(SERCOM1_IRQn); // Enable vector
    SERCOM1_REGS->I2CS.SERCOM_INTENSET = SERCOM_I2CS_INTFLAG_DRDY_Msk | SERCOM_I2CS_INTFLAG_AMATCH_Msk; // Enable DRDY and AMATCH flags

    SERCOM1_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE_Msk;

    while(SERCOM1_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->I2CS.SERCOM_STATUS = SERCOM_I2CS_STATUS_Msk;

    while(SERCOM1_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);
}
void sercom1_i2c_slave_set_addr_isr(sercom_i2c_slave_addr_isr_t pfISR)
{
    pfSERCOM1I2CSlaveAddrISR = pfISR;
}
void sercom1_i2c_slave_set_tx_data_isr(sercom_i2c_slave_tx_data_isr_t pfISR)
{
    pfSERCOM1I2CSlaveTXDataISR = pfISR;
}
void sercom1_i2c_slave_set_rx_data_isr(sercom_i2c_slave_rx_data_isr_t pfISR)
{
    pfSERCOM1I2CSlaveRXDataISR = pfISR;
}
#endif // SERCOM1_MODE_I2C_SLAVE
#ifdef SERCOM1_MODE_SPI
void sercom1_spi_init(uint32_t ulBaud, uint8_t ubMode, uint8_t ubBitMode, uint8_t ubDIPO, uint8_t ubDOPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM1_Msk, 1);

    while(SERCOM1_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_SWRST_Msk;

    while(SERCOM1_REGS->SPIM.SERCOM_CTRLA & SERCOM_SPIM_CTRLA_SWRST_Msk);

    SERCOM1_REGS->SPIM.SERCOM_CTRLA = (ubBitMode == SERCOM_SPI_MSB_FIRST ? 0 : SERCOM_SPIM_CTRLA_DORD_Msk) | (ubMode & 2 ? SERCOM_SPIM_CTRLA_CPOL_Msk : 0) | (ubMode & 1 ? SERCOM_SPIM_CTRLA_CPHA_Msk : 0) | SERCOM_SPIM_CTRLA_DIPO(ubDIPO) | SERCOM_SPIM_CTRLA_DOPO(ubDOPO) | SERCOM_SPIM_CTRLA_MODE_SPI_MASTER;

    while(SERCOM1_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;
    SERCOM1_REGS->SPIM.SERCOM_BAUD = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM1_CORE_Val] / (ulBaud << 1) - 1;

    while(SERCOM1_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM1_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE_Msk;

    while(SERCOM1_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom1_spi_transfer_byte(const uint8_t ubData)
{
    while(!(SERCOM1_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(!(SERCOM1_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk))
    {
        SERCOM1_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN_Msk;

        while(SERCOM1_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
    }

    SERCOM1_REGS->SPIM.SERCOM_DATA = ubData;

    while(!(SERCOM1_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk));

    return SERCOM1_REGS->SPIM.SERCOM_DATA;
}
void sercom1_spi_write_byte(const uint8_t ubData, const uint8_t ubWait)
{
    while(!(SERCOM1_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(SERCOM1_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk)
        SERCOM1_REGS->SPIM.SERCOM_CTRLB &= ~SERCOM_SPIM_CTRLB_RXEN_Msk;

    SERCOM1_REGS->SPIM.SERCOM_DATA = ubData;

    while(ubWait && !(SERCOM1_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_TXC_Msk));
}
#endif // SERCOM1_MODE_SPI
#ifdef SERCOM1_MODE_UART
static volatile uint8_t *pubSERCOM1UARTFIFO = NULL;
static volatile uint16_t usSERCOM1UARTFIFOWritePos, usSERCOM1UARTFIFOReadPos;

void _sercom1_uart_isr()
{
    while(SERCOM1_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk)
    {
        pubSERCOM1UARTFIFO[usSERCOM1UARTFIFOWritePos++] = SERCOM1_REGS->USART_INT.SERCOM_DATA;

        if(usSERCOM1UARTFIFOWritePos >= SERCOM1_FIFO_SIZE)
            usSERCOM1UARTFIFOWritePos = 0;
    }
}

void sercom1_uart_init(uint32_t ulBaud, uint32_t ulFrameSettings, uint8_t ubRXPO, uint8_t ubTXPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM1_Msk, 1);

    while(SERCOM1_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    free((uint8_t *)pubSERCOM1UARTFIFO);

    pubSERCOM1UARTFIFO = (volatile uint8_t *)malloc(SERCOM1_FIFO_SIZE);

    if(!pubSERCOM1UARTFIFO)
        return;

    memset((uint8_t *)pubSERCOM1UARTFIFO, 0, SERCOM1_FIFO_SIZE);

    usSERCOM1UARTFIFOWritePos = 0;
    usSERCOM1UARTFIFOReadPos = 0;

    SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SWRST_Msk;

    while(SERCOM1_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_SWRST_Msk);

    SERCOM1_REGS->USART_INT.SERCOM_CTRLA = (ulFrameSettings & SERCOM_USART_INT_CTRLA_DORD_Msk) | SERCOM_USART_INT_CTRLA_CMODE_ASYNC | (ulFrameSettings & SERCOM_USART_INT_CTRLA_FORM_Msk) | SERCOM_USART_INT_CTRLA_RXPO(ubRXPO) | SERCOM_USART_INT_CTRLA_TXPO(ubTXPO) | SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK;
    SERCOM1_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_RXEN_Msk | SERCOM_USART_INT_CTRLB_TXEN_Msk | (ulFrameSettings & SERCOM_USART_INT_CTRLB_PMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_SBMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_CHSIZE_Msk);

    uint64_t ullBaud = 65536U * 100U - 65536ULL * 16ULL * 100ULL * (uint64_t)ulBaud / (uint64_t)GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM1_CORE_Val];
    ullBaud = DIV_ROUND(ullBaud, 100);

    if(ullBaud > 65535)
        ullBaud = 65535;

    SERCOM1_REGS->USART_INT.SERCOM_BAUD = (uint16_t)ullBaud;

    while(SERCOM1_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    IRQ_CLEAR(SERCOM1_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM1_IRQn, 2, 1); // Set priority 2,1
    IRQ_ENABLE(SERCOM1_IRQn); // Enable vector
    SERCOM1_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC_Msk; // Enable RXC flag

    SERCOM1_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

    while(SERCOM1_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);
}
void sercom1_uart_write_byte(const uint8_t ubData)
{
    while(!(SERCOM1_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk));

    SERCOM1_REGS->USART_INT.SERCOM_DATA = ubData;
}
uint8_t sercom1_uart_read_byte()
{
    if(!sercom1_uart_available())
        return 0;

    uint8_t ubData;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ubData = pubSERCOM1UARTFIFO[usSERCOM1UARTFIFOReadPos++];

        if(usSERCOM1UARTFIFOReadPos >= SERCOM1_FIFO_SIZE)
            usSERCOM1UARTFIFOReadPos = 0;
    }

    return ubData;
}
uint32_t sercom1_uart_available()
{
    return (SERCOM1_FIFO_SIZE + usSERCOM1UARTFIFOWritePos - usSERCOM1UARTFIFOReadPos) % SERCOM1_FIFO_SIZE;
}
void sercom1_uart_flush()
{
    usSERCOM1UARTFIFOReadPos = usSERCOM1UARTFIFOWritePos = 0;
}
#endif // SERCOM1_MODE_UART

void _sercom1_isr()
{
    #if defined(SERCOM1_MODE_I2C_SLAVE) && !defined(SERCOM1_MODE_UART)
        _sercom1_i2c_slave_isr();
    #elif !defined(SERCOM1_MODE_I2C_SLAVE) && defined(SERCOM1_MODE_UART)
        _sercom1_uart_isr();
    #elif defined(SERCOM1_MODE_I2C_SLAVE) && defined(SERCOM1_MODE_UART)
        switch(SERCOM1_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_MODE_Msk)
        {
            case SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK:
                _sercom1_uart_isr();
            break;
            case SERCOM_USART_INT_CTRLA_MODE_I2C_SLAVE:
                _sercom1_i2c_slave_isr();
            break;
        }
    #endif
}


#ifdef SERCOM2_MODE_I2C_MASTER
void sercom2_i2c_master_init(uint32_t ulSCLFreq, uint32_t ulTRise)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM2_Msk, 1);

    while(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_SWRST_Msk;

    while(SERCOM2_REGS->I2CM.SERCOM_CTRLA & SERCOM_I2CM_CTRLA_SWRST_Msk);

    SERCOM2_REGS->I2CM.SERCOM_CTRLA = SERCOM_I2CM_CTRLA_LOWTOUT_Msk | SERCOM_I2CM_CTRLA_INACTOUT_55US | SERCOM_I2CM_CTRLA_SDAHOLD_450NS | SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;

    uint32_t ulSourceFreq = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM2_CORE_Val];
    int32_t lBaud = (10U * ulSourceFreq / ulSCLFreq) - 100;

    if(ulTRise)
        lBaud -= ulSourceFreq / (100000000U / ulTRise);

    lBaud = DIV_ROUND(lBaud, 20);

    if(lBaud > 255)
        lBaud = 255;
    else if(lBaud <= 1)
        lBaud = 1;

    SERCOM2_REGS->I2CM.SERCOM_BAUD = (uint8_t)lBaud;

    while(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE_Msk;

    while(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->I2CM.SERCOM_STATUS = SERCOM_I2CM_STATUS_LOWTOUT_Msk | (0x01 << SERCOM_I2CM_STATUS_BUSSTATE_Pos) | SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk;

    while(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom2_i2c_master_transmit(uint8_t ubAddress, uint8_t *pubSrc, uint32_t ulCount, uint8_t ubStop)
{
    if((ubAddress & 1) && !ulCount)
        return 0;

    if((SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_BUSSTATE_Msk) == SERCOM_I2CM_STATUS_BUSSTATE(3))
        return 0;

    SERCOM2_REGS->I2CM.SERCOM_ADDR = ubAddress;

    if(!ulCount)
    {
        while(!(SERCOM2_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

        if(SERCOM2_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        if(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
        {
            SERCOM2_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

            while(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

            return 0;
        }
    }
    else
    {
        do
        {
            if(ubAddress & 1) // Read
            {
                while(!(SERCOM2_REGS->I2CM.SERCOM_INTFLAG & (SERCOM_I2CM_INTFLAG_MB_Msk | SERCOM_I2CM_INTFLAG_SB_Msk)));

                if(SERCOM2_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM2_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_SB_Msk)
                {
                    *pubSrc++ = SERCOM2_REGS->I2CM.SERCOM_DATA;

                    if(ulCount > 1)
                        SERCOM2_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(2);
                }
            }
            else // Write
            {
                while(!(SERCOM2_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

                if(SERCOM2_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
                {
                    SERCOM2_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

                    while(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

                    return 0;
                }

                SERCOM2_REGS->I2CM.SERCOM_DATA = *pubSrc++;

                while(!(SERCOM2_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));
            }
        } while(--ulCount);
    }

    if(ubStop)
    {
        if(SERCOM2_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        SERCOM2_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_ACKACT_Msk | SERCOM_I2CM_CTRLB_CMD(3);

        while(SERCOM2_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
    }

    return 1;
}
#endif // SERCOM2_MODE_I2C_MASTER
#ifdef SERCOM2_MODE_I2C_SLAVE
static sercom_i2c_slave_addr_isr_t pfSERCOM2I2CSlaveAddrISR = NULL;
static sercom_i2c_slave_tx_data_isr_t pfSERCOM2I2CSlaveTXDataISR = NULL;
static sercom_i2c_slave_rx_data_isr_t pfSERCOM2I2CSlaveRXDataISR = NULL;

void _sercom2_i2c_slave_isr()
{
    static uint8_t ubLastWasAddress = 0;
    uint32_t ulFlags = SERCOM2_REGS->I2CS.SERCOM_INTFLAG;

    if(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk)
    {
        volatile uint8_t ubAddress = SERCOM2_REGS->I2CS.SERCOM_DATA;

        if(pfSERCOM2I2CSlaveAddrISR && pfSERCOM2I2CSlaveAddrISR(ubAddress))
            SERCOM2_REGS->I2CS.SERCOM_CTRLB &= ~SERCOM_I2CS_CTRLB_ACKACT_Msk;
        else
            SERCOM2_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk;

        SERCOM2_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_AMATCH_Msk;

        ubLastWasAddress = 1;
    }

    if(ulFlags & SERCOM_I2CS_INTFLAG_DRDY_Msk)
    {
        if(SERCOM2_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_DIR_Msk) // Master is trying to read from us
        {
            if(!ubLastWasAddress && (SERCOM2_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_RXNACK_Msk))
                SERCOM2_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(2);
            else
                SERCOM2_REGS->I2CS.SERCOM_DATA = pfSERCOM2I2CSlaveTXDataISR ? pfSERCOM2I2CSlaveTXDataISR() : 0xFF;
        }
        else // Master is trying to write to us, there is data available
        {
            volatile uint8_t ubData = SERCOM2_REGS->I2CS.SERCOM_DATA;

            if(pfSERCOM2I2CSlaveRXDataISR && pfSERCOM2I2CSlaveRXDataISR(ubData))
                SERCOM2_REGS->I2CS.SERCOM_CTRLB = (SERCOM2_REGS->I2CS.SERCOM_CTRLB & ~SERCOM_I2CS_CTRLB_ACKACT_Msk) | SERCOM_I2CS_CTRLB_CMD(3);
            else
                SERCOM2_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk | SERCOM_I2CS_CTRLB_CMD(2);
        }
    }

    if(!(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk) && ubLastWasAddress)
        ubLastWasAddress = 0;
}

void sercom2_i2c_slave_init(uint8_t ubAddress)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM2_Msk, 1);

    while(SERCOM2_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_SWRST_Msk;

    while(SERCOM2_REGS->I2CS.SERCOM_CTRLA & SERCOM_I2CS_CTRLA_SWRST_Msk);

    SERCOM2_REGS->I2CS.SERCOM_CTRLA = SERCOM_I2CS_CTRLA_SDAHOLD_450NS | SERCOM_I2CS_CTRLA_RUNSTDBY_Msk | SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE;
    SERCOM2_REGS->I2CS.SERCOM_CTRLB = (0 << SERCOM_I2CS_CTRLB_AMODE_Pos);
    SERCOM2_REGS->I2CS.SERCOM_ADDR = SERCOM_I2CS_ADDR_ADDR(ubAddress);

    SERCOM2_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_Msk; // Clear all flags
    IRQ_CLEAR(SERCOM2_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM2_IRQn, 2, 0); // Set priority 2,0
    IRQ_ENABLE(SERCOM2_IRQn); // Enable vector
    SERCOM2_REGS->I2CS.SERCOM_INTENSET = SERCOM_I2CS_INTFLAG_DRDY_Msk | SERCOM_I2CS_INTFLAG_AMATCH_Msk; // Enable DRDY and AMATCH flags

    SERCOM2_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE_Msk;

    while(SERCOM2_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->I2CS.SERCOM_STATUS = SERCOM_I2CS_STATUS_Msk;

    while(SERCOM2_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);
}
void sercom2_i2c_slave_set_addr_isr(sercom_i2c_slave_addr_isr_t pfISR)
{
    pfSERCOM2I2CSlaveAddrISR = pfISR;
}
void sercom2_i2c_slave_set_tx_data_isr(sercom_i2c_slave_tx_data_isr_t pfISR)
{
    pfSERCOM2I2CSlaveTXDataISR = pfISR;
}
void sercom2_i2c_slave_set_rx_data_isr(sercom_i2c_slave_rx_data_isr_t pfISR)
{
    pfSERCOM2I2CSlaveRXDataISR = pfISR;
}
#endif // SERCOM2_MODE_I2C_SLAVE
#ifdef SERCOM2_MODE_SPI
void sercom2_spi_init(uint32_t ulBaud, uint8_t ubMode, uint8_t ubBitMode, uint8_t ubDIPO, uint8_t ubDOPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM2_Msk, 1);

    while(SERCOM2_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_SWRST_Msk;

    while(SERCOM2_REGS->SPIM.SERCOM_CTRLA & SERCOM_SPIM_CTRLA_SWRST_Msk);

    SERCOM2_REGS->SPIM.SERCOM_CTRLA = (ubBitMode == SERCOM_SPI_MSB_FIRST ? 0 : SERCOM_SPIM_CTRLA_DORD_Msk) | (ubMode & 2 ? SERCOM_SPIM_CTRLA_CPOL_Msk : 0) | (ubMode & 1 ? SERCOM_SPIM_CTRLA_CPHA_Msk : 0) | SERCOM_SPIM_CTRLA_DIPO(ubDIPO) | SERCOM_SPIM_CTRLA_DOPO(ubDOPO) | SERCOM_SPIM_CTRLA_MODE_SPI_MASTER;

    while(SERCOM2_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;
    SERCOM2_REGS->SPIM.SERCOM_BAUD = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM2_CORE_Val] / (ulBaud << 1) - 1;

    while(SERCOM2_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM2_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE_Msk;

    while(SERCOM2_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom2_spi_transfer_byte(const uint8_t ubData)
{
    while(!(SERCOM2_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(!(SERCOM2_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk))
    {
        SERCOM2_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN_Msk;

        while(SERCOM2_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
    }

    SERCOM2_REGS->SPIM.SERCOM_DATA = ubData;

    while(!(SERCOM2_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk));

    return SERCOM2_REGS->SPIM.SERCOM_DATA;
}
void sercom2_spi_write_byte(const uint8_t ubData, const uint8_t ubWait)
{
    while(!(SERCOM2_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(SERCOM2_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk)
        SERCOM2_REGS->SPIM.SERCOM_CTRLB &= ~SERCOM_SPIM_CTRLB_RXEN_Msk;

    SERCOM2_REGS->SPIM.SERCOM_DATA = ubData;

    while(ubWait && !(SERCOM2_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_TXC_Msk));
}
#endif // SERCOM2_MODE_SPI
#ifdef SERCOM2_MODE_UART
static volatile uint8_t *pubSERCOM2UARTFIFO = NULL;
static volatile uint16_t usSERCOM2UARTFIFOWritePos, usSERCOM2UARTFIFOReadPos;

void _sercom2_uart_isr()
{
    while(SERCOM2_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk)
    {
        pubSERCOM2UARTFIFO[usSERCOM2UARTFIFOWritePos++] = SERCOM2_REGS->USART_INT.SERCOM_DATA;

        if(usSERCOM2UARTFIFOWritePos >= SERCOM2_FIFO_SIZE)
            usSERCOM2UARTFIFOWritePos = 0;
    }
}

void sercom2_uart_init(uint32_t ulBaud, uint32_t ulFrameSettings, uint8_t ubRXPO, uint8_t ubTXPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM2_Msk, 1);

    while(SERCOM2_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    free((uint8_t *)pubSERCOM2UARTFIFO);

    pubSERCOM2UARTFIFO = (volatile uint8_t *)malloc(SERCOM2_FIFO_SIZE);

    if(!pubSERCOM2UARTFIFO)
        return;

    memset((uint8_t *)pubSERCOM2UARTFIFO, 0, SERCOM2_FIFO_SIZE);

    usSERCOM2UARTFIFOWritePos = 0;
    usSERCOM2UARTFIFOReadPos = 0;

    SERCOM2_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SWRST_Msk;

    while(SERCOM2_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_SWRST_Msk);

    SERCOM2_REGS->USART_INT.SERCOM_CTRLA = (ulFrameSettings & SERCOM_USART_INT_CTRLA_DORD_Msk) | SERCOM_USART_INT_CTRLA_CMODE_ASYNC | (ulFrameSettings & SERCOM_USART_INT_CTRLA_FORM_Msk) | SERCOM_USART_INT_CTRLA_RXPO(ubRXPO) | SERCOM_USART_INT_CTRLA_TXPO(ubTXPO) | SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK;
    SERCOM2_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_RXEN_Msk | SERCOM_USART_INT_CTRLB_TXEN_Msk | (ulFrameSettings & SERCOM_USART_INT_CTRLB_PMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_SBMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_CHSIZE_Msk);

    uint64_t ullBaud = 65536U * 100U - 65536ULL * 16ULL * 100ULL * (uint64_t)ulBaud / (uint64_t)GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM2_CORE_Val];
    ullBaud = DIV_ROUND(ullBaud, 100);

    if(ullBaud > 65535)
        ullBaud = 65535;

    SERCOM2_REGS->USART_INT.SERCOM_BAUD = (uint16_t)ullBaud;

    while(SERCOM2_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    IRQ_CLEAR(SERCOM2_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM2_IRQn, 2, 1); // Set priority 2,1
    IRQ_ENABLE(SERCOM2_IRQn); // Enable vector
    SERCOM2_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC_Msk; // Enable RXC flag

    SERCOM2_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

    while(SERCOM2_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);
}
void sercom2_uart_write_byte(const uint8_t ubData)
{
    while(!(SERCOM2_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk));

    SERCOM2_REGS->USART_INT.SERCOM_DATA = ubData;
}
uint8_t sercom2_uart_read_byte()
{
    if(!sercom2_uart_available())
        return 0;

    uint8_t ubData;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ubData = pubSERCOM2UARTFIFO[usSERCOM2UARTFIFOReadPos++];

        if(usSERCOM2UARTFIFOReadPos >= SERCOM2_FIFO_SIZE)
            usSERCOM2UARTFIFOReadPos = 0;
    }

    return ubData;
}
uint32_t sercom2_uart_available()
{
    return (SERCOM2_FIFO_SIZE + usSERCOM2UARTFIFOWritePos - usSERCOM2UARTFIFOReadPos) % SERCOM2_FIFO_SIZE;
}
void sercom2_uart_flush()
{
    usSERCOM2UARTFIFOReadPos = usSERCOM2UARTFIFOWritePos = 0;
}
#endif // SERCOM2_MODE_UART

void _sercom2_isr()
{
    #if defined(SERCOM2_MODE_I2C_SLAVE) && !defined(SERCOM2_MODE_UART)
        _sercom2_i2c_slave_isr();
    #elif !defined(SERCOM2_MODE_I2C_SLAVE) && defined(SERCOM2_MODE_UART)
        _sercom2_uart_isr();
    #elif defined(SERCOM2_MODE_I2C_SLAVE) && defined(SERCOM2_MODE_UART)
        switch(SERCOM2_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_MODE_Msk)
        {
            case SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK:
                _sercom2_uart_isr();
            break;
            case SERCOM_USART_INT_CTRLA_MODE_I2C_SLAVE:
                _sercom2_i2c_slave_isr();
            break;
        }
    #endif
}


#ifdef SERCOM3_MODE_I2C_MASTER
void sercom3_i2c_master_init(uint32_t ulSCLFreq, uint32_t ulTRise)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM3_Msk, 1);

    while(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_SWRST_Msk;

    while(SERCOM3_REGS->I2CM.SERCOM_CTRLA & SERCOM_I2CM_CTRLA_SWRST_Msk);

    SERCOM3_REGS->I2CM.SERCOM_CTRLA = SERCOM_I2CM_CTRLA_LOWTOUT_Msk | SERCOM_I2CM_CTRLA_INACTOUT_55US | SERCOM_I2CM_CTRLA_SDAHOLD_450NS | SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;

    uint32_t ulSourceFreq = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM3_CORE_Val];
    int32_t lBaud = (10U * ulSourceFreq / ulSCLFreq) - 100;

    if(ulTRise)
        lBaud -= ulSourceFreq / (100000000U / ulTRise);

    lBaud = DIV_ROUND(lBaud, 20);

    if(lBaud > 255)
        lBaud = 255;
    else if(lBaud <= 1)
        lBaud = 1;

    SERCOM3_REGS->I2CM.SERCOM_BAUD = (uint8_t)lBaud;

    while(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->I2CM.SERCOM_CTRLA |= SERCOM_I2CM_CTRLA_ENABLE_Msk;

    while(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->I2CM.SERCOM_STATUS = SERCOM_I2CM_STATUS_LOWTOUT_Msk | (0x01 << SERCOM_I2CM_STATUS_BUSSTATE_Pos) | SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk;

    while(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom3_i2c_master_transmit(uint8_t ubAddress, uint8_t *pubSrc, uint32_t ulCount, uint8_t ubStop)
{
    if((ubAddress & 1) && !ulCount)
        return 0;

    if((SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_BUSSTATE_Msk) == SERCOM_I2CM_STATUS_BUSSTATE(3))
        return 0;

    SERCOM3_REGS->I2CM.SERCOM_ADDR = ubAddress;

    if(!ulCount)
    {
        while(!(SERCOM3_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

        if(SERCOM3_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        if(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
        {
            SERCOM3_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

            while(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

            return 0;
        }
    }
    else
    {
        do
        {
            if(ubAddress & 1) // Read
            {
                while(!(SERCOM3_REGS->I2CM.SERCOM_INTFLAG & (SERCOM_I2CM_INTFLAG_MB_Msk | SERCOM_I2CM_INTFLAG_SB_Msk)));

                if(SERCOM3_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM3_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_SB_Msk)
                {
                    *pubSrc++ = SERCOM3_REGS->I2CM.SERCOM_DATA;

                    if(ulCount > 1)
                        SERCOM3_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(2);
                }
            }
            else // Write
            {
                while(!(SERCOM3_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));

                if(SERCOM3_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
                    return 0;

                if(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_RXNACK_Msk)
                {
                    SERCOM3_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_CMD(3);

                    while(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);

                    return 0;
                }

                SERCOM3_REGS->I2CM.SERCOM_DATA = *pubSrc++;

                while(!(SERCOM3_REGS->I2CM.SERCOM_INTFLAG & SERCOM_I2CM_INTFLAG_MB_Msk));
            }
        } while(--ulCount);
    }

    if(ubStop)
    {
        if(SERCOM3_REGS->I2CM.SERCOM_STATUS & (SERCOM_I2CM_STATUS_ARBLOST_Msk | SERCOM_I2CM_STATUS_BUSERR_Msk))
            return 0;

        SERCOM3_REGS->I2CM.SERCOM_CTRLB = SERCOM_I2CM_CTRLB_ACKACT_Msk | SERCOM_I2CM_CTRLB_CMD(3);

        while(SERCOM3_REGS->I2CM.SERCOM_STATUS & SERCOM_I2CM_STATUS_SYNCBUSY_Msk);
    }

    return 1;
}
#endif // SERCOM3_MODE_I2C_MASTER
#ifdef SERCOM3_MODE_I2C_SLAVE
static sercom_i2c_slave_addr_isr_t pfSERCOM3I2CSlaveAddrISR = NULL;
static sercom_i2c_slave_tx_data_isr_t pfSERCOM3I2CSlaveTXDataISR = NULL;
static sercom_i2c_slave_rx_data_isr_t pfSERCOM3I2CSlaveRXDataISR = NULL;

void _sercom3_i2c_slave_isr()
{
    static uint8_t ubLastWasAddress = 0;
    uint32_t ulFlags = SERCOM3_REGS->I2CS.SERCOM_INTFLAG;

    if(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk)
    {
        volatile uint8_t ubAddress = SERCOM3_REGS->I2CS.SERCOM_DATA;

        if(pfSERCOM3I2CSlaveAddrISR && pfSERCOM3I2CSlaveAddrISR(ubAddress))
            SERCOM3_REGS->I2CS.SERCOM_CTRLB &= ~SERCOM_I2CS_CTRLB_ACKACT_Msk;
        else
            SERCOM3_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk;

        SERCOM3_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_AMATCH_Msk;

        ubLastWasAddress = 1;
    }

    if(ulFlags & SERCOM_I2CS_INTFLAG_DRDY_Msk)
    {
        if(SERCOM3_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_DIR_Msk) // Master is trying to read from us
        {
            if(!ubLastWasAddress && (SERCOM3_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_RXNACK_Msk))
                SERCOM3_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_CMD(2);
            else
                SERCOM3_REGS->I2CS.SERCOM_DATA = pfSERCOM3I2CSlaveTXDataISR ? pfSERCOM3I2CSlaveTXDataISR() : 0xFF;
        }
        else // Master is trying to write to us, there is data available
        {
            volatile uint8_t ubData = SERCOM3_REGS->I2CS.SERCOM_DATA;

            if(pfSERCOM3I2CSlaveRXDataISR && pfSERCOM3I2CSlaveRXDataISR(ubData))
                SERCOM3_REGS->I2CS.SERCOM_CTRLB = (SERCOM3_REGS->I2CS.SERCOM_CTRLB & ~SERCOM_I2CS_CTRLB_ACKACT_Msk) | SERCOM_I2CS_CTRLB_CMD(3);
            else
                SERCOM3_REGS->I2CS.SERCOM_CTRLB |= SERCOM_I2CS_CTRLB_ACKACT_Msk | SERCOM_I2CS_CTRLB_CMD(2);
        }
    }

    if(!(ulFlags & SERCOM_I2CS_INTFLAG_AMATCH_Msk) && ubLastWasAddress)
        ubLastWasAddress = 0;
}

void sercom3_i2c_slave_init(uint8_t ubAddress)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM3_Msk, 1);

    while(SERCOM3_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_SWRST_Msk;

    while(SERCOM3_REGS->I2CS.SERCOM_CTRLA & SERCOM_I2CS_CTRLA_SWRST_Msk);

    SERCOM3_REGS->I2CS.SERCOM_CTRLA = SERCOM_I2CS_CTRLA_SDAHOLD_450NS | SERCOM_I2CS_CTRLA_RUNSTDBY_Msk | SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE;
    SERCOM3_REGS->I2CS.SERCOM_CTRLB = (0 << SERCOM_I2CS_CTRLB_AMODE_Pos);
    SERCOM3_REGS->I2CS.SERCOM_ADDR = SERCOM_I2CS_ADDR_ADDR(ubAddress);

    SERCOM3_REGS->I2CS.SERCOM_INTFLAG = SERCOM_I2CS_INTFLAG_Msk; // Clear all flags
    IRQ_CLEAR(SERCOM3_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM3_IRQn, 2, 0); // Set priority 2,0
    IRQ_ENABLE(SERCOM3_IRQn); // Enable vector
    SERCOM3_REGS->I2CS.SERCOM_INTENSET = SERCOM_I2CS_INTFLAG_DRDY_Msk | SERCOM_I2CS_INTFLAG_AMATCH_Msk; // Enable DRDY and AMATCH flags

    SERCOM3_REGS->I2CS.SERCOM_CTRLA |= SERCOM_I2CS_CTRLA_ENABLE_Msk;

    while(SERCOM3_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->I2CS.SERCOM_STATUS = SERCOM_I2CS_STATUS_Msk;

    while(SERCOM3_REGS->I2CS.SERCOM_STATUS & SERCOM_I2CS_STATUS_SYNCBUSY_Msk);
}
void sercom3_i2c_slave_set_addr_isr(sercom_i2c_slave_addr_isr_t pfISR)
{
    pfSERCOM3I2CSlaveAddrISR = pfISR;
}
void sercom3_i2c_slave_set_tx_data_isr(sercom_i2c_slave_tx_data_isr_t pfISR)
{
    pfSERCOM3I2CSlaveTXDataISR = pfISR;
}
void sercom3_i2c_slave_set_rx_data_isr(sercom_i2c_slave_rx_data_isr_t pfISR)
{
    pfSERCOM3I2CSlaveRXDataISR = pfISR;
}
#endif // SERCOM3_MODE_I2C_SLAVE
#ifdef SERCOM3_MODE_SPI
void sercom3_spi_init(uint32_t ulBaud, uint8_t ubMode, uint8_t ubBitMode, uint8_t ubDIPO, uint8_t ubDOPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM3_Msk, 1);

    while(SERCOM3_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_SWRST_Msk;

    while(SERCOM3_REGS->SPIM.SERCOM_CTRLA & SERCOM_SPIM_CTRLA_SWRST_Msk);

    SERCOM3_REGS->SPIM.SERCOM_CTRLA = (ubBitMode == SERCOM_SPI_MSB_FIRST ? 0 : SERCOM_SPIM_CTRLA_DORD_Msk) | (ubMode & 2 ? SERCOM_SPIM_CTRLA_CPOL_Msk : 0) | (ubMode & 1 ? SERCOM_SPIM_CTRLA_CPHA_Msk : 0) | SERCOM_SPIM_CTRLA_DIPO(ubDIPO) | SERCOM_SPIM_CTRLA_DOPO(ubDOPO) | SERCOM_SPIM_CTRLA_MODE_SPI_MASTER;

    while(SERCOM3_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->SPIM.SERCOM_CTRLB = SERCOM_SPIM_CTRLB_CHSIZE_8_BIT;
    SERCOM3_REGS->SPIM.SERCOM_BAUD = GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM3_CORE_Val] / (ulBaud << 1) - 1;

    while(SERCOM3_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);

    SERCOM3_REGS->SPIM.SERCOM_CTRLA |= SERCOM_SPIM_CTRLA_ENABLE_Msk;

    while(SERCOM3_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
}
uint8_t sercom3_spi_transfer_byte(const uint8_t ubData)
{
    while(!(SERCOM3_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(!(SERCOM3_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk))
    {
        SERCOM3_REGS->SPIM.SERCOM_CTRLB |= SERCOM_SPIM_CTRLB_RXEN_Msk;

        while(SERCOM3_REGS->SPIM.SERCOM_STATUS & SERCOM_SPIM_STATUS_SYNCBUSY_Msk);
    }

    SERCOM3_REGS->SPIM.SERCOM_DATA = ubData;

    while(!(SERCOM3_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_RXC_Msk));

    return SERCOM3_REGS->SPIM.SERCOM_DATA;
}
void sercom3_spi_write_byte(const uint8_t ubData, const uint8_t ubWait)
{
    while(!(SERCOM3_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_DRE_Msk));

    if(SERCOM3_REGS->SPIM.SERCOM_CTRLB & SERCOM_SPIM_CTRLB_RXEN_Msk)
        SERCOM3_REGS->SPIM.SERCOM_CTRLB &= ~SERCOM_SPIM_CTRLB_RXEN_Msk;

    SERCOM3_REGS->SPIM.SERCOM_DATA = ubData;

    while(ubWait && !(SERCOM3_REGS->SPIM.SERCOM_INTFLAG & SERCOM_SPIM_INTFLAG_TXC_Msk));
}
#endif // SERCOM3_MODE_SPI
#ifdef SERCOM3_MODE_UART
static volatile uint8_t *pubSERCOM3UARTFIFO = NULL;
static volatile uint16_t usSERCOM3UARTFIFOWritePos, usSERCOM3UARTFIFOReadPos;

void _sercom3_uart_isr()
{
    while(SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk)
    {
        pubSERCOM3UARTFIFO[usSERCOM3UARTFIFOWritePos++] = SERCOM3_REGS->USART_INT.SERCOM_DATA;

        if(usSERCOM3UARTFIFOWritePos >= SERCOM3_FIFO_SIZE)
            usSERCOM3UARTFIFOWritePos = 0;
    }
}

void sercom3_uart_init(uint32_t ulBaud, uint32_t ulFrameSettings, uint8_t ubRXPO, uint8_t ubTXPO)
{
    pm_apbc_clock_gate(PM_APBCMASK_SERCOM3_Msk, 1);

    while(SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    free((uint8_t *)pubSERCOM3UARTFIFO);

    pubSERCOM3UARTFIFO = (volatile uint8_t *)malloc(SERCOM3_FIFO_SIZE);

    if(!pubSERCOM3UARTFIFO)
        return;

    memset((uint8_t *)pubSERCOM3UARTFIFO, 0, SERCOM3_FIFO_SIZE);

    usSERCOM3UARTFIFOWritePos = 0;
    usSERCOM3UARTFIFOReadPos = 0;

    SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_SWRST_Msk;

    while(SERCOM3_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_SWRST_Msk);

    SERCOM3_REGS->USART_INT.SERCOM_CTRLA = (ulFrameSettings & SERCOM_USART_INT_CTRLA_DORD_Msk) | SERCOM_USART_INT_CTRLA_CMODE_ASYNC | (ulFrameSettings & SERCOM_USART_INT_CTRLA_FORM_Msk) | SERCOM_USART_INT_CTRLA_RXPO(ubRXPO) | SERCOM_USART_INT_CTRLA_TXPO(ubTXPO) | SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK;
    SERCOM3_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_RXEN_Msk | SERCOM_USART_INT_CTRLB_TXEN_Msk | (ulFrameSettings & SERCOM_USART_INT_CTRLB_PMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_SBMODE_Msk) | (ulFrameSettings & SERCOM_USART_INT_CTRLB_CHSIZE_Msk);

    uint64_t ullBaud = 65536U * 100U - 65536ULL * 16ULL * 100ULL * (uint64_t)ulBaud / (uint64_t)GCLK_CLOCK_FREQ[GCLK_CLKCTRL_ID_SERCOM3_CORE_Val];
    ullBaud = DIV_ROUND(ullBaud, 100);

    if(ullBaud > 65535)
        ullBaud = 65535;

    SERCOM3_REGS->USART_INT.SERCOM_BAUD = (uint16_t)ullBaud;

    while(SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);

    IRQ_CLEAR(SERCOM3_IRQn); // Clear pending vector
    IRQ_SET_PRIO(SERCOM3_IRQn, 2, 1); // Set priority 2,1
    IRQ_ENABLE(SERCOM3_IRQn); // Enable vector
    SERCOM3_REGS->USART_INT.SERCOM_INTENSET = SERCOM_USART_INT_INTENSET_RXC_Msk; // Enable RXC flag

    SERCOM3_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

    while(SERCOM3_REGS->USART_INT.SERCOM_STATUS & SERCOM_USART_INT_STATUS_SYNCBUSY_Msk);
}
void sercom3_uart_write_byte(const uint8_t ubData)
{
    while(!(SERCOM3_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk));

    SERCOM3_REGS->USART_INT.SERCOM_DATA = ubData;
}
uint8_t sercom3_uart_read_byte()
{
    if(!sercom3_uart_available())
        return 0;

    uint8_t ubData;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ubData = pubSERCOM3UARTFIFO[usSERCOM3UARTFIFOReadPos++];

        if(usSERCOM3UARTFIFOReadPos >= SERCOM3_FIFO_SIZE)
            usSERCOM3UARTFIFOReadPos = 0;
    }

    return ubData;
}
uint32_t sercom3_uart_available()
{
    return (SERCOM3_FIFO_SIZE + usSERCOM3UARTFIFOWritePos - usSERCOM3UARTFIFOReadPos) % SERCOM3_FIFO_SIZE;
}
void sercom3_uart_flush()
{
    usSERCOM3UARTFIFOReadPos = usSERCOM3UARTFIFOWritePos = 0;
}
#endif // SERCOM3_MODE_UART

void _sercom3_isr()
{
    #if defined(SERCOM3_MODE_I2C_SLAVE) && !defined(SERCOM3_MODE_UART)
        _sercom3_i2c_slave_isr();
    #elif !defined(SERCOM3_MODE_I2C_SLAVE) && defined(SERCOM3_MODE_UART)
        _sercom3_uart_isr();
    #elif defined(SERCOM3_MODE_I2C_SLAVE) && defined(SERCOM3_MODE_UART)
        switch(SERCOM3_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_MODE_Msk)
        {
            case SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK:
                _sercom3_uart_isr();
            break;
            case SERCOM_USART_INT_CTRLA_MODE_I2C_SLAVE:
                _sercom3_i2c_slave_isr();
            break;
        }
    #endif
}
