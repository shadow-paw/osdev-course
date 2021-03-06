cpu 586
bits 32

%include "kernel.inc"

global bootstrap, MMU_PD, tss
extern kmain, idt_init, mmu_init
extern _kernel_end
extern ctor_start, ctor_end, dtor_start, dtor_end
extern _init, _fini

; Data
; ---------------------------------------------------------------------
section .bss
align 4096
; Mini-stack for bootstrap, we shall setup a proper stack afterward.
%define KSTACK_SIZE (8192)
kstack  resb KSTACK_SIZE
align 4096
MMU_PD resb 4096
MMU_PT resb 4096
tss    resb 104

section .data
align 16
gdtr    dw 6 *8 -1
        dd gdt
        dw 0
align 16
gdt     dd 0, 0
        dd 0x0000FFFF, 0x00CF9A00  ; 0x08 CODE32 DPL0
        dd 0x0000FFFF, 0x00CF9200  ; 0x10 DATA32 DPL0
        dd 0x0000FFFF, 0x00CFFA00  ; 0x18 CODE32 DPL3
        dd 0x0000FFFF, 0x00CFF200  ; 0x20 DATA32 DPL3
        dd 0, 0                    ; TSS

; Bootstrap Code
; ---------------------------------------------------------------------
section .text
bootstrap:
    ; Upon boot, ebx is point to multiboot information.
    ; We will avoid using ebx in the bootstrap and it should be
    ; preserved when calling ctor/dtor (cdecl convention).
    ; The multiboot information should be passed to kmain.

    ; Setup page tables
    ; Linear 1M -> Physical 1M (kernel loaded addr)
    ; Linear 3.75G -> Physical 1M (kernel loaded addr)
    mov     esi, VMA2PMA(MMU_PD)
    mov     edi, VMA2PMA(MMU_PT)
    ; PDT: both PMA and VMA points to the PTs
    mov     dword [esi + (KERNEL_PMA>>22)*4], VMA2PMA(MMU_PT) +3
    mov     dword [esi + (KERNEL_VMA>>22)*4], VMA2PMA(MMU_PT) +3
    ; PT: iterate all 4k pages until kernel end
    mov     ecx, VMA2PMA(_kernel_end)
    mov     eax, 3
.1:
    stosd
    add     eax, 4096
    cmp     eax, ecx
    jb      .1
    ; Enable paging
    mov     cr3, esi
    mov     eax, cr0
    or      eax, 0x80000000
    mov     cr0, eax
    ; eip -> higher end of virtual address space
    mov     eax, dword .higher_half
    jmp     eax
.higher_half:
    ; Load GDT & Reload selectors
    lgdt    [gdtr]
    mov     eax, SEG_DATA32_0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    mov     esp, dword kstack + KSTACK_SIZE
    jmp     dword SEG_CODE32_0:.reload_cs
.reload_cs:
    ; Unmap 1M
    mov     dword [esi + (KERNEL_PMA>>22)*4], 0
    mov     cr3, esi

    ; TSS
    mov     dword [tss + tss32_ss0], SEG_DATA32_0
    mov     dword [tss + tss32_esp0], esp
    mov     eax, tss
    shl     eax, 16
    or      eax, 67h                ; [Base 15..00][Limit 15..00]
    mov     [gdt + SEG_TSS], eax
    mov     eax, dword tss
    mov     edx, dword tss
    shr     edx, 16
    and     eax, 0xFF000000
    and     edx, 0x000000FF
    or      eax, edx
    or      eax, 0x00008900
    mov     [gdt + SEG_TSS +4], eax
    mov     eax, SEG_TSS
    ltr     ax

    ; Setup IDT
    call    idt_init

    ; Setup minimal C environment
    xor     ebp, ebp

    add     ebx, KERNEL_BASE ; multiboot info, convert to VMA

    ; init mmu
    push    ebx
    call    mmu_init
    ; add     esp, 4  ; leave it on stack for kmain

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

    call    kmain
    add     esp, 4  ; we got one from init_mmu

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

    cli
.halt:
    hlt
    jmp .halt
