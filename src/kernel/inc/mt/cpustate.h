#ifndef CPUSTATE_H
#define CPUSTATE_H

#include<stdint.h>

typedef struct {
    // manually secured registers
    uint32_t   eax;
    uint32_t   ebx;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   esi;
    uint32_t   edi;
    uint32_t   ebp;

    uint32_t   intr;
    uint32_t   error;

    // secured by cpu
    uint32_t   eip;
    uint32_t   cs;
    uint32_t   eflags;
    uint32_t   esp;
    uint32_t   ss;
} cpu_state;

#endif
