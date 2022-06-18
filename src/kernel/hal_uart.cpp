#include <stdbool.h>
#include "ddk/uart.h"
#include "drivers/uart/uart.h"
#include "hal.h"

namespace kernel {

bool HAL::probe_uart() {
    if (!driver_uart(&_drivers.uart)) return false;
    for (int port=0; port<4; port++) {
        if (!_drivers.uart.open(&_drivers.uart, &_devices.uart[port], port)) {
            _devices.uart[port].port = 0xFFFFFFFF; // not available
        }
    }
    return true;
}
bool HAL::uart_putc(unsigned int port, char c) {
    if (port >= 4) return false;
    return _drivers.uart.putc(&_drivers.uart, &_devices.uart[port], c);
}
char HAL::uart_getc(unsigned int port) {
    if (port >= 4) return false;
    return _drivers.uart.getc(&_drivers.uart, &_devices.uart[port]);
}
bool HAL::uart_puts(unsigned int port, const char* s) {
    for (const char* p = s; *p; p++) {
        uart_putc(port, *p);
    }
    return true;
}
size_t HAL::uart_gets(unsigned int port, char* des, size_t maxlen, bool echo) {
    size_t len = 0;
    if (maxlen == 0) return 0;
    if (maxlen == 1) {
        des[0] = 0;
        return 0;
    }
    for (;;) {
        char c = uart_getc(port);
        if (c == 0 || c == '\r' || c == '\n') {
            des[len] = 0;
            return len;
        }
        if (echo) {
            uart_putc(port, c); // echo so user can see what typed
        }
        des[len] = c;
        len ++;
        if (len + 1 >= maxlen) {
            des[len] = 0;
            return len;
        }
    }
}

}  // namespace
