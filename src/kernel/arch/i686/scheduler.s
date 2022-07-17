cpu 586
bits 32

%include "kernel.inc"

global scheduler_tick, scheduler_yield
extern tss, g_scheduler, scheduler_pick

struc SCHEDULER
    s_elapsed resd    1
    s_quantum resd    1
    s_current resd    1
endstruc
struc TCB_CONTEXT
    tcb_eax         resd    1
    tcb_ebx         resd    1
    tcb_ecx         resd    1
    tcb_edx         resd    1
    tcb_ebp         resd    1
    tcb_esp         resd    1
    tcb_esi         resd    1
    tcb_edi         resd    1
    tcb_eflags      resd    1
    tcb_esp0        resd    1
    tcb_pagedir     resd    1
endstruc

section .text
align 16
scheduler_tick:
    inc     dword [g_scheduler + s_elapsed]
    dec     dword [g_scheduler + s_quantum]
    jz      scheduler_yield
    ret

align 16
scheduler_yield:
    pushf
    cli
    push    esi
    mov     esi, [g_scheduler + s_current]
    ; Save CPU Context
    mov     [esi +tcb_eax], eax
    mov     [esi +tcb_ebx], ebx
    mov     [esi +tcb_ecx], ecx
    mov     [esi +tcb_edx], edx
    pop     eax                         ; esi
    pop     ebx                         ; eflags
    mov     [esi +tcb_ebp], ebp
    mov     [esi +tcb_esp], esp
    mov     [esi +tcb_esi], eax
    mov     [esi +tcb_edi], edi
    mov     [esi +tcb_eflags], ebx
    ; Pick next process
    call    scheduler_pick
    mov     [g_scheduler + s_current], eax
    ; Load CPU Context
    mov     ebx, [eax + tcb_pagedir]
    or      ebx, ebx
    jz      .skip_pagedir
    mov     cr3, ebx
.skip_pagedir:
    mov     ebx, [eax +tcb_eflags]
    mov     ecx, [eax +tcb_esp0]
    mov     ebp, [eax +tcb_ebp]
    mov     esp, [eax +tcb_esp]
    mov     [tss + tss32_esp0], ecx
    mov     esi, [eax +tcb_esi]
    mov     edi, [eax +tcb_edi]
    push    ebx
    mov     ebx, [eax +tcb_ebx]
    mov     ecx, [eax +tcb_ecx]
    mov     edx, [eax +tcb_edx]
    mov     eax, [eax +tcb_eax]
    popf
    ret
