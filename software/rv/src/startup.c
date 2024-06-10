#include <stdint.h>
#include "custom_ops.h"
#include "irq.h"
#include "utils.h"

extern uint32_t _estack; // Stack pointer

extern uint32_t _siiram1; // DDR3 code source
extern uint32_t _siram1; // DDR3 code destination
extern uint32_t _eiram1;

extern uint32_t _sidram1; // DDR3 data source
extern uint32_t _sdram1; // DDR3 data destination
extern uint32_t _edram1;

extern uint32_t _sbss; // BSS destination
extern uint32_t _ebss;

extern uint32_t _end;


void _default_isr()
{
    while(1);
}

void __attribute__ ((weak)) __libc_init_array()
{

}

extern int init();
extern int main();

#define DEFAULT_ISR "_default_isr"

void BRAM_CODE __attribute__ ((naked)) __start()
{
    volatile uint32_t *src, *dst;

    src = &_siiram1;
    dst = &_siram1;

    while (dst < &_eiram1) // Copy DDR3 code
        *(dst++) = *(src++);

    src = &_sidram1;
    dst = &_sdram1;

    while (dst < &_edram1) // Copy DDR3 data
        *(dst++) = *(src++);

    src = 0;
    dst = &_sbss;

    while (dst < &_ebss) // Zero BSS
        *(dst++) = 0;

    __asm__ volatile("lui sp, %%hi(%[stack])" "\n" :: [stack] "i" (&_estack));
    __asm__ volatile("addi sp, sp, %%lo(%[stack])" "\n" :: [stack] "i" (&_estack));

    __libc_init_array();

    __enable_irqs();

    init();
    main();

    __disable_irqs();
    while(1);
}
void BRAM_CODE __irq_handler(uintptr_t pc, uint32_t irqs, uint32_t *regs)
{
	static unsigned int ext_irq_4_count = 0;
	static unsigned int ext_irq_5_count = 0;
	static unsigned int timer_irq_count = 0;

	// checking compressed isa q0 reg handling
	if ((irqs & 6) != 0) {
		uint32_t _pc = (pc & 1) ? pc - 3 : pc - 4;
		uint32_t instr = *(uint16_t*)_pc;

		if ((instr & 3) == 3)
			instr = instr | (*(uint16_t*)(_pc + 2)) << 16;

		if (((instr & 3) != 3) != (pc & 1)) {
			// print_str("Mismatch between q0 LSB and decoded instruction word! q0=0x");
			// print_hex(regs[0], 8);
			// print_str(", instr=0x");
			// if ((instr & 3) == 3)
			// 	print_hex(instr, 8);
			// else
			// 	print_hex(instr, 4);
			// print_str("\n");
			__asm__ volatile ("ebreak");
		}
	}

	if ((irqs & (1<<4)) != 0) {
		ext_irq_4_count++;
		// print_str("[EXT-IRQ-4]");
	}

	if ((irqs & (1<<5)) != 0) {
		ext_irq_5_count++;
		// print_str("[EXT-IRQ-5]");
	}

	if ((irqs & 1) != 0) {
		timer_irq_count++;
		// print_str("[TIMER-IRQ]");
	}

	if ((irqs & 6) != 0)
	{
		uint32_t _pc = (pc & 1) ? pc - 3 : pc - 4;
		uint32_t instr = *(uint16_t*)_pc;

		if ((instr & 3) == 3)
			instr = instr | (*(uint16_t*)(_pc + 2)) << 16;

		// print_str("\n");
		// print_str("------------------------------------------------------------\n");

		if ((irqs & 2) != 0) {
			if (instr == 0x00100073 || instr == 0x9002) {
				// print_str("EBREAK instruction at 0x");
				// print_hex(pc, 8);
				// print_str("\n");
			} else {
				// print_str("Illegal Instruction at 0x");
				// print_hex(pc, 8);
				// print_str(": 0x");
				// print_hex(instr, ((instr & 3) == 3) ? 8 : 4);
				// print_str("\n");
			}
		}

		if ((irqs & 4) != 0) {
			// print_str("Bus error in Instruction at 0x");
			// print_hex(pc, 8);
			// print_str(": 0x");
			// print_hex(instr, ((instr & 3) == 3) ? 8 : 4);
			// print_str("\n");
		}

		for (int i = 0; i < 8; i++)
		for (int k = 0; k < 4; k++)
		{
			int r = i + k*8;

			if (r == 0) {
				// print_str("pc  ");
			} else
			if (r < 10) {
				// print_chr('x');
				// print_chr('0' + r);
				// print_chr(' ');
				// print_chr(' ');
			} else
			if (r < 20) {
				// print_chr('x');
				// print_chr('1');
				// print_chr('0' + r - 10);
				// print_chr(' ');
			} else
			if (r < 30) {
				// print_chr('x');
				// print_chr('2');
				// print_chr('0' + r - 20);
				// print_chr(' ');
			} else {
				// print_chr('x');
				// print_chr('3');
				// print_chr('0' + r - 30);
				// print_chr(' ');
			}

			// print_hex(regs[r], 8);
			// print_str(k == 3 ? "\n" : "    ");
		}

		// print_str("------------------------------------------------------------\n");

		// print_str("Number of fast external IRQs counted: ");
		// print_dec(ext_irq_4_count);
		// print_str("\n");

		// print_str("Number of slow external IRQs counted: ");
		// print_dec(ext_irq_5_count);
		// print_str("\n");

		// print_str("Number of timer IRQs counted: ");
		// print_dec(timer_irq_count);
		// print_str("\n");

		__asm__ volatile ("ebreak");
	}
}

void __attribute__ ((naked, section(".__reset_vec"))) __reset_vec()
{
    // Max 16 bytes
    __asm__ volatile("lui x1, %%hi(%[offset])" "\n" :: [offset] "i" (__start));
    __asm__ volatile("addi x1, x1, %%lo(%[offset])" "\n" :: [offset] "i" (__start));
    __asm__ volatile("jalr zero, x1, 0" "\n");
    __asm__ volatile("nop" "\n");
}
void __attribute__ ((naked, section(".__irq_vec")))   __irq_vec()
{
    __asm__ volatile("addi sp, sp, -64" "\n");

    __asm__ volatile("sw x1,  0(sp)" "\n");
    __asm__ volatile("sw x5,  4(sp)" "\n");
    __asm__ volatile("sw x6,  8(sp)" "\n");
    __asm__ volatile("sw x7,  12(sp)" "\n");
    __asm__ volatile("sw x10, 16(sp)" "\n");
    __asm__ volatile("sw x11, 20(sp)" "\n");
    __asm__ volatile("sw x12, 24(sp)" "\n");
    __asm__ volatile("sw x13, 28(sp)" "\n");
    __asm__ volatile("sw x14, 32(sp)" "\n");
    __asm__ volatile("sw x15, 36(sp)" "\n");
    __asm__ volatile("sw x16, 40(sp)" "\n");
    __asm__ volatile("sw x17, 44(sp)" "\n");
    __asm__ volatile("sw x28, 48(sp)" "\n");
    __asm__ volatile("sw x29, 52(sp)" "\n");
    __asm__ volatile("sw x30, 56(sp)" "\n");
    __asm__ volatile("sw x31, 60(sp)" "\n");

    __asm__ volatile(picorv32_getq_insn(a0, q0) "\n");
    // __asm__ volatile("addi a0, x3, 0" "\n");
    __asm__ volatile(picorv32_getq_insn(a1, q1) "\n");
    // __asm__ volatile("addi a1, x4, 0" "\n");
    __asm__ volatile("addi a2, sp, 0" "\n");

    __asm__ volatile("lui x1, %%hi(%[offset])" "\n" :: [offset] "i" (__irq_handler));
    __asm__ volatile("addi x1, x1, %%lo(%[offset])" "\n" :: [offset] "i" (__irq_handler));
    __asm__ volatile("jalr ra, x1, 0" "\n");

    __asm__ volatile("lw x1,  0(sp)" "\n");
    __asm__ volatile("lw x5,  4(sp)" "\n");
    __asm__ volatile("lw x6,  8(sp)" "\n");
    __asm__ volatile("lw x7,  12(sp)" "\n");
    __asm__ volatile("lw x10, 16(sp)" "\n");
    __asm__ volatile("lw x11, 20(sp)" "\n");
    __asm__ volatile("lw x12, 24(sp)" "\n");
    __asm__ volatile("lw x13, 28(sp)" "\n");
    __asm__ volatile("lw x14, 32(sp)" "\n");
    __asm__ volatile("lw x15, 36(sp)" "\n");
    __asm__ volatile("lw x16, 40(sp)" "\n");
    __asm__ volatile("lw x17, 44(sp)" "\n");
    __asm__ volatile("lw x28, 48(sp)" "\n");
    __asm__ volatile("lw x29, 52(sp)" "\n");
    __asm__ volatile("lw x30, 56(sp)" "\n");
    __asm__ volatile("lw x31, 60(sp)" "\n");

    __asm__ volatile("addi sp, sp, 64" "\n");

    __asm__ volatile(picorv32_retirq_insn() "\n");
};

void _nmi_isr()                           __attribute__ ((weak,  alias (DEFAULT_ISR)));

