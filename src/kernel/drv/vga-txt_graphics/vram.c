/*   <src-path>/src/kernel/drv/vga-txt_graphics is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *   Copyright (C) 2013  martin.erhardt98@googlemail.com
 *
 *  Lizarx is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Lizarx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU LESSER General Public License
 *  along with Lizarx.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <drv/vga-txt_graphics/vram.h>
#include <asm_inline.h>

#define VIDEO_X 80
#define VIDEO_Y 25
/*
  NOTE: I decided to implement the VGA-txt-graphics driver into the Kernel;
	This goes with an increase of Performance. but also with an increase of Syscalls and complexity
 */
// Pointer zum VRAM
static uint16_t* video = (uint16_t*) 0xB8000;
static size_t vram_size =sizeof(figure)*VIDEO_Y*VIDEO_X;
uint8_t curx = 0;
uint8_t cury = 0;
// printed chars
uint32_t kprintf_res = 0;
bool scr_enb = TRUE;

static void scroll(uint8_t by);

static void newline();
//Clear Screen function

void clrscr(uint8_t forgcol, uint8_t backcol)
{
	uint16_t fig=0xffff & (forgcol|(backcol>>4));
	memset(video,(uint32_t) fig | (fig<<16),sizeof(figure)*VIDEO_X*VIDEO_Y);
	curx=cury=0;
}

// put stream in memory and increment X
void kputs(const char* s,atrbyt font)
{
	while (*s)
	{
		kput(*s++,font);
	}
}

void kput(uint8_t chr, atrbyt font)
{
	//OUTB(0x3f8, chr);
	figure fig={chr, font};
	uint16_t* adr=0;
	if((curx>=VIDEO_X)&&(cury<(VIDEO_Y-1)))// Test
		newline();
	if(chr =='\n')
	{
		newline();
		return;
	}
	if((scr_enb==TRUE)&&(cury==VIDEO_Y )){
		scroll(1);
		cury--;
		curx=0;
	}
	else if((scr_enb==TRUE)&& (cury==(VIDEO_Y-1)) && (curx==VIDEO_X) )
	{
		scroll(1);
		curx=0;
	}
	if(chr == '\t')
	{
		curx = curx - curx%8 +8;
		return;
	}
	if(chr == '\b')
	{
		if(curx==0)
		{
			//cury--;
			//curx = VIDEO_X-1;
		}
		else{ curx--;}
		uint16_t* adr = (uint16_t*) (video+(cury * VIDEO_X) + curx);
		memmove(adr,0x0,2);
		return;
	}
	// berechnen der Adresse
	adr = (uint16_t*) (video+(cury * VIDEO_X) + curx);  // eine Multiplikation mit 2 darf hier nicht erfolgen, da off vom type uint16_t ist
	
	// setzen des zeichens
	memmove(adr,&fig,sizeof(fig));
	//*adr = fig;
	curx++;
}

uint8_t getcurx()
{
	return curx;
}
uint8_t getcury()
{
	return cury;
}

void setcurs(uint8_t xp,uint8_t yp)
{
	curx = xp;
	cury = yp;
}

void drawcurs()
{
	/*
	uint16_t tmp= (cury*VIDEO_X+curx)-1;
	
	OUTB(0x3d4,14);
	OUTB(0x3d5,(uint8_t)(tmp >> 8));
	OUTB(0x3d4,15);
	OUTB(0x3d5,(uint8_t)tmp);
	*/
}
void rmvcurs()
{
	/*
	OUTB(0x3d4,14);
	OUTB(0x3d5,0x07);
	OUTB(0x3d4,15);
	OUTB(0x3d5,0xd0);
	*/
}
 
static void newline()
{
	cury++;
	curx=0;
}

void kprintn_scr(unsigned long x, int base, atrbyt font)
{
	kputs(itoa(x,base),font);
}

//Kernel printcolor prints a colored string

unsigned int kprintfcol_scr(uint8_t forgcol, uint8_t backcol, const char* fmt, ...)
{
	va_list ap;
	atrbyt font={backcol, forgcol};
	va_start(ap,fmt);
	kprintfstrcol_scr(font,fmt,ap);
	va_end(ap);
	return kprintf_res;
}

//Kernel print string colored prints a colored string

unsigned int kprintfstrcol_scr(atrbyt font, const char* fmt, va_list appar)
{
	const char* s;
	uint_t n;
	//const char a= 'a';
	//const char b= 'b';
	kprintf_res = 0;
	spinlock_ackquire(&console_lock);
	//OUTB(0x3f8, a );
	while (*fmt) {
		// serial console(only QEMU)
		
		
		if (*fmt == '%') 
		{
			fmt++;
			switch (*fmt) 
			{
				case 's':
					s = va_arg(appar, char*);
					kputs(s,font);
					break;
				case 'd':
				case 'u':
					n = va_arg(appar, uint_t );
					kprintn_scr(n, 10,font);
					break;
				case 'x':
				case 'p':
					n = va_arg(appar, uint_t);
					kprintn_scr(n, 16,font);
					break;
				case '%':
					kput('%',font);
					break;
				case '\0':
					goto out;
				default:
					kput('%',font);
					kput(*fmt,font);
					break;
			}
		} 
		else 
		{
			kput(*fmt,font);
		}
		fmt++;
	}

out:
	va_end(appar);
	drawcurs();
	spinlock_release(&console_lock);
	//OUTB(0x3f8, b );
	return kprintf_res;
}
//FIXME puffer awayscrolled chars to scroll-back later
static void scroll(uint8_t by)
{
	size_t off = sizeof(figure)*VIDEO_X*by;
	size_t size_of_copy=vram_size-off;
	memmove(video,(uint32_t*)((uintptr_t)video+off),size_of_copy);
	memset((uint32_t*)((uintptr_t)video+size_of_copy),0x0,off);//FIXME 0x0 can be something else than the backgroundcol
}
