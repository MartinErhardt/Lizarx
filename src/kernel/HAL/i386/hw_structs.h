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

#include<stdint.h>
#include<mm/pmm.h>

//------------------------------------------------------------------GDT-structs--------------------------------------------------------------------
struct gdt_entry
{
	uint16_t 	limit;
	uint32_t 	base :24;
	uint8_t 	accessbyte;
	uint32_t 	limit2 :4;
	uint32_t 	flags :4;
	uint8_t 	base2;
}__attribute__((packed));

//------------------------------------------------------------------IDT-structs--------------------------------------------------------------------
struct idt_entry
{
    uint16_t isr_offset;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t isr_offset2;
}__attribute__((packed));
//-----------------------------------------------------------------Paging-structures---------------------------------------------------------------
struct vmm_pagedir
{
    uint8_t rw_flags; 
    uint8_t reserved :4;
    uintptr_t pagetbl_ptr :20;
}__attribute__((packed));

struct vmm_pagetbl
{
    uint8_t rw_flags; 
    uint8_t reserved :4;
    uintptr_t page_ptr :20;
}__attribute__((packed));

struct stack_frame
{
        struct stack_frame * base_ptr;
        uintptr_t return_addr;
}__attribute__((packed));

#endif
