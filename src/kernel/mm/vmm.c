/*   <src-path>/src/kernel/mm/vmm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
static int_t vmm_map_inallcon(uintptr_t virt, uintptr_t phys,uint8_t flgs);
static void vmm_mark_used_inallcon(uint_t page);
static void vmm_mark_used(vmm_context*first,uint_t page);
static bool vmm_is_alloced(vmm_context* context,uint_t page);
static void vmm_mark_free(vmm_context* context,uint_t page);

vmm_context vmm_init(void)
{
	kprintf("[VMM] I: VMM setup...");
	/* Speicherkontext anlegen */
	startup_context = vmm_crcontext();

	SET_CONTEXT((uintptr_t)startup_context.pd)
    
	ENABLE_PAGING 
	paging_activated=TRUE;
	kprintf("SUCCESS\n");
	return startup_context;
}

vmm_context vmm_crcontext()
{
	//kprintf("4d is alloced = 0x%x",pmm_is_alloced(0x4d)); 
	vmm_context new_context = {
	.pd=kvmm_malloc(PAGE_SIZE*2),
	.tr=kvmm_malloc(PAGE_SIZE),
	};
	//kprintf("phys at 0x%x/n",virt_to_phys(&startup_context,(uintptr_t)new_context.pd));
	memset((void*)new_context.pd,0x00000000,PAGE_SIZE*2);// clear the PgDIR to avoid invalid values
	memset((void*)new_context.tr,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values
	vmm_map_kernel(&new_context);
	return new_context;
}

void* kvmm_malloc(size_t size){
    
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
	//kprintf("size= 0x%x ",size);
	if(!paging_activated)
	{
		return (void*)(phys*PAGE_SIZE);
	}

	uintptr_t virt = vmm_find_freemem(&startup_context,size,0x00000000,KERNEL_SPACE);

	if(virt)
	{
		for(i=0;i<size;i++)
		{
			if(vmm_map_inallcon(virt+(i*PAGE_SIZE),(phys+i)*PAGE_SIZE,FLGCOMBAT_KERNEL)<0)
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
			if(vmm_map(context,virt+(i*PAGE_SIZE),phys+(i*PAGE_SIZE),FLGCOMBAT_USER)<0)
			{
				kprintf("[VMM] E: vmm_malloc gets invalid return value from vmm_map_page\n");
			}
			vmm_mark_used(context,virt/PAGE_SIZE+size);
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
	uintptr_t virt=0x0;// avoid unitialized Pointer Bug
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
				virt = i*PAGE_SIZE;
				goto out;
			}
		}
	}
out:
	
	if(virt ==0x0)
	{// nothing was found
	  kprintf("[VMM] E: vmm_find_freemem couldn't get free space in addressspace\n");
	  return 0x0;
	}
	else
	{
	  return virt;
	}
}
int vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs)
{
	int j,k;
	uintptr_t page =(uintptr_t) ptr/PAGE_SIZE;
	
	if((size%PAGE_SIZE)==0)
	{
		size=size/PAGE_SIZE;
	}
	else
	{
		size=size/PAGE_SIZE+1;
	}
	uintptr_t phys=(uintptr_t) pmm_malloc(size)*PAGE_SIZE;
	
	for(j=0;j<size;j++) 
	{
		if (vmm_is_alloced(context,page+j)==TRUE) 
		{
			kprintf("[VMM] E: vmm_realloc says: page 0x%x000 alloiced\n",page +j);
			return -1;
		} 
		else if(j==size-1)
		{
			for(k =0;k<size;k++)
			{
				if(vmm_map(context,(page+k)*PAGE_SIZE,phys,flgs)<0)
				{
					kprintf("[VMM] E: vmm_realloc called vmm_map_page and got error\n");
					return -1;
				}
			}
			return 0;
		}
	}
	kprintf("[VMM] E: vmm_realloc couldn't get free space in addressspace\n");
	return -1;
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
	if(startup_context.pd)
	{
		vmm_unmap(&startup_context,(uintptr_t)page);
		vmm_mark_free(&startup_context,(uintptr_t)(page)/PAGE_SIZE);
	}
}
/*
 * Write copyin/out for user-kernel data exchange
 *
 */

void* cpyout(vmm_context* context,void* src,size_t siz){
	void* dst =uvmm_malloc((void*)((uintptr_t)(context)),siz);
	memcpy(dst,src,siz);
	return dst;
}
void* cpyin(void* src,size_t siz){
	void* dst =kvmm_malloc(siz);
	memcpy(dst,src,siz);
	return dst;
}
int_t vmm_map_inallcon(uintptr_t virt, uintptr_t phys,uint8_t flgs){
	struct proc* cur =first_proc;
	if(cur!=NULL)
	{
		while(cur!=NULL)
		{
			if(vmm_map(cur->context, virt,phys,flgs)){
				kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
				return -1;
			}
			cur=cur->next;
		}
	  
	}
	if(startup_context.pd){
		if(vmm_map(&startup_context,virt,phys,flgs)){
			kprintf("[VMM] E: kvmm_malloc vmm_mark_used_inallcon get invalid vmm_mark_used return\n");
			return -1;
		}
	}
	return 0;
}
int_t vmm_map(vmm_context* context, uintptr_t virt, uintptr_t phys,uint8_t flgs){
	struct vmm_pagetbl* page_table=NULL;
	
	uint_t page =virt/PAGE_SIZE;
	uint_t pd_index = page / 1024;
	uint_t pt_index = page % 1024;
	vmm_context* curcontext=NULL;
	if(current_thread==NULL)
	{
	    curcontext= &startup_context;
	}
	else
	{
	    curcontext= current_thread->proc->context;
	}
	// We need 4k alignment 
	if (((virt % PAGE_SIZE)!=0) || ((phys % PAGE_SIZE)!= 0)) {
	    kprintf("[VMM] E: vmm_map has no 4k alignment\n",virt,phys);
	    return -1;
	}
	
	struct vmm_pagetblentr new_pgtblentr ={
	    .rw_flags = flgs,
	    .reserved = 0x0,
	    .page_ptr =phys/PAGE_SIZE
	};
	/*
	* -----------------------------------------------------find-or-create-pagetable---------------------------------------------------------------
	*/
	if (context->pd->pgdir[pd_index].rw_flags& FLG_IN_MEM) 
	{
		/* Page Table ist schon vorhanden */
		page_table = (void*)(context->pd->pgdir[pd_index].pagetbl_ptr*PAGE_SIZE);
		
		if((paging_activated)&&(virt != TMP_PAGEBUF))// if paging is activated TMP_PAGEBUF will be allways mapped
		{
			vmm_map(curcontext,TMP_PAGEBUF,(uintptr_t)page_table,FLGCOMBAT_KERNEL);
			
			page_table =(struct vmm_pagetbl *)TMP_PAGEBUF;
		}
		else if(virt==TMP_PAGEBUF){
			page_table =(struct vmm_pagetbl *)((uintptr_t)(context->pd)+PAGE_SIZE);
		}
	} 
	else 
	{
		// setup new pagetable 
		if(pd_index==0){
			if(curcontext->pd!=NULL){
				page_table =(struct vmm_pagetbl *)(virt_to_phys(&startup_context,(uintptr_t)context->pd)+PAGE_SIZE);
			}
			else{
				page_table =(struct vmm_pagetbl *)((uintptr_t)context->pd+PAGE_SIZE);
			}
		}
		else
		{
			
			page_table = (struct vmm_pagetbl*)(pmm_malloc(1)*PAGE_SIZE);
		}
		
		if(page_table==NULL)
		{
		    kprintf("[VMM] E: vmm_map_page doesn't found a Page Table\n");
		    return -1;
		}
		struct vmm_pagedirentr new_pgdirentr ={
		    .rw_flags = flgs,
		    .reserved=0x0,
		    .pagetbl_ptr =(uintptr_t)(page_table)/PAGE_SIZE
		};
		context->pd->pgdir[pd_index] =new_pgdirentr;
		if((paging_activated)&&(virt!=TMP_PAGEBUF))
		{// if paging is activated TMP_PAGEBUF will be allways mapped
			vmm_map(curcontext,TMP_PAGEBUF,(uintptr_t)page_table,FLGCOMBAT_KERNEL);
			page_table =(struct vmm_pagetbl*)TMP_PAGEBUF;
		}
		memset(page_table, 0x00000000, PAGE_SIZE);
	} 
	page_table->pgtbl[pt_index] =new_pgtblentr;
	INVALIDATE_TLB(virt)
	
	if((paging_activated)&&(virt!=TMP_PAGEBUF))
	{
	    vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	}
	return 0;
}
void vmm_unmap(vmm_context*context,uintptr_t page){
	uint_t page_index = (uintptr_t)(page) / PAGE_SIZE;
	struct vmm_pagedirentr *pdirentr = &context->pd->pgdir[page_index/1024];
	struct vmm_pagetbl *pgtbl = (struct vmm_pagetbl *) ((uintptr_t)(pdirentr->pagetbl_ptr*PAGE_SIZE));
	struct vmm_pagetblentr *pgtblentr=&pgtbl->pgtbl[page_index%1024];
	vmm_context* curcontext=NULL;
	
	if(!intr_activated)
	{
		curcontext= &startup_context;
	}
	else
	{
		curcontext= current_thread->proc->context;
	}
	if((pgtblentr->page_ptr==0x00000000)||(pdirentr->pagetbl_ptr==0x00000000))
	{
		kprintf("[VMM] E: vmm_free can't free page- page not mapped\n");
		return;
	}
	vmm_map(curcontext,TMP_PAGEBUF,(uintptr_t)pgtbl,FLGCOMBAT_KERNEL);
	pgtbl =(struct vmm_pagetbl *)TMP_PAGEBUF;
	memset(pgtblentr,0x00000000,4);
	vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);   
	
	return;
}
bool vmm_is_alloced(vmm_context* context,uint_t page)//FIXME No Overflow check
{
	uint_t master_ind = page/1024;
	uint_t node_ind=page%1024;
	uint_t inner_nodepkgoff=(master_ind%32)*1024;
	struct vmm_tree_master*tree = context->tr;
	struct vmm_tree_nodepkg* nodes = (void*)(tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
	
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
	struct vmm_tree_master*tree = context->tr;
	struct vmm_tree_nodepkg* nodes = (void*)(tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
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
	struct vmm_tree_master*tree = context->tr;
	struct vmm_tree_nodepkg* nodes = (void*)(tree->nodepkg[master_ind/32].nodepkg_ptr*PAGE_SIZE);
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
	if(startup_context.pd)
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
	vmm_context* curcontext=NULL;
	
	if(!intr_activated)
	{
	    curcontext= &startup_context;
	}
	else
	{
	    curcontext= current_thread->proc->context;
	}
	if(paging_activated)
	{
		for(i=0;i<32;i++)
		{
			node_virt=vmm_find_freemem(curcontext,1,0x0,KERNEL_SPACE/PAGE_SIZE);
			node_phys =(uintptr_t)pmm_malloc(1);
			context->tr->nodepkg[i].nodepkg_ptr=node_virt/PAGE_SIZE;
			vmm_map(curcontext,node_virt,node_phys*PAGE_SIZE,FLGCOMBAT_KERNEL);
			memset((void*)node_virt,0x00000000,PAGE_SIZE);
			vmm_mark_used_inallcon(context->tr->nodepkg[i].nodepkg_ptr);
			//kprintf("tree%d node virt = 0x%x",i,node_virt);
		}
		for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++) 
		{
			if(vmm_is_alloced(curcontext,i))
			{
				phys=virt_to_phys(curcontext,i*PAGE_SIZE);
				//kprintf("map 0x%x to 0x%x",phys,i*PAGE_SIZE);
				if(vmm_map(context,i*PAGE_SIZE,phys,FLGCOMBAT_KERNEL)<0)
				{
					kprintf("erro"); 
				}
				vmm_mark_used(context,i);
			}
		}
	}
	else
	{
		for(i=0;i<32;i++)
		{
			node_phys =(uintptr_t)pmm_malloc(1);
			context->tr->nodepkg[i].nodepkg_ptr=node_phys;
			memset((void*)(node_phys*PAGE_SIZE),0x00000000,PAGE_SIZE);
		}
		
		for (i = 0; i <KERNEL_SPACE/PAGE_SIZE; i ++) 
		{
			if(pmm_is_alloced(i))
			{
				if(vmm_map(context,i*PAGE_SIZE,i*PAGE_SIZE,FLGCOMBAT_KERNEL)<0)
				{
					kprintf("err");
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
	uint32_t page =virt/PAGE_SIZE;
	uint32_t phys =0;
	uint32_t pd_index = page / 1024;
	uint32_t pt_index = page % 1024;
	vmm_context* curcontext=NULL;

	if(current_thread==NULL)
	{
		curcontext= &startup_context;
	}
	else
	{
		curcontext= current_thread->proc->context;
	}

	struct vmm_pagedirentr *pdirentr = &(context->pd->pgdir[pd_index]);
	struct vmm_pagetbl *pgtbl = (struct vmm_pagetbl *) ((uintptr_t)(pdirentr->pagetbl_ptr*PAGE_SIZE));
	vmm_map(curcontext,TMP_PAGEBUF,(uintptr_t)pgtbl,FLGCOMBAT_KERNEL);
	pgtbl=(struct vmm_pagetbl *)TMP_PAGEBUF;
	struct vmm_pagetblentr *pgtblentr=&pgtbl->pgtbl[pt_index];
	phys=pgtblentr->page_ptr*PAGE_SIZE;
	vmm_map(curcontext,TMP_PAGEBUF,TMP_PAGEBUF,FLGCOMBAT_KERNEL);
	return phys;
}
