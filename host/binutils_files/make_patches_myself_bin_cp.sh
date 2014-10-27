BINUTILS_PATH_TO=../srcs/binutils-2.24
cp $BINUTILS_PATH_TO/ld/Makefile.am	Makefile.am
cp $BINUTILS_PATH_TO/gas/configure.tgt	configure.tgt
cp $BINUTILS_PATH_TO/bfd/config.bfd	config.bfd
cp $BINUTILS_PATH_TO/config.sub		config.sub
cp $BINUTILS_PATH_TO/ld/configure.tgt	configure_ld.tgt

cp $BINUTILS_PATH_TO/ld/Makefile.am	Makefile.orig.am
cp $BINUTILS_PATH_TO/gas/configure.tgt	configure.orig.tgt
cp $BINUTILS_PATH_TO/bfd/config.bfd	config.orig.bfd
cp $BINUTILS_PATH_TO/config.sub		config.orig.sub
cp $BINUTILS_PATH_TO/ld/configure.tgt	configure_ld.orig.tgt
