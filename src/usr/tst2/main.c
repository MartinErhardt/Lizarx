#include "foo.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

#define SYS_WRITE 0
#define SYS_ERROR 8

char * itoa2(unsigned int n, unsigned int base);
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
int main(void)
{
    uprintf("hello main");
    uprintf(itoa2((uintptr_t)&foo,16));
    foo();
    uprintf("hello main 2");
    while(1){}
    return 0;
}

void uprintf(char* fmt, ...){
    uprintfstrcol_scr(VGA_WHITE,fmt);
}
void uprintfstrcol_scr(uint8_t font, char* fmt){
    asm volatile( "nop" :: "d" (font));
    asm volatile( "nop" :: "b" ((uint32_t)fmt));
    //asm volatile( "nop" :: "c" (sizeof("sghs")));
    
    asm volatile( "nop" :: "a" (SYS_WRITE));
    
    asm volatile ("int $0x30");
}

/**
 * converts a long to a char
 * @return ptr to converted int
 **/
char * itoa2(unsigned int n, unsigned int base)
{
	static char new_str[16];
	unsigned int i = 14;
	
	do
	{
		unsigned int cur_digit = n % base;
		if (cur_digit < 10)
		{
			new_str[i--] = cur_digit + '0';
		}
		else
		{
			new_str[i--] = (cur_digit - 10) + 'a';
		}
	}
	while((n /= base) > 0);
	new_str[15] = '\0';
	return (char*)(new_str+(i+1));
}
