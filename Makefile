.PHONY: build dist run debug gdb clean

CROSS_AS=i686-elf-as
CROSS_GCC=i686-elf-gcc
CROSS_OBJCOPY=i686-elf-objcopy
CROSS_CFLAGS=-ffreestanding -std=gnu99 -O2 -Wall -Wextra -g
CROSS_LDFLAGS=-ffreestanding -O2 -nostdlib
CROSS_GDB=i686-elf-gdb

build: kernel-build
	@:
dist: kernel-dist
	@:
run: dist
	@qemu-system-i386 -display curses -cdrom dist/myos.iso
debug: dist
	@qemu-system-i386 -s -S -display curses -cdrom dist/myos.iso
gdb:
	@$(CROSS_GDB) \
	  --eval-command="set disassembly-flavor intel" \
	  --eval-command="set architecture i386" \
	  --eval-command="target remote localhost:1234" \
	  --eval-command="set history save on" \
	  --symbols=$(KERNEL_SYM)
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
KERNEL_SYM=$(KERNEL_BIN:.bin=.sym)

kernel-build: $(KERNEL_BIN)
	@:

kernel-mkdir:
	@mkdir -p $(KERNEL_PATH_OBJ) $(KERNEL_PATH_BIN)

$(KERNEL_BIN): kernel-mkdir $(KERNEL_PATH_OBJ)boot.o $(KERNEL_FILE_O) Makefile
	@echo "[LINK] $@"
	@$(CROSS_GCC) $(CROSS_LDFLAGS) \
	  -T src/kernel/kernel.ld \
	  -lgcc \
	  -o $@ \
	  $(KERNEL_PATH_OBJ)boot.o $(KERNEL_FILE_O)
	@$(CROSS_OBJCOPY) --only-keep-debug $@ $(KERNEL_SYM)
	@$(CROSS_OBJCOPY) --strip-debug --strip-unneeded $@

$(KERNEL_PATH_OBJ)boot.o: $(KERNEL_PATH_SRC)boot.s Makefile
	@echo "[AS  ] $<"
	@$(CROSS_AS) -o $@ $<

$(KERNEL_PATH_OBJ)%_c.o: $(KERNEL_PATH_SRC)%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_GCC) $(CROSS_CFLAGS) -c $< -o $@

kernel-dist: kernel-build
	@mkdir -p dist/iso/boot/grub
	@cp src/boot/grub.cfg dist/iso/boot/grub/
	@cp $(KERNEL_BIN) dist/iso/boot/
	@grub-mkrescue -o dist/myos.iso dist/iso

kernel-clean:
	@rm $(KERNEL_FILE_O) $(KERNEL_BIN) $(KERNEL_SYM)
