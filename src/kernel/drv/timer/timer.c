/*   <src-path>/src/kernel/drv/timer/timer.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<hal.h>
#include<mm/gdt.h>
#include<drv/timer/timer.h>
#include<mt/ts.h>
#include<stdbool.h>
#include<stdint.h>
#include <drv/vga-txt_graphics/vram.h>

uint32_t time=0;
void timer_handler(CPU_STATE** new_cpu){
    time++;
    drawcurs();
    *new_cpu=schedule(*new_cpu);
    tss[1] = (uint32_t) (*new_cpu + 1);
}