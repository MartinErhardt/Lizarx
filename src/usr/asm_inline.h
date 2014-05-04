#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL(N)	asm volatile ("syscall"::"a" (N) );

#endif
