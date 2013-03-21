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

void pmm_test(){
      uint32_t i, j;
      size_t vlarge_puffer_size =PAGE_SIZE*20000;
      uint8_t vlarge_puffer_src[PAGE_SIZE];
      void * vlarge_puffer_ptr = pmm_malloc(vlarge_puffer_size);
      atrbyt font={0xF,0x0};
      kprintf("[PMM] I: test started ...");
      if(vlarge_puffer_ptr==NULL){
	  kprintf("FAILED: No memory found\n");
	  return;
      }
      
      for(i=0;i<PAGE_SIZE;i++){
	  vlarge_puffer_src[i] = 0xFF;
      }
      kprintf("...\nPAGE: ");
      for(j=0;j<vlarge_puffer_size/PAGE_SIZE;j++){
	  memcpy((void*)((uintptr_t) (vlarge_puffer_ptr)+j*PAGE_SIZE),&vlarge_puffer_src, PAGE_SIZE);
	  kprintn_scr(j,10,font);
	  setcurs(5,1);
      }
      
      kprintf("SUCCESS\n");
}
void pmm_init(multiboot_info* mb_info)
{
    multiboot_mmap* mmap = mb_info->mbs_mmap_addr;
    multiboot_mmap* mmap_end = (void*)
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
                pmm_free((void*) addr);
                addr += 0x1000;
            }
        }
        mmap++;
	
    }

    /* Den Kernel wieder als belegt kennzeichnen */
    uintptr_t addr = (uintptr_t) &kernel_start;
    while (addr < (uintptr_t) &kernel_end) {
        pmm_mark_used((void*) addr);
        addr += 0x1000;
    }
        /*
     * Die Multibootstruktur auch, genauso wie die Liste von Multibootmodulen.
     * Wir gehen bei beiden davon aus, dass sie maximal 4k gross werden
     */
    multiboot_module* modules = mb_info->mbs_mods_addr;

    pmm_mark_used(mb_info);
    pmm_mark_used(modules);

    /* Und die Multibootmodule selber sind auch belegt */
    int i;
    for (i = 0; i < mb_info->mbs_mods_count; i++) {
        addr = modules[i].mod_start;
        while (addr < modules[i].mod_end) {
            pmm_mark_used((void*) addr);
            addr += 0x1000;
        }
    }
    
    pmm_mark_used((void*)0x0);//0x0000000 is reserved for NULL pointers
    memset(0x00000000,0x00000000,PAGE_SIZE);
    pmm_mark_used((void*)0x1000);//0x1000 is reserved,because that's,where we tmp map our pagetables to
    
    /*
    for(i=0;i<4096;i++){
	kprintf("%x",pmm_is_alloced(i+50000));
    }*/
    kprintf("SUCCESS\n");
}

void* pmm_malloc_4k(void)
{
    int i;

    for (i = 0; i < BITMAP_SIZE*32; i++) {
      
	if (pmm_is_alloced(i)==TRUE) {
	    continue;
	} else {
	    pmm_mark_used((void*) (i*PAGE_SIZE));
	    return (void*)(i * PAGE_SIZE);
	}
	
    }
    kprintf("[PMM] E: pmm_malloc_4k found no free memory");
    /* Scheint wohl nichts frei zu sein... */
    return NULL;
}
void* pmm_malloc(size_t size)
{
    int i, j,k;
     /* size in 4kb(pages) we are adding one page coz we have to allocate pages which have been written on to 1% also*/
    if((size%PAGE_SIZE)==0){
	size=size/PAGE_SIZE;
    }else{
	size=size/PAGE_SIZE+1;
    }
    for (i = 0; i < BITMAP_SIZE*32-size; i++) {
      
mark:
	
	for(j=0;j<size;j++){
	    if (pmm_is_alloced(i+j)==TRUE) {
		i++;
		goto mark;
	    } else if(j==size-1){
		for(k =0;k<size;k++){
		    pmm_mark_used((void*) ((i+k)*PAGE_SIZE));
		}
		return (void*)(i * PAGE_SIZE);
	    }
	}
    }
    kprintf("[PMM] E: pmm_malloc found no free memory size: %d",size);
    /* Scheint wohl nichts frei zu sein... */
    return NULL;
}
bool pmm_realloc(void* ptr, size_t size)
{
    int j,k;
    uintptr_t page =(uintptr_t) ptr/PAGE_SIZE;
    if((size%PAGE_SIZE)==0){
	size=size/PAGE_SIZE;
    }else{
	size=size/PAGE_SIZE+1;
    }
    for(j=0;j<size;j++) {
	    kprintf("loop");
	    if (pmm_is_alloced((uint32_t )page+j)==TRUE) {
		return FALSE;
	    } else if(j==size-1){
		for(k =0;k<size;k++){
		    pmm_mark_used((void*) (k*PAGE_SIZE));
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

void pmm_mark_used(void* page)
{
    uintptr_t index = (uintptr_t) page / PAGE_SIZE;
    physbitmap[index / 32] &= ~(1 << (index % 32));
}

void pmm_free(void* page)
{
    uintptr_t index = (uintptr_t) page / PAGE_SIZE;
    physbitmap[index / 32] |= (1 << (index % 32));
}