# 4. Memory Management

## Memory Type
- Ram chip.
- Device memory, e.g. display memory.
- Memory mapped IO.
- Bus mastering / DMA

## Memory Address
- Legacy segment model (real mode), protected mode, long mode.
- Linear Address
- Virtual Address
- Descriptor Tables: GDT, LDT, IDT

## OS Memory Layout

### Higher Half Kernel
#### Physical Address
```
0M  1M       4M               16M                                     4032M     4096M
┌───┬────────┬─────────────────┬┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┬─────────┐
│ R │ KERNEL │ AVAILABLE RAM   │ INACCESSIBLE                           │ DEVICE  │
└───┴────────┴─────────────────┴┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┴─────────┘
```
#### Virtual Address
```
0M                                    3840M 3841M                         4092M 4096M
┌───────────────────────────────────────┬───┬────────┬────────┬──────────────┬────┐
│ USERLAND                              │ R │ KERNEL | KSTACK | KHEAP        │ PT │
└───────────────────────────────────────┴───┴────────┴────────┴──────────────┴────┘
```
#### Why?
- https://wiki.osdev.org/Higher_Half_Kernel
- gcc [mcmodel=kernel](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html). Generate code for the kernel code model. The kernel runs in the negative 2 GB of the address space.

### Address space randomization
- https://en.wikipedia.org/wiki/Address_space_layout_randomization

## Paging
### How it work
- CR0, CR3
- Page Directory
- Page Table
- Page Fault
- https://wiki.osdev.org/Paging

### Identity Mapping
```
Physical Address
0M  1M       4M               16M                                     4032M     4096M
┌───┬────────┬─────────────────┬┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┬─────────┐
│ R │ KERNEL │ AVAILABLE RAM   │ INACCESSIBLE                           │ DEVICE  │
└───┴────────┴─────────────────┴┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┈┴─────────┘

Virtual Address
0M  1M       4M                       3840M 3841M                         4092M 4096M
┌───┬────────┬──────────────────────────┬───┬────────┬────────┬──────────────┬────┐
│ R │ KERNEL │                          │ R │ KERNEL | KSTACK | KHEAP        │ PT │
└───┴────────┴──────────────────────────┴───┴────────┴────────┴──────────────┴────┘
```
1. Map PMA 0M ~ 4M to VMA 0-4M and 3840M ~ 3844M.
2. Jump from low address to high address.
3. Un-map 0M ~ 4M.

### Frame Allocator
- Memory Map (aka BIOS E820).
- Bookkeeping of 4K pages.
- https://wiki.osdev.org/Page_Frame_Allocation

### Recursive Paging
The allocation of PD/PT itself rely on the paging system itself. The trick is demand paging and specially calculated address space layout.

### Other Techniques
#### Lazy Allocation, Demand Paging
- https://en.wikipedia.org/wiki/Demand_paging
#### CoW
- https://en.wikipedia.org/wiki/Copy-on-write
#### Swap
- https://forum.osdev.org/viewtopic.php?f=1&t=24536

### TLB
- https://wiki.osdev.org/TLB
- https://forum.osdev.org/viewtopic.php?f=1&t=27618


## High Level Memory Management
### Heap
- https://wiki.osdev.org/Memory_Allocation

### `kmalloc`
- Why not `malloc`? what about freestanding?
- https://en.wikipedia.org/wiki/Buddy_memory_allocation


## Reference
- https://wiki.osdev.org/Memory_Map_(x86)
- https://wiki.osdev.org/Paging
- https://wiki.osdev.org/Memory_management
- https://wiki.osdev.org/Page_Frame_Allocation
- https://www.kernel.org/doc/gorman/html/understand/understand011.html
- IA32 Manual Vol.1 `3.3.1 IA-32 Memory Models`
- IA32 Manual Vol.3 `3.4 LOGICAL AND LINEAR ADDRESSES`
- IA32 Manual Vol.3 `Chapter 4 PAGING`
