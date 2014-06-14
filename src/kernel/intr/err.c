/*   <src-path>/src/kernel/intr/err.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<intr/err.h>
#include<cpu.h>
#include<dbg/console.h>
#include<dbg/stack_trace.h>
#include<drv/vga-txt_graphics/vram.h>
#include<boot/init.h>
#include<stdlib.h>
#include<../x86_common/local_apic.h>

void handle_exception(struct cpu_state* cpu)
{
	err_ocurred = 1;
	kprintf("\nError on core: 0x%x\n", get_cur_cpu()->apic_id );
	redscreen(cpu);
	
	cpu_halt();
}
void cpu_halt()
{
	while(1) asm volatile("cli; hlt");
}
void redscreen(struct cpu_state* cpu)
{
	//clrscr(VGA_BLACK, VGA_RED);
#ifdef ARCH_X86_64
	uint64_t cr2;
	uint64_t cr3;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2) : : "memory");
	asm volatile ("mov %%cr3, %0" : "=r" (cr3) : : "memory");
	kprintfcol_scr(VGA_RED,VGA_BLACK,"\nA CRITICAL ERROR HAS OCURRED:\n"\
	    "Error: %d \n"\
	    "Intr: %d \n"\
	    "RAX: 0x%x \tRBX: 0x%x \tRCX: 0x%x \tRDX: 0x%x\n"\
	    "RSI: 0x%x \tRDI: 0x%x\n"\
	    "RSP: 0x%x \tRBP 0x%x\n"\
	    "RIP: 0x%x \n"\
	    "RFLAGS:0x%x\n"\
	    "R8: 0x%x \tR9: 0x%x \tR10 0x%x \tR11 0x%x\n"
	    "R12 0x%x \tR13 0x%x \tR14 0x%x \tR15 0x%x\n"
	    "CS: 0x%x \tSS: 0x%x \tCR2 0x%x \tCR3 0x%x\n", 
	    cpu->error,
	    cpu->intr,
	    cpu->rax, cpu->rbx, cpu->rcx, cpu->rdx,
	    cpu->rsi, cpu->rdi,
	    cpu->rsp, cpu->rbp,
	    cpu->rip,
	    cpu->rflags, 
	    cpu->r8, cpu->r9, cpu->r10, cpu->r11, 
	    cpu->r12, cpu->r13, cpu->r14, cpu->r15,
	    cpu->cs, cpu->ss, cr2, cr3
	  );
	
#endif
#ifdef ARCH_X86
	uint32_t cr2;
	asm volatile ("mov %%cr2, %0" : "=r" (cr2) : : "memory");
	kprintfcol_scr(VGA_RED,VGA_BLACK,"\nA CRITICAL ERROR HAS OCURRED:\n"\
	    "Error: %d \n"\
	    "Intr: %d \n"\
	    "EAX: 0x%x \tEBX: 0x%x\tECX: 0x%x\tEDX: 0x%x\n"\
	    "ESI: 0x%x\tEDI: 0x%x\n"\
	    "ESP: 0x%x\tEBP 0x%x\n"\
	    "EIP: 0x%x \n"\
	    "EFLAGS:0x%x\n"\
	    "CS:  0x%x\tSS: 0x%x\tCR2 0x%x\n", 
	    cpu->error,
	    cpu->intr,
	    cpu->eax, cpu->ebx, cpu->ecx, cpu->edx,
	    cpu->esi, cpu->edi,
	    cpu->esi, cpu->ebp,
	    cpu->eip,
	    cpu->eflags, 
	    cpu->cs, cpu->ss, cr2
	  );
	get_stack_trace(kernel_elf,(uintptr_t)cpu->ebp,(uintptr_t)cpu->eip);
/*#else
	  #error lizarx build: No valid arch found in src/kernel/intr/err.c*/
#endif

}
