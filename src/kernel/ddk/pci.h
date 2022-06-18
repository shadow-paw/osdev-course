#ifndef KERNEL_DDK_PCI_H_
#define KERNEL_DDK_PCI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct PCI_ADDR {
    unsigned int bus_id;
    unsigned int slot_id;
    unsigned int func_id;
};

struct PCI_INFO {
    unsigned int device_id;
    unsigned int vendor_id;
    unsigned int class_code;
    unsigned int subclass_code;
    unsigned int revision_id;
};

struct PCI_DRIVER {
    bool (*get_info)(struct PCI_DRIVER* driver, const struct PCI_ADDR* addr, struct PCI_INFO* info);
    bool (*get_bar)(struct PCI_DRIVER* driver, const struct PCI_ADDR* addr, unsigned int index, uintptr_t* bar, uint32_t* size);
    bool (*get_irq)(struct PCI_DRIVER* driver, const struct PCI_ADDR* addr, uint32_t* irq);
    bool (*command)(struct PCI_DRIVER* driver, const struct PCI_ADDR* addr, uint16_t cmd);
};

#ifdef __cplusplus
};
#endif

#endif  // KERNEL_DDK_PCI_H_
