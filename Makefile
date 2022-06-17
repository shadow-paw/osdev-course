.PHONY: build dist run debug gdb clean

CROSS_AS=nasm
CROSS_GCC=i686-elf-gcc
CROSS_OBJCOPY=i686-elf-objcopy
CROSS_ASMFLAGS=-felf32 -g
CROSS_CFLAGS=-ffreestanding -std=c11 -masm=intel \
             -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
			 -O2 -Wall -Wextra -g
CROSS_CPPFLAGS=-ffreestanding -std=c++17 -masm=intel \
			   -fno-use-cxa-atexit \
               -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
               -fno-exceptions -fno-rtti -fno-stack-protector \
			   -O2 -Wall -Wextra -g
CROSS_LDFLAGS=-fPIC -ffreestanding -O2 -nostdlib
CROSS_GDB=i686-elf-gdb

build: kernel-build
	@:
dist: kernel-dist
	@sync
run: dist
	@qemu-system-i386 -m 32 -display curses -cdrom dist/myos.iso
debug: dist
	@qemu-system-i386 -s -S -m 32 -display curses -cdrom dist/myos.iso
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
KERNEL_FILE_ASM:=$(sort $(wildcard $(KERNEL_PATH_SRC)*.s))
KERNEL_FILE_C:=$(sort $(wildcard $(KERNEL_PATH_SRC)*.c))
KERNEL_FILE_CPP:=$(sort $(wildcard $(KERNEL_PATH_SRC)*.cpp))
KERNEL_FILE_O:=$(patsubst $(KERNEL_PATH_SRC)%.s,$(KERNEL_PATH_OBJ)%_s.o,$(KERNEL_FILE_ASM)) \
               $(patsubst $(KERNEL_PATH_SRC)%.c,$(KERNEL_PATH_OBJ)%_c.o,$(KERNEL_FILE_C)) \
               $(patsubst $(KERNEL_PATH_SRC)%.cpp,$(KERNEL_PATH_OBJ)%_cpp.o,$(KERNEL_FILE_CPP))
KERNEL_FILE_DEP:=$(patsubst %.o,%.d,$(KERNEL_FILE_O))
KERNEL_BIN=$(KERNEL_PATH_BIN)myos.bin
KERNEL_SYM=$(KERNEL_BIN:.bin=.sym)

CRTBEGIN_OBJ:=$(shell $(CROSS_GCC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CROSS_GCC) -print-file-name=crtend.o)

-include $(KERNEL_FILE_DEP)

kernel-build: $(KERNEL_BIN)
	@:

kernel-mkdir:
	@mkdir -p $(KERNEL_PATH_OBJ) $(KERNEL_PATH_BIN)

$(KERNEL_BIN): kernel-mkdir $(KERNEL_FILE_O) src/kernel/kernel.ld Makefile
	@echo "[LINK] $@"
	@$(CROSS_GCC) $(CROSS_LDFLAGS) \
	  -T src/kernel/kernel.ld \
	  -lgcc \
	  -o $@ \
	  $(CRTBEGIN_OBJ) $(CRTEND_OBJ) $(KERNEL_FILE_O)
	@$(CROSS_OBJCOPY) --only-keep-debug $@ $(KERNEL_SYM)
	@$(CROSS_OBJCOPY) --strip-debug --strip-unneeded $@

$(KERNEL_PATH_OBJ)%_s.o: $(KERNEL_PATH_SRC)%.s Makefile
	@echo "[AS  ] $<"
	@$(CROSS_AS) $(CROSS_ASMFLAGS) -I$(KERNEL_PATH_SRC) -MD $(KERNEL_PATH_OBJ)$*_s.d -MP -o $@ $<

$(KERNEL_PATH_OBJ)%_c.o: $(KERNEL_PATH_SRC)%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_GCC) $(CROSS_CFLAGS) -MD -MP -c $< -o $@

$(KERNEL_PATH_OBJ)%_cpp.o: $(KERNEL_PATH_SRC)%.cpp Makefile
	@echo "[C++ ] $<"
	@$(CROSS_GCC) $(CROSS_CPPFLAGS) -MD -MP -c $< -o $@

kernel-dist: kernel-build
	@mkdir -p dist/iso/boot/grub
	@cp src/boot/grub.cfg dist/iso/boot/grub/
	@cp $(KERNEL_BIN) dist/iso/boot/
	@grub-mkrescue -o dist/myos.iso dist/iso

kernel-clean:
	@rm $(KERNEL_FILE_O) $(KERNEL_BIN) $(KERNEL_SYM)
