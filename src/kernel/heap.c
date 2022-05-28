#include "mmu.h"
#include "heap.h"

bool heap_create(struct HEAP* heap, const void* start, size_t size, unsigned int flag) {
    heap->start = (size_t)start;
    heap->ptr = 0;
    heap->size = size;
    heap->flag = flag;
    return true;
}
void* heap_alloc(struct HEAP* heap, size_t size) {
    size_t from, to;
    void* mem;
    if (heap->ptr + size >= heap->size) return NULL;
    if ((heap->flag & HEAP_ALLOC_MAPADDRESSONLY) == 0) {
        from = ((heap->start + heap->ptr) >> 12) << 12;
        to = ((heap->start + heap->ptr + size + 4095) >> 12) << 12;
        if (to > from) {
            mmu_mmap((void*)from, 0, (to-from), heap->flag & 0xFFFF);
        }
    }
    mem = (void*) (heap->start + heap->ptr);
    heap->ptr += size;
    return mem;
}
