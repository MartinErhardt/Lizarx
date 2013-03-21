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
struct gdt_entry{
	uint16_t 	limit;
	uint32_t 	base :24;
	uint8_t 		accessbyte;
	uint32_t 	limit2 :4;
	uint32_t 	flags :4;
	uint8_t 		base2;
}__attribute__((packed));

uint32_t tss[32];

void gdt_set_entry(uint8_t i,uint32_t limit,uint32_t base,uint8_t accessbyte,uint8_t flags);
void init_gdt(void);
#endif