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
#include <idt.h>
#include <gdt.h>
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
#include <smp_capabilities.h>
#include <stdlib.h>
#include <boot/init.h>
#include <local_apic.h>
#include <cpu.h>
#include <libOS/lock.h>
#include <intr/irq.h>
#include <intr/syscall.h>
#include <asm_inline.h>
#include <libOS/list.h>
#include <intr/err.h>

uint32_t cores_booted			= 1; 

/* / \
    | the BSP is booted
*/
void init(struct multiboot_info * mb_info)
{
	// initialize global variables
	uint32_t i;
	startup_context.highest_paging	= 0;
	startup_context.mm_tree		= 0;
	apic_ready			= LOCK_FREE;
	to_flush			= 0;
	cores_from_tables		= 0;
	all_cores_mask			= 0;
	process_system_lock		= LOCK_FREE;
	heap_lock			= LOCK_FREE;
	invld_lock			= LOCK_FREE;
	err_ocurred 			= 0;
	all_APs_booted			= LOCK_USED;
	//struct tm* time_isi		= NULL;
	to_invalidate_first		= NULL;
	//memset(&proc_list, 0, sizeof(struct alist_st));
	struct multiboot_module * modules = (struct multiboot_module *) ((uintptr_t)(mb_info->mbs_mods_addr) & 0xffffffff);
	modules_glob			= modules;
	
	kernel_elf=(void * )(uintptr_t)modules[0].mod_start;
#ifdef ARCH_X86
	clrscr(VGA_BLACK,VGA_WHITE);
	init_gdt();
#else
	setcurs(23,1);
	kprintf("... SUCCESS\n");
#endif
	kprintf("[INIT] I: init started\n");
	
	pmm_init(mb_info);
	vmm_init();
	vheap_init();
#if defined(ARCH_X86) || defined(ARCH_X86_64)
	init_idt();
	
	uintptr_t apic_support_ = check_mp();
	
	if(apic_support_)
	{
		
		local_apic_init(apic_support_);
		
		cores_booted=1;
		cpu_caps();
		if(cores_from_tables - 1)
			startup_APs();
	}
#endif
	
	bsp_info.stack = ((uintptr_t)kvmm_malloc(STDRD_STACKSIZ));
	
	kprintf("[INIT] I: init loads Bootmods...");
	if(mb_info->mbs_mods_count ==0)
	    kprintf("FAILED No Programs found\n");
	else
		for(i=2;i<mb_info->mbs_mods_count;i++)// first boot mod is kernel itself
			if(init_elf((void*) (uintptr_t)modules[i].mod_start))
				kprintf("FAILED with mod: %d",i);
	kprintf(" SUCCESS\n");
#ifdef ARCH_X86_64
	init_SYSCALL();
	tss.rsp0 = bsp_info.stack+STDRD_STACKSIZ-0x10;
	
	setup_tss();
#else
	tss.esp0 = bsp_info.stack+STDRD_STACKSIZ-0x10;
#endif
	all_cores_mask |= (1<<(get_cur_cpu()->apic_id) );
	
	//kprintf("cores_mask 0x%x",all_cores_mask);
	enable_intr();
	
	while(1);
}
void AP_init()
{
	spinlock_ackquire(((lock_t *)0x7208));
	//local_apic_init_AP();
	uintptr_t stack = ((uintptr_t)kvmm_malloc(STDRD_STACKSIZ));
	
	asm volatile("nop":: "a"(stack+ STDRD_STACKSIZ-0x10));
#ifdef ARCH_X86
	asm volatile ("mov %eax, %esp" );
#endif
#ifdef ARCH_X86_64
	asm volatile ("mov %rax, %rsp" );
#endif
	local_apic_init_AP();
	cpu_caps();
	get_cur_cpu()->stack = stack;

	init_gdt_AP();
	init_idt_AP();
#ifdef ARCH_X86_64
	init_SYSCALL();
#endif
	cores_booted++;
	all_cores_mask |= (1<<(get_cur_cpu()->apic_id) );
	//
	local_apic_eoi();
	
	if(cores_booted == cores_from_tables)
		spinlock_release(&all_APs_booted);
	else
		spinlock_release(((lock_t *)0x7208));
	
	ENABLE_INTR
	
	while(1);
}

