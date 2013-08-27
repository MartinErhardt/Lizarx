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

// this function can only be used at the beginning
void cpu_caps()
{
	uint8_t * cpuid_support =(uint8_t *) 0x7200;
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
	{
		bsp_info.cpu_info_support=1;
	}
	else
	{
		return;
	}
	memset(&bsp_info, 0x00, sizeof(struct cpu_info));
	uint_t func =0x0; 
	uint_t ebx =0x0; 
	uint_t ecx =0x0; 
	uint_t edx = 0x0;
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
