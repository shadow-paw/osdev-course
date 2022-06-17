#ifndef KERNEL_MMU_H_
#define KERNEL_MMU_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "multiboot.h"

// Kernel Zone (-256MB ~ TOP)
// -------------------------------------------------
#define MMU_KERNEL_BASE (0xF0000000)
#define MMU_KERNEL_PMA  (0x00100000)
#define MMU_KERNEL_VMA  ((uintptr_t)(MMU_KERNEL_BASE + MMU_KERNEL_PMA))
#define MMU_VMA2PMA(x)  (((uintptr_t)(x)) - MMU_KERNEL_BASE)
#define MMU_PMA2VMA(x)  (((uintptr_t)(x)) + MMU_KERNEL_BASE)

// CPU DEFINED
#define MMU_PROT_PRESENT  (0x0001)
#define MMU_PROT_RO       (0x0000)
#define MMU_PROT_RW       (0x0002)
#define MMU_PROT_USER     (0x0004)
#define MMU_PROT_MASK     (MMU_PROT_RO|MMU_PROT_RW|MMU_PROT_USER)
// PAGE TABLE FLAG
#define MMU_PAGE_ONDEMAND (0x0100)
#define MMU_PAGE_MAPPHY   (0x0200)

#define MMU_MUNMAP_RELEASE   (0)
#define MMU_MUNMAP_NORELEASE (1)

typedef uint32_t MMU_PHYADDR;

#ifdef __cplusplus
extern "C" {
#endif
bool mmu_init(const struct MULTIBOOT_BOOTINFO* multiboot);
bool mmu_mmap(const void* mem, MMU_PHYADDR paddr, size_t size, unsigned int flag);
bool mmu_munmap(const void* mem, size_t size, unsigned int flag);
#ifdef __cplusplus
};
#endif


#endif  // KERNEL_MMU_H_
