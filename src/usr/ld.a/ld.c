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
#include"elf.h"
#include"ld.h"
#include"lib_stat/st_stdint.h"
#include"lib_stat/st_stdlib.h"
#include"lib_stat/st_string.h"
#include"lib_stat/video.h"
#define SYS_VMM_MALLOC 10
static st_uintptr_t st_vmm_malloc(st_size_t);
static struct elf_section_header * get_dynamic(struct elf_header* elf);
static void section_to_dynamic(struct elf_dyn* cur_dyn,
					   st_int_t section_dynamic_num,
					   struct elf_header* elf,st_uintptr_t B,
					   struct dynamic * fill_in);

struct dynamic dynamic_main=
{
	.sym=ST_NULL,
	.jmp_rel=ST_NULL,
	.jmp_rel_count=0,
	.hash=ST_NULL,
	.str=ST_NULL
};
struct dynamic dynamic_lib=
{
	.sym=ST_NULL,
	.jmp_rel=ST_NULL,
	.jmp_rel_count=0,
	.hash=ST_NULL,
	.str=ST_NULL
};
void link_lib_against(struct elf_header* elf_main,struct elf_header* elf_lib)
{
	
}
void link_against(struct elf_header* elf_main,struct elf_header* elf_lib, st_uintptr_t B)
{
	
	unsigned int i =0;
	st_uint_t * cur_reloc_field=ST_NULL;
	struct elf_rel * cur_rel = ST_NULL;
	
	struct elf_section_header * dynamic_main_sh=get_dynamic(elf_main);
	struct elf_section_header * dynamic_lib_sh=get_dynamic(elf_lib);
	
	section_to_dynamic
	(
		(struct elf_dyn*)((st_uintptr_t)(elf_main)+
		(dynamic_main_sh->off)),
		
		dynamic_main_sh->size/sizeof(struct elf_dyn),elf_main, 0,
		&dynamic_main
	);
	section_to_dynamic( 
		(struct elf_dyn*)((st_uintptr_t)(elf_lib)+(dynamic_lib_sh->off)), 
		dynamic_lib_sh->size/sizeof(struct elf_dyn),elf_lib, (st_uintptr_t)elf_lib,
		&dynamic_lib
	);
	for(i=0;i<dynamic_main.jmp_rel_count;i++)
	{
		
		cur_rel=&dynamic_main.jmp_rel[i];
		cur_reloc_field = (st_uint_t *)cur_rel->offset;
		
		*cur_reloc_field=dynamic_lib.sym[ELF_REL_BIND(cur_rel->info)].value+B;
	}
	for(i=0;i<dynamic_lib.jmp_rel_count;i++)
	{
		cur_rel=&dynamic_lib.jmp_rel[i];
		
		cur_reloc_field = (st_uint_t *)(cur_rel->offset+B);
		*cur_reloc_field=dynamic_main.sym[ELF_REL_BIND(cur_rel->info)].value;
	}
}
static struct elf_section_header * get_dynamic(struct elf_header* elf)
{
	struct elf_section_header* section_header = (struct elf_section_header*)((st_uintptr_t)elf+(elf)->sh_offset);
	
	unsigned int i = 0;
	unsigned int section_num = (elf)->sh_entry_count;
	// get 
	for(i=0;i<section_num;i++)
	{
		if(section_header->type==ELF_SECTION_TYPE_DYNAMIC)
		{
			return section_header;
		}
		section_header=(struct elf_section_header*)((st_uintptr_t)(section_header)+sizeof(struct elf_section_header));
	}
	return ST_NULL;
}
void section_to_dynamic(struct elf_dyn* cur_dyn,
					   st_int_t section_dynamic_num,
					   struct elf_header* elf,st_uintptr_t B,
					   struct dynamic * fill_in)
{
	unsigned int i = 0;
	
	for(i=0;i<section_dynamic_num;i++)
	{
		if(cur_dyn->type == ELF_DYNAMIC_TYPE_SYM_TAB)
		{
			fill_in->sym = (struct elf_symbol * )(cur_dyn->un.ptr+B);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_STR_TAB)
		{
			fill_in->str = (char*)(cur_dyn->un.ptr+(st_uintptr_t)(elf)+B);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_HASH)
		{
			fill_in->hash = (st_int_t * )cur_dyn->un.ptr+B;
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_JMPREL)
		{
			fill_in->jmp_rel = (struct	elf_rel * )(cur_dyn->un.ptr+B);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_PLTRELSZ)
		{
			fill_in->jmp_rel_count = cur_dyn->un.val/sizeof(struct elf_rel);
		}
		cur_dyn=(struct elf_dyn*)((st_uintptr_t)(cur_dyn)+sizeof(struct elf_dyn));
	}
	// Sanity checks
	if (	(fill_in->str == ST_NULL) || 
		(fill_in->hash == ST_NULL) || 
		(fill_in->sym == ST_NULL) || 
		(fill_in->jmp_rel==ST_NULL))
	{
	}
}
void * init_shared_lib(void* image, st_size_t size)
{
	/*
	* FIXME Wir muessen eigentlich die Laenge vom Image pruefen, damit wir bei
	* korrupten ELF-Dateien nicht ueber das Dateiende hinauslesen.
	*/
	struct elf_header* header = image;
	struct elf_program_header* ph;
	int i;
	void * dest=ST_NULL;
	st_uintptr_t file_dest=0x0;
	/* Ist es ueberhaupt eine ELF-Datei? */
	//kprintf("0x%x",(uintptr_t)image);
	
	if (header->i_magic != ELF_MAGIC) 
	{
		vprintf("[ELF_LOADER] E: init_elf couldn't find valid ELF-Magic!\n");
		return ST_NULL;
	}
	if (header->i_class != ELF_CLASS_32)
	{
		vprintf("[ELF_LOADER] E: init_elf found elf with class != 32!\n");
		return ST_NULL;
	}
	if ((header->i_data != ELF_DATA_LITTLEENDIAN)||(header->version != ELF_DATA_LITTLEENDIAN))
	{
		vprintf("[ELF_LOADER] E: init_elf found elf with data != ELF_DATA_LITTLEENDIAN!\n");
		return ST_NULL;
	}
	if (header->type != ELF_TYPE_DYN)
	{
		vprintf("[ELF_LOADER] E: init_elf found elf with type != ELF_TYPE_EXEC\n");
	}
	if (header->machine != ELF_MACHINE_386)
	{
		vprintf("[ELF_LOADER] E: init_elf found elf with invalid target!\n");
	}
	if (header->i_version != ELF_VERSION_CURRENT)
	{
	    vprintf("[ELF_LOADER] E: init_elf found version != ELF_VERSION_CURRENT!\n");
	    return ST_NULL;
	}
	/*
	* Alle Program Header durchgehen und den Speicher an die passende Stelle
	* kopieren.
	*
	* FIXME Wir erlauben der ELF-Datei hier, jeden beliebigen Speicher zu
	* ueberschreiben, einschliesslich dem Kernel selbst.
	*/
	file_dest = st_vmm_malloc(size);
	ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
	for (i = 0; i < header->ph_entry_count; i++, ph++)
	{
		if (ph->type != ELF_PROGRAM_TYPE_LOAD)
		{
			continue;
		}
		dest = (void*)(file_dest+ph->offset);
		void* src = ((char*) image) + ph->offset;
		
		st_memset(dest, 0x00000000, ph->mem_size);
		st_memcpy(dest, src, ph->file_size);
	}
	//print_symbols(image,(struct elf_section_header*)((uintptr_t)(image)+header->sh_offset),header->sh_entry_count);
	return (void*)file_dest;
}
st_uintptr_t st_vmm_malloc(st_size_t alloc_size)
{
	st_uintptr_t free_space;
	asm volatile( "nop" :: "d" (alloc_size));
	asm volatile ("int $0x30"::"a" (SYS_VMM_MALLOC) );
	asm volatile("nop" : "=d" (free_space) );
	return free_space;
}
/*
static size_t elf_get_size(struct elf_header * elf)
{
	size_t size =0x0;
	struct elf_program_header * cur_program_header=(struct elf_program_header *)((uintptr_t)elf+elf->ph_offset);
	int_t i=0;
	for(i=0;i<elf->ph_entry_count;i++)
	{
		if(cur_program_header->offset+cur_program_header->mem_size>size)
		{
			size=cur_program_header->offset+cur_program_header->mem_size;
		}
		cur_program_header=(struct elf_program_header *)((uintptr_t)cur_program_header + sizeof(struct elf_program_header));
	}
	return size;
}*/
