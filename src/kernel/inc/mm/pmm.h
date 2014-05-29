/*   <src-path>/src/kernel/inc/mm/pmm.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef PMM_H
#define PMM_H

#include <boot/multiboot.h>
#include <stdint.h>
#include <stdbool.h>
#include <libOS/lock.h>

lock_t pmm_lock;

void pmm_init(struct multiboot_info* mb_info);
uint_t pmm_malloc_4k(void);
uint_t pmm_malloc(uint_t pages);
bool pmm_realloc(uint_t index, uint_t pages);

void pmm_free(uint_t page, uint_t n);
void pmm_free_4k_glob(uint_t page);
void pmm_free_4k_unsafe(uint_t page);
bool pmm_is_alloced_glob(uint_t page);

#endif
