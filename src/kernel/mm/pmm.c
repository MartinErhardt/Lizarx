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
#include <boot/multiboot.h>
#include <dbg/console.h>
#include <drv/vga-txt_graphics/vram.h>
#include <boot/init.h>
/*
 * So far we implement the PMM as a flat Bitmap where each bit indicates one Page's status.
 * 0 = free space
 * 1 = used
 * PAGE_SIZE =0x1000(on AMD64 and i386 at least
 */

#define WORDWITH 32
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

static uint32_t physbitmap[BITMAP_SIZE];

//static void pmm_mark_used(void* page);

extern const void kernel_start;
extern const void kernel_end;

void pmm_init(struct multiboot_info * mb_info)
{
	
	struct multiboot_mmap* mmap = (void *)((uintptr_t)(mb_info->mbs_mmap_addr));
	struct multiboot_mmap* mmap_end = (void*)
	    ((uintptr_t) (mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length));
	/* by default everything is reserved */
	memset(&physbitmap, 0x00000000, sizeof(physbitmap));
	kprintf("[PMM] I: pmm_init ... ");
	
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
			uint64_t addr = mmap->base;
			uint64_t end_addr = addr + mmap->length;
			while ((addr/PAGE_SIZE < end_addr/PAGE_SIZE)&&(addr<0x100000000)) 
			{
				pmm_free(((uint32_t)addr)/PAGE_SIZE);
				addr += PAGE_SIZE;
			}
		}
		mmap++;
	}
	/* 
	 * here we mark the kernel as used
	 */
	uintptr_t addr = (uintptr_t) &kernel_start;
	
	while (addr < (uintptr_t) &kernel_end) 
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
			addr += 0x1000;
		}
	}
	
	pmm_mark_used(0x0);//0x0000000 is reserved for NULL pointers
	memset(0x00000000,0x00000000,PAGE_SIZE);
	pmm_mark_used(0x1);//0x1000 is reserved,because that's,where we tmp map our pagetables to
	
	pmm_mark_used(0x7);//0x1000 is reserved,because that's,where we tmp map our pagetables to
	pmm_mark_used(0x104);// That's our Stack which is still the MB Loader
	pmm_mark_used(0x105);// That's our Stack which is still the MB Loader
	kprintf("SUCCESS\n");
}

uint_t pmm_malloc_4k(void)
{
	int i;
	for (i = 0; i < BITMAP_SIZE*32; i++) 
	{ 
		if (pmm_is_alloced(i)==TRUE) 
		{
			continue;
		} 
		else 
		{
			pmm_mark_used(i);
			return i;
		}
	      
	}
	kprintf("[PMM] E: pmm_malloc_4k found no free memory");
	/* Scheint wohl nichts frei zu sein... */
	return 0x0;
}
uint_t pmm_malloc(uint_t pages)
{
	int i, j,k;
	for (i = 0; i < BITMAP_SIZE*32-pages; i++) 
	{
mark:
		for(j=0;j<pages;j++)
		{
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
				{
					//kprintf("mark 0x%x",i+k);
					pmm_mark_used(i+k);
				}
				return i;
			}
		}
	}
	kprintf("[PMM] found no free memory at: %x");
	/* Scheint wohl nichts frei zu sein... */
	return 0x0;
}
bool pmm_realloc(uint_t index, uint_t pages)
{
	int j,k;
	
	for(j=0;j<pages;j++) 
	{
		//kprintf("loop");
		if (pmm_is_alloced(index+j)==TRUE) 
		{
			return FALSE;
		} 
		else if(j==pages-1)
		{
			for(k =0;k<pages;k++)
			{
				pmm_mark_used(k);
			}
			return TRUE;
		}
	}
	kprintf("[PMM] W: pmm_realloc found no free memory");
	return FALSE;
}
bool pmm_is_alloced(uint_t page)
{
	if(physbitmap[page/32] & (1<<(page%32)))
	{
		return FALSE;
	}
	return TRUE;
}

void pmm_mark_used(uint_t page)
{
	physbitmap[page / 32] &= ~(1 << (page % 32));
}

void pmm_free(uint_t page)
{
	physbitmap[page / 32] |= (1 << (page % 32));
}
