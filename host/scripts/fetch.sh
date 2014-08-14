setphase "FETCH BINUTILS"
wget -c http://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VER}.tar.bz2
tar -xf binutils-${BINUTILS_VER}.tar.bz2

setphase "FETCH GCC"
wget -c http://ftp.gnu.org/gnu/gcc/gcc-${GCC_VER}/gcc-${GCC_VER}.tar.gz
tar -xf gcc-${GCC_VER}.tar.gz

setphase "FETCH GMP"

wget -c ftp://ftp.gmplib.org/pub/gmp-${GMP_VER}/gmp-${GMP_VER}.tar.bz2
tar -xf gmp-${GMP_VER}.tar.bz2

setphase "FETCH MPFR"
wget -c http://ftp.gnu.org/gnu/mpfr/mpfr-${MPFR_VER}.tar.gz
tar -xf mpfr-${MPFR_VER}.tar.gz

setphase "FETCH MPC"
wget -c http://www.multiprecision.org/mpc/download/mpc-${MPC_VER}.tar.gz
tar -xf mpc-${MPC_VER}.tar.gz

setphase "FETCH NEWLIB"
wget -c  ftp://sourceware.org/pub/newlib/newlib-${NEWLIB_VER}.tar.gz
tar -xf newlib-${NEWLIB_VER}.tar.gz

setphase "FETCH AUTOCONF"
wget -c ftp://ftp.gnu.org/gnu/autoconf/autoconf-${AUTOCONF_VER}.tar.gz -O autoconf-${AUTOCONF_VER}.tar.gz
tar -xf autoconf-${AUTOCONF_VER}.tar.gz

setphase "FETCH AUTOMAKE"
wget -c ftp://ftp.gnu.org/gnu/automake/automake-${AUTOMAKE_VER}.tar.gz -O automake-${AUTOMAKE_VER}.tar.gz
tar -xf automake-${AUTOMAKE_VER}.tar.gz
