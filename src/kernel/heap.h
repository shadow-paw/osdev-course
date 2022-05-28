#ifndef KERNEL_HEAP_H_
#define KERNEL_HEAP_H_

#include <stdbool.h>
#include <stddef.h>

struct HEAP {
    size_t       start;
    size_t       ptr;
    size_t       size;
    unsigned int flag;
};

#define HEAP_ALLOC_MAPADDRESSONLY (0x00010000)

#ifdef __cplusplus
extern "C" {
#endif
bool  heap_create(struct HEAP* heap, const void* start, size_t size, unsigned int flag);
void* heap_alloc(struct HEAP* heap, size_t size);
#ifdef __cplusplus
};
#endif


#endif  // KERNEL_HEAP_H_
