#include <dbg/console.h>
#include <mt/cpustate.h>
#include<drv/io/ioport.h>
#include <drv/keyboard/keyboard.h>
#include <drv/timer/timer.h>
#include <intr/irq.h>
#include <mt/ts.h>

cpu_state* handle_irq(cpu_state* cpu)
{       cpu_state* new_cpu = cpu;
	//kprintf("intr end");
	if (cpu->intr >= 0x28) {
	            // EOI an Slave-PIC
	            outb(0xa0, 0x20);
	}else if(cpu->intr==0x21){kbc_handler(0x21);}
	 else if (cpu->intr == 0x20) {
	    timer_handler(&new_cpu);
	}
	// EOI an Master-PIC
	outb(0x20, 0x20);
	//kprintf("intr end");
	return new_cpu;
}
