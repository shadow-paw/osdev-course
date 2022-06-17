#include "inlineasm.h"
#include "multiboot.h"
#include "mmu.h"
#include "kmalloc.h"
#include "pic.h"
#include "pci.h"
#include "hal.h"
#include "kdebug.h"

extern "C" void kmain() {
    kernel::hal.probe();

    pic_init();
    _STI();
    pci_scanbus();

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
