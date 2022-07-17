cpu 586
bits 32

%include "kernel.inc"

global _syscall
extern syscall_table

section .text
align 16
_syscall:
    cmp     eax, 12
    ja      .fault
    ; save registers
    sub     esp, 10*4
    mov     [esp+20], ebx
    mov     [esp+20+4], ecx
    mov     [esp+20+8], edx
    mov     [esp+20+12], esi
    mov     [esp+20+16], edi
    ; invoke handler
    mov     [esp], ebx
    mov     [esp+4], ecx
    mov     [esp+8], edx
    mov     [esp+12], esi
    mov     [esp+16], edi
    call    dword [syscall_table + eax*4]
    ; restore registers
    mov     ebx, [esp+20]
    mov     ecx, [esp+20+4]
    mov     edx, [esp+20+8]
    mov     esi, [esp+20+12]
    mov     edi, [esp+20+16]
    add     esp, 10*4
    iretd
.fault:
    mov     eax, -1
    iretd
