BINUTILS_PATH_TO=../srcs/binutils-2.24
diff -rupN Makefile.orig.am		Makefile.am >binutils.patch
diff -rupN configure.orig.tgt		configure.tgt >> binutils.patch
diff -rupN config.orig.bfd		config.bfd >> binutils.patch
diff -rupN config.orig.sub		config.sub >> binutils.patch
diff -rupN configure_ld.orig.tgt	configure_ld.tgt >> binutils.patch
