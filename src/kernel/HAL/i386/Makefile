SRCS_OPTI = mm/pmm.c\
	mm/vmm.c \
	mm/vheap.c \
	mm/gdt.c \
	mt/elf.c \
	mt/sched.c \
	mt/proc.c \
	mt/threads.c \
	HAL/i386/asm/start_x86.S \
	HAL/i386/asm/intr_stub_x86.S \
	lib/time.c \
	intr/irq.c \
	intr/syscall.c \
	intr/err.c \
	intr/idt.c \
	boot/init.c \
	dbg/stack_trace.c \
	dbg/console.c \
	drv/timer/timer.c \
	drv/hwtime/hwtime.c \
	drv/keyboard/sc2kc.c \
	drv/keyboard/keyboard.c
SRCS_UNOPTI = lib/stdlib.c lib/string.c drv/vga-txt_graphics/vram.c

OBJS_OPTI = $(addsuffix .o,$(basename $(SRCS_OPTI)))
OBJS_UNOPTI = $(addsuffix .o,$(basename $(SRCS_UNOPTI)))

CFLAGS = -m32 -Wall -Werror -g -fno-stack-protector -fno-builtin -nostdinc -ffreestanding -I inc -I lib -I HAL -fno-tree-loop-distribute-patterns

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
