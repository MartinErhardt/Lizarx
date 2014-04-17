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
#include<mm/gdt.h>
#include<asm_inline.h>
#include<libOS/lock.h>
#include<smp_capabilities.h>
#include<cpu.h>
#include<../x86_common/local_apic.h>

static uint32_t num_threads = 0;

int32_t create_thread(void* entry,uint32_t p_id)
{
	struct proc* in_proc=get_proc(p_id);
	struct thread* new_t=(struct thread*)malloc(sizeof(struct thread));
	
	struct cpu_state* new_state=(struct cpu_state*)malloc(sizeof(struct cpu_state));
	
	total_thread_count++;
	
	struct cpu_info * cpu = get_best_cpu();
	
	uint8_t* user_stack 	= uvmm_malloc(in_proc->context, STDRD_STACKSIZ);
	
	if(in_proc==NULL)
	{
	    kprintf("couldn't get pid");
	    return -1;
	}
	num_threads++;
	
	INIT_STATE(new_state);
	spinlock_ackquire(&multi_threading_lock);
	new_state->REG_IP = (uintptr_t) entry;
	new_state->REG_STACKPTR= (uintptr_t) user_stack+STDRD_STACKSIZ -0x8 ;
	
	new_t->t_id=num_threads;
	new_t->state = new_state;
	new_t->user_stack = user_stack;
	new_t->proc=in_proc;
	new_t->next=cpu->first_thread;
	cpu->first_thread = new_t;
	cpu->thread_count++;
	cpu->is_no_thread = 0;
	spinlock_release(&multi_threading_lock);
	return 0;
}
struct cpu_info * get_best_cpu()
{
	uint32_t average_thread_count=(total_thread_count*10)/cores_from_tables;// 1: 2 2: 5
	struct cpu_info * this_cpu = &bsp_info;
	struct cpu_info * best_fit = &bsp_info;
	int32_t best_fit_diff = (((&bsp_info)->thread_count+1)*10)-average_thread_count;// 1: 12 2: 15
	int32_t diff_with_process;
	if(best_fit_diff<0)
			best_fit_diff = best_fit_diff*(-1);
	spinlock_ackquire(&multi_threading_lock);
	while(this_cpu!=NULL) 
	{
		diff_with_process = ((this_cpu->thread_count+1)*10)-average_thread_count; // 1.1: 8 2.1: 8 1.2: 15 2.2: 5
		
		if(diff_with_process<0)
			diff_with_process = diff_with_process*(-1);
		//kprintf("diff_with_process 0x%x\n",diff_with_process);
		if(diff_with_process<best_fit_diff)
		{
			best_fit = this_cpu;
			best_fit_diff = diff_with_process;
		}
		this_cpu=this_cpu->next;
	}
	spinlock_release(&multi_threading_lock);
	return best_fit;
}
struct cpu_info * move_if_it_make_sense(struct cpu_info * this_cpu,struct thread * to_move)
{
	this_cpu->thread_count--;
	uint32_t average_thread_count=(total_thread_count*10)/cores_from_tables;
	struct cpu_info * best_fit = this_cpu; 
	struct cpu_info * cur_cpu = &bsp_info;
	int32_t best_fit_diff = (((&bsp_info)->thread_count+1)*10)-average_thread_count;// 1: 12 2: 15
	int32_t diff_with_thread;
	struct thread * last_thread = this_cpu->first_thread;
	if(best_fit_diff<0)
			best_fit_diff = best_fit_diff*(-1);
	while(cur_cpu->next!=NULL) 
	{
		cur_cpu=cur_cpu->next;
		diff_with_thread = ((cur_cpu->thread_count+1)*10)-average_thread_count;
		if(diff_with_thread<0)
			diff_with_thread = diff_with_thread*(-1);
		if(diff_with_thread<best_fit_diff)
		{
			best_fit_diff = diff_with_thread;
			best_fit = cur_cpu;
		}
	}
	if(best_fit!=this_cpu)
	{
		if(last_thread!=to_move)
		{
			while( (last_thread->next != to_move)&&(last_thread->next!=NULL) )
			{
				last_thread = last_thread->next;
			}
			last_thread->next = to_move->next;
		}
		else
		{
			kprintf("hi");
			this_cpu->is_no_thread=1;
			this_cpu->first_thread=NULL;
		}
		
		to_move->next = best_fit->first_thread;
		best_fit->first_thread = to_move;
		//cur_cpu->first_thread=
	}
	this_cpu->thread_count++;
	return best_fit;
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
	
	//if(this_cpu->apic_id!=0)
	//	while(1);
	spinlock_ackquire(&multi_threading_lock);
	do {
		if (this_cpu->current_thread == NULL)
		{
			curcontext= &startup_context;
			next_context = virt_to_phys(curcontext,(uintptr_t)this_cpu->first_thread->proc->context->highest_paging);
			//kprintf("next con: 0x%x apic_id 0x%x\n",next_context,this_cpu->apic_id);
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
	}
	while(move_if_it_make_sense(this_cpu,this_cpu->current_thread)!=this_cpu);
	spinlock_ackquire(&this_cpu->is_no_thread);
	spinlock_release(&this_cpu->is_no_thread);
#ifdef ARCH_X86								// #WHY
	this_cpu->current_thread->state->ss = 0x23;
	tss.esp0 = ((uintptr_t) cpu)+sizeof(struct cpu_state);
#endif
	/* Prozessorzustand des neuen Tasks aktivieren */
	cpu = this_cpu->current_thread->state;
	this_cpu->cur_proc=this_cpu->current_thread->proc;
	if(this_cpu->current_thread->proc->context!=curcontext)
	{
		SET_CONTEXT(next_context)
	} 
	//kprintf("id: %x",this_cpu->apic_id);
	spinlock_release(&multi_threading_lock);
	return cpu;
}
int32_t switchto_thread(uint32_t t_id,struct cpu_state* cpu)
{
/*
	struct thread*prev=current_thread;
	struct thread*switch_to=get_thread(t_id);
	vmm_context* curcontext=get_cur_context();
	
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