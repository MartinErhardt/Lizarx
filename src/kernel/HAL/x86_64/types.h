/*   <src-path>/src/kernel/HAL/x86/types.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_64_TYPES_H
#define X86_64_TYPES_H

#define WORD_WIDTH 64
/*
 * The code below only works with LP64, which is th default on GCC with x86_64 target
 */

#define UN_64 unsigned long long
#define UN_32 unsigned int
#define UN_16 unsigned short
#define UN_8  unsigned char

#define N_64 long long
#define N_32 int
#define N_16 short
#define N_8  char

#define UINT_T UN_64
#define INT_T N_64
#define SIZE_T unsigned long
#define UPTR_T unsigned long
#define PTR_T long

#endif