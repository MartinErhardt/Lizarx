/*   <src-path>/src/kernel/HAL/x86/cmos.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_64_CMOS_H
#define X86_64_CMOS_H

#include<stdint.h>
#include"asm_inline.h"

#define BCD_DECODE(x) ((0xf & x) + (10 * (((0xf << 4) & x) >> 4)))

#define CMOS_PORT_ADDRESS 0x70
#define CMOS_PORT_DATA    0x71

static inline uint8_t cmos_read(uint8_t off)
{
	uint8_t tmp =0;
	uint8_t data =0;
	INB((uint16_t)CMOS_PORT_ADDRESS,tmp)
	OUTB((uint16_t)CMOS_PORT_ADDRESS,(tmp & 0x80) | (off & 0x7F))
	INB(CMOS_PORT_DATA,data)
	return data;
}

static inline void cmos_write(uint8_t off,uint8_t data)
{
	uint8_t tmp =0;
	INB(CMOS_PORT_ADDRESS,tmp)
	OUTB(CMOS_PORT_ADDRESS,(tmp & 0x80) | (off & 0x7F))
	OUTB(CMOS_PORT_DATA,data)
}

#endif