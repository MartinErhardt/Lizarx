/*   <src-path>/src/kernel/mm/pmm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <boot/multiboot.h>
#include <dbg/console.h>
#include <drv/vga-txt_graphics/vram.h>
#include <boot/init.h>
#include <libOS/lock.h>
#include <macros.h>
/*
 * So far we implement the PMM as a flat Bitmap where each bit indicates one Page's status.
 * 1 = free space
 * 0 = used
 * PAGE_SIZE =0x1000(on AMD64 and i386 at least
 */
#define PMM_USED 0
#define PMM_FREE 1

#define PMM_STACK_SIZE 1000
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

static uint32_t physbitmap[BITMAP_SIZE];
uint_t pmm_stack[PMM_STACK_SIZE];
uint_t * pmm_stack_ptr = NULL;
extern const void kernel_start;
extern const void kernel_end;

static void pmm_init_stack();
static uint_t pmm_pop_stack();
static void pmm_push_stack(uint_t to_push);
static uint_t pmm_malloc_4k_no_stack();
static bool pmm_is_alloced(uint_t page);
static void pmm_mark_used(uint_t page);

void pmm_init(struct multiboot_info * mb_info)
{
	
	struct multiboot_mmap* mmap = (void *)((uintptr_t)(mb_info->mbs_mmap_addr));
	struct multiboot_mmap* mmap_end = (void*)
	    ((uintptr_t) (mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length));
	
	uint64_t addr_mark_manual_until_that=0;
	uint64_t addr_mark_manual_after_that=0;
	uint64_t addr = 0;
	uint64_t end_addr = 0;
	
	/* by default everything is reserved */
	memset(&physbitmap, 0x00000000, sizeof(physbitmap));
	kprintf("[PMM] I: pmm_init ... ");

	pmm_stack_ptr = &pmm_stack[PMM_STACK_SIZE];
	/*
	* Here we look in the memory map for free space
	*/
	while (mmap < mmap_end) 
	{
		if (mmap->type == 1)
		{
			/* 
			 * We and the addresses with 0xffffffff to prevent a buffer overflow
			 * FIXME our Bitmap only covers a 32 bit address space
			 */
			addr = mmap->base;
			end_addr = addr + mmap->length;
			if(addr%PAGE_SIZE)
				addr=((addr/PAGE_SIZE)+1)*PAGE_SIZE;
			if(end_addr%PAGE_SIZE)
				end_addr=((end_addr/PAGE_SIZE)-1)*PAGE_SIZE;
			
			addr_mark_manual_until_that=(((addr/PAGE_SIZE)/32)*32);
			addr_mark_manual_after_that=(((end_addr/PAGE_SIZE)/32)*32);
			if(addr_mark_manual_until_that<addr)
				addr_mark_manual_until_that=((((addr/PAGE_SIZE)/32)+1)*32);
			if(addr_mark_manual_after_that<addr)
				addr_mark_manual_after_that=((((end_addr/PAGE_SIZE)/32)+1)*32);
			
			if(addr_mark_manual_after_that==addr_mark_manual_until_that)
			{
				/* 
				* We and the addresses with 0xffffffff to prevent a buffer overflow
				* FIXME our Bitmap only covers a 32 bit address space
				*/
				while ((addr/PAGE_SIZE < end_addr/PAGE_SIZE)&&(addr<0x100000000)) 
				{
					pmm_free_4k_unsafe(((uint32_t)addr)/PAGE_SIZE);
					addr += PAGE_SIZE;
				}
				goto cont;
			}
			
			while ((addr/PAGE_SIZE < addr_mark_manual_until_that)&&(addr<0x100000000)) 
			{
				pmm_free_4k_unsafe(((uint32_t)addr)/PAGE_SIZE);
				addr += PAGE_SIZE;
			}
			
			while ((addr/PAGE_SIZE < addr_mark_manual_after_that)&&(addr<0x100000000)) 
			{
				physbitmap[((uint32_t)addr) / PAGE_SIZE / 32] = 0xffffffff;
				addr += PAGE_SIZE*32;
			}
			while ((addr/PAGE_SIZE <= end_addr/PAGE_SIZE)&&(addr<0x100000000)) 
			{
				pmm_free_4k_unsafe(((uint32_t)addr)/PAGE_SIZE);
				addr += PAGE_SIZE;
			}
		}
cont:
		mmap++;
	}
	
	/* 
	 * here we mark the kernel as used
	 */
	addr = (uintptr_t) &kernel_start;
	
	while (addr <= (uintptr_t) &kernel_end) 
	{
		//kprintf("x%x",addr/PAGE_SIZE);
		pmm_mark_used(addr/PAGE_SIZE);
		addr += 0x1000;
	}
	/*
	* the multiboot structures and the mb_info shouldn't be overwritten either. their maximum allowed size is one Page
	*/
	//struct multiboot_module* modules = (void *)((uintptr_t)(mb_info->mbs_mods_addr) & 0xffffffff);
	pmm_mark_used((uintptr_t)(mb_info)/PAGE_SIZE);
	pmm_mark_used((uintptr_t)(modules_glob)/PAGE_SIZE);
	
	/* Und die Multibootmodule selber sind auch belegt */
	int i;
	
	for (i = 0; i < mb_info->mbs_mods_count; i++)
	{
		addr = modules_glob[i].mod_start;
		while (addr < modules_glob[i].mod_end) 
		{
			pmm_mark_used(addr/PAGE_SIZE);
			addr += PAGE_SIZE;
		}
	}
	
	pmm_mark_used(0x0);//0x0000000 is reserved for NULL pointers
	memset(0x00000000,0x00000000,PAGE_SIZE);
	pmm_mark_used( DIV_PAGE_SIZE(TMP_PAGEBUF) 			);//0x1000 is reserved,because that's,where we tmp map our pagetables to
	
	pmm_mark_used( DIV_PAGE_SIZE(TRAMPOLINE)			);// Trampoline space
	for (i = 0; i<DIV_PAGE_SIZE(STDRD_STACKSIZ); i++)
		pmm_mark_used( DIV_PAGE_SIZE(BSP_STACK) +i);		  // That's our Stack which is still the MB Loader
#ifdef ARCH_X86_64
	
	pmm_mark_used( DIV_PAGE_SIZE(INIT_PAGE_TBL_ADDR)		);
	pmm_mark_used( DIV_PAGE_SIZE(INIT_PAGEDIR_TBL_ADDR)		);
	pmm_mark_used( DIV_PAGE_SIZE(INIT_PAGEDIRPTR_TBL_ADDR)		);
	pmm_mark_used( DIV_PAGE_SIZE(INIT_PAGEMAPLEVEL4_TBL_ADDR)	);
	pmm_mark_used( DIV_PAGE_SIZE(GDT_X86_64) );
#endif
	pmm_init_stack();
	//while(1);
	kprintf("SUCCESS\n");
}

uint_t pmm_malloc_4k()
{
	spinlock_ackquire(&pmm_lock);
	uint_t page_to_alloc;
	
	do page_to_alloc = pmm_pop_stack();
	while(pmm_is_alloced(page_to_alloc) || !page_to_alloc );
	
	pmm_mark_used(page_to_alloc);
	
	spinlock_release(&pmm_lock);
	if(page_to_alloc == 0x12a) kprintf("AHA");
	return page_to_alloc;
}/*

These functions are DEPRECATED

uint_t pmm_malloc(uint_t pages)
{
	int i, j,k;
	spinlock_ackquire(&pmm_lock);
	for (i = 0; i < (BITMAP_SIZE*32)-pages; i++) 
	{
mark:	
		for(j=0;j<pages;j++)
			if (pmm_is_alloced(i+j)==TRUE) 
			{
				//if(i+j==0x4d){
				//	kprintf("i+j is alloced 0x%x",i+j);
				//}
				i++;
				goto mark;
			} 
			else if(j+1==pages)
			{
				for(k =0;k<pages;k++)
					pmm_mark_used(i+k);
				spinlock_release(&pmm_lock);
				return i;
			}
	}
	spinlock_release(&pmm_lock);
	kprintf("[PMM] found no free memory at: %x");
	return 0x0;
}
bool pmm_realloc(uint_t index, uint_t pages)
{
	int j,k;
	spinlock_ackquire(&pmm_lock); 
	for(j=0;j<pages;j++) 
	{
		if (pmm_is_alloced(index+j)==TRUE) 
			goto something_not_free;
		else if(j==pages-1)
		{
			for(k =0;k<pages;k++)
				pmm_mark_used(index+k);
			spinlock_release(&pmm_lock);
			return TRUE;
		}
	}
something_not_free:
	spinlock_release(&pmm_lock);
	kprintf("[PMM] W: pmm_realloc found no free memory");
	return FALSE;
}*/

void pmm_free_4k_unsafe(uint_t page)
{
	physbitmap[page /32] |= ( 1 << (page % 32) );
	pmm_push_stack(page);
}
void pmm_free_4k_glob(uint_t page)
{
	spinlock_ackquire(&pmm_lock);
	pmm_free_4k_unsafe(page);
	spinlock_release(&pmm_lock);
}
static bool pmm_is_alloced(uint_t page)
{
	if(physbitmap[page /32] & (1<<(page % 32)))
		return FALSE;
	return TRUE;
}
bool pmm_is_alloced_glob(uint_t page)
{
	spinlock_ackquire(&pmm_lock);
	if(physbitmap[page /32] & (1<<(page % 32)))
	{
		spinlock_release(&pmm_lock);
		return FALSE;
	}
	spinlock_release(&pmm_lock);
	return TRUE;
}

static void pmm_mark_used(uint_t page)
{
	physbitmap[page /32 ] &= ~(1 << (page % 32));
}

/*
 * I combined the Bitmap with a stack
 */

static uint_t pmm_malloc_4k_no_stack()
{
	int i;
	for (i = 0; i < BITMAP_SIZE*32 ; i++)
	{
		if (!pmm_is_alloced(i)) 
		{
			pmm_mark_used(i);	
			return i;
		}
	}
	kprintf("[PMM] E: pmm_malloc_4k found no free memory");
	/* Scheint wohl nichts frei zu sein... */
	return 0x0;
}
static void pmm_push_stack(uint_t to_push)
{
	if((uintptr_t)pmm_stack_ptr >  (uintptr_t)( &(pmm_stack[0])  ))
	{
		pmm_stack_ptr--;
		*pmm_stack_ptr = to_push;
	}// else kprintf("[PMM] E: pmm_push_stack can't push\n");
}
static uint_t pmm_pop_stack() 
{
	uint_t * val = pmm_stack_ptr;
	
	pmm_stack_ptr++;
	if( pmm_stack_ptr > &pmm_stack[PMM_STACK_SIZE-1] )
	{
		kprintf("[PMM] I: pmm_pop_stack reinitializes pmm_stack ...");
		pmm_init_stack();
		kprintf(" SUCCESS\n");
	//	kprintf("pmm_stack_ptr 0x%p", pmm_stack_ptr);
	//	kprintf("pmm_stack 0x%p", &pmm_stack[PMM_STACK_SIZE]);
		val = pmm_stack_ptr;
	}
	return *val;
}
static void pmm_init_stack() 
{
	int i;
	for(i =0; i<PMM_STACK_SIZE;i++)
		pmm_stack[i] = pmm_malloc_4k_no_stack();
	pmm_stack_ptr = &pmm_stack[PMM_STACK_SIZE];
	for(i=PMM_STACK_SIZE-1; i!=0; i--) 
		pmm_free_4k_unsafe(pmm_stack[i]);
}
