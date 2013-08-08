/*   <src-path>/src/usr/ld.a/lib_stat/string.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef STRING_H
#define STRING_H

#include"st_stdint.h"

#define ST_NULL ((void*) 0)

void *st_memcpy(void *dst, const void *src, st_size_t len);
void *st_memmove ( void *dst, const void *src, st_size_t len );
void *st_memset ( void *ptr, st_uint32_t val, st_size_t len );
st_size_t st_strlen(const char *s);
int st_strcmp(const char *s1, const char *s2);
#endif
