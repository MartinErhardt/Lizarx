/*  <src-path>/src/kernel/inc/mm/vmm.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
 *  Copyright (C) 2013, 2014  martin.erhardt98@googlemail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef VMM_H
#define VMM_H

#include<stdbool.h>
#include<stdint.h>
#include <string.h>
#include <idt.h>
#include "pmm.h"
#include <hw_structs.h>
#include <macros.h>

#define FLGCOMBAT_KERNEL	FLG_IN_MEM  | FLG_WRITABLE | FLG_WRITECACHING

#define FLGCOMBAT_USER  	FLG_IN_MEM | FLG_WRITECACHING | FLG_WRITABLE | FLG_USERACCESS

#define TMP_PAGEBUF 0x1000
#define TMP_PAGEBUF2 0x2000
#define TMP_PAGEBUF3 0x3000
#define TMP_PAGEBUF4 0x4000

#define DIV_PAGE_SIZE(VAL) ((VAL)>>12)
#define MUL_PAGE_SIZE(VAL) ((VAL)<<12)

typedef struct vmm_paging_entry * highest_paging_struct;

lock_t vmm_lock;
struct stack_entry
{
	uintptr_t start;
	uintptr_t end;
};
typedef struct
{
	highest_paging_struct highest_paging;
	struct vmm_paging_entry * other_first_tables[PAGING_HIER_SIZE -1];
	struct stack_entry * mm_stack;
	struct stack_entry * mm_stack_ptr;
} vmm_context;

vmm_context startup_context;
uint_t cores_invalidated;
lock_t invld_lock;
struct to_invalid * to_invalidate_first;
void sync_addr_space();
void vmm_init(struct multiboot_info * mb_info);
vmm_context vmm_crcontext();
vmm_context * get_cur_context_glob();
void vmm_map_range_glob(vmm_context*context,uintptr_t start, uintptr_t end,uint8_t flgs);
void vmm_delcontext(vmm_context* to_del);
void* kvmm_malloc(size_t size);
void* uvmm_malloc(vmm_context* context,size_t size);
void vmm_free(vmm_context* context,void* to_free, size_t size);
void kvmm_free(void* to_free,size_t size);//FIXME No Overflow check
void uvmm_push_free_glob(vmm_context* context,uint_t page, size_t length);
void * vmm_malloc(vmm_context * context, size_t size, bool kernel, uint8_t flags);
void* cpyin(void* src,size_t siz);
void* cpyout(void* src,size_t siz);
uintptr_t vmm_share_to_user(uintptr_t in_kernel,size_t length);
uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt);
void vmm_map_startup_glob(uintptr_t phys, uintptr_t virt, uint8_t flgs);
void make_guard_page(uintptr_t at, vmm_context * context);
#endif

