#ifndef __GPIO_H__
#define __GPIO_H__

#include <em_device.h>
#include "cmu.h"
#include "systick.h"
#include "utils.h"
#include "nvic.h"

// LED MACROS
#define LED1_HIGH()         PERI_REG_BIT_SET(&(GPIO->P[0].DOUT)) = BIT(12)
#define LED1_LOW()          PERI_REG_BIT_CLEAR(&(GPIO->P[0].DOUT)) = BIT(12)
#define LED1_TOGGLE()       GPIO->P[0].DOUTTGL = BIT(12);
#define LED1_STATUS()       !!(GPIO->P[0].DOUT & BIT(12))
#define LED2_HIGH()         PERI_REG_BIT_SET(&(GPIO->P[0].DOUT)) = BIT(13)
#define LED2_LOW()          PERI_REG_BIT_CLEAR(&(GPIO->P[0].DOUT)) = BIT(13)
#define LED2_TOGGLE()       GPIO->P[0].DOUTTGL = BIT(13);
#define LED2_STATUS()       !!(GPIO->P[0].DOUT & BIT(13))

// TX MACROS
#define TX_SELECT()           PERI_REG_BIT_CLEAR(&(GPIO->P[5].DOUT)) = BIT(12)
#define TX_UNSELECT()         PERI_REG_BIT_SET(&(GPIO->P[5].DOUT)) = BIT(12)
#define TX_RESET()            PERI_REG_BIT_SET(&(GPIO->P[1].DOUT)) = BIT(6)
#define TX_UNRESET()          PERI_REG_BIT_CLEAR(&(GPIO->P[1].DOUT)) = BIT(6)

// RX MACROS
#define RX_SELECT()           PERI_REG_BIT_CLEAR(&(GPIO->P[5].DOUT)) = BIT(11)
#define RX_UNSELECT()         PERI_REG_BIT_SET(&(GPIO->P[5].DOUT)) = BIT(11)
#define RX_RESET()            PERI_REG_BIT_SET(&(GPIO->P[5].DOUT)) = BIT(10)
#define RX_UNRESET()          PERI_REG_BIT_CLEAR(&(GPIO->P[5].DOUT)) = BIT(10)

// SYNTH MACROS
#define SYNTH_SELECT()        PERI_REG_BIT_CLEAR(&(GPIO->P[0].DOUT)) = BIT(3)
#define SYNTH_UNSELECT()      PERI_REG_BIT_SET(&(GPIO->P[0].DOUT)) = BIT(3)
#define SYNTH_RESET()         PERI_REG_BIT_CLEAR(&(GPIO->P[4].DOUT)) = BIT(13)
#define SYNTH_UNRESET()       PERI_REG_BIT_SET(&(GPIO->P[4].DOUT)) = BIT(13)
#define SYNTH_MUTE()          PERI_REG_BIT_SET(&(GPIO->P[0].DOUT)) = BIT(4)
#define SYNTH_UNMUTE()        PERI_REG_BIT_CLEAR(&(GPIO->P[0].DOUT)) = BIT(4)
#define SYNTH_MUTED()         !!(GPIO->P[0].DOUT & BIT(4))
#define SYNTH_LOCKED()        !!(GPIO->P[0].DIN & BIT(5))
#define SYNTH_ENABLE()        PERI_REG_BIT_SET(&(GPIO->P[4].DOUT)) = BIT(15)
#define SYNTH_DISABLE()       PERI_REG_BIT_CLEAR(&(GPIO->P[4].DOUT)) = BIT(15)

// EXT LO ATT MACROS
#define EXT_LO_ATT_LEN()      PERI_REG_BIT_SET(&(GPIO->P[1].DOUT)) = BIT(4)
#define EXT_LO_ATT_LDIS()     PERI_REG_BIT_CLEAR(&(GPIO->P[1].DOUT)) = BIT(4)

// LO SEL MACROS
#define TX_LO_SRC_SEL0()      PERI_REG_BIT_CLEAR(&(GPIO->P[4].DOUT)) = BIT(7)
#define TX_LO_SRC_SEL1()      PERI_REG_BIT_SET(&(GPIO->P[4].DOUT)) = BIT(7)
#define TX_LO_DST_SEL0()      PERI_REG_BIT_CLEAR(&(GPIO->P[4].DOUT)) = BIT(6)
#define TX_LO_DST_SEL1()      PERI_REG_BIT_SET(&(GPIO->P[4].DOUT)) = BIT(6)
#define RX_LO_SRC_SEL0()      PERI_REG_BIT_CLEAR(&(GPIO->P[4].DOUT)) = BIT(5)
#define RX_LO_SRC_SEL1()      PERI_REG_BIT_SET(&(GPIO->P[4].DOUT)) = BIT(5)
#define RX_LO_DST_SEL0()      PERI_REG_BIT_CLEAR(&(GPIO->P[4].DOUT)) = BIT(4)
#define RX_LO_DST_SEL1()      PERI_REG_BIT_SET(&(GPIO->P[4].DOUT)) = BIT(4)

// TSENS MACROS
#define TSENS_EN()            PERI_REG_BIT_SET(&(GPIO->P[4].DOUT)) = BIT(9)
#define TSENS_DIS()           PERI_REG_BIT_CLEAR(&(GPIO->P[4].DOUT)) = BIT(9)

// PWR MACROS
#define TX_DIG_PWR_EN()       PERI_REG_BIT_SET(&(GPIO->P[0].DOUT)) = BIT(14)
#define TX_DIG_PWR_DIS()      PERI_REG_BIT_CLEAR(&(GPIO->P[0].DOUT)) = BIT(14)
#define TX_DIG_PWR_STATUS()   !!(GPIO->P[0].DOUT & BIT(14))
#define RX_DIG_PWR_EN()       PERI_REG_BIT_SET(&(GPIO->P[1].DOUT)) = BIT(11)
#define RX_DIG_PWR_DIS()      PERI_REG_BIT_CLEAR(&(GPIO->P[1].DOUT)) = BIT(11)
#define RX_DIG_PWR_STATUS()   !!(GPIO->P[1].DOUT & BIT(11))
#define TX_ANA_PWR_EN()       PERI_REG_BIT_SET(&(GPIO->P[2].DOUT)) = BIT(5)
#define TX_ANA_PWR_DIS()      PERI_REG_BIT_CLEAR(&(GPIO->P[2].DOUT)) = BIT(5)
#define TX_ANA_PWR_STATUS()   !!(GPIO->P[2].DOUT & BIT(5))
#define RX_ANA_PWR_EN()       PERI_REG_BIT_SET(&(GPIO->P[1].DOUT)) = BIT(13)
#define RX_ANA_PWR_DIS()      PERI_REG_BIT_CLEAR(&(GPIO->P[1].DOUT)) = BIT(13)
#define RX_ANA_PWR_STATUS()   !!(GPIO->P[1].DOUT & BIT(13))
#define LO_ANA_PWR_EN()       PERI_REG_BIT_SET(&(GPIO->P[2].DOUT)) = BIT(4)
#define LO_ANA_PWR_DIS()      PERI_REG_BIT_CLEAR(&(GPIO->P[2].DOUT)) = BIT(4)
#define LO_ANA_PWR_STATUS()   !!(GPIO->P[2].DOUT & BIT(4))
#define RF_GP_PWR_EN()        PERI_REG_BIT_SET(&(GPIO->P[1].DOUT)) = BIT(7)
#define RF_GP_PWR_DIS()       PERI_REG_BIT_CLEAR(&(GPIO->P[1].DOUT)) = BIT(7)
#define RF_GP_PWR_STATUS()    !!(GPIO->P[1].DOUT & BIT(7))
#define SYNTH_PWR_EN()        PERI_REG_BIT_SET(&(GPIO->P[1].DOUT)) = BIT(14)
#define SYNTH_PWR_DIS()       PERI_REG_BIT_CLEAR(&(GPIO->P[1].DOUT)) = BIT(14)
#define SYNTH_PWR_STATUS()    !!(GPIO->P[1].DOUT & BIT(14))

void gpio_init();

#endif  // __GPIO_H__
