#ifndef VMM_H
#define VMM_H

#include<stdbool.h>
#include<stdint.h>
#include <string.h>

/*
 * Flags for the Page Table and Page Directory
 */
#define FLG_IN_MEM 	0x01
#define FLG_WRITABLE 	0x02
#define FLG_USERACCESS	0x04
#define FLG_WRITECACHING	0x08
#define FLG_NOCACHE	0x10
//dirty access bits automatically set by CPU
#define FLG_USED		0x20
#define FLG_WRITTEN	0x40 // ignored in Page Directory Entry, if 4K Page
#define FLGPD_NOT4K	0x80 // only in Page Directory Entry

#define FLGCOMBAT_KERNEL	FLG_IN_MEM  | FLG_WRITABLE | FLG_WRITECACHING

#define FLGCOMBAT_USER  	FLG_IN_MEM | FLG_WRITECACHING | FLG_WRITABLE | FLG_USERACCESS

#define TMP_PAGEBUF 0x1000

#define KERNEL_SPACE 	0x10000000 //=256 MB

typedef struct {
    struct vmm_pagedir* pd;
    struct vmm_tree_master* tr;
} vmm_context;

vmm_context vmm_init(void);
vmm_context vmm_crcontext();
void* kvmm_malloc(size_t size);
void* uvmm_malloc(vmm_context* context,size_t size);
void* vmm_malloc(vmm_context* context,size_t size, uintptr_t from,uintptr_t to,uint8_t flgs);
int32_t vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs);
int vmm_map(vmm_context* context, uintptr_t virt, uintptr_t phys,uint8_t flgs);

int32_t vmm_free(vmm_context* context,void* page);
void vmm_set_context(vmm_context* context);

void* cpyin(void* src,size_t siz);
void* cpyout(vmm_context* context,void* src,size_t siz);

uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt);
uintptr_t phys_to_virt(vmm_context* context,uintptr_t phys);

uintptr_t vmm_find_freemem(vmm_context* context,uint32_t size, uintptr_t from,uintptr_t to);

vmm_context* get_startupcontext();
void set_startupcontext(vmm_context* context);
#endif