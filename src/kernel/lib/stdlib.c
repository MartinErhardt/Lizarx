/*   <src-path>/src/kernel/lib/stdlib.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<stdlib.h>

/**
 * converts a long to a char
 * @return ptr to converted int
 **/
char * itoa(int x, int radix) {
    char buf[65];
    const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* p=0x0;
    
    if (radix > 36) {// radix mustn't be larger than 36 for security(overflow)
        return 0x0;
    }

    p = buf + 64;
    *p = '\0';
    do {
        *--p = digits[x % radix];
        x /= radix;
    } while (x);
    return p;
}