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

class AList
{
	alist_t list;
	public:
		AList()
		{
			memset(&list,0,sizeof(alist_t));
		};
		void * alist_get_by_index(unsigned long index);
		long alist_get_index(void * entry);
		void * alist_get_by_entry8(off_t off, uint8_t val);
		void * alist_get_by_entry( off_t off, unsigned long val);
		void alist_add(void * entry);
		unsigned long alist_get_entry_n();
		int alist_remove(void * to_remove);
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
