/*  <src-path>/src/kernel/lib/libOS/mmio.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include"mmio.h"
#include<stdint.h>

uint32_t mmio_read32(uintptr_t base, uint32_t reg)
{
	return *((volatile uint32_t * )(base+reg));
}
void mmio_write32(uintptr_t base, uint32_t reg, uint32_t value)
{
	*((volatile uint32_t * )(base+reg))=value;
}
