#ifndef KERNEL_VFS_H_
#define KERNEL_VFS_H_

#include "ddk/filesystem.h"

namespace kernel {

class VFS {
  public:
    VFS();
    bool mount(const char* mount_point, struct FS_DEVICE* fs);
    bool fstat(const char* filename, struct FS_FSTAT* stat);
    int64_t read(const char* filename, void* buf, uint64_t offset, uint64_t size);
    int64_t write(const char* filename, const void* buf, uint64_t offset, uint64_t size);

  private:
    struct Entry {
        char path[256];
        size_t path_len;
        struct FS_DEVICE* fs;
    };
    Entry _entries[64];
    int   _entries_count;
    Entry* find(const char* filename);
};

extern VFS vfs;

}  // nsmespace

#endif  // KERNEL_VFS_H_
