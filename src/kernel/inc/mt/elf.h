/*   <src-path>src/kernel/inc/mt/elf.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef ELF_H
#define ELF_H
 
#include <stdint.h>
 
#define ELF_MAGIC 0x464C457F
 
struct elf_header {
    uint32_t    magic;
    uint32_t    version;
    uint64_t    reserved;
    uint64_t    version2;
    uint32_t    entry;
    uint32_t    ph_offset;
    uint32_t    sh_offset;
    uint32_t    flags;
    uint16_t    header_size;
    uint16_t    ph_entry_size;
    uint16_t    ph_entry_count;
    uint16_t    sh_entry_size;
    uint16_t    sh_entry_count;
    uint16_t    sh_str_table_index;
} __attribute__((packed));
 
struct elf_program_header {
    uint32_t    type;
    uint32_t    offset;
    uint32_t    virt_addr;
    uint32_t    phys_addr;
    uint32_t    file_size;
    uint32_t    mem_size;
    uint32_t    flags;
    uint32_t    alignment;
} __attribute__((packed));

int32_t init_elf(void* image,int snd);
#endif