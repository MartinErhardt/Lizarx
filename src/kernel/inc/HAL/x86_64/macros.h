/*  <src-path>/src/kernel/inc/HAL/x86_64/macros.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef X86_64_MACROS_H
#define X86_64_MACROS_H
/*Flags
3	 0x8	 Granularity bit
2	 0x4	 Size bit
1	 0x2	 Long Mode bit
0	 0x1	 Available Bit
*/
#define GDT_FLAG_32_BIT  0x2
#define EASYMAPTBL_SIZ (0x1000 *515)

#define STDRD_STACKSIZ 0x2000

#define KERNEL_CODE_SEG_N	1
#define KERNEL_STACK_SEG_N	2
#define KERNEL_DATA_SEG_N	3
#define USER_CODE_SEG32_N	4
#define USER_CODE_STACK_SEG_N	5
#define USER_CODE_SEG64_N	6
#define USER_DATA_SEG_N		7
#define TSS_SEG_N		8

#define BITMAP_SIZE 		8192
#define PAGE_SIZE 		4096 // page size in bytes
#define MSR_STAR		0xC0000081
#define MSR_LSTAR		0xC0000082
#define MSR_EFER		0xC0000080
//-------------------------------------------------------------------------memory-map--------------------------------------------------------------
//#define INIT_PAGE_TBL_ADDR		0x113000
//#define INIT_PAGEDIR_TBL_ADDR		0x112000
//#define INIT_PAGEDIRPTR_TBL_ADDR	0x111000
//#define INIT_PAGEMAPLEVEL4_TBL_ADDR	0x110000
//#define BSP_STACK 			0x105000-STDRD_STACKSIZ
//uintptr_t static_data;
//#define BSP_STACK_ static_data;
//#define INIT_PAGE_TBL_ADDR 0x113000
//#define INIT_PAGEDIR_TBL_ADDR INIT_PAGE_TBL_ADDR-0x1000
//#define INIT_PAGEDIRPTR_TBL_ADDR INIT_PAGEDIR_TBL_ADDR-0x1000
//#define INIT_PAGEMAPLEVEL4_TBL_ADDR INIT_PAGEDIRPTR_TBL_ADDR-0x1000
#define TRAMPOLINE 			0x7000
#define KERNEL_SPACE	0x40000000 //= 67 MB
//-------------------------------------------------------------------------IDT---------------------------------------------------------------------

#define IDT_SIZE		256
#define GDT_KERNEL_CODE_SEGMENT 0x8
#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT	0x80
#define IDT_FLAG_RING0		0x00
#define IDT_FLAG_RING3		0x60

#define INTR_TIMER		28
#define INTR_KEYBOARD		0x21
#define INTR_SOFT_INTR		0x48
#define INTR
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
