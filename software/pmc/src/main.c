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
#include "lt7182s.h"

// Structs
typedef struct __attribute((__packed__))
{
    uint8_t ubTest[2];
} i2c_rom_t;

// Helper macros
#define I2C_SLAVE_ADDRESS                           0x26
#define I2C_SLAVE_REGISTER_COUNT                    256
#define I2C_SLAVE_REGISTER(t, a)                    (*(t *)&ubI2CRegister[(a)])
#define I2C_SLAVE_REGISTER_WRITE_MASK(t, a)         (*(t *)&ubI2CRegisterWriteMask[(a)])
#define I2C_SLAVE_REGISTER_READ_MASK(t, a)          (*(t *)&ubI2CRegisterReadMask[(a)])
#define I2C_SLAVE_REGISTER_STATUS                   0x00 // 8-bit
#define I2C_SLAVE_REGISTER_CONFIG                   0x01 // 8-bit
#define I2C_SLAVE_REGISTER_VIN_VOLTAGE              0xB0 // 32-bit
#define I2C_SLAVE_REGISTER_VEXT_VOLTAGE             0xB4 // 32-bit
#define I2C_SLAVE_REGISTER_12V0_VOLTAGE             0xB8 // 32-bit
#define I2C_SLAVE_REGISTER_VBUS_VOLTAGE             0xBC // 32-bit
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

static uint8_t pmc_check_input_voltages();

static void i2c_slave_register_init();
static uint8_t i2c_slave_addr_isr(uint8_t ubRnW);
static uint8_t i2c_slave_tx_data_isr();
static uint8_t i2c_slave_rx_data_isr(uint8_t ubData);

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
uint32_t ulVIN = 0;
uint32_t ulVEXT = 0;
uint32_t ul12V0 = 0;
uint32_t ulVBUS = 0;

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

uint8_t pmc_check_input_voltages()
{
    ulVIN = adc_get_vin();
    ulVEXT = adc_get_vext();

    if(!VEXT_STATUS() && ulVEXT > 7000 && ulVEXT > ulVIN)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            PCIE_12V0_DISABLE();
            VBUS_SNK_DISABLE();

            VEXT_ENABLE();
        }

        return 1;
    }

    ul12V0 = adc_get_12v0();

    if(!PCIE_12V0_STATUS() && ul12V0 > 7000 && ul12V0 > ulVIN)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            VBUS_SNK_DISABLE();
            VEXT_DISABLE();

            PCIE_12V0_ENABLE();
        }

        return 1;
    }

    ulVBUS = adc_get_vbus();

    if(!VBUS_SNK_STATUS() && ulVBUS > 7000 && ulVBUS > ulVIN)
    {
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            PCIE_12V0_DISABLE();
            VEXT_DISABLE();

            VBUS_SNK_ENABLE();
        }

        return 1;
    }

    return 0;
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
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_VIN_VOLTAGE)              = 0;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_VIN_VOLTAGE)              = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_VIN_VOLTAGE)              = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_VEXT_VOLTAGE)             = 0;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_VEXT_VOLTAGE)             = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_VEXT_VOLTAGE)             = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_12V0_VOLTAGE)             = 0;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_12V0_VOLTAGE)             = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_12V0_VOLTAGE)             = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_VBUS_VOLTAGE)             = 0;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_VBUS_VOLTAGE)             = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_VBUS_VOLTAGE)             = 0xFFFFFFFF;
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

    sercom0_spi_init(1000000, 0, SERCOM_SPI_MSB_FIRST, 0, 2);
    sercom1_i2c_master_init(SERCOM_I2C_FAST, SERCOM_I2C_TRISE_DEFAULT);
    sercom3_uart_init(500000, SERCOM_USART_INT_CTRLA_DORD_LSB | SERCOM_USART_INT_CTRLA_FORM_USART_FRAME_NO_PARITY | SERCOM_USART_INT_CTRLB_SBMODE_1_BIT | SERCOM_SPIM_CTRLB_CHSIZE_8_BIT, 0, 1);

    // sercom1_i2c_slave_init(I2C_SLAVE_ADDRESS); // Initialized as slave later, after LT7182S startup is done
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

    DBGPRINTLN_CTX("IcyRadio PMC v%lu (%s %s)!", BUILD_VERSION, __DATE__, __TIME__);
    DBGPRINTLN_CTX("Core: %s", szDeviceCoreName);
    DBGPRINTLN_CTX("Device: %s", szDeviceName);
    DBGPRINTLN_CTX("Revision: %hhu", get_device_revision());
    DBGPRINTLN_CTX("Cal Temp: %hhu.%hhu C", (TEMP_LOG_FUSES_REGS->FUSES_TEMP_LOG_WORD_0 & FUSES_TEMP_LOG_WORD_0_NVMCTRL_ROOM_TEMP_VAL_INT_Msk) >> FUSES_TEMP_LOG_WORD_0_NVMCTRL_ROOM_TEMP_VAL_INT_Pos, (TEMP_LOG_FUSES_REGS->FUSES_TEMP_LOG_WORD_0 & FUSES_TEMP_LOG_WORD_0_NVMCTRL_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_TEMP_LOG_WORD_0_NVMCTRL_ROOM_TEMP_VAL_DEC_Pos);
    DBGPRINTLN_CTX("Flash: %hu kB", nvmctrl_get_flash_size() >> 10);
    DBGPRINTLN_CTX("RAM: %hu kB (Free: %lu B)", HRAMC0_SIZE >> 10, get_free_ram());
    DBGPRINTLN_CTX("Unique ID: %08X-%08X-%08X-%08X", ulUniqueID[0], ulUniqueID[1], ulUniqueID[2], ulUniqueID[3]);

    DBGPRINTLN_CTX("RMU:");
    DBGPRINTLN_CTX("  Reset cause: %s", pm_get_reset_reason_string(pm_get_reset_reason()));

    int32_t lTemp = adc_get_temperature();

    DBGPRINTLN_CTX("ADC:");
    DBGPRINTLN_CTX("  IOVDD: %u mV", adc_get_iovdd());
    DBGPRINTLN_CTX("  Core: %u mV", adc_get_corevdd());
    DBGPRINTLN_CTX("  VIN: %u mV", adc_get_vin());
    DBGPRINTLN_CTX("  VEXT: %u mV", adc_get_vext());
    DBGPRINTLN_CTX("  12V0: %u mV", adc_get_12v0());
    DBGPRINTLN_CTX("  VBUS: %u mV", adc_get_vbus());
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

    // DBGPRINTLN_CTX("Scanning I2C bus 1...");

    // for(uint8_t a = 0x08; a < 0x78; a++)
    //     if(sercom1_i2c_master_write(a, NULL, 0, SERCOM_I2C_STOP))
    //         DBGPRINTLN_CTX("  Address 0x%02X ACKed!", a);

    if(lt7182s_init())
        DBGPRINTLN_CTX("LT7182S init OK!");
    else
        DBGPRINTLN_CTX("LT7182S init NOK!");

    return 0;
}
int main()
{
    // I2C Slave Register block
    i2c_slave_register_init();

    char szMFRBuf[8];
    uint8_t ubMFRBuf[16];
    uint8_t ubMFRLen = 0;

    DBGPRINTLN_CTX("LT7182S:");

    if(!lt7182s_read_mfr_id(szMFRBuf, sizeof(szMFRBuf)))
        DBGPRINTLN_CTX("Failed to read manufacturer ID!");
    else
        DBGPRINTLN_CTX("  MFR ID: %s", szMFRBuf);

    if(!lt7182s_read_mfr_model(szMFRBuf, sizeof(szMFRBuf)))
        DBGPRINTLN_CTX("Failed to read manufacturer model!");
    else
        DBGPRINTLN_CTX("  MFR Model: %s", szMFRBuf);

    if(!(ubMFRLen = lt7182s_read_mfr_revision(ubMFRBuf, sizeof(ubMFRBuf))))
    {
        DBGPRINTLN_CTX("Failed to read manufacturer revision!");
    }
    else
    {
        DBGPRINT_CTX("  MFR Revision: ");

        for(uint8_t i = 0; i < ubMFRLen; i++)
            DBGPRINT("%02X", ubMFRBuf[i]);

        DBGPRINTLN("");
    }

    if(!(ubMFRLen = lt7182s_read_mfr_serial(ubMFRBuf, sizeof(ubMFRBuf))))
    {
        DBGPRINTLN_CTX("Failed to read manufacturer serial!");
    }
    else
    {
        DBGPRINT_CTX("  MFR Serial: ");

        for(uint8_t i = 0; i < ubMFRLen; i++)
            DBGPRINT("%02X", ubMFRBuf[i]);

        DBGPRINTLN("");
    }

    DBGPRINTLN_CTX("  MFR Special ID: 0x%04X", lt7182s_read_mfr_special_id());
    DBGPRINTLN_CTX("  MFR Fault Log Timestamp: %llu", lt7182s_mfr_fault_log_get_timestamp());

    // Configure Channel 0
    lt7182s_set_operation(0, LT7182S_OPERATION_TURN_OFF_IMMED);
    lt7182s_set_on_off_config(0, BIT(3)); // Only care about OPERATION commands, ignore RUN pin
    lt7182s_set_pwm_config(0, (3 << 11) | (2 << 9) | (7 << 6) | (3 << 3) | BIT(2)); // GMEA = 150 uS, ILIM = 6.5 A / -4 A, CITH = 80 pF, RITH = 40 kOhm, FCM, Low VOUT Disabled
    lt7182s_set_vin_on(0, 7.f); // 7 V Input Voltage to start power conversion
    lt7182s_set_vin_off(0, 6.5f); // 6.5 V Input Voltage to stop power conversion (UVLO)
    lt7182s_set_vin_uv_warn(0, 6.75f); // 6.75 V Input Undervoltage Warning
    lt7182s_set_iin_oc_warn(0, 4.5f); // 4.5 A Input Overcurrent Warning
    lt7182s_set_vout_max(0, 5.4f); // 5.4 V Maximum Output Voltage
    lt7182s_set_vout(0, 5.2f); // 5.2 V Nominal Output Voltage
    lt7182s_set_vout_margin_high(0, 5.25f); // 5.25 V High Margin
    lt7182s_set_vout_margin_low(0, 5.15f); // 5.15 V Low Margin
    lt7182s_set_vout_ov_fault(0, 5.5f); // 5.5 V Overvoltage Fault
    lt7182s_set_vout_ov_warn(0, 5.4f); // 5.4 V Overvoltage Warning
    lt7182s_set_vout_uv_fault(0, 4.95f); // 4.95 V Undervoltage Fault
    lt7182s_set_vout_uv_warn(0, 5.05f); // 5.05 V Undervoltage Warning
    lt7182s_set_iout_oc_warn(0, 5.5f); // 5.5 A Output Overcurrent Warning

    // Configure Channel 1
    lt7182s_set_operation(1, LT7182S_OPERATION_TURN_OFF_IMMED);
    lt7182s_set_on_off_config(1, BIT(3) | BIT(2) | BIT(0)); // Only start if OPERATION is on and RUN is high (PGOOD from previous regulator)
    lt7182s_set_pwm_config(1, (0 << 11) | (2 << 9) | (7 << 6) | (3 << 3) | BIT(2) | BIT(1)); // GMEA = 150 uS, ILIM = 6.5 A / -4 A, CITH = 80 pF, RITH = 40 kOhm, FCM, Low VOUT Enabled
    lt7182s_set_vin_on(1, 7.f); // 7 V Input Voltage to start power conversion
    lt7182s_set_vin_off(1, 6.5f); // 6.5 V Input Voltage to stop power conversion (UVLO)
    lt7182s_set_vin_uv_warn(1, 6.75f); // 6.75 V Input Undervoltage Warning
    lt7182s_set_iin_oc_warn(1, 0.8f); // 0.8 A Input Overcurrent Warning
    lt7182s_set_vout_max(1, 1.1f); // 1.1 V Maximum Output Voltage
    lt7182s_set_vout(1, 1.f); // 1 V Nominal Output Voltage
    lt7182s_set_vout_margin_high(1, 1.05f); // 1.05 V High Margin
    lt7182s_set_vout_margin_low(1, 1.f); // 1 V Low Margin
    lt7182s_set_vout_ov_fault(1, 1.15f); // 1.15 V Overvoltage Fault
    lt7182s_set_vout_ov_warn(1, 1.1f); // 1.1 V Overvoltage Warning
    lt7182s_set_vout_uv_fault(1, 0.9f); // 0.9 V Undervoltage Fault
    lt7182s_set_vout_uv_warn(1, 0.95f); // 0.95 V Undervoltage Warning
    lt7182s_set_iout_oc_warn(1, 3.f); // 3 A Output Overcurrent Warning TODO: Up up

    for(uint8_t i = 0; i < 2; i++)
    {
        DBGPRINTLN_CTX("  Channel #%hhu:", i);
        DBGPRINTLN_CTX("    VIN: %.3f V (Pk: %.3f V)", lt7182s_read_vin(i), lt7182s_read_vin_peak(i));
        DBGPRINTLN_CTX("    VIN ON: %.3f V", lt7182s_get_vin_on(i));
        DBGPRINTLN_CTX("    VIN OFF: %.3f V", lt7182s_get_vin_off(i));
        DBGPRINTLN_CTX("    VIN UV Warn: %.3f V", lt7182s_get_vin_uv_warn(i));
        DBGPRINTLN_CTX("    IIN: %.3f A", lt7182s_read_iin(i));
        DBGPRINTLN_CTX("    IIN OC Warn: %.3f A", lt7182s_get_iin_oc_warn(i));
        DBGPRINTLN_CTX("    VOUT: %.3f V (Set: %.3f V, Pk: %.3f V)", lt7182s_read_vout(i), lt7182s_get_vout(i), lt7182s_read_vout_peak(i));
        DBGPRINTLN_CTX("    VOUT Max: %.3f V", lt7182s_get_vout_max(i));
        DBGPRINTLN_CTX("    VOUT Mg High: %.3f V", lt7182s_get_vout_margin_high(i));
        DBGPRINTLN_CTX("    VOUT Mg Low: %.3f V", lt7182s_get_vout_margin_low(i));
        DBGPRINTLN_CTX("    VOUT Tr Rate: %.3f V/ms", lt7182s_get_vout_transition_rate(i));
        DBGPRINTLN_CTX("    VOUT OV Fault: %.3f V", lt7182s_get_vout_ov_fault(i));
        DBGPRINTLN_CTX("    VOUT OV Warn: %.3f V", lt7182s_get_vout_ov_warn(i));
        DBGPRINTLN_CTX("    VOUT UV Fault: %.3f V", lt7182s_get_vout_uv_fault(i));
        DBGPRINTLN_CTX("    VOUT UV Warn: %.3f V", lt7182s_get_vout_uv_warn(i));
        DBGPRINTLN_CTX("    IOUT: %.3f A (Pk: %.3f A)", lt7182s_read_iout(i), lt7182s_read_iout_peak(i));
        DBGPRINTLN_CTX("    IOUT OC Warn: %.3f A", lt7182s_get_iout_oc_warn(i));
        DBGPRINTLN_CTX("    FREQ: %.3f kHz", lt7182s_read_freq(i));
        DBGPRINTLN_CTX("    POUT: %.3f W", lt7182s_read_pout(i));
        DBGPRINTLN_CTX("    VITH: %.3f V", lt7182s_read_ith(i));

        uint16_t usPWMConfig = lt7182s_get_pwm_config(i);

        DBGPRINTLN_CTX("    PWM Config:");
        DBGPRINTLN_CTX("      GMEA: %.3f uS", l17182s_pwm_config_parse_gmea(usPWMConfig));
        DBGPRINTLN_CTX("      ILIM Pos: %.3f A", l17182s_pwm_config_parse_pos_ilim(usPWMConfig));
        DBGPRINTLN_CTX("      ILIM Neg: %.3f A", l17182s_pwm_config_parse_neg_ilim(usPWMConfig));
        DBGPRINTLN_CTX("      CITH: %.3f pF", l17182s_pwm_config_parse_cith(usPWMConfig));
        DBGPRINTLN_CTX("      RITH: %.3f kOhm", l17182s_pwm_config_parse_rith(usPWMConfig));
        DBGPRINTLN_CTX("      Pulse Skip: %s", l17182s_pwm_config_parse_pulse_skipping(usPWMConfig) ? "Enabled" : "Disabled");
        DBGPRINTLN_CTX("      FCM at TOFF: %s", l17182s_pwm_config_parse_fcm_at_toff(usPWMConfig) ? "Enabled" : "Disabled");
        DBGPRINTLN_CTX("      Low VOUT: %s", l17182s_pwm_config_parse_low_vout_mode(usPWMConfig) ? "Enabled" : "Disabled");
        DBGPRINTLN_CTX("      Phase: %.3f deg", lt7182s_get_pwm_phase(i));
    }

    DBGPRINTLN_CTX("  EXTVCC: %.3f V", lt7182s_read_extvcc());
    DBGPRINTLN_CTX("  Temp: %.3f C (Pk: %.3f C)", lt7182s_read_temperature(), lt7182s_read_temperature_peak());
    DBGPRINTLN_CTX("  OT Fault Temp: %.3f C", lt7182s_get_ot_fault());
    DBGPRINTLN_CTX("  OT Warn Temp: %.3f C", lt7182s_get_ot_warn());
    DBGPRINTLN_CTX("  Freq: %.3f kHz", lt7182s_get_freq());

    //// UPD test
    // UPD350_SELECT();
    // delay_ms(50);
    // UPD350_UNSELECT();
    // delay_ms(500);

    // uint8_t buf[8];

    // UPD350_SELECT();

    // sercom0_spi_write_byte(0x0B, 0);

    // sercom0_spi_write_byte(0x00, 0);
    // sercom0_spi_write_byte(0x00, 0);

    // sercom0_spi_write_byte(0x00, 1);

    // sercom0_spi_read(buf, 8, 0x00);

    // UPD350_UNSELECT();

    // DBGPRINTLN_CTX("UPD350 ID: 0x%08X", *(uint32_t *)buf);
    // DBGPRINTLN_CTX("UPD350 USB Vendor ID: 0x%04X", *(uint16_t *)(buf + 4));
    // DBGPRINTLN_CTX("UPD350 USB Product ID: 0x%04X", *(uint16_t *)(buf + 6));
    //// End UPD test

    DBGPRINTLN_CTX("Checking power sources in 100 ms...");
    delay_ms(100);

    if(!pmc_check_input_voltages())
    {
        DBGPRINTLN_CTX("No suitable power source detected, retrying until one is found...");

        while(1)
        {
            LED_TOGGLE();
            delay_ms(100);

            if(pmc_check_input_voltages())
                break;
        }
    }

    DBGPRINTLN_CTX("Selected power source: %s", VEXT_STATUS() ? "VEXT" : PCIE_12V0_STATUS() ? "PCIe 12V0" : VBUS_SNK_STATUS() ? "VBUS" : "UNKNOWN");

    DBGPRINTLN_CTX("Enabling 5V0 and 1V0 in 100 ms...");
    delay_ms(100);

    lt7182s_mfr_fault_clear();
    lt7182s_cleak_faults();

    lt7182s_set_operation(1, LT7182S_OPERATION_ON); // First enable 1V0, it is held off by the 5V0 PG line
    lt7182s_set_operation(0, LT7182S_OPERATION_ON);
    delay_ms(50);

    for(uint8_t i = 0; i < 2; i++)
    {
        uint16_t usStatus = lt7182s_get_status_word(i);

        if(usStatus != 0x0000)
        {
            uint8_t ubStatusVOUT = lt7182s_get_status_vout(i);
            uint8_t ubStatusIOUT = lt7182s_get_status_iout(i);
            uint8_t ubStatusInput = lt7182s_get_status_input(i);
            uint8_t ubStatusTemperature = lt7182s_get_status_temperature();
            uint8_t ubStatusCML = lt7182s_get_status_cml();
            uint8_t ubStatusMFRSpecific = lt7182s_get_status_mfr_specific(i);
            uint8_t ubStatusMFRCommon = lt7182s_get_status_mfr_common();
            uint16_t usStatusMFRPads = lt7182s_get_status_mfr_pads();
            uint8_t ubStatusMFRPinConfig = lt7182s_get_status_mfr_pin_config();

            lt7182s_set_operation(0, LT7182S_OPERATION_TURN_OFF_IMMED);
            lt7182s_set_operation(1, LT7182S_OPERATION_TURN_OFF_IMMED);

            DBGPRINTLN_CTX("LT7182S Channel #%hhu failed to turn on!", i);

            DBGPRINTLN_CTX("Status (0x%04X):", usStatus);
            DBGPRINTLN_CTX("  VOUT: 0x%02X", ubStatusVOUT);
            DBGPRINTLN_CTX("  IOUT: 0x%02X", ubStatusIOUT);
            DBGPRINTLN_CTX("  Input: 0x%02X", ubStatusInput);
            DBGPRINTLN_CTX("  Temp: 0x%02X", ubStatusTemperature);
            DBGPRINTLN_CTX("  CML: 0x%02X", ubStatusCML);
            DBGPRINTLN_CTX("  MFR Spec: 0x%02X", ubStatusMFRSpecific);
            DBGPRINTLN_CTX("  MFR Com: 0x%02X", ubStatusMFRCommon);
            DBGPRINTLN_CTX("  MFR Pads: 0x%04X", usStatusMFRPads);
            DBGPRINTLN_CTX("  MFR PinCfg: 0x%02X", ubStatusMFRPinConfig);

            // TODO: Further investigate status bits to determine the cause

            while(1)
            {
                LED_TOGGLE();
                delay_ms(500);
            }
        }
    }

    if(!DBG_PRESENT())
    {
        // Done with I2C master, switch to slave
        DBGPRINTLN_CTX("Switching I2C bus 1 to slave mode...");

        sercom1_i2c_slave_init(I2C_SLAVE_ADDRESS);
    }
    else
    {
        DBGPRINTLN_CTX("Debugger detected, I2C will remain in master mode");
    }

    DBGPRINTLN_CTX("Releasing FPGA init in 100 ms...");
    delay_ms(100);

    FPGA_INIT_DEASSERT();

    if(!PCIE_12V0_STATUS()) // CM4 supported only when not using the PCIe slot
    {
        DBGPRINTLN_CTX("Turning CM4 ON in 200 ms...");
        delay_ms(200);

        //CM4_USB_OTG_DEVICE();
        //CM4_BTLDR_ENABLE();
        CM4_GLOBAL_ENABLE();

        DBGPRINTLN_CTX("Waiting for CM4 to be running...");

        uint64_t ullCM4StartTime = g_ullSystemTick;

        while(!CM4_RUNNING() && g_ullSystemTick - ullCM4StartTime < 5000);

        if(!CM4_RUNNING())
        {
            DBGPRINTLN_CTX("CM4 failed to start, assuming not present");

            CM4_GLOBAL_DISABLE();
        }
        else
        {
            DBGPRINTLN_CTX("CM4 is running");
        }
    }
    else
    {
        DBGPRINTLN_CTX("Board is powered via PCIe 12V, CM4 is not supported");
    }

    wdt_enable();

    while(1)
    {
        wdt_feed();

        // if(I2C_SLAVE_REGISTER(uint8_t, I2C_SLAVE_REGISTER_CONFIG) & BIT(7))
        // {
        //     delay_ms(20);

        //     reset();
        // }

        if(I2C_SLAVE_REGISTER(uint64_t, I2C_SLAVE_REGISTER_UPTIME) != g_ullSystemTick / 1000)
        {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                I2C_SLAVE_REGISTER(uint64_t, I2C_SLAVE_REGISTER_UPTIME) = g_ullSystemTick / 1000;
            }
        }

        static uint64_t ullLastHeartBeat = 0;
        static uint64_t ullLastPowerSourceChange = 0;
        static uint64_t ullLastCM4Check = 0;
        static uint64_t ullLastTelemetryUpdate = 0;

        if((g_ullSystemTick > 0 && ullLastHeartBeat == 0) || g_ullSystemTick - ullLastHeartBeat > 5000)
        {
            ullLastHeartBeat = g_ullSystemTick;

            LED_TOGGLE();

            if(LED_STATUS())
                ullLastHeartBeat -= 4900;
        }

        if((g_ullSystemTick > 0 && ullLastPowerSourceChange == 0) || g_ullSystemTick - ullLastPowerSourceChange > 1000)
        {
            if(!NO_VOLTAGE_ENABLED() && pmc_check_input_voltages())
            {
                DBGPRINTLN_CTX("Power source changed to: %s", VEXT_STATUS() ? "VEXT" : PCIE_12V0_STATUS() ? "PCIe 12V0" : VBUS_SNK_STATUS() ? "VBUS" : "UNKNOWN");

                ullLastPowerSourceChange = g_ullSystemTick; // Hysteresis
            }
        }

        if((g_ullSystemTick > 0 && ullLastCM4Check == 0) || g_ullSystemTick - ullLastCM4Check > 500)
        {
            ullLastCM4Check = g_ullSystemTick;

            static uint8_t ubCM4CheckCount = 0;

            if(CM4_GLOBAL_STATUS() && !CM4_RUNNING())
                ubCM4CheckCount++;
            else
                ubCM4CheckCount = 0;

            if(ubCM4CheckCount > 3)
            {
                DBGPRINTLN_CTX("RPi CM4 is OFF, shutting system down...");

                CM4_GLOBAL_DISABLE();
                FPGA_INIT_ASSERT();
                // lt7182s_set_operation(0, LT7182S_OPERATION_TURN_OFF_IMMED);
                // lt7182s_set_operation(1, LT7182S_OPERATION_TURN_OFF_IMMED);
                // delay_ms(50);
                VOLTAGE_DISABLE_ALL();

                DBGPRINTLN_CTX("Done!");

                // TODO: Remove below
                // wdt_disable();
                // delay_ms(2000);
                // CM4_GLOBAL_ENABLE();
                // while(!CM4_RUNNING());
                // wdt_feed();
                // wdt_enable();
            }
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
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_VIN_VOLTAGE) = ulVIN;
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_VEXT_VOLTAGE) = ulVEXT;
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_12V0_VOLTAGE) = ul12V0;
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_VBUS_VOLTAGE) = ulVBUS;
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE) = ulIOVDD;
                I2C_SLAVE_REGISTER(uint32_t, I2C_SLAVE_REGISTER_CORE_VOLTAGE) = ulCoreVDD;
            }

            if(DBG_PRESENT())
            {
                DBGPRINTLN("------------------------------");
                DBGPRINTLN("IOVDD: %u mV", ulIOVDD);
                DBGPRINTLN("Core: %u mV", ulCoreVDD);
                DBGPRINTLN("VIN: %u mV", ulVIN);
                DBGPRINTLN("VEXT: %u mV", ulVEXT);
                DBGPRINTLN("12V0: %u mV", ul12V0);
                DBGPRINTLN("VBUS: %u mV", ulVBUS);
                DBGPRINTLN("Temp: %d.%02u C", lTemp / 1000, ABS(lTemp % 1000));

                DBGPRINTLN("LT7182S:");

                for(uint8_t i = 0; i < 2; i++)
                {
                    DBGPRINTLN("  Channel #%hhu:", i);

                    uint16_t usStatus = lt7182s_get_status_word(i);

                    if(usStatus != 0x0000)
                    {
                        uint8_t ubStatusVOUT = lt7182s_get_status_vout(i);
                        uint8_t ubStatusIOUT = lt7182s_get_status_iout(i);
                        uint8_t ubStatusInput = lt7182s_get_status_input(i);
                        uint8_t ubStatusTemperature = lt7182s_get_status_temperature();
                        uint8_t ubStatusCML = lt7182s_get_status_cml();
                        uint8_t ubStatusMFRSpecific = lt7182s_get_status_mfr_specific(i);
                        uint8_t ubStatusMFRCommon = lt7182s_get_status_mfr_common();
                        uint16_t usStatusMFRPads = lt7182s_get_status_mfr_pads();
                        uint8_t ubStatusMFRPinConfig = lt7182s_get_status_mfr_pin_config();

                        DBGPRINTLN("    Status (0x%04X):", usStatus);
                        DBGPRINTLN("      VOUT: 0x%02X", ubStatusVOUT);
                        DBGPRINTLN("      IOUT: 0x%02X", ubStatusIOUT);
                        DBGPRINTLN("      Input: 0x%02X", ubStatusInput);
                        DBGPRINTLN("      Temp: 0x%02X", ubStatusTemperature);
                        DBGPRINTLN("      CML: 0x%02X", ubStatusCML);
                        DBGPRINTLN("      MFR Spec: 0x%02X", ubStatusMFRSpecific);
                        DBGPRINTLN("      MFR Com: 0x%02X", ubStatusMFRCommon);
                        DBGPRINTLN("      MFR Pads: 0x%04X", usStatusMFRPads);
                        DBGPRINTLN("      MFR PinCfg: 0x%02X", ubStatusMFRPinConfig);
                    }

                    DBGPRINTLN("    VIN: %.3f V (Pk: %.3f V)", lt7182s_read_vin(i), lt7182s_read_vin_peak(i));
                    DBGPRINTLN("    IIN: %.3f A", lt7182s_read_iin(i));
                    DBGPRINTLN("    VOUT: %.3f V (Set: %.3f V, Pk: %.3f V)", lt7182s_read_vout(i), lt7182s_get_vout(i), lt7182s_read_vout_peak(i));
                    DBGPRINTLN("    IOUT: %.3f A (Pk: %.3f A)", lt7182s_read_iout(i), lt7182s_read_iout_peak(i));
                    DBGPRINTLN("    FREQ: %.3f kHz", lt7182s_read_freq(i));
                    DBGPRINTLN("    POUT: %.3f W", lt7182s_read_pout(i));
                    DBGPRINTLN("    VITH: %.3f V", lt7182s_read_ith(i));
                }

                DBGPRINTLN("  EXTVCC: %.3f V", lt7182s_read_extvcc());
                DBGPRINTLN("  Temp: %.3f C (Pk: %.3f C)", lt7182s_read_temperature(), lt7182s_read_temperature_peak());
                DBGPRINTLN("CM4: %s", CM4_RUNNING() ? "Running" : "OFF");
            }
        }
    }

    return 0;
}