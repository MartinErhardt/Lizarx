/*   <src-path>/src/kernel/intr/syscall.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<cpu.h>
#include<dbg/console.h>
#include <drv/vga-txt_graphics/vram.h>
#include<intr/syscall.h>
#include<mm/vmm.h>
#include<mm/pmm.h>
#include<mm/vheap.h>
#include<string.h>
#include<boot/init.h>
#include<mt/threads.h>
#include<macros.h>
#include<libOS/lock.h>
#include<../x86_common/local_apic.h>
#include<asm_inline.h>
#include<mt/proc.h>
#include<mt/sched.h>
#include <intr/err.h>
#include <mm/shm.h>
#include <mt/msg.h>

struct cpu_state* handle_syscall(struct cpu_state* cpu)
{
	char* copybuf_ptr;
	uint8_t font;
	size_t bm_size;
	vmm_context* curcontext;
	if (err_ocurred)
		cpu_halt();
	switch(cpu->REG_FUNCRET)
	{
		case(SYS_DRAW):
			
			copybuf_ptr =(void*)cpu->REG_DATA1;
			font = (uint8_t)cpu->REG_DATA0;
			kprintfcol_scr((font>>4),font,copybuf_ptr);
			
		//	kprintf("virt_to_phys at 0x%x",virt_to_phys(get_cur_context(),cpu->rbp&0xfffff000));
		//	kprintf("rbp at 0x%x",cpu->rbp);
			break;
		case(SYS_INFO): break;
		case(SYS_GETTID):break;
		case(SYS_KILLTID):break;
		case(SYS_FORKTID):break;
		case(SYS_GETPID):break;
		case(SYS_KILLPID):break;
		case(SYS_FORKPID):break;
		case(SYS_ERROR):break;
		case(SYS_GET_BOOTMOD):
			bm_size= modules_glob[cpu->REG_DATA0].mod_end-modules_glob[cpu->REG_DATA0].mod_start;
			
			cpu->REG_FUNCRET=(uintptr_t)cpyout((void*) (uintptr_t)modules_glob[cpu->REG_DATA0].mod_start, bm_size);
			cpu->REG_DATA0=bm_size;
			break;
		case(SYS_VMM_MALLOC):
			curcontext=get_cur_context_glob();
			cpu->REG_DATA0=(uintptr_t)uvmm_malloc(curcontext,cpu->REG_DATA0);
			
			break;
		case(SYS_VMM_REALLOC):
			
			curcontext=get_cur_context_glob();
			
			if((vmm_realloc(curcontext,((void*)cpu->REG_DATA0),cpu->REG_DATA1,FLGCOMBAT_USER))<0)
				kprintf("error reallocating");
			break;
		case(SYS_EXIT):
			exit(get_cur_cpu()->current_thread->proc);
			asm volatile("int $28"); //FIXME
			cpu = schedule(cpu);
			
			break;
		case(SYS_SHMGET):
			cpu->REG_DATA0 = shmget(0, cpu->REG_DATA0, 0);
			break;
		case(SYS_SHMAT):
			cpu->REG_DATA0 = (uintptr_t)shmat(cpu->REG_DATA0, 0, 0);
			break;
		case(SYS_MSGGET):
			cpu->REG_DATA0 = msgget(0, 0);
			break;
		case(SYS_MSGSND):
			cpu->REG_DATA0 = msgsnd(cpu->REG_DATA1>>16, (void*)cpu->REG_DATA0, cpu->REG_DATA1, 0);
			break;
		case(SYS_MSGRCV):
			cpu->REG_DATA0 = msgrcv(cpu->REG_DATA1>>16, (void*)cpu->REG_DATA0, cpu->REG_DATA1, 0, 0);
			break;
		case(SYS_SLEEP):
			cpu = schedule(cpu);
			sleep(0);
			asm volatile("int $28");
			break;
		case(SYS_WAKEUP):
		  kprintf("wakeup");
			wakeup(cpu->REG_DATA0);
			break;
		default:break;
	}
	/*if(!cpu->REG_IP)
	{
		kprintf("[IRG] E: handle_irq notices !next_context bug\n");
		while(1);
	}*/
	//kprintf("stack at %x",rdmsr(0xC0000102));
	//kprintf("id: %d ",get_cur_cpu()->apic_id);
	return cpu;
}
#ifdef ARCH_X86_64
void init_SYSCALL()
{
	wrmsr(MSR_STAR,	( ( ((uint64_t) (USER_CODE_SEG32_N<<3) | 0x3)<<48) |	// Sysret cs
			( ((uint64_t) (KERNEL_CODE_SEG_N<<3) ) <<32) )		// Syscall cs
			&0xffffffff00000000LL					// first 32 bytes are reserved
	);
	
	wrmsr(MSR_LSTAR,(uintptr_t) &syscall_stub);
	wrmsr(MSR_EFER,rdmsr(MSR_EFER)|0x0000000000000001); // set EFER.SCE
	wrmsr(0xC0000084, 0x0000000000000200LL);
	wrmsr(0xC0000102, get_cur_cpu()->stack+STDRD_STACKSIZ-0x10);
}
#endif