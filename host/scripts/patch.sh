 
patch -p1 -d mpc-${MPC_VER} < ../patches/mpc.patch
patch -p1 -d gmp-${GMP_VER} < ../patches/gmp.patch
patch -p1 -d mpfr-${MPFR_VER} < ../patches/mpfr.patch
patch -p1 -d binutils-${BINUTILS_VER} < ../patches/binutils.patch
patch -p1 -d gcc-${GCC_VER} < ../patches/gcc.patch
patch -p1 -d newlib-${NEWLIB_VER} < ../patches/newlib.patch
