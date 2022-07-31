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
void* kmemset(void* des, uint8_t value, size_t len) {
    uint8_t* udes = (uint8_t*)des;
    for (size_t i=0; i<len; i++) {
        *udes++ = value;
    }
    return des;
}
size_t kstrncpy(char* des, const char* src, size_t deslen) {
    size_t copied = 0;
    if (deslen == 0) return 0;
    for (;;) {
        if (copied +1 >= deslen) {
            *des = 0;
            return copied;
        }
        char c = *src++;
        *des++ = c;
        if (c == 0) return copied;
        copied ++;
    }
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
int kstrncmp(const char* a, const char* b, size_t n) {
    if (n == 0) return 0;
    for (; *a && *b; a++, b++) {
        int d = *a - *b;
        if (d != 0) return d;
        n--;
        if (n == 0) return 0;
    }
    if (*a != 0) return -1;
    if (*b != 0) return 1;
    return 0;
}
size_t kstrlen(const char* s) {
    size_t count = 0;
    while (*s) {
        s++;
        count++;
    }
    return count;
}
