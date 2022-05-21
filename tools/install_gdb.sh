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
