#ifndef KERNEL_HAL_H_
#define KERNEL_HAL_H_

#include <stdbool.h>
#include <stddef.h>

namespace kernel {

class HAL {
  public:
    HAL();

    bool probe();
};

}  // namespace

#endif  // KERNEL_HAL_H_
