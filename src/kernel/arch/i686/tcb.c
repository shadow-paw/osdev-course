#include "kmalloc.h"
#include "tcb.h"

bool tcb_init(struct TCB* tcb, uint32_t priority, uint32_t quantum, size_t kstack_size, TCB_FUNC startfunc, TCB_FUNC exitfunc, void* ud) {
    void* kstack;
    uint32_t* esp;
    __builtin_memset(tcb, 0, sizeof(struct TCB));
    if ((kstack  = kmalloc(kstack_size)) == NULL) return false;
    esp = (uint32_t*)((uintptr_t)kstack + kstack_size);
    esp--; *esp = (uint32_t)ud;
    esp--; *esp = (uint32_t)ud;
    esp--; *esp = (uint32_t)exitfunc;
    esp--; *esp = (uint32_t)startfunc;
    tcb->kstack = kstack;
    tcb->cpu.eflags = 0x202;        // Interrupt enabled
    tcb->cpu.esp = (uint32_t) esp;
    tcb->cpu.esp0 = tcb->cpu.esp - 32;
    tcb->priority = priority;
    tcb->quantum = quantum;
    tcb->elapsed = 0;
    tcb->prev = tcb->next = NULL;
    return true;
}
