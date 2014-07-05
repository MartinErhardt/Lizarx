#include<math.h>
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include"../archdef.h"
#include"../asm_inline.h"

#define SYS_WRITE 0
#define SYS_ERROR 8
#define SYS_EXIT  12

void uprintf(char* fmt, ...);
void uprintfstrcol_scr(unsigned char font, char* fmt);
char * itoa(unsigned int n, unsigned int base);
unsigned long shmat(unsigned long id);
unsigned long msgrcv(unsigned long id, void *ptr, unsigned long size);
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
//extern "C" void _start();
unsigned long receive = 0;
int main(void)
{
	int i=0;
	char hellocpp[]="hello C++ \n";
	char hellolibc[]="hello libC; sqrt of 4 =";
	char shared_memory[] = "shared memory: ";
	char message_queue_1[] = "message queue #1: ";
	char message_queue_2[] = "message queue #2: ";
	char newline[]="\n";
	uprintf(&hellocpp[0]);
	uprintf(&hellolibc[0]);
	uprintf(itoa(sqrt(4),10));
	uprintf(&newline[0]);
	for(i=0;i<0xffffff;i++);
	unsigned long addr = shmat(0);
	uprintf(&shared_memory[0]);
	uprintf(itoa(*((unsigned long *)addr),16));
	uprintf(&newline[0]);
	msgrcv(0,&receive, sizeof(unsigned long));
	uprintf(&message_queue_1[0]);
	uprintf(itoa(receive,16));
	uprintf(&newline[0]);
	
	msgrcv(0,&receive, sizeof(unsigned long));
	uprintf(&message_queue_2[0]);
	uprintf(itoa(receive,16));
	uprintf(&newline[0]);
	asm volatile( "nop" :: "d" (4));
	SYSCALL(19);
	SYSCALL(SYS_EXIT);
	while(1);
	return 0;
}
void uprintf(char* fmt, ...)
{
	uprintfstrcol_scr(VGA_WHITE,fmt);
}
void uprintfstrcol_scr(unsigned char font, char* fmt)
{
	asm volatile( "nop" :: "d" (font));
	asm volatile( "nop" :: "b" ((unsigned long)fmt));
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
unsigned long shmat(unsigned long id)
{
	unsigned long addr;
	asm volatile( "nop" :: "d" (id));
	SYSCALL(14);
	asm volatile( "nop" : "=d" (addr));
	return addr;
}
unsigned long msgrcv(unsigned long id, void *ptr, unsigned long size)
{
	unsigned long suc;
	
	asm volatile( "nop" :: "d" ((uintptr_t)ptr));
	asm volatile( "nop" :: "b" (((uintptr_t)size)|(id<<16)));
	//asm volatile( "nop" :: "a" (id));
	SYSCALL(17);
	asm volatile( "nop" : "=d" (suc));
	return suc;
}
