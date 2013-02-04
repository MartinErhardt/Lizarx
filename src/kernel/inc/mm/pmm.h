#ifndef PMM_H
#define PMM_H

#include <boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

void kpmm_test(void);
void kpmm_init(multiboot_info* mb_info);
void* kpmm_malloc_4k(void);
void* kpmm_malloc(size_t size);
void kpmm_free(void* page);
bool kpmm_realloc(void* ptr, size_t size);

#endif
