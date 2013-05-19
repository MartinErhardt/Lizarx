SRCS_OPTI = $(shell find . -name '*.[cS]' -a ! -name 'stdlib.c' -a ! -name 'vram.c'  -a ! -name 'string.c')
SRCS_UNOPTI = lib/stdlib.c lib/string.c drv/vga-txt_graphics/vram.c

OBJS_OPTI = $(addsuffix .o,$(basename $(SRCS_OPTI)))
OBJS_UNOPTI = $(addsuffix .o,$(basename $(SRCS_UNOPTI)))

CFLAGS = -m32 -Wall -Werror -g -fno-stack-protector -fno-builtin -nostdinc -ffreestanding -I inc -I lib -I HAL

kernel: $(OBJS_OPTI) $(OBJS_UNOPTI)
	$(LD_CROSS) $(LDFLAGS) -o $@ $^
	rm $(OBJS_OPTI) $(OBJS_UNOPTI)

$(OBJS_OPTI): EXTRA_FLAGS := -Os

ASFLAGS =-m32 -nostdinc -ffreestanding -I HAL
LDFLAGS =  -Tkernel.ld

%.o: %.c
	$(CC_CROSS) $(CFLAGS) $(EXTRA_FLAGS)  -o $@ -c $<

%.o: %.S
	$(CC_CROSS) $(ASFLAGS) -c -o $@ $^
