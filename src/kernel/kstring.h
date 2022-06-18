#ifndef KERNEL_KSTRING_H_
#define KERNEL_KSTRING_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
size_t kmemcpy(void* des, const void* src, size_t len);
int kstrcmp(const char* a, const char* b);
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_KSTRING_H_
