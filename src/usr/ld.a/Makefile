all:
	$(CC_CROSS) -c ld.c -o ld.o -nostdinc -I lib_stat 
	$(CC_CROSS) -c lib_stat/st_string.c -o lib_stat/st_string.o -nostdinc 
	$(AR_CROSS) rcs libdld.a      ld.o lib_stat/st_string.o
