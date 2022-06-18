#include <stddef.h>
#include <stdint.h>
#include "ddk/ddk.h"
#include "kstring.h"
#include "tarfs.h"
#include "kdebug.h"

struct TAR_HEADER {
    char filename[100];
    char mode[8];   // ascii, e.g. 0000775\0
    char uid[8];
    char gid[8];
    char filelen[12];  // oct-based, e.g. 0000 0000 010\0 => 8 bytes
    char mtime[12];
    char header_checksum[8];
    char link_indicator;
    char link_filename[100];
    char pad[254];
};
uint64_t oct2uint64(const char* s) {
    uint64_t r = 0;
    for (; *s; s++) {
        int n = *s - '0';
        r = r * 8 + n;
    }
    return r;
}
bool tarfs_open(struct FS_DRIVER* driver, struct FS_DEVICE* device, struct DISK_DEVICE* disk) {
    if (!driver || !device) return false;
    device->driver = driver;
    device->driver_data = 0;
    device->disk = disk;
    return true;
}
bool tarfs_close(struct FS_DRIVER* driver, struct FS_DEVICE* device) {
    (void)driver;
    if (!device) return false;
    device->driver = 0;
    device->driver_data = 0;
    device->disk = 0;
    return true;
}
bool tarfs_dir_entry(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* path, unsigned int index, struct FS_DIRENT* entry) {
    (void)driver;
    (void)device;
    (void)path;
    (void)index;
    (void)entry;
    return false;
}
bool tarfs_fstat(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* filename, struct FS_FSTAT* stat) {
    (void)driver;
    if (!device || !filename || !stat) return false;
    uint64_t disksize = device->disk->driver->get_disksize(device->disk->driver, device->disk);
    for (uint64_t cursor=0; cursor<disksize; cursor+=512) {
        struct TAR_HEADER header;
        if (!device->disk->driver->read(device->disk->driver, device->disk, &header, cursor, sizeof(header))) return false;
        uint64_t filelen = oct2uint64(header.filelen);
        if (kstrcmp(header.filename, filename) != 0) {
            cursor += ((filelen + 511) >> 9) << 9; // increase by filelen, pad to nearest 512 bytes block
            continue;
        }
        // found
        stat->filelen = filelen;
        return true;
    }
    return false;
}
int64_t tarfs_read(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* filename, void* buf, uint64_t offset, uint64_t size) {
    (void)driver;
    if (!device || !filename || !buf) return false;
    uint64_t disksize = device->disk->driver->get_disksize(device->disk->driver, device->disk);
    for (uint64_t cursor=0; cursor<disksize; cursor+=512) {
        struct TAR_HEADER header;
        if (!device->disk->driver->read(device->disk->driver, device->disk, &header, cursor, sizeof(header))) return false;
        uint64_t filelen = oct2uint64(header.filelen);
        if (kstrcmp(header.filename, filename) != 0) {
            cursor += ((filelen + 511) >> 9) << 9; // increase by filelen, pad to nearest 512 bytes block
            continue;
        }
        // found
        if (offset >= filelen) return 0;
        if (offset + size > filelen) {
            size = filelen - offset;
        }
        cursor += 512;
        return device->disk->driver->read(device->disk->driver, device->disk, buf, cursor + offset, size);
    }
    return 0;
}
int64_t tarfs_write(struct FS_DRIVER* driver, struct FS_DEVICE* device, const char* filename, const void* buf, uint64_t offset, uint64_t size) {
    (void)driver;
    (void)device;
    (void)filename;
    (void)buf;
    (void)offset;
    (void)size;
    return -1;  // Not supported
}
bool driver_tarfs(struct FS_DRIVER* driver) {
    if (!driver) return false;
    driver->open = tarfs_open;
    driver->close = tarfs_close;
    driver->dir_entry = tarfs_dir_entry;
    driver->fstat = tarfs_fstat;
    driver->read = tarfs_read;
    driver->write = tarfs_write;
    return true;
}
