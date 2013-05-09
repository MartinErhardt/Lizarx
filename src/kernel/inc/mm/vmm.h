/*   <src-path>/src/kernel/inc/mm/vmm.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef VMM_H
#define VMM_H

#include<stdbool.h>
#include<stdint.h>
#include <string.h>
#include <intr/idt.h>
#include <hal.h>
#include "pmm.h"

#define FLGCOMBAT_KERNEL	FLG_IN_MEM  | FLG_WRITABLE | FLG_WRITECACHING

#define FLGCOMBAT_USER  	FLG_IN_MEM | FLG_WRITECACHING | FLG_WRITABLE | FLG_USERACCESS

#define TMP_PAGEBUF 0x1000

#define KERNEL_SPACE 	0x1000000 //=256 MB

typedef struct 
{
    struct vmm_pagedir* pd;
    struct vmm_tree_master* tr;
} vmm_context;

vmm_context startup_context;
    
vmm_context vmm_init(void);
vmm_context vmm_crcontext();

void* kvmm_malloc(size_t size);
void* uvmm_malloc(vmm_context* context,size_t size);
void* vmm_malloc(vmm_context* context,size_t size, uintptr_t from,uintptr_t to,uint8_t flgs);
int_t vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs);
void vmm_free(vmm_context* context,void* page);
void kvmm_free(void* page);

int vmm_map(vmm_context* context, uintptr_t virt, uintptr_t phys,uint8_t flgs);
void vmm_unmap(vmm_context*context,uintptr_t virt);

void* cpyin(void* src,size_t siz);
void* cpyout(vmm_context* context,void* src,size_t siz);

uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt);
uintptr_t phys_to_virt(vmm_context* context,uintptr_t phys);

uintptr_t vmm_find_freemem(vmm_context* context,size_t size, uintptr_t from,uintptr_t to);
#endif
