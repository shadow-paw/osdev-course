#!/bin/bash -e

NEWLIB_VERSION=4.2.0.20211231

function main {
  if [ ! -f newlib-${NEWLIB_VERSION}.tar.gz ]; then
    curl -L ftp://sourceware.org/pub/newlib/newlib-${NEWLIB_VERSION}.tar.gz -O
  fi
  if [ ! -d newlib-${NEWLIB_VERSION} ]; then
    tar xzf newlib-${NEWLIB_VERSION}.tar.gz
  fi
  for TARGET in i686-elf x86_64-elf
  do
  (
    echo "[] newlib-${NEWLIB_VERSION}: ${TARGET}"
    mkdir -p build-newlib-${NEWLIB_VERSION}-${TARGET}
    cd build-newlib-${NEWLIB_VERSION}-${TARGET}
    ../newlib-${NEWLIB_VERSION}/configure \
      --prefix="`pwd`/../../libc/" \
      --target="${TARGET}"
    make -j8
    make install
  )
  done
}

BASEDIR=$(cd $(dirname "$0") && pwd)
(
    cd "${BASEDIR}"
    main $@
)
