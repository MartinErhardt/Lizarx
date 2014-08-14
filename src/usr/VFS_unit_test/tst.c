#include"../archdef.h"
#include"../asm_inline.h"

#include<string.h>
#include<math.h>

#define SYS_WRITE 0
#define SYS_ERROR 8
#define SYS_EXIT  12

void uprintf(const char* fmt, ...);
void uprintfstrcol_scr(unsigned char font,const char* fmt);
char * itoa(unsigned int n, unsigned int base);
unsigned long shmget();
unsigned long shmat(unsigned long id);
unsigned long msgget();
unsigned long msgsnd(unsigned long id, void *ptr, unsigned long size);
int _open__(const char* path);
unsigned long get_pid___();
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
struct VFS_msg
{
	unsigned long type;
	unsigned long pid;
	char path[257];
};
typedef enum {
    VFS_OPEN,
    VFS_CLOSE,
    VFS_WRITE
} syscalls;

int main(void)
{
	int i;
	for(i=0;i<0xffffff;i++);
	_open__("/proc/cpuinfo");
	SYSCALL(12)
	return 0;
}
int _open__(const char* path)
{
	struct VFS_msg my_buf;
	my_buf.type=VFS_OPEN;
	my_buf.pid=get_pid___();
	strcpy((char*)&(my_buf.path),path);
	msgsnd(0,&my_buf, sizeof(struct VFS_msg));
	return 0;
}
unsigned long get_pid___()
{
	unsigned long pid;
	SYSCALL(5);
	asm volatile( "nop" : "=d" (pid));
	return pid;
}
void uprintf(const char* fmt, ...)
{
	uprintfstrcol_scr(VGA_WHITE,fmt);
}
void uprintfstrcol_scr(unsigned char font, const char* fmt)
{
	asm volatile( "nop" :: "d" (font));
	asm volatile( "nop" :: "b" ((unsigned long)fmt));
	//asm volatile( "nop" :: "c" (sizeof("sghs")));
	
	SYSCALL(SYS_WRITE);
}

unsigned long shmget()
{
	unsigned long id;
	asm volatile( "nop" :: "d" (0x100));
	SYSCALL(13);
	asm volatile( "nop" : "=d" (id));
	return id;
}
unsigned long shmat(unsigned long id)
{
	unsigned long addr;
	asm volatile( "nop" :: "d" (id));
	SYSCALL(14);
	asm volatile( "nop" : "=d" (addr));
	return addr;
}
unsigned long msgget()
{
	unsigned long id;
	SYSCALL(15);
	asm volatile( "nop" : "=d" (id));
	return id;
}
unsigned long msgsnd(unsigned long id, void *ptr, unsigned long size)
{
	unsigned long suc;
	//asm volatile( "nop" :: "a" (id));
	asm volatile( "nop" :: "d" ((unsigned long)ptr));
	asm volatile( "nop" :: "b" (((unsigned long)size)|(id<<16)) );
	
	SYSCALL(16);
	asm volatile( "nop" : "=d" (suc));
	return suc;
}
