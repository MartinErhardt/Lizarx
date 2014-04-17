/*   <src-path>/src/kernel/inc/intr/syscall.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *   Copyright (C) 2013  martin.erhardt98@googlemail.com
 *
 *  Lizarx is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Lizarx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU LESSER General Public License
 *  along with Lizarx.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SYSCALL_H
#define SYSCALL_H

#include<cpu.h>

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
    SYS_VMM_MALLOC,
    SYS_VMM_FIND,
    SYS_VMM_REALLOC
} syscalls;

extern void intr_stub_48(void);

struct cpu_state* handle_syscall(struct cpu_state* cpu);
/*
void do_err();
void do_err2();
void page_fault();
*/
#endif
