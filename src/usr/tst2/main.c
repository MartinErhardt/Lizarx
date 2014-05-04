#include "foo.h"
#include<stdint.h>
#include<string.h>
#include<stdlib.h>
#include "main.h"
#include "../ld.a/ld.h"
#include"../asm_inline.h"

#define SYS_WRITE 0
#define SYS_GET_BOOTMOD 9
#define SYS_VMM_MALLOC 10 

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

struct bootmod bootmod_main;
struct bootmod bootmod_lib;
uintptr_t st_vmm_malloc(st_size_t alloc_size)
{
	st_uintptr_t free_space;
	asm volatile( "nop" :: "d" (alloc_size));
	SYSCALL(SYS_VMM_MALLOC)
	asm volatile("nop" : "=d" (free_space) );
	return free_space;
}
int main(void)
{
	
	// outcomment to disable shared objects on x86
	struct elf_lib main_;
	struct elf_lib lib_;
	bootmod_lib.start	= NULL;
	bootmod_lib.size	= 0;
	bootmod_main.start	= NULL;
	bootmod_main.size	= 0;
	

	//*((uint64_t*)st_vmm_malloc(0x1000))=0x10101010;
	//*((uint64_t*)st_vmm_malloc(0x1000))=0x10101010;
	//*((uint64_t*)st_vmm_malloc(0x1000))=0x10101010;
	//*((uint64_t*)st_vmm_malloc(0x1000))=0x10101010;
	//*((uint64_t*)st_vmm_malloc(0x1000))=0x10101010;
	
	
	get_bootmod(2,&bootmod_main);
	get_bootmod(1,&bootmod_lib);
	
	uintptr_t lib_load_addr	=(uintptr_t)init_shared_lib(bootmod_lib.start,bootmod_lib.size);
	
	main_.header		= (struct elf_header *)bootmod_main.start;
	main_.runtime_addr	= 0;
	
	lib_.header		= (struct elf_header *)bootmod_lib.start;
	lib_.runtime_addr	= lib_load_addr;
	
	link_lib_against(&main_,&lib_,NULL);
	link_lib_against(&lib_,&main_,NULL);
	
	foo();
	
	//unsigned long malloced=(unsigned long)malloc(10);
	
	//*((unsigned int*)malloced)= 0xDEADBEEF;
	//uprintf(itoa(malloced,16));
	//uprintf("\n");
	while(1);
	return 0;
}

void uprintf(char* fmt, ...)
{
    uprintfstrcol_scr(VGA_WHITE,fmt);
}
void uprintfstrcol_scr(unsigned char font, char* fmt){
	asm volatile( "nop" :: "d" (font));
	asm volatile( "nop" :: "b" ((unsigned long)fmt));
	
	SYSCALL(SYS_WRITE)
}
void get_bootmod(int num, struct bootmod* fill_it)
{
	uintptr_t mod_addr;
	size_t mod_size;
	asm volatile( "nop" :: "d" (num));
	
	SYSCALL(SYS_GET_BOOTMOD)
	//while(1);
	asm volatile("nop" : "=a" (mod_addr) );
	asm volatile("nop" : "=d" (mod_size) );
	
	fill_it->size = mod_size;
	fill_it->start = (void * )mod_addr;
}
char * itoa(unsigned long n, unsigned int base)
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

