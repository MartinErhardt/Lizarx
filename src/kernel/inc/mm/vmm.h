/*   <src-path>/src/kernel/inc/mm/vmm.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef VMM_H
#define VMM_H

#include<stdbool.h>
#include<stdint.h>
#include <string.h>
#include <idt.h>
#include "pmm.h"
#include <hw_structs.h>

#define FLGCOMBAT_KERNEL	FLG_IN_MEM  | FLG_WRITABLE | FLG_WRITECACHING

#define FLGCOMBAT_USER  	FLG_IN_MEM | FLG_WRITECACHING | FLG_WRITABLE | FLG_USERACCESS

#define TMP_PAGEBUF 0x1000

#define KERNEL_SPACE 	0x4000000 //= 67 MB

#ifdef ARCH_X86_64
typedef struct vmm_pagemap_level4 * highest_paging_struct;
#endif

#ifdef ARCH_X86
typedef struct vmm_pagedir * highest_paging_struct;
#endif

typedef struct 
{
    highest_paging_struct highest_paging;
    struct vmm_tree_master* mm_tree;
} vmm_context;

vmm_context startup_context;
    
vmm_context vmm_init(void);
vmm_context vmm_crcontext();

vmm_context * get_cur_context();

void vmm_mark_used_inallcon(uint_t page);
int_t vmm_map_inallcon(uintptr_t phys, uintptr_t virt, uint8_t flgs);

void* kvmm_malloc(size_t size);
void* uvmm_malloc(vmm_context* context,size_t size);
void* vmm_malloc(vmm_context* context,size_t size, uintptr_t from,uintptr_t to,uint8_t flgs);
int_t vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs);
void vmm_free(vmm_context* context,void* page);
void kvmm_free(void* page);
bool vmm_is_alloced(vmm_context* context,uint_t page);

void* cpyin(void* src,size_t siz);
void* cpyout(void* src,size_t siz);

uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt);
uintptr_t phys_to_virt(vmm_context* context,uintptr_t phys);

uintptr_t vmm_find_freemem(vmm_context* context,size_t size, uintptr_t from,uintptr_t to);
#endif
