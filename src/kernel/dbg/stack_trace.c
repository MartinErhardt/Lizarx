/*   <src-path>src/kernel/dbg/stack_trace.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<stdint.h>
#include<mt/elf.h>
#include<dbg/console.h>
#include<mt/elf.h>
#include<mt/threads.h>
//-fno-omit-frame-pointer for user stacktracing

int get_stack_trace(void* elf, uintptr_t start_base_ptr, uintptr_t start_instr_ptr)
{
	struct stack_frame * cur_stack_frame;
	uintptr_t* last_func;
	kprintf("kernel stack_trace(last recent call first):\n");
#ifndef F_DBG
	kprintf("disable -Os in EXTRA_FLAGS in src/kernel/Makefile to achieve better results\n");
#endif
	vmm_context* curcontext=get_cur_context();
	
	if ((start_base_ptr)&&(vmm_is_alloced(curcontext,start_base_ptr/PAGE_SIZE)))
	{
		cur_stack_frame = (struct stack_frame*) start_base_ptr;
	}
	else
	{
		return -1;
	}
	
	while((cur_stack_frame->base_ptr != 0)&&(vmm_is_alloced(curcontext, (uintptr_t)cur_stack_frame->base_ptr/PAGE_SIZE)))
	{
		last_func=get_last_function(elf,cur_stack_frame->return_addr);
		kprintf("<%s + 0x%x>\n",
			(const char*)last_func[1],
			cur_stack_frame->return_addr-((struct elf_symbol*)last_func[0])->value);
		cur_stack_frame = cur_stack_frame->base_ptr;
	}
	return 0;
}
