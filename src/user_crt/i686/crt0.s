; -----------------------------------------------------------
;
; crt0.nasm
;
; Entry point for userland application
; This stub initialize C runtime environment
; -----------------------------------------------------------
cpu 586
bits 32

global _start
extern main, _exit
extern ctor_start, ctor_end, dtor_start, dtor_end

section .data
    env     dd  0

section .text
_start:
    xor     esi, esi
    xor     edi, edi
    xor     ebx, ebx
    xor     ecx, ecx
    xor     edx, edx
    xor     ebp, ebp

    ; constructors
    mov     esi, ctor_start
.ctors_next:
    cmp     esi, ctor_end
    jae     .ctors_done
    mov     eax, [esi]
    ; skip -1 and 0 as doc in https://gcc.gnu.org/onlinedocs/gccint/Initialization.html
    or      eax, eax
    jz      .ctors_skip
    cmp     eax, -1
    je      .ctors_skip
    call    eax
.ctors_skip:
    add     esi, 4
    jmp     .ctors_next
.ctors_done:

    push    env
    push    dword 0
    call    main

    ; exit code
    push    eax

    ; destructors
    mov     esi, dtor_end
.dtors_next:
    sub     esi, 4
    cmp     esi, dtor_start
    jb      .dtors_done
    mov     eax, [esi]
    ; skip -1 and 0 as doc in https://gcc.gnu.org/onlinedocs/gccint/Initialization.html
    or      eax, eax
    jz      .dtors_next
    cmp     eax, -1
    je      .dtors_next
    call    eax
    jmp     .dtors_next
.dtors_done:

    call    _exit
    ret
