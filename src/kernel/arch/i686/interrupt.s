cpu 586
bits 32

%include "kernel.inc"

global idt_init, idt_set
extern INT_00, INT_01, INT_02, INT_03, INT_04, INT_05, INT_06, INT_07
extern INT_08,         INT_0A, INT_0B, INT_0C, INT_0D, INT_0E, INT_0F
extern INT_10, INT_11, INT_12, INT_13

section .bss
; ----------------------------------------------
align 4096
idt resq 256         ; 256 interrupts

section .data
; ----------------------------------------------
align 16
idtr    dw  256*8-1
        dd  idt
        dw  0

section .text
; ----------------------------------------------
idt_init:
    call    idt_set, 0x00, dword _INT_00, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x01, dword _INT_01, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x02, dword _INT_02, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x03, dword _INT_03, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x04, dword _INT_04, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x05, dword _INT_05, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x06, dword _INT_06, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x07, dword _INT_07, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x08, dword _INT_08, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x0A, dword _INT_0A, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x0B, dword _INT_0B, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x0C, dword _INT_0C, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x0D, dword _INT_0D, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x0E, dword _INT_0E, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x10, dword _INT_10, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x11, dword _INT_11, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x12, dword _INT_12, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, 0x13, dword _INT_13, 1000111000000000b    ; P DPL=0 TYPE=1110
    lidt    [idtr]
    ret

; ----------------------------------------------
; idt_set(num, function, access (P:1 DPL:2 0:1 TYPE:4 0:5)
idt_set:
    push    eax
    push    ecx
    push    edx
    mov     eax, [esp+20]
    mov     ecx, [esp+24]
    mov     edx, eax
    and     eax, 0x0000FFFF
    and     edx, 0xFFFF0000
    or      eax, SEG_CODE32_0 << 16
    or      edx, ecx
    mov     ecx, [esp+16]
    mov     [idt +ecx*8], eax
    mov     [idt +ecx*8+4], edx
    pop     edx
    pop     ecx
    pop     eax
    ret

; INT00
; ----------------------------------------------
align 16
_INT_00:
    pusha
    call    INT_00
    popa
    iretd

; INT01
; ----------------------------------------------
align 16
_INT_01:
    pusha
    call    INT_01
    popa
    iretd

; INT02
; ----------------------------------------------
align 16
_INT_02:
    pusha
    call    INT_02
    popa
    iretd

; INT03
; ----------------------------------------------
align 16
_INT_03:
    pusha
    call    INT_03
    popa
    iretd

; INT04
; ----------------------------------------------
align 16
_INT_04:
    pusha
    call    INT_04
    popa
    iretd

; INT05
; ----------------------------------------------
align 16
_INT_05:
    pusha
    call    INT_05
    popa
    iretd

; INT06
; ----------------------------------------------
align 16
_INT_06:
    pusha
    call    INT_06
    popa
    iretd

; INT07
; ----------------------------------------------
align 16
_INT_07:
    pusha
    call    INT_07
    popa
    iretd

; INT08
; ----------------------------------------------
align 16
_INT_08:
    pusha
    call    INT_08
    popa
    iretd

; INT0A
; ----------------------------------------------
align 16
_INT_0A:
    pusha
    push    dword [esp+32]
    call    INT_0A
    add     esp, 4
    popa
    add     esp, 4
    iretd

; INT0B
; ----------------------------------------------
align 16
_INT_0B:
    pusha
    push    dword [esp+32]
    call    INT_0B
    add     esp, 4
    popa
    add     esp, 4
    iretd

; INT0C
; ----------------------------------------------
align 16
_INT_0C:
    pusha
    push    dword [esp+32]
    call    INT_0C
    add     esp, 4
    popa
    add     esp, 4
    iretd

; INT0D
; ----------------------------------------------
align 16
_INT_0D:
    pusha
    push    dword [esp+32]
    push    dword [esp+40]
    call    INT_0D
    add     esp, 8
    popa
    add     esp, 4
    iretd

; INT0E
; ----------------------------------------------
align 16
_INT_0E:
    pusha
    mov     eax, cr2
    push    dword [esp+36]
    push    eax
    push    dword [esp+40]
    call    INT_0E
    add     esp, 12
    popa
    add     esp, 4
    iretd

; INT10
; ----------------------------------------------
align 16
_INT_10:
    pusha
    call    INT_10
    popa
    iretd

; INT11
; ----------------------------------------------
align 16
_INT_11:
    pusha
    push    dword [esp+32]
    call    INT_11
    add     esp, 4
    popa
    add     esp, 4
    iretd

; INT12
; ----------------------------------------------
align 16
_INT_12:
    pusha
    call    INT_12
    popa
    iretd

; INT13
; ----------------------------------------------
align 16
_INT_13:
    pusha
    call    INT_13
    popa
    iretd