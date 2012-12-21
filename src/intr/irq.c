#include "console.h"
#include "irq.h"
#include "cpustate.h"
#include "../driver/keyboard/keyboard.h"
#include<ioport.h>
void handle_irq(struct cpu_state* cpu)
{
	if (cpu->intr >= 0x28) {
	            // EOI an Slave-PIC
	            outb(0xa0, 0x20);
	}else if(cpu->intr==0x21){irq_handler(0x21);}
	// EOI an Master-PIC
	outb(0x20, 0x20);
}
