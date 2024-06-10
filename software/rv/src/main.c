#include <stdint.h>
#include <math.h>
#include "irq.h"
#include "debug_macros.h"

void _putchar(char ch)
{
	*((volatile uint32_t*)0x10000000) = ch;
}

int init()
{
	DBGPRINTLN_CTX("hello world\n");

	return 0;
}
int main()
{
	uint32_t r = 0xFFFFFFFF;
	__set_irq_mask_save(r); // Mask all IRQs, return the current mask in r

	DBGPRINTLN("h\n");

	__set_irq_mask(r); // Restore the previous IRQ mask

	// char c = sqrtf(*((float *)0x80000000));

	// print_chr(c);

	return 0;
}

