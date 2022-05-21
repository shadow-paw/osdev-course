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
        *(.text)
		*(.gnu.linkonce.t*)
    }

    .rodata BLOCK(4K) : ALIGN(4K) {
        *(.rodata)
		*(.gnu.linkonce.r*)
		ctor_start = .;
        *(SORT(.ctors*))
        ctor_end = .;
        dtor_start = .;
        *(SORT(.dtors*))
        dtor_end = .;
    }

    .data BLOCK(4K) :ALIGN(4K) {
        *(.data)
		*(.gnu.linkonce.d*)
    }

    .bss BLOCK(4K) : ALIGN(4K) {
        sbss = .;
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
        ebss = .;
    }

    _kernel_end = .;

	/DISCARD/ :
    {
        *(.comment)
        *(.eh_frame*)
    }
}
