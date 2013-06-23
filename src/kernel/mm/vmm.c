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
#include <hal.h>

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

bool paging_activated=FALSE;
//--------------------------------------------------------static-function-declarations--------------------------------------------------------------

static void vmm_map_kernel(vmm_context* context);
static int_t vmm_map_inallcon(uintptr_t phys, uintptr_t virt,uint8_t flgs);
static void vmm_mark_used_inallcon(uint_t page);
static void vmm_mark_used(vmm_context*first,uint_t page);
static void vmm_mark_free(vmm_context* context,uint_t page);
static int_t vmm_map(vmm_context* context, uintptr_t phys, uintptr_t virt ,uint8_t flgs);
static void vmm_unmap(vmm_context*context,uintptr_t virt);

vmm_context vmm_init(void)
{
	kprintf("[VMM] I: vmm_init ...");
	/* Speicherkontext anlegen */
	startup_context = vmm_crcontext();
	
	SET_CONTEXT((uintptr_t)startup_context.highest_paging)
	
	//kprin	while(1);tf("print at 0x%p",&kprintf);
	ENABLE_PAGING
	
	paging_activated=TRUE;
	kprintf("SUCCESS\n");
	
	return startup_context;
}

vmm_context vmm_crcontext()
{
	//kprintf("4d is alloced = 0x%x",pmm_is_alloced(0x4d)); 
	vmm_context new_context = {
	.highest_paging=kvmm_malloc(PAGE_SIZE*PAGING_HIER_SIZE),
	.mm_tree=kvmm_malloc(PAGE_SIZE),
	};

	//kprintf("phys at 0x%x/n",virt_to_phys(&startup_context,(uintptr_t)new_context.pd));
	memset((void*)new_context.highest_paging,0x00000000,PAGE_SIZE*2);// clear the PgDIR to avoid invalid values
	memset((void*)new_context.mm_tree,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values

	vmm_map_kernel(&new_context);
	return new_context;
}

void* kvmm_malloc(size_t size)
{
	int i;
	if((size%PAGE_SIZE)==0)
	{
		size=size/PAGE_SIZE;
	}
	else
	{
	    size=size/PAGE_SIZE+1;
	}
	uintptr_t phys=pmm_malloc(size);
	
	if(!paging_activated)
	{
		return (void*)(phys*PAGE_SIZE);
	}

	uintptr_t virt = vmm_find_freemem(&startup_context,size,0x00000000,KERNEL_SPACE);

	if(virt)
	{
		for(i=0;i<size;i++)
		{
			if(vmm_map_inallcon((phys+i)*PAGE_SIZE,virt+(i*PAGE_SIZE),FLGCOMBAT_KERNEL)<0)
			{
				kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_map_page\n");
			}
			vmm_mark_used_inallcon(virt/PAGE_SIZE+i);
		}
	}
	else
	{
	    kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_find_freemem\n");
	}
	
	return (void*)virt;
}
void* uvmm_malloc(vmm_context* context,size_t size)
{
	
	int i;
	if((size%PAGE_SIZE)==0)
	{
		size=size/PAGE_SIZE;
	}
	else
	{
		size=size/PAGE_SIZE+1;
	}
	uintptr_t phys=(uintptr_t) (pmm_malloc(size)*PAGE_SIZE);
	uintptr_t virt = vmm_find_freemem(context,size,KERNEL_SPACE,0xffffffff);
	if(virt)
	{
		for(i=0;i<size;i++)
		{
			if(vmm_map(context,phys+(i*PAGE_SIZE),virt+(i*PAGE_SIZE),FLGCOMBAT_USER)<0)
			{
				kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_map_page\n");
			}
			vmm_mark_used(context,virt/PAGE_SIZE+i);
		}
	}
	else
	{
	    kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_find_freemem\n");
	}
	/* size in 4kb(pages) we are adding one page coz we have to allocate pages which have been written on to 1% also*/
	
	return (void*)virt;
}
uintptr_t vmm_find_freemem(vmm_context* context,size_t size, uintptr_t from,uintptr_t to)
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
			else if(j==size-1)
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
int_t vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs)
{
	int j;
	uintptr_t page =(uintptr_t) ptr/PAGE_SIZE;
	
	if((size%PAGE_SIZE)==0)
	{
		size=size/PAGE_SIZE;
	}
	else
	{
		size=size/PAGE_SIZE+1;
	}
	uintptr_t phys=pmm_malloc(size)*PAGE_SIZE;
	for(j=0;j<size;j++) 
	{
		if (vmm_is_alloced(context,page+j)==TRUE) 
		{
			kprintf("[VMM] E: vmm_realloc says: page 0x%x000 alloiced\n",page +j);
			return -1;
		} 
	}
	for(j =0;j<size;j++)
	{
		if(vmm_map(context,phys+(j*PAGE_SIZE),(page+j)*PAGE_SIZE,flgs)<0)
		{
			kprintf("[VMM] E: vmm_realloc called vmm_map_page and got error\n");
			return -1;
		}
		vmm_mark_used(context,page+j);
	}
	return 0;
}
/*
 * FIXME Map tmp at 0x1000
 */
void vmm_free(vmm_context* context,void* page)//FIXME No Overflow check
{
    vmm_unmap(context,(uintptr_t)page);
    vmm_mark_free(context,(uintptr_t)(page)/PAGE_SIZE);
}
void kvmm_free(void* page)//FIXME No Overflow check
{
	struct proc* cur =first_proc;
	if(cur!=NULL)
	{
		while(cur!=NULL)
		{
			vmm_unmap(cur->context,(uintptr_t)page);
			vmm_mark_free(cur->context,(uintptr_t)(page)/PAGE_SIZE);
			cur=cur->next;
		}
	}
	if(startup_context.highest_paging)
	{
		vmm_unmap(&startup_context,(uintptr_t)page);
		vmm_mark_free(&startup_context,(uintptr_t)(page)/PAGE_SIZE);
	}
}
/*
 * Write copyin/out for user-kernel data exchange
 * FIXME: pagefault, if context != cur_context so this feature is disabled
 */

void* cpyout(void* src,size_t siz)
{
	vmm_context* curcontext= current_thread->proc->context;
	void* dst =uvmm_malloc(curcontext,siz);
	memcpy(dst,src,siz);
	return dst;
}
void* cpyin(void* src,size_t siz)
{
	void* dst =kvmm_malloc(siz);
	memcpy(dst,src,siz);
	return dst;
}
int_t vmm_map_inallcon(uintptr_t phys, uintptr_t virt, uint8_t flgs)
{
	struct proc* cur =first_proc;
	if(cur!=NULL)
	{
		while(cur!=NULL)
		{
			if(vmm_map(cur->context, phys,virt,flgs))
			{
				kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
				return -1;
			}
			cur=cur->next;
		}
	}
	if(startup_context.highest_paging)
	{
		if(vmm_map(&startup_context,phys,virt,flgs))
		{
			kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
			return -1;
		}
	}
	return 0;
}
static int_t vmm_map(vmm_context* context, uintptr_t phys, uintptr_t virt,uint8_t flgs)
{
	
	// We need 4k alignment 
	if (((virt % PAGE_SIZE)!=0) || ((phys % PAGE_SIZE)!= 0)) 
	{
	    kprintf("[VMM] E: vmm_map has no 4k alignment\n",virt,phys);
	    return -1;
	}
#ifdef ARCH_X86
	struct vmm_pagetbl* page_table=NULL; // that's the pagetable we are using later
	
	vmm_context* curcontext=get_cur_context();
	
	uint_t page =virt/PAGE_SIZE;
	uint_t pd_index = page / 1024;
	uint_t pt_index = page % 1024;
	/*
	* -----------------------------------------------------find-or-create-pagetable---------------------------------------------------------------
	* (in the first case the Page table is allready existing)
	*/
	if (context->highest_paging[pd_index].rw_flags& FLG_IN_MEM) 
	{
		/*
		 * first we set page_table to it's physical value than we change it to virtual one by mapping it to TMP_PAGEBUF.
		 * If pd_index == 0 we know it's the page following to the PD. 
		 * We mustn't map it to TMP_PAGEBUF to prevent endless recursion when mapping to TMP_PAGEBUF, which is > 0x400000
		 */
		page_table = (void*)((uintptr_t)context->highest_paging[pd_index].pagetbl_ptr*PAGE_SIZE);
		if((pd_index==0)&&(paging_activated))
		{
			page_table =(struct vmm_pagetbl *)((uintptr_t)(context->highest_paging)+PAGE_SIZE);
		}
		else if(paging_activated)
		{
			vmm_map(curcontext,(uintptr_t)page_table,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			
			page_table =(struct vmm_pagetbl *)TMP_PAGEBUF;
		}
	}
	else 
	{
		/* 
		 * setup new pagetable 
		 * if that's the first page_table it's the next page from PD
		 */
		if(pd_index==0)
		{
			/*
			 * virt_to_phys() doesn't lead to a recursion because we hopefully only get there if a new context is created and paging enabled,
			 * so that we have to get the phys address of the PD in the creator pagetable
			 */
			if(curcontext->highest_paging!=NULL)
			{
				page_table =(struct vmm_pagetbl *)(virt_to_phys(&startup_context, (uintptr_t)context->highest_paging)+PAGE_SIZE);
			}
			else
			{
				page_table =(struct vmm_pagetbl *)((uintptr_t)context->highest_paging+PAGE_SIZE);
			}
		}
		else
		{
			page_table = (struct vmm_pagetbl*)(pmm_malloc(1)*PAGE_SIZE);// alloc physical memory
		}
		/*
		 * You shall not follow the NULL Pointer cause for chaos an madness at it's end ...
		 */
		if(page_table==NULL)
		{
		    kprintf("[VMM] E: vmm_map_page doesn't found a Page Table\n");
		    return -1;
		}
		
		context->highest_paging[pd_index].rw_flags=flgs;
		context->highest_paging[pd_index].reserved=0x0;
		context->highest_paging[pd_index].pagetbl_ptr =(uintptr_t)(page_table)/PAGE_SIZE;
		
		if((paging_activated)&&(virt!=TMP_PAGEBUF))
		{// if paging is activated TMP_PAGEBUF will be allways mapped
			vmm_map(curcontext,(uintptr_t)page_table,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			page_table =(struct vmm_pagetbl*)TMP_PAGEBUF;
		}
		memset(page_table, 0x00000000, PAGE_SIZE);
	} 
	
	page_table[pt_index].rw_flags = flgs;
	page_table[pt_index].reserved = 0x0;
	page_table[pt_index].page_ptr=phys/PAGE_SIZE;
	
	INVALIDATE_TLB(virt)
	
	if((paging_activated)&&(virt!=TMP_PAGEBUF))
	{
	    vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	}
#endif
#ifdef ARCH_X86_64
	struct vmm_pagedir_ptrtbl * pagedir_ptrtbl=NULL; // that's the pagetable we are using later
	struct vmm_pagedir * pagedir=NULL;
	struct vmm_pagetbl * page_table=NULL;
	
	vmm_context* curcontext=get_cur_context();
	
	uint_t page =virt/PAGE_SIZE;
	uint_t pt_index = page  % 512;
	uint_t pd_index = (page%(512*512))/512;
	uint_t pd_ptr_index = (page%(512*512*512))/(512*512);
	uint_t map_lvl4_index = page/(512*512*512);
	
	/*
	* -----------------------------------------------------find-or-create-pagedirectorytable---------------------------------------------------------------
	* (in the first case the pagedirectorytable is allready existing)
	*/
	if (context->highest_paging[map_lvl4_index].rw_flags& FLG_IN_MEM) 
	{
		/*
		 * first we set pagedir_ptrtbl to it's physical value than we change it to virtual one by mapping it to TMP_PAGEBUF.
		 * If pd_index == 0 we know it's the page following to the PD. 
		 * We mustn't map it to TMP_PAGEBUF to prevent endless recursion when mapping to TMP_PAGEBUF, which is > 0x400000
		 */
		pagedir_ptrtbl = (void*)((uintptr_t)(context->highest_paging[map_lvl4_index].pagedirptrtbl_ptr)*PAGE_SIZE);
		if((map_lvl4_index==0)&&(paging_activated))
		{
			pagedir_ptrtbl =(struct vmm_pagedir_ptrtbl *)((uintptr_t)(context->highest_paging)+PAGE_SIZE);
		}
		else if(paging_activated)
		{
			vmm_map(curcontext,(uintptr_t)pagedir_ptrtbl,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			
			pagedir_ptrtbl =(struct vmm_pagedir_ptrtbl *)TMP_PAGEBUF;
		}
	}
	else 
	{
		/* 
		 * setup new pagedirectorytable 
		 * if that's the first pagedir_ptrtbl it's the next page from PD
		 */
		if(map_lvl4_index==0)
		{
			/*
			 * virt_to_phys() doesn't lead to a recursion because we hopefully only get there if a new context is created and paging enabled,
			 * so that we have to get the phys address of the pagedirectorytable in the creator pagedirectorytable
			 */
			if(curcontext->highest_paging!=NULL)
			{
				pagedir_ptrtbl =(struct vmm_pagedir_ptrtbl *)(virt_to_phys(&startup_context, (uintptr_t)context->highest_paging)+PAGE_SIZE);
			}
			else
			{
				pagedir_ptrtbl =(struct vmm_pagedir_ptrtbl *)((uintptr_t)context->highest_paging+PAGE_SIZE);
			}
		}
		else
		{
			pagedir_ptrtbl = (struct vmm_pagedir_ptrtbl*)(pmm_malloc(1)*PAGE_SIZE);// alloc physical memory
		}
		
		context->highest_paging[map_lvl4_index].rw_flags=flgs;
		context->highest_paging[map_lvl4_index].reserved=0x0;
		context->highest_paging[map_lvl4_index].pagedirptrtbl_ptr =((uintptr_t)pagedir_ptrtbl)/PAGE_SIZE;
		
		if((paging_activated)&&(virt!=TMP_PAGEBUF))
		{// if paging is activated TMP_PAGEBUF will be allways mapped
			vmm_map(curcontext,(uintptr_t)pagedir_ptrtbl,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			pagedir_ptrtbl =(struct vmm_pagedir_ptrtbl*)TMP_PAGEBUF;
		}
		memset(pagedir_ptrtbl, 0x00000000, PAGE_SIZE);
	}
	/*
	 * You shall not follow the NULL Pointer cause for chaos an madness at it's end ...
	 */
	if(pagedir_ptrtbl==NULL)
	{
		kprintf("[VMM] E: vmm_map_page doesn't found a Page Table\n");
		return -1;
	}
	
	/*
	* -----------------------------------------------------find-or-create-pagedirectory---------------------------------------------------------------
	* (in the first case the pagedirectory is allready existing)
	*/
	if(pagedir_ptrtbl[pd_ptr_index].rw_flags& FLG_IN_MEM)
	{
		/*
		 * first we set pagedir to it's physical value than we change it to virtual one by mapping it to TMP_PAGEBUF.
		 * If pd_index == 0 we know it's the page following to the PD. 
		 * We mustn't map it to TMP_PAGEBUF to prevent endless recursion when mapping to TMP_PAGEBUF, which is > 0x400000
		 */
		pagedir = (void*)((uintptr_t)pagedir_ptrtbl[pd_ptr_index].pagedir_ptr*PAGE_SIZE);
		if((pd_ptr_index==0)&&(paging_activated))
		{
			pagedir =(struct vmm_pagedir *)((uintptr_t)(context->highest_paging)+(PAGE_SIZE*2));
		}
		else if(paging_activated)
		{
			vmm_map(curcontext,(uintptr_t)pagedir,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			
			pagedir =(struct vmm_pagedir *)TMP_PAGEBUF;
		}
	}
	else 
	{
		/* 
		 * setup new pagedirectory 
		 * if that's the first pagedir_ptrtbl it's the next page from PD
		 */
		if(pd_ptr_index==0)
		{
			/*
			 * virt_to_phys() doesn't lead to a recursion because we hopefully only get there if a new context is created and paging enabled,
			 * so that we have to get the phys address of the pagedirectory in the creator pagedirectory
			 */
			if(curcontext->highest_paging!=NULL)
			{
				pagedir =(struct vmm_pagedir *)(virt_to_phys(&startup_context, (uintptr_t)context->highest_paging)+(PAGE_SIZE*2));
			}
			else
			{
				pagedir =(struct vmm_pagedir *)((uintptr_t)context->highest_paging+(PAGE_SIZE*2));
			}
		}
		else
		{
			pagedir = (struct vmm_pagedir*)(pmm_malloc(1)*PAGE_SIZE);// alloc physical memory
		}
		
		pagedir_ptrtbl[pd_ptr_index].rw_flags=flgs;
		pagedir_ptrtbl[pd_ptr_index].reserved=0x0;
		pagedir_ptrtbl[pd_ptr_index].pagedir_ptr =((uintptr_t)pagedir)/PAGE_SIZE;
		
		if((paging_activated)&&(virt!=TMP_PAGEBUF))
		{// if paging is activated TMP_PAGEBUF will be allways mapped
			vmm_map(curcontext,(uintptr_t)pagedir_ptrtbl,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			pagedir=(struct vmm_pagedir*)TMP_PAGEBUF;
		}
		memset(pagedir, 0x00000000, PAGE_SIZE);
	}
	/*
	* You shall not follow the NULL Pointer cause for chaos an madness at it's end ...
	*/
	if(pagedir==NULL)
	{
		    kprintf("[VMM] E: vmm_map_page doesn't found a Page Table\n");
		    return -1;
	}
	/*
	* -----------------------------------------------------find-or-create-pagetable---------------------------------------------------------------
	* (in the first case the Page table is allready existing)
	*/
	if (pagedir[pd_index].rw_flags& FLG_IN_MEM) 
	{
		/*
		 * first we set page_table to it's physical value than we change it to virtual one by mapping it to TMP_PAGEBUF.
		 * If pd_index == 0 we know it's the page following to the PD. 
		 * We mustn't map it to TMP_PAGEBUF to prevent endless recursion when mapping to TMP_PAGEBUF, which is > 0x400000
		 */
		page_table = (void*)((uintptr_t)pagedir[pd_index].pagetbl_ptr*PAGE_SIZE);
		if((pd_index==0)&&(paging_activated))
		{
			page_table =(struct vmm_pagetbl *)((uintptr_t)(context->highest_paging)+(PAGE_SIZE*3));
		}
		else if(paging_activated)
		{
			vmm_map(curcontext,(uintptr_t)page_table,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			
			page_table =(struct vmm_pagetbl *)TMP_PAGEBUF;
		}
	}
	else 
	{
		/* 
		 * setup new pagetable 
		 * if that's the first page_table it's the next page from PD
		 */
		if(pd_index==0)
		{
			/*
			 * virt_to_phys() doesn't lead to a recursion because we hopefully only get there if a new context is created and paging enabled,
			 * so that we have to get the phys address of the PD in the creator pagetable
			 */
			if(curcontext->highest_paging!=NULL)
			{
				page_table =(struct vmm_pagetbl *)(virt_to_phys(&startup_context, (uintptr_t)context->highest_paging)+(PAGE_SIZE*3));
			}
			else
			{
				page_table =(struct vmm_pagetbl *)((uintptr_t)context->highest_paging+(PAGE_SIZE*3));
			}
		}
		else
		{
			page_table = (struct vmm_pagetbl*)(pmm_malloc(1)*PAGE_SIZE);// alloc physical memory
		}
		
		pagedir[pd_index].rw_flags=flgs;
		pagedir[pd_index].reserved=0x0;
		pagedir[pd_index].pagetbl_ptr =((uintptr_t)page_table)/PAGE_SIZE;
		
		if((paging_activated)&&(virt!=TMP_PAGEBUF))
		{// if paging is activated TMP_PAGEBUF will be allways mapped
			vmm_map(curcontext,(uintptr_t)page_table,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
			page_table =(struct vmm_pagetbl*)TMP_PAGEBUF;
		}
		memset(page_table, 0x00000000, PAGE_SIZE);
	}
	/*
	* You shall not follow the NULL Pointer cause for chaos an madness at it's end ...
	*/
	if(page_table==NULL)
	{
		kprintf("[VMM] E: vmm_map_page doesn't found a Page Table\n");
		return -1;
	}
	
	page_table[pt_index].rw_flags = flgs;
	page_table[pt_index].reserved = 0x0;
	page_table[pt_index].page_ptr=phys/PAGE_SIZE;
	
	INVALIDATE_TLB(virt)
	
	if((paging_activated)&&(virt!=TMP_PAGEBUF))
	{
	    vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	}
#endif
	return 0;
}
void vmm_unmap(vmm_context*context,uintptr_t page)
{
#ifdef ARCH_X86
	uint_t page_index = (uintptr_t)(page) / PAGE_SIZE;
	struct vmm_pagedir *pdirentr = &context->highest_paging[page_index/1024];
	struct vmm_pagetbl *pgtbl = (struct vmm_pagetbl *) ((uintptr_t)(pdirentr->pagetbl_ptr*PAGE_SIZE));
	struct vmm_pagetbl *pgtblentr=&pgtbl[page_index%1024];
	vmm_context * curcontext = get_cur_context();
	
	if((pgtblentr->page_ptr==0x00000000)||(pdirentr->pagetbl_ptr==0x00000000))
	{
		kprintf("[VMM] E: vmm_free can't free page- page not mapped\n");
		return;
	}
	vmm_map(curcontext,(uintptr_t)pgtbl,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	pgtbl =(struct vmm_pagetbl *)TMP_PAGEBUF;
	memset(pgtblentr,0x00000000,sizeof(struct vmm_pagetbl));
	vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);   
#endif
	return;
}
vmm_context * get_cur_context()
{
	if(current_thread==NULL)
	{
		return &startup_context;
	}
	else
	{
		return current_thread->proc->context;
	}
	
}
bool vmm_is_alloced(vmm_context* context,uint_t page)//FIXME No Overflow check
{
	uint_t master_ind = page/1024;
	uint_t node_ind=page%1024;
	uint_t inner_nodepkgoff=(master_ind%32)*1024;
	struct vmm_tree_master*tree = context->mm_tree;
	struct vmm_tree_nodepkg* nodes = (void*)((uintptr_t)tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
	
	/*if(tree->used[master_ind])
	{
	    kprintf("0x%x is alloced1 used at 0x%x",page*PAGE_SIZE,(uintptr_t)&tree->used[master_ind]);
	    return TRUE;
	}*/
	if(nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]&(1<<((node_ind+inner_nodepkgoff)%32)))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
static void vmm_mark_used(vmm_context* context,uint_t page)
{
	uint_t master_ind = page/1024;
	uint_t node_ind=page%1024;
	uint_t inner_nodepkgoff=(master_ind%32)*1024;
	struct vmm_tree_master*tree = context->mm_tree;
	struct vmm_tree_nodepkg* nodes = (void*)((uintptr_t)tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
	/*
	if(tree->used[master_ind])
	{
	    return 0;
	}*/
	nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]|=(1<<((node_ind+inner_nodepkgoff)%32));
}
static void vmm_mark_free(vmm_context* context,uint_t page)
{
	uint_t master_ind = page/1024;
	uint_t node_ind=page%1024;
	uint_t inner_nodepkgoff=(master_ind%32)*1024;
	struct vmm_tree_master*tree = context->mm_tree;
	struct vmm_tree_nodepkg* nodes = (void*)((uintptr_t)tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
	/*
	if(tree->used[master_ind])
	{
	    return 0;
	}*/
	nodes->nodepkgentr[(node_ind+inner_nodepkgoff)/32]&= ~(1<<((node_ind+inner_nodepkgoff)%32));
}
static void vmm_mark_used_inallcon(uint_t page)
{
	struct proc* cur =first_proc;
	
	if(cur!=NULL)
	{
		while(cur!=NULL)
		{
			vmm_mark_used(cur->context,page);
			cur=cur->next;
		}
	}
	if(startup_context.highest_paging)
	{
		vmm_mark_used(&startup_context,page);
	}
}

static void vmm_map_kernel(vmm_context* context)
{
	int i=0;
	uintptr_t node_virt=0x0;
	uintptr_t node_phys =0x0;
	uintptr_t phys =0x0;
	vmm_context* curcontext=get_cur_context();
	if(paging_activated)
	{
		for(i=0;i<32;i++)
		{
			node_virt=vmm_find_freemem(curcontext,1,0x0,KERNEL_SPACE/PAGE_SIZE);
			node_phys =pmm_malloc(1);
			context->mm_tree->nodepkg[i].nodepkg_ptr=node_virt/PAGE_SIZE;
			vmm_map_inallcon(node_phys*PAGE_SIZE,node_virt,FLGCOMBAT_KERNEL);
			memset((void*)node_virt,0x00000000,PAGE_SIZE);
			vmm_mark_used_inallcon(context->mm_tree->nodepkg[i].nodepkg_ptr);
			//kprintf("tree%d node virt = 0x%x",i,node_virt);
		}
		for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++)
		{
			if(vmm_is_alloced(curcontext,i))
			{
				phys=virt_to_phys(curcontext,i*PAGE_SIZE);
				//kprintf("map 0x%x to 0x%x",phys,i*PAGE_SIZE);
				if(vmm_map(context,phys,i*PAGE_SIZE,FLGCOMBAT_KERNEL)<0)
				{
					kprintf("[VMM] E: vmm_map_kernel received bad value \n");
				}
				vmm_mark_used(context,i);
			}
		}
	}
	else
	{
		for(i=0;i<32;i++)
		{
			node_phys =pmm_malloc(1);
			context->mm_tree->nodepkg[i].nodepkg_ptr=node_phys;
			memset((void*)(node_phys*PAGE_SIZE),0x00000000,PAGE_SIZE);
		}
		
		for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++)
		{
			if(pmm_is_alloced(i))
			{
				if(vmm_map(context,i*PAGE_SIZE,i*PAGE_SIZE,FLGCOMBAT_KERNEL)<0)
				{
					kprintf("[VMM] E: vmm_map_kernel received bad value \n");
				}
				vmm_mark_used(context,i);
			}
		}
	}
}
uintptr_t phys_to_virt(vmm_context* context,uintptr_t phys)
{
	return 0;
}
uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt)
{
#ifdef ARCH_X86
	uintptr_t page =virt/PAGE_SIZE;
	uintptr_t phys =0;
	uint32_t pd_index = page / 1024;
	uint32_t pt_index = page % 1024;
	vmm_context* curcontext=get_cur_context();
	
	struct vmm_pagedir *pdirentr = &context->highest_paging[pd_index];
	struct vmm_pagetbl *pgtbl = (struct vmm_pagetbl *) ((uintptr_t)(pdirentr->pagetbl_ptr*PAGE_SIZE));
	vmm_map(curcontext,(uintptr_t)pgtbl,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	pgtbl=(struct vmm_pagetbl *)TMP_PAGEBUF;
	struct vmm_pagetbl *pgtblentr=&pgtbl[pt_index];
	phys=pgtblentr->page_ptr*PAGE_SIZE;
	vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	return phys;
#endif
#ifdef ARCH_X86_64
	return virt;
#endif
}
