/*   <src-path>/src/kernel/boot/init.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
