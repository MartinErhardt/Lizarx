/*  <src-path>/src/usr/hlib/multiboot_modules.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include<asm_inline.h>
#include<syscall.h>
#include<stdint.h>
#include"multiboot_modules.h"
#include"dbg.h"
void get_bootmod(int num, struct bootmod* fill_it)
{
	uintptr_t mod_addr;
	size_t mod_size;
	SET_ARG1(num)
	//enum syscalls sys_= syscalls.SYS_GET_BOOTMOD;
	SYSCALL((unsigned long)SYS_GET_BOOTMOD)
	GET_RET2(mod_size)
	GET_RET1(mod_addr)
	fill_it->size = mod_size;
	fill_it->start = (void * )mod_addr;
}
