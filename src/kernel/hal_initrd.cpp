#include <stdbool.h>
#include "mmu.h"
#include "vma_alloc.h"
#include "drivers/ramdisk/ramdisk.h"
#include "drivers/tarfs/tarfs.h"
#include "hal.h"
#include "kdebug.h"

namespace kernel {

bool HAL::probe_initrd(MMU_PHYADDR phyaddr, size_t size, const char* mount_point) {
    kdebug("Probe initrd at %p, size: %x\n", phyaddr, size);
    if (!driver_ramdisk(&_drivers.initrd)) return false;
    char *memory = (char*)vma_alloc(size);
    mmu_mmap(memory, phyaddr, size, MMU_PAGE_NOALLOC);
    struct RAMDISK_OPEN_OPTIONS opt = { memory, size };
    if (!_drivers.initrd.open(&_drivers.initrd, &_devices.initrd, &opt)) return false;
    // assume tarfs
    if (!driver_tarfs(&_drivers.tarfs)) return false;
    if (!_drivers.tarfs.open(&_drivers.tarfs, &_devices.initrd_fs, &_devices.initrd)) return false;
    // mount it
    vfs.mount(mount_point, &_devices.initrd_fs);
    return true;
}

}  // namespace
