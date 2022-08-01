#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "ioport.h"

char tohex(int digit, bool upper) {
    static const char up_digits[] = "0123456789ABCDEF";
    static const char low_digits[] = "0123456789abcdef";
    if (digit < 0 || digit >= (int)sizeof(up_digits)) {
        return '-';
    }
    return upper ? up_digits[digit] : low_digits[digit];
}
void hextoasc32(uint32_t num, char* des, bool upper) {
    const int total_digits = sizeof(num) * 2;
    const int shift_bits = sizeof(num) * 8 - 4;
    for (int i=0; i < total_digits; i++) {
        uint32_t digit = (uint32_t)(num >> shift_bits);
        num = (num << 4);
        *des++ = tohex(digit, upper);
    } *des = 0;
}
void hextoasc64(uint64_t num, char* des, bool upper) {
    const int total_digits = sizeof(num) * 2;
    const int shift_bits = sizeof(num) * 8 - 4;
    for (int i=0; i < total_digits; i++) {
        uint64_t digit = (uint64_t)(num >> shift_bits);
        num = (num << 4);
        if (i == 8) {
            *des++ = ':';
        }
        *des++ = tohex(digit, upper);
    } *des = 0;
}

void kdebug_putc(char c) {
    // TODO: improve this
    const uint16_t base = 0x03F8;
    while ((inb(base + 5) & 0x20) == 0) {
      // Busy loop
    }
    outb(base, (uint8_t)c);
}
void kdebug_puts(const char* s) {
    for (const char* p = s; *p; p++) {
        kdebug_putc(*p);
    }
}
void kdebug(const char* fmt, ...) {
    char buf[32];
    const char* p;
    char c;
    uint32_t u32;
    uint64_t u64;
    const char *s;
    va_list va;
    bool is_long = false;
    bool upper;

    va_start(va, fmt);
    for (p=fmt; *p; p++) {
        c = *p;
        if (c == '\n') {
            kdebug_putc('\r');
            kdebug_putc('\n');
        } else if (c != '%') {
            kdebug_putc(c);
        } else {
            ++p;
            c = *p;
            if ( c == 'l' ) {
                is_long = true;
                c = *(++p);
            } else {
                is_long = false;
            }
            switch (c) {
                case 'd':
                    // TODO: not very useful, just dump hex now
                    if (is_long) {
                        u64 = va_arg(va, uint64_t);
                        hextoasc64(u64, buf, false);
                    } else {
                        u32 = va_arg(va, uint32_t);
                        hextoasc32(u32, buf, false);
                    }
                    kdebug_puts(buf);
                    break;
                case 'x':
                case 'X':
                    upper = c == 'X';
                    if (is_long) {
                        u64 = va_arg(va, uint64_t);
                        hextoasc64(u64, buf, upper);
                    } else {
                        u32 = va_arg(va, uint32_t);
                        hextoasc32(u32, buf, upper);
                    }
                    kdebug_puts(buf);
                    break;
                case 'p':
                    u64 = (uint64_t)(uintptr_t)va_arg(va, const char*);
                    if ((u64 >> 32) == 0) {
                        hextoasc32(u64 & 0xffffffff, buf, false);
                    } else {
                        hextoasc64(u64, buf, false);
                    }
                    kdebug_puts(buf);
                    break;
                case 'c':
                    u32 = va_arg(va, uint32_t);
                    if (u32 == '\n') {
                        kdebug_putc('\r');
                    }
                    kdebug_putc(u32);
                    break;
                case 's':
                    s = va_arg(va, const char*);
                    kdebug_puts(s);
                    break;
                case '%':
                    kdebug_putc(c);
                    break;
                default:
                    kdebug_putc('%');
                    kdebug_putc(c);
                    break;
            }
        }
    }
    va_end(va);
}
