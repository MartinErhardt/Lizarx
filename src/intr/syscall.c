#include"cpustate.h"
#include"console.h"

void handle_syscall(cpu_state* cpu){
	kprintf("Syscall:%p"+ cpu->intr);
}
