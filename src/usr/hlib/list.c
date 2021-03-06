/*  <src-path>/src/usr/hlib/list.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include"./list.h"
#include<sys/types.h>

void * alist_get_by_index(alist_t * list, unsigned long index)
{
	int i;
	int j=0;
	do{
		for(i=0;i<ENTRYS_PER_ARRAY;i++)
			if(list->pointer[i])
			{
				if(j==index)
					return list->pointer[i];
				j++;
			}
		list = list->next;
	} while(list);
	return NULL;
}
long alist_get_index(alist_t * list, void * entry)
{
	int i;
	long j = 0;
	do{
		for(i=0;i<ENTRYS_PER_ARRAY;i++)
			if(list->pointer[i] == entry)
				return j;
			else if(list->pointer[i]) j++;
		list = list->next;
	} while(list);
	return -1;
}
void * alist_get_by_entry8(alist_t * list, off_t off, uint8_t val)
{
	int i;
	do{
		for(i=0;i<ENTRYS_PER_ARRAY;i++)
			if(list->pointer[i] && *((uint8_t*)(((uintptr_t)list->pointer[i])+off)) == val)
				return list->pointer[i];
		list = list->next;
	} while(list);
	return NULL;
}
void * alist_get_by_entry(alist_t * list, off_t off, unsigned long val)
{
	int i;
	do{
		for(i=0;i<ENTRYS_PER_ARRAY;i++)
			if(list->pointer[i] && *((unsigned long*)(((uintptr_t)list->pointer[i])+off)) == val)
				return list->pointer[i];
		list = list->next;
	} while(list);
	return NULL;
}
void alist_add(alist_t * list, void * to_add)
{
	int i;
	while(list->used == ENTRYS_PER_ARRAY)
		list = list->next;
	for(i=0;i<ENTRYS_PER_ARRAY;i++)
		if(!list->pointer[i])
		{
			list->pointer[i] = to_add;
			list->used++;
			if((list->used == ENTRYS_PER_ARRAY) && list->next == NULL)
			{
				list->next = (alist_t *)malloc(sizeof(struct alist_st));
				memset((void *)list->next,0x0,sizeof(alist_t));
			}
			return;
		}
}
int alist_remove(alist_t * list, void * to_remove)
{
	int i=0;
	do{
		for(i=0;i<ENTRYS_PER_ARRAY;i++)
			if(list->pointer[i] == to_remove)
			{
				list->pointer[i] = NULL;
				list->used--;
				return 0;
			}
		list = list->next;
	} while(list);
	return -1;
}
unsigned long alist_get_entry_n(alist_t * list)
{
	unsigned long entry_n = 0;
	do{
		entry_n+= list->used;
		list = list->next;
	} while(list);
	return entry_n;
}
