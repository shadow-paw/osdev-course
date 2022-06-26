#ifndef KERNEL_ARCH_I686_TCB_H_
#define KERNEL_ARCH_I686_TCB_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct __attribute__ ((packed)) TCB {
    struct {
        uint32_t eax, ebx, ecx, edx;
        uint32_t ebp, esp, esi, edi;
        uint32_t eflags;
        uint32_t esp0;
    } __attribute__ ((packed)) cpu;
    void* kstack;
    // Scheduling
    uint32_t elapsed;
    uint32_t sleep_tick;
    uint32_t quantum;
    uint32_t priority;
    // Information
    uint32_t thread_id;
    uint32_t flags;
    // Chain
    struct TCB* next;
    struct TCB* prev;
} ;

typedef void (*TCB_FUNC)(void* ud);
bool tcb_init(struct TCB* tcb, uint32_t priority, uint32_t quantum, size_t kstack_size, TCB_FUNC startfunc, TCB_FUNC exitfunc, void* ud);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // KERNEL_ARCH_I686_TCB_H_
