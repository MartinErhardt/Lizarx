
.PHONY: all

all: kernel LM_Loader clean

clean: 
	rm  $(OBJS_OPTI) $(OBJS_UNOPTI) $(OBJS_LOADER)

ASFLAGS =  -nostdinc -ffreestanding -I HAL
CFLAGS = -Wall -Werror -g -fno-stack-protector -fno-builtin -nostdinc -ffreestanding -I inc -I lib -I HAL
C64FLAGS = -mno-red-zone  -mcmodel=large -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow
SRCS_OPTI = boot/init.c \
	mm/pmm.c\
	mm/vmm.c \
	mm/vheap.c \
	mt/elf.c \
	mt/sched.c \
	mt/proc.c \
	mt/threads.c \
	HAL/x86_64/asm/intr_stub_x86_64.S \
	lib/time.c \
	intr/irq.c \
	intr/syscall.c \
	intr/err.c \
	intr/idt.c \
	mm/gdt.c \
	dbg/stack_trace.c \
	dbg/console.c \
	drv/timer/timer.c \
	drv/hwtime/hwtime.c \
	drv/keyboard/sc2kc.c \
	drv/keyboard/keyboard.c
SRCS_UNOPTI = lib/stdlib.c \
	lib/string.c \
	drv/vga-txt_graphics/vram.c
OBJS_OPTI = $(addsuffix .o,$(basename $(SRCS_OPTI)))
OBJS_UNOPTI = $(addsuffix .o,$(basename $(SRCS_UNOPTI)))

$(OBJS_OPTI): EXTRA_FLAGS := -Os

kernel: $(OBJS_OPTI) $(OBJS_UNOPTI) 
	$(LD_CROSS) -Tkernel.ld -o $@ $^
%.o: %.c
	$(CC_CROSS) $(C64FLAGS) $(CFLAGS) $(EXTRA_FLAGS) $(LOADER_FLAGS)  -o $@ -c $<
%.o: %.S 
	$(CC_CROSS) $(C64FLAGS) $(ASFLAGS) $(LOADER_FLAGS) -c -o $@ $^

SRCS_LOADER = HAL/x86_64/asm/start_x86_64.S \
	boot/boot_longmode.c \
	boot/easy_map.c \
	lib/stdlib.c \
	lib/string.c \
	mm/gdt.c \
	drv/vga-txt_graphics/vram.c \
	dbg/console.c
OBJS_LOADER = $(addsuffix 32.o,$(basename $(SRCS_LOADER)))
$(OBJS_LOADER): LOADER_FLAGS := -m32

LM_Loader: $(OBJS_LOADER)
	$(LD32_CROSS) -Tloader.ld  -o $@ $^

%32.o: %.c
	$(CC_CROSS) $(CFLAGS) $(LOADER_FLAGS)  -o $@ -c $<
%32.o: %.S
	$(CC_CROSS) $(ASFLAGS) $(LOADER_FLAGS)  -o $@ -c $<
