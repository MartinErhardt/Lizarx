SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))
all:
	$(CPPC_CROSS) -Wall -fpic foo.c -c -fno-rtti -fno-exceptions 
	$(CPPC_CROSS) -shared -o libSO_example_lib.so foo.o -fno-rtti -fno-exceptions 
	$(CPPC_CROSS) -Wall -o SO_example_main.elf main.c -Wl,-Bstatic -L../ld.a -ldld -Wl,-Bdynamic -L. -lSO_example_lib -fno-rtti -fno-exceptions 
