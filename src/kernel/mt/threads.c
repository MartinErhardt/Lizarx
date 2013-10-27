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

static uint32_t num_threads = 0;

int32_t create_thread(void* entry,uint32_t p_id)
{
	struct proc* in_proc=get_proc(p_id);
	struct thread* new_t=(struct thread*)malloc(sizeof(struct thread));
	
	cpu_state* new_state=(cpu_state*)malloc(0xb0);
	uint8_t* user_stack 	= uvmm_malloc(in_proc->context, STDRD_STACKSIZ);
	if(in_proc==NULL)
	{
	    kprintf("couldn't get pid");
	    return -1;
	}
	num_threads++;
	
	INIT_STATE(new_state);
	new_state->REG_IP = (uintptr_t) entry;
	new_state->REG_STACKPTR= (uintptr_t) user_stack+STDRD_STACKSIZ -0x20 ;
	
	new_t->t_id=num_threads;
	new_t->state = new_state;
	new_t->user_stack 	= user_stack;
	new_t->proc=in_proc;
	new_t->next=first_thread;
	first_thread = new_t;
	return 0;
}
cpu_state* dispatch_thread(cpu_state* cpu)
{
	vmm_context* curcontext=NULL;
	uintptr_t next_context= 0x0;

	/*
	* Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
	* gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
	* wir spaeter nicht wieder.
	*/
	/*
	* Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
	*/
	if (current_thread == NULL) 
	{
		curcontext= &startup_context;
		
		next_context = virt_to_phys(curcontext,(uintptr_t)first_thread->proc->context->highest_paging);
		current_thread = first_thread;
	}
	else 
	{
		curcontext= current_thread->proc->context;
		//kprintf("ss=0x%x cs=0x%x eflags=0x%x",cpu->ss,cpu->cs,cpu->REG_FLAGS);
		
		*current_thread->state = *cpu;
		
		if(current_thread->next != NULL)
		{
			next_context = virt_to_phys(curcontext,(uintptr_t)current_thread->next->proc->context->highest_paging);
			//kprintf("next_contextc 0x%x real context 0x%x",next_context,(uintptr_t) first_thread->proc->context->pd);
			current_thread = current_thread->next;
		}
		else 
		{
			next_context = virt_to_phys(curcontext,(uintptr_t)first_thread->proc->context->highest_paging);
			//kprintf("next_contextaa 0x%x real context 0x%x",next_context,(uintptr_t) first_thread->proc->context->pd);
			current_thread = first_thread;
		}
	}
#ifdef ARCH_X86
	current_thread->state->ss = 0x23;
	tss.esp0 = ((uintptr_t) cpu)+sizeof(cpu_state);
#endif
	/* Prozessorzustand des neuen Tasks aktivieren */
	cpu = current_thread->state;
	//cpu->rsp= current_thread->state->rsp;
	cur_proc=current_thread->proc;
	
	if(current_thread->proc->context!=curcontext)
	{
		SET_CONTEXT(next_context);;
	}
	return cpu;
}
int32_t switchto_thread(uint32_t t_id,cpu_state* cpu)
{

	struct thread*prev=current_thread;
	struct thread*switch_to=get_thread(t_id);
	vmm_context* curcontext=get_cur_context();
	
	if(switch_to==NULL)
	{
	    return -1;
	}
	/*
	* Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
	* gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
	* wir spaeter nicht wieder.
	*/
	/*
	* Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
	*/
	current_thread->state = cpu;
	current_thread = switch_to;
	
	/* Prozessorzustand des neuen Tasks aktivieren */
	cpu = current_thread->state;
#ifdef ARCH_X86
	current_thread->state->ss = 0x23;
	tss.esp0 = ((uintptr_t) cpu)+sizeof(cpu_state);
#endif
	if(current_thread->proc!=prev->proc)
	{
	    SET_CONTEXT(virt_to_phys(curcontext,(uintptr_t)current_thread->proc->context));
	    
	}
	return 0;
}
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
