#ifndef PMM_H
#define PMM_H

#include <boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

void kpmm_test(void);
void kpmm_init(multiboot_info* mb_info);
void* kmalloc_4k(void);
void* kmalloc(size_t size);
void kfree(void* page);
bool krealloc(void* ptr);

#endif
