/*   <src-path>/src/kernel/HAL/i386/capabilities_smp.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
	uint64_t oem_id;
	uint64_t product_id_1;
	uint32_t product_id_2;
	uint32_t oem_table_ptr;
	uint16_t oem_table_siz;
	uint16_t entry_count;
	uint32_t local_apic_ptr;
	uint16_t extended_tbl_length;
	uint8_t  extended_tbl_checksum;
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

// FIXME check checksum

static inline struct floating_point * find_floating_pointer_struct()
{
	uint32_t _MP_ = 0x5f504d5f;//0x5f4d505f
	uintptr_t i;
	uintptr_t EBDA_ptr = (uintptr_t) (*((uint16_t *) 0x40e)*0x10);
	for(i=EBDA_ptr; i<0xa0000;i++)
	{
		if(*((uint32_t *)i)==_MP_)
			return ((struct floating_point *)i);
	}
	uintptr_t base_mem = (uintptr_t) ((*((uint16_t *) 0x413)-1)*0x400);
	for(i=base_mem; i<(base_mem+0x400);i++)
	{
		if(*((uint32_t *)i)==_MP_)
			return ((struct floating_point *)i);
	}
	for(i=0xF0000; i<0xFFFFF;i++)
	{
		if(*((uint32_t *)i)==_MP_)
			return ((struct floating_point *)i);
	}
	return NULL;
}
static inline void mp_init()
{
	struct floating_point * fp = find_floating_pointer_struct();
	if(fp->feauture)
		kprintf("default_config");
	struct config_table_header * cf_tbl_header= (struct config_table_header *)((uintptr_t)fp->config_table);
	uintptr_t cf_tbl_entry = ((uintptr_t) cf_tbl_header) + sizeof(struct config_table_header);
	uint32_t i;
	uint32_t cores=0;
	kprintf("Smp Spec: %d \n",fp->spec_rev);
	for(i=0;i<cf_tbl_header->entry_count;i++)
	{
		switch(*((uint8_t *)cf_tbl_entry))
		{
			case(0):
				if((((struct processor_entry*) cf_tbl_entry)->cpu_flags) & 0x01)
					cores++;
				cf_tbl_entry+=sizeof(struct processor_entry);
				break;
			case(1):
				cf_tbl_entry+=8;
				break;
			case(2):
				cf_tbl_entry+=8;
				break;
			case(3):
				cf_tbl_entry+=8;
				break;
			case(4):
				cf_tbl_entry+=8;
				break;
			default:break;
		}
	}
	kprintf("%dx",cores);
}

#endif
