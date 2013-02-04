#ifndef SYSCALL_H
#define SYSCALL_H
#include<mt/cpustate.h>

typedef enum {
    SYS_DRAW,
    SYS_INFO,
    SYS_GETTID,
    SYS_KILLTID,
    SYS_FORKTID,
    SYS_GETPID,
    SYS_KILLPID,
    SYS_FORKPID
} syscalls;

extern void intr_stub_48(void);
void handle_syscall(cpu_state* cpu);
#endif
