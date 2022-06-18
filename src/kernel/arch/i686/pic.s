; 8259A Programmable Interrupt Controller

cpu 586
bits 32

%include "kernel.inc"

global pic_init
extern idt_set, _timer

%define PIC_MASTER_CMD  (0x20)
%define PIC_MASTER_DATA (0x21)
%define PIC_SLAVE_CMD   (0xA0)
%define PIC_SLAVE_DATA  (0xA1)

section .text
; ----------------------------------------------
pic_init:
    call    idt_set, IRQ_BASE_INTNUM + 0, PIC_IRQ_00, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 1, PIC_IRQ_01, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 2, PIC_IRQ_02, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 3, PIC_IRQ_03, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 4, PIC_IRQ_04, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 5, PIC_IRQ_05, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 6, PIC_IRQ_06, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 7, PIC_IRQ_07, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 8, PIC_IRQ_08, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM + 9, PIC_IRQ_09, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM +10, PIC_IRQ_0A, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM +11, PIC_IRQ_0B, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM +12, PIC_IRQ_0C, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM +13, PIC_IRQ_0D, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM +14, PIC_IRQ_0E, 1000111000000000b    ; P DPL=0 TYPE=1110
    call    idt_set, IRQ_BASE_INTNUM +15, PIC_IRQ_0F, 1000111000000000b    ; P DPL=0 TYPE=1110
    ; PIC map IRQ0-IRQF to ISR20-T2F
    mov     al, 0x11
    out     PIC_MASTER_CMD, al
    out     PIC_SLAVE_CMD, al
    mov     al, IRQ_BASE_INTNUM
    out     PIC_MASTER_DATA, al
    mov     al, IRQ_BASE_INTNUM+8
    out     PIC_SLAVE_DATA, al
    mov     al, 0x04
    out     PIC_MASTER_DATA, al
    mov     al, 0x02
    out     PIC_SLAVE_DATA, al
    mov     al, 1
    out     PIC_MASTER_DATA, al
    out     PIC_SLAVE_DATA, al
    xor     al, al
    out     PIC_MASTER_DATA, al
    out     PIC_SLAVE_DATA, al
    ; Setup PIT timer with ~1000 Hz
    mov     al, 0x36
    out     0x43, al
    ; 1193180/1000Hz = 0x04A9
    ; mov     al, 0xA9
    ; out     0x40, al
    ; mov     al, 0x04
    ; out     0x40, al

    ; (1193180/18.2Hz) = 0xffff
    mov     al, 0xff
    out     0x40, al
    mov     al, 0xff
    out     0x40, al    ; (1193180/1000Hz) >> 8
    ret

; ----------------------------------------------
; IRQ Handlers
; ----------------------------------------------
align 16
PIC_IRQ_00:      ; PIT
    push    eax
    mov     al, 0x20
    out     0x20, al
    pop     eax
    pusha
    rdtsc
    call    _timer, eax, edx
    popa
    iretd

align 16
PIC_IRQ_01:      ; KBC
    push    eax
    mov     al, 0x20
    out     0x20, al
    pop     eax
    iretd

PIC_IRQ_02:
PIC_IRQ_03:
PIC_IRQ_04:
PIC_IRQ_05:
PIC_IRQ_06:
PIC_IRQ_07:
    push    eax
    mov     al, 0x20
    out     0x20, al
    pop     eax
    iretd

align 16
PIC_IRQ_08:
PIC_IRQ_09:
PIC_IRQ_0A:
PIC_IRQ_0B:
PIC_IRQ_0C:
PIC_IRQ_0D:
PIC_IRQ_0E:
PIC_IRQ_0F:
    push    eax
    mov     al, 0x20
    out     0xA0, al        ; ack slave PIC
    out     0x20, al        ; ack master PIC
    pop     eax
    iretd
