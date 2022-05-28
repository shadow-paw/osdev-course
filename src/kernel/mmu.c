#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "inlineasm.h"
#include "multiboot.h"
#include "mmu.h"
#include "kdebug.h"

// From bootstrap.s
extern uint32_t MMU_PD[];
extern char _kernel_start, _kernel_end;

// MMU recursive mapping (-4MB ~ TOP)
#define MMU_RECURSIVE_SLOT (1023UL)
// MMU Page Allocator (-8MB ~ -4MB), requires 4 MiB to story frames for 4GB ram
#define MMU_FRAMEPOOL_VMA  (0xFF800000)
// Address Conversion Helpers
#define MMU_PT_VMA         (MMU_RECURSIVE_SLOT << 22)
#define MMU_PD_VMA         (MMU_PT_VMA + (MMU_RECURSIVE_SLOT << 12))
#define MMU_PD_INDEX(addr) ((((uintptr_t)(addr))>>22) & 1023)
#define MMU_PT_INDEX(addr) ((((uintptr_t)(addr))>>12) & 1023)
#define MMU_PD_PTR(addr)   ((uint32_t*)(MMU_PD_VMA))
#define MMU_PT_PTR(addr)   ((uint32_t*)((uintptr_t)MMU_PT_VMA + ((((uintptr_t)(addr))>>10)&0x3FF000)))

// Some Global Data
// -----------------------------------------------------------------
uint32_t MMU_FRAMEPOOL_PT[1024] __attribute__((aligned(4096)));
uint32_t MMU_FRAMEPOOL_FIRSTPAGE[1024] __attribute__((aligned(4096)));
volatile MMU_PHYADDR* MMU_frames = (MMU_PHYADDR*)MMU_FRAMEPOOL_VMA;
volatile unsigned int MMU_frames_index = 0;  // index for next allocation
_SPINLOCK MMU_lock = 0;

bool mmu_mark(const void* addr, MMU_PHYADDR paddr, uint32_t flag) {
    uint32_t pd_index = MMU_PD_INDEX(addr);
    uint32_t pt_index = MMU_PT_INDEX(addr);
    uint32_t* pd = MMU_PD_PTR(addr);
    uint32_t* pt = MMU_PT_PTR(addr);
    if ((pd[pd_index] & (MMU_PROT_PRESENT|MMU_PAGE_ONDEMAND)) == 0) {
        pd[pd_index] = MMU_PAGE_ONDEMAND|MMU_PROT_RW|MMU_PROT_USER;
    }
    if ((pt[pt_index] & MMU_PROT_PRESENT) == 0) {
        if ((flag & MMU_PAGE_MAPPHY) == 0) {
            pt[pt_index] = (uint32_t)(MMU_PAGE_ONDEMAND | (flag & MMU_PROT_MASK));
        } else {
            pt[pt_index] = (uint32_t)(paddr | (flag & MMU_PROT_MASK) | MMU_PROT_PRESENT);
            _INVLPG(addr);
        }
    } else {
        if ((flag & MMU_PAGE_MAPPHY) == 0) {
            pt[pt_index] = (pt[pt_index] & (~(uint32_t)MMU_PROT_MASK)) | ((uint32_t)flag & MMU_PROT_MASK);
        } else {
            kdebug("MMU : map fail, addr:%p paddr:%X flag=%d entry:%X\n", addr, paddr, flag, pt[pt_index]);
            return false;
        }
    } return true;
}
bool mmu_init(const struct MULTIBOOT_BOOTINFO_MMAP* map, int map_count) {
    const uint64_t kend = (MMU_VMA2PMA((uintptr_t)&_kernel_end)) + 4096;
    uint64_t start, end;
    uint32_t pt_index = 0;
    // Install recursive page directory
    MMU_PD[MMU_PD_INDEX(MMU_PD_VMA)] = MMU_VMA2PMA(MMU_PD) +3;
    // entry for MMU_PHYPOOL
    MMU_PD[MMU_PD_INDEX(MMU_FRAMEPOOL_VMA)] = MMU_VMA2PMA(MMU_FRAMEPOOL_PT) +3;
    MMU_FRAMEPOOL_PT[0] = MMU_VMA2PMA(MMU_FRAMEPOOL_FIRSTPAGE) +3;
    _MOVCR3(MMU_VMA2PMA(MMU_PD));
    // Build stack of available physical page
    MMU_frames_index = 0;
    for (int i = 0; i < map_count; i++) {
        if (map[i].type != 1) continue;
        start = ((map[i].addr + 4095) >>12) <<12;            // align to 4K
        end = ((map[i].addr + map[i].len) >>12) <<12;
        for (; start < end; start += 4096) {
            // skip memory below kernel, those already mapped into kernel space
            if (start < kend) continue;
            // skip memory > 4GiB
            if (start >= 0x100000000) continue;
            // upon setting MMU_frames[512, 1024, ...] a page fault occur and demand allocated
            if (pt_index != (MMU_frames_index >> 9)) {
                pt_index = (MMU_frames_index >> 9);
                mmu_mark((const void*)&MMU_frames[MMU_frames_index], 0, MMU_PAGE_ONDEMAND);
                // Trigger #PF and we will pop a page there, which would decrease MMU_frames_index.
                // We want cpu to retry on the read op here, so the actual write op can use correct index.
                volatile uint32_t* ptr = &MMU_frames[MMU_frames_index];
                (void)*ptr;
            }
            MMU_frames[MMU_frames_index] = (uint32_t)start;
            MMU_frames_index++;
        }
    }
    return true;
}
MMU_PHYADDR mmu_alloc(void) {
    MMU_PHYADDR addr = 0;
    _INT_DISABLE();
    _SPIN_LOCK(&MMU_lock);
    if (MMU_frames_index > 0) {
        MMU_frames_index --;
        addr = MMU_frames[MMU_frames_index];
    }
    _SPIN_UNLOCK(&MMU_lock);
    _INT_RESTORE();
    return addr;
}
void mmu_free(MMU_PHYADDR addr) {
    _INT_DISABLE();
    _SPIN_LOCK(&MMU_lock);
    MMU_frames[MMU_frames_index] = addr;
    MMU_frames_index++;
    _SPIN_UNLOCK(&MMU_lock);
    _INT_RESTORE();
}
bool mmu_mmap(const void* mem, MMU_PHYADDR paddr, size_t size, unsigned int flag) {
    for (size_t off = 0; off < size; off += 4096) {
        if (!mmu_mark((const uint8_t*)mem + off, paddr+off, flag)) return false;
    }
    return true;
}
bool mmu_munmap(const void* mem, size_t size, unsigned int flag) {
    for (size_t off = 0; off < size; off += 4096) {
        const void* addr = (const uint8_t*)mem + off;
        uint32_t* pt = MMU_PT_PTR(addr);
        uint32_t entry = pt[MMU_PT_INDEX(addr)];
        pt[MMU_PT_INDEX(addr)] = 0;
        if ((entry & MMU_PROT_PRESENT) != 0) {
            if ((entry & MMU_PROT_PRESENT) != 0 && (flag & MMU_MUNMAP_NORELEASE) == 0) {
                entry &= 0xFFFFF000;
                if (entry) {
                    mmu_free(entry);
                    _INVLPG(addr);
                }
            }
        }
    }
    return true;
}
void INT_0E(uint32_t code, uint32_t addr, uint32_t ip) {
    uint32_t page, prot, pd_index;
    uint32_t* pt;
    kdebug("INT0E : #PF Page Fault Exception. IP:%X CODE:%d ADDR:%X\n"
           "      : PD[%d] PT[%d]\n", ip, code, addr, MMU_PD_INDEX(addr), MMU_PT_INDEX(addr));
    pt = MMU_PT_PTR(addr);
    if ((code & 1) == 0) {  // Page Not Present
        if ((pt[MMU_PT_INDEX(addr)] & MMU_PAGE_ONDEMAND) == 0) {
            kdebug("  #PF : Access to unallocated memory.\n");
            kdebug("      : ADDR: %X PT[%d]: %X\n", addr, MMU_PT_INDEX(addr), pt[MMU_PT_INDEX(addr)]);
            __asm volatile("cli; hlt");
        }
        page = mmu_alloc();
        // kdebug ("PTE: %X, %d, MMU_frames_index=%d, page=%X\n", pt[MMU_PT_INDEX(addr)], (addr>>12)&511, MMU_frames_index, page);
        prot = pt[MMU_PT_INDEX(addr)] & MMU_PROT_MASK;
        pt[MMU_PT_INDEX(addr)] = page | prot | MMU_PROT_PRESENT;
        _INVLPG((const void*)addr);
        // https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
        __builtin_memset((void*)((addr >> 12) << 12), 0, 4096);
        if ((pd_index=MMU_PD_INDEX(addr)) >= 512) {
            uint32_t* pd = MMU_PD_PTR(addr);
            MMU_PD[pd_index] = pd[pd_index];
        }
    } else {
        kdebug("  #PF : Access to protected memory.\n");
        kdebug("      : ADDR: %X PTE[%d]: %X\n", addr, MMU_PT_INDEX(addr), pt[MMU_PT_INDEX(addr)]);
        __asm volatile("cli; hlt");
    }
    // kdebug("INT0E : EXIT. CODE:%d ADDR:%X\n", code, addr);
}
