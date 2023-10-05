#ifndef __R8V97003_H__
#define __R8V97003_H__

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "axi_quad_spi.h"
#include "axi_gpio.h"
#include "utils.h"
#include "debug_macros.h"

#define R8V97003_REG_INTF_CONFIG        0x00
#define R8V97003_REG_BUF_READ           0x01
#define R8V97003_REG_CHIP_TYPE          0x03
#define R8V97003_REG_CHIP_ID_LOW        0x04
#define R8V97003_REG_CHIP_ID_HIGH       0x05
#define R8V97003_REG_CHIP_VERSION       0x06
#define R8V97003_REG_CHIP_OPTION        0x07
#define R8V97003_REG_VENDOR_ID_LOW      0x0C
#define R8V97003_REG_VENDOR_ID_HIGH     0x0D
#define R8V97003_REG_BUF_TRANSFER       0x0F
#define R8V97003_REG_NINT_LOW           0x10
#define R8V97003_REG_NINT_HIGH          0x11
#define R8V97003_REG_NFRAC_LOW          0x12
#define R8V97003_REG_NFRAC_MID0         0x13
#define R8V97003_REG_NFRAC_MID1         0x14
#define R8V97003_REG_NFRAC_HIGH         0x15
#define R8V97003_REG_NMOD_LOW           0x16
#define R8V97003_REG_NMOD_MID0          0x17
#define R8V97003_REG_NMOD_MID1          0x18
#define R8V97003_REG_NMOD_HIGH          0x19
#define R8V97003_REG_PHASE_LOW          0x1A
#define R8V97003_REG_PHASE_MID0         0x1B
#define R8V97003_REG_PHASE_MID1         0x1C
#define R8V97003_REG_PHASE_HIGH         0x1D
#define R8V97003_REG_DSM_CTL            0x1E
#define R8V97003_REG_MANUAL_VCO         0x1F
#define R8V97003_REG_MANUAL_DIGITAL     0x20
#define R8V97003_REG_MANUAL_CTL         0x21
#define R8V97003_REG_BAND_SEL_DIV_LOW   0x22
#define R8V97003_REG_BAND_SEL_DIV_HIGH  0x23
#define R8V97003_REG_LD_CTL0            0x26
#define R8V97003_REG_LD_CTL1            0x27
#define R8V97003_REG_PWR_CTL            0x28
#define R8V97003_REG_RDIV_LOW           0x29
#define R8V97003_REG_RDIV_HIGH          0x2A
#define R8V97003_REG_MULT_CTL0          0x2B
#define R8V97003_REG_MULT_CTL1          0x2C
#define R8V97003_REG_ICP_PMOS           0x2D
#define R8V97003_REG_ICP_NMOS           0x2E
#define R8V97003_REG_ICP_BLEEDER        0x2F
#define R8V97003_REG_PFD_PULSE_WIDTH    0x30
#define R8V97003_REG_AUTO_RESYNC        0x32
#define R8V97003_REG_RFOUTA_PWR         0x33
#define R8V97003_REG_RFOUTA_ENA         0x34
#define R8V97003_REG_RFOUTB_PWR         0x35
#define R8V97003_REG_RFOUTB_ENA         0x36
#define R8V97003_REG_OUT_DIV_DBL        0x3B
#define R8V97003_REG_LD_CAL_VCO_STATUS  0x44
#define R8V97003_REG_DIG_BAND_STATUS    0x45

// R8V97003_REG_INTF_CONFIG
#define R8V97003_REG_INTF_CONFIG_SOFT_RESET (BIT(7) | BIT(0))
#define R8V97003_REG_INTF_CONFIG_LSB_FIRST  (BIT(6) | BIT(1))
#define R8V97003_REG_INTF_CONFIG_ADDR_ASC   (BIT(5) | BIT(2))
#define R8V97003_REG_INTF_CONFIG_SDO_ACTIVE (BIT(4) | BIT(3))

// R8V97003_REG_BUF_READ
#define R8V97003_REG_BUF_READ_BUF_READ_MODE BIT(5)

// R8V97003_REG_BUF_TRANSFER
#define R8V97003_REG_BUF_TRANSFER_TRANSFER_ON   BIT(0)

// R8V97003_REG_DSM_CTL
#define R8V97003_REG_DSM_CTL_DSM_ORDER_OFF      0x00
#define R8V97003_REG_DSM_CTL_DSM_ORDER_1ST      0x10
#define R8V97003_REG_DSM_CTL_DSM_ORDER_2ND      0x20
#define R8V97003_REG_DSM_CTL_DSM_ORDER_3RD      0x30
#define R8V97003_REG_DSM_CTL_DITHER_G_LSB       0x00
#define R8V97003_REG_DSM_CTL_DITHER_G_LSB_X2    0x04
#define R8V97003_REG_DSM_CTL_DITHER_G_LSB_X4    0x08
#define R8V97003_REG_DSM_CTL_DITHER_G_LSB_X8    0x0C
#define R8V97003_REG_DSM_CTL_SHAPE_DITHER_EN    BIT(1)
#define R8V97003_REG_DSM_CTL_DITHER_EN          BIT(0)

// R8V97003_REG_MANUAL_VCO
#define R8V97003_REG_MANUAL_VCO_MANU_BAND_EN    BIT(7)

// R8V97003_REG_MANUAL_CTL
#define R8V97003_REG_MANUAL_CTL_FORCE_RELOCK    BIT(7)
#define R8V97003_REG_MANUAL_CTL_PH_ADJ          BIT(6)
#define R8V97003_REG_MANUAL_CTL_BAND_SEL_DIS    BIT(5)
#define R8V97003_REG_MANUAL_CTL_MANUAL_RESYNC   BIT(4)
#define R8V97003_REG_MANUAL_CTL_BAND_SEL_ACC_4X 0x02
#define R8V97003_REG_MANUAL_CTL_BAND_SEL_ACC_8X 0x03

// R8V97003_REG_LD_CTL0
#define R8V97003_REG_LD_CTL0_LD_DISABLE        BIT(4)
#define R8V97003_REG_LD_CTL0_AUTO_RECAL_EN     BIT(3)

// R8V97003_REG_LD_CTL1
#define R8V97003_REG_LD_CTL1_LD_PIN_MODE_LD         0x00
#define R8V97003_REG_LD_CTL1_LD_PIN_MODE_CAL_DONE   0x10
#define R8V97003_REG_LD_CTL1_LD_PIN_MODE_LOW        0x20
#define R8V97003_REG_LD_CTL1_LD_PIN_MODE_HIGH       0x30
#define R8V97003_REG_LD_CTL1_LD_PRECISION_0p375ns   0x00
#define R8V97003_REG_LD_CTL1_LD_PRECISION_0p75ns    0x01
#define R8V97003_REG_LD_CTL1_LD_PRECISION_1p5ns     0x02
#define R8V97003_REG_LD_CTL1_LD_PRECISION_2p4ns     0x03
#define R8V97003_REG_LD_CTL1_LD_PRECISION_5p2ns     0x04
#define R8V97003_REG_LD_CTL1_LD_PRECISION_8p5ns     0x06

// R8V97003_REG_PWR_CTL
#define R8V97003_REG_PWR_CTL_REF_VREG_PDOWN         BIT(6)
#define R8V97003_REG_PWR_CTL_PDCP_VREG_PDOWN        BIT(5)
#define R8V97003_REG_PWR_CTL_FB_VREG_PDOWN          BIT(4)
#define R8V97003_REG_PWR_CTL_OUTA_VREG_PDOWN        BIT(3)
#define R8V97003_REG_PWR_CTL_OUTB_BUF_VREG_PDOWN    BIT(2)
#define R8V97003_REG_PWR_CTL_ANA_PDOWN              BIT(1)
#define R8V97003_REG_PWR_CTL_VCO_EN                 BIT(0)

// R8V97003_REG_RDIV_HIGH
#define R8V97003_REG_RDIV_HIGH_REF_DBL_DELAY        BIT(4)
#define R8V97003_REG_RDIV_HIGH_INPUT_TYPE           BIT(3)
#define R8V97003_REG_RDIV_HIGH_REF_DBL_EN           BIT(2)

// R8V97003_REG_MULT_CTL0
#define R8V97003_REG_MULT_CTL0_MULT_EN              BIT(7)
#define R8V97003_REG_MULT_CTL0_MULT_RESET           BIT(6)

// R8V97003_REG_MULT_CTL1
#define R8V97003_REG_MULT_CTL1_MULT_MUX_ENA         BIT(7)
#define R8V97003_REG_MULT_CTL1_MULT_D2S_ENA         BIT(6)
#define R8V97003_REG_MULT_CTL1_MULT_CP_ENA          BIT(5)
#define R8V97003_REG_MULT_CTL1_MULT_FORCE_VCHI      BIT(4)
#define R8V97003_REG_MULT_CTL1_MULT_FORCE_VCLOW     BIT(3)

// R8V97003_REG_ICP_BLEEDER
#define R8V97003_REG_ICP_BLEEDER_CP_HIZ             BIT(7)

// R8V97003_REG_PFD_PULSE_WIDTH
#define R8V97003_REG_PFD_PULSE_WIDTH_PW_260ps       0x00
#define R8V97003_REG_PFD_PULSE_WIDTH_PW_348ps       0x04
#define R8V97003_REG_PFD_PULSE_WIDTH_PW_487ps       0x08
#define R8V97003_REG_PFD_PULSE_WIDTH_PW_583ps       0x0C

// R8V97003_REG_AUTO_RESYNC
#define R8V97003_REG_AUTO_RESYNC_AUTO_RESYNC_EN     BIT(7)

// R8V97003_REG_RFOUTA_ENA
#define R8V97003_REG_RFOUTA_ENA_MUTE_UNTIL_LD       BIT(5)
#define R8V97003_REG_RFOUTA_ENA_RFOUTA_ENA          BIT(4)

// R8V97003_REG_RFOUTB_ENA
#define R8V97003_REG_RFOUTB_ENA_RFOUTB_ENA          BIT(4)

// R8V97003_REG_OUT_DIV_DBL
#define R8V97003_REG_OUT_DIV_DBL_OUT_DBL_ENA        BIT(7)
#define R8V97003_REG_OUT_DIV_DBL_OUT_DIV_ENA        BIT(6)
#define R8V97003_REG_OUT_DIV_DBL_OUT_DBL_FREQ       BIT(5)
#define R8V97003_REG_OUT_DIV_DBL_OUT_DIV_DIV2       0x01
#define R8V97003_REG_OUT_DIV_DBL_OUT_DIV_DIV4       0x02
#define R8V97003_REG_OUT_DIV_DBL_OUT_DIV_DIV8       0x03
#define R8V97003_REG_OUT_DIV_DBL_OUT_DIV_DIV16      0x04
#define R8V97003_REG_OUT_DIV_DBL_OUT_DIV_DIV32      0x05

// R8V97003_REG_LD_CAL_VCO_STATUS
#define R8V97003_REG_LD_CAL_VCO_STATUS_DIG_LOCK         BIT(7)
#define R8V97003_REG_LD_CAL_VCO_STATUS_BAND_SEL_DONE    BIT(6)
#define R8V97003_REG_LD_CAL_VCO_STATUS_VCO_STS          0x0F


typedef struct r8v97003_mixed_number_t r8v97003_mixed_number_t;

struct r8v97003_mixed_number_t
{
    uint32_t ulInt; // Integer part
    uint32_t ulNum; // Fractional part numerator
    uint32_t ulDen; // Fractional part denominator
};

uint8_t r8v97003_init();

uint8_t r8v97003_get_chip_type();
uint16_t r8v97003_get_chip_id();
uint8_t r8v97003_get_chip_version();
uint8_t r8v97003_get_chip_option();
uint16_t r8v97003_get_vendor_id();

uint8_t r8v97003_pfd_config(uint32_t ulRefFreq, uint8_t ubDiff, uint8_t ubRefDouble, uint8_t ubMult, uint16_t usRDiv);

double r8v97003_get_cp_pmos_current();
double r8v97003_get_cp_nmos_current();
double r8v97003_get_cp_bleeder_current();
uint8_t r8v97003_set_cp_current(double dPMOS, double dNMOS, double dBleeder);

#endif // __R8V97003_H__