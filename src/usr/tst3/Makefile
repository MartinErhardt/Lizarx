SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

ASFLAGS = -m32
CFLAGS = -m32 -Werror -Wall -g -fno-stack-protector -nostdinc -I lib -ffreestanding
LDFLAGS =  -Ttst3.ld

tst3.elf: $(OBJS)
	$(LD_CROSS) $(LDFLAGS) -o $@ $^
	rm $(OBJS)

%.o: %.c
	$(CC_CROSS) $(CFLAGS) -c -o $@ $^
