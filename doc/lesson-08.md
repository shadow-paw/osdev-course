# 8. Virtual File System

## Objective
1. Provide uniform interface to access storage from different disk and with different file systems.

## Functions
- mount
- dir: list
- file: fstat
- file: read
- file: write

## Basic Design

### Disk
The storage. You will have a storage driver (e.g. ramdisk driver) and a storage device (e.g. piece of memory). Think
about block device on linux. The disk is only continuous block of "sectors".

### Filesystem
The file system put the concept of file and directory to the disk. In the example we will implement tarfs, which
allow accessing individual file stored inside a tar file. The tar file will be loaded by grub and visible to the kernel
in a memory address.

It is also possible to have filesystem not backed by physical disk, e.g. `sysfs` (`/dev` and `/proc`).

### Operations
Each mount point consist of a prefix path and respective file system. When perform action over a full pathname,
the vfs system search the mount table for matched record and invoke function on the mapped file system.

In the example we mount `/initrd/` into the ramdisk, which has `tarfs`, once we setup the vfs, we can then just
`read("/initrd/test.txt")` in `main.cpp` without care where it is and what filesystem.

## Cache
1. Lookup Cache. Interestingly, beside a normal cache, a negative-cache is also important. A negative cache is about caching negative result, e.g. a filename is not found.
2. Work with `mmap`, similar to "swap file".

## What Is Missing
1. Filesystem detection: We hard-coded `tarfs` when loading initrd.
2. Driver lookup, we use `_drivers.ramdisk` directly, in reality you may want `lookup_driver(type, name)`.
3. Directory function is not implemented in the example.

## Further Readings
1. https://wiki.osdev.org/File_Systems
2. https://wiki.osdev.org/VFS
3. https://en.wikipedia.org/wiki/Negative_cache
4. https://wiki.osdev.org/USTAR
