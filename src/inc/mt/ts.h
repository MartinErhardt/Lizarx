#ifndef TS_H
#define TS_H

#include<mt/cpustate.h>
#include<stdint.h>
#include <stdbool.h>

#define STDRD_STACKSIZE 4096 //Stack größe bei der Initialisierung
#define MAX_TASKS 32 // Maximal zahl an Tasks noch keine malloc() funktion implementiert

typedef struct {
    cpu_state *state;
    uint32_t pid;
    uint8_t* stack;
    uint8_t* user_stack;
} task;
task* init_task(void* entry);
cpu_state* schedule(cpu_state* cpu);
#endif