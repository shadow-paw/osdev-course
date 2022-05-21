#include "serial.h"

void kmain(void) {
    char name[1024];
    serial_init(0);
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
