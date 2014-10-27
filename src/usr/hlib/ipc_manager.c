/*  <src-path>/src/usr/hlib/ipc_manager.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include"ipc_helper.h"
#include<stdint.h>
#include"../dbg.h"
#include<asm_inline.h>
#include<syscall.h>
#include<sys/types.h>
#include"AList.h"
#include"../hlib_vfs.h"
struct sys_msg
{
	int id;
	void* ptr;
	size_t size;
};
void hlib_sleep()
{
	SYSCALL(SYS_SLEEP);
}
void hlib_wakeup(unsigned long pid)
{
	SET_ARG1(pid)
	SYSCALL(SYS_WAKEUP)
}
int hlib_shmget(size_t size)
{
	int id;
	SET_ARG1(size)
	SYSCALL(SYS_SHMGET)
	GET_RET1(id)
	return id;
}
void * hlib_shmat(int id)
{
	uintptr_t addr;
	SET_ARG1(id)
	SYSCALL(SYS_SHMAT)
	GET_RET1(addr)
	return (void*)addr;
}
int hlib_msgget()
{
	int id;
	SYSCALL(SYS_MSGGET);
	GET_RET1(id);
	return id;
}
int hlib_msgsnd(int id, void *ptr, size_t size)
{
	unsigned long suc;
	struct sys_msg my_args;
	my_args.id=id;
	my_args.ptr=ptr;
	my_args.size=size;
	SET_ARG1((uintptr_t)&my_args)
	SYSCALL(SYS_MSGSND)
	GET_RET1(suc)
	return suc;
}
int hlib_msgrcv(int id, void *ptr, size_t size)
{
	unsigned long suc;
	struct sys_msg my_args;
	my_args.id=id;
	my_args.ptr=ptr;
	my_args.size=size;
	SET_ARG1((uintptr_t)&my_args)
	SYSCALL(SYS_MSGRCV)
	GET_RET1(suc)
	return suc;
}
int hlib_semget(int id ,int nsems,int flag)
{
	int suc;
	SET_ARG1(nsems);
	SYSCALL(SYS_SEMGET);
	GET_RET1(suc);
	return suc;
}
int hlib_semop(int semid, struct sem_buf* sops, size_t nsops)
{
	int suc;
	struct sys_semop my_args;
	my_args.id=semid;
	my_args.sops=sops;
	my_args.nsops=nsops;
	SET_ARG1((uintptr_t)&my_args);
	SYSCALL(SYS_SEMOP);
	GET_RET1(suc);
	return suc;
}
void hlib_semup(int semid)
{
	struct sem_buf up;
	up.sem_num=0;
	up.semop=1;
	up.sem_flg=0;
	IPC_Helper::semop(semid, &up, 1);
}
void hlib_semdown(int semid)
{
	struct sem_buf down;
	down.sem_num=0;
	down.semop=-1;
	down.sem_flg=0;
	IPC_Helper::semop(semid, &down, 1);
}
/*int semctl()
{
	int suc;
	SYSCAL	
}*/
