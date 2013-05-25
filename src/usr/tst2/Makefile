SRCS = $(shell find -name '*.c')
OBJS = $(addsuffix .o,$(basename $(SRCS)))
all:
	$(CC_CROSS) -Wall -Werror -fpic foo.c -c
	$(CC_CROSS) -shared -o libSO_example_lib.so foo.o
	$(CC_CROSS) -Wall -o SO_example_main.elf main.c -Wl,-Bstatic -L../ld.a -ldld -Wl,-Bdynamic -L/home/martin/Lizarx/src/usr/tst2 -lSO_example_lib 
