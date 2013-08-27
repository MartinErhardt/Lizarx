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
/*
 *
 * The following code should be compatible to the ELF standard v1.1 <http://www.acm.uiuc.edu/sigops/rsrc/pfmt11.pdf>
 * if you see any incompatibilities contact me!
 */
#ifndef ELF_H
#define ELF_H
 
#include <stdint.h>
#include <mm/vmm.h>

#define ELF_MAGIC 0x464C457F

//---------------------------------elf-header-----------------------------------

#define ELF_MACHINE_NONE			0
#define ELF_MACHINE_M32			1
#define ELF_MACHINE_SPARC			2
#define ELF_MACHINE_386			3
#define ELF_MACHINE_68K			4
#define ELF_MACHINE_88K			5
#define ELF_MACHINE_860			7
#define ELF_MACHINE_MIPS			8

#define ELF_CLASS_NONE				0
#define ELF_CLASS_32				1
#define ELF_CLASS_64				2

#define ELF_DATA_NONE				0
#define ELF_DATA_LITTLEENDIAN			1
#define ELF_CLASS_BIGENDIAN			2

#define ELF_VERSION_NONE			0
#define ELF_VERSION_CURRENT			1

#define ELF_TYPE_NONE				0
#define ELF_TYPE_REL				1
#define ELF_TYPE_EXEC				2
#define ELF_TYPE_DYN				3// Shared Object
#define ELF_TYPE_CORE				4
#define ELF_TYPE_LOPROC			0xff00
#define ELF_TYPE_HIPROC			0xffff

//---------------------------------program-header-----------------------------------

#define ELF_PROGRAM_TYPE_NULL			0
#define ELF_PROGRAM_TYPE_LOAD			1
#define ELF_PROGRAM_TYPE_DYNAMIC		2
#define ELF_PROGRAM_TYPE_INTERP		3
#define ELF_PROGRAM_TYPE_NOTE			4
#define ELF_PROGRAM_TYPE_SHLIB			5

//---------------------------------section-header-----------------------------------

#define ELF_SECTION_TYPE_NULL			0
#define ELF_SECTION_TYPE_PROGBITS		1
#define ELF_SECTION_TYPE_SYMTAB		2
#define ELF_SECTION_TYPE_STRTAB		3
#define ELF_SECTION_TYPE_RELA			4
#define ELF_SECTION_TYPE_HASH			5
#define ELF_SECTION_TYPE_DYNAMIC		6
#define ELF_SECTION_TYPE_NOTE			7
#define ELF_SECTION_TYPE_NOBITS		8
#define ELF_SECTION_TYPE_REL			9
#define ELF_SECTION_TYPE_SHLIB			10
#define ELF_SECTION_TYPE_DYNSYM 		11
#define ELF_SECTION_TYPE_LOPROC		0x70000000
#define ELF_SECTION_TYPE_HIPROC 		0x7fffffff
#define ELF_SECTION_TYPE_LOUSER 		0x80000000
#define ELF_SECTION_TYPE_HIUSER 		0xffffffff

//---------------------------------symbols-----------------------------------

#define ELF_SYM_BIND(i)			((i)>>4)
#define ELF_SYM_TYPE(i)			((i)&0xf)
#define ELF_SYM_INFO(b,t)			(((b)<<4)+((t)&0xf))

#define ELF_SYM_BIND_LOCAL			0
#define ELF_SYM_BIND_GLOBAL			1
#define ELF_SYM_BIND_WEAK			2
#define ELF_SYM_BIND_LOPROC			13
#define ELF_SYM_BIND_HIPROC			15

#define ELF_SYM_TYPE_NONE			0
#define ELF_SYM_TYPE_OBJ			1
#define ELF_SYM_TYPE_FUNC			2
#define ELF_SYM_TYPE_SECTION			3
#define ELF_SYM_TYPE_FILE			4
#define ELF_SYM_TYPE_LOPROC			13
#define ELF_SYM_TYPE_HIPROC			15

#define ELF_SYM_SEC_ABS			0xfff1
#define ELF_SYM_SEC_COMMON			0xfff2
#define ELF_SYM_SEC_UNDEF			0

//---------------------------------relocation-----------------------------------

#define ELF_REL_BIND(i)			((i)>>8)
#define ELF_REL_TYPE(i)			((unsigned char) (i))
#define ELF_REL_INFO(b,t)			(((b)<<8)+((unsigned char)(t)))
/*
 * The type describes what value is finally computed
 * A= addon
 * B= base address of shared Library
 * G=?
 * GOT=?
 * P= storage unit being relocated
 * S= value of symbol
 */
#define ELF_REL_NONE				0 // none
#define ELF_REL_32				1 // S+A
#define ELF_REL_PC32				2 // S+A-P
#define ELF_REL_GOT32				3 // G+A-P
#define ELF_REL_PLT32				4 // L+A-P
#define ELF_REL_COPY				5 // none
#define ELF_REL_GLOB_DATA			6 // S
#define ELF_REL_JMP_SLOT			7 // S
#define ELF_REL_RELATIVE			8 // B+A
#define ELF_REL_GOTOFF				9 // S + A- GOT
#define ELF_REL_GOTPC				10// GOT+A- P
//-------------------------------------dynamic---------------------------------------
#define ELF_DYNAMIC_TYPE_NULL			0
#define ELF_DYNAMIC_TYPE_NEEDED		1
#define ELF_DYNAMIC_TYPE_PLTRELSZ		2
#define ELF_DYNAMIC_TYPE_PLTGOT		3
#define ELF_DYNAMIC_TYPE_HASH			4
#define ELF_DYNAMIC_TYPE_STR_TAB		5
#define ELF_DYNAMIC_TYPE_SYM_TAB		6
#define ELF_DYNAMIC_TYPE_RELA			7
#define ELF_DYNAMIC_TYPE_RELASZ		8
#define ELF_DYNAMIC_TYPE_RELAENT		9
#define ELF_DYNAMIC_TYPE_STRSZ			10
#define ELF_DYNAMIC_TYPE_SYMENT		11
#define ELF_DYNAMIC_TYPE_INIT			12
#define ELF_DYNAMIC_TYPE_FINI			13
#define ELF_DYNAMIC_TYPE_SONAME		14
#define ELF_DYNAMIC_TYPE_RPATH			15
#define ELF_DYNAMIC_TYPE_SYMBOLIC		16
#define ELF_DYNAMIC_TYPE_REL			17
#define ELF_DYNAMIC_TYPE_RELSZ			18
#define ELF_DYNAMIC_TYPE_RELENT		19
#define ELF_DYNAMIC_TYPE_PLTREL		20
#define ELF_DYNAMIC_TYPE_DEBUG			21
#define ELF_DYNAMIC_TYPE_TEXTREL		22
#define ELF_DYNAMIC_TYPE_JMPREL		23
#define ELF_DYNAMIC_TYPE_LOPROC		0x70000000
#define ELF_DYNAMIC_TYPE_HIPROC		0x7fffffff

/*
 * The following structures are used to read out the ELF format
 */
#ifdef ARCH_X86
struct elf_header
{
    uint32_t	i_magic;
    uint8_t	i_class;
    uint8_t	i_data;
    uint8_t	i_version;
    uint8_t	i_pad;
    uint64_t	i_reserved;
    
    uint16_t	type;
    uint16_t	machine;
    uint32_t	version;
    uint32_t	entry;
    uint32_t	ph_offset;
    uint32_t	sh_offset;
    uint32_t	flags;
    uint16_t	header_size;
    uint16_t	ph_entry_size;
    uint16_t	ph_entry_count;
    uint16_t	sh_entry_size;
    uint16_t	sh_entry_count;
    uint16_t	sh_str_table_index;
} __attribute__((packed));

struct elf_program_header
{
    uint32_t	type;
    uintptr_t	offset;
    uintptr_t	virt_addr;
    uintptr_t	phys_addr;
    size_t	file_size;
    size_t	mem_size;
    uint32_t	flags;
    size_t	alignment;
} __attribute__((packed));
#else
struct elf_header
{
    uint32_t	i_magic;
    uint8_t	i_class;
    uint8_t	i_data;
    uint8_t	i_version;
    uint8_t	i_pad;
    uint64_t	i_reserved;
    
    uint16_t	type;
    uint16_t	machine;
    uint32_t	version;
    uint64_t	entry;
    uint64_t	ph_offset;
    uint64_t	sh_offset;
    uint32_t	flags;
    uint16_t	header_size;
    uint16_t	ph_entry_size;
    uint16_t	ph_entry_count;
    uint16_t	sh_entry_size;
    uint16_t	sh_entry_count;
    uint16_t	sh_str_table_index;
} __attribute__((packed));


struct elf_program_header
{
    uint32_t	type;
    uint32_t	flags;
    uint64_t	offset;
    uint64_t	virt_addr;
    uint64_t	phys_addr;
    size_t	file_size;
    size_t	mem_size;
    size_t	alignment;
} __attribute__((packed));
#endif

struct elf_section_header 
{
    uint32_t	name;
    uint32_t	type;
    uint32_t	flags;
    uintptr_t	virt_addr;
    uintptr_t	off;
    size_t	size;
    uint32_t	link;
    uint32_t	info;
    size_t	align;
    uint32_t	entry_size;
} __attribute__((packed));

struct elf_symbol
{
    uint32_t	name;
    uintptr_t	value;
    size_t	size;
    uint8_t	info;
    uint8_t	other;
    uint16_t	section_index;
} __attribute__((packed));

struct elf_rel
{
    uintptr_t	offset;
    uint_t	info;
} __attribute__((packed));

struct elf_rela
{
    uintptr_t	offset;
    uint32_t	info;
    int32_t	addend;
} __attribute__((packed));

struct elf_dyn
{
    int_t type;
    union {
	uint_t val;
	uintptr_t ptr;
    } un;
}__attribute__((packed));
/*
 * OS-specific format
 */
struct dynamic
{
	struct elf_symbol * sym;
	
	struct	elf_rel * jmp_rel;
	int_t	jmp_rel_count;
	
	int_t * hash;
	char * str;
};
int32_t init_elf(void* image);
uintptr_t * get_last_function(void * elf_header, uintptr_t addr);

#endif
