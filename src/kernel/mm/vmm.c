/*   <src-path>/src/kernel/mm/vmm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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

struct vmm_nodepkg_ptr
{
    uintptr_t nodepkg_ptr :20;
}__attribute__((packed));

struct vmm_tree_master
{
    uint32_t used[32];
    struct vmm_nodepkg_ptr nodepkg[32];
}__attribute__((packed));

struct vmm_tree_nodepkg
{
    uint32_t nodepkgentr[1024];
}__attribute__((packed));

struct to_invalid
{
	uintptr_t virt;
	uint_t cores_invalidated;
};

bool paging_activated=FALSE;
//--------------------------------------------------------static-function-declarations--------------------------------------------------------------

static void vmm_mark_used(vmm_context*first,uint_t page);
static void vmm_mark_used_inallcon(uint_t page);

static void vmm_mark_free(vmm_context* context,uint_t page);
static void vmm_mark_free_inallcon(uint_t page);
static void vmm_remind_to_mark_free(uint_t page);

static void vmm_map_kernel(vmm_context* context);
static void vmm_map(vmm_context* context, uintptr_t phys, uintptr_t virt ,uint8_t flgs);
static void vmm_map_inallcon(uintptr_t phys, uintptr_t virt, uint8_t flgs);

//static void* kvmm_malloc_coherent(size_t size);
static bool vmm_is_alloced(vmm_context* context,uint_t page);
static uintptr_t virt_to_phys_unsafe(vmm_context* context,uintptr_t virt);
static uintptr_t vmm_find_freemem(vmm_context* context,size_t size, uintptr_t from,uintptr_t to);

static void kvmm_free_unsafe(void* to_free,size_t size);

static size_t from_bytes_to_pages(size_t bytes);
static vmm_context * get_cur_context();
#define MAX_STACK_INVLD_SIZ	100
struct to_invalid invalid_stack[MAX_STACK_INVLD_SIZ];
struct to_invalid * invalid_stackptr = &invalid_stack[0];

vmm_context vmm_init(void)
{
	vmm_lock = LOCK_FREE;
	kprintf("[VMM] I: vmm_init ...");
	/* Speicherkontext anlegen */
	startup_context = vmm_crcontext();

	SET_CONTEXT((uintptr_t)startup_context.highest_paging)

	ENABLE_PAGING

	paging_activated=TRUE;
	kprintf("SUCCESS\n");
	return startup_context;
}

/*
 * NOTE: the first table of every sort is allocated in a row of physical and virtual PAGEs - to avoid endless recursion
 */
vmm_context vmm_crcontext()
{
	vmm_context new_context = 
	{
		.highest_paging		= kvmm_malloc(PAGE_SIZE),
#ifdef ARCH_X86_64
		.other_first_tables	= {kvmm_malloc(PAGE_SIZE), kvmm_malloc(PAGE_SIZE), kvmm_malloc(PAGE_SIZE)},
#endif
#ifdef ARCH_X86
		.other_first_tables	= {kvmm_malloc(PAGE_SIZE)},
#endif
		.mm_tree		= kvmm_malloc(PAGE_SIZE),
	};
	if( !new_context.highest_paging || !new_context.mm_tree )
	{	
		kprintf("[VMM] E: vmm_crcontext couldn't alloc for context \n");
		while(1);
	}
	uint_t i;
	struct vmm_paging_entry * cur_struct_to_set = (struct vmm_paging_entry *) new_context.highest_paging;

	memset((void*)new_context.mm_tree,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values
	memset((void*)new_context.highest_paging,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values

	/*
	 * I loop through the paging hierarchy. This is possible, because in X86_64 a page map level 4 entry is similar to a pagedirectorypointertable entry, a pagedirectorytable entry and a pagletable entry.
	 * In X86 a paging directory table entry is similar to a pagetable entry.
	 */
	for(i=0;i<PAGING_HIER_SIZE-1;i++)
	{
		memset((void*)cur_struct_to_set,0x00000000,PAGE_SIZE);// clear the Paging_tablesto avoid invalid values
		cur_struct_to_set->rw_flags				= FLGCOMBAT_USER;
		cur_struct_to_set->next_paging_layer 			= DIV_PAGE_SIZE(virt_to_phys(get_cur_context(),(uintptr_t)new_context.other_first_tables[i]));
		cur_struct_to_set					= new_context.other_first_tables[i]; // in the first loop rotation cur_struct_to_set is new_context_paging
	}
	memset((void*)cur_struct_to_set,0x00000000,PAGE_SIZE);
	vmm_map_kernel(&new_context);
	return new_context;
}
void vmm_delcontext(vmm_context* to_del)
{
	int i, j;
	struct vmm_paging_entry * cur_struct_to_set = (struct vmm_paging_entry *) to_del->highest_paging;
	for(i =0;i<32;i++)
		if(to_del->mm_tree->nodepkg[i].nodepkg_ptr)
			kvmm_free_unsafe((void *)( (uintptr_t) MUL_PAGE_SIZE(to_del->mm_tree->nodepkg[i].nodepkg_ptr)), PAGE_SIZE);
	spinlock_ackquire(&pmm_lock);
	pmm_free_4k_unsafe(DIV_PAGE_SIZE(virt_to_phys_unsafe(get_cur_context(), (uintptr_t)cur_struct_to_set)));
	for(i = 0;i<PAGING_HIER_SIZE-1;i++)
	{
		for(j=0;j<PAGING_TABLE_ENTRY_N;j++)
			if(cur_struct_to_set[j].rw_flags & FLG_IN_MEM)
				pmm_free_4k_unsafe(cur_struct_to_set[j].next_paging_layer); //TODO free lower paging tables, this deleted paging table could refer to
		vmm_map_inallcon(0x0,(uintptr_t)cur_struct_to_set,0x0);// in the first loop rotation cur_struct_to_set is new_context_paging
		vmm_remind_to_mark_free(DIV_PAGE_SIZE((uintptr_t)cur_struct_to_set) );
		cur_struct_to_set = to_del->other_first_tables[i];
	}
	spinlock_release(&pmm_lock);
	//kvmm_free_unsafe((void *)cur_struct_to_set, PAGE_SIZE);// in the first loop rotation cur_struct_to_set is new_context_paging
}
vmm_context * get_cur_context_glob()
{
	spinlock_ackquire(&vmm_lock);
	//spinlock_ackquire(&multi_threading_lock);
	vmm_context * cur = get_cur_context();
	spinlock_release(&vmm_lock);
	//spinlock_release(&multi_threading_lock);
	return cur;
}
static vmm_context * get_cur_context()
{
	if(startup_context.highest_paging != NULL && (!get_cur_cpu() || get_cur_cpu()->current_thread==NULL))
		return &startup_context;
	else if(get_cur_cpu() && get_cur_cpu()->current_thread!=NULL)
		return get_cur_cpu()->current_thread->proc->context;
	return NULL;
}
/*
 * This function uses coherent physical memory. I only use that vmm_context.
 
static void* kvmm_malloc_coherent(size_t size)
{
	spinlock_ackquire(&vmm_lock);
	int i;
	
	size = from_bytes_to_pages(size);
	
	uintptr_t phys = MUL_PAGE_SIZE( pmm_malloc(size));
	
	if(!paging_activated)
	{
		spinlock_release(&vmm_lock);
		return (void*)phys;
	}
	uintptr_t virt = vmm_find_freemem(&startup_context,size,0x00000000,KERNEL_SPACE);
	
	if(virt)

		for(i=0;i < MUL_PAGE_SIZE(size);i+=PAGE_SIZE)
		{
			if(vmm_map_inallcon(phys+i,virt+i,FLGCOMBAT_KERNEL)<0)
				kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_map_page\n");
			vmm_mark_used_inallcon( (DIV_PAGE_SIZE(virt+i)) );
		}
	else
	    	kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_find_freemem\n");
	spinlock_release(&vmm_lock);
	return (void*) virt;
}*/
void* kvmm_malloc(size_t size)
{
	if(!paging_activated)
		return (void*) MUL_PAGE_SIZE(pmm_malloc_4k());
	return vmm_malloc(NULL, size, 0x0, KERNEL_SPACE, FLGCOMBAT_KERNEL);
}
void * uvmm_malloc(vmm_context* context,size_t size)
{
	return vmm_malloc(context, size, KERNEL_SPACE, 0xffffffff, FLGCOMBAT_USER);
}
void * vmm_malloc(vmm_context * context, size_t size, uintptr_t from, uintptr_t to, uint8_t flags)
{
	spinlock_ackquire(&vmm_lock);
	int i;

	size = from_bytes_to_pages(size);

	uintptr_t phys;
	uintptr_t virt;
	if(!context)
		virt = vmm_find_freemem(get_cur_context(),size,from,to);
	else virt = vmm_find_freemem(context,size,from,to);
	if(virt)
		for(i=0;i<MUL_PAGE_SIZE(size);i+=PAGE_SIZE)
		{
			phys = MUL_PAGE_SIZE( pmm_malloc_4k() );
			if(context)
			{
				vmm_map(context,phys,virt+i,flags);
				vmm_mark_used( context, (DIV_PAGE_SIZE(virt+i)) );
			}
			else
			{
				vmm_map_inallcon(phys,virt+i,flags);
				vmm_mark_used_inallcon( (DIV_PAGE_SIZE(virt+i)) );
			}
		}
	else
	    	kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_find_freemem\n");
	
	spinlock_release(&vmm_lock);
	return (void *)virt;
}

int_t vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs)
{
	int j;
	uintptr_t page = DIV_PAGE_SIZE((uintptr_t)ptr);
	spinlock_ackquire(&vmm_lock);

	size = from_bytes_to_pages(size);

	uintptr_t phys = 0x0;
	for(j=0;j<size;j++) 
		if (vmm_is_alloced(context,page+j)==TRUE) 
		{
			spinlock_release(&vmm_lock);
			kprintf("[VMM] E: vmm_realloc says: page 0x%x000 alloced\n",page +j);
			return -1;
		}
	for(j =0;j<size;j++)
	{
		phys = MUL_PAGE_SIZE( pmm_malloc_4k() );
		vmm_map(context,phys,MUL_PAGE_SIZE(page+j),flgs);
		vmm_mark_used(context,page+j);
	}
	spinlock_release(&vmm_lock);
	return 0;
}
void vmm_free(vmm_context* context,void* to_free,size_t size)//FIXME No Overflow check
{
	uint_t i;

	size = from_bytes_to_pages(size);

	spinlock_ackquire(&vmm_lock);
	for(i=0;i<size;i++)
	{
		pmm_free_4k_glob(DIV_PAGE_SIZE( virt_to_phys_unsafe(get_cur_context(), MUL_PAGE_SIZE( DIV_PAGE_SIZE((uintptr_t)to_free)+i) ) ));
		vmm_map(context,0x0,MUL_PAGE_SIZE(DIV_PAGE_SIZE((uintptr_t)to_free)+i), 0x0);
		vmm_mark_free(context,DIV_PAGE_SIZE((uintptr_t)to_free)+i);
	}
	spinlock_release(&vmm_lock);
}
void kvmm_free(void* to_free,size_t size)//FIXME No Overflow check
{
	spinlock_ackquire(&vmm_lock);
	kvmm_free_unsafe(to_free, size);
	spinlock_release(&vmm_lock);
}
static void kvmm_free_unsafe(void* to_free,size_t size)//FIXME No Overflow check
{
	uint_t i;
	size = from_bytes_to_pages(size);
	for(i=0;i<size;i++)
	{
		pmm_free_4k_glob(DIV_PAGE_SIZE( virt_to_phys_unsafe(get_cur_context(), MUL_PAGE_SIZE(  DIV_PAGE_SIZE((uintptr_t)to_free)  + i) ) ));
		vmm_remind_to_mark_free(DIV_PAGE_SIZE( (uintptr_t)to_free ) + i );
		vmm_map_inallcon(0x0,MUL_PAGE_SIZE(DIV_PAGE_SIZE((uintptr_t)to_free)+i),0x0);
	}
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
	if(!vmm_is_alloced(get_cur_context(),DIV_PAGE_SIZE((uintptr_t)src)) ) // sanity checks 
	{
		kprintf("[VMM] E: cpyin got invalid src input \n");
		return NULL;
	}
	void* dst = kvmm_malloc(siz);
	memcpy(dst,src,siz);
	return dst;
}
uintptr_t vmm_share_to_user(uintptr_t in_kernel,size_t length)
{
	spinlock_ackquire(&vmm_lock);
	vmm_context * cur_context = get_cur_context();
	size_t size = from_bytes_to_pages(length);
	uintptr_t virt = vmm_find_freemem(cur_context,size,KERNEL_SPACE,0xffffffff);
	uintptr_t phys = 0;
	uintptr_t i;
	for(i=0;i<MUL_PAGE_SIZE(size);i+=PAGE_SIZE)
	{
		phys = virt_to_phys_unsafe( cur_context, in_kernel + i);
		vmm_map(cur_context,phys,virt+i,FLGCOMBAT_USER);
		vmm_mark_used( cur_context, (DIV_PAGE_SIZE(virt+i)) );
	}
	spinlock_release(&vmm_lock);
	return virt;
}
//--------------------------------------------------------thread-safe-versions-of-static-functions--------------------------------------------------
uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt)
{
	spinlock_ackquire(&vmm_lock);
	uintptr_t phys = virt_to_phys_unsafe(context, virt);
	spinlock_release(&vmm_lock);
	return phys;
}
void vmm_mark_used_inallcon_glob(uint_t page)
{
	spinlock_ackquire(&vmm_lock);
	vmm_mark_used_inallcon(page);
	spinlock_release(&vmm_lock);
}
uintptr_t vmm_find_freemem_glob(vmm_context* context,size_t size, uintptr_t from,uintptr_t to)
{
	spinlock_ackquire(&vmm_lock);
	uintptr_t found = vmm_find_freemem(context,size,from,to);
	spinlock_release(&vmm_lock);
	return found;
}
bool vmm_is_alloced_glob(vmm_context* context,uint_t page)//FIXME No Overflow check
{
	spinlock_ackquire(&vmm_lock);
	bool is_alloced = vmm_is_alloced(context,page);
	spinlock_release(&vmm_lock);
	return is_alloced;
}
void vmm_map_inallcon_glob(uintptr_t phys, uintptr_t virt, uint8_t flgs)
{
	spinlock_ackquire(&vmm_lock);
	vmm_map_inallcon(phys,virt,flgs);
	spinlock_release(&vmm_lock);
}
//------------------------------------------------------------------static_functions----------------------------------------------------------------
static size_t from_bytes_to_pages(size_t bytes)
{
	if((bytes%PAGE_SIZE)==0)
		return DIV_PAGE_SIZE(bytes);
	else
		return DIV_PAGE_SIZE(bytes)+1;
}
/*
 * These functions get free memory or mark it as free/used
 */
static uintptr_t vmm_find_freemem(vmm_context* context,size_t size, uintptr_t from,uintptr_t to)
{
	uintptr_t virt=0x00000000;// avoid unitialized Pointer Bug
	int i,j;
	for (i = from/PAGE_SIZE; i < to/PAGE_SIZE-(size-1); i++) {// find and map free 
mark:

		for(j=0;j<size;j++)
		{
			if(vmm_is_alloced((void*)((uintptr_t)(context)),i+j)==TRUE)
			{
				/*
				if(i%1024==1023){
				context->tr->used[i/32768]&=(1<<((i/1024)%32));
				}*/
				i++;
				goto mark;
			} 
			if(j==size-1)
			{
				virt = i*PAGE_SIZE;//kprintf("mem %x %x",
				//virt,(uintptr_t)context);
				return virt;
			}
		}
	}
	// nothing was found
	kprintf("[VMM] E: vmm_find_freemem couldn't get free space in addressspace\n");
	return 0x0;
}
static bool vmm_is_alloced(vmm_context* context,uint_t page)//FIXME No Overflow check
{
	uint_t master_ind		= page>>10;		// = /1024
	uint_t node_ind			= page&0x3ff;		// = %1024
	uint_t inner_nodepkgoff		= (master_ind&0x1f)<<10;// = (master_ind%32)*1024;
	struct vmm_tree_master*tree	= context->mm_tree;
	struct vmm_tree_nodepkg* nodes	= (void*)((uintptr_t)tree->nodepkg[master_ind>>5].nodepkg_ptr*PAGE_SIZE);
	if(!nodes)
		return FALSE;
	/*if(tree->used[master_ind])
	{
	    kprintf("0x%x is alloced1 used at 0x%x",page*PAGE_SIZE,(uintptr_t)&tree->used[master_ind]);
	    return TRUE;
	}*/
	if(nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]&(1<<((node_ind+inner_nodepkgoff)%32)))
		return TRUE;
	else
		return FALSE;
}

static void vmm_mark_used(vmm_context* context,uint_t page)
{
	
	uint_t master_ind		= page>>10;		// = /1024
	uint_t node_ind			= page&0x3ff;		// = %1024
	uint_t inner_nodepkgoff		= (master_ind&0x1f)<<10;// = (master_ind%32)*1024;
	struct vmm_tree_master*tree	= context->mm_tree;
	struct vmm_tree_nodepkg* nodes	= (void*)((uintptr_t)tree->nodepkg[master_ind>>5].nodepkg_ptr*PAGE_SIZE);
	/*
	if(tree->used[master_ind])
	{
	    return 0;
	}*/
	if(!nodes)
	{
		uintptr_t node_phys = pmm_malloc_4k()*PAGE_SIZE;
		vmm_map(get_cur_context(),node_phys,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
		memset((void*)TMP_PAGEBUF,0x00000000,PAGE_SIZE);
		
		nodes=(struct vmm_tree_nodepkg* )TMP_PAGEBUF;
		nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]|=(1<<((node_ind+inner_nodepkgoff)%32));
		context->mm_tree->nodepkg[master_ind/32].nodepkg_ptr	= TMP_PAGEBUF;
		
		uintptr_t node_virt = vmm_find_freemem(get_cur_context(),1,0x0,DIV_PAGE_SIZE(KERNEL_SPACE));
		context->mm_tree->nodepkg[master_ind/32].nodepkg_ptr	= DIV_PAGE_SIZE(node_virt);
		vmm_mark_used_inallcon(context->mm_tree->nodepkg[master_ind/32].nodepkg_ptr);
		vmm_map_inallcon(node_phys,node_virt,FLGCOMBAT_KERNEL);
		return;
	}
	nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]|=(1<<((node_ind+inner_nodepkgoff)%32));
	//kprintf("hi2");
}
static void vmm_mark_used_inallcon(uint_t page)
{
	struct proc* cur =alist_get_by_index(&proc_list,0);
	int i=0;
	while(cur)
	{
		vmm_mark_used(cur->context,page);
		cur =alist_get_by_index(&proc_list,i);
		i++;
	}
	if(startup_context.highest_paging)
		vmm_mark_used(&startup_context,page);
}
static void vmm_mark_free(vmm_context* context,uint_t page)
{
	uint_t master_ind		= page>>10;		// = /1024
	uint_t node_ind			= page&0x3ff;		// =%1024
	uint_t inner_nodepkgoff		= (master_ind&0x1f)<<10;// =(master_ind%32)*1024;
	struct vmm_tree_master*tree	= context->mm_tree;
	struct vmm_tree_nodepkg* nodes	= (void*)((uintptr_t)tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
	/*
	if(tree->used[master_ind])
	{
	    return 0;
	}*/
	if(!nodes)
		return;
	nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]&= ~(1<<((node_ind+inner_nodepkgoff)%32));
}
static void vmm_remind_to_mark_free(uint_t page)
{
	spinlock_ackquire(&invld_lock);
	if(invalid_stackptr<&invalid_stack[MAX_STACK_INVLD_SIZ])
	{
		invalid_stackptr			++;
		invalid_stackptr->virt			= MUL_PAGE_SIZE(page);
		invalid_stackptr->cores_invalidated	= (1<<(get_cur_cpu()->apic_id) );
	}
	spinlock_release(&invld_lock);

}
static void vmm_mark_free_inallcon(uint_t page)
{
	struct proc* cur =alist_get_by_index(&proc_list,0);
	int i=0;
	while(cur)
	{
		vmm_mark_free(cur->context,page);
		cur =alist_get_by_index(&proc_list,i);
		i++;
	}
	if(startup_context.highest_paging)
		vmm_mark_free(&startup_context,page);
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
static void vmm_map_kernel(vmm_context* context)
{
	int i				= 0;
	uintptr_t node_phys		= 0x0;
	uintptr_t phys 			= 0x0;
	vmm_context* curcontext = get_cur_context();
	if(paging_activated)
	{
		spinlock_ackquire(&vmm_lock);

		for(i=0;i<32;i++)
		{
			context->mm_tree->nodepkg[i].nodepkg_ptr	= 0;
			//kprintf("tree%d node virt = 0x%x",i,node_virt);
		}
		for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++)
			if(vmm_is_alloced(curcontext,i))
			{
				phys=virt_to_phys_unsafe(curcontext,MUL_PAGE_SIZE(i));
				//kprintf("map 0x%x to 0x%x",phys,i*PAGE_SIZE);
				vmm_map(context,phys,i*PAGE_SIZE,FLGCOMBAT_KERNEL);
				vmm_mark_used(context,i);
			}
		spinlock_release(&vmm_lock);
	}
	else
	{
		for(i=0;i<32;i++)
		{
			node_phys = pmm_malloc_4k();
			context->mm_tree->nodepkg[i].nodepkg_ptr=node_phys;
			memset((void*)(node_phys*PAGE_SIZE),0x00000000,PAGE_SIZE);
		}
		for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++)
			if(pmm_is_alloced_glob(i))
			{
				vmm_map(context,MUL_PAGE_SIZE(i),MUL_PAGE_SIZE(i),FLGCOMBAT_KERNEL);
				vmm_mark_used(context,i);
			}
	}
}
static void vmm_map(vmm_context* context, uintptr_t phys, uintptr_t virt,uint8_t flgs)
{

	// We need 4k alignment 
	if (((virt % PAGE_SIZE)!=0) || ((phys % PAGE_SIZE)!= 0)) 
	{
		kprintf("[VMM] E: vmm_map has no 4k alignment\n",virt,phys);
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
	uint_t i=0;
	/*
	 * I loop through the paging hierarchy. This is possible, because in X86_64 a page map level 4 entry is similar to a pagedirectorypointertable entry, a pagedirectorytable entry and a pagletable entry.
	 * In X86 a paging directory table entry is similar to a pagetable entry.
	 */

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
		//	kprintf("free: 0x%x", to_invalid_cur->virt);
			vmm_mark_free_inallcon(DIV_PAGE_SIZE(to_invalid_cur->virt) );
			invalid_stackptr--;
		}
		//kprintf("cores 0x%x",invalid_stackptr->cores_invalidated);
		to_invalid_cur--;
	}
	spinlock_release(&invld_lock);
}
static void vmm_map_inallcon(uintptr_t phys, uintptr_t virt, uint8_t flgs)
{
	struct proc* cur =alist_get_by_index(&proc_list,0);
	int i=0;
	while(cur)
	{
		vmm_map(cur->context, phys,virt,flgs);
		cur =alist_get_by_index(&proc_list,i);
		i++;
	}
	if(startup_context.highest_paging)
		vmm_map(&startup_context,phys,virt,flgs);
}
