#ifndef KERNEL_KHEAPS_H_
#define KERNEL_KHEAPS_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
void* vma_alloc(size_t size);
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_KHEAPS_H_
