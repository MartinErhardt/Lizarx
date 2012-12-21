#ifndef SYSCALL_H
#define SYSCALL_H
#include"cpustate.h"
extern void intr_stub_48(void);
void handle_syscall(struct cpu_state* cpu);
#endif
