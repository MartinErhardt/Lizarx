#include <stdint.h>

#define SYS_WRITE 0

void uprintf(char* fmt, ...);
void uprintfstrcol_scr(uint8_t font, char* fmt);
//char * itoa(int x, int radix);

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
void _start(void)
{
    //unsigned int i=0;
    //char stringa[13] ="this is proc2";

    // Exception
    //asm ("movl $0, %ebx; div %ebx");
    while(1)
    { 
	//uprintf(&stringa[0]);
	//i++;
    }
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
/*
 * converts a long to a char
 * @return ptr to converted int
 *
char * itoa(int x, int radix) {
    char buf[65];
    const char* digits = "0123456789abcdefghijklmnopqrstuvwxyz";
    char* p=0x0;
    
    if (radix > 36) {// radix mustn't be larger than 36 for security(overflow)
        return 0x0;
    }

    p = buf + 64;
    *p = '\0';
    do {
        *--p = digits[x % radix];
        x /= radix;
    } while (x);
    return p;
}*/