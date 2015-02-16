/*  <src-path>/src/usr/arch/x86_64/syscall.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef SYSCALL_H_
#define SYSCALL_H_
typedef enum {
	SYS_DRAW,
	SYS_INFO,
	SYS_GETTID,
	SYS_KILLTID,
	SYS_FORKTID,
	SYS_GETPID,
	SYS_KILLPID,
	SYS_FORKPID,
	SYS_ERROR,
	SYS_GET_BOOTMOD,
	SYS_VMM_MALLOC,
	SYS_EXIT,
	SYS_SHMGET,
	SYS_SHMAT,
	SYS_MSGGET,
	SYS_MSGSND,
	SYS_MSGRCV,
	SYS_SLEEP,
	SYS_WAKEUP,
	SYS_SEMGET,
	SYS_SEMOP
} syscalls;
#endif
