#include"cpustate.h"
#include"console.h"

void handle_syscall(struct cpu_state* cpu){
	kprintf("Syscall:%d"+ cpu->intr,0x0A,0x00);
}
