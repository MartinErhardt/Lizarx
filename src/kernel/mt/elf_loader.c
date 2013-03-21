#include<mt/elf.h>
#include<mt/ts.h>
#include<dbg/console.h>
#include<string.h>
#include<mm/vmm.h>
#include<mm/pmm.h>

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

    vmm_context* curcontext=getcurcontext();
    
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
	//kprintf("header at %d ph is 0x%x",(uintptr_t)ph,ph->offset);
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
	vmm_set_context(new_task->context);

        memset(dest, 0x00000000, ph->mem_size);
	//kprintf("src=  0x%x dest= 0x%x size = 0x%x",(uintptr_t)src,(uintptr_t)dest,ph->file_size);
        memmove(dest, src, ph->file_size);
	//while(1){}
	vmm_set_context(curcontext);
	//while(1){}
	//kprintf("hello");
    }
    //while(1){}
//kprintf("hello");
    return 0;
}