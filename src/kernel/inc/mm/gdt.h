/*   <src-path>/src/kernel/inc/mm/gdt.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef GDT_H
#define GDT_H

#include <stdint.h>
#include <hw_structs.h>

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
struct tss_t tss;

#ifndef LOADER
void init_gdt_AP(void);
#endif

void gdt_set_entry(struct gdt_entry * gdttable_tofill,uint8_t i,uint16_t limit,uint32_t base,uint8_t accessbyte,uint8_t flags);
void init_gdt(void);
#ifdef ARCH_X86_64
void setup_tss();
#endif

#endif
