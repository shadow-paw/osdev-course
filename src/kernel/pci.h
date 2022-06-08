#ifndef KERNEL_PCI_H_
#define KERNEL_PCI_H_

#ifdef __cplusplus
extern "C" {
#endif

struct PCI_BUS_ADDR {
    unsigned int bus_id;
    unsigned int slot_id;
    unsigned int func_id;
};

struct PCI_BUS_INFO {
    unsigned int device_id;
    unsigned int vendor_id;
    unsigned int class_code;
    unsigned int subclass_code;
    unsigned int revision_id;
};

bool pci_get_info(const struct PCI_BUS_ADDR* addr, struct PCI_BUS_INFO* info);
bool pci_get_bar(const struct PCI_BUS_ADDR* addr, unsigned int index, uintptr_t* bar, uint32_t* size);
bool pci_get_irq(const struct PCI_BUS_ADDR* addr, uint32_t* irq);
bool pci_command(const struct PCI_BUS_ADDR* addr, uint16_t cmd);
bool pci_scanbus();

#ifdef __cplusplus
};
#endif

#endif  // KERNEL_PCI_H_
