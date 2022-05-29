# 1. Course Introduction, Toolchain Setup

## Course Introduction

OS development is one of the holy grail and romance for developer. In this course you will explore the internals of computer which you have never seen before. In the end you will have your own toy OS added into your portfolio which looks pretty cool!

### Worth Mentioning
- [https://wiki.osdev.org/Required_Knowledge](https://wiki.osdev.org/Required_Knowledge)
- [https://wiki.osdev.org/Beginner_Mistakes](https://wiki.osdev.org/Beginner_Mistakes)


## Prerequisite
While you can use Apple Silicon MacBook, an x86-64 Linux box is recommended. Using Apple Silicon for example, will put you in a test pilot position especially some tools may not work well. Having different architectures might also create extra complexity for debugging.

## Toolchains
For kernel development, we shall build a cross-compiler so that we would not accidentially use something from the host environment instead of the implementation by your OS. We will use GCC in this course but there are also alternatives like LLVM.
See [Crosss Compiler](https://wiki.osdev.org/GCC_Cross-Compiler) for more information.

### Dependency packages
#### Ubuntu
```
sudo apt-get install -y build-essential curl libgmp3-dev libmpfr-dev libmpc-dev zlib1g-dev texinfo libexpat-dev
```
#### Mac
```
brew install gmp mpfr libmpc texinfo
```

### nasm
#### Ubuntu
```
sudo apt-get install -y nasm
```
#### Mac
```
brew install nasm
```

### Binutils
```
#!/bin/bash -e

INSTALL_PATH="$HOME/.local/cross"
BINUTILS_VERSION=2.38

if [ ! -f binutils-${BINUTILS_VERSION}.tar.bz2 ]; then
  curl -L http://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.bz2 -O
fi
if [ ! -d binutils-${BINUTILS_VERSION} ]; then
  tar xjf binutils-${BINUTILS_VERSION}.tar.bz2
fi
for TARGET in i686-elf x86_64-elf arm-none-eabi aarch64-none-elf
do
  (
    echo "[] binutils-${BINUTILS_VERSION}: ${TARGET}"
    mkdir -p build-binutils-${BINUTILS_VERSION}-${TARGET}
    cd build-binutils-${BINUTILS_VERSION}-${TARGET}
    ../binutils-${BINUTILS_VERSION}/configure \
      --prefix="${INSTALL_PATH}" \
      --target="${TARGET}" \
      --with-sysroot \
      --enable-interwork --enable-multilib \
      --disable-nls --disable-werror
    make -j8
    make install
  )
done
```

### GCC
```
#!/bin/bash -e

INSTALL_PATH="$HOME/.local/cross"
GCC_VERSION=12.1.0

if [ ! -f gcc-${GCC_VERSION}.tar.xz ]; then
  curl -L http://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz -O
fi
if [ ! -d gcc-${GCC_VERSION} ]; then
  tar xf gcc-${GCC_VERSION}.tar.xz
fi
for TARGET in i686-elf x86_64-elf arm-none-eabi aarch64-none-elf
do
  (
    echo "[] gcc-${GCC_VERSION}: ${TARGET}"
    mkdir -p build-gcc-${GCC_VERSION}-${TARGET}
    cd build-gcc-${GCC_VERSION}-${TARGET}
    ../gcc-${GCC_VERSION}/configure \
      --prefix="${INSTALL_PATH}" \
      --target="${TARGET}" \
      --enable-language=c,c++ --without-headers \
      --enable-interwork --enable-multilib \
      --disable-bootstrap --disable-libmpx \
      --disable-nls \
      --with-system-zlib
    make -j8 all-gcc
    make -j8 all-target-libgcc
    make install-gcc
    make install-target-libgcc
  )
done
```
> 💡 On Mac, add `--with-gmp=/opt/homebrew --with-mpfr=/opt/homebrew --with-mpc=/opt/homebrew` to configure options.

### GDB
```
#!/bin/bash -e

INSTALL_PATH="$HOME/.local/cross"
GDB_VERSION=12.1

if [ ! -f gdb-${GDB_VERSION}.tar.xz ]; then
  curl -L http://ftp.gnu.org/gnu/gdb/gdb-${GDB_VERSION}.tar.xz -O
fi
if [ ! -d gdb-${GDB_VERSION} ]; then
  tar xf gdb-${GDB_VERSION}.tar.xz
fi
for TARGET in i686-elf x86_64-elf arm-none-eabi aarch64-none-elf
do
  (
    echo "[] gdb-${GDB_VERSION}: ${TARGET}"
    mkdir -p build-gdb-${GDB_VERSION}-${TARGET}
    cd build-gdb-${GDB_VERSION}-${TARGET}
    ../gdb-${GDB_VERSION}/configure \
      --prefix="${INSTALL_PATH}" \
      --target="${TARGET}" \
      --program-prefix="${TARGET}-" \
      --with-gmp=/usr/local \
      --with-libelf=/usr/local \
      --with-build-libsubdir=/usr/local \
      CFLAGS="-I/usr/local/include"
    make -j8
    make install
  )
done
```
> 💡 On Mac, add `--with-libgmp-prefix=/opt/homebrew` to configure options, and remove the ARM targets.

### QEMU
#### Ubuntu
```
sudo apt-get install qemu-system-x86 qemu-system-arm
```
#### Mac
```
brew install qemu
```

### GRUB2
💡 It is not recommended to mess with boot loader on your working machine. You should be very careful when working with GRUB and make sure it apply on a disk image only.

#### Ubuntu
```
sudo apt-get install grub2-common grub-pc-bin xorriso
```
#### Mac
GRUB2 is not supported on mac, to use the grub tools, one way is to use docker to launch an ubuntu and run the grub tools inside:
```
docker run -it --platform linux/amd64 --name osdev -v "$PWD:/mnt/data" ubuntu:20.04 bash
  apt-get update && apt-get install -y grub2-common grub-pc-bin xorriso
  grub-mkrescue -o /mnt/data/myos.iso /mnt/data/isodir
docker stop osdev
```

### Export PATH
```
export PATH=$PATH:$HOME/.local/cross/bin
```

## Useful Resources
- [https://wiki.osdev.org/](https://wiki.osdev.org/)
- [https://forum.osdev.org/](https://forum.osdev.org/)
- [Intel® 64 and IA-32 Architectures Software Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [Arm Architecture Reference Manual](https://developer.arm.com/documentation/ddi0487/ha/)
