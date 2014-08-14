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
#ifndef ALIST_H
#define ALIST_H
#include"../list.h"
#include<string.h>
#include<stdlib.h>
template <typename ElementType>
class AList
{
	alist_t list;
	unsigned long entrys_per_array;
	public:
		AList(unsigned long _entrys_per_array)
		{
			memset(&list,0,sizeof(alist_t));
			entrys_per_array=_entrys_per_array;
		};
		ElementType * alist_get_by_index(unsigned long index)
		{
			unsigned int i;
			unsigned long j=0;
			alist_t * curlist=&list;
			do{
				for(i=0;i<entrys_per_array;i++)
					if(curlist->pointer[i])
					{
						if(j==index)
							return (ElementType * )curlist->pointer[i];
						j++;
					}
				curlist = curlist->next;
			} while(curlist);
			return NULL;
		};
		long alist_get_index(ElementType * entry)
		{
			unsigned int i;
			long j = 0;
			alist_t * curlist = &list;
			do{
				for(i=0;i<entrys_per_array;i++)
					if(curlist->pointer[i] == entry)
						return j;
					else if(curlist->pointer[i]) j++;
				curlist = curlist->next;
			} while(curlist);
			return -1;
		};
		ElementType * alist_get_by_entry8(off_t off, uint8_t val)
		{
			unsigned int i;
			alist_t * curlist = &list;
			do{
				for(i=0;i<entrys_per_array;i++)
					if(curlist->pointer[i] && *((uint8_t*)(((uintptr_t)curlist->pointer[i])+off)) == val)
						return curlist->pointer[i];
				curlist = curlist->next;
			} while(curlist);
			return NULL;
		};
		ElementType * alist_get_by_entry( off_t off, unsigned long val)
		{
			unsigned int i;
			alist_t * curlist = &list;
			do{
				for(i=0;i<entrys_per_array;i++)
					if(curlist->pointer[i] && *((unsigned long*)(((uintptr_t)curlist->pointer[i])+off)) == val)
						return curlist->pointer[i];
				curlist = curlist->next;
			} while(curlist);
			return NULL;
		};
		void alist_add(ElementType * to_add)
		{
			unsigned int i;
			alist_t * curlist = &list;
			while(curlist->used == entrys_per_array)
				curlist = curlist->next;
			for(i=0;i<entrys_per_array;i++)
				if(!curlist->pointer[i])
				{
					curlist->pointer[i] = to_add;
					curlist->used++;
					if((curlist->used == entrys_per_array) && curlist->next == NULL)
					{
						curlist->next = (alist_t *)malloc(sizeof(struct alist_st));
						memset((ElementType *)curlist->next,0x0,sizeof(alist_t));
					}
					return;
				}
		};
		unsigned long alist_get_entry_n()
		{
			unsigned long entry_n = 0;
			alist_t * curlist = &list;
			do{
				entry_n+= curlist->used;
				curlist = curlist->next;
			} while(curlist);
			return entry_n;
		};
		int alist_remove(ElementType * to_remove)
		{
			unsigned int i=0;
			alist_t * curlist = &list;
			do{
				for(i=0;i<entrys_per_array;i++)
					if(curlist->pointer[i] == to_remove)
					{
						curlist->pointer[i] = NULL;
						curlist->used--;
						return 0;
					}
				curlist = curlist->next;
			} while(curlist);
			return -1;
		};
		~AList()
		{
			alist_t * curlist = &list;
			alist_t * nextlist = list.next;
			while(nextlist)
			{
				nextlist = curlist->next;
				free(curlist);
				curlist = nextlist;
			}
		};
};

#endif
