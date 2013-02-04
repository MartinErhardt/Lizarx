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
#define BITMAP_SIZE 32768
#define PAGE_SIZE 4096 // page size in bytes
#define WORDWITH 32

static uint32_t bitmap[BITMAP_SIZE];

static void kpmm_mark_used(void* page);
static bool kpmm_is_alloced(uint32_t page);

extern const void kernel_start;
extern const void kernel_end;

void kpmm_test(){
      uint32_t i, j;
      size_t vlarge_puffer_size =PAGE_SIZE*20000;
      uint8_t vlarge_puffer_src[PAGE_SIZE];
      void * vlarge_puffer_ptr = kpmm_malloc(vlarge_puffer_size);
      atrbyt font={0xF,0x0};
      
      if(vlarge_puffer_ptr==NULL){
	  kprintf("[PMM TEST]FAILED: No memory found\n");
	  return;
      }
      
      for(i=0;i<PAGE_SIZE;i++){
	  vlarge_puffer_src[i] = 0xFF;
      }
      kprintf("[PMM TEST]STARTED ...\nPAGE: ");
      for(j=0;j<vlarge_puffer_size/PAGE_SIZE;j++){
	  memcpy((void*)((uintptr_t) (vlarge_puffer_ptr)+j*PAGE_SIZE),&vlarge_puffer_src, PAGE_SIZE);
	  kprintn_scr(j,10,font);
	  setcurs(5,1);
      }
}
void kpmm_init(multiboot_info* mb_info)
{
    multiboot_mmap* mmap = mb_info->mbs_mmap_addr;
    multiboot_mmap* mmap_end = (void*)
        ((uintptr_t) mb_info->mbs_mmap_addr + mb_info->mbs_mmap_length);

    /* Per Default ist erst einmal alles reserviert */
    memset(bitmap, 0, sizeof(bitmap));

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
                kpmm_free((void*) addr);
                addr += 0x1000;
            }
        }
        mmap++;
    }

    /* Den Kernel wieder als belegt kennzeichnen */
    uintptr_t addr = (uintptr_t) &kernel_start;
    while (addr < (uintptr_t) &kernel_end) {
        kpmm_mark_used((void*) addr);
        addr += 0x1000;
    }
        /*
     * Die Multibootstruktur auch, genauso wie die Liste von Multibootmodulen.
     * Wir gehen bei beiden davon aus, dass sie maximal 4k gross werden
     */
    multiboot_module* modules = mb_info->mbs_mods_addr;

    kpmm_mark_used(mb_info);
    kpmm_mark_used(modules);

    /* Und die Multibootmodule selber sind auch belegt */
    int i;
    for (i = 0; i < mb_info->mbs_mods_count; i++) {
        addr = modules[i].mod_start;
        while (addr < modules[i].mod_end) {
            kpmm_mark_used((void*) addr);
            addr += 0x1000;
        }
    }
}

void* kpmm_malloc_4k(void)
{
    int i;

    for (i = 0; i < BITMAP_SIZE*32; i++) {
      
	if (kpmm_is_alloced(i)==TRUE) {
	    continue;
	} else {
	    kpmm_mark_used((void*) (i*PAGE_SIZE));
	    return (void*)(i * PAGE_SIZE);
	}
	
    }

    /* Scheint wohl nichts frei zu sein... */
    return NULL;
}
void* kpmm_malloc(size_t size)
{
    int i, j,k;
     /* size in 4kb(pages) we are adding one page coz we have to allocate pages which have been written on to 1% also*/
    size=size/PAGE_SIZE+1;
    
    for (i = 0; i < BITMAP_SIZE*32-size; i++) {
      
mark:
	
	for(j=0;j<size;j++){
	    if (kpmm_is_alloced(i+j)==TRUE) {
		i++;
		goto mark;
	    } else if(j==size-1){
		for(k =0;k<size;k++){
		    kpmm_mark_used((void*) ((i+k) * PAGE_SIZE));
		}
		return (void*)(i * PAGE_SIZE);
	    }
	}
    }

    /* Scheint wohl nichts frei zu sein... */
    return NULL;
}
bool kpmm_realloc(void* ptr, size_t size)
{
    int j,k;
    uintptr_t page =(uintptr_t) ptr/PAGE_SIZE;
    size=size/PAGE_SIZE+1;
    
    for(j=0;j<size;j++) {
      
	    if (kpmm_is_alloced((uint32_t )page+j*PAGE_SIZE)==TRUE) {
		return FALSE;
	    } else if(j==size-1){
		for(k =0;k<size;k++){
		    kpmm_mark_used((void*) ((k) * PAGE_SIZE));
		}
		return TRUE;
	    }
    }
    return FALSE;
}
static bool kpmm_is_alloced(uint32_t page)
{
    if(bitmap[page/32] & (1<<(page%32))){
	return FALSE;
    }
    return TRUE;
}

static void kpmm_mark_used(void* page)
{
    uintptr_t index = (uintptr_t) page / PAGE_SIZE;
    bitmap[index / 32] &= ~(1 << (index % 32));
}

void kpmm_free(void* page)
{
    uintptr_t index = (uintptr_t) page / PAGE_SIZE;
    bitmap[index / 32] |= (1 << (index % 32));
}