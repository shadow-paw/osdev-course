#ifndef KERNEL_DRIVER_PCI_H_
#define KERNEL_DRIVER_PCI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk_pci.h"

#ifdef __cplusplus
extern "C" {
#endif
bool driver_pci(struct PCI_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // KERNEL_DRIVER_PCI_H_
