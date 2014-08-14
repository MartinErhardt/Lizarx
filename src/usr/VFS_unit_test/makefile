SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

ASFLAGS = 
CFLAGS =  -Werror -Wall -fno-rtti -fno-exceptions 
LD_FLAGS=$(LIBDIR)/crt0.o -lc
tst1.elf: $(OBJS)
	$(LD_CROSS) -o $@ $^ $(LD_FLAGS)
	rm $(OBJS)

%.o: %.c
	$(CPPC_CROSS) $(CFLAGS) -c -o $@ $^
