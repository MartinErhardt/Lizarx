#ifndef SYSCALL_H
#define SYSCALL_H

#define SYSCALL(N)	asm volatile ("int $0x30"::"a" (N) );
#endif
