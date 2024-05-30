#ifndef __PORT_H__
#define __PORT_H__

#include <sam.h>
#include "utils.h"
#include "nvic.h"

// PORT Helper macros
#define PORT_PERI_MUX_SELECT(p, i, n) PORT_REGS->GROUP[(p)].PORT_PMUX[(i) >> 1] = (PORT_REGS->GROUP[(p)].PORT_PMUX[(i) >> 1] & ((i) & 1 ? ~PORT_PMUX_PMUXO_Msk : ~PORT_PMUX_PMUXE_Msk)) | ((n) << ((i) & 1 ? PORT_PMUX_PMUXO_Pos : PORT_PMUX_PMUXE_Pos));

// LED MACROS
#define LED_HIGH()            PORT_REGS->GROUP[0].PORT_OUTSET = BIT(11)
#define LED_LOW()             PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(11)
#define LED_TOGGLE()          PORT_REGS->GROUP[0].PORT_OUTTGL = BIT(11)
#define LED_STATUS()          !!(PORT_REGS->GROUP[0].PORT_OUT & BIT(11))

// TX MACROS
#define TX_SELECT()           PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(10)
#define TX_UNSELECT()         PORT_REGS->GROUP[0].PORT_OUTSET = BIT(10)
#define TX_RESET()            PORT_REGS->GROUP[0].PORT_OUTSET = BIT(4)
#define TX_UNRESET()          PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(4)

// RX MACROS
#define RX_SELECT()           PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(28)
#define RX_UNSELECT()         PORT_REGS->GROUP[0].PORT_OUTSET = BIT(28)
#define RX_RESET()            PORT_REGS->GROUP[0].PORT_OUTSET = BIT(27)
#define RX_UNRESET()          PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(27)

// PWR MACROS
#define TX_DIG_PWR_EN()       PORT_REGS->GROUP[0].PORT_OUTSET = BIT(23)
#define TX_DIG_PWR_DIS()      PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(23)
#define TX_DIG_PWR_STATUS()   !!(PORT_REGS->GROUP[0].PORT_OUT & BIT(23))
#define RX_DIG_PWR_EN()       PORT_REGS->GROUP[0].PORT_OUTSET = BIT(22)
#define RX_DIG_PWR_DIS()      PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(22)
#define RX_DIG_PWR_STATUS()   !!(PORT_REGS->GROUP[0].PORT_OUT & BIT(22))
#define TX_ANA_PWR_EN()       PORT_REGS->GROUP[0].PORT_OUTSET = BIT(1)
#define TX_ANA_PWR_DIS()      PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(1)
#define TX_ANA_PWR_STATUS()   !!(PORT_REGS->GROUP[0].PORT_OUT & BIT(1))
#define RX_ANA_PWR_EN()       PORT_REGS->GROUP[0].PORT_OUTSET = BIT(19)
#define RX_ANA_PWR_DIS()      PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(19)
#define RX_ANA_PWR_STATUS()   !!(PORT_REGS->GROUP[0].PORT_OUT & BIT(19))
#define LO_ANA_PWR_EN()       PORT_REGS->GROUP[0].PORT_OUTSET = BIT(0)
#define LO_ANA_PWR_DIS()      PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(0)
#define LO_ANA_PWR_STATUS()   !!(PORT_REGS->GROUP[0].PORT_OUT & BIT(0))

// DBG MACROS
#define DBG_PRESENT()         !(PORT_REGS->GROUP[0].PORT_IN & BIT(30)) // SWCLK is high when no debugger is connected


void port_init();

#endif  // __PORT_H__
