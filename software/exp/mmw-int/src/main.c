#include <em_device.h>
#include <stdlib.h>
#include <math.h>
#include "arm_math.h"
#include "arm_const_structs.h"
#include "debug_macros.h"
#include "utils.h"
#include "nvic.h"
#include "atomic.h"
#include "systick.h"
#include "rmu.h"
#include "emu.h"
#include "cmu.h"
#include "gpio.h"
#include "dbg.h"
#include "msc.h"
#include "crypto.h"
#include "crc.h"
#include "trng.h"
#include "rtcc.h"
#include "adc.h"
#include "usart.h"
#include "i2c.h"
#include "wdog.h"
#include "mcp9802.h"
#include "mcp4728.h"
#include "admv1013.h"
#include "admv1014.h"
#include "adrf5720.h"
#include "idt8v97003.h"

// Structs
typedef struct __attribute((__packed__))
{
    uint8_t ubTest[2];
} i2c_rom_t;

// Helper macros
#define TSENS_BOT_ADDR              0
#define TSENS_SYNTH_ADDR            1
#define TSENS_MMW_CONV_ADDR         2
#define I2C_SLAVE_ADDRESS                           0x22
#define I2C_SLAVE_REGISTER_COUNT                    256
#define I2C_SLAVE_REGISTER(t, a)                    (*(t *)&ubI2CRegister[(a)])
#define I2C_SLAVE_REGISTER_WRITE_MASK(t, a)         (*(t *)&ubI2CRegisterWriteMask[(a)])
#define I2C_SLAVE_REGISTER_READ_MASK(t, a)          (*(t *)&ubI2CRegisterReadMask[(a)])
#define I2C_SLAVE_REGISTER_STATUS                   0x00 // 8-bit
#define I2C_SLAVE_REGISTER_CONFIG                   0x01 // 8-bit
#define I2C_SLAVE_REGISTER_AVDD_VOLTAGE             0xC0 // 32-bit
#define I2C_SLAVE_REGISTER_DVDD_VOLTAGE             0xC4 // 32-bit
#define I2C_SLAVE_REGISTER_IOVDD_VOLTAGE            0xC8 // 32-bit
#define I2C_SLAVE_REGISTER_CORE_VOLTAGE             0xCC // 32-bit
#define I2C_SLAVE_REGISTER_EMU_TEMP                 0xD0 // 32-bit
#define I2C_SLAVE_REGISTER_ADC_TEMP                 0xD4 // 32-bit
#define I2C_SLAVE_REGISTER_BOARD_BOT_TEMP           0xD8 // 16-bit
#define I2C_SLAVE_REGISTER_BOARD_SYNTH_TEMP         0xDA // 16-bit
#define I2C_SLAVE_REGISTER_BOARD_MMW_CONV_TEMP      0xDC // 16-bit
#define I2C_SLAVE_REGISTER_UPTIME                   0xE0 // 64-bit
#define I2C_SLAVE_REGISTER_SW_VERSION               0xE8 // 16-bit
#define I2C_SLAVE_REGISTER_DEV_UIDL                 0xF0 // 32-bit
#define I2C_SLAVE_REGISTER_DEV_UIDH                 0xF4 // 32-bit

// Forward declarations
static void reset() __attribute__((noreturn));
static void sleep();

static uint32_t get_free_ram();

static void get_device_core_name(char *pszDeviceCoreName, uint32_t ulDeviceCoreNameSize);
static void get_device_name(char *pszDeviceName, uint32_t ulDeviceNameSize);
static uint16_t get_device_revision();

static void wdog_warning_isr();

static void i2c_slave_register_init();
static uint8_t i2c_slave_addr_isr(uint8_t ubRnW);
static uint8_t i2c_slave_tx_data_isr();
static uint8_t i2c_slave_rx_data_isr(uint8_t ubData);

static void rf_gain_vctrl_init();
static void rf_gain_set_tx_vctrl(uint8_t ubChannel, float fVoltage);
static float rf_gain_get_tx_vctrl(uint8_t ubChannel);
static void rf_gain_set_rx_vctrl(float fVoltage);
static float rf_gain_get_rx_vctrl();

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
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Configure Deep Sleep (EM2/3)

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        IRQ_CLEAR(RTCC_IRQn);

        __DMB(); // Wait for all memory transactions to finish before memory access
        __DSB(); // Wait for all memory transactions to finish before executing instructions
        __ISB(); // Wait for all memory transactions to finish before fetching instructions
        __SEV(); // Set the event flag to ensure the next WFE will be a NOP
        __WFE(); // NOP and clear the event flag
        __WFE(); // Wait for event
        __NOP(); // Prevent debugger crashes

        cmu_init();
        cmu_update_clocks();
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
    uint8_t ubFamily = (DEVINFO->PART & _DEVINFO_PART_DEVICE_FAMILY_MASK) >> _DEVINFO_PART_DEVICE_FAMILY_SHIFT;
    const char* szFamily = "?";

    switch(ubFamily)
    {
        case 0x10: szFamily = "EFR32MG1P";  break;
        case 0x11: szFamily = "EFR32MG1B";  break;
        case 0x12: szFamily = "EFR32MG1V";  break;
        case 0x13: szFamily = "EFR32BG1P";  break;
        case 0x14: szFamily = "EFR32BG1B";  break;
        case 0x15: szFamily = "EFR32BG1V";  break;
        case 0x19: szFamily = "EFR32FG1P";  break;
        case 0x1A: szFamily = "EFR32FG1B";  break;
        case 0x1B: szFamily = "EFR32FG1V";  break;
        case 0x1C: szFamily = "EFR32MG12P"; break;
        case 0x1D: szFamily = "EFR32MG12B"; break;
        case 0x1E: szFamily = "EFR32MG12V"; break;
        case 0x1F: szFamily = "EFR32BG12P"; break;
        case 0x20: szFamily = "EFR32BG12B"; break;
        case 0x21: szFamily = "EFR32BG12V"; break;
        case 0x25: szFamily = "EFR32FG12P"; break;
        case 0x26: szFamily = "EFR32FG12B"; break;
        case 0x27: szFamily = "EFR32FG12V"; break;
        case 0x28: szFamily = "EFR32MG13P"; break;
        case 0x29: szFamily = "EFR32MG13B"; break;
        case 0x2A: szFamily = "EFR32MG13V"; break;
        case 0x2B: szFamily = "EFR32BG13P"; break;
        case 0x2C: szFamily = "EFR32BG13B"; break;
        case 0x2D: szFamily = "EFR32BG13V"; break;
        case 0x2E: szFamily = "EFR32ZG13P"; break;
        case 0x31: szFamily = "EFR32FG13P"; break;
        case 0x32: szFamily = "EFR32FG13B"; break;
        case 0x33: szFamily = "EFR32FG13V"; break;
        case 0x34: szFamily = "EFR32MG14P"; break;
        case 0x35: szFamily = "EFR32MG14B"; break;
        case 0x36: szFamily = "EFR32MG14V"; break;
        case 0x37: szFamily = "EFR32BG14P"; break;
        case 0x38: szFamily = "EFR32BG14B"; break;
        case 0x39: szFamily = "EFR32BG14V"; break;
        case 0x3A: szFamily = "EFR32ZG14P"; break;
        case 0x3D: szFamily = "EFR32FG14P"; break;
        case 0x3E: szFamily = "EFR32FG14B"; break;
        case 0x3F: szFamily = "EFR32FG14V"; break;
        case 0x47: szFamily = "EFM32G";     break;
        case 0x48: szFamily = "EFM32GG";    break;
        case 0x49: szFamily = "EFM32TG";    break;
        case 0x4A: szFamily = "EFM32LG";    break;
        case 0x4B: szFamily = "EFM32WG";    break;
        case 0x4C: szFamily = "EFM32ZG";    break;
        case 0x4D: szFamily = "EFM32HG";    break;
        case 0x51: szFamily = "EFM32PG1B";  break;
        case 0x53: szFamily = "EFM32JG1B";  break;
        case 0x55: szFamily = "EFM32PG12B"; break;
        case 0x57: szFamily = "EFM32JG12B"; break;
        case 0x64: szFamily = "EFM32GG11B"; break;
        case 0x67: szFamily = "EFM32TG11B"; break;
        case 0x6A: szFamily = "EFM32GG12B"; break;
        case 0x78: szFamily = "EZR32LG";    break;
        case 0x79: szFamily = "EZR32WG";    break;
        case 0x7A: szFamily = "EZR32HG";    break;
    }

    uint8_t ubPackage = (DEVINFO->MEMINFO & _DEVINFO_MEMINFO_PKGTYPE_MASK) >> _DEVINFO_MEMINFO_PKGTYPE_SHIFT;
    char cPackage = '?';

    if(ubPackage == 74)
        cPackage = '?';
    else if(ubPackage == 76)
        cPackage = 'L';
    else if(ubPackage == 77)
        cPackage = 'M';
    else if(ubPackage == 81)
        cPackage = 'Q';

    uint8_t ubTempGrade = (DEVINFO->MEMINFO & _DEVINFO_MEMINFO_TEMPGRADE_MASK) >> _DEVINFO_MEMINFO_TEMPGRADE_SHIFT;
    char cTempGrade = '?';

    if(ubTempGrade == 0)
        cTempGrade = 'G';
    else if(ubTempGrade == 1)
        cTempGrade = 'I';
    else if(ubTempGrade == 2)
        cTempGrade = '?';
    else if(ubTempGrade == 3)
        cTempGrade = '?';

    uint16_t usPartNumber = (DEVINFO->PART & _DEVINFO_PART_DEVICE_NUMBER_MASK) >> _DEVINFO_PART_DEVICE_NUMBER_SHIFT;
    uint8_t ubPinCount = (DEVINFO->MEMINFO & _DEVINFO_MEMINFO_PINCOUNT_MASK) >> _DEVINFO_MEMINFO_PINCOUNT_SHIFT;

    snprintf(pszDeviceName, ulDeviceNameSize, "%s%huF%hu%c%c%hhu", szFamily, usPartNumber, FLASH_SIZE >> 10, cTempGrade, cPackage, ubPinCount);
}
uint16_t get_device_revision()
{
    uint16_t usRevision;

    /* CHIP MAJOR bit [3:0]. */
    usRevision = ((ROMTABLE->PID0 & _ROMTABLE_PID0_REVMAJOR_MASK) >> _ROMTABLE_PID0_REVMAJOR_SHIFT) << 8;
    /* CHIP MINOR bit [7:4]. */
    usRevision |= ((ROMTABLE->PID2 & _ROMTABLE_PID2_REVMINORMSB_MASK) >> _ROMTABLE_PID2_REVMINORMSB_SHIFT) << 4;
    /* CHIP MINOR bit [3:0]. */
    usRevision |= (ROMTABLE->PID3 & _ROMTABLE_PID3_REVMINORLSB_MASK) >> _ROMTABLE_PID3_REVMINORLSB_SHIFT;

    return usRevision;
}

void wdog_warning_isr()
{
    DBGPRINTLN_CTX("Watchdog warning!");
}

void i2c_slave_register_init()
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        I2C_SLAVE_REGISTER              (uint8_t,  I2C_SLAVE_REGISTER_STATUS)                   = 0x00;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint8_t,  I2C_SLAVE_REGISTER_STATUS)                   = 0x00;
        I2C_SLAVE_REGISTER_READ_MASK    (uint8_t,  I2C_SLAVE_REGISTER_STATUS)                   = 0xFF;
        I2C_SLAVE_REGISTER              (uint8_t,  I2C_SLAVE_REGISTER_CONFIG)                   = 0x00;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint8_t,  I2C_SLAVE_REGISTER_CONFIG)                   = 0xFF;
        I2C_SLAVE_REGISTER_READ_MASK    (uint8_t,  I2C_SLAVE_REGISTER_CONFIG)                   = 0xFF;
        I2C_SLAVE_REGISTER              (float,    I2C_SLAVE_REGISTER_AVDD_VOLTAGE)             = -1.f;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_AVDD_VOLTAGE)             = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_AVDD_VOLTAGE)             = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (float,    I2C_SLAVE_REGISTER_DVDD_VOLTAGE)             = -1.f;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_DVDD_VOLTAGE)             = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_DVDD_VOLTAGE)             = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (float,    I2C_SLAVE_REGISTER_IOVDD_VOLTAGE)            = -1.f;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE)            = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE)            = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (float,    I2C_SLAVE_REGISTER_CORE_VOLTAGE)             = -1.f;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_CORE_VOLTAGE)             = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_CORE_VOLTAGE)             = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (float,    I2C_SLAVE_REGISTER_EMU_TEMP)                 = -1.f;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_EMU_TEMP)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_EMU_TEMP)                 = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (float,    I2C_SLAVE_REGISTER_ADC_TEMP)                 = -1.f;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_ADC_TEMP)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_ADC_TEMP)                 = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint16_t, I2C_SLAVE_REGISTER_BOARD_BOT_TEMP)           = 0x0000;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint16_t, I2C_SLAVE_REGISTER_BOARD_BOT_TEMP)           = 0x0000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint16_t, I2C_SLAVE_REGISTER_BOARD_BOT_TEMP)           = 0xFFFF;
        I2C_SLAVE_REGISTER              (uint16_t, I2C_SLAVE_REGISTER_BOARD_SYNTH_TEMP)         = 0x0000;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint16_t, I2C_SLAVE_REGISTER_BOARD_SYNTH_TEMP)         = 0x0000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint16_t, I2C_SLAVE_REGISTER_BOARD_SYNTH_TEMP)         = 0xFFFF;
        I2C_SLAVE_REGISTER              (uint16_t, I2C_SLAVE_REGISTER_BOARD_MMW_CONV_TEMP)      = 0x0000;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint16_t, I2C_SLAVE_REGISTER_BOARD_MMW_CONV_TEMP)      = 0x0000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint16_t, I2C_SLAVE_REGISTER_BOARD_MMW_CONV_TEMP)      = 0xFFFF;
        I2C_SLAVE_REGISTER              (uint64_t, I2C_SLAVE_REGISTER_UPTIME)                   = g_ullSystemTick / 1000;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint64_t, I2C_SLAVE_REGISTER_UPTIME)                   = 0x0000000000000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint64_t, I2C_SLAVE_REGISTER_UPTIME)                   = 0xFFFFFFFFFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint16_t, I2C_SLAVE_REGISTER_SW_VERSION)               = BUILD_VERSION;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint16_t, I2C_SLAVE_REGISTER_SW_VERSION)               = 0x0000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint16_t, I2C_SLAVE_REGISTER_SW_VERSION)               = 0xFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_DEV_UIDL)                 = DEVINFO->UNIQUEL;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_DEV_UIDL)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_DEV_UIDL)                 = 0xFFFFFFFF;
        I2C_SLAVE_REGISTER              (uint32_t, I2C_SLAVE_REGISTER_DEV_UIDH)                 = DEVINFO->UNIQUEH;
        I2C_SLAVE_REGISTER_WRITE_MASK   (uint32_t, I2C_SLAVE_REGISTER_DEV_UIDH)                 = 0x00000000;
        I2C_SLAVE_REGISTER_READ_MASK    (uint32_t, I2C_SLAVE_REGISTER_DEV_UIDH)                 = 0xFFFFFFFF;
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
    rf_gain_set_tx_vctrl(0, 0.0);
    rf_gain_set_tx_vctrl(1, 0.0);

    // Check if RX is powered because min. RX gain = max voltage (1.8V)
    rf_gain_set_rx_vctrl(RX_ANA_PWR_STATUS() ? 1.8 : 0.0);
}
void rf_gain_set_tx_vctrl(uint8_t ubChannel, float fVoltage)
{
    if(ubChannel > 1)
        return;

    if(fVoltage > 1.8)
        fVoltage = 1.8;

    DBGPRINTLN_CTX("Set TX VCTRL%hhu to %.3f V", ubChannel + 1, fVoltage);

    uint16_t usCode = fVoltage * 4096.0 / MCP4728_INTERNAL_REF_V;

    if(ubChannel == 0)
        ubChannel = 3; // VCTRL1 is on channel 3
    else if(ubChannel == 1)
        ubChannel = 2; // VCTRL2 is on channel 2

    mcp4728_channel_write(ubChannel, MCP4728_CHAN_VREF_INTERNAL | MCP4728_CHAN_PD_NORMAL | MCP4728_CHAN_GAIN_X1 | (usCode & 0x0FFF));
}
float rf_gain_get_tx_vctrl(uint8_t ubChannel)
{
    if(ubChannel > 1)
        return 0;

    if(ubChannel == 0)
        ubChannel = 3; // VCTRL1 is on channel 3
    else if(ubChannel == 1)
        ubChannel = 2; // VCTRL2 is on channel 2

    uint16_t usCode = mcp4728_channel_read(ubChannel) & 0x0FFF;

    return (float)usCode * MCP4728_INTERNAL_REF_V / 4096.0;
}
void rf_gain_set_rx_vctrl(float fVoltage)
{
    if(fVoltage > 1.8)
        fVoltage = 1.8;

    DBGPRINTLN_CTX("Set RX VCTRL to %.3f V", fVoltage);

    uint16_t usCode = fVoltage * 4096.0 / MCP4728_INTERNAL_REF_V;

    mcp4728_channel_write(0, MCP4728_CHAN_VREF_INTERNAL | MCP4728_CHAN_PD_NORMAL | MCP4728_CHAN_GAIN_X1 | (usCode & 0x0FFF));
}
float rf_gain_get_rx_vctrl()
{
    uint16_t usCode = mcp4728_channel_read(0) & 0x0FFF;

    return (float)usCode * MCP4728_INTERNAL_REF_V / 4096.0;
}

int init()
{
    rmu_init(RMU_CTRL_PINRMODE_FULL, RMU_CTRL_SYSRMODE_EXTENDED, RMU_CTRL_LOCKUPRMODE_EXTENDED, RMU_CTRL_WDOGRMODE_EXTENDED); // Init RMU and set reset modes

    emu_init(1); // Init EMU, ignore DCDC and switch digital power immediatly to DVDD

    cmu_init(); // Init Clocks

    dbg_init(); // Init Debug module
    dbg_swo_config(BIT(0) | BIT(1), 6000000); // Init SWO channels 0 and 1 at 6 MHz

    msc_init(); // Init Flash, RAM and caches

    systick_init(); // Init system tick

    wdog_init((8 <<_WDOG_CTRL_PERSEL_SHIFT) | (3 << _WDOG_CTRL_WARNSEL_SHIFT)); // Init the watchdog timer, 2049 ms timeout, 75% warning
    wdog_set_warning_isr(wdog_warning_isr);

    gpio_init(); // Init GPIOs
    ldma_init(); // Init LDMA
    rtcc_init(); // Init RTCC
    trng_init(); // Init TRNG
    crypto_init(); // Init Crypto engine
    crc_init(); // Init CRC calculation unit
    adc_init(); // Init ADCs

    float fAVDDHighThresh, fAVDDLowThresh;
    float fDVDDHighThresh, fDVDDLowThresh;
    float fIOVDDHighThresh, fIOVDDLowThresh;

    emu_vmon_avdd_config(1, 3.1f, &fAVDDLowThresh, 3.22f, &fAVDDHighThresh); // Enable AVDD monitor
    emu_vmon_dvdd_config(1, 3.1f, &fDVDDLowThresh); // Enable DVDD monitor
    emu_vmon_iovdd_config(1, 3.15f, &fIOVDDLowThresh); // Enable IOVDD monitor

    fDVDDHighThresh = fDVDDLowThresh + 0.026f; // Hysteresis from datasheet
    fIOVDDHighThresh = fIOVDDLowThresh + 0.026f; // Hysteresis from datasheet

    usart0_init(1000000, 0, USART_SPI_MSB_FIRST, 0, 0, 0); // Init USART0 at 18 MHz (RF Converters)
    usart2_init(12000000, 0, USART_SPI_MSB_FIRST, USART_LOCATION_DISABLED, 1, 1); // Init USART2 at 12 MHz (Ext. LO Attenuator)
    usart3_init(18000000, 0, USART_SPI_MSB_FIRST, 0, 0, 0); // Init USART3 at 18 MHz (Synthesizer)

    i2c0_init(I2C_FAST, 1, 1); // Init I2C0 as master (DAC)
    i2c1_init(I2C_SLAVE_ADDRESS, 3, 3); // Init I2C1 as slave (Communication with the host)
    i2c1_set_slave_addr_isr(i2c_slave_addr_isr);
    i2c1_set_slave_tx_data_isr(i2c_slave_tx_data_isr);
    i2c1_set_slave_rx_data_isr(i2c_slave_rx_data_isr);
    i2c2_init(I2C_FAST, 0, 0); // Init I2C2 as master (Temperature Sensors)

    char szDeviceCoreName[32];
    char szDeviceName[32];

    get_device_core_name(szDeviceCoreName, 32);
    get_device_name(szDeviceName, 32);

    DBGPRINTLN_CTX("IcyRadio mmW Int. Exp. Controller v%lu (%s %s)!", BUILD_VERSION, __DATE__, __TIME__);
    DBGPRINTLN_CTX("Core: %s", szDeviceCoreName);
    DBGPRINTLN_CTX("Device: %s", szDeviceName);
    DBGPRINTLN_CTX("Device Revision: 0x%04X", get_device_revision());
    DBGPRINTLN_CTX("Calibration temperature: %hhu C", (DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >> _DEVINFO_CAL_TEMP_SHIFT);
    DBGPRINTLN_CTX("Flash Size: %hu KiB", FLASH_SIZE >> 10);
    DBGPRINTLN_CTX("RAM Size: %hu KiB", SRAM_SIZE >> 10);
    DBGPRINTLN_CTX("Free RAM: %lu KiB", get_free_ram() >> 10);
    DBGPRINTLN_CTX("Unique ID: %08X-%08X", DEVINFO->UNIQUEH, DEVINFO->UNIQUEL);

    DBGPRINTLN_CTX("RMU - Reset cause: %hhu", rmu_get_reset_reason());
    DBGPRINTLN_CTX("RMU - Reset state: %hhu", rmu_get_reset_state());

    rmu_clear_reset_reason();

    DBGPRINTLN_CTX("EMU - AVDD Fall Threshold: %.2f mV!", fAVDDLowThresh * 1000);
    DBGPRINTLN_CTX("EMU - AVDD Rise Threshold: %.2f mV!", fAVDDHighThresh * 1000);
    DBGPRINTLN_CTX("EMU - AVDD Voltage: %.2f mV", adc_get_avdd());
    DBGPRINTLN_CTX("EMU - AVDD Status: %s", g_ubAVDDLow ? "LOW" : "OK");
    DBGPRINTLN_CTX("EMU - DVDD Fall Threshold: %.2f mV!", fDVDDLowThresh * 1000);
    DBGPRINTLN_CTX("EMU - DVDD Rise Threshold: %.2f mV!", fDVDDHighThresh * 1000);
    DBGPRINTLN_CTX("EMU - DVDD Voltage: %.2f mV", adc_get_dvdd());
    DBGPRINTLN_CTX("EMU - DVDD Status: %s", g_ubDVDDLow ? "LOW" : "OK");
    DBGPRINTLN_CTX("EMU - IOVDD Fall Threshold: %.2f mV!", fIOVDDLowThresh * 1000);
    DBGPRINTLN_CTX("EMU - IOVDD Rise Threshold: %.2f mV!", fIOVDDHighThresh * 1000);
    DBGPRINTLN_CTX("EMU - IOVDD Voltage: %.2f mV", adc_get_iovdd());
    DBGPRINTLN_CTX("EMU - IOVDD Status: %s", g_ubIOVDDLow ? "LOW" : "OK");
    DBGPRINTLN_CTX("EMU - Core Voltage: %.2f mV", adc_get_corevdd());

    DBGPRINTLN_CTX("CMU - HFXO Oscillator: %.3f MHz", (float)HFXO_OSC_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFRCO Oscillator: %.3f MHz", (float)HFRCO_OSC_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - USHFRCO Oscillator: %.3f MHz", (float)USHFRCO_OSC_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - AUXHFRCO Oscillator: %.3f MHz", (float)AUXHFRCO_OSC_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - LFXO Oscillator: %.3f kHz", (float)LFXO_OSC_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LFRCO Oscillator: %.3f kHz", (float)LFRCO_OSC_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - ULFRCO Oscillator: %.3f kHz", (float)ULFRCO_OSC_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - HFSRC Clock: %.3f MHz", (float)HFSRC_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HF Clock: %.3f MHz", (float)HF_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFBUS Clock: %.3f MHz", (float)HFBUS_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFCORE Clock: %.3f MHz", (float)HFCORE_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFEXP Clock: %.3f MHz", (float)HFEXP_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFPER Clock: %.3f MHz", (float)HFPER_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFPERB Clock: %.3f MHz", (float)HFPERB_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFPERC Clock: %.3f MHz", (float)HFPERC_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - HFLE Clock: %.3f MHz", (float)HFLE_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - QSPI Clock: %.3f MHz", (float)QSPI_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - SDIO Clock: %.3f MHz", (float)SDIO_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - USB Clock: %.3f MHz", (float)USB_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - ADC0 Clock: %.3f MHz", (float)ADC0_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - ADC1 Clock: %.3f MHz", (float)ADC1_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - DBG Clock: %.3f MHz", (float)DBG_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - AUX Clock: %.3f MHz", (float)AUX_CLOCK_FREQ / 1000000);
    DBGPRINTLN_CTX("CMU - LFA Clock: %.3f kHz", (float)LFA_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LESENSE Clock: %.3f kHz", (float)LESENSE_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - RTC Clock: %.3f kHz", (float)RTC_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LCD Clock: %.3f kHz", (float)LCD_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LETIMER0 Clock: %.3f kHz", (float)LETIMER0_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LETIMER1 Clock: %.3f kHz", (float)LETIMER1_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LFB Clock: %.3f kHz", (float)LFB_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LEUART0 Clock: %.3f kHz", (float)LEUART0_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LEUART1 Clock: %.3f kHz", (float)LEUART1_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - SYSTICK Clock: %.3f kHz", (float)SYSTICK_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - CSEN Clock: %.3f kHz", (float)CSEN_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LFC Clock: %.3f kHz", (float)LFC_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - LFE Clock: %.3f kHz", (float)LFE_CLOCK_FREQ / 1000);
    DBGPRINTLN_CTX("CMU - RTCC Clock: %.3f kHz", (float)RTCC_CLOCK_FREQ / 1000);

    TSENS_EN();

    DBGPRINTLN_CTX("Scanning I2C bus 0...");

    for(uint8_t a = 0x08; a < 0x78; a++)
        if(i2c0_write(a, NULL, 0, I2C_STOP))
            DBGPRINTLN_CTX("  Address 0x%02X ACKed!", a);

    // TSENS_DIS();

    DBGPRINTLN_CTX("Scanning I2C bus 2...");

    for(uint8_t a = 0x08; a < 0x78; a++)
        if(i2c2_write(a, NULL, 0, I2C_STOP))
            DBGPRINTLN_CTX("  Address 0x%02X ACKed!", a);

    for(uint8_t i = 0; i < 3; i++)
        if(mcp9802_init(i))
            DBGPRINTLN_CTX("MCP9802 #%hhu init OK!", i);
        else
            DBGPRINTLN_CTX("MCP9802 #%hhu init NOK!", i);

    if(mcp4728_init())
        DBGPRINTLN_CTX("MCP4728 init OK!");
    else
        DBGPRINTLN_CTX("MCP4728 init NOK!");

    // mmWave Upconverter
    TX_DIG_PWR_EN();
    TX_ANA_PWR_EN();
    delay_ms(50);

    if(admv1013_init())
    {
        delay_ms(10);
        admv1013_power_down();
        // admv1013_update_lo_filters(6e9);

        DBGPRINTLN_CTX("ADMV1013 rev. %hhu init OK!", admv1013_get_revision());
    }
    else
    {
        TX_ANA_PWR_DIS();
        TX_DIG_PWR_DIS();

        DBGPRINTLN_CTX("ADMV1013 init NOK!");
    }

    // mmWave Downconverter
    RX_DIG_PWR_EN();
    RX_ANA_PWR_EN();
    delay_ms(50);

    if(admv1014_init())
    {
        delay_ms(10);
        admv1014_power_down();
        // admv1014_update_lo_filters(6e9);

        DBGPRINTLN_CTX("ADMV1014 rev. %hhu init OK!", admv1014_get_revision());
    }
    else
    {
        RX_ANA_PWR_DIS();
        RX_DIG_PWR_DIS();

        DBGPRINTLN_CTX("ADMV1014 init NOK!");
    }

    // General RF sections
    RF_GP_PWR_EN();
    delay_ms(10);

    //// RX LO path selection
    RX_LO_DST_SEL0(); // u.FL
    // RX_LO_DST_SEL1(); // Downconverter
    RX_LO_SRC_SEL0(); // Internal Synthesizer
    // RX_LO_SRC_SEL1(); // EXT LO

    //// TX LO path selection
    // TX_LO_DST_SEL0(); // Upconverter
    TX_LO_DST_SEL1(); // u.FL
    // TX_LO_SRC_SEL0(); // EXT
    TX_LO_SRC_SEL1(); // Internal Synthesizer

    //// External LO conditioning
    delay_ms(10);
    // LO_ANA_PWR_EN();

    adrf5720_set_attenuation(3.f);

    // TEMP SYNTH REF GENERATOR
    // cmu_hfper1_clock_gate(CMU_HFPERCLKEN1_WTIMER1, 1);
    // GPIO->P[2].MODEL |= GPIO_P_MODEL_MODE6_PUSHPULL;
    // WTIMER1->CTRL = WTIMER_CTRL_RSSCOIST | WTIMER_CTRL_PRESC_DIV1 | WTIMER_CTRL_CLKSEL_PRESCHFPERCLK | WTIMER_CTRL_FALLA_NONE | WTIMER_CTRL_RISEA_NONE | WTIMER_CTRL_MODE_UP;
    // WTIMER1->CNT = 0x0000;
    // WTIMER1->TOP = (HFPER_CLOCK_FREQ / (18000000UL << 1)) - 1;
    // WTIMER1->CC[3].CTRL = WTIMER_CC_CTRL_PRSCONF_LEVEL | WTIMER_CC_CTRL_CUFOA_NONE | WTIMER_CC_CTRL_COFOA_TOGGLE | WTIMER_CC_CTRL_CMOA_NONE | WTIMER_CC_CTRL_MODE_OUTPUTCOMPARE;
    // WTIMER1->ROUTELOC0 = WTIMER_ROUTELOC0_CC3LOC_LOC2;
    // WTIMER1->ROUTEPEN |= WTIMER_ROUTEPEN_CC3PEN;
    // WTIMER1->CMD = TIMER_CMD_START;

    // Synthesizer
    SYNTH_PWR_EN();
    delay_ms(50);

    if(idt8v97003_init())
    {
        delay_ms(10);
        idt8v97003_power_up(IDT8V97003_PWR_ALL);
        idt8v97003_mute(-1, 1); // Mute all channels via hardware pin

        idt8v97003_enable_lock_detect(1);
        idt8v97003_enable_auto_recal(1);
        idt8v97003_set_lock_detect_pin_mode(IDT8V97003_REG_LD_CTL1_LD_PIN_MODE_LD);
        idt8v97003_set_lock_detect_precision(IDT8V97003_REG_LD_CTL1_LD_PRECISION_1p0ns);

        idt8v97003_config_reference_input(26e6, 0);
        idt8v97003_config_pfd_freq(234e6, IDT8V97003_REG_PFD_PULSE_WIDTH_PW_260ps);

        idt8v97003_loop_filter_t sLoopFilter;

        sLoopFilter.fRs = 20;
        sLoopFilter.fCs = 1e-6;
        sLoopFilter.fCp = 10e-9;
        sLoopFilter.fR3 = 62;
        sLoopFilter.fC3 = 2.2e-9;

        idt8v97003_set_loop_filter(&sLoopFilter);
        idt8v97003_set_target_loop_bandwidth(100e3);

        idt8v97003_charge_pump_config_t sChargePump;

        sChargePump.fPositiveCurrent = 5e-3;
        sChargePump.fNegativeCurrent = 5e-3;
        sChargePump.fBleederCurrent = 0.0;

        idt8v97003_set_charge_pump_config(&sChargePump);

        idt8v97003_set_frequency(8e9, 1, 10, 100);

        idt8v97003_set_rf_output_power(0, 3);
        idt8v97003_enable_rf_output(0, 1);
        idt8v97003_set_rf_output_power(1, 3);
        idt8v97003_enable_rf_output(1, 1);

        idt8v97003_set_mute_until_locked(0);
        idt8v97003_mute(-1, 0); // Unmute all channels

        DBGPRINTLN_CTX("IDT8V97003 rev. %hhu (opt %hhu) init OK!", idt8v97003_get_chip_version(), idt8v97003_get_chip_option());

        {
            DBGPRINTLN_CTX("Synthesizer:");
            DBGPRINTLN_CTX("  Reference Input: %.6f MHz", idt8v97003_get_reference_frequency() * 1e-6);
            DBGPRINTLN_CTX("  Reference Doubler Input: %.6f MHz", idt8v97003_get_reference_doubler_input_frequency() * 1e-6);
            DBGPRINTLN_CTX("  Reference Doubler Output: %.6f MHz", idt8v97003_get_reference_doubler_output_frequency() * 1e-6);
            DBGPRINTLN_CTX("  Reference Multiplier Input: %.6f MHz", idt8v97003_get_reference_multiplier_input_frequency() * 1e-6);
            DBGPRINTLN_CTX("  Reference Multiplier Output: %.6f MHz", idt8v97003_get_reference_multiplier_output_frequency() * 1e-6);
            DBGPRINTLN_CTX("  Reference R-Divider Input: %.6f MHz", idt8v97003_get_reference_divider_input_frequency() * 1e-6);
            DBGPRINTLN_CTX("  Reference R-Divider Output: %.6f MHz", idt8v97003_get_reference_divider_output_frequency() * 1e-6);
            DBGPRINTLN_CTX("  PFD Frequency: %.6f MHz", idt8v97003_get_pfd_frequency() * 1e-6);

            idt8v97003_loop_filter_t lf;

            idt8v97003_get_loop_filter(&lf);

            DBGPRINTLN_CTX("  Loop Filter:");
            DBGPRINTLN_CTX("    Rs: %.3f Ohm", lf.fRs);
            DBGPRINTLN_CTX("    Cs: %.3f nF", lf.fCs * 1e9);
            DBGPRINTLN_CTX("    Cp: %.3f nF", lf.fCp * 1e9);
            DBGPRINTLN_CTX("    R3: %.3f Ohm", lf.fR3);
            DBGPRINTLN_CTX("    C3: %.3f nF", lf.fC3 * 1e9);

            DBGPRINTLN_CTX("  Charge Pump PMOS Current: %.3f mA", idt8v97003_get_charge_pump_positive_current() * 1e3);
            DBGPRINTLN_CTX("  Charge Pump NMOS Current: %.3f mA", idt8v97003_get_charge_pump_negative_current() * 1e3);
            DBGPRINTLN_CTX("  Charge Pump Bleeder Current: %.3f uA", idt8v97003_get_charge_pump_bleeder_current() * 1e6);

            idt8v97003_loop_freq_response_t lfr;

            idt8v97003_get_loop_frequency_response(&lfr);

            DBGPRINTLN_CTX("  Loop Frequency Response:");
            DBGPRINTLN_CTX("    Zero: %.3f kHz", lfr.fFz * 1e-3);
            DBGPRINTLN_CTX("    Bandwidth: %.3f kHz", lfr.fFc * 1e-3);
            DBGPRINTLN_CTX("    1st Pole: %.3f kHz", lfr.fFp * 1e-3);
            DBGPRINTLN_CTX("    2nd Pole: %.3f kHz", lfr.fFp2 * 1e-3);
            DBGPRINTLN_CTX("    Phase Margin: %.3f deg", lfr.fPhaseMargin);
            DBGPRINTLN_CTX("  Target Loop Bandwidth: %.3f kHz", idt8v97003_get_target_loop_bandwidth() * 1e-3);

            DBGPRINTLN_CTX("  VCO Frequency: %.6f MHz", idt8v97003_get_vco_frequency() * 1e-6);
            DBGPRINTLN_CTX("  Output Frequency: %.6f MHz", idt8v97003_get_frequency() * 1e-6);

            float dist = 0;
            uint8_t frac = idt8v97003_is_feedback_divider_fractional(&dist);

            if(frac)
                DBGPRINTLN_CTX("  Distance to integer boundary: %.6f %%", dist * 100);
            else
                DBGPRINTLN_CTX("  PLL in integer mode");

            if(idt8v97003_is_band_select_done())
                DBGPRINTLN_CTX("  PLL band selection is done!");
            else
                DBGPRINTLN_CTX("  PLL band selection is NOT done!");

            if(idt8v97003_is_pll_locked())
                DBGPRINTLN_CTX("  PLL is locked! (HW %hhu)", SYNTH_LOCKED());
            else
                DBGPRINTLN_CTX("  PLL is NOT locked! (HW %hhu)", SYNTH_LOCKED());

            DBGPRINTLN_CTX("  Output A:");
            DBGPRINTLN_CTX("    Enabled: %s", idt8v97003_is_rf_output_enabled(0) ? "YES" : "NO");
            DBGPRINTLN_CTX("    Muted: %s (HW %hhu)", idt8v97003_is_muted(0) ? "YES" : "NO", SYNTH_MUTED());
            DBGPRINTLN_CTX("    Power: %hhu", idt8v97003_get_rf_output_power(0));

            DBGPRINTLN_CTX("  Output B:");
            DBGPRINTLN_CTX("    Enabled: %s", idt8v97003_is_rf_output_enabled(1) ? "YES" : "NO");
            DBGPRINTLN_CTX("    Muted: %s (HW %hhu)", idt8v97003_is_muted(1) ? "YES" : "NO", SYNTH_MUTED());
            DBGPRINTLN_CTX("    Power: %hhu", idt8v97003_get_rf_output_power(1));
        }

        // idt8v97003_power_down(IDT8V97003_PWR_ALL);
    }
    else
    {
        // Bring all IOs low to avoid reverse biasing
        SYNTH_RESET();
        SYNTH_DISABLE();
        SYNTH_UNMUTE();
        SYNTH_SELECT();

        SYNTH_PWR_DIS();

        DBGPRINTLN_CTX("IDT8V97003 init NOK!");
    }

    return 0;
}
int main()
{
    // I2C Slave Register block
    i2c_slave_register_init();

    // Gain DAC
    rf_gain_vctrl_init();

    // rf_gain_set_tx_vctrl(0, 1.8);
    // rf_gain_set_tx_vctrl(1, 1.8);
    // rf_gain_set_rx_vctrl(0.0);

    while(1)
    {
        wdog_feed();

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

            LED2_TOGGLE();

            if(LED2_STATUS())
                ullLastHeartBeat -= 900;
        }

        if((g_ullSystemTick > 0 && ullLastTelemetryUpdate == 0) || g_ullSystemTick - ullLastTelemetryUpdate > 2000)
        {
            ullLastTelemetryUpdate = g_ullSystemTick;

            // System Temperatures
            float fADCTemp = adc_get_temperature();
            float fEMUTemp = emu_get_temperature();
            uint16_t usBoardTemp[3];

            for(uint8_t i = 0; i < 3; i++)
                usBoardTemp[i] = mcp9802_get_ambient_temp_reg(i);

            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                I2C_SLAVE_REGISTER(float, I2C_SLAVE_REGISTER_ADC_TEMP) = fADCTemp;
                I2C_SLAVE_REGISTER(float, I2C_SLAVE_REGISTER_EMU_TEMP) = fEMUTemp;
                I2C_SLAVE_REGISTER(uint16_t, I2C_SLAVE_REGISTER_BOARD_BOT_TEMP) = usBoardTemp[0];
                I2C_SLAVE_REGISTER(uint16_t, I2C_SLAVE_REGISTER_BOARD_SYNTH_TEMP) = usBoardTemp[1];
                I2C_SLAVE_REGISTER(uint16_t, I2C_SLAVE_REGISTER_BOARD_MMW_CONV_TEMP) = usBoardTemp[2];
            }

            // System Voltages/Currents
            float fAVDD = adc_get_avdd();
            float fDVDD = adc_get_dvdd();
            float fIOVDD = adc_get_iovdd();
            float fCoreVDD = adc_get_corevdd();

            ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
            {
                I2C_SLAVE_REGISTER(float, I2C_SLAVE_REGISTER_AVDD_VOLTAGE) = fAVDD;
                I2C_SLAVE_REGISTER(float, I2C_SLAVE_REGISTER_DVDD_VOLTAGE) = fDVDD;
                I2C_SLAVE_REGISTER(float, I2C_SLAVE_REGISTER_IOVDD_VOLTAGE) = fIOVDD;
                I2C_SLAVE_REGISTER(float, I2C_SLAVE_REGISTER_CORE_VOLTAGE) = fCoreVDD;
            }

            // RF Power Detector Voltages
            float fTXPwr = adc_get_tx_pwr_vdet();
            float fRXPwr = adc_get_rx_pwr_vdet();
            float fTXLOPwr = adc_get_tx_lo_vpwr();
            float fRXLOPwr = adc_get_rx_lo_vpwr();

            DBGPRINTLN_CTX("AVDD Voltage: %.2f mV", fAVDD);
            DBGPRINTLN_CTX("DVDD Voltage: %.2f mV", fDVDD);
            DBGPRINTLN_CTX("IOVDD Voltage: %.2f mV", fIOVDD);
            DBGPRINTLN_CTX("Core Voltage: %.2f mV", fCoreVDD);
            DBGPRINTLN_CTX("TX Power: %.2f mV", fTXPwr);
            DBGPRINTLN_CTX("RX Power: %.2f mV", fRXPwr);
            DBGPRINTLN_CTX("TX LO Power: %.2f mV", fTXLOPwr);
            DBGPRINTLN_CTX("RX LO Power: %.2f mV", fRXLOPwr);
            DBGPRINTLN_CTX("ADC temperature: %.2f C", fADCTemp);
            DBGPRINTLN_CTX("EMU temperature: %.2f C", fEMUTemp);
            for(uint8_t i = 0; i < 3; i++)
                DBGPRINTLN_CTX("Board temperature #%hhu: %.2f C", i, (int16_t)usBoardTemp[i] / 256.0);
            DBGPRINTLN_CTX("RTCC time: %lu s", rtcc_get_time());
            DBGPRINTLN_CTX("Synth locked: %s", SYNTH_LOCKED() ? "YES" : "NO");
        }
    }

    return 0;
}