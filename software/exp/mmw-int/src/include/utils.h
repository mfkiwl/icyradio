#ifndef __UTILS_H__
#define __UTILS_H__

#include <sam.h>

// Memory sections & aliases
#define IRAM0_TEXT __attribute__ ((section(".iram0.text")))
#define IRAM1_TEXT __attribute__ ((section(".iram1.text")))
#define IROM1_TEXT __attribute__ ((section(".irom1.text")))
#define DROM0_DATA __attribute__ ((section(".drom0.data")))

#define RAM_CODE IRAM0_TEXT
#define RAMH_CODE IRAM1_TEXT
#define BOOT_CODE IROM1_TEXT
#define USER_DATA DROM0_DATA

// Macro to make a dummy read
#define REG_DISCARD(reg) __asm__ volatile ("" : : "r" (*(volatile uint32_t *)(reg)))

// Macros to access bit band/set/clear regions
#define PERI_REG_BIT_SET_ADDR(reg)          (PER_BITSET_MEM_BASE + ((uint32_t)(reg) - PER_MEM_BASE))
#define PERI_REG_BIT_SET(reg)               *(volatile uint32_t *)PERI_REG_BIT_SET_ADDR(reg)
#define PERI_REG_BIT_CLEAR_ADDR(reg)        (PER_BITCLR_MEM_BASE + ((uint32_t)(reg) - PER_MEM_BASE))
#define PERI_REG_BIT_CLEAR(reg)             *(volatile uint32_t *)PERI_REG_BIT_CLEAR_ADDR(reg)

// Macro to check if the address has a valid app
#define IS_VALID_APP(addr) ((*(volatile uint32_t *)(addr) & 0xFFF80000) == SRAM_BASE)

// Macro to get the bit value
#define BIT(x) (1 << (x))

// Math
#define ABS(x)                  ((x) < 0 ? (-(x)) : (x))
#define D_ABS(x, y)             ((x) > (y) ? ((x) - (y)) : ((y) - (x)))
#define MIN(x, y)               (((x) < (y)) ? (x) : (y))
#define MIN_T(t, x, y)          (t)MIN((t)(x), (t)(y))
#define MAX(x, y)               (((x) > (y)) ? (x) : (y))
#define MAX_T(t, x, y)          (t)MAX((t)(x), (t)(y))
#define CLAMP(x, min, max)      (MAX(MIN((x), (max)), (min)))
#define CLAMP_T(t, x, min, max) (t)CLAMP((t)(x), (t)(min), (t)(max))
#define DIV_CEIL(x,y)       (((x) + (y) - 1) / (y))
#define DIV_ROUND(x, y)     (((x) + (y) / 2) / (y))

// Array size
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// Printf macros to print bits
#define UINT8BITSTR         "%c%c%c%c%c%c%c%c"
#define UINT16BITSTR        UINT8BITSTR UINT8BITSTR
#define UINT32BITSTR        UINT16BITSTR UINT16BITSTR
#define UINT64BITSTR        UINT32BITSTR UINT32BITSTR
#define UINT82BITSTR(b)     ((b) & 0x80 ? '1' : '0'), ((b) & 0x40 ? '1' : '0'), ((b) & 0x20 ? '1' : '0'), ((b) & 0x10 ? '1' : '0'), ((b) & 0x08 ? '1' : '0'), ((b) & 0x04 ? '1' : '0'), ((b) & 0x02 ? '1' : '0'), ((b) & 0x01 ? '1' : '0')
#define UINT162BITSTR(b)    UINT82BITSTR(((b) >> 8) & 0xFF), UINT82BITSTR(((b) >> 0) & 0xFF)
#define UINT322BITSTR(b)    UINT162BITSTR(((b) >> 16) & 0xFFFF), UINT162BITSTR(((b) >> 0) & 0xFFFF)
#define UINT642BITSTR(b)    UINT322BITSTR(((b) >> 32) & 0xFFFFFFFF), UINT322BITSTR(((b) >> 0) & 0xFFFFFFFF)

#endif  // __UTILS_H__
