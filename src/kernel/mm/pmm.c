/*   <src-path>/src/kernel/mm/pmm.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <mm/pmm.h>
#include <boot/multiboot.h>
#include <dbg/console.h>
#include <drv/vga-txt_graphics/vram.h>
/*
 * Der Einfachheit halber deklarieren wir die maximal benoetige Bitmapgroesse
 * statisch (Wir brauchen 4 GB / 4 kB = 1M Bits; 1M Bits sind 1M/32 = 32k
 * Eintraege fuer das Array)
 *
 * Willkuerliche Festlegung: 1 = Speicher frei, 0 = Speicher belegt
 */

#define WORDWITH 32

static uint32_t physbitmap[BITMAP_SIZE];

//static void pmm_mark_used(void* page);

extern const void kernel_start;
extern const void kernel_end;

void pmm_init(struct multiboot_info * mb_info)
{
    struct multiboot_mmap* mmap = mb_info->mbs_mmap_addr;
    struct multiboot_mmap* mmap_end = (void*)
        ((uintptr_t) mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length);
    /* Per Default ist erst einmal alles reserviert */
    memset(physbitmap, 0x00000000, sizeof(physbitmap));
    kprintf("[PMM] I: PMM setup ... ");
    /*
     * Nur, was die BIOS-Memory-Map als frei bezeichnet, wird wieder als frei
     * markiert
     */
    while (mmap < mmap_end) {
        if (mmap->type == 1) {
            /* Der Speicherbereich ist frei, entsprechend markieren */
            uintptr_t addr = mmap->base;
            uintptr_t end_addr = addr + mmap->length;
	    
            while (addr < end_addr) {
                pmm_free(addr/PAGE_SIZE);
                addr += 0x1000;
            }
        }
        mmap++;
	
    }

    /* Den Kernel wieder als belegt kennzeichnen */
    uintptr_t addr = (uintptr_t) &kernel_start;
    while (addr < (uintptr_t) &kernel_end) {
        pmm_mark_used(addr/PAGE_SIZE);
        addr += 0x1000;
    }
        /*
     * Die Multibootstruktur auch, genauso wie die Liste von Multibootmodulen.
     * Wir gehen bei beiden davon aus, dass sie maximal 4k gross werden
     */
    struct multiboot_module* modules = mb_info->mbs_mods_addr;

    pmm_mark_used((uintptr_t)(mb_info)/PAGE_SIZE);
    pmm_mark_used((uintptr_t)(modules)/PAGE_SIZE);

    /* Und die Multibootmodule selber sind auch belegt */
    int i;
    for (i = 0; i < mb_info->mbs_mods_count; i++) {
        addr = modules[i].mod_start;
        while (addr < modules[i].mod_end) {
            pmm_mark_used(addr/PAGE_SIZE);
            addr += 0x1000;
        }
    }
    
    pmm_mark_used(0x0);//0x0000000 is reserved for NULL pointers
    memset(0x00000000,0x00000000,PAGE_SIZE);
    pmm_mark_used(0x1);//0x1000 is reserved,because that's,where we tmp map our pagetables to
    pmm_mark_used(0xb8);//mark video ram as used
    /*
    for(i=0;i<4096;i++){
	kprintf("%x",pmm_is_alloced(i+50000));
    }*/
    kprintf("SUCCESS\n");
}

uint_t pmm_malloc_4k(void)
{
    int i;

    for (i = 0; i < BITMAP_SIZE*32; i++) {
      
	if (pmm_is_alloced(i)==TRUE) {
	    continue;
	} else {
	    pmm_mark_used(i);
	    return i;
	}
	
    }
    kprintf("[PMM] E: pmm_malloc_4k found no free memory");
    /* Scheint wohl nichts frei zu sein... */
    return 0x0;
}
uint_t pmm_malloc(uint32_t pages)
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
bool pmm_realloc(uint32_t index, uint32_t pages)
{
    int j,k;
    
    for(j=0;j<pages;j++) {
	    //kprintf("loop");
	    if (pmm_is_alloced(index+j)==TRUE) {
		return FALSE;
	    } else if(j==pages-1){
		for(k =0;k<pages;k++){
		    pmm_mark_used(k);
		}
		return TRUE;
	    }
    }
    kprintf("[PMM] W: pmm_realloc found no free memory");
    return FALSE;
}
bool pmm_is_alloced(uint32_t page)
{
    if(physbitmap[page/32] & (1<<(page%32))){
	return FALSE;
    }
    return TRUE;
}

void pmm_mark_used(uint32_t page)
{
    physbitmap[page / 32] &= ~(1 << (page % 32));
}

void pmm_free(uint32_t page)
{
    physbitmap[page / 32] |= (1 << (page % 32));
}
