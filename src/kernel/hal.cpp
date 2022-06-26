#include <stdbool.h>
#include "inlineasm.h"
#include "pic.h"
#include "hal.h"
#include "kdebug.h"

namespace kernel {

HAL hal;

HAL::HAL() {
    __builtin_memset(&_drivers, 0, sizeof(_drivers));
    __builtin_memset(&_devices, 0, sizeof(_devices));
}
bool HAL::probe() {
    pic_init();
    probe_uart();
    probe_pci();
    _STI();
    return true;
}

}  // namespace
