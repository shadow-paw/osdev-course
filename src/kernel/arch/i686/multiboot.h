#ifndef KERNEL_ARCH_I686_MULTIBOOT_H_
#define KERNEL_ARCH_I686_MULTIBOOT_H_

#include <stdint.h>

// https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

#define MULTIBOOT_BOOTINFO_FLAGS_MEM            (1<<0)
#define MULTIBOOT_BOOTINFO_FLAGS_BOOTDEVICE     (1<<1)
#define MULTIBOOT_BOOTINFO_FLAGS_CMDLINE        (1<<2)
#define MULTIBOOT_BOOTINFO_FLAGS_MODES          (1<<3)
#define MULTIBOOT_BOOTINFO_FLAGS_SYM_AOUT       (1<<4)
#define MULTIBOOT_BOOTINFO_FLAGS_SYM_ELF        (1<<5)
#define MULTIBOOT_BOOTINFO_FLAGS_MMAP           (1<<6)
#define MULTIBOOT_BOOTINFO_FLAGS_DRIVES         (1<<7)
#define MULTIBOOT_BOOTINFO_FLAGS_CONFIG         (1<<8)
#define MULTIBOOT_BOOTINFO_FLAGS_BOOTLOADERNAME (1<<9)
#define MULTIBOOT_BOOTINFO_FLAGS_APM            (1<<10)

struct MULTIBOOT_BOOTINFO {
    uint32_t flags;
    uint32_t mem_lower, mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count, mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length, mmap_addr;
    uint32_t drives_length, drive_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    // VBE info skipped
};

#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5

struct MULTIBOOT_BOOTINFO_MMAP {
  uint32_t size;
  uint64_t addr;
  uint64_t len;
  uint32_t type;
};

struct MULTIBOOT_BOOTINFO_MODULES {
  uint32_t start;
  uint32_t end;
  uint32_t name;
  uint32_t reserved;
};

#endif  // KERNEL_ARCH_I686_MULTIBOOT_H_
