#include <dbg/console.h>
#include <intr/idt.h>
#include <mm/gdt.h>
#include <drv/keyboard/keyboard.h>
#include <mt/ts.h>
#include <mt/zygote.h>
#include <mm/pmm.h>
#include <boot/multiboot.h>
#include <string.h>

void init(multiboot_info *mb_info)
{
    uint32_t i;
    void* load_addr = (void*)0x200000;//kmalloc(16384); // FIXME We suppose all modules to be 16 mb large at all
    /*
     * Nachdem die physische Speicherverwaltung initialisiert ist, duerfen wir
     * die Multiboot-Infostruktur nicht mehr benutzen, weil sie bei der
     * Initialisierung nicht reserviert wurde und daher nach dem ersten
     * pmm_alloc() ueberschrieben werden koennte.
     */
    kpmm_init(mb_info);
    clrscr();
    krealloc((void*)(0x200000));
    //kpmm_test();
    //kprintf("Hello World!\n",0x0A,0x00);
    
    //kprintfcol(0xF,FALSE,0xA,FALSE,"Hello World!\n");
    //kprintfcol(0x2,TRUE,0xC,FALSE,"Hello World!\n");
    
    //init_mt();
    //kprintf("Hello World \n Integer %s",0x20);
    kprintfcol(0xF,0xA,TRUE,"HelloWorld\n");
    init_keyboard();
    init_gdt();
    init_idt();
    if(mb_info->mbs_mods_count ==0){
	kprintf("[INIT] Multitasking - FAILED No Programs found");
    }else{
	multiboot_module* modules = mb_info->mbs_mods_addr;
	for(i=0;i<mb_info->mbs_mods_count;i++){
	    size_t length = modules[i].mod_end - modules[i].mod_start;
	    //load_addr = (void*)((uintptr_t)(load_addr)+length*4);
	    memcpy(load_addr, (void*) modules[i].mod_start, length);
	    init_task(load_addr);
	}
    }
}
