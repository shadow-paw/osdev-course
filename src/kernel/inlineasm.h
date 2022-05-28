#ifndef KERNEL_INLINEASM_H__
#define KERNEL_INLINEASM_H__

#include <stdint.h>
#include <stddef.h>

// -------------------------------------------------
#define _INT_DISABLE()  __asm volatile("pushf\ncli")
#define _INT_RESTORE()  __asm volatile("popf")
static inline void _CLI() { __asm volatile("cli"); }
static inline void _STI() { __asm volatile("sti"); }
static inline void _HLT() { __asm volatile("hlt"); }

// -------------------------------------------------
// CPU Model Specific Register
// -------------------------------------------------
static inline void _WRMSR(uint32_t msr, uint64_t value) {
    __asm__ volatile ("wrmsr"
                      :
                      : "c" (msr), "A" (value)
                      );
}
static inline uint64_t _RDMSR(uint32_t msr) {
    uint64_t value;
    __asm__ volatile ("rdmsr"
                      : "=A" (value)
                      : "c" (msr)
                      );
    return value;
}

// -------------------------------------------------
// Paging
// -------------------------------------------------
static inline void _MOVCR3(uint32_t physcal_addr) {
    __asm volatile("mov cr3, %0" : : "r"(physcal_addr) : "memory");
}
static inline void _INVLPG(const void* addr) {
    __asm volatile("invlpg [%0]" : : "r"(addr) : "memory");
}

// -------------------------------------------------
// SPINLOCK
// -------------------------------------------------
typedef volatile uint32_t _SPINLOCK __attribute__ ((aligned(16)));
static inline void _SPIN_LOCK(_SPINLOCK* lock) {
    __asm (
           "lock bts %0, 0\n"
           "jnc 1f\n"
           "0:\n"
           "pause\n"
           "test %0, 1\n"
           "je 0b\n"
           "lock bts %0, 0\n"
           "jc 0b\n"
           "1:\n"
           :
           : "m"(lock)
           :
           );
}
static inline void _SPIN_UNLOCK(_SPINLOCK* lock) {
    *lock = 0;
}

#endif  // KERNEL_INLINEASM_H__