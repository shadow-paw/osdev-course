#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk/ddk.h"
#include "pci.h"

#define PCI_CONFIG_ADDRESS (0xCF8)
#define PCI_CONFIG_DATA    (0xCFC)

unsigned int pci_base(unsigned int bus_id, unsigned int slot_id, unsigned int func_id) {
    if ( bus_id >= 256 || slot_id >= 32 || func_id >= 8 ) return 0;
    return (unsigned int)(0x80000000 | (bus_id<<16) | (slot_id<<11) | (func_id<<8));
}

bool pci_get_info(const struct PCI_ADDR* addr, struct PCI_INFO* info) {
    unsigned int base;
    uint32_t data;
    if (!addr || !info) return false;
    if ((base=pci_base(addr->bus_id, addr->slot_id, addr->func_id)) == 0) return false;

    outl(PCI_CONFIG_ADDRESS, base +0);
    if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return false;
    info->device_id = (unsigned int)(data >> 16);
    info->vendor_id = (unsigned int)(data & 0xFFFF);

    outl(PCI_CONFIG_ADDRESS, base +8);
    if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return false;
    info->class_code    = (unsigned int)(data >> 24);
    info->subclass_code = (unsigned int)((data >> 16) & 0xFF);
    info->revision_id   = (unsigned int)(data & 0xFF);
    return true;
}
bool pci_get_bar(const struct PCI_ADDR* addr, unsigned int index, uintptr_t* bar, uint32_t* size) {
    unsigned int base;
    uint32_t b=0, s=0;
    if (!addr || !bar || index > 4) return false;
    if ((base=pci_base(addr->bus_id, addr->slot_id, addr->func_id)) == 0) return false;

    outl(PCI_CONFIG_ADDRESS, base +16 + index*4);
    if ((b = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return false;
    *bar = (uintptr_t)b;
    if (size) {
        outl(PCI_CONFIG_ADDRESS, base +16 + index*4);
        outl(PCI_CONFIG_DATA, 0xFFFFFFFF);
        if ((s = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return false;
        outl(PCI_CONFIG_ADDRESS, base +16 + index*4);
        outl(PCI_CONFIG_DATA, b);
        *size = (~s) +1;
    }
    return true;
}
bool pci_get_irq(const struct PCI_ADDR* addr, uint32_t* irq) {
    unsigned int base;
    uint32_t data;
    if (!addr || !irq) return false;
    if ((base=pci_base(addr->bus_id, addr->slot_id, addr->func_id)) == 0) return false;

    outl(PCI_CONFIG_ADDRESS, base +0x3C);
    if ((data = inl(PCI_CONFIG_DATA)) == 0xFFFFFFFF) return false;
    *irq = data & 0xFF;
    return true;
}
bool pci_command(const struct PCI_ADDR* addr, uint16_t cmd) {
    unsigned int base;
    if (!addr) return false;
    if ((base=pci_base(addr->bus_id, addr->slot_id, addr->func_id)) == 0) return false;

    outl(PCI_CONFIG_ADDRESS, base +6);
    outl(PCI_CONFIG_DATA, cmd);
    return true;
}
bool driver_pci(struct PCI_DRIVER* driver) {
    if (!driver) return false;
    driver->get_info = pci_get_info;
    driver->get_bar = pci_get_bar;
    driver->get_irq = pci_get_irq;
    driver->command = pci_command;
    return true;
}