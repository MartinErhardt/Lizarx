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
uint32_t total_thread_count;
typedef uint_t tid_t;
struct thread
{
	tid_t	t_id;
	struct cpu_state * state;
	uint8_t*	user_stack;
	struct proc *	proc;
	struct thread *	next_in_proc;
};

lock_t multi_threading_lock;
extern void return_idle_thread();
int32_t create_thread(void* entry, struct proc * in_proc);
struct cpu_info * move_if_it_make_sense(struct cpu_info * this_cpu,struct thread * to_move);
struct cpu_info * get_best_cpu();
void kill_thread(struct thread * to_kill, struct proc * in_proc);
struct cpu_state * dispatch_thread(struct cpu_state* cpu);
int32_t switchto_thread(uint_t t_id,struct cpu_state* cpu);
struct thread* get_thread(uint_t t_id);

#endif
