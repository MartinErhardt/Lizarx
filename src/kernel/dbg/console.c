/*  <src-path>/src/kernel/dbg/console.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#include <drv/vga-txt_graphics/vram.h>
#include <dbg/console.h>
#include<libOS/lock.h>

// Kernel print standard function

int kprintf(const char* fmt, ...)
{
    va_list ap;
    uint32_t kprintf_res;
    atrbyt font={VGA_WHITE,VGA_BLACK};
    
    va_start(ap,fmt);
    kprintf_res=kprintfstrcol_scr(font,fmt,ap);
    va_end(ap);
    //va_list ap;
    return kprintf_res;//kprintfcol(0xF,0,0x0,0,*fmt, va_list);
}
