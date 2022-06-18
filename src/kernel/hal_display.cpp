#include <stdbool.h>
#include "ddk_display.h"
#include "driver_bga.h"
#include "hal.h"
#include "kdebug.h"

namespace kernel {

bool HAL::probe_display_found(struct PCI_DRIVER* pci_driver, const struct PCI_ADDR* addr, const struct PCI_INFO* info) {
    if (!addr || !info) return false;
    if (info->device_id == 0x1111 && info->vendor_id == 0x1234) {
        // BGA
        if (!driver_bga(&_drivers.display)) return false;
    }
    // open display
    return _drivers.display.open(&_devices.display, pci_driver, addr, info);
}
bool HAL::display_setmode(unsigned int mode) {
    return _drivers.display.set_mode(&_devices.display, mode);
}
bool HAL::display_clearscreen() {
    return _drivers.display.clear_screen(&_devices.display);
}

}  // namespace
