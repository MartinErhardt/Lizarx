SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))

ASFLAGS = 
CFLAGS =  -Werror -Wall -fno-rtti -fno-exceptions 

tst1.elf: $(OBJS)
	$(LD_CROSS) $(LDFLAGS) -o $@ $^ $(LD_CRT0_FLG)
	rm $(OBJS)

%.o: %.c
	$(CPPC_CROSS) $(CFLAGS) -c -o $@ $^
