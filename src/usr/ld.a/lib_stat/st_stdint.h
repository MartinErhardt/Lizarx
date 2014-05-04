/*   <src-path>/src/usr/ld.a/lib_stat/stdint.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef ST_STDINT_H
#define ST_STDINT_H

#include"../../archdef.h"

#ifdef ARCH_X86

typedef unsigned long long st_uint64_t;
typedef unsigned long st_uint32_t;
typedef unsigned short st_uint16_t;
typedef unsigned char st_uint8_t;

typedef long long  st_int64_t;
typedef long st_int32_t;
typedef short st_int16_t;
typedef char st_int8_t;

// size 
typedef unsigned long st_size_t;

// Signed pointer-sized integer 
typedef long st_intptr_t;
// Unsigned pointer-sized integer
typedef unsigned long st_uintptr_t;

typedef unsigned long st_uint_t;
typedef long st_int_t;

#endif
#ifdef ARCH_X86_64

typedef unsigned long long st_uint64_t;
typedef unsigned int st_uint32_t;
typedef unsigned short st_uint16_t;
typedef unsigned char st_uint8_t;

typedef long long  st_int64_t;
typedef int st_int32_t;
typedef short st_int16_t;
typedef char st_int8_t;

// size 
typedef unsigned long long st_size_t;

// Signed pointer-sized integer 
typedef long long st_intptr_t;
// Unsigned pointer-sized integer
typedef unsigned long st_uintptr_t;

typedef unsigned long st_uint_t;
typedef long st_int_t;

#endif

#endif
