 /*   <src-path>/src/kernel/lib/libOS/list.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef LIST_H
#define LIST_H

#include<stdint.h>

#define GET_OFF(STRUCT, STRUCT_ENTRY) ((uintptr_t) STRUCT_ENTRY) - ((uintptr_t)STRUCT)
#define ENTRYS_PER_ARRAY 5

struct alist_st
{
	struct alist_st * next;
	uint8_t used;
	void * pointer[ENTRYS_PER_ARRAY];
};
typedef struct alist_st alist_t;

void * alist_get_by_index(alist_t * list, uint_t index);
int_t alist_get_index(alist_t * list, void * entry);
void * alist_get_by_entry8(alist_t * list, uint_t off, uint8_t val);
void * alist_get_by_entry(alist_t * list, uint_t off, uint_t val);
void alist_add(alist_t * list, void * entry);
int alist_remove(alist_t * list, void * entry);
uint_t alist_get_entry_n(alist_t * list);

#endif
