#include "foo.h"
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include "main.h"
#include "../ld.a/ld.h"
#define SYS_WRITE 0
#define SYS_GET_BOOTMOD 9

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
extern "C"
{
	void * init_shared_lib(void* image, st_size_t size);
	void link_lib_against(struct elf_lib* first_elf, ...);
}
struct bootmod
{
    void * start;
    size_t size;
};
void get_bootmod(int num, struct bootmod* fill_it);

struct bootmod bootmod_main = 
{
    .start=NULL,
    .size=0
};
struct bootmod bootmod_lib = 
{
    .start=NULL,
    .size=0
};

int main(void)
{
    //
    //
    //while(1){}
    get_bootmod(2,&bootmod_main);
    get_bootmod(1,&bootmod_lib);
    
    uintptr_t lib_load_addr=(uintptr_t)init_shared_lib(bootmod_lib.start,bootmod_lib.size);
    struct elf_lib main_=
    {
	.header=(struct elf_header *)bootmod_main.start,
	.runtime_addr=0
    };
    struct elf_lib lib_=
    {
	.header=(struct elf_header *)bootmod_lib.start,
	.runtime_addr=lib_load_addr
    };

    link_lib_against(&main_,&lib_,NULL);
    link_lib_against(&lib_,&main_,NULL);
    foo();
    unsigned int malloced=(unsigned int)malloc(10);
    *((unsigned int*)malloced)= 0xDEADBEEF;
    uprintf(itoa(malloced,16));
    while(1);
    return 0;
}

void uprintf(char* fmt, ...)
{
    uprintfstrcol_scr(VGA_WHITE,fmt);
}
void uprintfstrcol_scr(unsigned char font, char* fmt){
    asm volatile( "nop" :: "d" (font));
    asm volatile( "nop" :: "b" ((unsigned int)fmt));
    //asm volatile( "nop" :: "c" (sizeof("sghs")));
    
    asm volatile( "nop" :: "a" (SYS_WRITE));
    asm volatile ("int $0x30");
}
void get_bootmod(int num, struct bootmod* fill_it)
{
    uintptr_t mod_addr;
    size_t mod_size;
    asm volatile( "nop" :: "d" (num));
    asm volatile ("int $0x30"::"a" (SYS_GET_BOOTMOD) );
    asm volatile("nop" : "=d" (mod_addr) );
    asm volatile("nop" : "=c" (mod_size) );
    fill_it->size=mod_size;
    fill_it->start=(void * )mod_addr;
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

