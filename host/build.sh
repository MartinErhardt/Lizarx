. scripts/config.sh
#TARGET=x86_64-pc-lizarx
TARGET=$1
export MAKEINFO=makeinfo-4.13a make
export COMPILER_PATH=$(pwd)/$TOOLCHAIN_DIR/toolchain/$TARGET/bin/$TARGET-gcc
export PATH=$PATH:$COMPILER_PATH
export LD_CROSS=$(pwd)/toolchain/$TARGET/bin/$TARGET-ld
export LD32_CROSS=$(pwd)/toolchain/i386-pc-lizarx/bin/i386-pc-lizarx-ld
export CC_CROSS=$(pwd)/toolchain/$TARGET/bin/$TARGET-gcc
export CPPC_CROSS=$(pwd)/toolchain/$TARGET/bin/$TARGET-g++
export AR_CROSS=$(pwd)/toolchain/$TARGET/bin/$TARGET-ar
export INCLUDEDIR=$(pwd)/toolchain/$TARGET/$TARGET/include
export LIBDIR=$(pwd)/toolchain/$TARGET/$TARGET/lib
if [ "$TARGET" == "x86_64-pc-lizarx" ]
then
	export ARCH_SUBDIR="arch/x86_64"
	export ARCH_MACHINE_HEADERS=$ARCH_SUBDIR/machine/fpu.h
	export GCC_VER=4.5.4
else
	export ARCH_SUBDIR="arch/i387"
	export ARCH_MACHINE_HEADERS=$ARCH_SUBDIR/machine/npx.h
fi
mkdir -p toolchain
export PREFIX=`pwd`/toolchain/$TARGET
echo $PREFIX
mkdir -p $PREFIX
export PATH=$PREFIX/bin:$PATH
mkdir -p build 
cd build
rm -rf binutils-obj
rm -rf gcc-obj
rm -rf gmp-obj
rm -rf mpfr-obj
rm -rf mpc-obj
rm -rf newlib-obj
rm -rf autoconf-obj
rm -rf automake-obj
#rm -rf $PREFIX
mkdir -p binutils-obj
mkdir -p gcc-obj
mkdir -p gmp-obj
mkdir -p mpfr-obj
mkdir -p mpc-obj
mkdir -p newlib-obj
mkdir -p autoconf-obj
mkdir -p automake-obj
. ../scripts/fetch.sh
. ../scripts/patch.sh
cp ../gcc_files/gcc/config/* gcc-${GCC_VER}/gcc/config/ || exit
# --- Compile all packages ---
cp ../binutils_files/ld/emulparams/* ./binutils-${BINUTILS_VER}/ld/emulparams/
rm -rf ./newlib-${NEWLIB_VER}/newlib/libc/sys/lizarx
cp -r ../newlib_files-$TARGET ./newlib-${NEWLIB_VER}/newlib/libc/sys/lizarx || exit
mkdir -p $INCLUDEDIR
cp -r newlib-2.1.0/newlib/libc/include $INCLUDEDIR
setphase "COMPILE BINUTILS"
cd binutils-obj
../binutils-${BINUTILS_VER}/configure --target=$TARGET --prefix=$PREFIX --disable-werror --enable-gold --enable-plugins || exit
make -j$NCPU all-gold || exit
make -j$NCPU || exit
make install || exit
cd ..
setphase "COMPILE GMP"
cd gmp-obj
../gmp-${GMP_VER}/configure --prefix=$PREFIX --enable-cxx --disable-shared || exit
make -j$NCPU || exit
make install || exit
cd ..

setphase "COMPILE MPFR"
cd mpfr-obj
../mpfr-${MPFR_VER}/configure --prefix=$PREFIX --with-gmp=$PREFIX --disable-shared
make -j$NCPU || exit
make install || exit
cd ..

setphase "COMPILE MPC"
cd mpc-obj
../mpc-${MPC_VER}/configure --target=$TARGET --prefix=$PREFIX --with-gmp=$PREFIX --with-mpfr=$PREFIX --disable-shared || exit
make -j$NCPU || exit
make install || exit
cd ..
export MAKEINFO=makeinfo-4.13a make
setphase "COMPILE GCC"
cd gcc-obj
../gcc-${GCC_VER}/configure --target=$TARGET --prefix=$PREFIX --enable-languages=c,c++ --without-docdir --without-headers --disable-libssp --with-gmp=$PREFIX --with-mpfr=$PREFIX --with-mpc=$PREFIX --disable-nls --enable-shared --with-newlib || exit
export MAKEINFO=makeinfo-4.13a make
make -j$NCPU all-gcc || exit
make install-gcc || exit
cd ..
setphase "COMPILE AUTOCONF"
cd autoconf-obj
../autoconf-${AUTOCONF_VER}/configure --prefix=$PREFIX || exit
make -j$NCPU all || exit
make install || exit
cd ..

setphase "COMPILE AUTOMAKE"
cd automake-obj
../automake-${AUTOMAKE_VER}/configure --prefix=$PREFIX || exit
make -j$NCPU all || exit
make install || exit
cd ..

cd ../toolchain/$TARGET/share
ln -s aclocal-1.11 aclocal
cd ../../../build

hash -r

setphase "AUTOCONF NEWLIB"
cd newlib-${NEWLIB_VER}/newlib/libc/sys
autoconf || exit
cd lizarx
autoreconf || exit
cd ../../../../..

setphase "COMPILE NEWLIB"
cd newlib-obj
../newlib-${NEWLIB_VER}/configure --target=$TARGET --prefix=$PREFIX --with-gmp=$PREFIX --with-mpfr=$PREFIX -enable-newlib-hw-fp || exit
make -j$NCPU || exit
make install || exit
cd ..

setphase "PASS-2 COMPILE GCC"
cd gcc-obj
make all-target-libgcc || exit
make install-target-libgcc || exit
make -j$NCPU all-target-libstdc++-v3 || exit
make install-target-libstdc++-v3 || exit
make -j$NCPU all || exit
make install || exit
cd ../..

#mkdir -p $INCLUDEDIR
#setphase "COMPILE LIBS"
#cd ../../../src/usr/libc || exit
#make || exit
#cd ../libm
#make || exit
#cd ../../../.. || exit
cp toolchain/$TARGET/lib/gcc/$TARGET/${GCC_VER}/libgcc.a toolchain/$TARGET/$TARGET/lib/libgcc.a
cp toolchain/$TARGET/lib/gcc/$TARGET/${GCC_VER}/libgcov.a  toolchain/$TARGET/$TARGET/lib/libgcov.a
