SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))
export COMPILER_PATH = /home/martin/git/lizarx/Lizarx/src/usr/buildtools/local/bin
export PATH := /home/martin/git/lizarx/Lizarx/src/usr/buildtools/local/lib

CC = ../buildtools/local/bin/i386-pc-lizarx-gcc 
LD = ../buildtools/local/bin/i386-pc-lizarx-ld 

CFLAGS = -Werror -Wall
tst1.elf:
	$(CC) $(CFLAGS) -o tst1.elf tst.c
