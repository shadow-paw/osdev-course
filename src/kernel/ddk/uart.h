#ifndef KERNEL_DDK_UART_H_
#define KERNEL_DDK_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct UART_DRIVER;
struct UART_DEVICE {
    struct UART_DRIVER* driver;
    unsigned int port;
};

struct UART_DRIVER {
    bool (*open)(struct UART_DRIVER* driver, struct UART_DEVICE* device, unsigned int port);
    bool (*close)(struct UART_DRIVER* driver, struct UART_DEVICE* device);
    bool (*putc)(struct UART_DRIVER* driver, struct UART_DEVICE* device, char c);
    char (*getc)(struct UART_DRIVER* driver, struct UART_DEVICE* device);
};

#ifdef __cplusplus
}
#endif

#endif  // KERNEL_DDK_UART_H_
