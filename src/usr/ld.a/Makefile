all:
	$(CC_CROSS) -c ld.c -o ld.o -nostdinc -I lib_stat 
	$(CC_CROSS) -c lib_stat/st_string.c -o lib_stat/st_string.o -nostdinc 
	$(CC_CROSS) -c lib_stat/st_stdlib.c -o lib_stat/st_stdlib.o -nostdinc 
	$(CC_CROSS) -c lib_stat/video.c -o lib_stat/video.o -nostdinc 
	$(AR_CROSS) rcs libdld.a      ld.o lib_stat/st_string.o lib_stat/video.o lib_stat/st_stdlib.o
