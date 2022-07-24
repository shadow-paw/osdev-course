.PHONY: build clean

all: build
	@:

CROSS_AS=nasm
CROSS_GCC=i686-elf-gcc
CROSS_LD=i686-elf-ld
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
CROSS_LDFLAGS=-O2 -nostdlib
CROSS_AR=i686-elf-ar
CROSS_ARFLAGS=-rcs
CROSS_GDB=i686-elf-gdb

LIBC=../../libc/i686-elf/

APP_PATH_SRC=./
APP_PATH_OBJ=./
APP_PATH_BIN=../../bin/
APP_C:=$(sort $(wildcard $(APP_PATH_SRC)*.c))
APP_CPP:=$(sort $(wildcard $(APP_PATH_SRC)*.cpp))
APP_OBJ:=$(patsubst $(APP_PATH_SRC)%.c,$(APP_PATH_OBJ)%_c.o,$(APP_C)) \
         $(patsubst $(APP_PATH_SRC)%.cpp,$(APP_PATH_OBJ)%_cpp.o,$(APP_CPP))
APP_BIN=$(APP_PATH_BIN)testapp
APP_SYM=$(APP_PATH_BIN)testapp.sym

APP_DEP:=$(patsubst %.o,%.d,$(APP_OBJ))
-include $(APP_DEP)

build: $(APP_BIN)
	@:

$(APP_BIN): $(APP_OBJ) app.ld Makefile
	@echo "[LINK] $@"
	@$(CROSS_LD) $(CROSS_LDFLAGS) -Tapp.ld -o $@ ../../lib/i686/crt0.o $(APP_OBJ) -L../../lib/i686/ -lgloss -L$(LIBC)lib -lc
	@$(CROSS_OBJCOPY) --only-keep-debug $@ $(APP_SYM)
	@$(CROSS_OBJCOPY) --strip-debug --strip-unneeded $@

$(APP_PATH_OBJ)%_c.o: $(APP_PATH_SRC)%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_GCC) $(CROSS_CFLAGS) -I$(LIBC)include -I$(APP_PATH_SRC) -MD -MP -c $< -o $@

$(APP_PATH_OBJ)%_cpp.o: $(APP_PATH_SRC)%.cpp Makefile
	@echo "[C++ ] $<"
	@$(CROSS_GCC) $(CROSS_CPPFLAGS) -I$(LIBC)include -I$(APP_PATH_SRC) -MD -MP -c $< -o $@

clean:
	@-rm -f $(APP_OBJ) $(APP_DEP) $(APP_BIN) $(APP_SYM)
