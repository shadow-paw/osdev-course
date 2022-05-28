#ifndef KERNEL_IOPORT_H_
#define KERNEL_IOPORT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void outb(uint16_t port, uint8_t val) {
    __asm volatile("outb %0, %1" : : "Nd"(port), "a"(val));
}
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm volatile("inb %0, %1" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outw(uint16_t port, uint16_t val) {
    __asm volatile("outw %0, %1" : : "Nd"(port), "a"(val));
}
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm volatile("inw %0, %1" : "=a"(ret) : "Nd"(port));
    return ret;
}
static inline void outl(uint16_t port, uint32_t val) {
    __asm volatile("outd %0, %1" : : "Nd"(port), "a"(val));
}
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    __asm volatile("ind %0, %1" : "=a"(ret) : "Nd"(port));
    return ret;
}

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // KERNEL_IOPORT_H_
