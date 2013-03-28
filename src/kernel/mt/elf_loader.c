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
#include<mt/ts.h>
#include<dbg/console.h>
#include<string.h>
#include<mm/vmm.h>
#include<hal.h>
int32_t init_elf(void* image)
{
    /*
     * FIXME Wir muessen eigentlich die Laenge vom Image pruefen, damit wir bei
     * korrupten ELF-Dateien nicht ueber das Dateiende hinauslesen.
     */
     
    struct elf_header* header = image;
    struct elf_program_header* ph;
    int i;
    struct task* new_task=NULL; 

    vmm_context* curcontext=NULL;
    if(!intr_activated){
        curcontext= &startup_context;
    }else if((current_task==NULL)){
        
    }else{
        curcontext= current_task->context;
    }
    
    /* Ist es ueberhaupt eine ELF-Datei? */
    //kprintf("0x%x",(uintptr_t)image);
    if (header->magic != ELF_MAGIC) {
        kprintf("[ELF_LOADER] E: init_elf couldn't find valid ELF-Magic!\n");
        return -1;
    }
    //asm volatile("int $0xe");
    new_task=init_task((void*) header->entry);
    
    /*
     * Alle Program Header durchgehen und den Speicher an die passende Stelle
     * kopieren.
     *
     * FIXME Wir erlauben der ELF-Datei hier, jeden beliebigen Speicher zu
     * ueberschreiben, einschliesslich dem Kernel selbst.
     */
    //kprintf("dest: 0x%x",dest);
    
    ph = (struct elf_program_header*) (((char*) image) + header->ph_offset);
    for (i = 0; i < header->ph_entry_count; i++, ph++) {
        void* dest = (void*) ph->virt_addr;
        void* src = ((char*) image) + ph->offset;

	/* Nur Program Header vom Typ LOAD laden */
        if (ph->type != 1) {
            continue;
        }
        
	if(ph->virt_addr<KERNEL_SPACE){
	    kprintf("[ELF_LOADER] E: init_elf an elf want to be loaded at %x ; That's in Kernelspace!\n",ph->virt_addr);
	}
	
	//kprintf("[ELF_LOADER] E: init_elf an elf want to be loaded at %x ; That's not in Kernelspace!\n",ph->virt_addr);
	if(vmm_realloc(new_task->context,(void*)ph->virt_addr,ph->mem_size,FLGCOMBAT_USER)<0){
	    kprintf("[ELF_LOADER] W: init_elf couldn't realloc for PH!\n");//it is only a warning yet ,coz the header could be in the same Page and that's not tested yet
	}
	//vmm_map_page(curcontext,vmm_find_freemem(ph->file_size/PAGE_SIZE,0x0,KERNEL_SPACE),pmm_malloc());
	SET_CONTEXT(new_task->context);

        memset(dest, 0x00000000, ph->mem_size);
	//kprintf("src=  0x%x dest= 0x%x size = 0x%x",(uintptr_t)src,(uintptr_t)dest,ph->file_size);
        memcpy(dest, src, ph->file_size);
	//while(1){}
	SET_CONTEXT(curcontext);
	//while(1){}
	//kprintf("hello");
    }
    //while(1){}
//kprintf("hello");
    return 0;
}