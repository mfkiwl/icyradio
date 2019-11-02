#ifndef __PMC_H__
#define __PMC_H__

#include <sam.h>
#include "utils.h"

extern uint32_t MAINRCO_VALUE;
extern uint32_t SLOWRCO_VALUE;

extern uint32_t MAINCK_CLOCK_FREQ;
extern uint32_t SLCK_CLOCK_FREQ;
extern uint32_t PLLACK_CLOCK_FREQ;
extern uint32_t UPLLCK_CLOCK_FREQ;
extern uint32_t MCK_CLOCK_FREQ;
extern uint32_t HCLK_CLOCK_FREQ;

void pmc_init();
void pmc_update_clocks();

uint32_t pmc_get_pck_clock_freq(uint8_t ubID);
uint32_t pmc_get_generic_clock_freq(uint8_t ubPeripheralID);

#endif  // __PMC_H__
