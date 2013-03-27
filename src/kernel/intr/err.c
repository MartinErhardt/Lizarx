/*   <src-path>/src/kernel/intr/err.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<intr/err.h>
#include<mt/cpustate.h>
#include<dbg/console.h>
#include<drv/vga-txt_graphics/vram.h>

void handle_exception(cpu_state* cpu){
	//redscreen(cpu);
	kprintf("err");
	while(1) {
	   // Prozessor anhalten
	   asm volatile("cli; hlt");
	}
}
void redscreen(cpu_state* cpu){
    clrscr(VGA_BLACK, VGA_RED);
    kprintfcol_scr(VGA_RED,VGA_BLACK,"A CRITICAL ERROR HAS OCURRED:\n"\
	    "Error: %d \n"\
	    "EAX: \t0x%p \t   EBX: \t0x%p \t   ECX: \t0x%p \t   EDX: \t0x%p \n"\
	    "ESI: \t0x%p \t   EDI: \t0x%p \n"\
	    "ESP: \t0x%p \t   EBP \t0x%p \n"\
	    "EIP: \t0x%p \n"\
	    "EFLAGS:\t0x%p \n"\
	    "CS:  \t0x%p  \t   SS: \t0x%p", 
	    cpu->intr,
	    cpu->eax, cpu->ebx, cpu->ecx, cpu->edx,
	    cpu->esi, cpu->edi,
	    cpu->esi, cpu->ebp,
	    cpu->eip,
	    cpu->eflags, 
	    cpu->cs, cpu->ss
 	  );
}
