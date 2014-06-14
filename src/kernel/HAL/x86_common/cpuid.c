/*   <src-path>/src/kernel/HAL/x86_common/cpuid.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <stdint.h>
#include <string.h>
#include <dbg/console.h>
#include <cpu.h>
#include <libOS/mmio.h>
#include <local_apic.h>
#include <mm/vheap.h>
#include <mm/vmm.h>
#include<libOS/list.h>

uint8_t bsp_filled=0;
// this function can only be used at the beginning
void cpu_caps()
{
	uint8_t * cpuid_support =(uint8_t *) 0x7200;
	struct cpu_info * this_cpu = &bsp_info;
	if(bsp_filled)
		this_cpu = kmalloc(sizeof(struct cpu_info));
	else
		bsp_filled=1;
	memset((void*)this_cpu, 0x00, sizeof(struct cpu_info));
	alist_add(&cpu_list, (void*) this_cpu);
#ifdef ARCH_X86_64
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
	    "cpuid_unsupported: xor %eax, %eax; mov %eax, 0x7200;jmp out;" // FIXME
	    "cpu_supported: mov $1, %eax; mov %eax, 0x7200; jmp out;"
	    "out:");
#else
	asm volatile(
	    "pushf;"
	    "pop %ecx;"
	    "mov %ecx, %eax;"
	    "xor $0x200000, %eax;"
	    "push %eax;"
	    "popf;"
	    "pushf;"
	    "pop %eax;"
	    "xor %ecx, %eax;"
	    "je cpuid_unsupported;"
	    "jmp cpu_supported;"
	    "cpuid_unsupported: xor %eax, %eax; mov %eax, 0x7200;jmp out;" // FIXME
	    "cpu_supported: mov $1, %eax; mov %eax, 0x7200; jmp out;"
	    "out:");
#endif
	if(*cpuid_support)
		this_cpu->cpu_info_support=1;
	else
		return;
	
	uint_t func =0x0; 
	uint_t ebx =0x0; 
	uint_t ecx =0x0; 
	uint_t edx = 0x0;
	asm volatile( "cpuid" : "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
	memcpy(&this_cpu->vendor_id[0],&ebx,4);
	memcpy(&this_cpu->vendor_id[4],&edx,4);
	memcpy(&this_cpu->vendor_id[8],&ecx,4);
	kprintf(&this_cpu->vendor_id[0]);
	if(!strcmp(&this_cpu->vendor_id[0], "GenuineIntel"))
	{
		func = 0x80000002;
		asm volatile( "cpuid" : "=a"(func), "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
		memcpy(&this_cpu->cpu_name[0],&func,4);
		memcpy(&this_cpu->cpu_name[4],&ebx,4);
		memcpy(&this_cpu->cpu_name[8],&ecx,4);    
		memcpy(&this_cpu->cpu_name[12],&edx,4);  
		func = 0x80000003;
		asm volatile( "cpuid" : "=a"(func), "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
		memcpy(&this_cpu->cpu_name[16],&func,4);
		memcpy(&this_cpu->cpu_name[20],&ebx,4);
		memcpy(&this_cpu->cpu_name[24],&ecx,4);    
		memcpy(&this_cpu->cpu_name[28],&edx,4);  
		func = 0x80000004;
		asm volatile( "cpuid" : "=a"(func), "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
		memcpy(&this_cpu->cpu_name[32],&func,4);
		memcpy(&this_cpu->cpu_name[36],&ebx,4);
		memcpy(&this_cpu->cpu_name[40],&ecx,4);    
		memcpy(&this_cpu->cpu_name[44],&edx,4);  
		kprintf(&this_cpu->cpu_name[0]);
		kprintf("\n");
	}
	func=0x00000001;
	asm volatile( "cpuid" : "=b"(ebx), "=c" (ecx), "=d" (edx) : "a"(func));
	kprintf("cpuid_flags: ");
	if(edx&0x2000000)
	{
		this_cpu->sse_support=1;
		kprintf(", sse_1");
	}
	if(edx&0x4000000)
	{
		this_cpu->sse_support=1;
		kprintf(", sse_2");
	}
	if(ecx&0x1)
	{
		this_cpu->sse_3_support=1;
		kprintf(", sse3");
	}
	if(ecx&0x200)
	{
		this_cpu->ssse_3_support=1;
		kprintf(", ssse3");
	}
	if(ecx&0x80000)
	{
		this_cpu->sse_4_1_support=1;
		kprintf(", sse_4.1");
	}
	if( !strcmp(&this_cpu->vendor_id[0], "GenuineIntel") && ecx&0x100000 )
	{
		this_cpu->sse_4_2_support=1;
		kprintf(", sse_4.2");
	}
	this_cpu->apic_id=(mmio_read32(local_apic_virt,LOCAL_APIC_ID_REG)>>24);
	
#ifdef ARCH_X86_64
	memset(&(this_cpu->idle_state),0x0, sizeof(struct cpu_state));
	
	void * stack = kvmm_malloc(PAGE_SIZE);
	
	this_cpu->idle_state.cs = (KERNEL_CODE_SEG_N<<3);
	this_cpu->idle_state.ss = (KERNEL_STACK_SEG_N<<3);
	
	this_cpu->idle_state.REG_STACKPTR =(uintptr_t)stack+PAGE_SIZE-0x10;
	kprintf("at 0x%p",this_cpu->idle_state.REG_STACKPTR);
	this_cpu->idle_state.REG_FLAGS = 0x200;
	this_cpu->idle_state.REG_IP = (uintptr_t)&idle_thread;
#endif
	
	kprintf("\n");
}
