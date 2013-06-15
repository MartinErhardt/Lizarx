/*   <src-path>/src/kernel/HAL/x86/hw_structs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_HW_STRUCTS_H
#define X86_HW_STRUCTS_H

#include<mm/pmm.h>

//------------------------------------------------------------------GDT-structs--------------------------------------------------------------------
struct gdt_entry
{
	unsigned short 	limit;
	unsigned int 	base :24;
	unsigned char 	accessbyte;
	unsigned int 	limit2 :4;
	unsigned int 	flags :4;
	unsigned char 	base2;
}__attribute__((packed));

//------------------------------------------------------------------IDT-structs--------------------------------------------------------------------
struct idt_entry
{
    unsigned short isr_offset;
    unsigned short selector;
    unsigned char ignore;
    unsigned char flags;
    unsigned short isr_offset2;
}__attribute__((packed));
//-----------------------------------------------------------------Paging-structures---------------------------------------------------------------
struct vmm_pagemap_level4_entr
{
    unsigned char rw_flags; 
    unsigned char reserved :4;
    unsigned long long pagedirptrtbl_ptr :40;
    unsigned short NXnfree :12;
}__attribute__((packed));

struct vmm_pagedir_ptrtbl_entr
{
    unsigned char rw_flags; 
    unsigned char reserved :4;
    unsigned long long pagedir_ptr :40;
    unsigned short NXnfree :12;
}__attribute__((packed));

struct vmm_pagedirentr
{
    unsigned char rw_flags; 
    unsigned char reserved :4;
    unsigned long long pagetbl_ptr :40;
    unsigned short NXnfree :12;
}__attribute__((packed));

struct vmm_pagetblentr
{
    unsigned char rw_flags; 
    unsigned char reserved :4;
    unsigned long long page_ptr :40;
    unsigned short NXnfree :12;
}__attribute__((packed));

struct vmm_pagemap_level4
{
    struct vmm_pagemap_level4_entr pgdir[512];
}__attribute__((packed, aligned(PAGE_SIZE)));

struct vmm_pagedir_ptrtbl
{
    struct vmm_pagedir_ptrtbl_entr pgdir[512];
}__attribute__((packed, aligned(PAGE_SIZE)));

struct vmm_pagedir
{
    struct vmm_pagedirentr pgdir[512];
}__attribute__((packed, aligned(PAGE_SIZE)));

struct vmm_pagetbl
{
    struct vmm_pagetblentr pgtbl[512];
}__attribute__((packed, aligned(PAGE_SIZE)));

struct stack_frame
{
        struct stack_frame * base_ptr;
        unsigned int return_addr;
}__attribute__((packed));

#endif
