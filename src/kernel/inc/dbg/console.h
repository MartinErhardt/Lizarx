/*   <src-path>/src/kernel/inc/dbg/console.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef CONSOLE_H
#define CONSOLE_H

#include<stdint.h>
#include<stdbool.h>
#include<stdarg.h>
#include <drv/vga-txt_graphics/vram.h>

#define CALL_VA_FUNC(func,param,param1) \
        va_start(ap,param1);\
        func(param,param1,ap);\
        va_end(ap);

int kprintf(const char* fmt, ...);
#endif
