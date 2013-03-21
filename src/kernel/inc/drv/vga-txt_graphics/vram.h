/*   <src-path>/src/kernel/inc/drv/vga-txt_graphics/vram.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef VRAM_H
#define VRAM_H

#include <stdint.h>
#include <stdarg.h>

// Aufschlüsselung des Attribut-Bytes vom Text Modus
typedef struct 
{
    uint8_t bckgrndnblnk : 4;
    uint8_t frgrndnbld : 4;

}__attribute__((packed)) atrbyt;
// Ein Zeichen
typedef struct{
  
    uint8_t literal;
    atrbyt font;
    
}__attribute__((packed)) figure;


#define VGA_BLACK 0x0 
#define VGA_BLUE 0x1
#define VGA_GREEN 0x2
#define VGA_CYAN 0x3
#define VGA_RED 0x4
#define VGA_MAGENTA 0x5
#define VGA_BROWN 0x6
#define VGA_LGREY 0x7
#define VGA_GREY 0x8
#define VGA_LBLUE 0x9
#define VGA_LGREEN 0xa
#define VGA_LCYAN 0xb
#define VGA_LRED 0xc
#define VGA_LMAGENTA 0xd
#define VGA_YELLOW 0xe
#define VGA_WHITE 0xf

void clrscr(uint8_t forgcol, uint8_t backcol);
void kputs(const char* s,atrbyt font);
void kput(uint8_t chr, atrbyt font);

uint8_t getcurx();
uint8_t getcury();

void setcurs(uint8_t xp,uint8_t yp);

void rmvcurs();
void drawcurs();

uint32_t kprintfcol_scr(uint8_t forgcol, uint8_t backcol, const char* fmt, ...);
uint32_t kprintfstrcol_scr(atrbyt font,const char* fmt, va_list appar);
void kprintn_scr(unsigned long x, int base, atrbyt font);
#endif
