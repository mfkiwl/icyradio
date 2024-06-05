#include <sam.h>
#include <stdlib.h>
#include <math.h>
#include "debug_macros.h"
#include "utils.h"
#include "nvic.h"
#include "atomic.h"
#include "systick.h"
#include "sysctrl.h"
#include "gclk.h"
#include "pm.h"
#include "port.h"
#include "nvmctrl.h"
#include "adc.h"
#include "sercom.h"
#include "wdt.h"
#include "mcp4728.h"
#include "admv1013.h"
#include "admv1014.h"

// Structs
typedef struct __attribute((__packed__))
{
    uint8_t ubTest[2];
} i2c_rom_t;

// Helper macros
#define I2C_SLAVE_ADDRESS                           0x22
#define I2C_SLAVE_REGISTER_COUNT                    256
#define I2C_SLAVE_REGISTER(t, a)                    (*(t *)&ubI2CRegister[(a)])
#define I2C_SLAVE_REGISTER_WRITE_MASK(t, a)         (*(t *)&ubI2CRegisterWriteMask[(a)])
#define I2C_SLAVE_REGISTER_READ_MASK(t, a)          (*(t *)&ubI2CRegisterReadMask[(a)])
#define I2C_SLAVE_REGISTER_STATUS                   0x00 // 8-bit
#define I2C_SLAVE_REGISTER_CONFIG                   0x01 // 8-bit
#define I2C_SLAVE_REGISTER_IOVDD_VOLTAGE            0xC0 // 32-bit
#define I2C_SLAVE_REGISTER_CORE_VOLTAGE             0xC4 // 32-bit
#define I2C_SLAVE_REGISTER_TEMP                     0xD0 // 32-bit
#define I2C_SLAVE_REGISTER_UPTIME                   0xE0 // 64-bit
#define I2C_SLAVE_REGISTER_SW_VERSION               0xE8 // 16-bit
#define I2C_SLAVE_REGISTER_DEV_UID0                 0xF0 // 32-bit
#define I2C_SLAVE_REGISTER_DEV_UID1                 0xF4 // 32-bit
#define I2C_SLAVE_REGISTER_DEV_UID2                 0xF8 // 32-bit
#define I2C_SLAVE_REGISTER_DEV_UID3                 0xFC // 32-bit

// Forward declarations
static void reset() __attribute__((noreturn));
static void sleep();

static uint32_t get_free_ram();

static void get_device_core_name(char *pszDeviceCoreName, uint32_t ulDeviceCoreNameSize);
static void get_device_name(char *pszDeviceName, uint32_t ulDeviceNameSize);
static uint8_t get_device_revision();

static void wdt_warning_isr();

static void i2c_slave_register_init();
static uint8_t i2c_slave_addr_isr(uint8_t ubRnW);
static uint8_t i2c_slave_tx_data_isr();
static uint8_t i2c_slave_rx_data_isr(uint8_t ubData);

static void rf_gain_vctrl_init();
static void rf_gain_set_tx_vctrl(uint8_t ubChannel, uint32_t ulVoltageUv);
static uint32_t rf_gain_get_tx_vctrl(uint8_t ubChannel);
static void rf_gain_set_rx_vctrl(uint32_t ulVoltageUv);
static uint32_t rf_gain_get_rx_vctrl();

// Variables
const i2c_rom_t sI2CROM0 =
{
    .ubTest = {0x55, 0xAA},
};
volatile uint8_t ubI2CRegister[I2C_SLAVE_REGISTER_COUNT];
volatile uint8_t ubI2CRegisterWriteMask[I2C_SLAVE_REGISTER_COUNT];
volatile uint8_t ubI2CRegisterReadMask[I2C_SLAVE_REGISTER_COUNT];
volatile uint8_t ubI2CBuffer[I2C_SLAVE_REGISTER_COUNT];
volatile uint8_t ubI2CRegisterPointer = 0x00;
volatile uint8_t ubI2CByteCount = 0;
volatile uint8_t ubI2CReadSize = 0;
volatile uint8_t ubI2CReadSrc = 0; // 0 = Registers, 1 = ROM
volatile uint8_t ubI2CChecksum = 0;
volatile uint8_t ubI2CChecksumOK = 0;

// ISRs

// Functions
void reset()
{
    SCB->AIRCR = 0x05FA0000 | _VAL2FLD(SCB_AIRCR_SYSRESETREQ, 1);

    while(1);
}
void sleep()
{
    // rtcc_set_alarm(rtcc_get_time() + 5);

    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Configure Deep Sleep (EM2/3)

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // IRQ_CLEAR(RTCC_IRQn);

        __DMB(); // Wait for all memory transactions to finish before memory access
        __DSB(); // Wait for all memory transactions to finish before executing instructions
        __ISB(); // Wait for all memory transactions to finish before fetching instructions
        __SEV(); // Set the event flag to ensure the next WFE will be a NOP
        __WFE(); // NOP and clear the event flag
        __WFE(); // Wait for event
        __NOP(); // Prevent debugger crashes

        // cmu_init();
        // cmu_update_clocks();
    }
}

uint32_t get_free_ram()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        extern void *_sbrk(int);

        void *pCurrentHeap = _sbrk(1);

        if(!pCurrentHeap)
            return 0;

        uint32_t ulFreeRAM = (uint32_t)__get_MSP() - (uint32_t)pCurrentHeap;

        _sbrk(-1);

        return ulFreeRAM;
    }
}

void get_device_core_name(char *pszDeviceCoreName, uint32_t ulDeviceCoreNameSize)
{
    uint8_t ubImplementer = (SCB->CPUID & SCB_CPUID_IMPLEMENTER_Msk) >> SCB_CPUID_IMPLEMENTER_Pos;
    const char* szImplementer = "?";

    switch(ubImplementer)
    {
        case 0x41: szImplementer = "ARM"; break;
    }

    uint16_t usPartNo = (SCB->CPUID & SCB_CPUID_PARTNO_Msk) >> SCB_CPUID_PARTNO_Pos;
    const char* szPartNo = "?";

    switch(usPartNo)
    {
        case 0xC20: szPartNo = "Cortex-M0"; break;
        case 0xC60: szPartNo = "Cortex-M0+"; break;
        case 0xC21: szPartNo = "Cortex-M1"; break;
        case 0xD20: szPartNo = "Cortex-M23"; break;
        case 0xC23: szPartNo = "Cortex-M3"; break;
        case 0xD21: szPartNo = "Cortex-M33"; break;
        case 0xC24: szPartNo = "Cortex-M4"; break;
        case 0xC27: szPartNo = "Cortex-M7"; break;
    }

    uint8_t ubVariant = (SCB->CPUID & SCB_CPUID_VARIANT_Msk) >> SCB_CPUID_VARIANT_Pos;
    uint8_t ubRevision = (SCB->CPUID & SCB_CPUID_REVISION_Msk) >> SCB_CPUID_REVISION_Pos;

    snprintf(pszDeviceCoreName, ulDeviceCoreNameSize, "%s %s r%hhup%hhu", szImplementer, szPartNo, ubVariant, ubRevision);
}
void get_device_name(char *pszDeviceName, uint32_t ulDeviceNameSize)
{
    uint8_t ubSeries =  (DSU_REGS->DSU_DID & DSU_DID_SERIES_Msk) >> DSU_DID_SERIES_Pos;
    const char* szSeries = "?";

    switch(ubSeries)
    {
        case 0x00: szSeries = "SAMD20"; break;
        case 0x01: szSeries = "SAMD21"; break;
    }

    const char *szFlashSize = "?";

    switch(nvmctrl_get_flash_size() >> 10)
    {
        case  32: szFlashSize = "15"; break;
        case  64: szFlashSize = "16"; break;
        case 128: szFlashSize = "17"; break;
        case 256: szFlashSize = "18"; break;
    }

    uint8_t ubFamily =  (DSU_REGS->DSU_DID & DSU_DID_FAMILY_Msk) >> DSU_DID_FAMILY_Pos;
    char cFamily = '?';

    switch(ubFamily)
    {
        case 0x00: cFamily = 'A'; break;
        case 0x01: cFamily = 'B'; break;
    }

    snprintf(pszDeviceName, ulDeviceNameSize, "AT%sx%s%c", szSeries, szFlashSize, cFamily);
}
uint8_t get_device_revision()
{
    return (DSU_REGS->DSU_DID & DSU_DID_REVISION_Msk) >> DSU_DID_REVISION_Pos;
}

void wdt_warning_isr()
{
    DBGPRINTLN_CTX("Watchdog warning!");
}

void i2c_slave_register_init()
{
    uint32_t ulUniqueID[4];

    nvmctrl_get_unique_id(ulUniqueID);

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        I2C_SLAVE_REGISTER              (uint8_t,  I2C_SLAVE_REGISTER_STATUS)                   = 0x00;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint8_t,  I2C_SLAVE_REGISTER_STATUS)                   = 0x00;
        I2C_SLAVE_REGISTER_READ_MASK    (uint8_t,  I2C_SLAVE_REGISTER_STATUS)                   = 0xFF;
        I2C_SLAVE_REGISTER              (uint8_t,  I2C_SLAVE_REGISTER_CONFIG)                   = 0x00;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint8_t,  I2C_SLAVE_REGISTER_CONFIG)                   = 0xFF;
        I2C_SLAVE_REGISTER_READ_MASK    (uint8_t,  I2C_SLAVE_REGISTER_CONFIG)                   = 0xFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE)            = 0;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE)            = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE)            = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_CORE_VOLTAGE)             = 0;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_CORE_VOLTAGE)             = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_CORE_VOLTAGE)             = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (int32_t,  I2C_SLAVE_REGISTER_TEMP)                     = 0;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_TEMP)                     = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_TEMP)                     = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint64_t, I2C_SLAVE_REGISTER_UPTIME)                   = g_ullSystemTick / 1000;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint64_t, I2C_SLAVE_REGISTER_UPTIME)                   = 0x0000000000000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint64_t, I2C_SLAVE_REGISTER_UPTIME)                   = 0xFFFFFFFFFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint16_t, I2C_SLAVE_REGISTER_SW_VERSION)               = BUILD_VERSION;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint16_t, I2C_SLAVE_REGISTER_SW_VERSION)               = 0x0000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint16_t, I2C_SLAVE_REGISTER_SW_VERSION)               = 0xFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_DEV_UID0)                 = ulUniqueID[0];
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_DEV_UID0)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_DEV_UID0)                 = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_DEV_UID1)                 = ulUniqueID[1];
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_DEV_UID1)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_DEV_UID1)                 = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_DEV_UID2)                 = ulUniqueID[2];
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_DEV_UID2)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_DEV_UID2)                 = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_DEV_UID3)                 = ulUniqueID[3];
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_DEV_UID3)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_DEV_UID3)                 = 0xFFFFFFFF;
    }
}
uint8_t i2c_slave_addr_isr(uint8_t ubAddress)
{
    ubI2CByteCount = 0;
    ubI2CChecksum = I2C_SLAVE_ADDRESS;

    // Hardware address comparator already verifies if the address matches
    // This is only called if address is valid
    // All we need to do is ACK

    return 1; // ACK
}
uint8_t i2c_slave_tx_data_isr()
{
    if(!ubI2CChecksumOK)
        return 0xFF;

    if(ubI2CByteCount > ubI2CReadSize + 1)
        return 0xFF;

    ubI2CByteCount++;

    if(ubI2CByteCount == ubI2CReadSize + 1)
    {
        ubI2CChecksumOK = 0;
        ubI2CReadSize = 0;
        ubI2CReadSrc = 0;

        return (0xFF - ubI2CChecksum) + 1;
    }

    uint8_t ubData = 0xFF;

    if(ubI2CReadSrc == 0)
        ubData = ubI2CRegister[ubI2CRegisterPointer] & ubI2CRegisterReadMask[ubI2CRegisterPointer];
    else if(ubI2CRegisterPointer < sizeof(sI2CROM0))
        ubData = ((const uint8_t *)&sI2CROM0)[ubI2CRegisterPointer];

    ubI2CRegisterPointer++;

    ubI2CChecksum += ubData;

    // switch(ubI2CRegisterPointer)
    // {
    //     case I2C_SLAVE_REGISTER_RF_OUT_PWR_STATUS + sizeof(uint8_t):
    //     {
    //         if(ubI2CByteCount < sizeof(uint8_t))
    //             break;

    //         I2C_SLAVE_REGISTER(uint8_t, I2C_SLAVE_REGISTER_RF_OUT_PWR_STATUS) &= ~BIT(0); // Clear on read
    //     }
    //     break;
    // }

    return ubData;
}
uint8_t i2c_slave_rx_data_isr(uint8_t ubData)
{
    ubI2CBuffer[ubI2CByteCount++] = ubData;
    ubI2CChecksum += ubData;

    if(ubI2CByteCount < 2)
        return 1; // ACK

    uint8_t ubExpectedLength = 3;

    if(ubI2CBuffer[1] > I2C_SLAVE_REGISTER_COUNT - 3)
        ubExpectedLength++;
    else
        ubExpectedLength += ubI2CBuffer[1];

    if(ubI2CByteCount < ubExpectedLength)
        return 1; // ACK

    ubI2CChecksum -= ubData;
    ubI2CChecksum = (0xFF - ubI2CChecksum) + 1;

    if(ubI2CChecksum != ubI2CBuffer[ubI2CByteCount - 1])
    {
        ubI2CByteCount = 0;
        ubI2CChecksum = I2C_SLAVE_ADDRESS;

        return 0; // NAK
    }

    ubI2CByteCount = 0;
    ubI2CChecksum = I2C_SLAVE_ADDRESS;
    ubI2CRegisterPointer = ubI2CBuffer[0];

    if(ubI2CBuffer[1] > I2C_SLAVE_REGISTER_COUNT - 3)
    {
        ubI2CChecksumOK = 1;
        ubI2CReadSize = ubI2CBuffer[2];

        if(ubI2CBuffer[1] == I2C_SLAVE_REGISTER_COUNT - 2)
            ubI2CReadSrc = 1;
        else
            ubI2CReadSrc = 0;

        return 1; // ACK
    }

    for(uint8_t i = 0; i < ubI2CBuffer[1]; i++)
    {
        ubI2CRegister[ubI2CRegisterPointer] = (ubI2CRegister[ubI2CRegisterPointer] & ~ubI2CRegisterWriteMask[ubI2CRegisterPointer]) | (ubI2CBuffer[2 + i] & ubI2CRegisterWriteMask[ubI2CRegisterPointer]);
        ubI2CRegisterPointer++;

        switch(ubI2CRegisterPointer)
        {
            // case REG + sizeof(uint8_t):
            // {
            //     if(ubI2CBuffer[1] < sizeof(uint8_t))
            //         break;

            //     var |= I2C_SLAVE_REGISTER(uint8_t, REG);
            // }
            // break;
        }
    }

    return 1; // ACK
}

void rf_gain_vctrl_init()
{
    // Set minimum gain
    rf_gain_set_tx_vctrl(0, 0);
    rf_gain_set_tx_vctrl(1, 0);

    // Check if RX is powered because min. RX gain = max voltage (1.8V)
    rf_gain_set_rx_vctrl(RX_ANA_PWR_STATUS() ? 1800000 : 0);
}
void rf_gain_set_tx_vctrl(uint8_t ubChannel, uint32_t ulVoltageUv)
{
    if(ubChannel > 1)
        return;

    if(ulVoltageUv > 1800000)
        ulVoltageUv = 1800000;

    DBGPRINTLN_CTX("Set TX VCTRL%hhu to %lu uV", ubChannel + 1, ulVoltageUv);

    ulVoltageUv /= 100; // Resolution is 0.5 mV, by removing the last two digits we avoid using uint64_t
    ulVoltageUv = ulVoltageUv * 4096 / (MCP4728_INTERNAL_REF_UV / 100);

    if(ubChannel == 0)
        ubChannel = 3; // VCTRL1 is on channel 3
    else if(ubChannel == 1)
        ubChannel = 2; // VCTRL2 is on channel 2

    mcp4728_channel_write(ubChannel, MCP4728_CHAN_VREF_INTERNAL | MCP4728_CHAN_PD_NORMAL | MCP4728_CHAN_GAIN_X1 | (ulVoltageUv & 0x0FFF));
}
uint32_t rf_gain_get_tx_vctrl(uint8_t ubChannel)
{
    if(ubChannel > 1)
        return 0;

    if(ubChannel == 0)
        ubChannel = 3; // VCTRL1 is on channel 3
    else if(ubChannel == 1)
        ubChannel = 2; // VCTRL2 is on channel 2

    uint32_t ulVoltageUv = mcp4728_channel_read(ubChannel) & 0x0FFF;

    ulVoltageUv = ulVoltageUv * ((MCP4728_INTERNAL_REF_UV / 100) / 4096);

    return ulVoltageUv;
}
void rf_gain_set_rx_vctrl(uint32_t ulVoltageUv)
{
    if(ulVoltageUv > 1800000)
        ulVoltageUv = 1800000;

    DBGPRINTLN_CTX("Set RX VCTRL to %lu uV", ulVoltageUv);

    ulVoltageUv /= 100; // Resolution is 0.5 mV, by removing the last two digits we avoid using uint64_t
    ulVoltageUv = ulVoltageUv * 4096 / (MCP4728_INTERNAL_REF_UV / 100);

    mcp4728_channel_write(0, MCP4728_CHAN_VREF_INTERNAL | MCP4728_CHAN_PD_NORMAL | MCP4728_CHAN_GAIN_X1 | (ulVoltageUv & 0x0FFF));
}
uint32_t rf_gain_get_rx_vctrl()
{
    uint32_t ulVoltageUv = mcp4728_channel_read(0) & 0x0FFF;

    ulVoltageUv = ulVoltageUv * ((MCP4728_INTERNAL_REF_UV / 100) / 4096);

    return ulVoltageUv;
}

int init()
{
    nvmctrl_init(); // Init NVMCTRL

    sysctrl_init(); // Init Oscillators
    gclk_init(); // Init GCLKs
    pm_init(); // Init System clock tree

    systick_init(); // Init system tick

    wdt_init(WDT_CONFIG_PER_16K, WDT_EWCTRL_EWOFFSET_4K); // Init the watchdog time (500 ms timeout, 125 ms warning, typical)
    wdt_set_warning_isr(wdt_warning_isr);

    port_init(); // Init GPIOs
    adc_init(); // Init ADC

    sercom0_spi_init(3000000, 0, SERCOM_SPI_MSB_FIRST, 1, 1);
    sercom2_i2c_master_init(SERCOM_I2C_FAST, SERCOM_I2C_TRISE_DEFAULT);
    sercom3_uart_init(500000, SERCOM_USART_INT_CTRLA_DORD_LSB | SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY | SERCOM_USART_INT_CTRLB_SBMODE_1_BIT | SERCOM_SPIM_CTRLB_CHSIZE_8_BIT, 0, 1);

    sercom1_i2c_slave_init(I2C_SLAVE_ADDRESS);
    sercom1_i2c_slave_set_addr_isr(i2c_slave_addr_isr);
    sercom1_i2c_slave_set_tx_data_isr(i2c_slave_tx_data_isr);
    sercom1_i2c_slave_set_rx_data_isr(i2c_slave_rx_data_isr);

    char szDeviceCoreName[32];
    char szDeviceName[32];
    uint32_t ulUniqueID[4];

    get_device_core_name(szDeviceCoreName, 32);
    get_device_name(szDeviceName, 32);
    nvmctrl_get_unique_id(ulUniqueID);

    printf("\x1B[2J\x1B[2J\x1B[2J\x1B[2J"); // Clear the screen
    printf("\x1B[H"); // Move cursor to top left corner

    DBGPRINTLN_CTX("IcyRadio mmW Int. Exp. Controller v%lu (%s %s)!", BUILD_VERSION, __DATE__, __TIME__);
    DBGPRINTLN_CTX("Core: %s", szDeviceCoreName);
    DBGPRINTLN_CTX("Device: %s", szDeviceName);
    DBGPRINTLN_CTX("Revision: %hhu", get_device_revision());
    DBGPRINTLN_CTX("Cal Temp: %hhu.%hhu C", (TEMP_LOG_FUSES_REGS->FUSES_TEMP_LOG_WORD_0 & FUSES_TEMP_LOG_WORD_0_ROOM_TEMP_VAL_INT_Msk) >> FUSES_TEMP_LOG_WORD_0_ROOM_TEMP_VAL_INT_Pos, (TEMP_LOG_FUSES_REGS->FUSES_TEMP_LOG_WORD_0 & FUSES_TEMP_LOG_WORD_0_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_TEMP_LOG_WORD_0_ROOM_TEMP_VAL_DEC_Pos);
    DBGPRINTLN_CTX("Flash: %hu kB", nvmctrl_get_flash_size() >> 10);
    DBGPRINTLN_CTX("RAM: %hu kB (Free: %lu B)", HMCRAMC0_SIZE >> 10, get_free_ram());
    DBGPRINTLN_CTX("Unique ID: %08X-%08X-%08X-%08X", ulUniqueID[0], ulUniqueID[1], ulUniqueID[2], ulUniqueID[3]);

    DBGPRINTLN_CTX("RMU:");
    DBGPRINTLN_CTX("  Reset cause: %s", pm_get_reset_reason_string(pm_get_reset_reason()));

    int32_t lTemp = adc_get_temperature();

    DBGPRINTLN_CTX("ADC:");
    DBGPRINTLN_CTX("  IOVDD: %u mV", adc_get_iovdd());
    DBGPRINTLN_CTX("  Core: %u mV", adc_get_corevdd());
    DBGPRINTLN_CTX("  Temp: %d.%02u C", lTemp / 1000, ABS(lTemp % 1000));

    nvmctrl_config_waitstates(PM_CPU_CLOCK_FREQ, adc_get_iovdd()); // Optimize flash wait states for frequency and voltage

    DBGPRINTLN_CTX("SYSCTRL:");
    DBGPRINTLN_CTX("  XOSC: %u Hz", SYSCTRL_XOSC_CLOCK_FREQ);
    DBGPRINTLN_CTX("  OSC8M: %u Hz", SYSCTRL_OSC8M_CLOCK_FREQ);
    DBGPRINTLN_CTX("  DFLL48M: %u Hz", SYSCTRL_DFLL48M_CLOCK_FREQ);

    DBGPRINTLN_CTX("PM:");
    DBGPRINTLN_CTX("  MAIN: %u Hz", PM_MAIN_CLOCK_FREQ);
    DBGPRINTLN_CTX("  CPU: %u Hz", PM_CPU_CLOCK_FREQ);
    DBGPRINTLN_CTX("  AHB: %u Hz", PM_AHB_CLOCK_FREQ);
    DBGPRINTLN_CTX("  APBA: %u Hz", PM_APBA_CLOCK_FREQ);
    DBGPRINTLN_CTX("  APBB: %u Hz", PM_APBB_CLOCK_FREQ);
    DBGPRINTLN_CTX("  APBC: %u Hz", PM_APBC_CLOCK_FREQ);

    DBGPRINTLN_CTX("WDT:");
    DBGPRINTLN_CTX("  Timeout: %u ns", wdt_get_timeout_period());
    DBGPRINTLN_CTX("  Warn: %u ns", wdt_get_warning_period());

    DBGPRINTLN_CTX("Scanning I2C bus 2...");

    for(uint8_t a = 0x08; a < 0x78; a++)
        if(sercom2_i2c_master_write(a, NULL, 0, SERCOM_I2C_STOP))
            DBGPRINTLN_CTX("  Address 0x%02X ACKed!", a);

    if(mcp4728_init())
        DBGPRINTLN_CTX("MCP4728 init OK!");
    else
        DBGPRINTLN_CTX("MCP4728 init NOK!");

    TX_DIG_PWR_EN();
    TX_ANA_PWR_EN();
    delay_ms(50);

    if(admv1013_init())
    {
        admv1013_power_down();

        DBGPRINTLN_CTX("ADMV1013 rev. %hhu init OK!", admv1013_get_revision());
    }
    else
    {
        TX_ANA_PWR_DIS();
        TX_DIG_PWR_DIS();

        DBGPRINTLN_CTX("ADMV1013 init NOK!");
    }

    RX_DIG_PWR_EN();
    RX_ANA_PWR_EN();
    delay_ms(50);

    if(admv1014_init())
    {
        // admv1014_power_down();

        DBGPRINTLN_CTX("ADMV1014 rev. %hhu init OK!", admv1014_get_revision());
    }
    else
    {
        RX_ANA_PWR_DIS();
        RX_DIG_PWR_DIS();

        DBGPRINTLN_CTX("ADMV1014 init NOK!");
    }

    return 0;
}
int main()
{
    // I2C Slave Register block
    i2c_slave_register_init();

    // Gain DAC
    rf_gain_vctrl_init();

    wdt_enable();

    while(1)
    {
        wdt_feed();

        if(I2C_SLAVE_REGISTER(uint8_t, I2C_SLAVE_REGISTER_CONFIG) & BIT(7))
        {
            delay_ms(20);

            reset();
        }

        if(I2C_SLAVE_REGISTER(uint64_t, I2C_SLAVE_REGISTER_UPTIME) != g_ullSystemTick / 1000)
        {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                I2C_SLAVE_REGISTER(uint64_t, I2C_SLAVE_REGISTER_UPTIME) = g_ullSystemTick / 1000;
            }
        }

        static uint64_t ullLastHeartBeat = 0;
        static uint64_t ullLastTelemetryUpdate = 0;

        if((g_ullSystemTick > 0 && ullLastHeartBeat == 0) || g_ullSystemTick - ullLastHeartBeat > 1000)
        {
            ullLastHeartBeat = g_ullSystemTick;

            LED_TOGGLE();

            if(LED_STATUS())
                ullLastHeartBeat -= 900;
        }

        if((g_ullSystemTick > 0 && ullLastTelemetryUpdate == 0) || g_ullSystemTick - ullLastTelemetryUpdate > 5000)
        {
            ullLastTelemetryUpdate = g_ullSystemTick;

            // System Temperatures
            int32_t lTemp = adc_get_temperature();

            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                I2C_SLAVE_REGISTER(int32_t, I2C_SLAVE_REGISTER_TEMP) = lTemp;
            }

            // System Voltages/Currents
            uint32_t ulIOVDD = adc_get_iovdd();
            uint32_t ulCoreVDD = adc_get_corevdd();

            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE) = ulIOVDD;
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_CORE_VOLTAGE) = ulCoreVDD;
            }

            // RF Power Detector Voltages
            uint32_t ulTXPwr = adc_get_tx_pwr_vdet();
            uint32_t ulRXPwr = adc_get_rx_pwr_vdet();

            if(DBG_PRESENT())
            {
                DBGPRINTLN("------------------------------");
                DBGPRINTLN("IOVDD: %u mV", ulIOVDD);
                DBGPRINTLN("Core: %u mV", ulCoreVDD);
                DBGPRINTLN("Temp: %d.%02u C", lTemp / 1000, ABS(lTemp % 1000));
                DBGPRINTLN("TX Pwr: %u uV", ulTXPwr);
                DBGPRINTLN("RX Pwr: %u uV", ulRXPwr);
            }
        }
    }

    return 0;
}