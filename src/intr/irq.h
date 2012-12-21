#ifndef IRQ_H
#define IRQ_H
#include"cpustate.h"

extern void intr_stub_32(void);
extern void intr_stub_33(void);


void handle_irq(struct cpu_state* cpu);
#endif
