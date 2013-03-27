/*   <src-path>/src/kernel/inc/mm/pmm.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef PMM_H
#define PMM_H

#include <boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>

#define BITMAP_SIZE 32768
#define PAGE_SIZE 4096 // page size in bytes

void pmm_init(multiboot_info* mb_info);
uint32_t pmm_malloc_4k(void);
uint32_t pmm_malloc(uint32_t pages);
bool pmm_realloc(uint32_t index, uint32_t pages);

void pmm_free(uint32_t page);
bool pmm_is_alloced(uint32_t page);
void pmm_mark_used(uint32_t page);

#endif
