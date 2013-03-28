#ifndef INTR_H
#define INTR_H
#include<stdint.h>
#include <stdbool.h>

void init_idt(void);
void enable_intr();
bool intr_activated;
#endif
