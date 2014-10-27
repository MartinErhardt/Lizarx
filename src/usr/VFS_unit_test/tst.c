#include<hlib_vfs.h>
#include<archdef.h>
#include<asm_inline.h>
#include<sys/types.h>
#include<math.h>
#include<stdint.h>
#include<syscall.h>
#include<dbg.h>
#include<stdlib.h>
#include<errno.h>
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
//	uprintf("I want to open /proc/cpuinfo");
	//char haha_overwrite[20]="THIS IS OVERWRITTEN";
	char blabla[9]="NOCALHOS";
	SYSCALL(SYS_SLEEP);
	init_ipc();

	uprintf("and more ...");
	//uprintf(&blabla[0]);
	int fd2=hlib_open("/etc/third_level/hostname", 0);
	uprintf("here");
	hlib_read(fd2, &blabla, 9);
	uprintf(&blabla[0]);
	SYSCALL(12)
	return 0;
}
