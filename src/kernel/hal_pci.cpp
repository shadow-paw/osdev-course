#include <stdbool.h>
#include "driver_pci.h"
#include "hal.h"
#include "kdebug.h"

namespace kernel {

bool HAL::probe_pci() {
    struct PCI_ADDR addr;
    struct PCI_INFO info;

    if (!driver_pci(&_drivers.pci)) return false;
    for (unsigned int bus_id=0; bus_id<256; bus_id++) {
        for (unsigned int slot_id=0; slot_id<32; slot_id++) {
            for (unsigned int func_id=0; func_id<8; func_id++) {
                addr.bus_id  = bus_id;
                addr.slot_id = slot_id;
                addr.func_id = func_id;
                if (!_drivers.pci.get_info(&addr, &info)) continue;
                probe_pci_found(&addr, &info);
            }
        }
    }
    return true;
}
bool HAL::probe_pci_found(const struct PCI_ADDR* addr, const struct PCI_INFO* info) {
    kdebug("  PCI : bus:%d slot:%d func:%d\n"
           "        device:%X vendor:%X class:%d:%d\n",
        addr->bus_id, addr->slot_id, addr->func_id,
        info->device_id, info->vendor_id, info->class_code, info->subclass_code
    );
    switch (info->class_code) {
    case 2:
        switch (info->subclass_code) {
        case 0:     // Ethernet
            break;
        } break;
    case 3:
        switch (info->subclass_code) {
        case 0:     // Display
            probe_display_found(&_drivers.pci, addr, info);
            break;
        } break;
    }
    return true;
}

}  // namespace
