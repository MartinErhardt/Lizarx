/*   <src-path>src/kernel/dbg/stack_trace.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<hal.h>
#include<stdint.h>
#include<mt/elf.h>
#include<dbg/console.h>

//-fno-omit-frame-pointer for user stacktracing

int get_stack_trace(void* elf, uintptr_t start_base_ptr, uintptr_t start_instr_ptr)
{
	struct stack_frame * cur_stack_frame;
	uintptr_t* last_func;
	kprintf("kernel stack_trace(last recent call first):\n");
#ifndef F_DBG
	kprintf("disable -Os in EXTRA_FLAGS in src/kernel/Makefile to achieve better results\n");
#endif
	if (start_base_ptr)
	{
		cur_stack_frame = (struct stack_frame*) start_base_ptr;
	}
	else
	{
		return -1;
	}
	while(cur_stack_frame->base_ptr != 0)
	{
		last_func=get_last_function(elf,cur_stack_frame->return_addr);
		kprintf("<%s + 0x%x>\n",
			(const char*)last_func[1],
			cur_stack_frame->return_addr-((struct elf_symbol*)last_func[0])->value);
		cur_stack_frame = cur_stack_frame->base_ptr;
	}
	return 0;
}
