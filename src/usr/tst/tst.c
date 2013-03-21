#include <stdint.h>

#define SYS_WRITE 0

void uprintf(const char* fmt, ...);
void uprintfstrcol_scr(uint8_t font, const char* fmt);

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
    const char string[16] = "[TST] started\n#>";
    uprintf(&string[0]);
    // Exception
    //asm ("movl $0, %ebx; div %ebx");
    while(1);
}
void uprintf(const char* fmt, ...){
    uprintfstrcol_scr(VGA_WHITE,fmt);
}
void uprintfstrcol_scr(uint8_t font, const char* fmt){
    asm volatile( "nop" :: "d" (font));
    asm volatile( "nop" :: "b" ((uint32_t)fmt));
    //asm volatile( "nop" :: "c" (sizeof("sghs")));
    
    asm volatile( "nop" :: "a" (SYS_WRITE));
    
    asm volatile ("int $0x30");
}