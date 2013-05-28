/*   <src-path>/src/kernel/intr/syscall.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<dbg/console.h>
#include <drv/vga-txt_graphics/vram.h>
#include<intr/syscall.h>
#include<mm/vmm.h>
#include<mm/pmm.h>
#include<mm/vheap.h>
#include<string.h>
#include<boot/init.h>
#include<mt/threads.h>
CPU_STATE* handle_syscall(CPU_STATE* cpu)
{
	char* copybuf_ptr;
	uint8_t font;
	size_t bm_size;
	vmm_context* curcontext;
	kprintf("sbrk was here");
	switch(cpu->REG_FUNCRET){
	    case(SYS_DRAW):
	        
		copybuf_ptr =(void*)cpu->REG_DATA1;
		font = (uint8_t)cpu->REG_DATA0;
		kprintfcol_scr((font>>4),font,copybuf_ptr);
		
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
		cpu->REG_DATA0=(uintptr_t)cpyout((void*) modules_glob[cpu->REG_DATA0].mod_start, bm_size);
		cpu->REG_DATA1=bm_size;
		break;
	    case(SYS_VMM_MALLOC):
		curcontext=NULL;
		if(current_thread==NULL)
		{
			curcontext= &startup_context;
		}
		else
		{
			curcontext= current_thread->proc->context;
		}
		cpu->REG_DATA0=(uintptr_t)uvmm_malloc(curcontext,cpu->REG_DATA0);
		
		break;
	    default:break;
	}
	return cpu;
}