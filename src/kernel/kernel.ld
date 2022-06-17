ENTRY(bootstrap)

/* Our kernel will be sit on top -256MB */
KERNEL_BASE = 0xF0000000;
KERNEL_PMA = 0x00100000;
KERNEL_VMA = KERNEL_BASE + KERNEL_PMA;

SECTIONS
{
    . = KERNEL_VMA;
    _kernel_start = .;

    .text BLOCK(4K) : AT(KERNEL_PMA) {
        *(.multiboot)
        *(.init)
        *(.fini)
        *(.text)
		*(.gnu.linkonce.t*)
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
        . = ALIGN(16);
		*(.gnu.linkonce.r*)
        . = ALIGN(16);
		ctor_start = .;
        *(SORT(.ctors*))
        ctor_end = .;
        dtor_start = .;
        *(SORT(.dtors*))
        dtor_end = .;
    }

    .data BLOCK(4K) :ALIGN(4K) {
        *(.data)
        . = ALIGN(16);
		*(.gnu.linkonce.d*)
        . = ALIGN(16);
        *(.eh_frame*)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
        sbss = .;
        *(COMMON)
        *(.bss)
        . = ALIGN(16);
        *(.gnu.linkonce.b*)
        ebss = .;
    }

    . = ALIGN(4096);
    _kernel_end = .;

	/DISCARD/ :
    {
        *(.comment)
    }
}
