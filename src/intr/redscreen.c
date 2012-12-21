#include"redscreen.h"
#include"cpustate.h"
#include"console.h"
void handle_exception(struct cpu_state* cpu){
	kprintf("Exception: %d"+ cpu->intr,0x04,0x0);

	while(1) {
	   // Prozessor anhalten
	   asm volatile("cli; hlt");
	}
}
