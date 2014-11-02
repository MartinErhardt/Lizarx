/*  <src-path>/src/kernel/ipc/sem.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include<ipc/sem.h>
#include<mm/vheap.h>
#include<mt/threads.h>
#include<local_apic.h>
#include<stdint.h>
int   semget(key_t key, int nsems, int flag)
{
	flag=IPC_CREAT;
	spinlock_ackquire(&sem_atomic);
	struct sem_group*my_semgroup=alist_get_by_entry(&sem_groups,0,key);
	struct semaphore *new_sem=my_semgroup->all_semaphores;
	if(!my_semgroup)
	{
		new_sem=kmalloc(sizeof(struct semaphore)*nsems);
		my_semgroup=kmalloc(sizeof(struct sem_group));
		memset(new_sem,0,sizeof(struct semaphore)*nsems);
		alist_add(&sem_groups,my_semgroup);
		my_semgroup->key=alist_get_index(&sem_groups, my_semgroup)+1;
		my_semgroup->semid.sem_nsems=nsems;
		my_semgroup->all_semaphores=new_sem;
	} 
	spinlock_release(&sem_atomic);
	return alist_get_index(&sem_groups,my_semgroup);
}
int   semop(int id, struct sem_buf * sops, size_t nsops)
{
	//FIXME Do security checks here
	int i,j;
	uint_t tid;
	if(id <0) 
		return -1;
	spinlock_ackquire(&sem_atomic);
	struct sem_group* my_group=alist_get_by_index(&sem_groups,id);
	for(i=0;i<nsops;i++)
	{
		if(sops[i].sem_num>=my_group->semid.sem_nsems)
			continue;
		my_group->all_semaphores[sops[i].sem_num].semval+=sops[i].semop;
		if(my_group->all_semaphores[sops[i].sem_num].semval<0)
		{
			alist_add(&(my_group->all_semaphores[sops[i].sem_num].asleep),(void*) (get_cur_cpu()->current_thread->t_id));
		//	kprintf("sem sleep: 0x%x",get_cur_cpu()->current_thread->t_id);
			sleep(0);
			spinlock_release(&sem_atomic);
			return -1;
		}
		else
			for(j=0; j<alist_get_entry_n(&(my_group->all_semaphores[sops[i].sem_num].asleep));j++)
			{
				tid=(uint_t)alist_get_by_index(&(my_group->all_semaphores[sops[i].sem_num].asleep),0);
				wakeup(tid);
	//			kprintf("sem wakeup: 0x%x",tid);
				alist_remove(&(my_group->all_semaphores[sops[i].sem_num].asleep),(void*)tid);
			}
	}
	spinlock_release(&sem_atomic);
	return 0;
}
