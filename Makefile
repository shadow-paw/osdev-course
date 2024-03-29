.PHONY: build dist initrd run debug gdb clean

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
CROSS_AR=i686-elf-ar
CROSS_ARFLAGS=-rcs
CROSS_GDB=i686-elf-gdb

BASE_PATH_SRC=src/
BASE_PATH_LIB=lib/

build:
	@ARCH=i686 make kernel-build libgloss-build usercrt-build
dist: build
	@mkdir -p dist/iso/boot/grub dist/iso/boot/i686
	@cp src/boot/grub.cfg dist/iso/boot/grub/
# i686: kernel
	@cp $(KERNEL_PATH_BIN)kernel-i686.bin dist/iso/boot/i686/kernel.bin
# i686: initrd
	@mkdir -p dist/initrd/i686/
	@rsync -qavr src/initrd/i686/ dist/initrd/i686/
	@cp -f bin/testapp dist/initrd/i686/
	@find dist/initrd/i686/ -maxdepth 1 -printf "%P\n" | tar -C dist/initrd/i686/ -czf dist/iso/boot/i686/initrd --owner=0 --group=0 --no-same-owner --no-same-permissions -T -
	@grub-mkrescue -o dist/myos.iso dist/iso
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
clean:
	@ARCH=i686 make kernel-clean

# Kernel
###########################################################
KERNEL_PATH_SRC=$(BASE_PATH_SRC)kernel/
KERNEL_PATH_OBJ=obj/kernel/
KERNEL_PATH_BIN=bin/kernel/
KERNEL_CORE_ASM:=$(sort $(wildcard $(KERNEL_PATH_SRC)*.s))
KERNEL_CORE_C  :=$(sort $(wildcard $(KERNEL_PATH_SRC)*.c))
KERNEL_CORE_CPP:=$(sort $(wildcard $(KERNEL_PATH_SRC)*.cpp))
KERNEL_CORE_OBJ:=$(patsubst $(KERNEL_PATH_SRC)%.s,$(KERNEL_PATH_OBJ)%_s.o,$(KERNEL_CORE_ASM)) \
                 $(patsubst $(KERNEL_PATH_SRC)%.c,$(KERNEL_PATH_OBJ)%_c.o,$(KERNEL_CORE_C)) \
                 $(patsubst $(KERNEL_PATH_SRC)%.cpp,$(KERNEL_PATH_OBJ)%_cpp.o,$(KERNEL_CORE_CPP))
KERNEL_ARCH_PATH_SRC:=$(KERNEL_PATH_SRC)arch/$(ARCH)/
KERNEL_ARCH_PATH_OBJ:=$(KERNEL_PATH_OBJ)arch/$(ARCH)/
KERNEL_ARCH_ASM:=$(sort $(wildcard $(KERNEL_ARCH_PATH_SRC)*.s))
KERNEL_ARCH_C  :=$(sort $(wildcard $(KERNEL_ARCH_PATH_SRC)*.c))
KERNEL_ARCH_CPP:=$(sort $(wildcard $(KERNEL_ARCH_PATH_SRC)*.cpp))
KERNEL_ARCH_OBJ:=$(patsubst $(KERNEL_ARCH_PATH_SRC)%.s,$(KERNEL_ARCH_PATH_OBJ)%_s.o,$(KERNEL_ARCH_ASM)) \
                 $(patsubst $(KERNEL_ARCH_PATH_SRC)%.c,$(KERNEL_ARCH_PATH_OBJ)%_c.o,$(KERNEL_ARCH_C)) \
                 $(patsubst $(KERNEL_ARCH_PATH_SRC)%.cpp,$(KERNEL_ARCH_PATH_OBJ)%_cpp.o,$(KERNEL_ARCH_CPP))

DRIVERS_PATH_SRC=$(BASE_PATH_SRC)drivers/
DRIVERS_PATH_OBJ=obj/drivers/
DRIVERS_C  :=$(sort $(wildcard $(DRIVERS_PATH_SRC)**/*.c))
DRIVERS_OBJ:=$(patsubst $(DRIVERS_PATH_SRC)%.c,$(DRIVERS_PATH_OBJ)%_c.o,$(DRIVERS_C))

KERNEL_ALL_OBJ:=$(KERNEL_ARCH_OBJ) $(KERNEL_CORE_OBJ) $(DRIVERS_OBJ)
KERNEL_ALL_DEP:=$(patsubst %.o,%.d,$(KERNEL_ALL_OBJ))
KERNEL_BIN=$(KERNEL_PATH_BIN)kernel-$(ARCH).bin
KERNEL_SYM=$(KERNEL_BIN:.bin=.sym)

CRTBEGIN_OBJ:=$(shell $(CROSS_GCC) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CROSS_GCC) -print-file-name=crtend.o)

-include $(KERNEL_ALL_DEP)

kernel-build: $(KERNEL_BIN)
	@:

kernel-mkdir:
	@mkdir -p $(KERNEL_PATH_BIN) $(KERNEL_PATH_OBJ) $(KERNEL_ARCH_PATH_OBJ)

$(KERNEL_BIN): kernel-mkdir $(KERNEL_ALL_OBJ) $(KERNEL_ARCH_PATH_SRC)kernel.ld Makefile
	@echo "[LINK] $@"
	@$(CROSS_GCC) $(CROSS_LDFLAGS) \
	  -T $(KERNEL_ARCH_PATH_SRC)kernel.ld \
	  -lgcc \
	  -o $@ \
	  $(CRTBEGIN_OBJ) $(CRTEND_OBJ) $(KERNEL_ALL_OBJ)
	@$(CROSS_OBJCOPY) --only-keep-debug $@ $(KERNEL_SYM)
	@$(CROSS_OBJCOPY) --strip-debug --strip-unneeded $@

$(KERNEL_PATH_OBJ)%_s.o: $(KERNEL_PATH_SRC)%.s Makefile
	@echo "[AS  ] $<"
	@$(CROSS_AS) $(CROSS_ASMFLAGS) -I$(KERNEL_PATH_SRC) -I$(KERNEL_ARCH_PATH_SRC) -MD $(KERNEL_PATH_OBJ)$*_s.d -MP -o $@ $<

$(KERNEL_PATH_OBJ)%_c.o: $(KERNEL_PATH_SRC)%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_GCC) $(CROSS_CFLAGS) -I$(KERNEL_PATH_SRC) -I$(KERNEL_ARCH_PATH_SRC) -I$(BASE_PATH_SRC) -MD -MP -c $< -o $@

$(KERNEL_PATH_OBJ)%_cpp.o: $(KERNEL_PATH_SRC)%.cpp Makefile
	@echo "[C++ ] $<"
	@$(CROSS_GCC) $(CROSS_CPPFLAGS) -I$(KERNEL_PATH_SRC) -I$(KERNEL_ARCH_PATH_SRC) -I$(BASE_PATH_SRC) -MD -MP -c $< -o $@

$(KERNEL_ARCH_PATH_OBJ)%_s.o: $(KERNEL_ARCH_PATH_SRC)%.s Makefile
	@echo "[AS  ] $<"
	@$(CROSS_AS) $(CROSS_ASMFLAGS) -I$(KERNEL_ARCH_PATH_SRC) -MD $(KERNEL_ARCH_PATH_OBJ)$*_s.d -MP -o $@ $<

$(KERNEL_ARCH_PATH_OBJ)%_c.o: $(KERNEL_ARCH_PATH_SRC)%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_GCC) $(CROSS_CFLAGS) -I$(KERNEL_PATH_SRC) -I$(KERNEL_ARCH_PATH_SRC) -MD -MP -c $< -o $@

$(KERNEL_ARCH_PATH_OBJ)%_cpp.o: $(KERNEL_ARCH_PATH_SRC)%.cpp Makefile
	@echo "[C++ ] $<"
	@$(CROSS_GCC) $(CROSS_CPPFLAGS) -I$(KERNEL_PATH_SRC) -I$(KERNEL_ARCH_PATH_SRC) -MD -MP -c $< -o $@

$(DRIVERS_PATH_OBJ)%_c.o: $(DRIVERS_PATH_SRC)%.c Makefile
	@echo "[CC  ] $<"
	@mkdir -p $(dir $@)
	@$(CROSS_GCC) $(CROSS_CFLAGS) -I$(KERNEL_PATH_SRC) -I$(KERNEL_ARCH_PATH_SRC) -MD -MP -c $< -o $@

kernel-clean:
	@-rm $(KERNEL_ALL_OBJ) $(KERNEL_ALL_DEP) $(KERNEL_BIN) $(KERNEL_SYM)

# LIBGLOSS
###########################################################
LIBGLOSS_PATH_SRC=$(BASE_PATH_SRC)libgloss/$(ARCH)/
LIBGLOSS_PATH_OBJ=obj/libgloss/$(ARCH)/
LIBGLOSS_ASM:=$(sort $(wildcard $(LIBGLOSS_PATH_SRC)*.s))
LIBGLOSS_OBJ:=$(patsubst $(LIBGLOSS_PATH_SRC)%.s,$(LIBGLOSS_PATH_OBJ)%_s.o,$(LIBGLOSS_ASM))
LIBGLOSS_DEP:=$(patsubst %.o,%.d,$(LIBGLOSS_OBJ))
LIBGLOSS_LIB=$(BASE_PATH_LIB)/$(ARCH)/libgloss.a

-include $(LIBGLOSS_DEP)

libgloss-build: $(LIBGLOSS_LIB)
	@:

libgloss-mkdir:
	@mkdir -p $(BASE_PATH_LIB)/$(ARCH)/ $(LIBGLOSS_PATH_OBJ)

$(LIBGLOSS_LIB): libgloss-mkdir $(LIBGLOSS_OBJ) Makefile
	@echo "[AR] $@"
	@$(CROSS_AR) $(CROSS_ARFLAGS) $@ $(LIBGLOSS_OBJ)

$(LIBGLOSS_PATH_OBJ)%_s.o: $(LIBGLOSS_PATH_SRC)%.s Makefile
	@echo "[AS  ] $<"
	@$(CROSS_AS) $(CROSS_ASMFLAGS) -MD $(LIBGLOSS_PATH_OBJ)$*_s.d -MP -o $@ $<

libgloss-clean:
	@-rm $(LIBGLOSS_PATH_OBJ) $(LIBGLOSS_DEP) $(LIBGLOSS_LIB)


# LIBGLOSS
###########################################################
USERCRT_PATH_SRC=$(BASE_PATH_SRC)user_crt/$(ARCH)/
USERCRT_PATH_OBJ=obj/user_crt/$(ARCH)/
USERCRT_ASM:=$(sort $(wildcard $(USERCRT_PATH_SRC)*.s))
USERCRT_OBJ:=$(patsubst $(USERCRT_PATH_SRC)%.s,$(USERCRT_PATH_OBJ)%.o,$(USERCRT_ASM))
USERCRT_DEP:=$(patsubst %.o,%.d,$(USERCRT_OBJ))

usercrt-build: usercrt-mkdir usercrt-lib
	@:

usercrt-mkdir:
	@mkdir -p $(BASE_PATH_LIB)$(ARCH)/ $(USERCRT_PATH_OBJ)

usercrt-lib: $(USERCRT_OBJ) Makefile
	@cp -f $(USERCRT_OBJ) $(BASE_PATH_LIB)$(ARCH)/

$(USERCRT_PATH_OBJ)%.o: $(USERCRT_PATH_SRC)%.s Makefile
	@echo "[AS  ] $<"
	@$(CROSS_AS) $(CROSS_ASMFLAGS) -MD $(USERCRT_PATH_OBJ)$*_s.d -MP -o $@ $<

usercrt-clean:
	@-rm $(USERCRT_OBJ) $(USERCRT_DEP)
