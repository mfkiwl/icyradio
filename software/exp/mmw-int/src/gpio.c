#include "gpio.h"

static void gpio_isr(uint32_t ulFlags)
{
    // extern void XXXX_isr();

    // if(ulFlags & BIT(0))
    //     XXXX_isr();
}
void _gpio_even_isr()
{
    uint32_t ulFlags = GPIO->IF;

    gpio_isr(ulFlags & 0x55555555);

    GPIO->IFC = 0x55555555; // Clear all even flags
}
void _gpio_odd_isr()
{
    uint32_t ulFlags = GPIO->IF;

    gpio_isr(ulFlags & 0xAAAAAAAA);

    GPIO->IFC = 0xAAAAAAAA; // Clear all odd flags
}

void gpio_init()
{
    cmu_hfbus_clock_gate(CMU_HFBUSCLKEN0_GPIO, 1);

    // NC - Not Connected (not available in mcu package)
    // NR - Not routed (no routing to pin on pcb, floating)
    // NU - Not used (not currently in use)

    // Port A
    GPIO->P[0].CTRL   = GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG | (5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT)
                      | GPIO_P_CTRL_DRIVESTRENGTH_STRONG | (5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
    GPIO->P[0].MODEL  = GPIO_P_MODEL_MODE0_PUSHPULL                 // SYNTH_SPI_MOSI - USART3 - Location 0
                      | GPIO_P_MODEL_MODE1_DISABLED                 // SYNTH_SPI_MISO - USART3 - Location 0 (Not in use)
                      | GPIO_P_MODEL_MODE2_PUSHPULL                 // SYNTH_SPI_SCK - USART3 - Location 0
                      | GPIO_P_MODEL_MODE3_PUSHPULL                 // SYNTH_SPI_CSn (- USART3 - Location 0)
                      | GPIO_P_MODEL_MODE4_PUSHPULL                 // SYNTH_MUTE
                      | GPIO_P_MODEL_MODE5_INPUTPULLFILTER          // SYNTH_LD
                      | GPIO_P_MODEL_MODE6_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE7_DISABLED;                // NC
    GPIO->P[0].MODEH  = GPIO_P_MODEH_MODE8_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE9_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE10_DISABLED                // NC
                      | GPIO_P_MODEH_MODE11_DISABLED                // NC
                      | GPIO_P_MODEH_MODE12_PUSHPULL                // LED1
                      | GPIO_P_MODEH_MODE13_PUSHPULL                // LED2
                      | GPIO_P_MODEH_MODE14_PUSHPULL                // TX_DIG_PWR_EN
                      | GPIO_P_MODEH_MODE15_DISABLED;               // NC
    GPIO->P[0].DOUT   = 0; // SYNTH_SPI_CSn can be low because power is off
    GPIO->P[0].OVTDIS = 0;

    // Port B
    GPIO->P[1].CTRL   = GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG | (5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT)
                      | GPIO_P_CTRL_DRIVESTRENGTH_STRONG | (5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
    GPIO->P[1].MODEL  = GPIO_P_MODEL_MODE0_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE1_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE2_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE3_PUSHPULL                 // EXT_LO_ATT_SERIN - USART2 - Location 1
                      | GPIO_P_MODEL_MODE4_PUSHPULL                 // EXT_LO_ATT_LE
                      | GPIO_P_MODEL_MODE5_PUSHPULL                 // EXT_LO_ATT_CLK - USART2 - Location 1
                      | GPIO_P_MODEL_MODE6_PUSHPULL                 // TX_RST
                      | GPIO_P_MODEL_MODE7_PUSHPULL;                // RF_GP_PWR_EN
    GPIO->P[1].MODEH  = GPIO_P_MODEH_MODE8_DISABLED                 // NR
                      | GPIO_P_MODEH_MODE9_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE10_DISABLED                // NC
                      | GPIO_P_MODEH_MODE11_PUSHPULL                // RX_DIG_PWR_EN
                      | GPIO_P_MODEH_MODE12_DISABLED                // NC
                      | GPIO_P_MODEH_MODE13_PUSHPULL                // RX_ANA_PWR_EN
                      | GPIO_P_MODEH_MODE14_PUSHPULL                // SYNTH_PWR_EN
                      | GPIO_P_MODEH_MODE15_DISABLED;               // NC
    GPIO->P[1].DOUT   = 0; // EXT_LO_ATT_LE can be low because power is off
    GPIO->P[1].OVTDIS = 0;

    // Port C
    GPIO->P[2].CTRL   = GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG | (5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT)
                      | GPIO_P_CTRL_DRIVESTRENGTH_STRONG | (5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
    GPIO->P[2].MODEL  = GPIO_P_MODEL_MODE0_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE1_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE2_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE3_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE4_PUSHPULL                 // LO_ANA_PWR_EN
                      | GPIO_P_MODEL_MODE5_PUSHPULL                 // TX_ANA_PWR_EN
                      | GPIO_P_MODEL_MODE6_DISABLED                 // EXT_IO_1_3
                      | GPIO_P_MODEL_MODE7_DISABLED;                // EXT_IO_1_4
    GPIO->P[2].MODEH  = GPIO_P_MODEH_MODE8_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE9_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE10_DISABLED                // NC
                      | GPIO_P_MODEH_MODE11_DISABLED                // NC
                      | GPIO_P_MODEH_MODE12_DISABLED                // NC
                      | GPIO_P_MODEH_MODE13_DISABLED                // NC
                      | GPIO_P_MODEH_MODE14_DISABLED                // NC
                      | GPIO_P_MODEH_MODE15_DISABLED;               // NC
    GPIO->P[2].DOUT   = 0;
    GPIO->P[2].OVTDIS = 0;

    // Port D
    GPIO->P[3].CTRL   = GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG | (5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT)
                      | GPIO_P_CTRL_DRIVESTRENGTH_STRONG | (5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
    GPIO->P[3].MODEL  = GPIO_P_MODEL_MODE0_DISABLED                 // RX_PWR_VDET_SCALED
                      | GPIO_P_MODEL_MODE1_DISABLED                 // TX_PWR_VDET_SCALED
                      | GPIO_P_MODEL_MODE2_DISABLED                 // RX_LO_VPWR_SCALED
                      | GPIO_P_MODEL_MODE3_DISABLED                 // TX_LO_VPWR_SCALED
                      | GPIO_P_MODEL_MODE4_WIREDANDPULLUPFILTER     // MCU_I2C_SDA - I2C1 - Location 3
                      | GPIO_P_MODEL_MODE5_WIREDANDPULLUPFILTER     // MCU_I2C_SCL - I2C1 - Location 3
                      | GPIO_P_MODEL_MODE6_WIREDANDPULLUPFILTER     // DAC_I2C_SDA - I2C0 - Location 1
                      | GPIO_P_MODEL_MODE7_WIREDANDPULLUPFILTER;    // DAC_I2C_SCL - I2C0 - Location 1
    GPIO->P[3].MODEH  = GPIO_P_MODEH_MODE8_DISABLED                 // EXT_IO_1_5
                      | GPIO_P_MODEH_MODE9_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE10_DISABLED                // NC
                      | GPIO_P_MODEH_MODE11_DISABLED                // NC
                      | GPIO_P_MODEH_MODE12_DISABLED                // NC
                      | GPIO_P_MODEH_MODE13_DISABLED                // NC
                      | GPIO_P_MODEH_MODE14_DISABLED                // NC
                      | GPIO_P_MODEH_MODE15_DISABLED;               // NC
    GPIO->P[3].DOUT   = BIT(4) | BIT(5) | BIT(6) | BIT(7);
    GPIO->P[3].OVTDIS = BIT(0) | BIT(1) | BIT(2) | BIT(3);

    // Port E
    GPIO->P[4].CTRL   = GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG | (5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT)
                      | GPIO_P_CTRL_DRIVESTRENGTH_STRONG | (5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
    GPIO->P[4].MODEL  = GPIO_P_MODEL_MODE0_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE1_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE2_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE3_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE4_PUSHPULL                 // RX_LO_DST_SEL
                      | GPIO_P_MODEL_MODE5_PUSHPULL                 // RX_LO_SRC_SEL
                      | GPIO_P_MODEL_MODE6_PUSHPULL                 // TX_LO_DST_SEL
                      | GPIO_P_MODEL_MODE7_PUSHPULL;                // TX_LO_SRC_SEL
    GPIO->P[4].MODEH  = GPIO_P_MODEH_MODE8_WIREDANDFILTER           // TSENS_I2C_SDA - I2C2 - Location 0
                      | GPIO_P_MODEH_MODE9_PUSHPULL                 // TSENS_VDD
                      | GPIO_P_MODEH_MODE10_PUSHPULL                // RF_SPI_MOSI - USART0 - Location 0
                      | GPIO_P_MODEH_MODE11_INPUTPULL               // RF_SPI_MISO - USART0 - Location 0
                      | GPIO_P_MODEH_MODE12_PUSHPULL                // RF_SPI_SCK - USART0 - Location 0
                      | GPIO_P_MODEH_MODE13_PUSHPULL                // SYNTH_RESETn
                      | GPIO_P_MODEH_MODE14_PUSHPULL                // SYNTH_SYNC
                      | GPIO_P_MODEH_MODE15_PUSHPULL;               // SYNTH_CE
    GPIO->P[4].DOUT   = 0; // TSENS_I2C_SDA can be low because power is off
    GPIO->P[4].OVTDIS = 0;

    // Port F
    GPIO->P[5].CTRL   = GPIO_P_CTRL_DRIVESTRENGTHALT_STRONG | (5 << _GPIO_P_CTRL_SLEWRATEALT_SHIFT)
                      | GPIO_P_CTRL_DRIVESTRENGTH_STRONG | (5 << _GPIO_P_CTRL_SLEWRATE_SHIFT);
    GPIO->P[5].MODEL  = GPIO_P_MODEL_MODE0_PUSHPULL                 // SWCLK - Location 0
                      | GPIO_P_MODEL_MODE1_PUSHPULL                 // SWDIO - Location 0
                      | GPIO_P_MODEL_MODE2_PUSHPULL                 // SWO - Location 0
                      | GPIO_P_MODEL_MODE3_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE4_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE5_WIREDANDFILTER           // TSENS_I2C_SCL - I2C2 - Location 0
                      | GPIO_P_MODEL_MODE6_DISABLED                 // NC
                      | GPIO_P_MODEL_MODE7_DISABLED;                // NC
    GPIO->P[5].MODEH  = GPIO_P_MODEH_MODE8_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE9_DISABLED                 // NC
                      | GPIO_P_MODEH_MODE10_PUSHPULL                // RX_RST
                      | GPIO_P_MODEH_MODE11_PUSHPULL                // RX_SPI_CSn
                      | GPIO_P_MODEH_MODE12_PUSHPULL                // TX_SPI_CSn
                      | GPIO_P_MODEH_MODE13_DISABLED                // NC
                      | GPIO_P_MODEH_MODE14_DISABLED                // NC
                      | GPIO_P_MODEH_MODE15_DISABLED;               // NC
    GPIO->P[5].DOUT   = BIT(11) | BIT(12); // TSENS_I2C_SCL can be low because power is off
    GPIO->P[5].OVTDIS = 0;

    // Debugger Route
    GPIO->ROUTEPEN &= ~(GPIO_ROUTEPEN_TDIPEN | GPIO_ROUTEPEN_TDOPEN);   // Disable JTAG
    GPIO->ROUTEPEN |= GPIO_ROUTEPEN_SWVPEN;                             // Enable SWO
    GPIO->ROUTELOC0 = GPIO_ROUTELOC0_SWVLOC_LOC0;                       // SWO on PF2

    // External interrupts
    GPIO->EXTIPSELL = GPIO_EXTIPSELL_EXTIPSEL0_PORTA            // NU
                    | GPIO_EXTIPSELL_EXTIPSEL1_PORTA            // NU
                    | GPIO_EXTIPSELL_EXTIPSEL2_PORTA            // NU
                    | GPIO_EXTIPSELL_EXTIPSEL3_PORTA            // NU
                    | GPIO_EXTIPSELL_EXTIPSEL4_PORTA            // NU
                    | GPIO_EXTIPSELL_EXTIPSEL5_PORTA            // NU
                    | GPIO_EXTIPSELL_EXTIPSEL6_PORTA            // NU
                    | GPIO_EXTIPSELL_EXTIPSEL7_PORTA;           // NU
    GPIO->EXTIPSELH = GPIO_EXTIPSELH_EXTIPSEL8_PORTA            // NU
                    | GPIO_EXTIPSELH_EXTIPSEL9_PORTA            // NU
                    | GPIO_EXTIPSELH_EXTIPSEL10_PORTA           // NU
                    | GPIO_EXTIPSELH_EXTIPSEL11_PORTA           // NU
                    | GPIO_EXTIPSELH_EXTIPSEL12_PORTA           // NU
                    | GPIO_EXTIPSELH_EXTIPSEL13_PORTA           // NU
                    | GPIO_EXTIPSELH_EXTIPSEL14_PORTA           // NU
                    | GPIO_EXTIPSELH_EXTIPSEL15_PORTA;          // NU

    GPIO->EXTIPINSELL = GPIO_EXTIPINSELL_EXTIPINSEL0_PIN0       // NU
                      | GPIO_EXTIPINSELL_EXTIPINSEL1_PIN0       // NU
                      | GPIO_EXTIPINSELL_EXTIPINSEL2_PIN0       // NU
                      | GPIO_EXTIPINSELL_EXTIPINSEL3_PIN0       // NU
                      | GPIO_EXTIPINSELL_EXTIPINSEL4_PIN4       // NU
                      | GPIO_EXTIPINSELL_EXTIPINSEL5_PIN4       // NU
                      | GPIO_EXTIPINSELL_EXTIPINSEL6_PIN4       // NU
                      | GPIO_EXTIPINSELL_EXTIPINSEL7_PIN4;      // NU
    GPIO->EXTIPINSELH = GPIO_EXTIPINSELH_EXTIPINSEL8_PIN8       // NU
                      | GPIO_EXTIPINSELH_EXTIPINSEL9_PIN8       // NU
                      | GPIO_EXTIPINSELH_EXTIPINSEL10_PIN8      // NU
                      | GPIO_EXTIPINSELH_EXTIPINSEL11_PIN8      // NU
                      | GPIO_EXTIPINSELH_EXTIPINSEL12_PIN12     // SW
                      | GPIO_EXTIPINSELH_EXTIPINSEL13_PIN12     // NU
                      | GPIO_EXTIPINSELH_EXTIPINSEL14_PIN12     // NU
                      | GPIO_EXTIPINSELH_EXTIPINSEL15_PIN12;    // NU

    GPIO->EXTIRISE = 0; // ...
    GPIO->EXTIFALL = 0; // ...

    GPIO->IFC = _GPIO_IFC_MASK; // Clear pending IRQs
    IRQ_CLEAR(GPIO_EVEN_IRQn); // Clear pending vector
    IRQ_CLEAR(GPIO_ODD_IRQn); // Clear pending vector
    IRQ_SET_PRIO(GPIO_EVEN_IRQn, 0, 0); // Set priority 0,0 (max)
    IRQ_SET_PRIO(GPIO_ODD_IRQn, 0, 0); // Set priority 0,0 (max)
    IRQ_ENABLE(GPIO_EVEN_IRQn); // Enable vector
    IRQ_ENABLE(GPIO_ODD_IRQn); // Enable vector
    GPIO->IEN = 0; // Enable interrupts
}