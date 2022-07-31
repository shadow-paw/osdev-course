#ifndef KERNEL_KSTRING_H_
#define KERNEL_KSTRING_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
size_t kmemcpy(void* des, const void* src, size_t len);
void* kmemset(void* des, uint8_t value, size_t len);
size_t kstrncpy(char* des, const char* src, size_t deslen);
int kstrcmp(const char* a, const char* b);
int kstrncmp(const char* a, const char* b, size_t n);
size_t kstrlen(const char* s);
#ifdef __cplusplus
};
#endif

#endif  // KERNEL_KSTRING_H_
