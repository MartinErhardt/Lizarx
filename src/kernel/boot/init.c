#include <dbg/console.h>
#include <intr/idt.h>
#include <mm/gdt.h>
#include <drv/keyboard/keyboard.h>
#include <mt/ts.h>
#include <mt/zygote.h>
#include <boot/multiboot.h>
#include <string.h>
#include <drv/vga-txt_graphics/vram.h>
#include <mt/elf.h>
#include <mm/vmm.h>
#include <mm/pmm.h>

void init(multiboot_info *mb_info)
{
    uint32_t i;
    /*
     * Nachdem die physische Speicherverwaltung initialisiert ist, duerfen wir
     * die Multiboot-Infostruktur nicht mehr benutzen, weil sie bei der
     * Initialisierung nicht reserviert wurde und daher nach dem ersten
     * pmm_alloc() ueberschrieben werden koennte.
     */
    memset(0x0,0x0,4);
    clrscr(VGA_BLACK,VGA_WHITE);
    kprintfcol_scr(VGA_RED,VGA_WHITE,"[INIT] I: init started\n");
    pmm_init(mb_info);
    
    vmm_init();
//while(1){}
    
    //kpmm_test();
    //init_mt();
    //kprintf("Hello World \n Integer %s",0x20);
    
    
    //init_keyboard();
    init_gdt();
    init_idt();
    kprintf("[INIT] I: init loads Bootmods...");
    if(mb_info->mbs_mods_count ==0){
	kprintf("FAILED No Programs found\n");
    }else{
	multiboot_module* modules = mb_info->mbs_mods_addr;
	for(i=0;i<mb_info->mbs_mods_count;i++){
	    //size_t length = modules[i].mod_end - modules[i].mod_start;
	    if(init_elf((void*) modules[i].mod_start)==0){
		kprintf("SUCCESS with mod: %d",i);
	    }else{
		kprintf("FAILED with mod: %d",i);
	    }
	}
    }
    kprintf("\n");

    enable_intr();
}
