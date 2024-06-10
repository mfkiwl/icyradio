#ifndef __IRQ_H__
#define __IRQ_H__

#include "custom_ops.h"

#define __enable_irqs() \
    __asm__ volatile(picorv32_maskirq_insn(zero, zero) "\n" ::: "memory")

#define __enable_irqs_save(r) \
    __asm__ volatile(picorv32_setq_insn(q3, tp) "\n" \
                     picorv32_maskirq_insn(tp, zero) "\n" \
                     "addi %[reg], tp, 0" "\n" \
                     picorv32_getq_insn(tp, q3) "\n" \
                     :: [reg] "r" (r) : "memory")

#define __set_irq_mask(r) \
    __asm__ volatile(picorv32_setq_insn(q3, tp) "\n" \
                     "addi tp, %[reg], 0" "\n" \
                     picorv32_maskirq_insn(zero, tp) "\n" \
                     picorv32_getq_insn(tp, q3) "\n" \
                     :: [reg] "r" (r) : "memory")

#define __set_irq_mask_save(r) \
    __asm__ volatile(picorv32_setq_insn(q3, tp) "\n" \
                     "addi tp, %[reg], 0" "\n" \
                     picorv32_maskirq_insn(tp, tp) "\n" \
                     "addi %[reg], tp, 0" "\n" \
                     picorv32_getq_insn(tp, q3) "\n" \
                     :: [reg] "r" (r) : "memory")

static inline void __disable_irqs()
{
    uint32_t r = 0xFFFFFFFF;

    __set_irq_mask(r);
}

#endif  // __IRQ_H__