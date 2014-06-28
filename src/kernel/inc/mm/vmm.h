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
#include <macros.h>

#define FLGCOMBAT_KERNEL	FLG_IN_MEM  | FLG_WRITABLE | FLG_WRITECACHING

#define FLGCOMBAT_USER  	FLG_IN_MEM | FLG_WRITECACHING | FLG_WRITABLE | FLG_USERACCESS

#define TMP_PAGEBUF 0x1000

#define KERNEL_SPACE	0x4000000 //= 67 MB

#define DIV_PAGE_SIZE(VAL) ((VAL)>>12)
#define MUL_PAGE_SIZE(VAL) ((VAL)<<12)

typedef struct vmm_paging_entry * highest_paging_struct;
lock_t vmm_lock;
typedef struct
{
	highest_paging_struct highest_paging;
	struct vmm_paging_entry * other_first_tables[PAGING_HIER_SIZE -1];
	struct vmm_tree_master* mm_tree;
} vmm_context;

vmm_context startup_context;
uint_t cores_invalidated;

lock_t invld_lock;
struct to_invalid * to_invalidate_first;

void sync_addr_space();
vmm_context vmm_init(void);
vmm_context vmm_crcontext();

vmm_context * get_cur_context_glob();
void vmm_delcontext(vmm_context* to_del);
void* kvmm_malloc(size_t size);
void* uvmm_malloc(vmm_context* context,size_t size);
int_t vmm_realloc(vmm_context* context,void* ptr, size_t size,uint8_t flgs);
void vmm_free(vmm_context* context,void* to_free, size_t size);
void kvmm_free(void* to_free,size_t size);//FIXME No Overflow check
bool vmm_is_alloced_glob(vmm_context* context,uint_t page);
void * vmm_malloc(vmm_context * context, size_t size, uintptr_t from, uintptr_t to, uint8_t flags);

void* cpyin(void* src,size_t siz);
void* cpyout(void* src,size_t siz);
uintptr_t vmm_share_mem(void* aim_proc, void * origin_proc, uintptr_t segment_addr_in_origin, size_t segment_size);

uintptr_t virt_to_phys(vmm_context* context,uintptr_t virt);

uintptr_t vmm_find_freemem_glob(vmm_context* context,size_t size, uintptr_t from,uintptr_t to);
void vmm_mark_used_inallcon_glob(uint_t page);
void vmm_map_inallcon_glob(uintptr_t phys, uintptr_t virt, uint8_t flgs);

#endif

