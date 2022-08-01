cpu 586
bits 32

%include "kernel.inc"

global enter_ring3

align 16
; void enter_ring3(uint32_t ring3_eip, uint32_t ring3_esp);
enter_ring3:
    mov     ecx, SEG_DATA32_3 +3
    mov     ds, cx
    mov     es, cx
    mov     fs, cx
    mov     gs, cx
    mov     eax, [esp+8]    ; esp
    mov     ebx, [esp+4]    ; eip
    push    ecx
    push    eax
    push    0x0202          ; eflags
    push    SEG_CODE32_3 +3
    push    ebx
    iretd
