NCPU=$(grep -c ^processor /proc/cpuinfo)

BINUTILS_VER=2.24
GCC_VER=4.9.1
GMP_VER=6.0.0
MPFR_VER=3.1.2
MPC_VER=1.0.2
NEWLIB_VER=2.1.0

AUTOCONF_VER=2.68
AUTOMAKE_VER=1.11.6
title() {
		echo -en "\033]0;$@\007"
}

setphase() {
		title $1
		echo ">>>>>>> $1"
}
