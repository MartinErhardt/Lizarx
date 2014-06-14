/*   <src-path>/src/kernel/mt/proc.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<stdint.h>
#include<mt/proc.h>
#include<mm/vheap.h>
#include<mt/user.h>
#include<mt/threads.h>
#include<dbg/console.h>
#include<libOS/lock.h>
#include<../x86_common/local_apic.h>

uint_t num_proc = 0;
uint_t pid_counter = 0;

struct proc* create_proc()
{
	vmm_context *new_con	= kmalloc(sizeof(vmm_context));
	struct proc* new_proc	= kmalloc(sizeof(struct proc));
	struct user* new_user	= kmalloc(sizeof(struct user));
	*new_con		= vmm_crcontext();
	spinlock_ackquire(&process_system_lock);
	num_proc		++;
	pid_counter		++;
	new_user->u_id		= 0;
	new_proc->context	= new_con;
	new_proc->user		= new_user;
	new_proc->p_id		= pid_counter;
	new_proc->first_thread = NULL;
	alist_add(&proc_list, (void*)new_proc);
	spinlock_release(&process_system_lock);
	return new_proc;
}
void exit(struct proc* to_exit)
{
	spinlock_ackquire(&process_system_lock);
	spinlock_ackquire(&multi_threading_lock);
	struct thread * cur_thread = to_exit->first_thread;
	//kprintf("hi");
	
	num_proc--;
	while(cur_thread)
	{
		kill_thread(cur_thread,to_exit);

		cur_thread = cur_thread->next_in_proc;
	}
	alist_remove(&proc_list, to_exit);
	spinlock_ackquire(&vmm_lock);
	vmm_delcontext(to_exit->context);
	kfree(to_exit->context);
	kfree(to_exit);
	spinlock_release(&multi_threading_lock);
	spinlock_release(&process_system_lock);
	spinlock_release(&vmm_lock);

}
struct proc * get_proc(uint_t p_id)
{
	return alist_get_by_entry(&proc_list, 0,p_id);
}
uint_t get_pid()
{
	return get_cur_cpu()->current_thread->proc->p_id;
}
