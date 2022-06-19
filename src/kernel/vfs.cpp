#include "kstring.h"
#include "vfs.h"
#include "kdebug.h"

namespace kernel {

VFS vfs;

VFS::VFS() {
    _entries_count = 0;
}
bool VFS::mount(const char* mount_point, struct FS_DEVICE* fs) {
    // TODO: ensure path ends with /
    int maxEntry = sizeof(_entries) / sizeof(_entries[0]);
    if (_entries_count >= maxEntry) return false;
    VFS::Entry* entry = &_entries[_entries_count];
    _entries_count++;
    entry->path_len = kstrncpy(entry->path, mount_point, sizeof(entry->path));
    entry->fs = fs;
    return true;
}
VFS::Entry* VFS::find(const char* filename) {
    for (int i=0; i<_entries_count; i++) {
        VFS::Entry& entry = _entries[i];
        if (kstrncmp(entry.path, filename, entry.path_len) == 0) {
            return &entry;
        }
    }
    return nullptr;
}

bool VFS::fstat(const char* filename, struct FS_FSTAT* stat) {
    VFS::Entry* entry = find(filename);
    if (!entry) return false;
    const char* subpath = filename + entry->path_len;
    return entry->fs->driver->fstat(entry->fs->driver, entry->fs, subpath, stat);
}
int64_t VFS::read(const char* filename, void* buf, uint64_t offset, uint64_t size) {
    VFS::Entry* entry = find(filename);
    if (!entry) return false;
    const char* subpath = filename + entry->path_len;
    return entry->fs->driver->read(entry->fs->driver, entry->fs, subpath, buf, offset, size);
}
int64_t VFS::write(const char* filename, const void* buf, uint64_t offset, uint64_t size) {
    VFS::Entry* entry = find(filename);
    if (!entry) return false;
    const char* subpath = filename + entry->path_len;
    return entry->fs->driver->write(entry->fs->driver, entry->fs, subpath, buf, offset, size);
}

}  // namespace
