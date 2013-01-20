#ifndef TIMER_H
#define TIMER_H

#include<mt/cpustate.h>
#include<stdint.h>
#include<stdbool.h>


extern uint32_t tss[32];
//extern uint32_t time;
void timer_handler(cpu_state** new_cpu);

#endif