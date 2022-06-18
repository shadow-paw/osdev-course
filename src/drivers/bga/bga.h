#ifndef DRIVERS_BGA_BGA_H_
#define DRIVERS_BGA_BGA_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk/display.h"

#ifdef __cplusplus
extern "C" {
#endif
bool driver_bga(struct DISPLAY_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // DRIVERS_BGA_BGA_H_
