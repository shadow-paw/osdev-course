#ifndef KERNEL_DRIVER_UART_H_
#define KERNEL_DRIVER_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk_uart.h"

#ifdef __cplusplus
extern "C" {
#endif
bool driver_uart(struct UART_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // KERNEL_DRIVER_UART_H_
