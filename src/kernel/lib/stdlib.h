/*  <src-path>/src/kernel/lib/stdlib.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef STDLIB_H
#define STDLIB_H
#include<stdint.h>
#include"mm/vheap.h"

char * itoa(unsigned long n, unsigned int base);

static inline void* malloc (size_t size) { return kmalloc(size); }
static inline void* calloc (size_t size) { return kcalloc(size); }
static inline void free (void* ptr) { kfree(ptr); }
static inline void*realloc(void* ptr, size_t size) { return krealloc(ptr,size); }

#endif
