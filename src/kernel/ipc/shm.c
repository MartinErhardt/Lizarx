/*  <src-path>/src/kernel/ipc/shm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include<ipc/shm.h>
#include<stdint.h>
#include<mm/vheap.h>
#include<mm/vmm.h>
#include<../x86_common/local_apic.h>
#include<cpu.h>
#include<mt/threads.h>

/*struct shmid_ds
{
	uint_t id;
	pid_t shm_cpid;
	uintptr_t virt_in_orig_proc;
	size_t size;
}*/
int shmget(key_t key, size_t size, int shmflg)
{
	spinlock_ackquire(&shm_lock);
	struct shmid_ds * buf 	= kmalloc(sizeof(struct shmid_ds));
	alist_add(&shmid_list, buf);
	buf->id 		= alist_get_index(&shmid_list, buf);
	buf->shm_cpid 		= get_pid();
	buf->virt_in_kernel 	= 0;
	buf->size 		= size;
	spinlock_release(&shm_lock);

	return buf->id;
}
void *shmat(int shmid, const void *shmaddr, int shmflg)
{
	spinlock_ackquire(&shm_lock);
	struct shmid_ds * id = alist_get_by_entry_s(&shmid_list, 0,shmid);
	if(!id)
	{
		spinlock_release(&shm_lock);
		return NULL;
	}
	if(!id->virt_in_kernel)
		id->virt_in_kernel = (uintptr_t) kvmm_malloc(id->size);
	uintptr_t virt_in_proc =vmm_share_to_user(id->virt_in_kernel, id->size);
	spinlock_release(&shm_lock);
	return (void*) virt_in_proc;
}
int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	return -1;
}
int shmdt(const void *shmaddr)
{
	return -1;
}
