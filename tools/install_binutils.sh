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
