/*  <src-path>/src/usr/hlib/vmm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
// TODO change to mmap
#include"vmm.h"
#include<asm_inline.h>
#include<syscall.h>
unsigned long vmm_malloc(unsigned long size)
{
	unsigned long new_space=0x0;
	SET_ARG1(size)
	SYSCALL(SYS_VMM_MALLOC)
	GET_RET1(new_space)
	return new_space;
}
