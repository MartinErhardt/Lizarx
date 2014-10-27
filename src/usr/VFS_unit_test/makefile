SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

ASFLAGS = 
CFLAGS =  -Werror -Wall -fno-rtti -fno-exceptions 
tst1.elf: $(OBJS)
	$(CPPC_CROSS) -o $@ $^ -L../hlib -lhelper 
	rm $(OBJS)

%.o: %.c
	$(CPPC_CROSS) $(CFLAGS) -I ../arch/$(ARCH2) -I ../hlib  -c -o $@ $^
