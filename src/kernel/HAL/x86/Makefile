SRCS_OPTI = mm/vmm.c \
	mm/vheap.c \
	mm/gdt.c \
	mt/elf.c \
	mt/sched.c \
	mt/proc.c \
	mt/threads.c \
	HAL/x86/asm/start_x86.S \
	HAL/x86/asm/intr_stub_x86.S \
	HAL/x86/asm/trampoline.S \
	lib/time.c \
	intr/irq.c \
	intr/syscall.c \
	intr/err.c \
	intr/idt.c \
	dbg/stack_trace.c \
	dbg/console.c \
	drv/timer/timer.c \
	drv/hwtime/hwtime.c \
	drv/keyboard/sc2kc.c \
	drv/keyboard/keyboard.c \
	intr/local_apic.c \
	boot/init.c \
	HAL/x86_common/smp_capabilities.c \
	HAL/x86_common/cpuid.c
SRCS_UNOPTI = lib/stdlib.c lib/string.c drv/vga-txt_graphics/vram.c mm/pmm.c

OBJS_OPTI = $(addsuffix .o,$(basename $(SRCS_OPTI)))
OBJS_UNOPTI = $(addsuffix .o,$(basename $(SRCS_UNOPTI)))

CFLAGS = -m32 -Wall -Werror -g -fno-stack-protector -fno-builtin -nostdinc -ffreestanding -I inc -I lib -I inc/HAL/x86 -I inc/HAL/x86_common -DARCH_X86=x86 -DARCH=x86 -fno-tree-loop-distribute-patterns 

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
