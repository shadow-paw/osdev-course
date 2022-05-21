# 3. Naïve Serial Console. First Prompt. GDB.

In this lesson we will do some coding and implement our very first prompt. Our kernel will prompt user over serial I/O,
ask for name and say hello.

## PC Architecture
- [https://en.wikipedia.org/wiki/Southbridge_(computing)](https://en.wikipedia.org/wiki/Southbridge_(computing))

## Code
Code for communicating over serial port are provided in `ioport.h`, `serial.h` and `serial.c`.

## Example
An example is given showing you how to print a message to the serial console.
```
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "serial.h"

void serial_puts(int port, const char* s) {
    for (const char* p = s; *p; p++) {
        serial_putc(port, *p);
    }
}

void kernel_main(void) {
    serial_init(0);
    serial_puts(0, "Hello World");
}
```
### Run It
Boot the kernel, press `CTRL-ALT-3` or `CTRL-OPTION-3` to switch to serial console. You should see the `Hello World`.

## Mission: Simple Prompt
Adjust your kernel, implement `serial_gets`, boot up your own kernel and say hello to user!
```
What is your name? John Doe
Hello John Doe

What is your name? _
```

## Debug With GDB
1. Compile with `-g` for additional debug information. For example:
```
i686-elf-gcc -ffreestanding -std=gnu99 -O2 -Wall -Wextra -g -c $< -o $@
```

2. Create debug symbol file
```
i686-elf-objcopy --only-keep-debug kernel.bin kernel.sym
```

3. Start QEMU in debug mode (`-s -S` switch)
```
qemu-system-i386 -s -S -cdrom dist/myos.iso
```

4. Start GDB
```
i686-elf-gdb \
  --eval-command="set disassembly-flavor intel" \
  --eval-command="set architecture i386" \
  --eval-command="target remote localhost:1234" \
  --eval-command="set history save on" \
  --symbols=bin/kernel/kernel.sym
```

5. Using GDB
```
Reading symbols from bin/kernel/myos.sym...
The target architecture is set to "i386".
Remote debugging using localhost:1234
warning: No executable has been specified and target does not support
determining executable automatically.  Try using the "file" command.
0x0000fff0 in ?? ()
(gdb) b kernel_main
Breakpoint 1 at 0x100020: file src/kernel/main.c, line 5.
(gdb) c
Continuing.

Breakpoint 1, kernel_main () at src/kernel/main.c:5
5           serial_init(0);
(gdb)
```

## Reference
- [https://en.wikibooks.org/wiki/X86_Assembly/GNU_assembly_syntax](https://en.wikibooks.org/wiki/X86_Assembly/GNU_assembly_syntax)
- [https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html)
- [https://en.wikipedia.org/wiki/Southbridge_(computing)](https://en.wikipedia.org/wiki/Southbridge_(computing))
- [https://wiki.osdev.org/Serial_Ports](https://wiki.osdev.org/Serial_Ports)
- [https://wiki.osdev.org/GDB](https://wiki.osdev.org/GDB)
