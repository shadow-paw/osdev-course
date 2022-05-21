# 2. Boot Sequence

## Information
- [https://wiki.osdev.org/Boot_Sequence](https://wiki.osdev.org/Boot_Sequence)
- [https://developer.ibm.com/articles/l-linuxboot/](https://developer.ibm.com/articles/l-linuxboot/)


## GRUB & Multi-Boot Standard
In this lesson we will make a minimal boot image of our very first OS. The code will be copy from the [osdev example](https://wiki.osdev.org/Bare_Bones#Booting_the_Operating_System).


### Code Walkthrough
There will be 3 main files:

#### boot.s
This file contain 2 important things:
1. multiboot header, which tells GRUB2 how to load our kernel.
2. bootstrap code, which setup a minimal C runtime (freestanding) environment, e.g. stack.

#### main.c
Our main kernel code. It just print a message to the [VGA text memory](https://wiki.osdev.org/Text_UI).

#### kernel.ld
The linker script to tell linker where to put each sections (code, data, etc). Loading a program is not merely reading a file into memory, for instant, there are bss which we don't actually put a bunch of zeros in the file. The loader (ie. GRUB2) will use those information to put everything in place.

## Building Kernel
```
i686-elf-as boot.s -o boot.o
i686-elf-gcc -ffreestanding -O2 -std=gnu99 -Wall -Wextra -o main.o -c main.c
i686-elf-gcc -ffreestanding -O2 -nostdlib -T kernel.ld -o myos.bin -lgcc boot.o main.o
```

## Create ISO Image
`grub.cfg`
```
menuentry "myos" {
  multiboot /boot/myos.bin
}
```

```
mkdir -p dist/iso/boot/grub/
cp myos.bin dist/iso/boot/myos.bin
cp grub.cfg dist/iso/boot/grub/grub.cfg
grub-mkrescue -o myos.iso dist/iso
```

## Run It
```
qemu-system-i386 -cdrom myos.iso
```
To terminate, press `CTRL-ALT-2` (Linux) or `CTRL-OPTION-2` (Mac) then type quit. `CRTL-ALT-G`/`CTRL-OPTION-G` to release mouse.

### QEMU Over SSH
```
qemu-system-i386 -display curses -cdrom myos.iso
```
To terminate, press `ALT-2` then type `quit`.

## Reference
- [https://wiki.osdev.org/Printing_To_Screen](https://wiki.osdev.org/Printing_To_Screen)
- [https://www.gnu.org/software/grub/manual/multiboot/multiboot.html](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)
