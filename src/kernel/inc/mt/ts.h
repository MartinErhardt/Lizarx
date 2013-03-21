#ifndef TS_H
#define TS_H

#include <mt/cpustate.h>
#include <stdint.h>
#include <stdbool.h>
#include <mm/vmm.h>

#define STDRD_STACKSIZE 0x2000 //Stack größe bei der Initialisierung

// FIXME Make different thread and Process
struct task {
    cpu_state *	state;
    uint32_t 	pid;
    uint8_t* 	stack;
    uint8_t* 	user_stack;
    vmm_context*	context;
    struct task*	next;
} ;

struct task* init_task(void* entry);
//void init_mt();

struct task* getldtasks();
struct task* getcurtask();
vmm_context* getcurcontext();

cpu_state* schedule(cpu_state* cpu);


#endif