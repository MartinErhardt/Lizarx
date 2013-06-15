/*   <src-path>/src/kernel/boot/easy_map.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <boot/easy_map.h>
#include <hal.h>
#include <stdint.h>
#include <mm/pmm.h>
#include <dbg/console.h>

#define FLGCOMBAT_INIT	FLG_IN_MEM  | FLG_WRITABLE | FLG_NOCACHE

#define INIT_PAGE_TBL_ADDR		0x113000
#define INIT_PAGEDIR_TBL_ADDR		0x112000
#define INIT_PAGEDIRPTR_TBL_ADDR	0x111000
#define INIT_PAGEMAPLEVEL4_TBL_ADDR	0x110000

extern const void loader_start;
extern const void loader_end;

void init_easymap()
{
	struct vmm_pagemap_level4_entr * new_map_lvl4=(struct vmm_pagemap_level4_entr *) INIT_PAGEMAPLEVEL4_TBL_ADDR;
	struct vmm_pagedir_ptrtbl_entr * new_pd_ptrtbl=(struct vmm_pagedir_ptrtbl_entr * ) INIT_PAGEDIRPTR_TBL_ADDR;
	struct vmm_pagedirentr * new_pd=(struct vmm_pagedirentr * ) INIT_PAGEDIR_TBL_ADDR;
	//struct vmm_pagetblentr * new_pagetbl=(struct vmm_pagetblentr * ) INIT_PAGE_TBL_ADDR;
	long i =0;
	
	new_map_lvl4->pagedirptrtbl_ptr=INIT_PAGEDIRPTR_TBL_ADDR/PAGE_SIZE;
	new_map_lvl4->rw_flags=FLGCOMBAT_INIT;
	
	new_pd_ptrtbl->pagedir_ptr=INIT_PAGEDIR_TBL_ADDR/PAGE_SIZE;
	new_pd_ptrtbl->rw_flags=FLGCOMBAT_INIT;
	
	new_pd->pagetbl_ptr=INIT_PAGE_TBL_ADDR/PAGE_SIZE;
	new_pd->rw_flags=FLGCOMBAT_INIT;
	/*
	easymap(INIT_PAGE_TBL_ADDR);
	easymap(0xB8000);*/
	for(i=0;i<512;i++)
	{
		easymap(i*PAGE_SIZE);
		
	}
}
void easymap(uint32_t ptr)
{
	unsigned int ndx=ptr/PAGE_SIZE;
	struct vmm_pagetblentr * pagetbl=(struct vmm_pagetblentr * ) INIT_PAGE_TBL_ADDR;
	
	pagetbl[ndx].page_ptr=ptr/PAGE_SIZE;
	pagetbl[ndx].rw_flags=FLGCOMBAT_INIT;
	//kprintf("ndx = 0x%x map: 0x%x sizeof = 0x%x",ndx, ndx*0x8, sizeof(struct vmm_pagetblentr) );
}
