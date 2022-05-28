#ifndef KERNEL_KDEBUG_H_
#define KERNEL_KDEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

void kdebug(const char* fmt, ...);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // KERNEL_KDEBUG_H_
