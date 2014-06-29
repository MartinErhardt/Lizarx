/*   <src-path>/src/kernel/mm/shm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<mm/shm.h>
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
	buf->virt_in_orig_proc 	= 0;
	buf->size 		= size;
	spinlock_release(&shm_lock);
	
	return buf->id;
}
void *shmat(uint_t shmid, const void *shmaddr, int shmflg)
{
	spinlock_release(&shm_lock);
	struct shmid_ds * id = alist_get_by_entry(&shmid_list, 0,shmid);
	if(!id->virt_in_orig_proc )
	{
		if(id->shm_cpid != get_pid())
			id->shm_cpid = get_pid();
		id->virt_in_orig_proc =(uintptr_t) uvmm_malloc(get_cur_context_glob(),id->size);
		spinlock_release(&shm_lock);
		return (void*)id->virt_in_orig_proc;
	}
	else
	{
		spinlock_release(&shm_lock);
		
		return (void*) vmm_share_mem(
						get_cur_cpu()->current_thread->proc,
						(void*) get_proc(id->shm_cpid),
						id->virt_in_orig_proc,
						id->size);
	}
	return NULL;
}
int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
	return -1;
}
int shmdt(const void *shmaddr)
{
	return -1;
}
