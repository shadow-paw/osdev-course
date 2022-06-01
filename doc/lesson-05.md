# 5. Memory Management - Code Walkthrough

## `boot.s`

### Move kernel to higher half
1. Identity map. We map VMA 0 Mb ~ 1.x Mb (`_kernel_end`) → PMA 0 Mb ~ 1.x Mb, also VMA 3.75 Gb ~ 3.7x Gb → PMA 0 Mb ~ 1.x Mb.
2. Enable paging. cr3 → page directory, cr0 |= 0x80000000.
3. Update selectors (`ds`, `es`, `fs`, `gs`, `ss`) and jump to high address (`cs`).

### TSS
Let’s pretend we don’t see that. It is used when we implement user space and transit between rings.

### Setup IDT
IDT is interrupt descriptor table. It tell the CPU our handlers when something happen (IRQ, CPU exception, or triggered by `int` instruction). For paging to work we need to have a handler for page fault.

### multiboot information
- [https://www.gnu.org/software/grub/manual/multiboot/multiboot.html](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
- [3.2 Machine state](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state)
- [3.3 Boot information format](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Boot-information-format)

The boot information is on `ebx`, but it is in terms of physical address. We simply convert it to our high address by `ebx + 3.75G` and pass it to `kmain`.

## `main.c`
### Dump the memory map
```
mmap: 00000000:00000000, size: 00000000:0009fc00 = 00000001
mmap: 00000000:0009fc00, size: 00000000:00000400 = 00000002
mmap: 00000000:000f0000, size: 00000000:00010000 = 00000002
mmap: 00000000:00100000, size: 00000000:01ee0000 = 00000001
mmap: 00000000:01fe0000, size: 00000000:00020000 = 00000002
mmap: 00000000:fffc0000, size: 00000000:00040000 = 00000002
```

## `mmu.c`
### Frame allocator
1. Define special virtual address for the stack → `MMU_FRAMEPOOL_VMA`
2. Map one 4k page to the begin of `MMU_FRAMEPOOL_VMA`. That 4k page is from `bss` which is within `_kernel_end` which will be reserved and excluded from the allocator.
3. walk the memory map, push each 4k into the stack.
    1. Upon pushing 512'th page, the stack grow over 4k boundary and will trigger a page fault since the virtual address `MMU_FRAMEPOOL_VMA` + 4096 is not mapped.
    2. Page fault handler will pop a page from the stack, which already have 512 entry, and map to the virtual address `MMU_FRAMEPOOL_VMA` + 4096.
    3. Upon handler return, CPU retry to push to 512th entry - this will be mis-sync with our stack pointer as we already pop a page!
4. Remedy - try to read that before write, so CPU will retry the read instruction!

```
mmu_mark((const void*)&MMU_frames[MMU_frames_index], 0, MMU_PAGE_ONDEMAND);
// Trigger #PF and we will pop a page there, which would decrease MMU_frames_index.
// We want cpu to retry on the read op here, so the actual write op can use correct index.
volatile uint32_t* ptr = &MMU_frames[MMU_frames_index];
(void)*ptr;
```

### Frame Stack
Just a naïve stack with push and pop. Alternately you may use other data structure to support feature like [page coloring](https://forum.osdev.org/viewtopic.php?f=15&t=32717).

### Page Fault Handler
1. Triggered by CPU exception, remember we setup IDT.
2. Information
    1. `code`: what happened?
    2. `addr`: what virtual address the CPU is trying to access?
    3. `ip`: instruction pointer. We can use this to deduce the nature of code.
3. Handling
    1. On-demand Paging
        1. Get a free page from frame allocator.
        2. Map it by putting information to PD and PT.
        3. `invlpg` to tell CPU to invalidate the TLB cache on that address.
        4. Optionally, clear the memory.
        5. Return, the CPU will retry the offending instruction, which should the reading the memory.

## `heap.c`
Reserve virtual address range and map physical memory when needed. For now the address range is hard-coded but we can
also introduce a virtual address manager for address range allocation.

## `kmalloc.c`
### `kmalloc`
1. free list holds memory that have been `kfree`. We have multiple lists for different size of memory.
2. Upon `kmalloc`, we check if there is any previously free'd block from the free lists. If found, just reuse that.
3. If no free list to be reused, get a new chunk from heap.
### `kfree`
Put the memory to free list so it can be reused later.
