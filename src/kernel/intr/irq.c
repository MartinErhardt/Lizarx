/*  <src-path>/src/kernel/intr/irq.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include <drv/keyboard/keyboard.h>
#include <drv/timer/timer.h>
#include <intr/irq.h>
#include <mt/threads.h>
#include <asm_inline.h>
#include <local_apic.h>
#include <libOS/lock.h>
#include<mt/sched.h>
#include <intr/err.h>

struct cpu_state* handle_irq(struct cpu_state* cpu)
{
	//OUTB(0x3f8, 'i' );
	if (err_ocurred)
		cpu_halt();
	if (cpu->INFO_INTR == 28)
	{
		sync_addr_space();
		cpu = schedule(cpu);
		local_apic_eoi();
	}
#ifdef ARCH_X86
	if (cpu->INFO_INTR >= 0x28) 
		OUTB(0xa0, 0x20)
	else if(cpu->INFO_INTR==0x21)
		kbc_handler(0x21);
#endif
	OUTB(0x20, 0x20)

	return cpu;
}
