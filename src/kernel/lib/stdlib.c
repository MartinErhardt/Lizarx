/*  <src-path>/src/kernel/lib/stdlib.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include<stdlib.h>

/**
 * converts a long to a char
 * @return ptr to converted int
 **/
char * itoa(unsigned long n, unsigned int base)
{
	static char new_str[17];
	unsigned int i = 15;
	
	do
	{
		unsigned int cur_digit = n % base;
		if (cur_digit < 10)
			new_str[i--] = cur_digit + '0';
		else
			new_str[i--] = (cur_digit - 10) + 'a';
	}
	while((n /= base) > 0);
	new_str[16] = '\0';
	return (char*)(new_str+(i+1));
}
