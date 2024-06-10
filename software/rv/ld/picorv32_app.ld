ENTRY(__start)

_min_heap_size = 0x800;
_min_stack_size = 0x400;

MEMORY
{
    irom0  (rx)     : ORIGIN = 0x00000000, LENGTH = 0x00800000
    drom0  (r)      : ORIGIN = 0x00000000, LENGTH = 0x00000000
    iram0  (rwx)    : ORIGIN = 0x01000000, LENGTH = 0x0001E000
    iram1  (rwx)    : ORIGIN = 0x20000000, LENGTH = 0x00000000
    dram0  (rw)     : ORIGIN = 0x0101E000, LENGTH = 0x00002000
    dram1  (rw)     : ORIGIN = 0x20000000, LENGTH = 0x20000000
}

/* Initial stack pointer (must be 16 byte aligned) */
_estack = (ORIGIN(dram1) + LENGTH(dram1)) & ~15;

SECTIONS
{
    /* Flash Code */
    .irom0.text :
    {
        . = ALIGN(4);
        _sirom0 = .;

        *(.irom0.text)     /* .text sections (code) */
        *(.irom0.text*)    /* .text* sections (code) */
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.srodata)         /* .srodata sections (constants, strings, etc.) */
        *(.srodata*)        /* .srodata* sections (constants, strings, etc.) */
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */

        KEEP(*(.init))
        KEEP(*(.fini))

        . = ALIGN(4);
        _eirom0 = .;
    } > irom0

    /* RV */
    .riscv.attributes :
    {
        *(.riscv.attributes)
    } > irom0

    /* C Array init/fini */
    .preinit_array :
    {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array*))
        PROVIDE_HIDDEN(__preinit_array_end = .);
    } > irom0
    .init_array :
    {
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array*))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > irom0
    .fini_array :
    {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(.fini_array*))
        KEEP(*(SORT(.fini_array.*)))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > irom0

    /* BRAM Code */
    .iram0.text :
    {
        . = ALIGN(4);
        _siram0 = .;

        *(.__reset_vec)
        KEEP(*(.__reset_vec))

        . = ALIGN(16);

        *(.__irq_vec)
        KEEP(*(.__irq_vec))

        *(.iram0.text)           /* .text sections (code) */
        *(.iram0.text*)          /* .text* sections (code) */

        . = ALIGN(4);
        _eiram0 = .;
    } > iram0

    /* BRAM Data */
    .dram0.data :
    {
        . = ALIGN(4);
        _sdram0 = .;        /* create a global symbol at data start */

        *(.__irq_regs)
        *(.__irq_stack)
        KEEP(*(.__irq_regs))
        KEEP(*(.__irq_stack))

        *(.dram0.data)      /* .data sections */
        *(.dram0.data*)     /* .data* sections */

        . = ALIGN(4);
        _edram0 = .;        /* define a global symbol at data end */
    } > dram0

    /* DDR3 Code */
    _siiram1 = LOADADDR(.iram1.text);

    .iram1.text :
    {
        . = ALIGN(4);
        _siram1 = .;

        *(.iram1.text)           /* .text sections (code) */
        *(.iram1.text*)          /* .text* sections (code) */

        . = ALIGN(4);
        _eiram1 = .;
    } > iram1 AT > irom0

    /* DDR3 Data */
    _sidram1 = LOADADDR(.dram1.data);

    .dram1.data :
    {
        . = ALIGN(4);
        _sdram1 = .;        /* create a global symbol at data start */

        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        *(.sdata)           /* .sdata sections */
        *(.sdata*)          /* .sdata* sections */

        . = ALIGN(4);
        _edram1 = .;        /* define a global symbol at data end */
    } > dram1 AT > irom0

    /* Flash Data */
    .drom0.data :
    {
        . = ALIGN(4);
        _sdrom0 = .;

        *(.drom0.data)           /* .data sections */
        *(.drom0.data*)          /* .data* sections */

        . = ALIGN(4);
        _edrom0 = .;
    } > drom0

    /* BSS */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;

        *(.sbss)
        *(.sbss*)
        *(.bss)
        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } > dram1

    PROVIDE(end = _ebss);
    PROVIDE(_end = _ebss);

    /* Ensure minimum stack & heap */
    .min_heap_stack :
    {
        . = ALIGN(4);

        . = . + _min_heap_size;
        . = . + _min_stack_size;

        . = ALIGN(4);
    } > dram1

    /* Remove unused code from libs */
    /DISCARD/ :
    {
        libgcc.a(*)
        libc.a(*)
        libm.a(*)
    }
}
