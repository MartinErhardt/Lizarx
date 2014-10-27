/*  <src-path>/src/kernel/inc/HAL/x86_common/smp_capabilities.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef CAPABILITIES_SMP_H
#define CAPABILITIES_SMP_H

#include<stdint.h>
#include<dbg/console.h>

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

uint_t cores_from_tables;

struct config_table_header * cf_tbl_header;
struct floating_point * fp;

struct floating_point * find_floating_pointer_struct();
uintptr_t check_mp();

uint_t tsl(uint_t lock,uint_t value);

#endif
