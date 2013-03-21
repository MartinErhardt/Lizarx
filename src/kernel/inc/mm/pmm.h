#ifndef PMM_H
#define PMM_H

#include <boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

#define BITMAP_SIZE 32768
#define PAGE_SIZE 4096 // page size in bytes

void pmm_test(void);
void pmm_init(multiboot_info* mb_info);
void* pmm_malloc_4k(void);
void* pmm_malloc(size_t size);
void pmm_free(void* page);
bool pmm_realloc(void* ptr, size_t size);
bool pmm_is_alloced(uint32_t page);
void pmm_mark_used(void* page);
#endif
