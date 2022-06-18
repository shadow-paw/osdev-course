; Multiboot Header
; ---------------------------------------------------------------------
%define MULTIBOOT_ALIGN    (1<<0)                                  ; align loaded modules on page boundaries
%define MULTIBOOT_MEMINFO  (1<<1)                                  ; provide memory map
%define MULTIBOOT_FLAGS    (MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO)   ; this is the Multiboot 'flag' field
%define MULTIBOOT_MAGIC    (0x1BADB002)                            ; 'magic number' lets bootloader find the header
%define MULTIBOOT_CHECKSUM (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS))  ; checksum of above, to prove we are multiboot

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM
