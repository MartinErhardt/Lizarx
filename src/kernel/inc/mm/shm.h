/*   <src-path>/src/kernel/mm/inc/shm.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef SHM_H
#define SHM_H

#include<stdint.h>
#include<mt/proc.h>

struct shmid_ds
{
	uint_t id;
	pid_t shm_cpid;
	uintptr_t virt_in_orig_proc;
	size_t size;
};
struct shm_seg
{
	uint_t id;
	uintptr_t addr;
	size_t size;
};
typedef uint_t key_t;

lock_t shm_lock;
alist_t shmid_list;

int shmget(key_t key, size_t size, int shmflg);
void *shmat(uint_t shmid, const void *shmaddr, int shmflg);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
int shmdt(const void *shmaddr);

#endif
