/*   <src-path>/src/kernel/HAL/x86/hw_structs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_HW_STRUCTS_H
#define X86_HW_STRUCTS_H

#include<mm/pmm.h>
struct tss_t
{
    unsigned int reserved;
    unsigned long long rsp0;
    unsigned long long rsp1;
    unsigned long long rsp2;
    unsigned long long reserved2;
    unsigned long long ist1;
    unsigned long long ist2;
    unsigned long long ist3;
    unsigned long long ist4;
    unsigned long long ist5;
    unsigned long long ist6;
    unsigned long long ist7;
    unsigned long long reserved3;
    unsigned short reserved4;
    unsigned short iomap;
} __attribute__((packed));
//------------------------------------------------------------------GDT-structs--------------------------------------------------------------------
struct gdt_entry
{
	unsigned short limit;
	unsigned int base :24;
	unsigned char accessbyte;
	unsigned int limit2 :4;
	unsigned int flags :4;
	unsigned char base2;
}__attribute__((packed));

//------------------------------------------------------------------IDT-structs--------------------------------------------------------------------
struct idt_entry
{
	unsigned short isr_offset;
	unsigned short selector;
	unsigned char ist;
	unsigned char flags;
	unsigned long long isr_offset2 :48;
	unsigned int reserved;
}__attribute__((packed));

//-----------------------------------------------------------------Paging-structures---------------------------------------------------------------
struct vmm_pagemap_level4
{
	unsigned char rw_flags; 
	unsigned char reserved :4;
	unsigned long long pagedirptrtbl_ptr :40;
	unsigned short NXnfree :12;
}__attribute__((packed));

struct vmm_pagedir_ptrtbl
{
	unsigned char rw_flags; 
	unsigned char reserved :4;
	unsigned long long pagedir_ptr :40;
	unsigned short NXnfree :12;
}__attribute__((packed));

struct vmm_pagedir
{
	unsigned char rw_flags; 
	unsigned char reserved :4;
	unsigned long long pagetbl_ptr :40;
	unsigned short NXnfree :12;
}__attribute__((packed));

struct vmm_pagetbl
{
	unsigned char rw_flags; 
	unsigned char reserved :4;
	unsigned long long page_ptr :40;	
	unsigned short NXnfree :12;
}__attribute__((packed));

struct stack_frame
{
	struct stack_frame * base_ptr;
	unsigned int return_addr;
}__attribute__((packed));
struct gdt_tss_entry
{
	unsigned short limit;
	unsigned int base :24;
	unsigned char accessbyte;
	unsigned char limit2 :4;
	unsigned char flags :4;
	unsigned long long base2 :40;
	unsigned char reserved;
	unsigned char zero :4;
	unsigned int reserved2 :20;
}__attribute__((packed));
#endif
