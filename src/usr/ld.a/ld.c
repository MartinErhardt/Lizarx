/*   <src-path>src/usr/ld.a/ld.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include"elf.h"
#include"ld.h"
#include"lib_stat/st_stdint.h"
#include"lib_stat/st_stdlib.h"
#include"lib_stat/st_string.h"
#include"lib_stat/st_stdarg.h"
#include"../archdef.h"
#include"../asm_inline.h"

#include"lib_stat/video.h"

#define SYS_VMM_MALLOC 10 

static st_uintptr_t st_vmm_malloc(st_size_t alloc_size);
static st_uintptr_t st_vmm_find(st_size_t alloc_size);
static st_uint_t st_vmm_realloc(st_uintptr_t ptr, st_size_t alloc_size);
static struct elf_section_header * get_dynamic(struct elf_header* elf);
static void section_to_dynamic(	struct elf_dyn* cur_dyn,
				st_int_t dynamic_num,
				struct elf_header* elf,
				struct dynamic * fill_in);
static void resolve_references(struct dynamic * to_resolve,
			       struct dynamic * all_elfs,int elf_num);
static struct elf_symbol * get_sym_by_name(struct dynamic * dyn_struct,
					   char * sym_name);
static st_uint32_t elf_hash(const unsigned char *name);

static void resolve_references(struct dynamic * to_resolve,struct dynamic * all_elfs,int elf_num)
{
	st_uint_t * cur_reloc_field=ST_NULL;
	struct elf_rel * cur_rel = ST_NULL;
	struct elf_rela * cur_rela = ST_NULL;
	int i,j,strndx,symndx;
	struct elf_symbol * cur_sym=ST_NULL;
	if(to_resolve->jmp_rel != ST_NULL)
	{
		for(i=0;i<to_resolve->jmp_rel_count;i++)
		{
			
			cur_rel=&to_resolve->jmp_rel[i];
			
			if(ELF_REL_TYPE(cur_rel->info)!=ELF_REL_JMP_SLOT)
			{
				continue;
			}
			
			cur_reloc_field = (st_uint_t *)(cur_rel->offset+to_resolve->runtime_addr);
			
			symndx=ELF_REL_BIND(cur_rel->info);
			strndx=to_resolve->sym[symndx].name;
			for(j=0;j<elf_num;j++)
			{
				cur_sym=get_sym_by_name(&all_elfs[j],&to_resolve->str[strndx]);
				
				if((cur_sym!=ST_NULL)&&(cur_sym->section_index))
				{
					*cur_reloc_field=cur_sym->value+all_elfs[j].runtime_addr;
					break;
				}
				
			}
			
		}
	}
	
	if(to_resolve->dyn_rel != ST_NULL)
	{
		for(i=0;i<to_resolve->dyn_rel_count;i++)
		{
			cur_rel=&to_resolve->dyn_rel[i];
			if(ELF_REL_TYPE(cur_rel->info)!=ELF_REL_GLOB_DATA)
			{
				continue;
			}
			cur_reloc_field = (st_uint_t *)(cur_rel->offset+to_resolve->runtime_addr);
			symndx=ELF_REL_BIND(cur_rel->info);
			strndx=to_resolve->sym[symndx].name;
			for(j=0;j<elf_num;j++)
			{
				cur_sym=get_sym_by_name(&all_elfs[j],&to_resolve->str[strndx]);
				
				if((cur_sym!=ST_NULL)&&(cur_sym->section_index))
				{
					
					*cur_reloc_field=cur_sym->value+all_elfs[j].runtime_addr;
					break;
				}
			}
		}
	}
	
	if(to_resolve->jmp_rela != ST_NULL)
	{
		for(i=0;i<to_resolve->dyn_rela_count;i++)
		{
			cur_rela=&to_resolve->jmp_rela[i];
			if(ELF_REL_TYPE(cur_rela->info)!=ELF_REL_JMP_SLOT)
			{
				continue;
			}
			cur_reloc_field = (st_uint_t *)(cur_rela->offset+to_resolve->runtime_addr);
			symndx=ELF_REL_BIND(cur_rela->info);
			strndx=to_resolve->sym[symndx].name;
			
			for(j=0;j<elf_num;j++)
			{
				cur_sym=get_sym_by_name(&all_elfs[j],&to_resolve->str[strndx]);
				
				if((cur_sym!=ST_NULL)&&(cur_sym->section_index))
				{
					*cur_reloc_field=cur_sym->value+all_elfs[j].runtime_addr;
					break;
				}
			}
		}
	}
	
	if(to_resolve->dyn_rela != ST_NULL)
	{
		for(i=0;i<to_resolve->dyn_rela_count;i++)
		{
			cur_rela=&to_resolve->dyn_rela[i];
			
			if(ELF_REL_TYPE(cur_rela->info)!=ELF_REL_GLOB_DATA)
			{
				continue;
			}
			cur_reloc_field = (st_uint_t *)(cur_rela->offset+to_resolve->runtime_addr);
			symndx=ELF_REL_BIND(cur_rela->info);
			strndx=to_resolve->sym[symndx].name;
			
			for(j=0;j<elf_num;j++)
			{
				
				cur_sym=get_sym_by_name(&all_elfs[j],&to_resolve->str[strndx]);
				if((cur_sym!=ST_NULL)&&(cur_sym->section_index))
				{
					*cur_reloc_field=cur_sym->value+all_elfs[j].runtime_addr;
					break;
				}
			}
		}
	}
}
void link_lib_against(struct elf_lib* first_elf,struct elf_lib * second_elf)
{
	struct elf_lib * cur_elf_lib=first_elf;
	int i;
	struct elf_section_header * cur_dynamic_sh=ST_NULL;
	
	struct dynamic dynamic_libs[2];
	cur_elf_lib=first_elf;
	//vprintf("cur_elf_lib:");
	//vprintf(st_itoa((unsigned  long)cur_elf_lib->header,0x10));
	for(i=0;i<2;i++)
	{
		
		cur_dynamic_sh=get_dynamic(cur_elf_lib->header);
		dynamic_libs[i].runtime_addr=cur_elf_lib->runtime_addr;
		section_to_dynamic(
				(struct elf_dyn*)((st_uintptr_t)(cur_elf_lib->header)+(cur_dynamic_sh->off)),
				cur_dynamic_sh->size/sizeof(struct elf_dyn),
				cur_elf_lib->header,
				&dynamic_libs[i]
				);
		
		cur_elf_lib=second_elf;
	}
	
	resolve_references(&dynamic_libs[0],&dynamic_libs[0],2);
}
static struct elf_section_header * get_dynamic(struct elf_header* elf)
{
	struct elf_section_header* section_header = (struct elf_section_header*)(((st_uintptr_t)elf)+(elf)->sh_offset);
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
static void section_to_dynamic(struct elf_dyn* cur_dyn,
					   st_int_t dynamic_num,
					   struct elf_header* elf,
					   struct dynamic * fill_in)
{
	unsigned int i = 0;
	fill_in->header=elf;
	for(i=0;i<dynamic_num;i++)
	{
		if(cur_dyn->type == ELF_DYNAMIC_TYPE_SYM_TAB)
		{
			fill_in->sym = (struct elf_symbol * )(cur_dyn->un.ptr+fill_in->runtime_addr);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_STR_TAB)
		{
			fill_in->str = (char*)(cur_dyn->un.ptr+fill_in->runtime_addr);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_HASH)
		{
			fill_in->hash = (st_int32_t * )(cur_dyn->un.ptr+fill_in->runtime_addr);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_JMPREL)
		{
#ifdef ARCH_X86
			fill_in->jmp_rel = (struct	elf_rel * )(cur_dyn->un.ptr+fill_in->runtime_addr);
#endif
#ifdef ARCH_X86_64
			fill_in->jmp_rela = (struct	elf_rela * )(cur_dyn->un.ptr+fill_in->runtime_addr);
#endif
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_REL)
		{
			fill_in->dyn_rel = (struct	elf_rel * )(cur_dyn->un.ptr+fill_in->runtime_addr);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_RELA)
		{
			fill_in->dyn_rela = (struct	elf_rela * )(cur_dyn->un.ptr+fill_in->runtime_addr);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_RELSZ)
		{
			fill_in->dyn_rel_count = cur_dyn->un.val/sizeof(struct elf_rel);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_RELASZ)
		{
			fill_in->dyn_rela_count = cur_dyn->un.val/sizeof(struct elf_rela);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_PLTRELSZ)
		{
#ifdef ARCH_X86
			fill_in->jmp_rel_count = cur_dyn->un.val/sizeof(struct elf_rel);
#endif
#ifdef ARCH_X86_64
			fill_in->jmp_rela_count = cur_dyn->un.val/sizeof(struct elf_rela);
#endif
		}
		cur_dyn=(struct elf_dyn*)((st_uintptr_t)(cur_dyn)+sizeof(struct elf_dyn));
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
	st_uintptr_t file_dest = 0x0;
	/* Ist es ueberhaupt eine ELF-Datei? */
	
	if (header->i_magic != ELF_MAGIC) 
	{
		vprintf("[LD.A] E: init_elf couldn't find valid ELF-Magic!\n");
		vprintf(st_itoa(((st_uintptr_t)header),16));
		return ST_NULL;
	}
#ifdef ARCH_X86
	if (header->i_class != ELF_CLASS_32)
	{
		vprintf("[LD.A] E: init_elf found elf with class != 32!\n");
		return ST_NULL;
	}
	if (header->machine != ELF_MACHINE_386)
	{
		vprintf("[ELF_LOADER] E: init_elf found elf with invalid target!\n");
	}
#endif
#ifdef ARCH_X86_64
	if (header->i_class != ELF_CLASS_64)
	{
		vprintf("[LD.A] E: init_elf found elf with class != 32!\n");
		return ST_NULL;
	}
#endif
	if ((header->i_data != ELF_DATA_LITTLEENDIAN)||(header->version != ELF_DATA_LITTLEENDIAN))
	{
		vprintf("[LD.A] E: init_elf found elf with data != ELF_DATA_LITTLEENDIAN!\n");
		return ST_NULL;
	}
	if (header->type != ELF_TYPE_DYN)
	{
		vprintf("[LD.A] E: init_elf found elf with type != ELF_TYPE_EXEC\n");
	}
	if (header->i_version != ELF_VERSION_CURRENT)
	{
	    vprintf("[LD.A] E: init_elf found version != ELF_VERSION_CURRENT!\n");
	    return ST_NULL;
	}
	/*
	* Alle Program Header durchgehen und den Speicher an die passende Stelle
	* kopieren.
	*
	* FIXME Wir erlauben der ELF-Datei hier, jeden beliebigen Speicher zu
	* ueberschreiben, einschliesslich dem Kernel selbst.
	*/
	
	ph = (struct elf_program_header*) (((st_uintptr_t) image) + header->ph_offset);
	
	for (i = 0; i < header->ph_entry_count; i++, ph++)
	{
		if(size<ph->virt_addr)
			size = ph->virt_addr;
	}
	
	file_dest = st_vmm_malloc(size);
	ph = (struct elf_program_header*) (((st_uintptr_t) image) + header->ph_offset);
	
	for (i = 0; i < header->ph_entry_count; i++, ph++)
	{
		if ((ph->type != ELF_PROGRAM_TYPE_LOAD)&&(ph->type != ELF_PROGRAM_TYPE_DYNAMIC))
		{
			
			continue;
		}
		
		//st_vmm_realloc(file_dest+ph->virt_addr,ph->file_size);
		dest = (void*)(file_dest+ph->virt_addr);
		
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
	SYSCALL(SYS_VMM_MALLOC);
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
static st_uint32_t elf_hash(const unsigned char *name)
{
	st_uint32_t h = 0, g;
	while (*name) 
	{
		h = (h << 4) + *name++;
		
		if ((g = h & 0xf0000000) != 0) 
		{
			h ^= g >> 24;
		}
		h &= ~g;
	}
	return h;
}
static struct elf_symbol * get_sym_by_name(struct dynamic * dyn_struct, char * sym_name) 
{
	struct elf_symbol * sym;
	st_int32_t * bucket;
	st_int32_t * chain;
	int		nbucket;
	//int		nchain;
	int		x;
	int		y;
	// Calcuate the hash value 
	x = elf_hash(sym_name);
	// Find the hashes and chains 
	nbucket = dyn_struct->hash[0];
	//nchain  = dyn_struct->hash[1];
	bucket  = &dyn_struct->hash[2];
	chain   = &dyn_struct->hash[2 + nbucket];
	y = bucket[x % nbucket];
	
	do
	{
		sym = &dyn_struct->sym[y];
		if (!st_strcmp(sym_name, dyn_struct->str + sym->name))
		{
			return sym;
		}
		
		y = chain[y];
		
	} while (y);
	
	return ST_NULL;
}
