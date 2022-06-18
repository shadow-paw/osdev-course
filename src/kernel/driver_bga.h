#ifndef KERNEL_DRIVER_BGA_H_
#define KERNEL_DRIVER_BGA_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk_display.h"

#ifdef __cplusplus
extern "C" {
#endif
bool driver_bga(struct DISPLAY_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // KERNEL_DRIVER_BGA_H_
