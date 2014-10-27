/*  <src-path>/src/kernel/inc/ipc/sem.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef SEM_H
#define SEM_H

#include"ipc.h"
#include<mt/proc.h>
#include<libOS/list.h>
#include<libOS/lock.h>

alist_t sem_groups;
lock_t sem_atomic;
struct semaphore
{
	short semval;
	pid_t sempid;
	unsigned short semncnt;
	unsigned short semzcnt;
	alist_t asleep;
};
struct semid_ds
{
	struct ipc_perm sem_perm;
	unsigned short   sem_nsems;
	time_t           sem_otime;
	time_t           sem_ctime;
};
struct sem_group
{
	key_t key;
	struct semid_ds semid;
	struct semaphore* all_semaphores;
};
struct sem_buf
{
	unsigned short sem_num;
	short semop;
	short sem_flg;
};
int   semctl(int, int, int, ...);
int   semget(key_t, int, int);
int   semop(int, struct sem_buf *, size_t);
#endif
