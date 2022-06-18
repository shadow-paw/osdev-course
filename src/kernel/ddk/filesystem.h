#ifndef KERNEL_DDK_FILESYSTEM_H_
#define KERNEL_DDK_FILESYSTEM_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "disk.h"
#ifdef __cplusplus
extern "C" {
#endif

struct FS_DRIVER;
struct FS_DEVICE {
    struct FS_DRIVER* driver;
    void* driver_data;
    struct DISK_DEVICE* disk;
};

struct FS_DIRENT {
    const char name[256];
};
struct FS_FSTAT {
    uint64_t filelen;
};

struct FS_DRIVER {
    bool (*open)(struct FS_DRIVER* driver, struct FS_DEVICE* device, struct DISK_DEVICE* disk);
    bool (*close)(struct FS_DRIVER* driver, struct FS_DEVICE* device);
    // directory
    bool (*dir_entry)(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* path, unsigned int index, struct FS_DIRENT* entry);
    bool (*fstat)(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* filename, struct FS_FSTAT* stat);
    int64_t (*read)(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* filename, void* buf, uint64_t offset, uint64_t size);
    int64_t (*write)(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* filename, const void* buf, uint64_t offset, uint64_t size);
};

#ifdef __cplusplus
}
#endif

#endif  // KERNEL_DDK_FILESYSTEM_H_
