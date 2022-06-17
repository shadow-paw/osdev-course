#include "inlineasm.h"
#include "multiboot.h"
#include "mmu.h"
#include "kmalloc.h"
#include "serial.h"
#include "pic.h"
#include "pci.h"
#include "hal.h"
#include "kdebug.h"

extern "C" void kmain() {
    kernel::HAL* hal = new kernel::HAL();

    serial_init(0);
    pic_init();
    _STI();
    pci_scanbus();

    hal->probe();

    for (;;) {
        char name[1024];
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
