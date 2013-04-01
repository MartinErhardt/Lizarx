/*   <src-path>/src/kernel/mt/threads.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<stdint.h>
#include<mt/threads.h>
#include<hal.h>
#include<mt/proc.h>
#include<mm/vheap.h>
#include<mm/vmm.h>
#include<dbg/console.h>

static uint32_t num_threads = 0;

int32_t create_thread(void* entry,uint32_t p_id)
{
	struct proc* in_proc=get_proc(p_id);
	struct thread* new_t=(struct thread*)kmalloc(sizeof(struct thread));
	
	CPU_STATE* new_st_=(CPU_STATE*)kmalloc(sizeof(CPU_STATE));
	uint8_t* user_stack 	= uvmm_malloc(in_proc->context, STDRD_STACKSIZ);

	if(in_proc==NULL){
	    kprintf("couldn't get pid");
	    return -1;
	}
	num_threads++;
	
	NEW_STATE
	*new_st_=new_state;
	new_st_->eip = (uintptr_t) entry;
#ifdef ARCH_X86
	new_st_->cs = 0x18 | 0x03;
	new_st_->ss = 0x20 | 0x03;
	SET_IRQ(new_st_->REG_FLAGS)
#endif
	new_t->t_id=num_threads;
	new_t->state = new_st_;
	new_t->stack=kvmm_malloc(PAGE_SIZE);
	new_t->user_stack 	= user_stack;
	new_t->proc=in_proc;
	new_t->state->esp= (uintptr_t) user_stack+STDRD_STACKSIZ;
	new_t->next=first_thread;
	first_thread = new_t;
	//kprintf("stackphys at 0x%x stackvirt at 0x%x stack at 0x%x",virt_to_phys(&startup_context,(uintptr_t)new_st_),(uintptr_t)new_st_,new_t->stack);
	return 0;
}
CPU_STATE* dispatch_thread(CPU_STATE* cpu){
   
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
    //kprintf("hi");
    if (current_thread == NULL) {

 	curcontext= &startup_context;
	next_context = virt_to_phys(curcontext,(uintptr_t)first_thread->proc->context->pd);
	
        current_thread = first_thread;

    } 
    else 
    {
	curcontext= current_thread->proc->context;
	*current_thread->state = *cpu;
	if(current_thread->next != NULL)
	{
		next_context = virt_to_phys(curcontext,(uintptr_t)current_thread->next->proc->context->pd);
		current_thread = current_thread->next;
	}
	else 
	{
	    next_context = virt_to_phys(curcontext,(uintptr_t)first_thread->proc->context->pd);
            current_thread = first_thread;
        }
    }

    /* Prozessorzustand des neuen Tasks aktivieren */
    cpu = current_thread->state;
    
#ifdef ARCH_X86
	cpu->cs = 0x18 | 0x03;
	cpu->ss = 0x20 | 0x03;
	SET_IRQ(cpu->REG_FLAGS)
#endif
    //kprintf("cur=0x%x ",(uintptr_t)current_thread->state->ss);
    /*
    if(!next_context) {
	kprintf("next context at 0x%x",(uintptr_t)first_thread->proc->context);
	while(1){}
    }*/
    
    if(current_thread->proc->context!=curcontext){
	SET_CONTEXT(next_context);
    }

    return cpu;
}
int32_t switchto_thread(uint32_t t_id,CPU_STATE* cpu){

    struct thread*prev=current_thread;
    struct thread*switch_to=get_thread(t_id);
    vmm_context* curcontext=NULL;
    
    if(!intr_activated){
        curcontext= &startup_context;
	//kprintf("her");
    }else{
        curcontext= current_thread->proc->context;
    }
    if(switch_to==NULL){
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
    
    if(current_thread->proc!=prev->proc){
	
	SET_CONTEXT(virt_to_phys(curcontext,(uintptr_t)current_thread->proc->context));
    }
    return 0;
}
struct thread* get_thread(uint32_t t_id){
	struct thread* cur=first_thread;
	while(cur->next!=NULL){
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