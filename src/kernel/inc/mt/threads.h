/*   <src-path>/src/kernel/mt/inc/threads.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef THREADS_H
#define THREADS_H

#include<cpu.h>
#include<stdint.h>
#include"proc.h"

#define STDRD_STACKSIZ 0x3000

struct thread* first_thread;
struct thread* current_thread;

struct thread {
	cpu_state *	state;
	uint32_t	t_id;
	uint8_t*	user_stack;
	struct proc*	proc;
	struct thread*	next;
};

int32_t create_thread(void* entry,uint32_t p_id);

cpu_state* dispatch_thread(cpu_state* cpu);
int32_t switchto_thread(uint32_t t_id,cpu_state* cpu);
struct thread* get_thread(uint32_t t_id);

#endif
