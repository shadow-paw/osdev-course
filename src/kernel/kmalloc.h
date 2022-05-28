#ifndef KERNEL_KMALLOC_H_
#define KERNEL_KMALLOC_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
void* kmalloc(size_t size);
void  kfree(const void* ptr);
#ifdef __cplusplus
};
#endif


#endif  // KERNEL_KMALLOC_H_
