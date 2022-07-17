#include "kmalloc.h"
#include "tcb.h"

bool tcb_init(
    struct TCB* tcb,
    struct PROCESS* process,
    MMU_PHYADDR pagedir,
    uint32_t priority, uint32_t quantum,
    size_t kstack_size, TCB_FUNC startfunc, TCB_FUNC exitfunc, void* ud
) {
    void* kstack;
    uint32_t* esp;
    __builtin_memset(tcb, 0, sizeof(struct TCB));
    if ((kstack  = kmalloc(kstack_size)) == NULL) return false;
    esp = (uint32_t*)((uintptr_t)kstack + kstack_size);
    esp--; *esp = (uint32_t)ud;
    esp--; *esp = (uint32_t)ud;
    esp--; *esp = (uint32_t)exitfunc;
    esp--; *esp = (uint32_t)startfunc;
    tcb->context.pagedir = pagedir;
    tcb->context.eflags = 0x202;        // Interrupt enabled
    tcb->context.esp = (uint32_t) esp;
    tcb->context.esp0 = tcb->context.esp - 32;
    tcb->process = process;
    tcb->kstack = kstack;
    tcb->priority = priority;
    tcb->quantum = quantum;
    tcb->elapsed = 0;
    tcb->prev = tcb->next = NULL;
    return true;
}
