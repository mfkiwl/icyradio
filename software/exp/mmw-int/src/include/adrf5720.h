#ifndef __ADRF5720_H__
#define __ADRF5720_H__

#include <em_device.h>
#include "gpio.h"
#include "usart.h"
#include "atomic.h"
#include "systick.h"

uint8_t adrf5720_init();

uint8_t adrf5720_set_attenuation(float fAttenuation);
float adrf5720_get_attenuation();

#endif // __ADRF5720_H__