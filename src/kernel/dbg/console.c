#include <drv/vga-txt_graphics/vram.h>
#include <dbg/console.h>

// Kernel print standard function

int kprintf(const char* fmt, ...)
{
    va_list ap;
    uint32_t kprintf_res;
    atrbyt font={VGA_WHITE,VGA_BLACK};
    
    va_start(ap,fmt);
    kprintf_res=kprintfstrcol_scr(font,fmt,ap);
    va_end(ap);
    //va_list ap;
    return kprintf_res;//kprintfcol(0xF,0,0x0,0,*fmt, va_list);
}
