/*  <src-path>/src/kernel/lib/stdint.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef STDINT_H
#define STDINT_H

#include<types.h>

typedef UN_64 uint64_t;
typedef UN_32 uint32_t;
typedef UN_16 uint16_t;
typedef UN_8 uint8_t;

typedef N_64 int64_t;
typedef N_32 int32_t;
typedef N_16 int16_t;
typedef N_8 int8_t;

// size 
typedef SIZE_T size_t;

// Signed pointer-sized integer 
typedef PTR_T intptr_t;
// Unsigned pointer-sized integer
typedef UPTR_T uintptr_t;

typedef INT_T int_t;
typedef UINT_T uint_t;
typedef int_t ssize_t;

#endif
