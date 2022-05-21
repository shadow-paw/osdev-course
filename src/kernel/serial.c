#include <stddef.h>
#include <stdint.h>
#include "ioport.h"
#include "serial.h"

static const uint16_t SERIAL_PORTS[] = {0x03F8, 0x02F8, 0x03E8, 0x02E8};
void serial_init(int port) {
    const uint16_t base = SERIAL_PORTS[port];
    outb(base + 1, 0x00); // Disable all interrupts
    outb(base + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(base + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(base + 1, 0x00); //                  (hi byte)
    outb(base + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(base + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(base + 4, 0x0B); // IRQs enabled, RTS/DSR set
}
void serial_putc(int port, int c) {
    const uint16_t base = SERIAL_PORTS[port];
    // wait for transmit empty
    while ((inb(base + 5) & 0x20) == 0) {
      // Busy loop
    }
    outb(base, (uint8_t)c);
}
int serial_getc(int port) {
    const uint16_t base = SERIAL_PORTS[port];
    // wait for input
    while ((inb(base + 5) & 1) == 0) {
      // Busy loop
    }
    return (int)inb(base);
}
void serial_puts(int port, const char* s) {
    for (const char* p = s; *p; p++) {
        serial_putc(port, *p);
    }
}
size_t serial_gets(int port, char* des, bool echo) {
    size_t len = 0;
    for (;;) {
        int c = serial_getc(port);
        if (c == '\r' || c == '\n') {
            des[len] = 0;
            return len;
        }
        if (echo) {
            serial_putc(port, c); // echo so user can see what typed
        }
        // NOTE: potential buffer overrun, but simplicity for demo purpose.
        des[len] = c;
        len ++;
    }
}
