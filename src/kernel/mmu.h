#ifndef KERNEL_MMU_H_
#define KERNEL_MMU_H_

#include <stddef.h>
#include <stdint.h>

// Kernel Zone (-256MB ~ TOP)
// -------------------------------------------------
#define MMU_KERNEL_BASE (0xF0000000)
#define MMU_KERNEL_PMA  (0x00100000)
#define MMU_KERNEL_VMA  ((uintptr_t)(MMU_KERNEL_BASE + MMU_KERNEL_PMA))
#define MMU_VMA2PMA(x)  (((uintptr_t)(x)) - MMU_KERNEL_BASE)
#define MMU_PMA2VMA(x)  (((uintptr_t)(x)) + MMU_KERNEL_BASE)

#endif  // KERNEL_MMU_H_
