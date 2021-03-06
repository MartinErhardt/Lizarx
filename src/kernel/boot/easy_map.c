/*  <src-path>/src/kernel/boot/easy_map.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include <boot/easy_map.h>
#include <hw_structs.h>
#include <macros.h>
#include <stdint.h>
#include <mm/pmm.h>
#include <dbg/console.h>
#include <string.h>

#define FLGCOMBAT_INIT	FLG_IN_MEM  | FLG_WRITABLE | FLG_NOCACHE
#define EASYMAP(ENTRY, PTR) PTR; ENTRY->rw_flags=FLGCOMBAT_INIT
extern const void loader_start;
extern const void loader_end;
void init_easymap()
{
	struct vmm_pagemap_level4 * new_map_lvl4	= (struct vmm_pagemap_level4 *) easy_map_tbl;
	struct vmm_pagedir_ptrtbl * new_pd_ptrtbl	= (struct vmm_pagedir_ptrtbl * ) (easy_map_tbl+0x1000);
	struct vmm_pagedir * new_pd			= (struct vmm_pagedir * ) (easy_map_tbl+0x2000);
	struct vmm_pagetbl * cur_pagetbl		= (struct vmm_pagetbl *) (easy_map_tbl+0x3000);
	//struct vmm_pagetblentr * new_pagetbl=(struct vmm_pagetbl * ) INIT_PAGE_TBL_ADDR;
	uint_t i,j;
	memset((void*)new_map_lvl4,0x00000000,PAGE_SIZE);// clear the PgDIR to avoid invalid values
	new_map_lvl4->pagedirptrtbl_ptr			= EASYMAP(new_map_lvl4, (easy_map_tbl+0x1000)/PAGE_SIZE);
	
	memset((void*)new_pd_ptrtbl,0x00000000,PAGE_SIZE);
	new_pd_ptrtbl->pagedir_ptr			= EASYMAP(new_pd_ptrtbl, (easy_map_tbl+0x2000)/PAGE_SIZE);
	
	memset((void*)new_pd,0x00000000,PAGE_SIZE);
	for(i=easy_map_tbl/0x1000+3;i<(easy_map_tbl/0x1000+515);i++)
	{
		//kprintf("writting 0x%x", i);
		//kprintf("at 0x%x", ()
		new_pd->pagetbl_ptr=EASYMAP(new_pd,i);
		new_pd++;
	}
	memset((void*)(easy_map_tbl+0x3000),0x00000000,PAGE_SIZE*512);
	for(i=0;i<512;i++)
		for(j=0;j<512;j++)
		{
			cur_pagetbl->page_ptr=EASYMAP(cur_pagetbl,i*512+j);
			cur_pagetbl++;
		}
}
