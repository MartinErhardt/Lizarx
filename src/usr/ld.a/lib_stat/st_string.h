/*   <src-path>/src/usr/ld.a/lib_stat/string.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
