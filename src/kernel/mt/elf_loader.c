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

int32_t init_elf(void* image,int snd)
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
    if(!intr_activated){
        curcontext= &startup_context;
    }else{
        curcontext= current_thread->proc->context;
    }
    uintptr_t curpd_phys= virt_to_phys(curcontext, (uintptr_t)curcontext->pd);
    /* Ist es ueberhaupt eine ELF-Datei? */
    //kprintf("0x%x",(uintptr_t)image);
    if (header->magic != ELF_MAGIC) {
        kprintf("[ELF_LOADER] E: init_elf couldn't find valid ELF-Magic!\n");
        return -1;
    }
    //asm volatile("int $0xe");

    new_proc=create_proc();
    //kprintf("crthread pid= 0x%x",new_proc->p_id);
    //if(snd){while(1){};}
    create_thread((void*) header->entry,new_proc->p_id);
    
    //kprintf("virt at 0x%x phys at 0x%x",virt_to_phys(curcontext, (uintptr_t)new_proc->context->pd),new_proc->context->pd);
    //if(snd){while(1);}
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
	if(vmm_realloc(new_proc->context,(void*)ph->virt_addr,ph->mem_size,FLGCOMBAT_USER)<0){
	    kprintf("[ELF_LOADER] W: init_elf couldn't realloc for PH!\n");//it is only a warning yet ,coz the header could be in the same Page and that's not tested yet
	}
	
	
	SET_CONTEXT(virt_to_phys(curcontext, (uintptr_t)new_proc->context->pd));

        memset(dest, 0x00000000, ph->mem_size);
	//kprintf("src=  0x%x dest= 0x%x size = 0x%x",(uintptr_t)src,(uintptr_t)dest,ph->file_size);
        memcpy(dest, src, ph->file_size);
	
	SET_CONTEXT(curpd_phys);
    }

    return 0;
}