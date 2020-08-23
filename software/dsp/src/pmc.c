#include "pmc.h"


uint32_t MAINXO_OSC_FREQ = 12000000UL;
uint32_t MAINRCO_OSC_FREQ = 12000000UL;
uint32_t SLOWXO_OSC_FREQ;
uint32_t SLOWRCO_OSC_FREQ = 32000UL;

uint32_t MAINCK_CLOCK_FREQ;
uint32_t SLCK_CLOCK_FREQ;
uint32_t PLLACK_CLOCK_FREQ;
uint32_t UPLLCK_CLOCK_FREQ;
uint32_t MCK_CLOCK_FREQ;
uint32_t FCLK_CLOCK_FREQ;

void pmc_init()
{
    // Bypass main xtal oscillator
    PMC->CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCXTBY;

    // Switch MAINCK to main xtal oscillator
    PMC->CKGR_MOR |= CKGR_MOR_KEY_PASSWD | CKGR_MOR_MOSCSEL;

    // Wait for it to be selected
    while(!(PMC->PMC_SR & PMC_SR_MOSCSELS));

    // Wait until clock is valid
    do
    {
        PMC->CKGR_MCFR = CKGR_MCFR_CCSS | CKGR_MCFR_RCMEAS;

        while(!(PMC->CKGR_MCFR & CKGR_MCFR_MAINFRDY));
    } while(!(PMC->CKGR_MCFR & CKGR_MCFR_MAINF_Msk));

    // Setup and enable PLLA to output 348 MHz, 63 SCLK cycles to lock
    PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | (((348000000UL / MAINXO_OSC_FREQ) - 1) << CKGR_PLLAR_MULA_Pos) | (63 << CKGR_PLLAR_PLLACOUNT_Pos) | CKGR_PLLAR_DIVA_BYPASS;

    // Wait for it to be locked
    while(!(PMC->PMC_SR & PMC_SR_LOCKA));

    // Configure flash waitstates
    eefc_config_waitstates(174000000);

    // Setup FCLK and MCK dividers
    PMC->PMC_MCKR = (PMC->PMC_MCKR & PMC_MCKR_CSS_Msk) | PMC_MCKR_UPLLDIV2 | PMC_MCKR_MDIV_PCK_DIV2 | PMC_MCKR_PRES_CLK_1;

    // Wait for them to be ready
    while(!(PMC->PMC_SR & PMC_SR_MCKRDY));

    // Switch HCLK and MCK to PLLA clock
    PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_PLLA_CLK;

    // Wait for it to be selected
    while(!(PMC->PMC_SR & PMC_SR_MCKRDY));
}
void pmc_update_clocks()
{
    switch(SUPC->SUPC_SR & SUPC_SR_OSCSEL_Msk)
    {
        case SUPC_SR_OSCSEL_RC:
            SLCK_CLOCK_FREQ = SLOWRCO_OSC_FREQ;
        break;
        case SUPC_SR_OSCSEL_CRYST:
            SLCK_CLOCK_FREQ = SLOWXO_OSC_FREQ;
        break;
    }

    switch(PMC->CKGR_MOR & CKGR_MOR_MOSCSEL_Msk)
    {
        case 0:
            MAINCK_CLOCK_FREQ = MAINRCO_OSC_FREQ;
        break;
        case CKGR_MOR_MOSCSEL:
            MAINCK_CLOCK_FREQ = MAINXO_OSC_FREQ;
        break;
    }

    float fPLLADiv = (PMC->CKGR_PLLAR & CKGR_PLLAR_DIVA_Msk) >> CKGR_PLLAR_DIVA_Pos;
    float fPLLAMul = (PMC->CKGR_PLLAR & CKGR_PLLAR_MULA_Msk) >> CKGR_PLLAR_MULA_Pos;

    if(fPLLADiv == 0 || fPLLAMul == 0)
        PLLACK_CLOCK_FREQ = 0;
    else
        PLLACK_CLOCK_FREQ = ((float)MAINCK_CLOCK_FREQ / fPLLADiv) * (fPLLAMul + 1.f);

    if(!(PMC->CKGR_UCKR & CKGR_UCKR_UPLLEN))
        UPLLCK_CLOCK_FREQ = 0;
    else
        UPLLCK_CLOCK_FREQ = 480000000UL;

    switch(PMC->PMC_MCKR & PMC_MCKR_CSS_Msk)
    {
        case PMC_MCKR_CSS_SLOW_CLK:
            FCLK_CLOCK_FREQ = SLCK_CLOCK_FREQ;
        break;
        case PMC_MCKR_CSS_MAIN_CLK:
            FCLK_CLOCK_FREQ = MAINCK_CLOCK_FREQ;
        break;
        case PMC_MCKR_CSS_UPLL_CLK:
            FCLK_CLOCK_FREQ = UPLLCK_CLOCK_FREQ;
        break;
        case PMC_MCKR_CSS_PLLA_CLK:
            FCLK_CLOCK_FREQ = PLLACK_CLOCK_FREQ;
        break;
    }

    switch(PMC->PMC_MCKR & PMC_MCKR_PRES_Msk)
    {
        case PMC_MCKR_PRES_CLK_1:
            FCLK_CLOCK_FREQ >>= 0;
        break;
        case PMC_MCKR_PRES_CLK_2:
            FCLK_CLOCK_FREQ >>= 1;
        break;
        case PMC_MCKR_PRES_CLK_4:
            FCLK_CLOCK_FREQ >>= 2;
        break;
        case PMC_MCKR_PRES_CLK_8:
            FCLK_CLOCK_FREQ >>= 3;
        break;
        case PMC_MCKR_PRES_CLK_16:
            FCLK_CLOCK_FREQ >>= 4;
        break;
        case PMC_MCKR_PRES_CLK_32:
            FCLK_CLOCK_FREQ >>= 5;
        break;
        case PMC_MCKR_PRES_CLK_3:
            FCLK_CLOCK_FREQ /= 3;
        break;
    }

    MCK_CLOCK_FREQ = FCLK_CLOCK_FREQ;

    switch(PMC->PMC_MCKR & PMC_MCKR_MDIV_Msk)
    {
        case PMC_MCKR_MDIV_EQ_PCK:
            MCK_CLOCK_FREQ >>= 0;
        break;
        case PMC_MCKR_MDIV_PCK_DIV2:
            MCK_CLOCK_FREQ >>= 1;
        break;
        case PMC_MCKR_MDIV_PCK_DIV4:
            MCK_CLOCK_FREQ >>= 2;
        break;
        case PMC_MCKR_MDIV_PCK_DIV3:
            MCK_CLOCK_FREQ /= 3;
        break;
    }
}

uint32_t pmc_get_pck_clock_freq(uint8_t ubID)
{
    if(ubID > 7)
        return 0;

    if(!(PMC->PMC_SR & BIT(ubID + 8)))
        return 0;

    uint32_t ulFreq = 0;

    switch(PMC->PMC_PCK[ubID] & PMC_PCK_CSS_Msk)
    {
        case PMC_PCK_CSS_SLOW_CLK:
            ulFreq = SLCK_CLOCK_FREQ;
        break;
        case PMC_PCK_CSS_MAIN_CLK:
            ulFreq = MAINCK_CLOCK_FREQ;
        break;
        case PMC_PCK_CSS_UPLL_CLK:
            ulFreq = UPLLCK_CLOCK_FREQ >> !!(PMC->PMC_MCKR & PMC_MCKR_UPLLDIV2);
        break;
        case PMC_PCK_CSS_PLLA_CLK:
            ulFreq = PLLACK_CLOCK_FREQ;
        break;
        case PMC_PCK_CSS_MCK:
            ulFreq = MCK_CLOCK_FREQ;
        break;
    }

    return ulFreq / (((PMC->PMC_PCK[ubID] & PMC_PCK_PRES_Msk) >> PMC_PCK_PRES_Pos) + 1);
}
uint32_t pmc_get_generic_clock_freq(uint8_t ubPeripheralID)
{
    if(ubPeripheralID > 127)
        return 0;

    PMC->PMC_PCR = (ubPeripheralID << PMC_PCR_PID_Pos);

    if(!(PMC->PMC_PCR & PMC_PCR_GCLKEN))
        return 0;

    uint32_t ulFreq = 0;

    switch(PMC->PMC_PCR & PMC_PCR_GCLKCSS_Msk)
    {
        case PMC_PCR_GCLKCSS_SLOW_CLK:
            ulFreq = SLCK_CLOCK_FREQ;
        break;
        case PMC_PCR_GCLKCSS_MAIN_CLK:
            ulFreq = MAINCK_CLOCK_FREQ;
        break;
        case PMC_PCR_GCLKCSS_PLLA_CLK:
            ulFreq = UPLLCK_CLOCK_FREQ >> !!(PMC->PMC_MCKR & PMC_MCKR_UPLLDIV2);
        break;
        case PMC_PCR_GCLKCSS_UPLL_CLK:
            ulFreq = PLLACK_CLOCK_FREQ;
        break;
        case PMC_PCR_GCLKCSS_MCK_CLK:
            ulFreq = MCK_CLOCK_FREQ;
        break;
    }

    return ulFreq / (((PMC->PMC_PCR & PMC_PCR_GCLKDIV_Msk) >> PMC_PCR_GCLKDIV_Pos) + 1);
}