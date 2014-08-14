/*   <src-path>src/usr/Daemons/VFS/ipc_manager.cpp is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include"ipc_helper.h"
#include<stdint.h>
#include"../dbg.h"
#include<asm_inline.h>

void IPC_Helper::sleep()
{
	SYSCALL(18);
}
void IPC_Helper::wakeup(unsigned long pid)
{
	asm volatile( "nop" :: "d" (pid));
	SYSCALL(19);
}
unsigned long IPC_Helper::shmget()
{
	unsigned long id;
	asm volatile( "nop" :: "d" (0x100));
	SYSCALL(13);
	asm volatile( "nop" : "=d" (id));
	return id;
}
void * IPC_Helper::shmat(unsigned long id)
{
	uintptr_t addr;
	asm volatile( "nop" :: "d" (id));
	SYSCALL(14);
	asm volatile( "nop" : "=d" (addr));
	return (void*)addr;
}
unsigned long IPC_Helper::msgget()
{
	unsigned long id;
	SYSCALL(15);
	asm volatile( "nop" : "=d" (id));
	return id;
}
unsigned long IPC_Helper::msgsnd(unsigned long id, void *ptr, unsigned long size)
{
	unsigned long suc;
	
	asm volatile( "nop" :: "d" ((uintptr_t)ptr));
	asm volatile( "nop" :: "b" (((uintptr_t)size)|(id<<16)) );
	
	SYSCALL(16);
	asm volatile( "nop" : "=c" (suc));
	return suc;
}
unsigned long IPC_Helper::msgrcv(unsigned long id, void *ptr, unsigned long size)
{
	unsigned long suc;
	
	asm volatile( "nop" :: "d" ((uintptr_t)ptr));
	asm volatile( "nop" :: "b" (((unsigned long)size)|(id<<16)));
	SYSCALL(17);
	asm volatile( "nop" : "=d" (suc));
	return suc;
}
