#include <stddef.h>
#include <stdint.h>
#include "kmalloc.h"
#include "kstring.h"
#include "ddk/ddk.h"
#include "ramdisk.h"

struct RAMDISK_DRIVER_DATA {
    uint8_t* memory;
    uint64_t size;
};

bool ramdisk_open(struct DISK_DRIVER* driver, struct DISK_DEVICE* device, const void* options) {
    if (!driver || !device || !options) return false;
    const struct RAMDISK_OPEN_OPTIONS* opt = (const struct RAMDISK_OPEN_OPTIONS*)options;
    struct RAMDISK_DRIVER_DATA* driver_data = kmalloc(sizeof(struct RAMDISK_DRIVER_DATA));
    driver_data->memory = (uint8_t*)opt->memory;
    driver_data->size = (uint64_t)opt->size;
    device->driver = driver;
    device->driver_data = driver_data;
    return true;
}
bool ramdisk_close(struct DISK_DRIVER* driver, struct DISK_DEVICE* device) {
    (void)driver;
    if (!device) return false;
    kfree(device->driver_data);
    device->driver = 0;
    device->driver_data = 0;
    return true;
}
uint64_t ramdisk_get_disksize(struct DISK_DRIVER* driver, struct DISK_DEVICE* device) {
    (void)driver;
    if (!device) return 0;
    const struct RAMDISK_DRIVER_DATA* driver_data = (const struct RAMDISK_DRIVER_DATA*)device->driver_data;
    return driver_data->size;
}

int64_t ramdisk_read(struct DISK_DRIVER* driver, struct DISK_DEVICE* device, void* buf, uint64_t offset, uint64_t size) {
    (void)driver;
    if (!device) return -1;
    const struct RAMDISK_DRIVER_DATA* driver_data = (const struct RAMDISK_DRIVER_DATA*)device->driver_data;
    if (offset >= driver_data->size) return 0;
    if (offset + size > driver_data->size) {
        size = driver_data->size - offset;
    }
    kmemcpy(buf, driver_data->memory + offset, size);
    return (int64_t)size;
}
int64_t ramdisk_write(struct DISK_DRIVER* driver, struct DISK_DEVICE* device, const void* buf, uint64_t offset, uint64_t size) {
    (void)driver;
    if (!device) return -1;
    const struct RAMDISK_DRIVER_DATA* driver_data = (const struct RAMDISK_DRIVER_DATA*)device->driver_data;
    if (offset >= driver_data->size) return 0;
    if (offset + size > driver_data->size) {
        size = driver_data->size - offset;
    }
    kmemcpy(driver_data->memory + offset, buf, size);
    return (int64_t)size;
}
bool driver_ramdisk(struct DISK_DRIVER* driver) {
    if (!driver) return false;
    driver->open = ramdisk_open;
    driver->close = ramdisk_close;
    driver->get_disksize = ramdisk_get_disksize;
    driver->read = ramdisk_read;
    driver->write = ramdisk_write;
    return true;
}
