/*  <src-path>/src/kernel/inc/HAL/x86_64/hw_structs.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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

#include<mm/pmm.h>
//uintptr_t trampoline;
uintptr_t easy_map_tbl;
uintptr_t bsp_stack;
struct tss_t
{
    uint32_t reserved;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t iomap;
} __attribute__((packed));
//------------------------------------------------------------------GDT-structs--------------------------------------------------------------------
struct gdt_entry
{
	uint16_t limit;
	uint32_t base :24;
	uint8_t accessbyte;
	uint32_t limit2 :4;
	uint32_t flags :4;
	uint8_t base2;
}__attribute__((packed));

//------------------------------------------------------------------IDT-structs--------------------------------------------------------------------
struct idt_entry
{
	uint16_t isr_offset;
	uint16_t selector;
	uint8_t ist;
	uint8_t flags;
	uint64_t isr_offset2 :48;
	uint32_t reserved;
}__attribute__((packed));

//-----------------------------------------------------------------Paging-structures---------------------------------------------------------------
struct vmm_paging_entry
{
	uint8_t rw_flags; 
	uint8_t reserved :4;
	uint64_t next_paging_layer :40;
	uint16_t NXnfree :12;
}__attribute__((packed));
#define PAGING_TABLE_ENTRY_N 512

// The following structs are DEPRECATED
struct vmm_pagemap_level4
{
	uint8_t rw_flags; 
	uint8_t reserved :4;
	uint64_t pagedirptrtbl_ptr :40;
	uint16_t NXnfree :12;
}__attribute__((packed));
struct vmm_pagedir_ptrtbl
{
	uint8_t rw_flags; 
	uint8_t reserved :4;
	uint64_t pagedir_ptr :40;
	uint16_t NXnfree :12;
}__attribute__((packed));

struct vmm_pagedir
{
	uint8_t rw_flags; 
	uint8_t reserved :4;
	uint64_t pagetbl_ptr :40;
	uint16_t NXnfree :12;
}__attribute__((packed));

struct vmm_pagetbl
{
	uint8_t rw_flags; 
	uint8_t reserved :4;
	uint64_t page_ptr :40;	
	uint16_t NXnfree :12;
}__attribute__((packed));

struct stack_frame
{
	struct stack_frame * base_ptr;
	uint64_t return_addr;
}__attribute__((packed));
struct gdt_tss_entry
{
	uint16_t limit;
	uint32_t base :24;
	uint8_t accessbyte;
	uint8_t limit2 :4;
	uint8_t flags :4;
	uint64_t base2 :40;
	uint8_t reserved;
	uint8_t zero :4;
	uint32_t reserved2 :20;
}__attribute__((packed));
#endif
