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
#include <mt/cpustate.h>
#include<drv/io/ioport.h>
#include <drv/keyboard/keyboard.h>
#include <drv/timer/timer.h>
#include <intr/irq.h>
#include <mt/ts.h>

cpu_state* handle_irq(cpu_state* cpu)
{       cpu_state* new_cpu = cpu;
	//kprintf("intr end");
	if (cpu->intr >= 0x28) {
	            // EOI an Slave-PIC
	            outb(0xa0, 0x20);
	}else if(cpu->intr==0x21){kbc_handler(0x21);}
	 else if (cpu->intr == 0x20) {
	    timer_handler(&new_cpu);
	}
	// EOI an Master-PIC
	outb(0x20, 0x20);
	//kprintf("intr end");
	return new_cpu;
}
