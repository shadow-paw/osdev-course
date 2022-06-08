#include "inlineasm.h"
#include "multiboot.h"
#include "mmu.h"
#include "kmalloc.h"
#include "serial.h"
#include "pic.h"
#include "kdebug.h"

void _timer(uint64_t tsc) {
    kdebug("timer: %lx\n", tsc);
}
void kmain(const struct MULTIBOOT_BOOTINFO* multiboot) {
    char name[1024];
    serial_init(0);

    kdebug("multiboot mmap: %p\n", MMU_PMA2VMA(multiboot->mmap_addr));
    const struct MULTIBOOT_BOOTINFO_MMAP* map =
        (const struct MULTIBOOT_BOOTINFO_MMAP*)MMU_PMA2VMA(multiboot->mmap_addr);
    const uint32_t map_count = multiboot->mmap_length / sizeof(struct MULTIBOOT_BOOTINFO_MMAP);
    for (uint32_t i = 0; i < map_count; i++) {
        kdebug("mmap: %lx, size: %lx = %X\n", map[i].addr, map[i].len, map[i].type);
    }

    mmu_init(map, map_count);
    char* p = (char*)kmalloc(16);
    p[0] = 'A';
    p[1] = 0;
    kdebug("p: %p, %s\n", p, p);

    pic_init();
    _STI();

    for (;;) {
        serial_puts(0, "What is your name? ");
        size_t len = serial_gets(0, name, true);
        if (len > 0) {
            serial_puts(0, "\r\nHello ");
            serial_puts(0, name);
            serial_puts(0, "\r\n\r\n");
        } else {
            serial_puts(0, "\r\n");
        }
    }
}
