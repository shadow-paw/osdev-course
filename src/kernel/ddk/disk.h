#ifndef KERNEL_DDK_DISK_H_
#define KERNEL_DDK_DISK_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct DISK_DRIVER;
struct DISK_DEVICE {
    struct DISK_DRIVER* driver;
    void* driver_data;
};

struct DISK_DRIVER {
    bool (*open)(struct DISK_DRIVER* driver, struct DISK_DEVICE* device, const void* options);
    bool (*close)(struct DISK_DRIVER* driver, struct DISK_DEVICE* device);
    uint64_t (*get_disksize)(struct DISK_DRIVER* driver, struct DISK_DEVICE* device);
    int64_t (*read)(struct DISK_DRIVER* driver, struct DISK_DEVICE* device, void* buf, uint64_t offset, uint64_t size);
    int64_t (*write)(struct DISK_DRIVER* driver, struct DISK_DEVICE* device, const void* buf, uint64_t offset, uint64_t size);
};

#ifdef __cplusplus
}
#endif

#endif  // KERNEL_DDK_DISK_H_
