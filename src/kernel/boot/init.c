/*  <src-path>/src/kernel/boot/init.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
 *  Copyright (C) 2013, 2014  martin.erhardt98@googlemail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
#include <drv/hwtime/hwtime.h>
#include <ipc/sem.h>
#include <ipc/msg.h>
#include <ipc/shm.h>
#include <hw_structs.h>
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
	sem_atomic			= LOCK_FREE;
	msq_lock			= LOCK_FREE;
	err_ocurred 			= 0;
	all_APs_booted			= LOCK_USED;
	to_invalidate_first		= NULL;
	memset(&proc_list, 0, sizeof(alist_t));
	memset(&cpu_list, 0, sizeof(alist_t));
	memset(&sem_groups, 0, sizeof(alist_t));
	memset(&msqid_list,0, sizeof(alist_t));
	memset(&shmid_list,0, sizeof(alist_t));
	struct multiboot_module * modules = (struct multiboot_module *) ((uintptr_t)(mb_info->mbs_mods_addr) & 0xffffffff);
	modules_glob			= modules;
	
	kernel_elf = (void * )(uintptr_t)modules[0].mod_start;
#ifdef ARCH_X86
	clrscr(VGA_BLACK,VGA_WHITE);
	init_gdt();
#else
	setcurs(23,1);
	kprintf("... SUCCESS\n");
#endif
	
	kprintf("[INIT] I: init started\n");

	pmm_init_mmap(mb_info);
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
		cpu_caps((uintptr_t)bsp_info.stack);
		if(cores_from_tables - 1)
			startup_APs();
	}
#endif

	bsp_info.stack = ((uintptr_t)kvmm_malloc(STDRD_STACKSIZ));
	kprintf("[INIT] I: init loads Bootmods...");
	if(mb_info->mbs_mods_count ==0)
		kprintf("FAILED No Programs found\n");
	else
		for(i=2;i<4;i++)// first boot mod is kernel itself
			if(init_elf((void*) (uintptr_t)modules[i].mod_start))
				kprintf("FAILED with mod: %d",i);
	kprintf(" SUCCESS\n");
#ifdef ARCH_X86_64
	init_SYSCALL();
	tss.rsp0 = bsp_info.stack+STDRD_STACKSIZ-sizeof(struct cpu_state)-0x10;
	
	setup_tss();
#else
	tss.esp0 = bsp_info.stack+STDRD_STACKSIZ-sizeof(struct cpu_state)-0x10;
#endif
	all_cores_mask |= (1<<(get_cur_cpu()->apic_id) );
	//kprintf("console at 0x%p", &console_lock);
	/*time_is = get_time();
	time_t timer = mktime(time_is);
	kprintf("UNIX time is  %d \n",timer);
	gmtime_r(&timer, time_is); 
	print_time(time_is);*/
	//kprintf("cores_mask 0x%x",all_cores_mask);
	enable_intr();
	
	while(1);
}
void AP_init()
{
	spinlock_ackquire(((lock_t *)TRAMPOLINE_LOCK));
	//local_apic_init_AP();
	uintptr_t stack = ((uintptr_t)kvmm_malloc(STDRD_STACKSIZ));
#if defined(ARCH_X86) || defined(ARCH_X86_64)
	asm volatile("nop":: "a"(stack+ STDRD_STACKSIZ-sizeof(struct cpu_state)-0x10));
#ifdef ARCH_X86
	asm volatile ("mov %eax, %esp" );
#endif
#ifdef ARCH_X86_64
	asm volatile ("mov %rax, %rsp" );
#endif
//	memset((void*)stack, 0, STDRD_STACKSIZ);
	local_apic_init_AP();
	cpu_caps(stack);
	init_gdt_AP();
	init_idt_AP();
#ifdef ARCH_X86_64
	init_SYSCALL();
#endif
#endif
	cores_booted++;
	all_cores_mask |= (1<<(get_cur_cpu()->apic_id) );
	//
	local_apic_eoi();
	
	if(cores_booted == cores_from_tables)
		spinlock_release(&all_APs_booted);
	else
		spinlock_release(((lock_t *)TRAMPOLINE_LOCK));
	ENABLE_INTR
	
	while(1);
}

