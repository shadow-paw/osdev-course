#include <stddef.h>
#include <stdint.h>
#include "ddk/ddk.h"
#include "uart.h"

static const uint16_t __UART_DRIVER_ioports[] = { 0x03F8, 0x02F8, 0x03E8, 0x02E8 };

bool uart_open(struct UART_DEVICE* device, unsigned int port) {
    if (!device || port >= 4) return false;
    const uint16_t base = __UART_DRIVER_ioports[port];
    outb(base + 1, 0x00); // Disable all interrupts
    outb(base + 3, 0x80); // Enable DLAB (set baud rate divisor)
    outb(base + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    outb(base + 1, 0x00); //                  (hi byte)
    outb(base + 3, 0x03); // 8 bits, no parity, one stop bit
    outb(base + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    outb(base + 4, 0x0B); // IRQs enabled, RTS/DSR set
    device->port = port;
    return true;
}
bool uart_close(struct UART_DEVICE* device) {
    if (!device) return false;
    device->port = -1;
    return true;
}
bool uart_putc(struct UART_DEVICE* device, char c) {
    if (!device || device->port >= 4) return false;
    const uint16_t base = __UART_DRIVER_ioports[device->port];
    while ((inb(base + 5) & 0x20) == 0) {
      // Busy loop
    }
    outb(base, (uint8_t)c);
    return true;
}
char uart_getc(struct UART_DEVICE* device) {
    if (!device || device->port >= 4) return 0;
    const uint16_t base = __UART_DRIVER_ioports[device->port];
    // wait for input
    while ((inb(base + 5) & 1) == 0) {
      // Busy loop
    }
    return (char)inb(base);
}

bool driver_uart(struct UART_DRIVER* driver) {
    if (!driver) return false;
    driver->open = uart_open;
    driver->close = uart_close;
    driver->putc = uart_putc;
    driver->getc = uart_getc;
    return true;
}
