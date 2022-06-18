#ifndef DRIVERS_UART_UART_H_
#define DRIVERS_UART_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk/uart.h"

#ifdef __cplusplus
extern "C" {
#endif
bool driver_uart(struct UART_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // DRIVERS_UART_UART_H_
