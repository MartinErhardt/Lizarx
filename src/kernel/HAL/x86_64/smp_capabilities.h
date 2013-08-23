/*   <src-path>/src/kernel/HAL/x86_64/capabilities_smp.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef CAPABILITIES_SMP_H
#define CAPABILITIES_SMP_H

#include<stdint.h>
#include<dbg/console.h>
#include <mm/vmm.h>

#define _MP_ 0x5f504d5f

#define BDA_EBDA_PTR_ADDR	0x40e
#define BDA_BASE_MEM_SIZE	0x413

#define BIOS_ROM_START_ADDR	0xf0000
#define BIOS_ROM_END_ADDR	0xfffff

#define ENTRY_TYPE_PROCESSOR			0
#define ENTRY_TYPE_BUS				1
#define ENTRY_TYPE_IO_APIC			2
#define ENTRY_TYPE_INTERRUPT_ASSIGNMENT		3
#define ENTRY_TYPE_LOCAL_INTERRUPT_ASSIGNMENT	4

struct floating_point
{
	uint32_t signature;
	uint32_t config_table;
	uint8_t length;
	uint8_t spec_rev;
	uint8_t checksum;
	uint8_t feauture;
	uint8_t IMCRP;
	uint32_t reserved :24;
}__attribute__((packed));

struct config_table_header
{
	uint32_t signature;
	uint16_t base_table_length;
	uint8_t spec_rev;
	uint8_t checksum;
	char oem[8];
	char product[12];
	uint32_t oem_table_ptr;
	uint16_t oem_table_siz;
	uint16_t entry_count;
	uint32_t local_apic_ptr;
	uint16_t extended_tbl_length;
	uint8_t  extended_tbl_checksum;
	uint8_t reserved;
}__attribute__((packed));

struct processor_entry
{
	uint8_t entry_type;
	uint8_t local_apic_id;
	uint8_t local_apic_ver;
	uint8_t cpu_flags;
	uint32_t cpu_signature;
	uint32_t feauture_flags;
	uint64_t reserved;
}__attribute__((packed));

struct config_table_header * cf_tbl_header;
struct floating_point * fp;

// FIXME check checksum

static inline struct floating_point * find_floating_pointer_struct()
{
	uintptr_t i;
	uintptr_t EBDA_ptr = (uintptr_t) (*((uint16_t *) BDA_EBDA_PTR_ADDR)*0x10);
	for(i=EBDA_ptr; i<=(EBDA_ptr+0x400);i++)
	{
		if(*((uint32_t *)i)== _MP_ )
			return ((struct floating_point *)i);
	}
	uintptr_t base_mem = (uintptr_t) ((*((uint16_t *) BDA_BASE_MEM_SIZE)-1)*0x400);
	for(i=base_mem; i<=(base_mem+0x400);i++)
	{
		if(*((uint32_t *)i)== _MP_ )
			return ((struct floating_point *)i);
	}
	for(i=BIOS_ROM_START_ADDR; i<=BIOS_ROM_END_ADDR;i++)
	{
		if(*((uint32_t *)i)== _MP_ )
			return ((struct floating_point *)i);
	}
	return NULL;
}
static inline uintptr_t check_mp()
{
	fp = find_floating_pointer_struct();
	if(fp->feauture)
		kprintf("default_config");
	cf_tbl_header= (struct config_table_header *)((uintptr_t)fp->config_table);
	uintptr_t cf_tbl_entry = ((uintptr_t) cf_tbl_header) + sizeof(struct config_table_header);
	uint32_t i;
	uint32_t cores=0;
	for(i=0;i<cf_tbl_header->entry_count;i++)
	{
		switch(*((uint8_t *)cf_tbl_entry))
		{
			case(ENTRY_TYPE_PROCESSOR):
				if((((struct processor_entry*) cf_tbl_entry)->cpu_flags) & 0x01)
				{
					cores++;
					kprintf("local apic 0x%x \n",(((struct processor_entry*) cf_tbl_entry)->local_apic_id));
				}
				cf_tbl_entry+=sizeof(struct processor_entry);
				break;
			case(ENTRY_TYPE_BUS):
				cf_tbl_entry+=8;
				break;
			case(ENTRY_TYPE_IO_APIC):
				cf_tbl_entry+=8;
				break;
			case(ENTRY_TYPE_INTERRUPT_ASSIGNMENT):
				cf_tbl_entry+=8;
				break;
			case(ENTRY_TYPE_LOCAL_INTERRUPT_ASSIGNMENT):
				cf_tbl_entry+=8;
				break;
			default:break;
		}
	}
	kprintf("%dx",cores);
	return (uintptr_t)cf_tbl_header->local_apic_ptr;
}

#endif
