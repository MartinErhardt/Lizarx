#include<intr/err.h>
#include<mt/cpustate.h>
#include<dbg/console.h>
void handle_exception(cpu_state* cpu){
	kprintf("Error: %d", cpu->intr);

	while(1) {
	   // Prozessor anhalten
	   asm volatile("cli; hlt");
	}
}
