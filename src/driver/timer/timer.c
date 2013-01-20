#include<stdint.h>
#include<cpustate.h>
#include<stdbool.h>
#include <gdt.h>
#include"timer.h"
#include"../../mt/ts.h"

uint32_t time=0;
void timer_handler(cpu_state** new_cpu){
    time++;
    *new_cpu=schedule(*new_cpu);
    tss[1] = (uint32_t) (*new_cpu + 1);
}