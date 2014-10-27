/*  <src-path>/src/kernel/inc/mt/proc.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
 *  Copyright (C) 2013, 2014  martin.erhardt98@googlemail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef PROC_H
#define PROC_H

#include<mm/vmm.h>
#include<stdint.h>
#include"user.h"
#include<time.h>
#include<libOS/list.h>

typedef uint_t pid_t;
struct proc {
	pid_t		p_id;
	struct user*	user;
//	char[10]	cmd;
//	clock_t		clock;
	struct thread * first_thread;
	alist_t		shm_segs;
	vmm_context*	context; 
};

alist_t proc_list;
struct proc* first_proc;
uint8_t process_system_lock;
struct proc* create_proc();
struct proc* get_proc(uint_t p_id);
uint_t get_pid();
void exit(struct proc* to_exit);

#endif
