/*   <src-path>/src/kernel/mm/vheap.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<mm/pmm.h>
#include<mm/vmm.h>
#include<dbg/console.h>
#include<stdint.h>
#include<stdbool.h>
#include<dbg/console.h>
#include<libOS/lock.h>
#include<mm/vheap.h>

static struct heap_block* first = NULL;
static void vheap_enlarge(size_t size);

struct heap_block
{
	struct heap_block* next;
	uint8_t free;
	size_t size;
};
void vheap_init()
{
	kprintf("[VHEAP] I: vheap_init ... ");
	first		= (struct heap_block*)kvmm_malloc(PAGE_SIZE);
	first->size	= PAGE_SIZE;
	first->next	= NULL;
	first->free	= TRUE;
	kprintf("SUCCESS\n");
}
void* kmalloc(size_t size)
{
	struct heap_block* cur;
	spinlock_ackquire(&heap_lock);
	if(size>=PAGE_SIZE-sizeof(struct heap_block))
		vheap_enlarge(size);
retry:
	
	cur	= first;
	while(cur != NULL)
	{
		if(cur->size-sizeof(struct heap_block)>=size && cur->free )
		{
			if(cur->size-size>sizeof(struct heap_block))
			{
				struct heap_block * free=(struct heap_block *)( ((uintptr_t)cur) + ( cur->size-(size+sizeof(struct heap_block)) ) );
				
				free->free = 0;
				free->next = first;
				free->size = size+sizeof(struct heap_block);
				
				first      = free;
				
				cur->size -= (size+sizeof(struct heap_block));
				spinlock_release(&heap_lock);
				return (void*)(((uintptr_t)free)+sizeof(struct heap_block));
			} 
			else
			{
				cur->free = 0;
				spinlock_release(&heap_lock);
				return (void*)(((uintptr_t)cur) + sizeof(struct heap_block));
			}
		}
		else{
			cur = cur->next; }
	}
	kprintf("[VHEAP]  E: kmalloc cannnot find free_space \n");
	spinlock_release(&heap_lock);
	vheap_enlarge(PAGE_SIZE);
	goto retry;
	return NULL;
}
void* kcalloc(size_t size)
{
	void* returnval = kmalloc(size);
	memset(returnval,0x00000000,size);
	return returnval;
}
void kfree(void* ptr)
{
	spinlock_ackquire(&heap_lock);
	struct heap_block* cur =first;
	while(cur!=NULL)
	{
		if( (uintptr_t)cur+sizeof(struct heap_block)==(uintptr_t)ptr && !cur->free )
		{
			cur->free = 1;
			spinlock_release(&heap_lock);
			return;
		}
		else
			cur=cur->next;
	}
	spinlock_release(&heap_lock);
	return;
}
void* krealloc()
{
	return NULL;
}
static void vheap_enlarge(size_t size)
{
	size_t size_we_have_to_enlarge 	= MUL_PAGE_SIZE( DIV_PAGE_SIZE(size) +1 );
	kprintf("[VHEAP] I: vheap_enlarge is enlarging heap by 0x%x\n", size_we_have_to_enlarge);
	struct heap_block *new_heap_sp	= (struct heap_block *)kvmm_malloc( size_we_have_to_enlarge );
	new_heap_sp->next		= first;
	new_heap_sp->free		= 1;
	new_heap_sp->size		= size_we_have_to_enlarge;
	
	first				= new_heap_sp;
}
