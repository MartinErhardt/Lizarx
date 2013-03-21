/*   <src-path>/src/kernel/drv/io/ioport.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<stdint.h>
#include<drv/io/ioport.h>

/* writes a byte into an I/O-Port */
void outb(uint16_t port, uint8_t data)
{
    asm volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}
/* reads a Byte from an I/O-Port */
uint8_t inb(uint16_t port){
	uint8_t result;
	asm ("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}
