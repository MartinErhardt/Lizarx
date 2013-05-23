/*   <src-path>/src/kernel/mt/elf_loader.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<mt/elf.h>
#include<dbg/console.h>
#include<string.h>
#include<mm/vmm.h>
#include<hal.h>
#include<mt/proc.h>
#include<mt/threads.h>
#include<stdlib.h>

static struct elf_section_header * get_dynamic(struct elf_header* elf);
static struct dynamic * section_to_dynamic(struct elf_dyn* cur_dyn,int_t section_dynamic_num,struct elf_header* elf,uintptr_t B);
//static uint32_t elf_hash(const unsigned char *name);
//static struct elf_symbol * get_sym_by_name(struct dynamic * dyn_struct, char * sym_name);
static size_t elf_get_size(struct elf_header*elf);

uintptr_t * get_last_function(void* elf_header, uintptr_t addr)
{
	static uintptr_t ret_buf[2];
	struct elf_section_header* section_header = (struct elf_section_header*)((uintptr_t)elf_header+((struct elf_header*)elf_header)->sh_offset);
	uintptr_t str_section=0x0;
	struct elf_symbol* cur_sym=NULL;
	struct elf_symbol* closest_sym=NULL;
	unsigned int sym_num=0; unsigned int i = 0;
	unsigned int section_num = ((struct elf_header*)elf_header)->sh_entry_count;
	for(i=0;i<section_num;i++)
	{
		if(section_header->type==ELF_SECTION_TYPE_SYMTAB)
		{
			cur_sym=(struct elf_symbol*)((uintptr_t)(elf_header)+(section_header->off));
			sym_num=section_header->size/sizeof(struct elf_symbol);
		}
		if(section_header->type==ELF_SECTION_TYPE_STRTAB)
		{
			str_section=(uintptr_t)(elf_header)+(section_header->off);
		}
		section_header=(struct elf_section_header*)((uintptr_t)(section_header)+sizeof(struct elf_section_header));
	}
	for(i=0;i<sym_num;i++)
	{
		cur_sym=(struct elf_symbol*)((uintptr_t)(cur_sym)+sizeof(struct elf_symbol));
		if(cur_sym==NULL) 
		{
			continue;
		}
		if((cur_sym->value)
		  &&(ELF_SYM_TYPE(cur_sym->info)==ELF_SYM_TYPE_FUNC))
		{
			if((addr - closest_sym->value  > addr - cur_sym->value )&&(addr>=cur_sym->value))
			{
				closest_sym = cur_sym;
			}
		}
	}
	ret_buf[0] = (uintptr_t)closest_sym;
	ret_buf[1] = str_section+closest_sym->name;
	return ret_buf;
}
int32_t init_elf(void* image)
{
	/*
	* FIXME Wir muessen eigentlich die Laenge vom Image pruefen, damit wir bei
	* korrupten ELF-Dateien nicht ueber das Dateiende hinauslesen.
	*/
	
	struct elf_header* header = image;
	struct elf_program_header* ph;
	int i;
	struct proc* new_proc=NULL; 
	vmm_context* curcontext=NULL;
	if(!intr_activated)
	{
		curcontext= &startup_context;
	}
	else
	{
		curcontext= current_thread->proc->context;
	}
	uintptr_t curpd_phys= virt_to_phys(curcontext, (uintptr_t)curcontext->pd);
	/* Ist es ueberhaupt eine ELF-Datei? */
	//kprintf("0x%x",(uintptr_t)image);
	if (header->i_magic != ELF_MAGIC) 
	{
		kprintf("[ELF_LOADER] E: init_elf couldn't find valid ELF-Magic!\n");
		return -1;
	}
#ifdef ARCH_X86
	if (header->i_class != ELF_CLASS_32)
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with class != 32!\n");
		return -1;
	}
	if ((header->i_data != ELF_DATA_LITTLEENDIAN)||(header->version != ELF_DATA_LITTLEENDIAN))
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with data != ELF_DATA_LITTLEENDIAN!\n");
		return -1;
	}
	if (header->type != ELF_TYPE_EXEC)
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with type != ELF_TYPE_EXEC\n");
	}
	if (header->machine != ELF_MACHINE_386)
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with invalid target!\n");
	}
#else
    #error lizarx build: No valid arch found in src/kernel/mt/threads.c
#endif
	if (header->i_version != ELF_VERSION_CURRENT)
	{
	    kprintf("[ELF_LOADER] E: init_elf found version != ELF_VERSION_CURRENT!\n");
	    return -1;
	}
	new_proc=create_proc();
	create_thread((void*) header->entry,new_proc->p_id);
	
	/*
	* Alle Program Header durchgehen und den Speicher an die passende Stelle
	* kopieren.
	*
	* FIXME Wir erlauben der ELF-Datei hier, jeden beliebigen Speicher zu
	* ueberschreiben, einschliesslich dem Kernel selbst.
	*/
	ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
	for (i = 0; i < header->ph_entry_count; i++, ph++) 
	{
		void* dest = (void*) ph->virt_addr;
		void* src = ((char*) image) + ph->offset;
		/* Nur Program Header vom Typ LOAD laden */
		if (ph->type != ELF_PROGRAM_TYPE_LOAD)
		{
			continue;
		}
		if(ph->virt_addr<KERNEL_SPACE)
		{
		    kprintf("[ELF_LOADER] E: init_elf an elf want to be loaded at %x ; That's in Kernelspace!\n",ph->virt_addr);
		}
		
		if(vmm_realloc(new_proc->context,(void*)ph->virt_addr,ph->mem_size,FLGCOMBAT_USER)<0)
		{
			kprintf("[ELF_LOADER] W: init_elf couldn't realloc for PH!\n");//it is only a warning yet ,coz the header could be in the same Page and that's not tested yet
		}
		SET_CONTEXT(virt_to_phys(curcontext, (uintptr_t)new_proc->context->pd));
		memset(dest, 0x00000000, ph->mem_size);
		memcpy(dest, src, ph->file_size);

		SET_CONTEXT(curpd_phys);
	}
	//print_symbols(image,(struct elf_section_header*)((uintptr_t)(image)+header->sh_offset),header->sh_entry_count);
	return 0;
}
void * init_shared_lib(void* image, vmm_context * context)
{
	/*
	* FIXME Wir muessen eigentlich die Laenge vom Image pruefen, damit wir bei
	* korrupten ELF-Dateien nicht ueber das Dateiende hinauslesen.
	*/
	
	struct elf_header* header = image;
	struct elf_program_header* ph;
	int i;
	vmm_context* curcontext=NULL;
	void * dest=NULL;
	uintptr_t file_dest=0x0;
	if(!intr_activated)
	{
		curcontext= &startup_context;
	}
	else
	{
		curcontext= current_thread->proc->context;
	}
	uintptr_t curpd_phys= virt_to_phys(curcontext, (uintptr_t)curcontext->pd);
	/* Ist es ueberhaupt eine ELF-Datei? */
	//kprintf("0x%x",(uintptr_t)image);
	if (header->i_magic != ELF_MAGIC) 
	{
		kprintf("[ELF_LOADER] E: init_elf couldn't find valid ELF-Magic!\n");
		return NULL;
	}
#ifdef ARCH_X86
	if (header->i_class != ELF_CLASS_32)
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with class != 32!\n");
		return NULL;
	}
	if ((header->i_data != ELF_DATA_LITTLEENDIAN)||(header->version != ELF_DATA_LITTLEENDIAN))
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with data != ELF_DATA_LITTLEENDIAN!\n");
		return NULL;
	}
	if (header->type != ELF_TYPE_DYN)
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with type != ELF_TYPE_EXEC\n");
	}
	if (header->machine != ELF_MACHINE_386)
	{
		kprintf("[ELF_LOADER] E: init_elf found elf with invalid target!\n");
	}
#else
    #error lizarx build: No valid arch found in src/kernel/mt/threads.c
#endif
	if (header->i_version != ELF_VERSION_CURRENT)
	{
	    kprintf("[ELF_LOADER] E: init_elf found version != ELF_VERSION_CURRENT!\n");
	    return NULL;
	}
	/*
	* Alle Program Header durchgehen und den Speicher an die passende Stelle
	* kopieren.
	*
	* FIXME Wir erlauben der ELF-Datei hier, jeden beliebigen Speicher zu
	* ueberschreiben, einschliesslich dem Kernel selbst.
	*/
	file_dest = (uintptr_t)uvmm_malloc(context,elf_get_size(header));
	ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
	for (i = 0; i < header->ph_entry_count; i++, ph++)
	{
		if (ph->type != ELF_PROGRAM_TYPE_LOAD)
		{
			continue;
		}
		dest = (void*)(file_dest+ph->offset);
		kprintf("offset %x\n",(uintptr_t)dest);
		void* src = ((char*) image) + ph->offset;
		SET_CONTEXT(virt_to_phys(curcontext, (uintptr_t)context->pd));
		memset(dest, 0x00000000, ph->mem_size);
		memcpy(dest, src, ph->file_size);
		SET_CONTEXT(curpd_phys);
	}
	//print_symbols(image,(struct elf_section_header*)((uintptr_t)(image)+header->sh_offset),header->sh_entry_count);
	return (void*)file_dest;
}
void link_against(struct elf_header* elf_main,struct elf_header* elf_lib)
{
	
}
void link_lib_against(struct elf_header* elf_main,struct elf_header* elf_lib)
{
	
}
void link_main_against(struct elf_header* elf_main,struct elf_header* elf_lib,vmm_context * context, uintptr_t B)
{
	unsigned int i =0;
	uint_t * cur_reloc_field=NULL;
	struct elf_rel * cur_rel = NULL;
	
	struct elf_section_header * dynamic_main_sh=get_dynamic(elf_main);
	struct elf_section_header * dynamic_lib_sh=get_dynamic(elf_lib);
	
	struct dynamic * dynamic_main = section_to_dynamic
	(
		(struct elf_dyn*)((uintptr_t)(elf_main)+
		(dynamic_main_sh->off)),
		
		dynamic_main_sh->size/sizeof(struct elf_dyn),elf_main, 0
	);
	struct dynamic * dynamic_lib = section_to_dynamic( 
		(struct elf_dyn*)((uintptr_t)(elf_lib)+(dynamic_lib_sh->off)), 
		dynamic_lib_sh->size/sizeof(struct elf_dyn),elf_lib, (uintptr_t)elf_lib
	);
	vmm_context* curcontext=NULL;
	
	if(!intr_activated)
	{
		curcontext= &startup_context;
	}
	else
	{
		curcontext= current_thread->proc->context;
	}
	uintptr_t curpd_phys= virt_to_phys(curcontext, (uintptr_t)curcontext->pd);
	
	SET_CONTEXT(virt_to_phys(curcontext, (uintptr_t)context->pd));
	
	for(i=0;i<dynamic_main->jmp_rel_count;i++)
	{
		
		cur_rel=&dynamic_main->jmp_rel[i];
		cur_reloc_field = (uint_t *)cur_rel->offset;
		
		*cur_reloc_field=dynamic_lib->sym[ELF_REL_BIND(cur_rel->info)].value+B;
	}
	for(i=0;i<dynamic_lib->jmp_rel_count;i++)
	{
		cur_rel=&dynamic_lib->jmp_rel[i];
		
		cur_reloc_field = (uint_t *)(cur_rel->offset+B);
		kprintf("reloc field: %p",cur_reloc_field);
		*cur_reloc_field=dynamic_main->sym[ELF_REL_BIND(cur_rel->info)].value;
	}
	
	SET_CONTEXT(curpd_phys);
}
static struct elf_section_header * get_dynamic(struct elf_header* elf)
{
	struct elf_section_header* section_header = (struct elf_section_header*)((uintptr_t)elf+(elf)->sh_offset);
	
	unsigned int i = 0;
	unsigned int section_num = (elf)->sh_entry_count;
	// get 
	for(i=0;i<section_num;i++)
	{
		if(section_header->type==ELF_SECTION_TYPE_DYNAMIC)
		{
			return section_header;
		}
		section_header=(struct elf_section_header*)((uintptr_t)(section_header)+sizeof(struct elf_section_header));
	}
	return NULL;
}
static struct dynamic * section_to_dynamic(struct elf_dyn* cur_dyn,int_t section_dynamic_num,struct elf_header* elf,uintptr_t B)
{
	unsigned int i = 0;
	struct dynamic * new_dynamic=calloc(sizeof(struct dynamic));
	kprintf("dyn at 0x%x size: 0x%x",(uintptr_t)new_dynamic,sizeof(struct dynamic));
	for(i=0;i<section_dynamic_num;i++)
	{
		if(cur_dyn->type == ELF_DYNAMIC_TYPE_SYM_TAB)
		{
			new_dynamic->sym = (struct elf_symbol * )(cur_dyn->un.ptr+B);
			kprintf("sym: %x\n",cur_dyn->un.ptr+B);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_STR_TAB)
		{
			new_dynamic->str = (char*)(cur_dyn->un.ptr+(uintptr_t)(elf)+B);
			kprintf("str: %x\n",cur_dyn->un.ptr+B);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_HASH)
		{
			new_dynamic->hash = (int_t * )cur_dyn->un.ptr+B;
			kprintf("hash: %x\n",cur_dyn->un.ptr+B);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_JMPREL)
		{
			new_dynamic->jmp_rel = (struct	elf_rel * )(cur_dyn->un.ptr+B);
			kprintf("jmprel: %p %x\n",new_dynamic->jmp_rel);
		}
		else if(cur_dyn->type == ELF_DYNAMIC_TYPE_PLTRELSZ)
		{
			new_dynamic->jmp_rel_count = cur_dyn->un.val/sizeof(struct elf_rel);
			kprintf("size: %x\n",new_dynamic->jmp_rel_count);
		}
		cur_dyn=(struct elf_dyn*)((uintptr_t)(cur_dyn)+sizeof(struct elf_dyn));
	}
	// Sanity checks
	if (	(new_dynamic->str == NULL) || 
		(new_dynamic->hash == NULL) || 
		(new_dynamic->sym == NULL) || 
		(new_dynamic->jmp_rel==NULL))
	{
		kprintf("[ELF] Error: section_to_dynamic couldn't find struct dynamic entrys\n");
		/* This file has no symbols */
		return NULL;
	}
	return new_dynamic;
}
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
}
/*
 * We need that later when resolving the debug symbols
 * 
static uint32_t elf_hash(const unsigned char *name)
{
    uint32_t h = 0, g;
    
    while (*name) {
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
    int_t * bucket;
    int_t * chain;
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
        if (!strcmp(sym_name, dyn_struct->str + sym->st_name))
	{
            return sym;
        }
        y = chain[y];
    } while (y);

    return NULL;
}
*/
