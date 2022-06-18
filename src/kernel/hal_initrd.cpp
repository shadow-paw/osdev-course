#include <stdbool.h>
#include "mmu.h"
#include "vma_alloc.h"
#include "hal.h"
#include "kdebug.h"

namespace kernel {

bool HAL::probe_initrd(MMU_PHYADDR phyaddr, size_t size) {
    kdebug("Probe initrd at %p, size: %x\n", phyaddr, size);
    char *memory = (char*)vma_alloc(size);
    mmu_mmap(memory, phyaddr, size, MMU_PAGE_NOALLOC);
    return true;
}

}  // namespace
