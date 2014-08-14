#!/bin/bash
QEMU_VER=2.1.0
TOOLCHAIN_DIR=$(pwd)
mkdir -p toolchain/host
cd build
rm -rf qemu
rm -rf qemu-obj
mkdir -p qemu-obj
mkdir -p qemu
if [ ! -f ./qemu.tar.bz2  ]
then
	curl -o qemu.tar.bz2 http://wiki.qemu-project.org/download/qemu-${QEMU_VER}.tar.bz2
fi
mkdir qemu_2
tar -xf qemu.tar.bz2
mv qemu qemu_2
mv qemu_2/qemu/qemu-${QEMU_VER} qemu
rm -rf qemu_2
mv qemu-${QEMU_VER} qemu
cd qemu-obj
${TOOLCHAIN_DIR}/build/qemu/configure --disable-kvm --python=/usr/bin/python2 --prefix=${TOOLCHAIN_DIR}/toolchain/host --target-list="i386-softmmu x86_64-softmmu" --enable-debug
make -j4
make install
cd ..
