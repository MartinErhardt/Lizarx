#include<mt/cpustate.h>
#include<mm/gdt.h>
#include<drv/timer/timer.h>
#include<mt/ts.h>
#include<stdbool.h>
#include<stdint.h>
#include <drv/vga-txt_graphics/vram.h>

uint32_t time=0;
void timer_handler(cpu_state** new_cpu){
    time++;
    drawcurs();
    *new_cpu=schedule(*new_cpu);
    tss[1] = (uint32_t) (*new_cpu + 1);
}