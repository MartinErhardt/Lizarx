SRCS_HLIB = 	list.c \
		OO/AList.cpp
OBJS_HLIB = $(addsuffix .o,$(basename $(SRCS_HLIB)))

CPPC_CROSS=/home/martin/git/Lizarx/Lizarx/host/buildtools/x86_64-pc-lizarx/bin/x86_64-pc-lizarx-g++
LD_CROSS=/home/martin/git/Lizarx/Lizarx/host/buildtools/x86_64-pc-lizarx/bin/x86_64-pc-lizarx-ld
AR_CROSS=/home/martin/git/Lizarx/Lizarx/host/buildtools/x86_64-pc-lizarx/bin/x86_64-pc-lizarx-ar
LD_CRT0_FLG=/home/martin/git/Lizarx/Lizarx/host/buildtools/x86_64-pc-lizarx/bin/crt0.o

hlib.a: $(OBJS_HLIB)
	$(AR_CROSS) rcs hlib.a $(OBJS_HLIB)
	rm $(OBJS_HLIB)
%.o: %.c
	$(CPPC_CROSS) -o $@ -c $<
