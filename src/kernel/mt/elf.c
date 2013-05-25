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
//static uint32_t elf_hash(const unsigned char *name);
//static struct elf_symbol * get_sym_by_name(struct dynamic * dyn_struct, char * sym_name);

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
