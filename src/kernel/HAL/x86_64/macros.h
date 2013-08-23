/*   <src-path>/src/kernel/HAL/x86/macros.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_64_MACROS_H
#define X86_64_MACROS_H
/*Flags
3	 0x8	 Granularity bit
2	 0x4	 Size bit
1	 0x2	 Long Mode bit
0	 0x1	 Available Bit
*/
#define GDT_FLAG_32_BIT  0x2

//-------------------------------------------------------------------------IDT---------------------------------------------------------------------

#define IDT_SIZE		256
#define GDT_KERNEL_CODE_SEGMENT 0x8
#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT	0x80
#define IDT_FLAG_RING0		0x00
#define IDT_FLAG_RING3		0x60
//-------------------------------------------------------------------------Paging------------------------------------------------------------------

/*
 * Flags for the Page Table and Page Directory
 */
#define FLG_IN_MEM 	0x01
#define FLG_WRITABLE 	0x02
#define FLG_USERACCESS	0x04
#define FLG_WRITECACHING 0x08
#define FLG_NOCACHE	0x10
//dirty access bits automatically set by CPU
#define FLG_USED	0x20
#define FLG_WRITTEN	0x40 // ignored in Page Directory Entry, if 4K Page
#define FLGPD_NOT4K	0x80 // only in Page Directory Entry

#define PAGING_HIER_SIZE 0x4 // we have

#endif
