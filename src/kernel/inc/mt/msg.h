/*   <src-path>/src/kernel/mt/inc/msg.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef MSG_H
#define MSG_H

#include<mt/proc.h>
#include<stdint.h>
#include<libOS/list.h>

struct msqid_ds
{
	uint_t id;
	//uint_t msg_qnum;
	//pid_t msg_lrpid;
	//pid_t msg_lspid;
	uint_t * first_message;
};
lock_t msq_lock;
alist_t msqid_list;

int       msgctl(int msqid, int cmd, struct msqid_ds * buf);
uint_t       msgget(key_t key, int flag);
ssize_t   msgrcv(uint_t id, void * ptr, size_t size, long type, int flag);
int       msgsnd(uint_t id, const void * ptr, size_t size, int flag);

#endif
