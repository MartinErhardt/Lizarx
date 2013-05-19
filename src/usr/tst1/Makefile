SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CFLAGS = -Werror -Wall
tst1.elf:
	$(CC_CROSS) $(CFLAGS) -o tst1.elf tst.c