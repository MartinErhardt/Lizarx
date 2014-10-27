/*  <src-path>/src/kernel/inc/HAL/x86/hw_structs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
 *  Copyright (C) 2013, 2014  martin.erhardt98@googlemail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

struct vmm_paging_entry
{
	uint8_t rw_flags;
	uint8_t reserved :4;
	uintptr_t next_paging_layer :20;
}__attribute__((packed));
#define PAGING_TABLE_ENTRY_N 1024

// The following two structs are DEPRECATED
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
struct tss_t
{
	uint16_t link;
	uint16_t reserved0;
	uint32_t esp0;
	uint16_t ss0;
	// Everything below is useless!
	uint16_t reserved1;
	uint32_t esp1;
	uint16_t ss1;
	uint16_t reserved2;
	uint32_t esp2;
	uint16_t ss2;
	uint16_t reserved3;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint16_t es;
	uint16_t reserved4;
	uint16_t cs;
	uint16_t reserved5;
	uint16_t ss;
	uint16_t reserved6;
	uint16_t ds;
	uint16_t reserved7;
	uint16_t fs;
	uint16_t reserved8;
	uint16_t gs;
	uint16_t reserved9;
	uint16_t LDT_desc;
	uint16_t reserved10;
	uint16_t trap_bitnreserved;
	uint16_t IO_Map_addr;
}__attribute__((packed));
#endif
