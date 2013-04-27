#!/bin/bash
#if [ ! -f buildtools/build/gcc-obj/gcc/gcc-cross ]
#then
    cd buildtools
#    ./build.sh
    cd ..
#fi
export COMPILER_PATH=$(pwd)/buildtools/build/gcc-obj/gcc
export PATH=$PATH:$COMPILER_PATH
chmod 771 $COMPILER_PATH/cc1
chmod 771 $COMPILER_PATH/gcc-cross
chmod 771 buildtools/build/binutils-obj/ld/ld-new
