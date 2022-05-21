.PHONY: build dist run clean

AS=i686-elf-as
GCC=i686-elf-gcc
CFLAGS=-ffreestanding -std=gnu99 -O2 -Wall -Wextra
LDFLAGS=-ffreestanding -O2 -nostdlib


build: kernel-build
	@:
dist: kernel-dist
	@:
run: dist
	@qemu-system-i386 -display curses -cdrom dist/myos.iso
clean: kernel-clean
	@:

# Kernel
###########################################################
KERNEL_PATH_SRC=src/kernel/
KERNEL_PATH_OBJ=obj/kernel/
KERNEL_PATH_BIN=bin/kernel/
KERNEL_FILE_C:=$(sort $(wildcard $(KERNEL_PATH_SRC)*.c))
KERNEL_FILE_O:=$(patsubst $(KERNEL_PATH_SRC)%.c,$(KERNEL_PATH_OBJ)%_c.o,$(KERNEL_FILE_C))
KERNEL_BIN=$(KERNEL_PATH_BIN)myos.bin

kernel-build: $(KERNEL_BIN)
	@:

kernel-mkdir:
	@mkdir -p $(KERNEL_PATH_OBJ) $(KERNEL_PATH_BIN)

$(KERNEL_BIN): kernel-mkdir $(KERNEL_PATH_OBJ)boot.o $(KERNEL_FILE_O) Makefile
	@echo "[LINK] $@"
	@$(GCC) $(LDFLAGS) \
	  -T src/kernel/kernel.ld \
	  -lgcc \
	  -o $@ \
	  $(KERNEL_PATH_OBJ)boot.o $(KERNEL_FILE_O)

$(KERNEL_PATH_OBJ)boot.o: $(KERNEL_PATH_SRC)boot.s Makefile
	@echo "[AS  ] $<"
	@$(AS) -o $@ $<

$(KERNEL_PATH_OBJ)%_c.o: $(KERNEL_PATH_SRC)%.c Makefile
	@echo "[CC  ] $<"
	@$(GCC) $(CFLAGS) -c $< -o $@

kernel-dist: kernel-build
	@mkdir -p dist/iso/boot/grub
	@cp src/boot/grub.cfg dist/iso/boot/grub/
	@cp $(KERNEL_BIN) dist/iso/boot/
	@grub-mkrescue -o dist/myos.iso dist/iso

kernel-clean:
	@rm $(KERNEL_FILE_O) $(KERNEL_BIN)
