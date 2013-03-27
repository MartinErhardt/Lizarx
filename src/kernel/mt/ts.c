/*   <src-path>/src/kernel/mt/ts.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <mt/cpustate.h>
#include <mt/ts.h>
#include <mm/pmm.h>
#include <mm/vheap.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <dbg/console.h>
#include <intr/idt.h>

static uint32_t num_tasks = 0;


/*
 * Jeder Task braucht seinen eigenen Stack, auf dem er beliebig arbeiten kann,
 * ohne dass ihm andere Tasks Dinge ueberschreiben. Ausserdem braucht ein Task
 * einen Einsprungspunkt.
 */
struct task* init_task(void* entry)
{	
    uint8_t* stack = kmalloc(PAGE_SIZE);

    num_tasks++;
    /*
     * CPU-Zustand fuer den neuen Task festlegen
     */
    cpu_state new_state = {
        .eax = 0,
        .ebx = 0,
        .ecx = 0,
        .edx = 0,
        .esi = 0,
        .edi = 0,
        .ebp = 0,
        .esp = 0x0,
        .eip = (uintptr_t) entry,
 
        /* Ring-0-Segmentregister nicht mehr benutzt*/
        //.cs  = 0x08,
	/* Ring-3-Segmentregister */
        .cs  = 0x18 | 0x03,
        .ss  = 0x20 | 0x03,
        /* IRQs einschalten (IF = 1) */
        .eflags = 0x202,
    };
    
    /*
     * Den angelegten CPU-Zustand auf den Stack des Tasks kopieren, damit es am
     * Ende so aussieht als waere der Task durch einen Interrupt unterbrochen
     * worden. So kann man dem Interrupthandler den neuen Task unterschieben
     * und er stellt einfach den neuen Prozessorzustand "wieder her".
     */
    cpu_state* state = (void*) ((uintptr_t)stack + PAGE_SIZE - sizeof(new_state)-1);    
    *state = new_state;
    
    struct task* task = kmalloc(sizeof(struct task));
    vmm_context *new_con = kmalloc(sizeof(vmm_context));
    *new_con =vmm_crcontext();
    
    task->state = state;
    task->pid = num_tasks;
    task->stack = stack;
    
    task->next = first_task;
    
    task->context = new_con;
    
    first_task = task;
    
    uint8_t* user_stack = uvmm_malloc(first_task->context, STDRD_STACKSIZE);
    first_task->user_stack 	= user_stack;
    first_task->state->esp	= (uintptr_t) user_stack+STDRD_STACKSIZE-4;
    
    return task;
}

/*
 * Gibt den Prozessorzustand des naechsten Tasks zurueck. Der aktuelle
 * Prozessorzustand wird als Parameter uebergeben und gespeichert, damit er
 * beim naechsten Aufruf des Tasks wiederhergestellt werden kann
 */
cpu_state* schedule(cpu_state* cpu)
{
    /*
     * Wenn schon ein Task laeuft, Zustand sichern. Wenn nicht, springen wir
     * gerade zum ersten Mal in einen Task. Diesen Prozessorzustand brauchen
     * wir spaeter nicht wieder.
     */
    if (current_task != NULL) {
        current_task->state = cpu;
    }
    
    /*
     * Naechsten Task auswaehlen. Wenn alle durch sind, geht es von vorne los
     */
    if (current_task == NULL) {
        current_task = first_task;
    } else {
        current_task = current_task->next;
        if (current_task == NULL) {
            current_task = first_task;
        }
    }
    //kprintf("cur_task0x%x",(uintptr_t)current_task);
    /* Prozessorzustand des neuen Tasks aktivieren */
    cpu = current_task->state;
    
    vmm_set_context(current_task->context);
    
    return cpu;
}