/*   <src-path>/src/kernel/inc/HAL/x86/cpu.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_64_CPU_H
#define X86_64_CPU_H

#include<stdint.h>
#include<dbg/console.h>
#include<string.h>
#include<mt/threads.h>

#include<macros.h>

#define REG_FUNCRET 	rax
#define REG_IP 		rip
#define REG_DATA0 	rdx
#define REG_DATA1 	rbx
#define REG_DATA2 	rcx
#define REG_STACKPTR 	rsp
#define REG_FLAGS 	rflags
#define INFO_INTR 	intr
#define INIT_STATE(STATE)		STATE->rax	= 0x0; \
					STATE->rbx	= 0x0; \
					STATE->rcx	= 0x0; \
					STATE->rdx	= 0x0; \
					STATE->rsi	= 0x0; \
					STATE->rdi	= 0x0; \
					STATE->rbp	= 0x0; \
					STATE->r8	= 0; \
					STATE->r9	= 0; \
					STATE->r10	= 0; \
					STATE->r11	= 0; \
					STATE->r12	= 0; \
					STATE->r13	= 0; \
					STATE->r14	= 0; \
					STATE->r15	= 0; \
					STATE->intr	= 0x0; \
					STATE->error	= 0x0; \
					STATE->rip	= 0x0; \
					STATE->cs	= ( USER_CODE_SEG32_N<<3 ) | 0x3; \
					STATE->ss	= ( USER_DATA_SEG_N<<3 ) | 0x3; \
					STATE->rsp	= 0x0; \
					STATE->rflags	= 0x202; 
#define SET_IRQ(FLAGS) FLAGS|0x200;

struct cpu_info bsp_info;

struct cpu_state{
	// manually secured registers
	uint64_t   rax;
	uint64_t   rbx;
	uint64_t   rcx;
	uint64_t   rdx;
	uint64_t   rsi;
	uint64_t   rdi;
	uint64_t   rbp;
	uint64_t   r8;
	uint64_t   r9;
	uint64_t   r10;
	uint64_t   r11;
	uint64_t   r12;
	uint64_t   r13;
	uint64_t   r14;
	uint64_t   r15;
	//uint64_t kernel;
	/*
	uint64_t ds;
	uint64_t es;
	uint64_t fs;
	uint64_t gs;
	*/
	uint64_t   intr;
	uint64_t   error;
	
	// secured by cpu
	uint64_t   rip;
	uint64_t   cs;
	uint64_t   rflags;
	uint64_t   rsp;
	uint64_t   ss;
};

struct cpu_info
{
	  uint8_t cpu_info_support;
	  uint32_t cpu_n;
	  uint32_t apic_id;
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
	  struct thread * first_thread;
	  struct thread * current_thread;
	  uintptr_t stack;
	  uint32_t thread_count;
	  uint8_t is_no_thread;
	  struct proc * cur_proc;
	  struct cpu_info * next;
};
void cpu_caps();
         /* Ring-0-Segmentregister nicht mehr benutzt*/
        //.cs  = 0x08,
	/* Ring-3-Segmentregister */   
	/* IRQs einschalten (IF = 1) */
#endif
