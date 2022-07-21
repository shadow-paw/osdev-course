# 10. Multi-Tasking II

## Process
### Address Space
In ancient days like in DOS, all process shares the same address space (mostly loaded onto the first 640kb). For
better isolation, and with the introduction of paging, all modern OS would isolate process with their own address space,
so that each program can reference to a predefined address without putting everything to the symbol table and require
huge amount of dynamic patching/relocation.

When creating a new process, you construct a new page directory structure, and with critical part of kernel (at least for
context switching code) being mapped to same virtual address. Upon context switching the CR3 will be loaded with the
process's own page table. The tricks is to maintain consistent for certain part of the paging region across different
process.

## Syscall
It is the way application code make call to kernel. There are a few mechanism:
1. software INT (e.g. `INT 80h`)
2. `sysenter` / `syscall` instruction.

### ABI
Since this way to "call the kernel function" is not the usual things defined by the C ABI, OS will need to define its
own way to pass argument and return value. Common ways are pass with stack or registers. Application usually does not
deal with kernel call directly but to leverage libraries like `libc`, for some implementation like `newlib`, the OS only
need to implement the glue code (aka `libgloss`) which define the ABI and function contract.

## Protection Ring
1. Drop to ring 3 by setting CS to lower privilege GDT entry.
2. Set stack pointer to userland stack.
3. Initialize CPU flags.

## Further Reference
1. https://wiki.osdev.org/Processes_and_Threads
2. https://wiki.osdev.org/Getting_to_Ring_3
3. https://sourceware.org/newlib/
