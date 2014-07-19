SRCS = main.cpp \
	ipc_manager.cpp

OBJS = $(addsuffix .o,$(basename $(SRCS)))

ASFLAGS = 
CFLAGS =  -Werror -Wall -fno-rtti -fno-exceptions
CPPC_CROSS=/home/martin/git/Lizarx/Lizarx/host/buildtools/x86_64-pc-lizarx/bin/x86_64-pc-lizarx-g++
LD_CROSS=/home/martin/git/Lizarx/Lizarx/host/buildtools/x86_64-pc-lizarx/bin/x86_64-pc-lizarx-ld
LD_CRT0_FLG=/home/martin/git/Lizarx/Lizarx/host/buildtools/x86_64-pc-lizarx/bin/crt0.o

vfs.elf: $(OBJS)
	$(LD_CROSS) $(LDFLAGS) -o $@ $^ $(LD_CRT0_FLG)
	rm $(OBJS)

%.o: %.c
	$(CPPC_CROSS) $(CFLAGS) -c -o $@ $^
