#include "kstring.h"
#include "inlineasm.h"
#include "multiboot.h"
#include "mmu.h"
#include "kmalloc.h"
#include "hal.h"
#include "scheduler.h"
#include "kdebug.h"

extern "C" void zombie_cleaner(void*) {
    for (;;) {
        scheduler_cleanzombie();
        scheduler_sleep(10);
    }
}
extern "C" void thread_func1(void*) {
    for (int i=0; i<10; i++) {
        kdebug("thread %d\n", scheduler_current()->thread_id);
        scheduler_sleep(10);
    }
}
extern "C" void thread_func2(void*) {
    for (;;) {
        kdebug("thread %d\n", scheduler_current()->thread_id);
        scheduler_sleep(3);
    }
}
void thread_exitfunc(void*) {
    scheduler_exitthread();
}

extern "C" void kmain(const struct MULTIBOOT_BOOTINFO* multiboot) {
    scheduler_init();
    kernel::hal.probe();
    kernel::hal.display_setmode(2);
    kernel::hal.display_clearscreen();

    struct TCB* tcb0 = (struct TCB*)kmalloc(sizeof(struct TCB));
    struct TCB* tcb1 = (struct TCB*)kmalloc(sizeof(struct TCB));
    struct TCB* tcb2 = (struct TCB*)kmalloc(sizeof(struct TCB));
    tcb_init(tcb0, SCHEDULE_PRIORITY_IDLE, SCHEDULE_QUANTUM_LONG, 4096, zombie_cleaner, NULL, NULL);
    tcb_init(tcb1, SCHEDULE_PRIORITY_NORMAL, SCHEDULE_QUANTUM_SHORT, 4096, thread_func1, thread_exitfunc, NULL);
    tcb_init(tcb2, SCHEDULE_PRIORITY_NORMAL, SCHEDULE_QUANTUM_SHORT, 4096, thread_func2, thread_exitfunc, NULL);
    scheduler_run(tcb0);
    scheduler_run(tcb1);
    scheduler_run(tcb2);

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
