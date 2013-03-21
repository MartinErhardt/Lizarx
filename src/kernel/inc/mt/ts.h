/*   <src-path>/src/kernel/inc/mm/ts.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *   Copyright (C) 2013  martin.erhardt98@googlemail.com
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
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