#ifndef DRIVERS_PCI_PCI_H_
#define DRIVERS_PCI_PCI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk/pci.h"

#ifdef __cplusplus
extern "C" {
#endif
bool driver_pci(struct PCI_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // DRIVERS_PCI_PCI_H_
