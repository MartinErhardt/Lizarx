#ifndef IRQ_H
#define IRQ_H
#include<hal.h>

extern void intr_stub_32(void);
extern void intr_stub_33(void);


CPU_STATE* handle_irq(CPU_STATE* cpu);
#endif
