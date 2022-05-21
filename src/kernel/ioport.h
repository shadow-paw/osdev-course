#ifndef __KERNEL_IOPORT_H__
#define __KERNEL_IOPORT_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void outb(uint16_t port, uint8_t val) {
    __asm volatile("outb %1, %0" : : "Nd"(port), "a"(val));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outw(uint16_t port, uint16_t val) {
    __asm volatile("outw %1, %0" : : "Nd"(port), "a"(val));
}
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm volatile("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outl(uint16_t port, uint32_t val) {
    __asm volatile("outd %1, %0" : : "Nd"(port), "a"(val));
}
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm volatile("ind %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // __KERNEL_IOPORT_H__
