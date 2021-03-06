#ifndef __TSCS25XX_H__
#define __TSCS25XX_H__

#include <em_device.h>
#include <stdlib.h>
#include <math.h>
#include "systick.h"
#include "utils.h"
#include "biquad.h"
#include "atomic.h"
#include "gpio.h"
#include "i2c.h"

#define TSCS25XX_I2C_ADDR 0x71

// Registers
#define TSCS25XX_REG_HPVOLL             0x00
#define TSCS25XX_REG_HPVOLR             0x01
#define TSCS25XX_REG_SPKVOLL            0x02 // Unused in TSCS25xx
#define TSCS25XX_REG_SPKVOLR            0x03 // Unused in TSCS25xx
#define TSCS25XX_REG_DACVOLL            0x04
#define TSCS25XX_REG_DACVOLR            0x05
#define TSCS25XX_REG_ADCVOLL            0x06
#define TSCS25XX_REG_ADCVOLR            0x07
#define TSCS25XX_REG_INVOLL             0x08
#define TSCS25XX_REG_INVOLR             0x09
#define TSCS25XX_REG_VUCTL              0x0A
#define TSCS25XX_REG_INMODE             0x0B
#define TSCS25XX_REG_INSELL             0x0C
#define TSCS25XX_REG_INSELR             0x0D
#define TSCS25XX_REG_ALC0               0x0E
#define TSCS25XX_REG_ALC1               0x0F
#define TSCS25XX_REG_ALC2               0x10
#define TSCS25XX_REG_ALC3               0x11
#define TSCS25XX_REG_NGATE              0x12
#define TSCS25XX_REG_AIC1               0x13
#define TSCS25XX_REG_AIC2               0x14
#define TSCS25XX_REG_AIC3               0x15
#define TSCS25XX_REG_CNVRTR0            0x16
#define TSCS25XX_REG_ADCSR              0x17
#define TSCS25XX_REG_CNVRTR1            0x18
#define TSCS25XX_REG_DACSR              0x19
#define TSCS25XX_REG_PWRM1              0x1A
#define TSCS25XX_REG_PWRM2              0x1B
#define TSCS25XX_REG_CTL                0x1C // Partially unused in TSCS25xx
#define TSCS25XX_REG_THERMST            0x1D // Unused in TSCS25xx
#define TSCS25XX_REG_THERMSPKR1         0x1E // Unused in TSCS25xx
#define TSCS25XX_REG_CONFIG0            0x1F
#define TSCS25XX_REG_CONFIG1            0x20
#define TSCS25XX_REG_GAINCTL            0x21
#define TSCS25XX_REG_COP1               0x22 // Unused in TSCS25xx
#define TSCS25XX_REG_COP2               0x23 // Unused in TSCS25xx
#define TSCS25XX_REG_DMICCTL            0x24 // Unused in TSCS25xx [RESERVED]
#define TSCS25XX_REG_CLECTL             0x25
#define TSCS25XX_REG_MUGAIN             0x26
#define TSCS25XX_REG_COMPTH             0x27
#define TSCS25XX_REG_CMPRAT             0x28
#define TSCS25XX_REG_CATKTCL            0x29
#define TSCS25XX_REG_CATKTCH            0x2A
#define TSCS25XX_REG_CRELTCL            0x2B
#define TSCS25XX_REG_CRELTCH            0x2C
#define TSCS25XX_REG_LIMTH              0x2D
#define TSCS25XX_REG_LIMTGT             0x2E
#define TSCS25XX_REG_LATKTCL            0x2F
#define TSCS25XX_REG_LATKTCH            0x30
#define TSCS25XX_REG_LRELTCL            0x31
#define TSCS25XX_REG_LRELTCH            0x32
#define TSCS25XX_REG_EXPTH              0x33
#define TSCS25XX_REG_EXPRAT             0x34
#define TSCS25XX_REG_XATKTCL            0x35
#define TSCS25XX_REG_XATKTCH            0x36
#define TSCS25XX_REG_XRELTCL            0x37
#define TSCS25XX_REG_XRELTCH            0x38
#define TSCS25XX_REG_FXCTL              0x39
#define TSCS25XX_REG_DACCRWRL           0x3A
#define TSCS25XX_REG_DACCRWRM           0x3B
#define TSCS25XX_REG_DACCRWRH           0x3C
#define TSCS25XX_REG_DACCRRDL           0x3D
#define TSCS25XX_REG_DACCRRDM           0x3E
#define TSCS25XX_REG_DACCRRDH           0x3F
#define TSCS25XX_REG_DACCRADDR          0x40
#define TSCS25XX_REG_DCOFSEL            0x41
#define TSCS25XX_REG_PWM0               0x42 // Unused in TSCS25xx
#define TSCS25XX_REG_PWM1               0x43 // Unused in TSCS25xx
#define TSCS25XX_REG_PWM2               0x44 // Unused in TSCS25xx
#define TSCS25XX_REG_PWM3               0x45 // Unused in TSCS25xx
#define TSCS25XX_REG_PLLCTL9            0x4E
#define TSCS25XX_REG_PLLCTLA            0x4F
#define TSCS25XX_REG_PLLCTLB            0x50
#define TSCS25XX_REG_PLLCTLC            0x51
#define TSCS25XX_REG_PLLCTLD            0x52
#define TSCS25XX_REG_PLLCTLE            0x53
#define TSCS25XX_REG_PLLCTLF            0x54
#define TSCS25XX_REG_PLLCTL10           0x55
#define TSCS25XX_REG_PLLCTL11           0x56
#define TSCS25XX_REG_PLLCTL12           0x57
#define TSCS25XX_REG_PLLCTL1B           0x60
#define TSCS25XX_REG_PLLCTL1C           0x61
#define TSCS25XX_REG_PLLCTL22           0x67 // Unused in TSCS25xx
#define TSCS25XX_REG_DTEST0             0x68 // [RESERVED]
#define TSCS25XX_REG_DTEST1             0x69 // [RESERVED]
#define TSCS25XX_REG_ATEST0             0x6A // [RESERVED]
#define TSCS25XX_REG_ATEST1             0x6B // [RESERVED]
#define TSCS25XX_REG_ATEST2             0x6C // [RESERVED]
#define TSCS25XX_REG_ATEST3             0x6D // [RESERVED]
#define TSCS25XX_REG_ATEST4             0x6E // [RESERVED]
#define TSCS25XX_REG_ATEST5             0x6F // [RESERVED]
#define TSCS25XX_REG_ATEST6             0x70 // [RESERVED]
#define TSCS25XX_REG_ATEST7             0x71 // [RESERVED]
#define TSCS25XX_REG_ATEST8             0x72 // [RESERVED]
#define TSCS25XX_REG_ATEST9             0x73 // [RESERVED]
#define TSCS25XX_REG_ATEST10            0x74 // [RESERVED]
#define TSCS25XX_REG_ATEST11            0x75 // [RESERVED]
#define TSCS25XX_REG_ATEST12            0x76 // [RESERVED]
#define TSCS25XX_REG_TIMEBASE           0x77
#define TSCS25XX_REG_MFG0               0x78 // [RESERVED]
#define TSCS25XX_REG_MFG1               0x79 // [RESERVED]
#define TSCS25XX_REG_MFG2               0x7A // [RESERVED]
#define TSCS25XX_REG_MFG3               0x7B // [RESERVED]
#define TSCS25XX_REG_DEVADR             0x7C
#define TSCS25XX_REG_DEVIDL             0x7D
#define TSCS25XX_REG_DEVIDH             0x7E
#define TSCS25XX_REG_REVID              0x7F
#define TSCS25XX_REG_RESET              0x80
#define TSCS25XX_REG_THERMSPKR2         0x88 // Unused in TSCS25xx
#define TSCS25XX_REG_COP3               0x89 // Unused in TSCS25xx
#define TSCS25XX_REG_DACCRSTAT          0x8A
#define TSCS25XX_REG_HPDETSTAT          0x8B
#define TSCS25XX_REG_ADCALCSTAT         0x8C // [RESERVED]
#define TSCS25XX_REG_SD_STAT            0x8D // [RESERVED]
#define TSCS25XX_REG_PLLCTL0            0x8E
#define TSCS25XX_REG_PLLREFSEL          0x8F
#define TSCS25XX_REG_BISTCTL            0x90 // [RESERVED]
#define TSCS25XX_REG_BISTSTAT1          0x91 // [RESERVED]
#define TSCS25XX_REG_BISTSTAT2          0x92 // [RESERVED]
#define TSCS25XX_REG_BISTSTAT3          0x93 // [RESERVED]
#define TSCS25XX_REG_BISTSTAT4          0x94 // [RESERVED]
#define TSCS25XX_REG_BISTSTAT5          0x95 // [RESERVED]
#define TSCS25XX_REG_ATEST13            0xA0 // [RESERVED]
#define TSCS25XX_REG_ATEST14            0xA1 // [RESERVED]
#define TSCS25XX_REG_ATEST15            0xA2 // [RESERVED]
#define TSCS25XX_REG_ATEST16            0xA3 // [RESERVED]
#define TSCS25XX_REG_ATEST17            0xA4 // [RESERVED]
#define TSCS25XX_REG_ATEST18            0xA5 // [RESERVED]
#define TSCS25XX_REG_TESTSTAT0          0xB0 // [RESERVED]
#define TSCS25XX_REG_GPIOCTL1           0xC0
#define TSCS25XX_REG_GPIOCTL2           0xC1
#define TSCS25XX_REG_GPIODATA           0xC2
#define TSCS25XX_REG_ADCPCMCTL1         0xC3
#define TSCS25XX_REG_ADCPCMCTL2         0xC4
#define TSCS25XX_REG_DACPCMCTL1         0xC5
#define TSCS25XX_REG_DACPCMCTL2         0xC6
#define TSCS25XX_REG_DACMBCEN           0xC7
#define TSCS25XX_REG_DACMBCCTL          0xC8
#define TSCS25XX_REG_DACMBCMUGn(n)      (0xC9 + (n) * 7)
#define TSCS25XX_REG_DACMBCTHRn(n)      (0xCA + (n) * 7)
#define TSCS25XX_REG_DACMBCRATn(n)      (0xCB + (n) * 7)
#define TSCS25XX_REG_DACMBCATKnL(n)     (0xCC + (n) * 7)
#define TSCS25XX_REG_DACMBCATKnH(n)     (0xCD + (n) * 7)
#define TSCS25XX_REG_DACMBCRELnL(n)     (0xCE + (n) * 7)
#define TSCS25XX_REG_DACMBCRELnH(n)     (0xCF + (n) * 7)

// TSCS25XX_REG_INVOLL
#define TSCS25XX_REG_INVOLL_INMUTEL_UNMUTE  0x00
#define TSCS25XX_REG_INVOLL_INMUTEL_MUTE    0x80
#define TSCS25XX_REG_INVOLL_IZCL_IMMED      0x00
#define TSCS25XX_REG_INVOLL_IZCL_ZCROSS     0x40

// TSCS25XX_REG_INVOLR
#define TSCS25XX_REG_INVOLR_IZCR_IMMED      0x00
#define TSCS25XX_REG_INVOLR_IZCR_ZCROSS     0x40

// TSCS25XX_REG_VUCTL
#define TSCS25XX_REG_VUCTL_ADC_IMMED        0x00
#define TSCS25XX_REG_VUCTL_ADC_FADE         0x80
#define TSCS25XX_REG_VUCTL_DAC_IMMED        0x00
#define TSCS25XX_REG_VUCTL_DAC_FADE         0x40
#define TSCS25XX_REG_VUCTL_INVOLU_IMMED     0x00
#define TSCS25XX_REG_VUCTL_INVOLU_HOLD      0x10
#define TSCS25XX_REG_VUCTL_ADCVOLU_IMMED    0x00
#define TSCS25XX_REG_VUCTL_ADCVOLU_HOLD     0x08
#define TSCS25XX_REG_VUCTL_DACVOLU_IMMED    0x00
#define TSCS25XX_REG_VUCTL_DACVOLU_HOLD     0x04
#define TSCS25XX_REG_VUCTL_SPKVOLU_IMMED    0x00
#define TSCS25XX_REG_VUCTL_SPKVOLU_HOLD     0x02
#define TSCS25XX_REG_VUCTL_HPVOLU_IMMED     0x00
#define TSCS25XX_REG_VUCTL_HPVOLU_HOLD      0x01

// TSCS25XX_REG_INMODE
#define TSCS25XX_REG_INMODE_DS_IN_1         0x00
#define TSCS25XX_REG_INMODE_DS_IN_2         0x01

// TSCS25XX_REG_INSELL
#define TSCS25XX_REG_INSELL_INSEL_1         0x00
#define TSCS25XX_REG_INSELL_INSEL_2         0x40
#define TSCS25XX_REG_INSELL_INSEL_D2S       0xC0
#define TSCS25XX_REG_INSELL_MICBST_BYP      0x00
#define TSCS25XX_REG_INSELL_MICBST_10DB     0x10
#define TSCS25XX_REG_INSELL_MICBST_20DB     0x20
#define TSCS25XX_REG_INSELL_MICBST_30DB     0x30

// TSCS25XX_REG_INSELR
#define TSCS25XX_REG_INSELR_INSEL_1         0x00
#define TSCS25XX_REG_INSELR_INSEL_2         0x40
#define TSCS25XX_REG_INSELR_INSEL_D2S       0xC0
#define TSCS25XX_REG_INSELR_MICBST_BYP      0x00
#define TSCS25XX_REG_INSELR_MICBST_10DB     0x10
#define TSCS25XX_REG_INSELR_MICBST_20DB     0x20
#define TSCS25XX_REG_INSELR_MICBST_30DB     0x30

// TSCS25XX_REG_ALC0
#define TSCS25XX_REG_ALC0_ALC_MODE_ALC      0x00
#define TSCS25XX_REG_ALC0_ALC_MODE_LIMITER  0x04
#define TSCS25XX_REG_ALC0_ALC_SEL_OFF       0x00
#define TSCS25XX_REG_ALC0_ALC_SEL_RIGHT     0x01
#define TSCS25XX_REG_ALC0_ALC_SEL_LEFT      0x02
#define TSCS25XX_REG_ALC0_ALC_SEL_STEREO    0x03

// TSCS25XX_REG_NGATE
#define TSCS25XX_REG_NGATE_NGG_CONSTANT     0x00
#define TSCS25XX_REG_NGATE_NGG_MUTE         0x02
#define TSCS25XX_REG_NGATE_NGAT_DISABLE     0x00
#define TSCS25XX_REG_NGATE_NGAT_ENABLE      0x01

// TSCS25XX_REG_AIC1
#define TSCS25XX_REG_AIC1_BCLK_NOT_INV      0x00
#define TSCS25XX_REG_AIC1_BCLK_INV          0x40
#define TSCS25XX_REG_AIC1_MS_SLAVE          0x00
#define TSCS25XX_REG_AIC1_MS_MASTER         0x20
#define TSCS25XX_REG_AIC1_LRCLK_NOT_INV     0x00
#define TSCS25XX_REG_AIC1_LRCLK_INV         0x10
#define TSCS25XX_REG_AIC1_WL_16_BIT         0x00
#define TSCS25XX_REG_AIC1_WL_20_BIT         0x04
#define TSCS25XX_REG_AIC1_WL_24_BIT         0x08
#define TSCS25XX_REG_AIC1_WL_32_BIT         0x0C
#define TSCS25XX_REG_AIC1_FORMAT_RIGHT      0x00
#define TSCS25XX_REG_AIC1_FORMAT_LEFT       0x01
#define TSCS25XX_REG_AIC1_FORMAT_I2S        0x02

// TSCS25XX_REG_AIC2
#define TSCS25XX_REG_AIC2_DAC_STEREO        0x00
#define TSCS25XX_REG_AIC2_DAC_MONO_LEFT     0x40
#define TSCS25XX_REG_AIC2_DAC_MONO_RIGHT    0x80
#define TSCS25XX_REG_AIC2_DAC_STEREO_INV    0xC0
#define TSCS25XX_REG_AIC2_ADC_STEREO        0x00
#define TSCS25XX_REG_AIC2_ADC_MONO_LEFT     0x10
#define TSCS25XX_REG_AIC2_ADC_MONO_RIGHT    0x20
#define TSCS25XX_REG_AIC2_ADC_STEREO_INV    0x30
#define TSCS25XX_REG_AIC2_TRI_OUTPUT        0x00
#define TSCS25XX_REG_AIC2_TRI_TRISTATE      0x08

// TSCS25XX_REG_AIC3
#define TSCS25XX_REG_AIC3_ADOPD_ENABLED     0x00
#define TSCS25XX_REG_AIC3_ADOPD_DISABLED    0x20
#define TSCS25XX_REG_AIC3_ALRPD_ENABLED     0x00
#define TSCS25XX_REG_AIC3_ALRPD_DISABLED    0x10
#define TSCS25XX_REG_AIC3_ABCPD_ENABLED     0x00
#define TSCS25XX_REG_AIC3_ABCPD_DISABLED    0x08
#define TSCS25XX_REG_AIC3_DDIPD_ENABLED     0x00
#define TSCS25XX_REG_AIC3_DDIPD_DISABLED    0x04
#define TSCS25XX_REG_AIC3_DLRPD_ENABLED     0x00
#define TSCS25XX_REG_AIC3_DLRPD_DISABLED    0x02
#define TSCS25XX_REG_AIC3_DBCPD_ENABLED     0x00
#define TSCS25XX_REG_AIC3_DBCPD_DISABLED    0x01

// TSCS25XX_REG_CNVRTR0
#define TSCS25XX_REG_CNVRTR0_ADCPOLR_NORMAL     0x00
#define TSCS25XX_REG_CNVRTR0_ADCPOLR_INV        0x80
#define TSCS25XX_REG_CNVRTR0_ADCPOLL_NORMAL     0x00
#define TSCS25XX_REG_CNVRTR0_ADCPOLL_INV        0x40
#define TSCS25XX_REG_CNVRTR0_AMONOMIX_STEREO    0x00
#define TSCS25XX_REG_CNVRTR0_AMONOMIX_AMONO_L   0x10
#define TSCS25XX_REG_CNVRTR0_AMONOMIX_AMONO_R   0x20
#define TSCS25XX_REG_CNVRTR0_AMONOMIX_DMONO_LR  0x30
#define TSCS25XX_REG_CNVRTR0_ADCMU_UNMUTED      0x00
#define TSCS25XX_REG_CNVRTR0_ADCMU_MUTED        0x08
#define TSCS25XX_REG_CNVRTR0_HPOR_DISCARD       0x00
#define TSCS25XX_REG_CNVRTR0_HPOR_STORE         0x04
#define TSCS25XX_REG_CNVRTR0_HPDR_ENABLE        0x00
#define TSCS25XX_REG_CNVRTR0_HPDR_DISABLE       0x02
#define TSCS25XX_REG_CNVRTR0_HPDL_ENABLE        0x00
#define TSCS25XX_REG_CNVRTR0_HPDL_DISABLE       0x01

// TSCS25XX_REG_ADCSR
#define TSCS25XX_REG_ADCSR_ABCM_AUTO        0x00
#define TSCS25XX_REG_ADCSR_ABCM_32x         0x40
#define TSCS25XX_REG_ADCSR_ABCM_40x         0x80
#define TSCS25XX_REG_ADCSR_ABCM_64x         0xC0
#define TSCS25XX_REG_ADCSR_ABR_32_KHZ       0x00
#define TSCS25XX_REG_ADCSR_ABR_44p1_KHZ     0x08
#define TSCS25XX_REG_ADCSR_ABR_48_KHZ       0x10
#define TSCS25XX_REG_ADCSR_ABM_0p25x        0x00
#define TSCS25XX_REG_ADCSR_ABM_0p5x         0x01
#define TSCS25XX_REG_ADCSR_ABM_1x           0x02
#define TSCS25XX_REG_ADCSR_ABM_2x           0x03

// TSCS25XX_REG_CNVRTR1
#define TSCS25XX_REG_CNVRTR1_DACPOLR_NORMAL     0x00
#define TSCS25XX_REG_CNVRTR1_DACPOLR_INV        0x80
#define TSCS25XX_REG_CNVRTR1_DACPOLL_NORMAL     0x00
#define TSCS25XX_REG_CNVRTR1_DACPOLL_INV        0x40
#define TSCS25XX_REG_CNVRTR1_DMONOMIX_STEREO    0x00
#define TSCS25XX_REG_CNVRTR1_DMONOMIX_MONO_L    0x10
#define TSCS25XX_REG_CNVRTR1_DMONOMIX_MONO_R    0x20
#define TSCS25XX_REG_CNVRTR1_DMONOMIX_MONO_LR   0x30
#define TSCS25XX_REG_CNVRTR1_DACMU_UNMUTED      0x00
#define TSCS25XX_REG_CNVRTR1_DACMU_MUTED        0x08
#define TSCS25XX_REG_CNVRTR1_DEEMP_DISABLED     0x00
#define TSCS25XX_REG_CNVRTR1_DEEMP_ENABLED      0x04
#define TSCS25XX_REG_CNVRTR1_DACDITH_DYN_HALF   0x00
#define TSCS25XX_REG_CNVRTR1_DACDITH_DYN_FULL   0x01
#define TSCS25XX_REG_CNVRTR1_DACDITH_DISABLED   0x02
#define TSCS25XX_REG_CNVRTR1_DACDITH_STATIC     0x03

// TSCS25XX_REG_DACSR
#define TSCS25XX_REG_DACSR_DBCM_AUTO        0x00
#define TSCS25XX_REG_DACSR_DBCM_32x         0x40
#define TSCS25XX_REG_DACSR_DBCM_40x         0x80
#define TSCS25XX_REG_DACSR_DBCM_64x         0xC0
#define TSCS25XX_REG_DACSR_DBR_32_KHZ       0x00
#define TSCS25XX_REG_DACSR_DBR_44p1_KHZ     0x08
#define TSCS25XX_REG_DACSR_DBR_48_KHZ       0x10
#define TSCS25XX_REG_DACSR_DBM_0p25x        0x00
#define TSCS25XX_REG_DACSR_DBM_0p5x         0x01
#define TSCS25XX_REG_DACSR_DBM_1x           0x02
#define TSCS25XX_REG_DACSR_DBM_2x           0x03

// TSCS25XX_REG_PWRM1
#define TSCS25XX_REG_PWRM1_BSTL_POWER_DOWN      0x00
#define TSCS25XX_REG_PWRM1_BSTL_POWER_UP        0x80
#define TSCS25XX_REG_PWRM1_BSTR_POWER_DOWN      0x00
#define TSCS25XX_REG_PWRM1_BSTR_POWER_UP        0x40
#define TSCS25XX_REG_PWRM1_PGAL_POWER_DOWN      0x00
#define TSCS25XX_REG_PWRM1_PGAL_POWER_UP        0x20
#define TSCS25XX_REG_PWRM1_PGAR_POWER_DOWN      0x00
#define TSCS25XX_REG_PWRM1_PGAR_POWER_UP        0x10
#define TSCS25XX_REG_PWRM1_ADCL_POWER_DOWN      0x00
#define TSCS25XX_REG_PWRM1_ADCL_POWER_UP        0x08
#define TSCS25XX_REG_PWRM1_ADCR_POWER_DOWN      0x00
#define TSCS25XX_REG_PWRM1_ADCR_POWER_UP        0x04
#define TSCS25XX_REG_PWRM1_MICB_POWER_DOWN      0x00
#define TSCS25XX_REG_PWRM1_MICB_POWER_UP        0x02
#define TSCS25XX_REG_PWRM1_DIGENB_MCLK_ENABLED  0x00
#define TSCS25XX_REG_PWRM1_DIGENB_MCLK_DISABLED 0x01

// TSCS25XX_REG_PWRM2
#define TSCS25XX_REG_PWRM2_D2S_POWER_DOWN   0x00
#define TSCS25XX_REG_PWRM2_D2S_POWER_UP     0x80
#define TSCS25XX_REG_PWRM2_HPL_POWER_DOWN   0x00
#define TSCS25XX_REG_PWRM2_HPL_POWER_UP     0x40
#define TSCS25XX_REG_PWRM2_HPR_POWER_DOWN   0x00
#define TSCS25XX_REG_PWRM2_HPR_POWER_UP     0x20
#define TSCS25XX_REG_PWRM2_SPKL_POWER_DOWN  0x00
#define TSCS25XX_REG_PWRM2_SPKL_POWER_UP    0x10
#define TSCS25XX_REG_PWRM2_SPKR_POWER_DOWN  0x00
#define TSCS25XX_REG_PWRM2_SPKR_POWER_UP    0x08
#define TSCS25XX_REG_PWRM2_VREF_POWER_DOWN  0x00
#define TSCS25XX_REG_PWRM2_VREF_POWER_UP    0x01

// TSCS25XX_REG_CONFIG0
#define TSCS25XX_REG_CONFIG0_ASDM_HALF          0x40
#define TSCS25XX_REG_CONFIG0_ASDM_FULL          0x80
#define TSCS25XX_REG_CONFIG0_ASDM_AUTO          0xC0
#define TSCS25XX_REG_CONFIG0_DSDM_HALF          0x10
#define TSCS25XX_REG_CONFIG0_DSDM_FULL          0x20
#define TSCS25XX_REG_CONFIG0_DSDM_AUTO          0x30
#define TSCS25XX_REG_CONFIG0_DC_FILTER_ON       0x00
#define TSCS25XX_REG_CONFIG0_DC_FILTER_BYPASS   0x02
#define TSCS25XX_REG_CONFIG0_SD_NO_FORCE_ON     0x00
#define TSCS25XX_REG_CONFIG0_SD_FORCE_ON        0x01

// TSCS25XX_REG_CONFIG1
#define TSCS25XX_REG_CONFIG1_EQ2_BYPASS         0x00
#define TSCS25XX_REG_CONFIG1_EQ2_ENABLED        0x80
#define TSCS25XX_REG_CONFIG1_EQ2_PRESC_ONLY     0x00
#define TSCS25XX_REG_CONFIG1_EQ2_PRESC_BAND_0   0x10
#define TSCS25XX_REG_CONFIG1_EQ2_PRESC_BAND_0_1 0x20
#define TSCS25XX_REG_CONFIG1_EQ2_PRESC_BAND_0_2 0x30
#define TSCS25XX_REG_CONFIG1_EQ2_PRESC_BAND_0_3 0x40
#define TSCS25XX_REG_CONFIG1_EQ2_PRESC_BAND_0_4 0x50
#define TSCS25XX_REG_CONFIG1_EQ2_PRESC_BAND_0_5 0x60
#define TSCS25XX_REG_CONFIG1_EQ1_BYPASS         0x00
#define TSCS25XX_REG_CONFIG1_EQ1_ENABLED        0x08
#define TSCS25XX_REG_CONFIG1_EQ1_PRESC_ONLY     0x00
#define TSCS25XX_REG_CONFIG1_EQ1_PRESC_BAND_0   0x01
#define TSCS25XX_REG_CONFIG1_EQ1_PRESC_BAND_0_1 0x02
#define TSCS25XX_REG_CONFIG1_EQ1_PRESC_BAND_0_2 0x03
#define TSCS25XX_REG_CONFIG1_EQ1_PRESC_BAND_0_3 0x04
#define TSCS25XX_REG_CONFIG1_EQ1_PRESC_BAND_0_4 0x05
#define TSCS25XX_REG_CONFIG1_EQ1_PRESC_BAND_0_5 0x06

// TSCS25XX_REG_GAINCTL
#define TSCS25XX_REG_GAINCTL_ZERODET_DETECTED   0x80
#define TSCS25XX_REG_GAINCTL_ZERODET_LEN_512    0x00
#define TSCS25XX_REG_GAINCTL_ZERODET_LEN_1024   0x10
#define TSCS25XX_REG_GAINCTL_ZERODET_LEN_2048   0x20
#define TSCS25XX_REG_GAINCTL_ZERODET_LEN_4096   0x30
#define TSCS25XX_REG_GAINCTL_AUTO_PWR           0x80
#define TSCS25XX_REG_GAINCTL_AUTO_MUTE          0x40

// TSCS25XX_REG_CLECTL
#define TSCS25XX_REG_CLECTL_LVL_MODE_AVERAGE    0x00
#define TSCS25XX_REG_CLECTL_LVL_MODE_PEAK       0x10
#define TSCS25XX_REG_CLECTL_WINDOW_SEL_512      0x00
#define TSCS25XX_REG_CLECTL_WINDOW_SEL_64       0x08
#define TSCS25XX_REG_CLECTL_EXPANDER_DISABLED   0x00
#define TSCS25XX_REG_CLECTL_EXPANDER_ENABLED    0x04
#define TSCS25XX_REG_CLECTL_LIMITER_DISABLED    0x00
#define TSCS25XX_REG_CLECTL_LIMITER_ENABLED     0x02
#define TSCS25XX_REG_CLECTL_COMPRESSOR_DISABLED 0x00
#define TSCS25XX_REG_CLECTL_COMPRESSOR_ENABLED  0x01

// TSCS25XX_REG_FXCTL
#define TSCS25XX_REG_FXCTL_3DEN_DISABLED    0x00
#define TSCS25XX_REG_FXCTL_3DEN_ENABLED     0x10
#define TSCS25XX_REG_FXCTL_TEEN_DISABLED    0x00
#define TSCS25XX_REG_FXCTL_TEEN_ENABLED     0x08
#define TSCS25XX_REG_FXCTL_TNLFBYP_ENABLED  0x00
#define TSCS25XX_REG_FXCTL_TNLFBYP_BYPASS   0x04
#define TSCS25XX_REG_FXCTL_BEEN_DISABLED    0x00
#define TSCS25XX_REG_FXCTL_BEEN_ENABLED     0x02
#define TSCS25XX_REG_FXCTL_BNLFBYP_ENABLED  0x00
#define TSCS25XX_REG_FXCTL_BNLFBYP_BYPASS   0x01

// TSCS25XX_REG_PLLCTL1C
#define TSCS25XX_REG_PLLCTL1C_PLL2_POWER_DOWN   0x00
#define TSCS25XX_REG_PLLCTL1C_PLL2_POWER_UP     0x04
#define TSCS25XX_REG_PLLCTL1C_PLL1_POWER_DOWN   0x00
#define TSCS25XX_REG_PLLCTL1C_PLL1_POWER_UP     0x02

// TSCS25XX_REG_DACCRSTAT
#define TSCS25XX_REG_DACCRSTAT_DACCR_NOT_BUSY   0x80
#define TSCS25XX_REG_DACCRSTAT_DACCR_BUSY       0x80

// TSCS25XX_REG_PLLCTL0
#define TSCS25XX_REG_PLLCTL0_PLL2_NOT_LOCKED    0x00
#define TSCS25XX_REG_PLLCTL0_PLL2_LOCKED        0x02
#define TSCS25XX_REG_PLLCTL0_PLL1_NOT_LOCKED    0x00
#define TSCS25XX_REG_PLLCTL0_PLL1_LOCKED        0x01

// TSCS25XX_REG_PLLREFSEL
#define TSCS25XX_REG_PLLREFSEL_PLL2_REF_MCLK2       0x10
#define TSCS25XX_REG_PLLREFSEL_PLL2_REF_DAC_BCLK    0x20
#define TSCS25XX_REG_PLLREFSEL_PLL2_REF_ADC_BCLK    0x30
#define TSCS25XX_REG_PLLREFSEL_PLL2_REF_PLL1        0x40
#define TSCS25XX_REG_PLLREFSEL_PLL1_REF_MCLK2       0x01
#define TSCS25XX_REG_PLLREFSEL_PLL1_REF_DAC_BCLK    0x02
#define TSCS25XX_REG_PLLREFSEL_PLL1_REF_ADC_BCLK    0x03
#define TSCS25XX_REG_PLLREFSEL_PLL1_REF_PLL2        0x04

// TSCS25XX_REG_GPIOCTL1
#define TSCS25XX_REG_GPIOCTL1_GPIO1_IO          0x00
#define TSCS25XX_REG_GPIOCTL1_GPIO1_INTERRUPT   0x20
#define TSCS25XX_REG_GPIOCTL1_GPIO0_IO          0x00
#define TSCS25XX_REG_GPIOCTL1_GPIO0_INTERRUPT   0x10
#define TSCS25XX_REG_GPIOCTL1_GPIO1_INPUT       0x00
#define TSCS25XX_REG_GPIOCTL1_GPIO1_OUTPUT      0x02
#define TSCS25XX_REG_GPIOCTL1_GPIO0_INPUT       0x00
#define TSCS25XX_REG_GPIOCTL1_GPIO0_OUTPUT      0x01

// TSCS25XX_REG_GPIODATA
#define TSCS25XX_REG_GPIODATA_GPIO1_OUT_LOW     0x00
#define TSCS25XX_REG_GPIODATA_GPIO1_OUT_HIGH    0x20
#define TSCS25XX_REG_GPIODATA_GPIO0_OUT_LOW     0x00
#define TSCS25XX_REG_GPIODATA_GPIO0_OUT_HIGH    0x10
#define TSCS25XX_REG_GPIODATA_GPIO1_IN_LOW      0x00
#define TSCS25XX_REG_GPIODATA_GPIO1_IN_HIGH     0x02
#define TSCS25XX_REG_GPIODATA_GPIO0_IN_LOW      0x00
#define TSCS25XX_REG_GPIODATA_GPIO0_IN_HIGH     0x01

// TSCS25XX_REG_GPIOCTL2
#define TSCS25XX_REG_GPIOCTL2_GPIO1_PULLUP      0x00
#define TSCS25XX_REG_GPIOCTL2_GPIO1_FLOAT       0x02
#define TSCS25XX_REG_GPIOCTL2_GPIO0_PULLUP      0x00
#define TSCS25XX_REG_GPIOCTL2_GPIO0_FLOAT       0x01

// TSCS25XX_REG_ADCPCMCTL1
#define TSCS25XX_REG_ADCPCMCTL1_GAIN_NORMAL         0x00
#define TSCS25XX_REG_ADCPCMCTL1_GAIN_REPLACE        0x10
#define TSCS25XX_REG_ADCPCMCTL1_BDELAYO_NO_DELAY    0x00
#define TSCS25XX_REG_ADCPCMCTL1_BDELAYO_DELAY       0x08
#define TSCS25XX_REG_ADCPCMCTL1_PCMFL_128BIT        0x00
#define TSCS25XX_REG_ADCPCMCTL1_PCMFL_256BIT        0x04
#define TSCS25XX_REG_ADCPCMCTL1_SLSYNC_ONE_CLOCK    0x00
#define TSCS25XX_REG_ADCPCMCTL1_SLSYNC_ONE_SLOT     0x02

// TSCS25XX_REG_ADCPCMCTL2
#define TSCS25XX_REG_ADCPCMCTL2_PCMMOMP_SLOT0_LEFT  0x00
#define TSCS25XX_REG_ADCPCMCTL2_PCMMOMP_SLOT0_RIGHT 0x40
#define TSCS25XX_REG_ADCPCMCTL2_PCMSOP_ONE          0x00
#define TSCS25XX_REG_ADCPCMCTL2_PCMSOP_TWO          0x20
#define TSCS25XX_REG_ADCPCMCTL2_PCMDSSP_16BIT       0x00
#define TSCS25XX_REG_ADCPCMCTL2_PCMDSSP_24BIT       0x08
#define TSCS25XX_REG_ADCPCMCTL2_PCMDSSP_32BIT       0x10

// TSCS25XX_REG_DACPCMCTL1
#define TSCS25XX_REG_DACPCMCTL1_BDELAYI_NO_DELAY    0x00
#define TSCS25XX_REG_DACPCMCTL1_BDELAYI_DELAY       0x08
#define TSCS25XX_REG_DACPCMCTL1_PCMFL_128BIT        0x00
#define TSCS25XX_REG_DACPCMCTL1_PCMFL_256BIT        0x04
#define TSCS25XX_REG_DACPCMCTL1_SLSYNC_ONE_CLOCK    0x00
#define TSCS25XX_REG_DACPCMCTL1_SLSYNC_ONE_SLOT     0x02

// TSCS25XX_REG_DACPCMCTL2
#define TSCS25XX_REG_DACPCMCTL2_PCMFORMAT_DISABLED  0x00
#define TSCS25XX_REG_DACPCMCTL2_PCMFORMAT_ENABLED   0x80
#define TSCS25XX_REG_DACPCMCTL2_PCMMIM_SLOT0_LEFT   0x00
#define TSCS25XX_REG_DACPCMCTL2_PCMMIM_SLOT0_RIGHT  0x40
#define TSCS25XX_REG_DACPCMCTL2_PCMSI_ONE           0x00
#define TSCS25XX_REG_DACPCMCTL2_PCMSI_TWO           0x20
#define TSCS25XX_REG_DACPCMCTL2_PCMDSS_16BIT        0x00
#define TSCS25XX_REG_DACPCMCTL2_PCMDSS_24BIT        0x08
#define TSCS25XX_REG_DACPCMCTL2_PCMDSS_32BIT        0x10
#define TSCS25XX_REG_DACPCMCTL2_PCMSIGNEXT_NORMAL   0x00
#define TSCS25XX_REG_DACPCMCTL2_PCMSIGNEXT_EXTENDED 0x04
#define TSCS25XX_REG_DACPCMCTL2_PCM13MODE_DISABLED  0x00
#define TSCS25XX_REG_DACPCMCTL2_PCMSIGNEXT_ENABLED  0x02

// TSCS25XX_REG_DACMBCEN
#define TSCS25XX_REG_DACMBCEN_CBAND3_DISABLED       0x00
#define TSCS25XX_REG_DACMBCEN_CBAND3_ENABLED        0x04
#define TSCS25XX_REG_DACMBCEN_CBAND2_DISABLED       0x00
#define TSCS25XX_REG_DACMBCEN_CBAND2_ENABLED        0x02
#define TSCS25XX_REG_DACMBCEN_CBAND1_DISABLED       0x00
#define TSCS25XX_REG_DACMBCEN_CBAND1_ENABLED        0x01

// TSCS25XX_REG_DACMBCCTL
#define TSCS25XX_REG_DACMBCCTL_LVL_MODE3_AVERAGE    0x00
#define TSCS25XX_REG_DACMBCCTL_LVL_MODE3_PEAK       0x20
#define TSCS25XX_REG_DACMBCCTL_WINDOW_SEL3_512      0x00
#define TSCS25XX_REG_DACMBCCTL_WINDOW_SEL3_64       0x10
#define TSCS25XX_REG_DACMBCCTL_LVL_MODE2_AVERAGE    0x00
#define TSCS25XX_REG_DACMBCCTL_LVL_MODE2_PEAK       0x08
#define TSCS25XX_REG_DACMBCCTL_WINDOW_SEL2_512      0x00
#define TSCS25XX_REG_DACMBCCTL_WINDOW_SEL2_64       0x04
#define TSCS25XX_REG_DACMBCCTL_LVL_MODE1_AVERAGE    0x00
#define TSCS25XX_REG_DACMBCCTL_LVL_MODE1_PEAK       0x02
#define TSCS25XX_REG_DACMBCCTL_WINDOW_SEL1_512      0x00
#define TSCS25XX_REG_DACMBCCTL_WINDOW_SEL1_64       0x01

// DAC RAM
#define TSCS25XX_DAC_RAM_SIZE   206

#define TSCS25XX_DAC_RAM_EQx_COEF_nFi_B0(x, n, i)   (0x40 * (x) + 0x20 * (n) + 0x05 * (i) + 0x00)
#define TSCS25XX_DAC_RAM_EQx_COEF_nFi_B1(x, n, i)   (0x40 * (x) + 0x20 * (n) + 0x05 * (i) + 0x01)
#define TSCS25XX_DAC_RAM_EQx_COEF_nFi_B2(x, n, i)   (0x40 * (x) + 0x20 * (n) + 0x05 * (i) + 0x02)
#define TSCS25XX_DAC_RAM_EQx_COEF_nFi_A1(x, n, i)   (0x40 * (x) + 0x20 * (n) + 0x05 * (i) + 0x03)
#define TSCS25XX_DAC_RAM_EQx_COEF_nFi_A2(x, n, i)   (0x40 * (x) + 0x20 * (n) + 0x05 * (i) + 0x04)
#define TSCS25XX_DAC_RAM_EQx_PRESCALEn(x, n)        (0x40 * (x) + 0x20 * (n) + 0x1F)
#define TSCS25XX_DAC_RAM_BASS_COEF_EXTn_B0(n)       (0x05 * (n) + 0x80)
#define TSCS25XX_DAC_RAM_BASS_COEF_EXTn_B1(n)       (0x05 * (n) + 0x81)
#define TSCS25XX_DAC_RAM_BASS_COEF_EXTn_B2(n)       (0x05 * (n) + 0x82)
#define TSCS25XX_DAC_RAM_BASS_COEF_EXTn_A1(n)       (0x05 * (n) + 0x83)
#define TSCS25XX_DAC_RAM_BASS_COEF_EXTn_A2(n)       (0x05 * (n) + 0x84)
#define TSCS25XX_DAC_RAM_TREB_COEF_EXTn_B0(n)       (0x05 * (n) + 0x97)
#define TSCS25XX_DAC_RAM_TREB_COEF_EXTn_B1(n)       (0x05 * (n) + 0x98)
#define TSCS25XX_DAC_RAM_TREB_COEF_EXTn_B2(n)       (0x05 * (n) + 0x99)
#define TSCS25XX_DAC_RAM_TREB_COEF_EXTn_A1(n)       (0x05 * (n) + 0x9A)
#define TSCS25XX_DAC_RAM_TREB_COEF_EXTn_A2(n)       (0x05 * (n) + 0x9B)

// Constants
#define TSCS25XX_PLL1_OUTPUT_FREQ       (48000UL * 2560) // 48.0 kHz sample rate and multiples
#define TSCS25XX_PLL2_OUTPUT_FREQ       (44100UL * 2560) // 44.1 kHz sample rate and multiples

#define TSCS25XX_ADC_INPUT_1     0
#define TSCS25XX_ADC_INPUT_2     1
#define TSCS25XX_ADC_INPUT_D2S   2

#define TSCS25XX_D2S_INPUT_1     0
#define TSCS25XX_D2S_INPUT_2     1

#define TSCS25XX_DAC_DITHER_DISABLED    0
#define TSCS25XX_DAC_DITHER_STATIC      1
#define TSCS25XX_DAC_DITHER_DYN_HALF    2
#define TSCS25XX_DAC_DITHER_DYN_FULL    3

#define TSCS25XX_MONO_MIX_STEREO    0
#define TSCS25XX_MONO_MIX_LEFT      1
#define TSCS25XX_MONO_MIX_RIGHT     2
#define TSCS25XX_MONO_MIX_BOTH      3

#define TSCS25XX_EQ1    0
#define TSCS25XX_EQ2    1

#define TSCS25XX_EQ_CHANNEL_LEFT    0
#define TSCS25XX_EQ_CHANNEL_RIGHT   1

#define TSCS25XX_EQ_BAND_PRESC      0
#define TSCS25XX_EQ_BAND_PRESC_B0   1
#define TSCS25XX_EQ_BAND_PRESC_B0_1 2
#define TSCS25XX_EQ_BAND_PRESC_B0_2 3
#define TSCS25XX_EQ_BAND_PRESC_B0_3 4
#define TSCS25XX_EQ_BAND_PRESC_B0_4 5
#define TSCS25XX_EQ_BAND_PRESC_B0_5 6

// Utility
#define TSCS25XX_REV_MAJOR(x)      (((x) & 0xF0) >> 4)
#define TSCS25XX_REV_MINOR(x)      (((x) & 0x0F) >> 0)


uint8_t tscs25xx_init();

void tscs25xx_soft_reset();

uint16_t tscs25xx_read_device_id();
uint8_t tscs25xx_read_revision_id();
uint8_t tscs25xx_read_mf_otp_version();
uint8_t tscs25xx_read_mf_wafer_id();
uint8_t tscs25xx_read_mf_wafer_x();
uint8_t tscs25xx_read_mf_wafer_y();

uint8_t tscs25xx_pll_config(uint8_t ubPLLID, uint32_t ulRefFrequency);
uint8_t tscs25xx_timebase_config(uint32_t ulRefFrequency);
uint8_t tscs25xx_sample_rate_config(uint32_t ulSampleRate);

void tscs25xx_adc_config_left_input(uint8_t ubSource, uint8_t ubGain, uint8_t ubInvert, uint8_t ubHighPassEnable);
void tscs25xx_adc_config_right_input(uint8_t ubSource, uint8_t ubGain, uint8_t ubInvert, uint8_t ubHighPassEnable);
void tscs25xx_adc_config_mono_mixer(uint8_t ubMode);
void tscs25xx_adc_config_d2s_amp(uint8_t ubInputChannel);

void tscs25xx_dac_config_left_output(uint8_t ubInvert);
void tscs25xx_dac_config_right_output(uint8_t ubInvert);
void tscs25xx_dac_config_mono_mixer(uint8_t ubMode);
void tscs25xx_dac_config(uint8_t ubDitherMode, uint8_t ubDeempEnable);

uint8_t tscs25xx_dac_ram_write(uint8_t ubAddress, uint32_t *pulData, uint8_t ubDataSize);
uint8_t tscs25xx_dac_ram_read(uint8_t ubAddress, uint32_t *pulData, uint8_t ubDataSize);

void tscs25xx_effects_config(uint8_t ub3DEnable, uint8_t ubTrebleEnable, uint8_t ubTrebleNLEnable, uint8_t ubBassEnable, uint8_t ubBassNLEnable);

void tscs25xx_eq_config(uint8_t ubID, uint8_t ubEnable, uint8_t ubBands);
void tscs25xx_eq_config_prescaler(uint8_t ubID, uint8_t ubChannelID, float fPrescale);
void tscs25xx_eq_config_band(uint8_t ubID, uint8_t ubChannelID, uint8_t ubBandID, biquad_t *pFilter);

void tscs25xx_volume_config(uint8_t ubFade, uint8_t ubIndividualUpdate, uint8_t ubZeroUpdate);

void tscs25xx_hp_set_left_volume(float fVolume);
float tscs25xx_hp_get_left_volume();
void tscs25xx_hp_set_right_volume(float fVolume);
float tscs25xx_hp_get_right_volume();
void tscs25xx_spkr_set_left_volume(float fVolume);
float tscs25xx_spkr_get_left_volume();
void tscs25xx_spkr_set_right_volume(float fVolume);
float tscs25xx_spkr_get_right_volume();
void tscs25xx_dac_set_left_volume(float fVolume);
float tscs25xx_dac_get_left_volume();
void tscs25xx_dac_set_right_volume(float fVolume);
float tscs25xx_dac_get_right_volume();
void tscs25xx_input_set_left_volume(float fVolume);
float tscs25xx_input_get_left_volume();
void tscs25xx_input_set_right_volume(float fVolume);
float tscs25xx_input_get_right_volume();
void tscs25xx_adc_set_left_volume(float fVolume);
float tscs25xx_adc_get_left_volume();
void tscs25xx_adc_set_right_volume(float fVolume);
float tscs25xx_adc_get_right_volume();

void tscs25xx_dac_set_mute(uint8_t ubMute);
uint8_t tscs25xx_dac_get_mute();
void tscs25xx_adc_set_mute(uint8_t ubMute);
uint8_t tscs25xx_adc_get_mute();

void tscs25xx_zero_det_config(uint8_t ubMute, uint16_t usLength);
uint8_t tscs25xx_zero_det_get_flag();

void tscs25xx_noise_gate_config(uint8_t ubEnable, float fThreshold);

#endif // __TSCS25XX_H__