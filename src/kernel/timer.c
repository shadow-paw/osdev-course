#include <stdint.h>
#include "kdebug.h"

void _timer(uint64_t tsc) {
    (void)tsc;
    // kdebug("timer: %lx\n", tsc);
}
