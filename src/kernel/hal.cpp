#include <stdbool.h>
#include "kdebug.h"
#include "hal.h"

namespace kernel {

HAL::HAL() {
    kdebug("HAL::HAL()\n");
}
bool HAL::probe() {
    kdebug("HAL::probe\n");
    return false;
}

}  // namespace
