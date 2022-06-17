#include <stdbool.h>
#include "hal.h"
#include "kdebug.h"

namespace kernel {

HAL hal;

HAL::HAL() {
    __builtin_memset(&_drivers, 0, sizeof(_drivers));
    __builtin_memset(&_devices, 0, sizeof(_devices));
    kdebug("HAL::HAL()\n");
}
bool HAL::probe() {
    probe_uart();
    return true;
}

}  // namespace
