#include <unistd.h>
#include <stdio.h>

void syscall_usleep(unsigned int usec) {
    __asm volatile(
        "int 0x80"
        :
        : "a"(12), "b"(usec)
    );
}
int main(int argc, const char *argv[]) {
    (void)argc;
    (void)argv;
    int pid = (int)getpid();
    for (int i=0; i<10; i++) {
        printf("PID[%d]: counter=%d\n", pid, i);
        syscall_usleep(1000 * 1000);
    }
    return (int)0x0BADC0DE;
}
