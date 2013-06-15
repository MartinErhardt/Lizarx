/*   <src-path>/src/kernel/inc/mm/gdt.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef GDT_H
#define GDT_H
#include <stdint.h>

//--------------------------------------------------------------------------GDT--------------------------------------------------------------------
#define GDT_SIZE 6

/*Accessbyte
7	 0x80	 Present bit
6 u. 5	 0x60	 Privilege
4	 0x10	 Segment bit
3	 0x08	 Executable bit
2	 0x04	 Direction bit/Conforming bit
1	 0x02	 Readable bit/Writable bit
0	 0x01	 Accessed bit
*/
#define GDT_ACCESS_DATASEG 0x02
#define GDT_ACCESS_CODESEG 0x0a
#define GDT_ACCESS_TSS     0x09// 0x08

#define GDT_ACCESS_SEGMENT 0x10
#define GDT_ACCESS_RING0   0x00
#define GDT_ACCESS_RING3   0x60
#define GDT_ACCESS_PRESENT 0x80
/*Flags
3	 0x8	 Granularity bit
2	 0x4	 Size bit
1	 0x2	 Long Mode bit
0	 0x1	 Available Bit
*/
#define GDT_FLAG_4KUNIT      0x08
uint32_t tss[32];

void gdt_set_entry(unsigned char i,unsigned int limit,unsigned int base,unsigned char accessbyte,unsigned char flags);
void init_gdt(void);
#endif
