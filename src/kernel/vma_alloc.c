#include "heap.h"
#include "vma_alloc.h"

struct HEAP __kheap_vmaalloc = {
    .start = (size_t)(0xE0000000),
    .ptr = 0,
    .size = 1024*1024*64, // max heap size 64MB
    .flag = HEAP_ALLOC_NOALLOC
};

void* vma_alloc(size_t size) {
    return heap_alloc(&__kheap_vmaalloc, size);
}
