/*   <src-path>/src/kernel/drv/timer/timer.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<drv/timer/timer.h>
#include<mt/sched.h>
#include<stdint.h>
#include<dbg/console.h>
#include<mt/threads.h>
#include<cpu.h>

uint32_t time_intrs=0;
cpu_state* timer_handler(cpu_state* new_cpu)
{
	time_intrs++;
	cpu_state* state = schedule(new_cpu);
	return state;
}
