#include <stdint.h>
#include "kstring.h"

size_t kmemcpy(void* des, const void* src, size_t len) {
    uint8_t* udes = (uint8_t*)des;
    const uint8_t* usrc = (const uint8_t*)src;
    for (size_t i = 0; i<len; i++) {
        *udes++ = *usrc++;
    }
    return len;
}
int kstrcmp(const char* a, const char* b) {
    for (; *a && *b; a++, b++) {
        int d = *a - *b;
        if (d != 0) return d;
    }
    if (*a != 0) return -1;
    if (*b != 0) return 1;
    return 0;
}
