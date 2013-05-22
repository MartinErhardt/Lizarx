/*   <src-path>/src/kernel/mt/proc.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<mt/proc.h>
#include<mm/vheap.h>
#include<mt/user.h>
#include<mt/threads.h>

static int32_t num_proc=0;

struct proc* create_proc()
{
	vmm_context *new_con = kmalloc(sizeof(vmm_context));
	struct proc* new_proc= kmalloc(sizeof(struct proc));
	struct user* new_user= kmalloc(sizeof(struct user));
	
	num_proc++;
	*new_con =vmm_crcontext();
	new_user->u_id=0;
	
	new_proc->context=new_con;
	new_proc->user=new_user;
	new_proc->p_id=num_proc;
	new_proc->next=first_proc;
	new_proc->clock=0x0;
	first_proc = new_proc;
	
	return new_proc;
}
struct proc* get_proc(uint32_t p_id)
{
	struct proc* cur=first_proc;
	
	do
	{
		if(cur->p_id==p_id)
		{
			return cur;
		}
		else
		{
			cur = cur->next;
		}
	}
	while(cur!=NULL);
	return NULL;
}
