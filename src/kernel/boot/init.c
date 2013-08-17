/*   <src-path>/src/kernel/boot/init.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <dbg/console.h>
#include <intr/idt.h>
#include <mm/gdt.h>
#include <drv/keyboard/keyboard.h>
#include <boot/multiboot.h>
#include <string.h>
#include <drv/vga-txt_graphics/vram.h>
#include <mt/elf.h>
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <mm/vheap.h>
#include <mt/threads.h>
#include <mt/proc.h>
#include <drv/hwtime/hwtime.h>
#include <stdlib.h>
#include <boot/init.h>

void init(struct multiboot_info * mb_info)
{
	// initialize global variables
	uint32_t i;
	first_thread = NULL;
	current_thread = NULL;
	first_proc = NULL;
	cur_proc = NULL;
	startup_context.highest_paging=0x0;
	startup_context.mm_tree=0x0;
	//struct tm* time_is=NULL;
	struct multiboot_module* modules = (struct multiboot_module*) ((uintptr_t)(mb_info->mbs_mods_addr) & 0xffffffff);
	modules_glob=modules;

	kernel_elf=(void * )(uintptr_t)modules[0].mod_start;
#ifdef ARCH_X86
	clrscr(VGA_BLACK,VGA_WHITE);
	init_gdt();
#else
	setcurs(23,1);
	kprintf("... SUCCESS\n");
#endif
	kprintf("[INIT] I: init started\n");
	
	mp_init();
	cpu_caps();
	
	
	pmm_init(mb_info);
	vmm_init();
	vheap_init();
	
	init_idt();
	
	kprintf("[INIT] I: init loads Bootmods...");
	if(mb_info->mbs_mods_count ==0)
	{
	    //kprintf("FAILED No Programs found\n");
	}
	else
	{
		for(i=2;i<mb_info->mbs_mods_count;i++)// first boot mod is kernel itself
		{
			if(init_elf((void*) (uintptr_t)modules[i].mod_start))
			{
				kprintf("FAILED with mod: %d",i);
			}
		}
	}
	kprintf(" SUCCESS\n");
#ifdef ARCH_X86_64
	
	tss.rsp0=((uintptr_t)kvmm_malloc(0x1000))+0xff0;
	
	setup_tss();
#endif
	//that's for testing purposes
	//time_is = get_time();
	enable_intr();
	while(1);
}
