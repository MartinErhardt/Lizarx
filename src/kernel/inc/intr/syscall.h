#ifndef SYSCALL_H
#define SYSCALL_H
#include<hal.h>

typedef enum {
    SYS_DRAW,
    SYS_INFO,
    SYS_GETTID,
    SYS_KILLTID,
    SYS_FORKTID,
    SYS_GETPID,
    SYS_KILLPID,
    SYS_FORKPID,
    SYS_ERROR,
    SYS_GET_BOOTMOD,
    SYS_VMM_MALLOC
} syscalls;

extern void intr_stub_48(void);

CPU_STATE* handle_syscall(CPU_STATE* cpu);
/*
void do_err();
void do_err2();
void page_fault();
*/
#endif
