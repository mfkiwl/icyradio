#include "adrf5720.h"

float ADRF5720_ATTENUATION;

uint8_t adrf5720_init()
{
    ADRF5720_ATTENUATION = 31.5f;

    return 1;
}

uint8_t adrf5720_set_attenuation(float fAttenuation)
{
    if(fAttenuation < 0.f || fAttenuation > 31.5f)
        return 0;

    uint8_t ubCode = fAttenuation / .5f;

    ubCode &= 0x3F;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        EXT_LO_ATT_LDIS();

        usart2_spi_write_byte(ubCode, 1);

        EXT_LO_ATT_LEN();
    }

    ADRF5720_ATTENUATION = (float)ubCode * .5f;

    return 1;
}
float adrf5720_get_attenuation()
{
    return ADRF5720_ATTENUATION;
}