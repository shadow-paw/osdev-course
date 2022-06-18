#include "multiboot.h"
#include "mmu.h"
#include "hal.h"
#include "kstring.h"
#include "kdebug.h"

extern "C" void kmain(const struct MULTIBOOT_BOOTINFO* multiboot) {
    kernel::hal.probe();

    kernel::hal.display_setmode(2);
    kernel::hal.display_clearscreen();

    kdebug("multiboot->mods_count: %d\n", multiboot->mods_count);

    const struct MULTIBOOT_BOOTINFO_MODULES* mods = (const struct MULTIBOOT_BOOTINFO_MODULES*)(mmu_pma2vma(multiboot->mods_addr));
    for (uint32_t i=0; i<multiboot->mods_count; i++) {
        const char* name = (const char*)mmu_pma2vma(mods[i].name);
        if (kstrcmp(name, "initrd") == 0) {
            size_t size = (size_t)(mods[i].end - mods[i].start);
            kernel::hal.probe_initrd(mods[i].start, size);
        }
    }

    for (;;) {
        char name[1024];
        kernel::hal.uart_puts(0, "What is your name? ");
        size_t len = kernel::hal.uart_gets(0, name, sizeof(name), true);
        if (len > 0) {
            kernel::hal.uart_puts(0, "\r\nHello ");
            kernel::hal.uart_puts(0, name);
            kernel::hal.uart_puts(0, "\r\n\r\n");
        } else {
            kernel::hal.uart_puts(0, "\r\n");
        }
    }
}
