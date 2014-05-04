/*   <src-path>/src/kernel/intr/irq.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <drv/keyboard/keyboard.h>
#include <drv/timer/timer.h>
#include <intr/irq.h>
#include <mt/threads.h>
#include <asm_inline.h>
#include <local_apic.h>
#include <libOS/lock.h>
#include<mt/sched.h>

struct cpu_state* handle_irq(struct cpu_state* cpu)
{
	if (cpu->INFO_INTR == 28)
	{
		cpu = schedule(cpu);
		local_apic_eoi();
		//kprintf("rip at 0x%x",cpu->rip);
	}
#ifdef ARCH_X86
	if (cpu->INFO_INTR >= 0x28) 
	{
		// EOI an Slave-PIC
		OUTB(0xa0, 0x20);
	}
	else if(cpu->INFO_INTR==0x21)
	{
		kbc_handler(0x21);
	}
#endif
		
	// EOI an Master-PIC
	OUTB(0x20, 0x20)

	return cpu;
}
