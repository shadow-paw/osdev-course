#include "kaddr.h"
#include "kmalloc.h"
#include "mmu.h"
#include "heap.h"
#include "scheduler.h"
#include "ld.h"
#include "process.h"

// ring3.s
extern "C" void enter_ring3(uint32_t ring3_eip, uint32_t ring3_esp);

uint32_t _next_process_id = 1;

struct PROCESS_START_DATA {
    struct PROCESS* process;
    const char* program;
};

void _process_startfunc(void* ud) {
    static const size_t app_stack_size = 8192;
    struct PROCESS_START_DATA* psd = (struct PROCESS_START_DATA*)ud;
    // create app stack and heap
    uint8_t* app_stack = (uint8_t*)KADDR_APP_STACK - app_stack_size;
    heap_create(&psd->process->heap, (void*)KADDR_APP_HEAP, 1024*1024*32, MMU_PROT_RW|MMU_PROT_USER);
    mmu_mmap(app_stack, 0, app_stack_size, MMU_PROT_RW|MMU_PROT_USER);
    kernel::ProgramLoader ld;
    ld.load_program(psd->program);
    // PROCESS_STARTFUNC f = (PROCESS_STARTFUNC);
    // f();
    enter_ring3(ld.get_entrypoint(), KADDR_APP_STACK);
}
void _process_exitfunc(void* ud) {
    struct PROCESS_START_DATA* psd = (struct PROCESS_START_DATA*)ud;
    kfree(psd->process);
    kfree(psd);
    scheduler_exitthread();
    // TODO: release process->pagedir and walk the page tree in zombie cleaner thread
}
extern "C" struct PROCESS* process_current() {
    struct TCB* tcb = scheduler_current();
    return tcb->process;
}
extern "C" struct PROCESS* process_create(const char* program, uint32_t priority) {
    struct PROCESS* process = (struct PROCESS*)kmalloc(sizeof(struct PROCESS));
    __builtin_memset(process, 0, sizeof(struct PROCESS));
    process->pagedir = mmu_clone_pagedir();
    struct PROCESS_START_DATA* psd = (struct PROCESS_START_DATA*)kmalloc(sizeof(struct PROCESS_START_DATA));
    psd->process = process;
    psd->program = program;
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
