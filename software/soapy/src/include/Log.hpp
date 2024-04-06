#pragma once

#include <SoapySDR/Logger.hpp>

#define DLOGF(LEVEL, FORMAT, ...) SoapySDR_logf(LEVEL, FORMAT, ##__VA_ARGS__)

// #define TRACE_STREAM // Used to disable SoapySDR_logf calls in the streaming API, to improve performance
#ifdef TRACE_STREAM
    #define DLOGF_S(LEVEL, FORMAT, ...) SoapySDR_logf(LEVEL, FORMAT, ##__VA_ARGS__)
#else
    #define DLOGF_S(LEVEL, FORMAT, ...) do {} while (0)
#endif

// #define TRACE_I2C // Used to disable scanning the I2C buses, which can take precious time