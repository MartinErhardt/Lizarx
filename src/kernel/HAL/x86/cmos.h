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
#ifndef X86_CMOS_H
#define X86_CMOS_H

#include<stdint.h>
#include"asm_inline.h"

#define CMOS_PORT_ADDRESS 0x70
#define CMOS_PORT_DATA    0x71

#define CMOS_READ(DATA,OFF) 	uint8_t tmp =0;\
				INB(CMOS_PORT_ADDRESS,tmp)\
				OUTB(CMOS_PORT_ADDRESS,(tmp & 0x80) | (OFF & 0x7F))\
				INB(CMOS_PORT_DATA,DATA)

#define CMOS_WRITE(DATA,OFF) 	uint8_t tmp =0;\
				INB(CMOS_PORT_ADDRESS,tmp)\
				OUTB(CMOS_PORT_ADDRESS,(tmp & 0x80) | (OFF & 0x7F))\
				OUTB(CMOS_PORT_DATA,DATA)

#endif