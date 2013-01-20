#include <stdint.h>
#include <string.h>
#include <mm/pmm.h>
#include <boot/multiboot.h>

/*
 * Der Einfachheit halber deklarieren wir die maximal benoetige Bitmapgroesse
 * statisch (Wir brauchen 4 GB / 4 kB = 1M Bits; 1M Bits sind 1M/32 = 32k
 * Eintraege fuer das Array)
 *
 * Willkuerliche Festlegung: 1 = Speicher frei, 0 = Speicher belegt
 */
#define BITMAP_SIZE 32768
static uint32_t bitmap[BITMAP_SIZE];

static void pmm_mark_used(void* page);

extern const void kernel_start;
extern const void kernel_end;

void pmm_init(multiboot_info* mb_info)
{
    struct multiboot_mmap* mmap = mb_info->mbs_mmap_addr;
    struct multiboot_mmap* mmap_end = (void*)
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
    
}

void* pmm_alloc(void)
{
    int i, j;

    /*
     * Zunaechst suchen wir komplette Eintraege ab. Wenn der Eintrag nicht null
     * ist, ist mindestens ein Bit gesetzt, d.h. hier ist ein Stueck Speicher
     * frei
     */
    for (i = 0; i < BITMAP_SIZE; i++) {
        if (bitmap[i] != 0) {

            /* Jetzt muessen wir nur noch das gesetzte Bit finden */
            for (j = 0; j < 32; j++) {
	      /*
		for(k = 0;k<4096%(size*(sizeof(size)/4));k++){
		    if (bitmap[i] & (1 << j)) {
			bitmap[i] &= ~(1 << j);
		    }
		    else{
			contin
		    }
		}*/
                if (bitmap[i] & (1 << j)) {
                    bitmap[i] &= ~(1 << j);
                    return (void*)( (i * 32 + j) * 4096);
                }
            }
        }
    }

    /* Scheint wohl nichts frei zu sein... */
    return NULL;
}

static void pmm_mark_used(void* page)
{
    uintptr_t index = (uintptr_t) page / 4096;
    bitmap[index / 32] &= ~(1 << (index % 32));
}

void pmm_free(void* page)
{
    uintptr_t index = (uintptr_t) page / 4096;
    bitmap[index / 32] |= (1 << (index % 32));
}