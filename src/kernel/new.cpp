#include <stddef.h>
#include "kmalloc.h"

void* operator new(size_t s) throw() {
    return kmalloc(s);
}
void* operator new[](size_t s) throw() {
    return kmalloc(s);
}
void* operator new(size_t, void* p) throw() {
    return p;
}
void* operator new[](size_t, void* p) throw() {
    return p;
}
void operator delete(void* p) throw() {
    kfree(p);
}
void operator delete(void* p, size_t s) throw() {
    (void)s;
    kfree(p);
}
void operator delete[](void* p) throw() {
    kfree(p);
}
void operator delete[](void* p, size_t s) throw() {
    (void)s;
    kfree(p);
}
