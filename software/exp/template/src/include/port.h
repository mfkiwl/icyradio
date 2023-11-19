#ifndef __PORT_H__
#define __PORT_H__

#include <sam.h>
#include "utils.h"
#include "nvic.h"

// PORT Helper macros
#define PORT_PERI_MUX_SELECT(p, i, n) PORT_REGS->GROUP[(p)].PORT_PMUX[(i) >> 1] = (PORT_REGS->GROUP[(p)].PORT_PMUX[(i) >> 1] & ((i) & 1 ? ~PORT_PMUX_PMUXO_Msk : ~PORT_PMUX_PMUXE_Msk)) | ((n) << ((i) & 1 ? PORT_PMUX_PMUXO_Pos : PORT_PMUX_PMUXE_Pos));

// LED MACROS
#define LED_HIGH()            PORT_REGS->GROUP[0].PORT_OUTSET = BIT(19)
#define LED_LOW()             PORT_REGS->GROUP[0].PORT_OUTCLR = BIT(19)
#define LED_TOGGLE()          PORT_REGS->GROUP[0].PORT_OUTTGL = BIT(19)
#define LED_STATUS()          !!(PORT_REGS->GROUP[0].PORT_OUT & BIT(19))

void port_init();

#endif  // __PORT_H__