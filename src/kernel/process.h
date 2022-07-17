#ifndef KERNEL_PROCESS_H_
#define KERNEL_PROCESS_H_

#include <stdbool.h>
#include <stdint.h>
#include "mmu.h"
#include "tcb.h"
#include "heap.h"

#ifdef __cplusplus
extern "C" {
#endif

struct PROCESS {
    uint32_t pid;
    MMU_PHYADDR pagedir;
    struct HEAP heap;
    struct TCB* main_thread;
};

typedef void (*PROCESS_STARTFUNC)();
struct PROCESS* process_create(PROCESS_STARTFUNC startfunc, void* ud, uint32_t priority);
struct PROCESS* process_current();

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // KERNEL_PROCESS_H_
