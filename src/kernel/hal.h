#ifndef KERNEL_HAL_H_
#define KERNEL_HAL_H_

#include <stdbool.h>
#include <stddef.h>
#include "ddk_uart.h"

namespace kernel {

class HAL {
  public:
    static HAL* inst;

    HAL();

    bool probe();

    // UART Functions
    bool uart_putc(unsigned int port, char c);
    char uart_getc(unsigned int port);
    bool uart_puts(unsigned int port, const char* s);
    size_t uart_gets(unsigned int port, char* des, size_t deslen, bool echo);

  private:
    struct {
      struct UART_DRIVER uart;
    } _drivers;
    struct {
      struct UART_DEVICE uart[4];
    } _devices;

    bool probe_uart();
};

extern HAL hal;

}  // namespace

#endif  // KERNEL_HAL_H_
