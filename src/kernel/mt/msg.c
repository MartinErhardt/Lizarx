/*   <src-path>/src/kernel/mt/inc/msg.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<stdint.h>
#include<mt/msg.h>
#include<libOS/list.h>
#include<mm/vheap.h>
#include<dbg/console.h>

int       msgctl(int msqid, int cmd, struct msqid_ds * buf)
{
	return -1;
}
uint_t       msgget(key_t key, int flag)
{
	spinlock_ackquire(&msq_lock);
	struct msqid_ds * buf 	= kmalloc(sizeof(struct msqid_ds));
	alist_add(&msqid_list, buf);
	buf->id 		= alist_get_index(&msqid_list, buf);
	buf->first_message	= NULL;
	spinlock_release(&msq_lock);
	return buf->id;
}
ssize_t   msgrcv(uint_t msqid, void * ptr, size_t size, long type, int flag)
{
	spinlock_ackquire(&msq_lock);
	struct msqid_ds * msqid_ = alist_get_by_entry(&msqid_list, 0,msqid);
	if(!msqid_)
	{
		spinlock_release(&msq_lock);
		return -1;
	}
	
	uint_t* message_block = msqid_->first_message;
	if(size > *(message_block+1))
	{
		spinlock_release(&msq_lock);
		return -1;
	}
	memcpy(ptr, (void *)(message_block+2), size);
	msqid_->first_message = (uint_t *) (*message_block);
	kfree((void*)message_block);
	spinlock_release(&msq_lock);
	return 0;
}
int       msgsnd(uint_t msqid, const void * ptr, size_t size, int flag) // FIXME check if it is mapped
{
	spinlock_ackquire(&msq_lock);
	struct msqid_ds * msqid_ = alist_get_by_entry(&msqid_list, 0,msqid);
	if(!msqid_)
	{
		spinlock_release(&msq_lock);
		return -1;
	}
	uint_t * cur_msqid_msg = msqid_->first_message;
	uint_t* message_block = kmalloc(size + (2*sizeof(size_t)));
	*(message_block+1) = size;
	*message_block = 0;
	memcpy((void *)(message_block+2), ptr, size);
	if(!cur_msqid_msg)
	{
		msqid_->first_message = message_block;
		spinlock_release(&msq_lock);
		return 0;
	}
	while(*cur_msqid_msg)
	{
		cur_msqid_msg = (uint_t *) *cur_msqid_msg;
	}
	*cur_msqid_msg = (uintptr_t) message_block;
	
	spinlock_release(&msq_lock);
	return 0;
}