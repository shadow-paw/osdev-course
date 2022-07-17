#include "kmalloc.h"
#include "mmu.h"
#include "scheduler.h"
#include "process.h"

uint32_t _next_process_id = 1;

struct PROCESS_START_DATA {
    struct PROCESS* process;
    PROCESS_STARTFUNC startfunc;
    void* ud;
};

void _process_startfunc(void* ud) {
    struct PROCESS_START_DATA* psd = (struct PROCESS_START_DATA*)ud;
    psd->startfunc(psd->process, psd->ud);
}
void _process_exitfunc(void* ud) {
    struct PROCESS_START_DATA* psd = (struct PROCESS_START_DATA*)ud;
    kfree(psd->process);
    kfree(psd);
    scheduler_exitthread();
    // TODO: release process->pagedir and walk the page tree in zombie cleaner thread
}
struct PROCESS* process_current() {
    struct TCB* tcb = scheduler_current();
    return tcb->process;
}
struct PROCESS* process_create(PROCESS_STARTFUNC startfunc, void* ud, uint32_t priority) {
    struct PROCESS* process = (struct PROCESS*)kmalloc(sizeof(struct PROCESS));
    __builtin_memset(process, 0, sizeof(struct PROCESS));
    process->pagedir = mmu_clone_pagedir();
    struct PROCESS_START_DATA* psd = (struct PROCESS_START_DATA*)kmalloc(sizeof(struct PROCESS_START_DATA));
    psd->process = process;
    psd->startfunc = startfunc;
    psd->ud = ud;
    struct TCB* tcb = (struct TCB*)kmalloc(sizeof(struct TCB));
    if (!tcb_init(tcb, process, process->pagedir, priority, SCHEDULE_QUANTUM_NORMAL, 4096, _process_startfunc, _process_exitfunc, psd)) {
        kfree(tcb);
        kfree(psd);
        kfree(process);
        return NULL;
    }
    process->pid = __sync_fetch_and_add(&_next_process_id, 1);
    process->main_thread = tcb;
    scheduler_run(tcb);
    return process;
}
