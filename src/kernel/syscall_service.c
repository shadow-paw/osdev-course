#include "mmu.h"
#include "heap.h"
#include "scheduler.h"
#include "process.h"
#include "kdebug.h"

int kservice_null() {
    kdebug("SYSCALL: service #0 not supported.\n");
    return -1;
}
int kservice_exit(int code) {
    kdebug("SYSCALL: exit(%X)\n", code);
    // TODO: exit process
    scheduler_exitthread();
    return 0;
}
int kservice_open(const char * file, int flags, int mode) {
    kdebug("SYSCALL: open(%s, %X, %X)\n", file, flags, mode);
    return -1;
}
int kservice_close(int fd) {
    kdebug("SYSCALL: close(%X)\n", fd);
    return -1;
}
size_t kservice_read(int fd, void* buf, size_t len) {
    kdebug("SYSCALL: read(%d, %X, %d)\n", fd, buf, len);
    return -1;
}
size_t kservice_write(int fd, char* s, size_t len) {
    (void)fd;
    // kdebug("SYSCALL: write(%d, %X, %d)\n", fd, s, len);
    for (size_t i=0; i<len; i++) {
        kdebug("%c", s[i]);
    }
    return len;
}
int kservice_lseek(int fd, uint32_t offset, int whence) {
    kdebug("SYSCALL: lseek(%X, %X, %X)\n", fd, offset, whence);
    return -1;
}
int kservice_unlink(const char* file) {
    kdebug("SYSCALL: unlink(%s)\n", file);
    return -1;
}
uint32_t kservice_getpid() {
    struct PROCESS* process = process_current();
    kdebug("SYSCALL: getpid() -> %d\n", process->pid);
    return process->pid;
}
int kservice_kill(int pid, int sig) {
    kdebug("SYSCALL: kill(%X, %X)\n", pid, sig);
    return -1;
}
int kservice_fstat(int fd, void* st) {
    kdebug("SYSCALL: fstat(%d, %p)\n", fd, st);
    if (st == NULL) return -1;
    // st->st_mode = S_IFCHR; // | S_IWUSR;
    //                        //    st->st_blksize = 1;
    return 0;
}
uint32_t kservice_sbrk(size_t nbytes) {
    kdebug ("SYSCALL: sbrk(%X)\n", nbytes);
    struct PROCESS* p = process_current();
    return (uint32_t)heap_alloc(&p->heap, nbytes);
}
int kservice_usleep(unsigned int usec) {
    // kdebug("SYSCALL: usleep(%X)\n", usec);
    scheduler_sleep(usec * 18 /1000/1000);
    return 0;
}
uint32_t syscall_table[] = {
    (uint32_t)kservice_null,
    (uint32_t)kservice_exit,
    (uint32_t)kservice_open,
    (uint32_t)kservice_close,
    (uint32_t)kservice_read,
    (uint32_t)kservice_write,
    (uint32_t)kservice_lseek,
    (uint32_t)kservice_unlink,
    (uint32_t)kservice_getpid,
    (uint32_t)kservice_kill,
    (uint32_t)kservice_fstat,
    (uint32_t)kservice_sbrk,
    (uint32_t)kservice_usleep
};
