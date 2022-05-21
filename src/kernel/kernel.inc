%ifndef KERNEL_KERNEL_INC_
%define KERNEL_KERNEL_INC_

; Address Translation used at early kernel stage
; ----------------------------------------------
; Our kernel will be sit on top -256MB
%define KERNEL_BASE (0xF0000000)
%define KERNEL_PMA  (0x00100000)
%define KERNEL_VMA  (KERNEL_BASE + KERNEL_PMA)
%define PMA2VMA(x)  ((x) + KERNEL_BASE)
%define VMA2PMA(x)  ((x) - KERNEL_BASE)

; Selector ID
; ----------------------------------------------
%define SEG_CODE32_0 (0x08)
%define SEG_DATA32_0 (0x10)
%define SEG_CODE32_3 (0x18)
%define SEG_DATA32_3 (0x20)
%define SEG_TSS      (0x28)

; Interrupt Numbering
; ----------------------------------------------
%define IRQ_BASE_INTNUM (0x20)

; TSS32
; ----------------------------------------------
struc TSS32
    tss32_plink     resw    1
                    resw    1
    tss32_esp0      resd    1
    tss32_ss0       resd    1
    tss32_esp1      resd    1
    tss32_ss1       resd    1
    tss32_esp2      resd    1
    tss32_ss2       resd    1
    tss32_cr3       resd    1
    tss32_eip       resd    1
    tss32_eflags    resd    1
    tss32_eax       resd    1
    tss32_ecx       resd    1
    tss32_edx       resd    1
    tss32_ebx       resd    1
    tss32_esp       resd    1
    tss32_ebp       resd    1
    tss32_esi       resd    1
    tss32_edi       resd    1
    tss32_es        resd    1
    tss32_cs        resd    1
    tss32_ss        resd    1
    tss32_ds        resd    1
    tss32_fs        resd    1
    tss32_gs        resd    1
    tss32_ldts      resd    1
    tss32_trap      resw    1
    tss32_iobase    resw    1
endstruc

%endif
