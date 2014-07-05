/*   <src-path>/src/kernel/mt/threads.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<mt/threads.h>
#include<mt/proc.h>
#include<stdlib.h>
#include<mm/vmm.h>
#include<dbg/console.h>
#include<gdt.h>
#include<asm_inline.h>
#include<libOS/lock.h>
#include<smp_capabilities.h>
#include<cpu.h>
#include<local_apic.h>
#include<libOS/list.h>

static bool are_there_still_threads(struct cpu_info * this_cpu);
struct cpu_info* get_cpu(uint_t t_id);
int wakeup_=0;
int32_t create_thread(void* entry, struct proc * in_proc)
{
	struct thread* new_t	= (struct thread*)kmalloc(sizeof(struct thread));
	struct cpu_state * new_state	= (struct cpu_state*)kmalloc(sizeof(struct cpu_state));
	uint8_t* user_stack	= uvmm_malloc(in_proc->context, STDRD_STACKSIZ);
	
	if(in_proc==NULL)
	{
		kprintf("couldn't get pid");
		return -1;
	}
	spinlock_ackquire(&multi_threading_lock);
	
	total_thread_count	++;
	
	struct cpu_info * cpu	= get_best_cpu();
	
	INIT_STATE(new_state);
	
	new_state->REG_IP	= (uintptr_t) entry;
	new_state->REG_STACKPTR	= (uintptr_t) user_stack+STDRD_STACKSIZ - 0x10 ;
	
	new_t->t_id		= total_thread_count;
	new_t->state		= new_state;
	new_t->user_stack	= user_stack;
	new_t->proc		= in_proc;
	new_t->next_in_proc	= in_proc->first_thread;
	new_t->exc_state	= THREAD_ACTIVE;
	//alist_add(&in_proc->threads_in_proc, new_t);
	alist_add(&cpu->thread_list, new_t);
	cpu->thread_count	++;in_proc->first_thread	= new_t;
#ifdef ARCH_X86
	cpu->is_no_thread	= 0;
#endif
	spinlock_release(&multi_threading_lock);
	return 0;
}
struct cpu_info * get_best_cpu()
{
	float average_thread_count = total_thread_count/cores_from_tables;// 1: 2 2: 5
	struct cpu_info * this_cpu = &bsp_info;
	struct cpu_info * best_fit = &bsp_info;
	float best_fit_diff = ((&bsp_info)->thread_count+1)-average_thread_count;// 1: 12 2: 15
	float diff_with_process;
	int i = 0;
	if(best_fit_diff<0)
			best_fit_diff = best_fit_diff*(-1);
	while(this_cpu!=NULL) 
	{
		diff_with_process = (this_cpu->thread_count+1)-average_thread_count; // 1.1: 8 2.1: 8 1.2: 15 2.2: 5
		
		if(diff_with_process<0)
			diff_with_process = diff_with_process*(-1);
		if(diff_with_process<best_fit_diff)
		{
			best_fit = this_cpu;
			best_fit_diff = diff_with_process;
		}
		i++;
		this_cpu = (struct cpu_info *)alist_get_by_index(&cpu_list,i);
	}
	
	return best_fit;
}
struct cpu_info * move_if_it_make_sense(struct cpu_info * this_cpu,struct thread * to_move)
{
	this_cpu->thread_count--;
	int i = 0;
	float average_thread_count = total_thread_count/cores_from_tables;
	struct cpu_info * best_fit = this_cpu; 
	struct cpu_info * cur_cpu = &bsp_info;
	float best_fit_diff = ((&bsp_info)->thread_count+1)-average_thread_count;// 1: 12 2: 15
	float diff_with_thread;
	if(best_fit_diff<0)
			best_fit_diff = best_fit_diff*(-1);
	while(cur_cpu)
	{
		diff_with_thread = (cur_cpu->thread_count+1)-average_thread_count;
		if(diff_with_thread<0)
			diff_with_thread = diff_with_thread*(-1);
		if(diff_with_thread<best_fit_diff)
		{
			best_fit_diff = diff_with_thread;
			best_fit = cur_cpu;
		}
		i++;
		cur_cpu = (struct cpu_info *)alist_get_by_index(&cpu_list,i);
	}
	if(best_fit!=this_cpu)
	{
		kprintf("[THREADS] I: move_if_it_makes_sense moves\n");
		alist_remove(&this_cpu->thread_list, to_move);
		alist_add(&best_fit->thread_list, to_move);
		this_cpu->current_thread = NULL;
		this_cpu->current_thread_index = 0;
#ifdef ARCH_X86
		if(!are_there_still_threads(this_cpu))
			this_cpu->is_no_thread = LOCK_USED;
#endif
	}
	else this_cpu->thread_count++;
	return best_fit;
}
void kill_thread(struct thread * to_kill, struct proc * in_proc)
{
	struct cpu_info * this_cpu = get_cur_cpu();
	//spinlock_ackquire(&multi_threading_lock);
	alist_remove(&this_cpu->thread_list, to_kill);
	SET_CONTEXT(virt_to_phys(&startup_context,(uintptr_t)(startup_context.highest_paging)))
	this_cpu->current_thread = NULL;
	this_cpu->current_thread_index = 0;
#ifdef ARCH_X86
	if(!are_there_still_threads(this_cpu))
		this_cpu->is_no_thread=1;
#endif
	//vmm_free(in_proc->context, to_kill->user_stack, STDRD_STACKSIZ);
	kfree(to_kill->state);
	kfree(to_kill);
	//spinlock_release(&multi_threading_lock);
}
static bool are_there_still_threads(struct cpu_info * this_cpu)
{
	if(alist_get_by_entry8(&this_cpu->thread_list, sizeof(uint_t), THREAD_ACTIVE))
		return TRUE;
	return FALSE;
}
uint_t sleep(uint_t time)
{
	spinlock_ackquire(&multi_threading_lock);
	struct cpu_info * cur_cpu = get_cur_cpu();
	cur_cpu->current_thread->exc_state = THREAD_BLCKD;
	if(!are_there_still_threads(cur_cpu))
	{
#ifdef ARCH_X86
		cur_cpu->is_no_thread = 1;
#endif
		SET_CONTEXT(virt_to_phys(&startup_context,(uintptr_t)(startup_context.highest_paging)))
		cur_cpu->current_thread=NULL;
	}
	spinlock_release(&multi_threading_lock);
	return 0;
}
void wakeup(uint_t t_id)
{
	spinlock_ackquire(&multi_threading_lock);
	struct thread * this_thread = get_thread( t_id);
	if(!this_thread)
		return;
	this_thread->exc_state = THREAD_ACTIVE;
#ifdef ARCH_X86
	struct cpu_info * cur_cpu = get_cpu( t_id);
	cur_cpu->is_no_thread = 0;
#endif
	wakeup_ =1;
	spinlock_release(&multi_threading_lock);
}
struct cpu_state* dispatch_thread(struct cpu_state* cpu)
{
	vmm_context* curcontext = NULL;
	uintptr_t next_context = 0x0;
	struct cpu_info * this_cpu = get_cur_cpu();
	
	/*
	* Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
	* gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
	* wir spaeter nicht wieder.
	*/
	/*
	* Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
	*/
retry:
#ifdef ARCH_X86_64
	if(!are_there_still_threads(this_cpu))
		return &(this_cpu->idle_state);
#endif
#ifdef ARCH_X86
	spinlock_ackquire(&this_cpu->is_no_thread);
	spinlock_release(&this_cpu->is_no_thread);
#endif
	spinlock_ackquire(&multi_threading_lock);
	
	struct thread * first_thread = (struct thread *)alist_get_by_index(&this_cpu->thread_list, 0);
	do if (this_cpu->current_thread == NULL)
		{
			if(!are_there_still_threads(this_cpu))
			{
				spinlock_release(&multi_threading_lock);
				goto retry;
			}
			curcontext= &startup_context;
			next_context = virt_to_phys(curcontext,(uintptr_t)(first_thread->proc->context->highest_paging));
			
			this_cpu->current_thread = first_thread;
		}
		else 
		{
			
			curcontext= this_cpu->current_thread->proc->context;
			
			*this_cpu->current_thread->state = *cpu;
			
			if(this_cpu->current_thread_index +1 < alist_get_entry_n(&this_cpu->thread_list))
			{
				next_context = virt_to_phys(curcontext,(uintptr_t)(uintptr_t)((struct thread *)alist_get_by_index(&this_cpu->thread_list, this_cpu->current_thread_index+1))->proc->context->highest_paging);
				this_cpu->current_thread_index ++;
			}
			else
			{
				next_context = virt_to_phys(curcontext,(uintptr_t)(first_thread->proc->context->highest_paging));
				this_cpu->current_thread_index = 0;
			}
			this_cpu->current_thread = alist_get_by_index(&this_cpu->thread_list, this_cpu->current_thread_index);
		}
	while(move_if_it_make_sense(this_cpu,this_cpu->current_thread)!=this_cpu);
	/* Prozessorzustand des neuen Tasks aktivieren */
	cpu = this_cpu->current_thread->state;
	this_cpu->cur_proc = this_cpu->current_thread->proc;
	
	if(this_cpu->current_thread->proc->context!=curcontext)
		SET_CONTEXT(next_context)
	spinlock_release(&multi_threading_lock);
	
	return cpu;
}
int32_t switchto_thread(uint_t t_id,struct cpu_state* cpu)
{
/*
	struct thread*prev=current_thread;
	struct thread*switch_to=get_thread(t_id);
	vmm_context* curcontext=get_cur_context_glob();
	
	if(switch_to==NULL)
	{
	    return -1;
	}
	
	// Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
	// gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
	// wir spaeter nicht wieder.
	
	
	// Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
	
	current_thread->state = cpu;
	current_thread = switch_to;
	
	// Prozessorzustand des neuen Tasks aktivieren 
	cpu = current_thread->state;
	current_thread->state->ss = 0x23;
	tss.esp0 = ((uintptr_t) cpu)+sizeof(cpu_state);
	
	if(current_thread->proc!=prev->proc)
	{
	    SET_CONTEXT(virt_to_phys(curcontext,(uintptr_t)current_thread->proc->context));
	    
	}*/
	return 0;
}

struct thread* get_thread(uint_t t_id)
{
	struct cpu_info * cpu = alist_get_by_index(&cpu_list, 0);
	uint_t i = 0;
	struct thread* cur_thread = NULL;
	while(cpu)
	{
		i++;
		cur_thread = alist_get_by_entry(&cpu->thread_list, 0, t_id);
		if(cur_thread)
			return cur_thread;
		cpu = alist_get_by_index(&cpu_list, i);
	}
	return NULL;
}
struct cpu_info* get_cpu(uint_t t_id)
{
	struct cpu_info * cpu = alist_get_by_index(&cpu_list, 0);
	uint_t i = 0;
	struct thread* cur_thread = NULL;
	while(cpu)
	{
		i++;
		cur_thread = alist_get_by_entry(&cpu->thread_list, 0, t_id);
		if(cur_thread)
			return cpu;
		cpu = alist_get_by_index(&cpu_list, i);
	}
	return NULL;
}
