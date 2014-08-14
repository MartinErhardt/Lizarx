/*   <src-path>/src/usr/ld.a/lib_stat/stdlib.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include"st_stdlib.h"


/**
 * converts a long to a char
 * @return ptr to converted int
 **/
char * st_itoa(unsigned long n, unsigned int base)
{
	static char new_str[16];
	unsigned int i = 14;
	
	do
	{
		unsigned int cur_digit = n % base;
		if (cur_digit < 10)
		{
			new_str[i--] = cur_digit + '0';
		}
		else
		{
			new_str[i--] = (cur_digit - 10) + 'a';
		}
	}
	while((n /= base) > 0);
	new_str[15] = '\0';
	return (char*)(new_str+(i+1));
}
