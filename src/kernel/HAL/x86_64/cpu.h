/*   <src-path>/src/kernel/HAL/x86/cpu.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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

#define REG_FUNCRET 	rax
#define REG_IP 		rip
#define REG_DATA0 	rdx
#define REG_DATA1 	rbx
#define REG_DATA2 	rcx
#define REG_STACKPTR 	rsp
#define REG_FLAGS 	rflags
#define INFO_INTR 	intr
#define INIT_STATE(STATE)		STATE->rax=0x0; \
					STATE->rbx=0x0; \
					STATE->rcx=0x0; \
					STATE->rdx=0x0; \
					STATE->rsi=0x0; \
					STATE->rdi=0x0; \
					STATE->rbp=0x0; \
					STATE->r8=0; \
					STATE->r9=0; \
					STATE->r10=0; \
					STATE->r11=0; \
					STATE->r12=0; \
					STATE->r13=0; \
					STATE->r14=0; \
					STATE->r15=0; \
					STATE->intr=0x0; \
					STATE->error=0x0; \
					STATE->rip=0x0; \
					STATE->cs=0x18 | 0x03; \
					STATE->ss=0x23; \
					STATE->rsp=0x0; \
					STATE->rflags=0x202; 
#define SET_IRQ(FLAGS) FLAGS=0x200;

struct cpu_info bsp_info;

typedef struct {
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
    

    uint64_t   intr;
    uint64_t   error;

    // secured by cpu
    uint64_t   rip;
    uint64_t   cs;
    uint64_t   rflags;
    uint64_t   rsp;
    uint64_t   ss;
} cpu_state;

struct cpu_info
{
	  uint8_t cpu_info_support;
	  uint32_t cpu_n;
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
	  struct cpu_info * next;
};
// this function can only be used at the beginning
static inline void cpu_caps()
{
	uint8_t * cpuid_support =(uint8_t *) 0x1000;
	asm volatile(
	    "pushf;"
	    "pop %rcx;"
	    "mov %rcx, %rax;"
	    "xor $0x200000, %rax;"
	    "push %rax;"
	    "popf;"
	    "pushf;"
	    "pop %rax;"
	    "xor %ecx, %eax;"
	    "je cpuid_unsupported;"
	    "jmp cpu_supported;"
	    "cpuid_unsupported: xor %eax, %eax; mov %eax, 0x1000;jmp out;" // FIXME
	    "cpu_supported: mov $1, %eax; mov %eax, 0x1000; jmp out;"
	    "out:");
	if(*cpuid_support){
		bsp_info.cpu_info_support=1;
	}
	else
	{
		return;
	}
	memset(&bsp_info, 0x00, sizeof(struct cpu_info));
	uint64_t func =0x0; 
	uint64_t ebx =0x0; 
	uint64_t ecx =0x0; 
	uint64_t edx = 0x0;
	asm volatile( "cpuid" : "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
	memcpy(&bsp_info.vendor_id[0],&ebx,4);
	memcpy(&bsp_info.vendor_id[4],&edx,4);
	memcpy(&bsp_info.vendor_id[8],&ecx,4);
	if(!strcmp(&bsp_info.vendor_id[0], "GenuineIntel"))
	{
		func = 0x80000002;
		asm volatile( "cpuid" : "=a"(func), "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
		memcpy(&bsp_info.cpu_name[0],&func,4);
		memcpy(&bsp_info.cpu_name[4],&ebx,4);
		memcpy(&bsp_info.cpu_name[8],&ecx,4);    
		memcpy(&bsp_info.cpu_name[12],&edx,4);  
		func = 0x80000003;
		asm volatile( "cpuid" : "=a"(func), "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
		memcpy(&bsp_info.cpu_name[16],&func,4);
		memcpy(&bsp_info.cpu_name[20],&ebx,4);
		memcpy(&bsp_info.cpu_name[24],&ecx,4);    
		memcpy(&bsp_info.cpu_name[28],&edx,4);  
		func = 0x80000004;
		asm volatile( "cpuid" : "=a"(func), "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
		memcpy(&bsp_info.cpu_name[32],&func,4);
		memcpy(&bsp_info.cpu_name[36],&ebx,4);
		memcpy(&bsp_info.cpu_name[40],&ecx,4);    
		memcpy(&bsp_info.cpu_name[44],&edx,4);  
		kprintf(&bsp_info.cpu_name[0]);
		kprintf("\n");
	}
	func=0x00000001;
	asm volatile( "cpuid" : "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
	kprintf("cpuid_flags: ");
	if(edx&0x2000000)
	{
		bsp_info.sse_support=1;
		kprintf(", sse_1");
	}
	if(edx&0x4000000)
	{
		bsp_info.sse_support=1;
		kprintf(", sse_2");
	}
	if(ecx&0x1)
	{
		bsp_info.sse_3_support=1;
		kprintf(", sse3");
	}
	if(ecx&0x200)
	{
		bsp_info.ssse_3_support=1;
		kprintf(", ssse3");
	}
	if(ecx&0x80000)
	{
		bsp_info.sse_4_1_support=1;
		kprintf(", sse_4.1");
	}
	if((!strcmp(&bsp_info.vendor_id[0], "GenuineIntel"))&&(ecx&0x100000))
	{
		bsp_info.sse_4_2_support=1;
		kprintf(", sse_4.2");
	}
	kprintf("\n");
}
         /* Ring-0-Segmentregister nicht mehr benutzt*/
        //.cs  = 0x08,
	/* Ring-3-Segmentregister */   
	/* IRQs einschalten (IF = 1) */
#endif
