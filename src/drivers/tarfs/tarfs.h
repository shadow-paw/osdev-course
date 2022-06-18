#ifndef DRIVERS_TARFS_TARFS_H_
#define DRIVERS_TARFS_TARFS_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "ddk/filesystem.h"

#ifdef __cplusplus
extern "C" {
#endif
bool driver_tarfs(struct FS_DRIVER* driver);
#ifdef __cplusplus
}
#endif

#endif  // DRIVERS_TARFS_TARFS_H_
