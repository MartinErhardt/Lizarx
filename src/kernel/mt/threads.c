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

struct cpu_state idle_state;

static bool are_there_still_threads(struct cpu_info * this_cpu);
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
	new_t->next		= cpu->first_thread;
	new_t->next_in_proc	= in_proc->first_thread;
	cpu->first_thread	= new_t;
	cpu->thread_count	++;
        cpu->is_no_thread	= 0;
	in_proc->first_thread	= new_t;
	spinlock_release(&multi_threading_lock);
	return 0;
}
/*void init_idle_thread()
{
	multi_threading_lock		= LOCK_FREE;
	memset(&idle_state,0x0, sizeof(struct cpu_state));
	//memcpy()
	void * stack = kvmm_malloc(PAGE_SIZE);
	memset(stack,0x0, PAGE_SIZE);
#if defined(ARCH_X86) || defined(ARCH_X86_64)
	idle_state.cs = (KERNEL_CODE_SEG_N<<3);
	idle_state.ss = (KERNEL_DATA_SEG_N<<3);
#endif
	idle_state.rsp =(uintptr_t)stack+PAGE_SIZE-0x10;
	idle_state.rflags = 0x202;
	idle_state.REG_IP = (uintptr_t)&idle_thread;
}*/
struct cpu_info * get_best_cpu()
{
	float average_thread_count = total_thread_count/cores_from_tables;// 1: 2 2: 5
	struct cpu_info * this_cpu = &bsp_info;
	struct cpu_info * best_fit = &bsp_info;
	float best_fit_diff = ((&bsp_info)->thread_count+1)-average_thread_count;// 1: 12 2: 15
	float diff_with_process;
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
		this_cpu=this_cpu->next;
	}
	//kprintf("apic_id: 0x%x\n",best_fit->apic_id);
	return best_fit;
}
struct cpu_info * move_if_it_make_sense(struct cpu_info * this_cpu,struct thread * to_move)
{
	this_cpu->thread_count--;
	float average_thread_count = total_thread_count/cores_from_tables;
	struct cpu_info * best_fit = this_cpu; 
	struct cpu_info * cur_cpu = &bsp_info;
	float best_fit_diff = ((&bsp_info)->thread_count+1)-average_thread_count;// 1: 12 2: 15
	float diff_with_thread;
	struct thread * last_thread = this_cpu->first_thread;
	if(best_fit_diff<0)
			best_fit_diff = best_fit_diff*(-1);
	while(cur_cpu->next!=NULL)
	{
		diff_with_thread = (cur_cpu->thread_count+1)-average_thread_count;
		if(diff_with_thread<0)
			diff_with_thread = diff_with_thread*(-1);
		if(diff_with_thread<best_fit_diff)
		{
			best_fit_diff = diff_with_thread;
			best_fit = cur_cpu;
		}
		cur_cpu=cur_cpu->next;
	}
	if(best_fit!=this_cpu)
	{
		kprintf("[THREADS] I: move_if_it_makes_sense moves\n");
		if(last_thread!=to_move)// if the thread which will be moved is the first we don't have to search for the one behind
		{
			while( (last_thread->next != to_move)&&(last_thread->next != NULL) ) // here we try to get the thread behind to_move the second case should never ocurr
				last_thread = last_thread->next;
			// set the next thread of this, which will be moved as next for the one behind the moved
			last_thread->next = to_move->next; 
		}
		else 
		{
			this_cpu->first_thread = to_move->next;
			if(!are_there_still_threads(this_cpu))
				this_cpu->is_no_thread=1;
		}
		
		to_move->next = best_fit->first_thread;
		best_fit->first_thread = to_move;
	}
	else this_cpu->thread_count++;
	return best_fit;
}
void kill_thread(struct thread * to_kill, struct proc * in_proc)
{
	struct cpu_info * this_cpu = get_cur_cpu();
	//spinlock_ackquire(&multi_threading_lock);
	struct thread * last_thread = this_cpu->first_thread;
	if(last_thread!=to_kill)// if the thread which will be moved is the first we don't have to search for the one behind
	{
		while( (last_thread->next != to_kill)&&(last_thread->next != NULL) ) // here we try to get the thread behind to_kill the second case should never ocurr
			last_thread = last_thread->next;
		// set the next thread of this, which will be moved as next for the one behind the moved
		last_thread->next = to_kill->next; 
		SET_CONTEXT(virt_to_phys(&startup_context,(uintptr_t)(startup_context.highest_paging)))
		this_cpu->current_thread = NULL;
	//	SET_CONTEXT(virt_to_phys(get_cur_context_glob(),(uintptr_t)(to_kill->next->proc->context->highest_paging)))
	//	this_cpu->current_thread = to_kill->next; 
	}
	else 
	{
		this_cpu->first_thread = to_kill->next;
		SET_CONTEXT(virt_to_phys(&startup_context,(uintptr_t)(startup_context.highest_paging)))
		this_cpu->current_thread = NULL;
		if(!are_there_still_threads(this_cpu))
			this_cpu->is_no_thread=1;
	}
	//vmm_free(in_proc->context, to_kill->user_stack, STDRD_STACKSIZ);
	kfree(to_kill->state);
	kfree(to_kill);
	//spinlock_release(&multi_threading_lock);
}
static bool are_there_still_threads(struct cpu_info * this_cpu)
{
	struct thread * cur_thread	= this_cpu->first_thread;
	bool are_there			= FALSE;
	while(cur_thread)
	{
		are_there	= TRUE;
		cur_thread	= cur_thread->next;
	}
	return are_there;
}
struct cpu_state* dispatch_thread(struct cpu_state* cpu)
{
	vmm_context* curcontext=NULL;
	uintptr_t next_context= 0x0;
	struct cpu_info * this_cpu = get_cur_cpu();
	/*
	* Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
	* gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
	* wir spaeter nicht wieder.
	*/
	/*
	* Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
	*/
	spinlock_ackquire(&this_cpu->is_no_thread);
	spinlock_release(&this_cpu->is_no_thread);
	
	spinlock_ackquire(&multi_threading_lock);
	/*if(!are_there_still_threads(this_cpu))
	{
		//kprintf("rflags 0x%x", idle_state.rflags);
		spinlock_release(&multi_threading_lock);
		
		//kprintf("rflags 0x%x", idle_state.rflags);
		return &idle_state;
	}*/
	spinlock_ackquire(&this_cpu->is_no_thread);
	spinlock_release(&this_cpu->is_no_thread);
	do if (this_cpu->current_thread == NULL)
		{
			
			curcontext= &startup_context;
			next_context = virt_to_phys(curcontext,(uintptr_t)this_cpu->first_thread->proc->context->highest_paging);
			
			this_cpu->current_thread = this_cpu->first_thread;
		}
		else 
		{
			
			curcontext= this_cpu->current_thread->proc->context;
			
			*this_cpu->current_thread->state = *cpu;
			
			if(this_cpu->current_thread->next != NULL)
			{
				next_context = virt_to_phys(curcontext,(uintptr_t)this_cpu->current_thread->next->proc->context->highest_paging);
				this_cpu->current_thread = this_cpu->current_thread->next;
			}
			else 
			{
				next_context = virt_to_phys(curcontext,(uintptr_t)this_cpu->first_thread->proc->context->highest_paging);
				this_cpu->current_thread = this_cpu->first_thread;
			}
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
int32_t switchto_thread(uint32_t t_id,struct cpu_state* cpu)
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
/*
struct thread* get_thread(uint32_t t_id)
{
	struct thread* cur=first_thread;
	while(cur->next!=NULL)
	{
		if(cur->t_id==t_id)
		{
			return cur;
		}
		else
		{
			cur = cur->next;
		}
	}
	return NULL;
}
*/
