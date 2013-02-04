#include <dbg/console.h>
#include <intr/idt.h>
#include <mm/gdt.h>
#include <drv/keyboard/keyboard.h>
#include <mt/ts.h>
#include <mt/zygote.h>
#include <mm/pmm.h>
#include <boot/multiboot.h>
#include <string.h>
#include <drv/vga-txt_graphics/vram.h>
#include <mt/elf.h>

void init(multiboot_info *mb_info)
{
    uint32_t i;
    /*
     * Nachdem die physische Speicherverwaltung initialisiert ist, duerfen wir
     * die Multiboot-Infostruktur nicht mehr benutzen, weil sie bei der
     * Initialisierung nicht reserviert wurde und daher nach dem ersten
     * pmm_alloc() ueberschrieben werden koennte.
     */
    kpmm_init(mb_info);
    
    clrscr(VGA_BLACK,VGA_WHITE);
    
    //kpmm_test();
    //init_mt();
    //kprintf("Hello World \n Integer %s",0x20);
    
    kprintfcol_scr(VGA_RED,VGA_WHITE,"[INIT] started\n");
    init_keyboard();
    init_gdt();
    init_idt();
    if(mb_info->mbs_mods_count ==0){
	kprintf("[INIT] Multitasking... FAILED No Programs found");
    }else{
	multiboot_module* modules = mb_info->mbs_mods_addr;
	for(i=0;i<mb_info->mbs_mods_count;i++){
	    size_t length = modules[i].mod_end - modules[i].mod_start;
	    kpmm_realloc((void*)(0x200000),length);
	    init_elf((void*) modules[i].mod_start);
	}
    }
    enable_intr();
}
