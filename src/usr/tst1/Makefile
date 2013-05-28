SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

ASFLAGS = -m32
CFLAGS = -m32 -Werror -Wall -fno-rtti -fno-exceptions 

tst1.elf: $(OBJS)
	$(LD_CROSS) $(LDFLAGS) -o $@ $^
	rm $(OBJS)

%.o: %.c
	$(CPPC_CROSS) $(CFLAGS) -c -o $@ $^
