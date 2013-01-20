#ifndef PMM_H
#define PMM_H

#include <boot/multiboot.h>

void pmm_init(multiboot_info* mb_info);
void* pmm_alloc(void);
void pmm_free(void* page);

#endif