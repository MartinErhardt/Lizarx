SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CC = gcc
LD = ld

ASFLAGS = -m32
CFLAGS = -m32 -Werror -Wall -g -fno-stack-protector -nostdinc -I lib -ffreestanding
LDFLAGS = -melf_i386 -Ttst3.ld

tst3.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^
	rm $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^
