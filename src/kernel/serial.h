#ifndef __KERNEL_SERIAL_H__
#define __KERNEL_SERIAL_H__

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void serial_init(int port);
void serial_putc(int port, int c);
void serial_puts(int port, const char* s);
int serial_getc(int port);
size_t serial_gets(int port, char* des, bool echo);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // __KERNEL_SERIAL_H__