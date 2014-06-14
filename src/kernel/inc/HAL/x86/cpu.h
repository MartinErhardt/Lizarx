/*   <src-path>/src/kernel/inc/HAL/x86/cpu.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_CPU_H
#define X86_CPU_H

#include<stdint.h>
#include<dbg/console.h>
#include<libOS/list.h>
#include<macros.h>

#define REG_FUNCRET 	eax
#define REG_IP 		eip
#define REG_DATA0 	edx
#define REG_DATA1 	ebx
#define REG_DATA2 	ecx
#define REG_STACKPTR 	esp
#define REG_FLAGS 	eflags
#define INFO_INTR 	intr

#define INIT_STATE(STATE)		STATE->eax=0x0; \
					STATE->ebx=0x0; \
					STATE->ecx=0x0; \
					STATE->edx=0x0; \
					STATE->esi=0x0; \
					STATE->edi=0x0; \
					STATE->ebp=0x0; \
					STATE->intr=0x0; \
					STATE->error=0x0; \
					STATE->eip=0x0; \
					STATE->cs= ( USER_CODE_SEG32_N<<3 ) | 0x3; \
					STATE->ss=( USER_DATA_SEG_N<<3 ) | 0x3; \
					STATE->esp=0x0; \
					STATE->eflags=0x202; 

#define SET_IRQ(FLAGS) FLAGS=0x202;

struct cpu_info bsp_info;
alist_t cpu_list;
struct cpu_state{
    // manually secured registers
    uint32_t   eax;
    uint32_t   ebx;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   esi;
    uint32_t   edi;
    uint32_t   ebp;

    uint32_t   intr;
    uint32_t   error;

    // secured by cpu
    uint32_t   eip;
    uint32_t   cs;
    uint32_t   eflags;
    uint32_t   esp;
    uint32_t   ss;
};

struct cpu_info
{
	  uint8_t cpu_info_support;
	  uint32_t cpu_n;
	  uint8_t apic_id;
	  uint8_t sse_support;
	  uint8_t sse_2_support;
	  uint8_t sse_3_support;
	  uint8_t ssse_3_support;
	  uint8_t sse_4_1_support;
	  uint8_t sse_4_2_support;
	  uint8_t sysenter_exit_support;
	  uint8_t apic_support;
	  uint8_t hyperthreading;
	  char vendor_id[12];
	  char cpu_name[48];
	  alist_t thread_list;
	  struct thread * current_thread;
	  uint_t current_thread_index;
	  uint32_t thread_count;
	  uintptr_t stack;
	  lock_t is_no_thread;
	  struct proc * cur_proc;
};

void cpu_caps();
         /* Ring-0-Segmentregister nicht mehr benutzt*/
        //.cs  = 0x08,
	/* Ring-3-Segmentregister */   
	/* IRQs einschalten (IF = 1) */
#endif
