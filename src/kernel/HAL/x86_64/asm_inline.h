/*   <src-path>/src/kernel/HAL/x86/asm_inline.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_64_ASMINLINE_H
#define X86_64_ASMINLINE_H

#include<stdint.h>
#include<mm/vmm.h>

#define OUTB( PORT, DATA) 	asm volatile ("outb %0, %1" : : "a" ((uint8_t)DATA), "Nd" ((uint16_t)PORT));
#define INB( PORT,DATA ) 	asm ("inb %1, %0" : "=a" (DATA) : "Nd" (PORT));

#define ENABLE_INTR		asm volatile("sti");
#define DISABLE_INTR		asm volatile("cli");

#define INVALIDATE_TLB(ADDR) 	asm volatile("invlpg %0" : : "m" (*(char*)ADDR));
#define ENABLE_PAGING 		// nothing because Paging is allready engabled when switching to long mode
#define SET_CONTEXT(PAGEDIR)	asm volatile("mov %0, %%cr3" : : "r" (PAGEDIR));

#define READ_MSR(MSR, LO, HI)	asm volatile("rdmsr": "=a"(LO), "=d"(HI) : "c"(MSR));
#define WRITE_MSR(MSR, LO, HI)	asm volatile("wrmsr": : "a"(LO), "d"(HI), : "c"(MSR));

#define LAST_ADDR 0xfffffffffffffff

#endif
