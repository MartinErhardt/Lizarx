/*   <src-path>/src/kernel/mm/vheap.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<mm/pmm.h>
#include<mm/vmm.h>
#include<dbg/console.h>
#include<stdint.h>
#include<stdbool.h>

static struct heap_block* first=NULL;
static void vheap_enlarge(size_t size);
struct heap_block{
     struct heap_block* next;
     struct heap_block* prev;
     uint8_t free;
     size_t size;
};
void vheap_init(){
    first= (struct heap_block*)kvmm_malloc(PAGE_SIZE);
    first->size=PAGE_SIZE;
    first->next=first;
    first->prev=first;
    first->free=TRUE;
}
void* kmalloc(size_t size){
    struct heap_block* cur =first ;
    if(size>=PAGE_SIZE){
	vheap_enlarge(size);
    }
    do{
	if((cur->size-sizeof(struct heap_block)>=size)&&(cur->free)){
		
		if(cur->size-size>sizeof(struct heap_block))
		{
			struct heap_block *free=(struct heap_block *)((uintptr_t)(cur)+cur->size-(size+sizeof(struct heap_block)));
			
			free->free=0;
			free->prev=cur;
			free->next=cur->next;
			free->size=size+sizeof(struct heap_block);
			
			cur->next=free;
			cur->next->prev=free;
			cur->size-=(size+sizeof(struct heap_block));
			
			return (void*)((uintptr_t)(free)+sizeof(struct heap_block));
		}
		else
		{
			cur->free=0;
			return (void*)((uintptr_t)cur + sizeof(struct heap_block));
		}
	}
	else
	{
		cur=cur->next;
	}
    }while(cur!=first);
    return NULL;
}
void kfree(void*ptr){
    struct heap_block* cur =first ;
    do{
	if(((uintptr_t)cur+sizeof(struct heap_block)==(uintptr_t)ptr)&&(!cur->free))
	{
		cur->free=1;
	}
	else
	{
		cur=cur->next;
	}
    }while(cur!=first);
    
    return;
}
void* krealloc(){
    return NULL;
}
static void vheap_enlarge(size_t size){
    //kprintf("hi");
    struct heap_block *new_heap_sp=(struct heap_block *)kvmm_malloc(size+PAGE_SIZE);
    
    size=size/PAGE_SIZE+1;
    
    first->prev->next=new_heap_sp;
    first->prev=new_heap_sp;
    
    new_heap_sp->next=first;
    new_heap_sp->prev=first->prev;
    new_heap_sp->free=1;
    new_heap_sp->size=size*PAGE_SIZE;
}