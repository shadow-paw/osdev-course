#include "kstring.h"
#include "inlineasm.h"
#include "multiboot.h"
#include "mmu.h"
#include "kmalloc.h"
#include "hal.h"
#include "scheduler.h"
#include "process.h"
#include "kdebug.h"

extern "C" void process_startfunc(struct PROCESS *process, void*) {
    uint32_t cr3;
    for (;;) {
        __asm volatile("mov %0, cr3" : "=A"(cr3));
        kdebug("process %d, cr3: %X\n", process->pid, cr3);
        scheduler_sleep(10);
    }
}
extern "C" void zombie_cleaner(void*) {
    for (;;) {
        scheduler_cleanzombie();
        scheduler_sleep(10);
    }
}
extern "C" void kmain(const struct MULTIBOOT_BOOTINFO* multiboot) {
    scheduler_init();
    kernel::hal.probe();
    kernel::hal.display_setmode(2);
    kernel::hal.display_clearscreen();

    struct TCB* tcb_zombie_cleaner = (struct TCB*)kmalloc(sizeof(struct TCB));
    tcb_init(tcb_zombie_cleaner, NULL, 0, SCHEDULE_PRIORITY_IDLE, SCHEDULE_QUANTUM_LONG, 4096, zombie_cleaner, NULL, NULL);
    scheduler_run(tcb_zombie_cleaner);

    process_create(process_startfunc, NULL, SCHEDULE_PRIORITY_NORMAL);
    process_create(process_startfunc, NULL, SCHEDULE_PRIORITY_NORMAL);

    // Mount initrd
    const struct MULTIBOOT_BOOTINFO_MODULES* mods = (const struct MULTIBOOT_BOOTINFO_MODULES*)(mmu_pma2vma(multiboot->mods_addr));
    for (uint32_t i=0; i<multiboot->mods_count; i++) {
        const char* name = (const char*)mmu_pma2vma(mods[i].name);
        if (kstrcmp(name, "initrd") == 0) {
            size_t size = (size_t)(mods[i].end - mods[i].start);
            kernel::hal.probe_initrd(mods[i].start, size, "/initrd/");
        }
    }

    // // test code
    // struct FS_FSTAT stat;
    // if (kernel::vfs.fstat("/initrd/test.txt", &stat)) {
    //     kdebug("file length: %ld\n", stat.filelen);
    //     char buf[512] = {0};
    //     if (kernel::vfs.read("/initrd/test.txt", buf, 0, stat.filelen) > 0) {
    //         buf[stat.filelen] = 0;
    //         kdebug("file content: %s\n", buf);
    //     }
    // }

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
