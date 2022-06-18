#include "inlineasm.h"
#include "heap.h"
#include "kmalloc.h"

extern char _kernel_end;

struct HEAP __kheap_kmalloc = {
    .start = (size_t)((char*)&_kernel_end) + 4096,
    .ptr = 0,
    .size = 1024*1024*16, // max heap size 16MB
    .flag = 0
};

struct KMALLOC_FREENODE {
    struct KMALLOC_FREENODE* next;
    size_t  size;
};
struct KMALLOC_FREELIST {
    _SPINLOCK lock;
    struct KMALLOC_FREENODE node;
};
// -------------------------------------------------
_SPINLOCK __kernel_kmalloc_heap_lock;
struct KMALLOC_FREELIST __kmalloc_64    = { .lock = 0, .node = { .next = 0, .size = 0} };
struct KMALLOC_FREELIST __kmalloc_512   = { .lock = 0, .node = { .next = 0, .size = 0} };
struct KMALLOC_FREELIST __kmalloc_1024  = { .lock = 0, .node = { .next = 0, .size = 0} };
struct KMALLOC_FREELIST __kmalloc_large = { .lock = 0, .node = { .next = 0, .size = 0} };
// -------------------------------------------------
void* kmalloc(size_t size) {
    struct KMALLOC_FREELIST *list;
    struct KMALLOC_FREENODE *node = 0;
    struct KMALLOC_FREENODE *parent;
    size = (((size + 15 + 16) >> 4) << 4);
    if (size <= 64) {
        list = &__kmalloc_64;
        size = 64;
    } else if (size <= 512) {
        list = &__kmalloc_512;
        size = 512;
    } else if (size <= 1024) {
        list = &__kmalloc_1024;
        size = 1024;
    } else {
        list = &__kmalloc_large;
    }
    _INT_DISABLE();
    _SPIN_LOCK(&list->lock);
    if (size <= 1024) {
        if ((node=list->node.next) != 0) {
            list->node.next = node->next;
        }
     } else {
        for (parent=&list->node; ; parent=node) {
            if ((node=parent->next) == 0) break;
            if (node->size >= size) {
                parent->next = node->next;
                break;
            }
        }
    }
    _SPIN_UNLOCK(&list->lock);
    _INT_RESTORE();

    if (node == 0) {
        _INT_DISABLE();
        node = (struct KMALLOC_FREENODE*)heap_alloc(&__kheap_kmalloc, size);
        _INT_RESTORE();
        if (node == 0) return NULL;
        node->size = size;
    }
    // kprintf ("kmalloc : return %X, %d bytes\n", node, size );
    return (void*) ((char*)node +16);  // sizeof(KMALLOC_FREENODE);
}
void kfree(const void* ptr) {
    struct KMALLOC_FREELIST* list;
    struct KMALLOC_FREENODE* node;
    if (ptr == NULL) return;
    node = (struct KMALLOC_FREENODE*)((size_t)ptr - 16);  // sizeof(KMALLOC_FREENODE));
    if (node->size <= 64) {
        list = &__kmalloc_64;
    } else if (node->size <= 512) {
        list = &__kmalloc_512;
    } else if (node->size <= 1024) {
        list = &__kmalloc_1024;
    } else {
        list = &__kmalloc_large;
    }
    _INT_DISABLE();
    _SPIN_LOCK(&list->lock);
    node->next = list->node.next;
    list->node.next = node;
    _SPIN_UNLOCK(&list->lock);
    _INT_RESTORE();
}
