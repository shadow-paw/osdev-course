ENTRY (_start)

PROGRAM_VMA = 0x00100000;
SECTIONS{
    . = PROGRAM_VMA;
    .text : {
        *crt0_a.o(.text*)
        *(.text*)
        *(.gnu.linkonce.t*)
        . = ALIGN(4096);
    }
    .rodata : {
        ctor_start = .;
        *(SORT(.ctors*))
        ctor_end = .;
        dtor_start = .;
        *(SORT(.dtors*))
        dtor_end = .;
        *(.rodata*)
        *(.gnu.linkonce.r*)
        . = ALIGN(4096);
    }
    .data : {
        *(.data*)
        *(.gnu.linkonce.d*)
        . = ALIGN(4096);
    }
    .bss : {
        *(COMMON*)
        *(.bss*)
        *(.gnu.linkonce.b*)
        . = ALIGN(4096);
    }
    end = .;
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame) /* You should discard this unless you're implementing runtime support for C++ exceptions. */
    }
}
