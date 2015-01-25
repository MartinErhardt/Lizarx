/*  <src-path>/src/kernel/mm/pmm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include <hw_structs.h>
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
#define EASYMAPTBL_SIZ 0x4000
#define ALIGNED_BASE(BASE) BASE % 0x1000 ? ( BASE +0x1000- BASE %0x1000) : BASE

uint_t pmm_stack[0x200000];
uint_t * pmm_stack_ptr = &pmm_stack[0];
#ifndef LOADER
static uint32_t physbitmap[BITMAP_SIZE];
extern const void kernel_start;
extern const void kernel_end;
static void pmm_mark_used(uint_t page);
#endif
void pmm_init_mmap(struct multiboot_info * mb_info)
{
	struct multiboot_mmap* mmap = (void *)((uintptr_t)(mb_info->mbs_mmap_addr));
	struct multiboot_mmap* mmap_end = (void*)
	    ((uintptr_t) (mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length));
	//trampoline=0;
	bsp_stack=0;
	easy_map_tbl=0;
        while (mmap < mmap_end)
        {
                if (mmap->type == 1)
                {
#ifdef LOADER
                        if( mmap->base <0x1000)
			{
				if (mmap->base+mmap->length<=0x1000)
				{
					mmap->type=0;
					continue;
				}
				else
				{
					mmap->length= mmap->base+mmap->length-0x1000;
					mmap->base=0x1000;
				}
			}
#endif

			/*
			if( ((mmap->base+mmap->length)&0xfffff000)-(ALIGNED_BASE(mmap->base))>=0x1000 && !trampoline)
			{
				trampoline=mmap->base&0xfffff000;
				if(change)
				{
					mmap->base+=0x1000;
					mmap->base=ALIGNED_BASE(mmap->base);
				}
                        }*/
#ifdef ARCH_X86_64
			if( ((mmap->base+mmap->length)&0xfffff000)-(ALIGNED_BASE(mmap->base))>=EASYMAPTBL_SIZ+STDRD_STACKSIZ && !easy_map_tbl)
			{
				easy_map_tbl=mmap->base&0xfffff000;
				bsp_stack=easy_map_tbl+EASYMAPTBL_SIZ;
#ifndef LOADER
				mmap->base+=EASYMAPTBL_SIZ+STDRD_STACKSIZ;
				mmap->base= ALIGNED_BASE(mmap->base);
#endif
                        }
#endif
		}
                mmap++;
        }
	
}
#ifndef LOADER
void pmm_init(struct multiboot_info * mb_info)
{
	struct multiboot_mmap* mmap = (void *)((uintptr_t)(mb_info->mbs_mmap_addr));
	struct multiboot_mmap* mmap_end = (void*)
	    ((uintptr_t) (mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length));
	uintptr_t end_addr;
	int i;
	kprintf("[PMM] I: pmm_init ... ");
	/* by default everything is free */
	memset(&physbitmap, 0xffffffff, sizeof(physbitmap));
	pmm_stack_ptr = &pmm_stack[PMM_STACK_SIZE];
	/*
	* Here we look in the memory map for free space
	*/
	uintptr_t addr = (uintptr_t) &kernel_start;
	
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
		pmm_mark_used( DIV_PAGE_SIZE(bsp_stack) +i);		  // That's our Stack which is still the MB Loader
	//pmm_mark_used(DIV_PAGE_SIZE(trampoline));
	pmm_mark_used(DIV_PAGE_SIZE(easy_map_tbl));
	pmm_mark_used(DIV_PAGE_SIZE(easy_map_tbl)+1);
#ifdef ARCH_X86_64
	pmm_mark_used(DIV_PAGE_SIZE(easy_map_tbl)+2);
	pmm_mark_used(DIV_PAGE_SIZE(easy_map_tbl)+3);
#endif
	pmm_mark_used(0xb8);
	pmm_mark_used(0xf0);
	addr=0;
	i=0;
	pmm_stack_ptr = &pmm_stack[0];

	while (mmap < mmap_end) 
	{
		if (mmap->type == 1)
		{
			/* 
			 * We and the addresses with 0xffffffff to prevent a buffer overflow
			 * FIXME our Bitmap only covers a 32 bit address space
			 */
			addr = ALIGNED_BASE(mmap->base);
			end_addr = addr +( mmap->length & 0xfffff000);
			while(addr<end_addr)
			{
				if(physbitmap[(addr/0x1000) /32] & (1<<((addr/0x1000) % 32)))
				{
					i++;
					*pmm_stack_ptr=addr;
					pmm_stack_ptr++;
				}
				addr+=0x1000;
			}

		}
		mmap++;
	}
	pmm_stack_ptr = &pmm_stack[0];
	kprintf("SUCCESS\n");
}

uint_t pmm_malloc_4k()
{
	spinlock_ackquire(&pmm_lock);
	uintptr_t to_ret=*pmm_stack_ptr;
	pmm_stack_ptr++;
	pmm_mark_used(to_ret/0x1000);
//	kprintf("value: 0x%x", to_ret);
	spinlock_release(&pmm_lock);
	return to_ret/0x1000;
}

void pmm_free_4k_glob(uint_t page)
{
	spinlock_ackquire(&pmm_lock);
	pmm_stack_ptr--;
	*pmm_stack_ptr=page*0x1000;
	spinlock_release(&pmm_lock);
}
void pmm_free_4k_unsafe(uint_t page)
{
	pmm_stack_ptr--;
	*pmm_stack_ptr=page*0x1000;
}/*
static bool pmm_is_alloced(uint_t page)
{
	return (physbitmap[page /32] & (1<<(page % 32))) ? FALSE : TRUE;
}*/
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
#endif
