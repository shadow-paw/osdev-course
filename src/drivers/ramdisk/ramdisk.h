#ifndef DRIVERS_RANDISK_RAMDISK_H_
#define DRIVERS_RANDISK_RAMDISK_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk/disk.h"

#ifdef __cplusplus
extern "C" {
#endif
struct RAMDISK_OPEN_OPTIONS {
    void* memory;
    size_t size;
};
bool driver_ramdisk(struct DISK_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // DRIVERS_RANDISK_RAMDISK_H_
