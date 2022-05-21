cpu 586
bits 32

global _start
extern kmain

; Data
; ---------------------------------------------------------------------
section .bss
align 16
; Mini-stack for bootstrap, we shall setup a proper stack afterward.
%define KSTACK_SIZE (8192)
kstack  resb KSTACK_SIZE

; Bootstrap Code
; ---------------------------------------------------------------------
section .text
_start:
    ; Setup mini-stack
    mov esp, kstack + KSTACK_SIZE

    call kmain

    cli
.halt:
    hlt
    jmp .halt
