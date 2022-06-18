#ifndef KERNEL_HAL_H_
#define KERNEL_HAL_H_

#include <stdbool.h>
#include <stddef.h>
#include "ddk/pci.h"
#include "ddk/display.h"
#include "ddk/uart.h"

namespace kernel {

class HAL {
  public:
    static HAL* inst;

    HAL();

    bool probe();

    // UART functions
    bool uart_putc(unsigned int port, char c);
    char uart_getc(unsigned int port);
    bool uart_puts(unsigned int port, const char* s);
    size_t uart_gets(unsigned int port, char* des, size_t deslen, bool echo);

    // Display functions
    bool display_setmode(unsigned int mode);
    bool display_clearscreen();

  private:
    struct {
      struct PCI_DRIVER pci;
      struct UART_DRIVER uart;
      struct DISPLAY_DRIVER display;
    } _drivers;
    struct {
      struct UART_DEVICE uart[4];
      struct DISPLAY_DEVICE display;
    } _devices;

    bool probe_uart();
    bool probe_pci();
    bool probe_pci_found(const struct PCI_ADDR* addr, const struct PCI_INFO* info);
    bool probe_display_found(struct PCI_DRIVER* pci_driver, const struct PCI_ADDR* addr, const struct PCI_INFO* info);
};

extern HAL hal;

}  // namespace

#endif  // KERNEL_HAL_H_
