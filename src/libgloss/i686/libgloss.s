; -----------------------------------------------------------
;
; libgloss
;
; syscall wrapper to provide low level support to libc
;
; -----------------------------------------------------------
cpu 586
bits 32

global open, close, read, write, lseek, fstat, stat, unlink
global getpid, kill, isatty, sbrk
global usleep, _exit

section .text
; int _exit();
_exit:
    push    ebx
    mov     eax, 1
    mov     ebx, [esp+8]
    int     0x80
    pop     ebx
    ret

; int open(const char * file, int flags, int mode);
open:
    push    ebx
    push    ecx
    push    edx
    mov     ebx, [esp+16]   ; file
    mov     ecx, [esp+20]   ; flags
    mov     edx, [esp+24]   ; mode
    mov     eax, 2
    int     0x80
    pop     edx
    pop     ecx
    pop     ebx
    ret

; int close(int fd);
close:
    push    ebx
    mov     ebx, [esp+8]    ; fd
    mov     eax, 3
    int     0x80
    pop     ebx
    ret

; int read(int fd, char *buf, int nbytes);
read:
    push    ebx
    push    ecx
    push    edx
    mov     ebx, [esp+16]   ; fd
    mov     ecx, [esp+20]   ; buf
    mov     edx, [esp+24]   ; nbytes
    mov     eax, 4
    int     0x80
    pop     edx
    pop     ecx
    pop     ebx
    ret

; int write(int fd, char *buf, int nbytes);
write:
    push    ebx
    push    ecx
    push    edx
    mov     ebx, [esp+16]   ; fd
    mov     ecx, [esp+20]   ; buf
    mov     edx, [esp+24]   ; nbytes
    mov     eax, 5
    int     0x80
    pop     edx
    pop     ecx
    pop     ebx
    ret

; off_t lseek(int fd, off_t offset, int whence);
lseek:
    push    ebx
    push    ecx
    push    edx
    mov     ebx, [esp+16]   ; fd
    mov     ecx, [esp+20]   ; offset
    mov     edx, [esp+24]   ; whence
    mov     eax, 6
    int     0x80
    pop     edx
    pop     ecx
    pop     ebx
    ret

; int unlink(const char *path);
unlink:
    push    ebx
    mov     ebx, [esp+8]    ; path
    mov     eax, 7
    int     0x80
    pop     ebx
    ret

; int getpid();
getpid:
    mov     eax, 8
    int     0x80
    ret

; int kill(int pid, int sig);
kill:
    push    ebx
    push    ecx
    mov     ebx, [esp+12]   ; pid
    mov     ecx, [esp+16]   ; sig
    mov     eax, 9
    int     0x80
    pop     ecx
    pop     ebx
    ret

; int fstat(int fd, struct stat *buf);
fstat:
    push    ebx
    push    ecx
    mov     ebx, [esp+12]   ; fd
    mov     ecx, [esp+16]   ; buf
    mov     eax, 10
    int     0x80
    pop     ecx
    pop     ebx
    ret

; char* sbrk(int nbytes);
sbrk:
    push    ebx
    mov     ebx, [esp+8]
    mov     eax, 11
    int     0x80
    pop     ebx
    ret

; void usleep(int usec);
usleep:
    push    ebx
    mov     ebx, [esp+8]
    mov     eax, 12
    int     0x80
    pop     ebx
    ret

; int stat(const char *path, struct stat *buf);
stat:
    mov     eax, -1
    ret

; int isatty(int fd);
isatty:
    xor     eax, eax
    ret
