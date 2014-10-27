/*  <src-path>/src/usr/hlib/dbg.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#define SYS_WRITE 0
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
#define VGA_LRED 0xb
#define VGA_LMAGENTA 0xc
#define VGA_YELLOW 0xd
#define VGA_WHITE 0xf

#include"dbg.h"
#include<asm_inline.h>
static void uprintfstrcol_scr(unsigned char font, const char* fmt);
void uprintf(const char* fmt)
{
	uprintfstrcol_scr(VGA_WHITE,fmt);
}

static void uprintfstrcol_scr(unsigned char font, const char* fmt)
{
	SET_ARG1(((unsigned long) fmt));
#ifndef PIC
	SET_ARG2(font);
#endif
	//asm volatile( "nop" :: "c" (sizeof("sghs")));
	
	SYSCALL(SYS_WRITE);
}
char * itoa(unsigned int n, unsigned int base)
{
	static char new_str[16];
	unsigned int i = 14;
	
	do
	{
		unsigned int cur_digit = n % base;
		if (cur_digit < 10)
			new_str[i--] = cur_digit + '0';
		else
			new_str[i--] = (cur_digit - 10) + 'a';
	}
	while((n /= base) > 0);
	new_str[15] = '\0';
	return (char*)(new_str+(i+1));
}/*
void hlib_printf(const char* fmt)
{
	uprintf(fmt);
}*/
