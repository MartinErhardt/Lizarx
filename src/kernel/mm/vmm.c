/*  <src-path>/src/kernel/mm/vmm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
/*
 * INFO: Here a Lower-Half Kernel is setup
 *
 * INFO: In this File macros from HAL/x86/macros.h and hw_structs from HAL/x86/hw_structs.h are used
 * 
 */
#include <mm/vmm.h>
#include <mm/pmm.h>
#include <dbg/console.h>
#include <mt/proc.h>
#include <mt/threads.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <hw_structs.h>
#include <macros.h>
#include <asm_inline.h>
#include <libOS/lock.h>
#include <intr/irq.h>
#include <../x86_common/local_apic.h>
#include <smp_capabilities.h>
#include <boot/init.h>

extern const void kernel_start;
extern const void kernel_end;

struct to_invalid
{
	uintptr_t virt;
	size_t length;
	uint_t cores_invalidated;
};
#define MAX_STACK_INVLD_SIZ	100
struct to_invalid invalid_stack[MAX_STACK_INVLD_SIZ];
struct to_invalid * invalid_stackptr = &invalid_stack[0];
bool paging_activated=FALSE;
//--------------------------------------------------------static-function-declarations--------------------------------------------------------------
#define ALIGNED_PAGE(PTR) (PTR + PAGE_SIZE-1) & (~(PAGE_SIZE-1))
#define ALIGNED_BASE(BASE) (BASE + 0x1000-1) & (~(0x1000-1))
// maybe move into hal
static void vmm_map(vmm_context* context, uintptr_t phys, uintptr_t virt ,uint8_t flgs);
static uintptr_t virt_to_phys_unsafe(vmm_context* context,uintptr_t virt);

static void vmm_remind_to_mark_free(uint_t page, size_t length);
static void kvmm_free_unsafe(void* to_free,size_t size);
static uintptr_t uvmm_find_freemem(vmm_context * context, size_t pages);
static uintptr_t kvmm_find_freemem(size_t pages);
static void uvmm_push_free(vmm_context* context,uint_t page, size_t length);
static void kvmm_push_free(uint_t page, size_t length);
static vmm_context * get_cur_context();
void vmm_init(struct multiboot_info * mb_info)
{
	struct multiboot_mmap* mmap = (void *)((uintptr_t)(mb_info->mbs_mmap_addr));
	struct multiboot_mmap* mmap_end = (void*)
	    ((uintptr_t) (mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length));
	uintptr_t addr, end_addr,i;
	vmm_lock = LOCK_FREE;
	kprintf("[VMM] I: vmm_init ...");
	/* Speicherkontext anlegen */
	startup_context.highest_paging=NULL;
	startup_context = vmm_crcontext();

	for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++)
		if(pmm_is_alloced_glob(i))
			vmm_map(&startup_context,MUL_PAGE_SIZE(i),MUL_PAGE_SIZE(i),FLGCOMBAT_KERNEL);
	while (mmap < mmap_end) 
	{
		if (mmap->type == 1 && startup_context.mm_stack_ptr<(startup_context.mm_stack+256))
		{
			addr = ALIGNED_BASE(mmap->base);
			if((end_addr = addr +( mmap->length & 0xfffff000))>KERNEL_SPACE)
				end_addr=KERNEL_SPACE;
			startup_context.mm_stack_ptr->start=addr;
			startup_context.mm_stack_ptr->end=end_addr;
			startup_context.mm_stack_ptr++;
		} 
		mmap++;
	}
	SET_CONTEXT((uintptr_t)startup_context.highest_paging)
	ENABLE_PAGING
	paging_activated=1;
	kprintf("SUCCESS\n");
}
/*
 * NOTE: the first table of every sort is allocated in a row of physical and virtual PAGEs - to avoid endless recursion
 */
vmm_context vmm_crcontext()
{
	vmm_context new_context; 
	uint_t i;
	if(startup_context.highest_paging)
	{
		new_context.highest_paging=kvmm_malloc(PAGE_SIZE);
#ifdef ARCH_X86_64
		new_context.other_first_tables[0]=kvmm_malloc(PAGE_SIZE);
		new_context.other_first_tables[1]=startup_context.other_first_tables[1];
		new_context.other_first_tables[2]=startup_context.other_first_tables[2];
#else
		new_context.other_first_tables[0]=kvmm_malloc(PAGE_SIZE);
#endif
		new_context.mm_stack=kvmm_malloc(PAGE_SIZE);
		new_context.mm_stack_ptr=new_context.mm_stack;
	}
	else
	{
		new_context.highest_paging=(void*)startup_tables;
		for(i=1;i<PAGING_HIER_SIZE;i++)
			new_context.other_first_tables[i-1]=(void*)(startup_tables+MUL_PAGE_SIZE(i));
		new_context.mm_stack=(struct stack_entry*)(startup_tables+0x5000);
		new_context.mm_stack_ptr=new_context.mm_stack;
		kprintf("mm_stack at 0x%p",new_context.mm_stack_ptr);
	}

	if( !new_context.highest_paging || !new_context.mm_stack )
	{	
		kprintf("[VMM] E: vmm_crcontext couldn't alloc for context \n");
		while(1);
	}
	struct vmm_paging_entry * cur_struct_to_set = (struct vmm_paging_entry *) new_context.highest_paging;

	memset((void*)new_context.mm_stack,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values
	memset((void*)new_context.highest_paging,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values

	/*
	 * I loop through the paging hierarchy. This is possible, because in X86_64 a page map level 4 entry is similar to a pagedirectorypointertable entry, a pagedirectorytable entry and a pagletable entry.
	 * In X86 a paging directory table entry is similar to a pagetable entry.
	 */
	for(i=0;i<PAGING_HIER_SIZE-1;i++)
	{
		if(startup_context.highest_paging && i>1)
			break;// FIXME not architecture independent
		memset((void*)cur_struct_to_set,0x00000000,PAGE_SIZE);// clear the Paging_tablesto avoid invalid values
		cur_struct_to_set->rw_flags				= FLGCOMBAT_USER;
		cur_struct_to_set->next_paging_layer 			= DIV_PAGE_SIZE(virt_to_phys(get_cur_context(),(uintptr_t)new_context.other_first_tables[i]));
		cur_struct_to_set					= new_context.other_first_tables[i]; // in the first loop rotation cur_struct_to_set is new_context_paging
	}
	if(!startup_context.highest_paging)
		memset((void*)cur_struct_to_set,0x00000000,PAGE_SIZE);

	return new_context;
}
void vmm_delcontext(vmm_context* to_del)
{
	struct vmm_paging_entry * cur_struct_to_set[PAGING_HIER_SIZE] = {(struct vmm_paging_entry *) to_del->highest_paging,
		to_del->other_first_tables[0]
#ifdef ARCH_X86_64
		,to_del->other_first_tables[1],
		to_del->other_first_tables[2]
#endif
	};

	spinlock_ackquire(&vmm_lock);
	vmm_context*context=get_cur_context();
	while((uintptr_t)(cur_struct_to_set[0]++)<((uintptr_t)to_del->highest_paging)+PAGE_SIZE)
		if(cur_struct_to_set[0]->rw_flags&FLG_IN_MEM && cur_struct_to_set[0]->rw_flags&FLG_WRITABLE)
		{
			vmm_map(context, MUL_PAGE_SIZE(cur_struct_to_set[0]->next_paging_layer),TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			cur_struct_to_set[1]=(struct vmm_paging_entry*)TMP_PAGEBUF;
#ifdef ARCH_X86
			cur_struct_to_set[1]=((struct vmm_paging_entry*)TMP_PAGEBUF)+1; // we don't want to remove the kernel space
#endif
			while((uintptr_t)(cur_struct_to_set[1]++)<TMP_PAGEBUF+PAGE_SIZE)
				if(cur_struct_to_set[1]->rw_flags&FLG_IN_MEM && cur_struct_to_set[1]->rw_flags&FLG_WRITABLE)
				{

#ifdef ARCH_X86_64
					vmm_map(context, MUL_PAGE_SIZE(cur_struct_to_set[1]->next_paging_layer),TMP_PAGEBUF2,FLGCOMBAT_KERNEL);
					cur_struct_to_set[2]=((struct vmm_paging_entry*)(TMP_PAGEBUF2))+1; //we don't want to remove kernel space
					while((uintptr_t)(cur_struct_to_set[2]++)<TMP_PAGEBUF2+PAGE_SIZE)
						if(cur_struct_to_set[2]->rw_flags&FLG_IN_MEM && cur_struct_to_set[2]->rw_flags&FLG_WRITABLE)
						{
							vmm_map(context, MUL_PAGE_SIZE(cur_struct_to_set[2]->next_paging_layer),TMP_PAGEBUF3,FLGCOMBAT_KERNEL);
							cur_struct_to_set[3]=((struct vmm_paging_entry*)TMP_PAGEBUF3);
							while((uintptr_t)(cur_struct_to_set[3]++)<TMP_PAGEBUF3+PAGE_SIZE)
								if(cur_struct_to_set[3]->rw_flags&FLG_IN_MEM && cur_struct_to_set[3]->rw_flags&FLG_WRITABLE)
									pmm_free_4k_glob(cur_struct_to_set[3]->next_paging_layer);
							pmm_free_4k_glob(cur_struct_to_set[2]->next_paging_layer);

						}
#endif
					pmm_free_4k_glob(cur_struct_to_set[1]->next_paging_layer);
				}
			pmm_free_4k_glob(cur_struct_to_set[0]->next_paging_layer);
		}
	kprintf("deleted everything");
	kvmm_free_unsafe(to_del->mm_stack, PAGE_SIZE);
	spinlock_release(&vmm_lock);

}
void* kvmm_malloc(size_t size)
{
	return vmm_malloc(&startup_context, size, 1, FLGCOMBAT_KERNEL);
}
void * uvmm_malloc(vmm_context* context,size_t size)
{
	return vmm_malloc(context, size, 0, FLGCOMBAT_USER);
}
void * vmm_malloc(vmm_context * context, size_t size, bool kernel, uint8_t flags)
{ 
	spinlock_ackquire(&vmm_lock);
	int i;
	size = ALIGNED_PAGE(size);
	uintptr_t virt;
	if(kernel)
		virt = kvmm_find_freemem(size);
	else virt = uvmm_find_freemem(context,size);
	if(virt)
		for(i=0;i<size;i+=PAGE_SIZE)
			vmm_map(context,MUL_PAGE_SIZE(pmm_malloc_4k()),virt+i,flags);
	else
	    	kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_find_freemem\n");
	spinlock_release(&vmm_lock);
	return (void *)virt;
}
void vmm_free(vmm_context* context,void* to_free,size_t size)//FIXME No Overflow check
{
	uint_t i;

	size = DIV_PAGE_SIZE(ALIGNED_PAGE(size));

	spinlock_ackquire(&vmm_lock);
	for(i=0;i<size;i+=PAGE_SIZE)
	{
		pmm_free_4k_glob(DIV_PAGE_SIZE( virt_to_phys_unsafe(get_cur_context(), (uintptr_t)to_free+i ) ));
		vmm_map(context,0x0,MUL_PAGE_SIZE(DIV_PAGE_SIZE((uintptr_t)to_free)+i), 0x0);
	}
	uvmm_push_free(context, (uintptr_t)to_free, size);
	spinlock_release(&vmm_lock);
}
static void kvmm_free_unsafe(void* to_free,size_t size)//FIXME No Overflow check
{
	uint_t i;
	size = ALIGNED_PAGE(size);	
	for(i=0;i<size;i+=PAGE_SIZE)
	{
		pmm_free_4k_glob(DIV_PAGE_SIZE( virt_to_phys_unsafe(get_cur_context(), ((uintptr_t)to_free) + i) ) );
		vmm_map(&startup_context,0x0,((uintptr_t)to_free)+i,0x0);
	}
	vmm_remind_to_mark_free((uintptr_t)to_free ,size);
}
/*
 * Write copyin/out for user-kernel data exchange
 */

void* cpyout(void* src,size_t siz)
{
	vmm_context* curcontext	= get_cur_context();
	void* dst 		= uvmm_malloc(curcontext,siz);
	memcpy(dst,src,siz);
	return dst;
}
void* cpyin(void* src,size_t siz)
{
	/*if(!vmm_is_alloced(get_cur_context(),DIV_PAGE_SIZE((uintptr_t)src)) )
	{
		kprintf("[VMM] E: cpyin got invalid src input \n");
		return NULL;
	}*/
	void* dst = kvmm_malloc(siz);
	memcpy(dst,src,siz);
	return dst;
}
uintptr_t vmm_share_to_user(uintptr_t in_kernel,size_t length)
{
	spinlock_ackquire(&vmm_lock);
	vmm_context * cur_context = get_cur_context();
	size_t size = ALIGNED_PAGE(length);
	uintptr_t virt = uvmm_find_freemem(cur_context,size);
	uintptr_t phys = 0;
	uintptr_t i;
	for(i=0;i<MUL_PAGE_SIZE(size);i+=PAGE_SIZE)
	{
		phys = virt_to_phys_unsafe( cur_context, in_kernel + i);
		vmm_map(cur_context,phys,virt+i,FLGCOMBAT_USER);
	}
	spinlock_release(&vmm_lock);
	return virt;
}
//--------------------------------------------------------thread-safe-versions-of-static-functions--------------------------------------------------
void kvmm_free(void* to_free,size_t size)//FIXME No Overflow check
{
	spinlock_ackquire(&vmm_lock);
	kvmm_free_unsafe(to_free, size);
	spinlock_release(&vmm_lock);
}
uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt)
{
	spinlock_ackquire(&vmm_lock);
	uintptr_t phys = virt_to_phys_unsafe(context, virt);
	spinlock_release(&vmm_lock);
	return phys;
}
void vmm_map_range_glob(vmm_context*context,uintptr_t start, uintptr_t end,uint8_t flgs)
{
	spinlock_ackquire(&vmm_lock);
	for(;start<end;start+=PAGE_SIZE)
		vmm_map(context, MUL_PAGE_SIZE(pmm_malloc_4k()),start, flgs);
	spinlock_release(&vmm_lock);
}
void vmm_map_startup_glob(uintptr_t phys, uintptr_t virt, uint8_t flgs)
{
	spinlock_ackquire(&vmm_lock);
	vmm_map(&startup_context,phys,virt,flgs);
	spinlock_release(&vmm_lock);
}
void uvmm_push_free_glob(vmm_context* context,uint_t page, size_t length)
{
	length=ALIGNED_PAGE(length);
	spinlock_ackquire(&vmm_lock);
	uvmm_push_free(context,page,length);
	spinlock_release(&vmm_lock);
}
vmm_context * get_cur_context_glob()
{
	spinlock_ackquire(&vmm_lock);
	vmm_context * cur = get_cur_context();
	spinlock_release(&vmm_lock);
	return cur;
}
//------------------------------------------------------------------static_functions----------------------------------------------------------------
/*
 * These functions get free memory or mark it as free/used
 */
static vmm_context * get_cur_context()
{
	if(startup_context.highest_paging && (!get_cur_cpu() || !get_cur_cpu()->current_thread))
		return &startup_context;
	else if(get_cur_cpu() && get_cur_cpu()->current_thread)
		return get_cur_cpu()->current_thread->proc->context;
	return NULL;
}

static uintptr_t kvmm_find_freemem(size_t pages)
{
	return uvmm_find_freemem(&startup_context,pages);
}
static uintptr_t uvmm_find_freemem(vmm_context * context, size_t pages)
{
	struct stack_entry * cur_stack_entry=context->mm_stack_ptr;
	uint_t pages_free=0;
	uintptr_t to_ret=0x0;
	while ( cur_stack_entry-- >= context->mm_stack && (pages_free=(cur_stack_entry->end-cur_stack_entry->start))<pages);
	if (pages_free == pages)
	{
		to_ret=cur_stack_entry->start;
		if(cur_stack_entry<context->mm_stack_ptr)
			memmove((void*)(cur_stack_entry),(void*)(cur_stack_entry+1),((uintptr_t)context->mm_stack_ptr)-((uintptr_t)cur_stack_entry));
		context->mm_stack_ptr--;
	}
	else if (pages_free>pages)
	{
		to_ret = cur_stack_entry->start;
		cur_stack_entry->start+=pages;
	} else
		kprintf("[VMM] E: kvmm_find_freemem says: stack to short!");
	return to_ret;
}
static void uvmm_push_free(vmm_context* context,uint_t page, size_t length)
{
	struct stack_entry*cur=context->mm_stack;
	while(cur++<=context->mm_stack_ptr) // in this loop we try to merge things up
	{
		if(cur->end==page)
		{
			cur->end=page+length;
			return;
		}
		else if(cur->start==page+length)
		{
			cur->start=page;
			return;
		}
	}
	context->mm_stack_ptr++;
	context->mm_stack_ptr->start=page;
	context->mm_stack_ptr->end=page+length;
}
static void kvmm_push_free(uint_t page, size_t length)
{
	uvmm_push_free(&startup_context, page, length);
}
static void vmm_remind_to_mark_free(uint_t page, size_t length)
{
	spinlock_ackquire(&invld_lock);
	if(invalid_stackptr<&invalid_stack[MAX_STACK_INVLD_SIZ])
	{
		invalid_stackptr			++;
		invalid_stackptr->virt			= page;
		invalid_stackptr->length		= length;
		invalid_stackptr->cores_invalidated	= (1<<(get_cur_cpu()->apic_id) );
	}
	spinlock_release(&invld_lock);
}
/*
 * Quite hardware related paging stuff
 */
static uintptr_t virt_to_phys_unsafe(vmm_context* context,uintptr_t virt)
{
	if(context == NULL)
		return virt;
	// We need 4k alignment 
	if ((virt % PAGE_SIZE)!=0)
	{
		kprintf("[VMM] E: vmm_map has no 4k alignment: 0x%x\n",virt);
		while(1);//return -1;
	}
	uintptr_t page = virt/PAGE_SIZE;
#ifdef ARCH_X86_64
	uint_t paging_indexes[]	= {page/(512*512*512),page%(512*512*512)/(512*512),(page%(512*512))/512, page  % 512};
#endif
#ifdef ARCH_X86
	uint_t paging_indexes[] = {page / 1024, page % 1024};
#endif
	struct vmm_paging_entry * current_paging_entry 	= (struct vmm_paging_entry *) ( ((uintptr_t)(((struct vmm_paging_entry *)context->highest_paging)[ paging_indexes[0] ].next_paging_layer))*PAGE_SIZE );
	struct vmm_paging_entry * next_paging_struct 	= NULL;
	uint_t i 					= 1;
	vmm_context* curcontext 			= get_cur_context();

	/*
	 * I loop through the paging hierarchy. This is possible, because in X86_64 a page map level 4 entry is similar to a pagedirectorypointertable entry, a pagedirectorytable entry and a pagletable entry.
	 * In X86 a paging directory table entry is similar to a pagetable entry.
	 */
	for(i = 1; i <	PAGING_HIER_SIZE; i++)
	{
		if(!current_paging_entry)
			return 0x0;
		vmm_map(curcontext, (uintptr_t)current_paging_entry,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
		current_paging_entry	= ( struct vmm_paging_entry *) TMP_PAGEBUF;
		next_paging_struct 	= ( struct vmm_paging_entry *) (((uintptr_t)(current_paging_entry[ paging_indexes[i] ].next_paging_layer))*PAGE_SIZE);
		current_paging_entry	= next_paging_struct;
	}
	return (uintptr_t)current_paging_entry;
}
void make_guard_page(uintptr_t at, vmm_context * context)
{
	vmm_map(context, 0x0,MUL_PAGE_SIZE(DIV_PAGE_SIZE(at)),0x0);
}
static void vmm_map(vmm_context* context, uintptr_t phys, uintptr_t virt,uint8_t flgs)
{

	// We need 4k alignment 
	if (((virt % PAGE_SIZE)!=0) || ((phys % PAGE_SIZE)!= 0)) 
	{
		kprintf("[VMM] E: vmm_map has no 4k alignment 0x%x 0x%x\n",virt,phys);
		return;
	}

	uint_t page = DIV_PAGE_SIZE(virt);
	vmm_context* curcontext=get_cur_context();
#ifdef ARCH_X86
	uint_t paging_indexes[]				= {page / 1024, page % 1024};
#endif
#ifdef ARCH_X86_64
	uint_t paging_indexes[]				= {page/(512*512*512),page%(512*512*512)/(512*512),(page%(512*512))/512, page  % 512};
#endif
	struct vmm_paging_entry * current_paging_entry 	= &context->highest_paging[0];
	struct vmm_paging_entry * next_paging_struct 	= NULL;
	uint_t i;
	/*
	 * I loop through the paging hierarchy. This is possible, because in X86_64 a page map level 4 entry is similar to a pagedirectorypointertable entry, a pagedirectorytable entry and a pagletable entry.
	 * In X86 a paging directory table entry is similar to a pagetable entry.
	 *
	if((uintptr_t)current_paging_entry==0x16fd00f && paging_activated){
		kprintf("startup_context: 0x%p",startup_context.highest_paging);
		kprintf("virt: 0x%x phys: 0x%x",virt,phys);
		kprintf("that's crazy: 0x%p",current_paging_entry);
		while(1);};*/
	for(i=0; i< ( PAGING_HIER_SIZE - 1 ); i++)
	{

		/*
		 * NOTE: the first table of every sort is allocated in a row of physical and virtual PAGEs - to avoid endless recursion
		 */
		/*
		* -----------------------------------------------------find-or-create-pagedirectorytable---------------------------------------------------------------
		* (in the first case the pagedirectorytable is allready existing)
		*/

		if (current_paging_entry[paging_indexes[i]].rw_flags& FLG_IN_MEM) 
		{
			/*
			* first we set next_paging_struct to it's physical value than we change it to virtual one by mapping it to TMP_PAGEBUF.
			* If pd_index == 0 we know it's the page following to the PD. 
			* We mustn't map it to TMP_PAGEBUF to prevent endless recursion when mapping to TMP_PAGEBUF, which is > 0x400000
			*/
			next_paging_struct = (void*)((uintptr_t)(current_paging_entry[paging_indexes[i]].next_paging_layer)*PAGE_SIZE);
			if((virt==TMP_PAGEBUF)&&(paging_activated))
				next_paging_struct = context->other_first_tables[i];
			else if( (paging_activated))
			{
				vmm_map(curcontext,(uintptr_t)next_paging_struct,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
				next_paging_struct =(struct vmm_paging_entry *)TMP_PAGEBUF;
			}

		}
		else 
		{
			/* 
			* setup new pagedirectorytable 
			* if that's the first next_paging_struct it's the next page from PD
			*/
			
			next_paging_struct = (struct vmm_paging_entry*)(pmm_malloc_4k()*PAGE_SIZE);// alloc physical memory
			current_paging_entry[paging_indexes[i]].rw_flags = FLGCOMBAT_USER;
			current_paging_entry[paging_indexes[i]].next_paging_layer = DIV_PAGE_SIZE((uintptr_t)next_paging_struct);
			if(paging_activated)
			{// if paging is activated TMP_PAGEBUF will be allways mapped
				vmm_map(curcontext,(uintptr_t)next_paging_struct,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
				next_paging_struct = (struct vmm_paging_entry*)TMP_PAGEBUF;
			}

			memset(next_paging_struct, 0x00000000, PAGE_SIZE);
		}
		current_paging_entry = next_paging_struct;
	}
	/*if(apic_ready==1)
	{
		to_flush=virt;
		local_apic_ipi_all_excluding_self(IPI_DELIVERY_MODE_FIXED, 28, 0x0);
	}*/	

	INVALIDATE_TLB(virt)
	uint_t index					= paging_indexes[PAGING_HIER_SIZE - 1];
	current_paging_entry[index].rw_flags 		= flgs;
	current_paging_entry[index].next_paging_layer	= DIV_PAGE_SIZE(phys);
}
void sync_addr_space()
{
	spinlock_ackquire(&invld_lock);
	struct to_invalid * to_invalid_cur = invalid_stackptr;
	uint8_t apic_id = get_cur_cpu()->apic_id;
	while(to_invalid_cur>&invalid_stack[0])
	{
		INVALIDATE_TLB(to_invalid_cur->virt)
		to_invalid_cur->cores_invalidated |= (1<<apic_id);
		if(to_invalid_cur->cores_invalidated == all_cores_mask && to_invalid_cur == invalid_stackptr)
		{
			kprintf("free: 0x%x", to_invalid_cur->virt);
			kvmm_push_free(to_invalid_cur->virt, to_invalid_cur->length);
			invalid_stackptr--;
		}
		to_invalid_cur--;
	}
	spinlock_release(&invld_lock);
}
