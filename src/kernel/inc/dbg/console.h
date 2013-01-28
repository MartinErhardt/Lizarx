#ifndef CONSOLE_H
#define CONSOLE_H

#include<stdint.h>
#include<stdbool.h>
#include<stdarg.h>
#include <drv/vga-txt_graphics/vram.h>

#define CALL_VA_FUNC(func,param,param1) \
        va_start(ap,param1);\
        func(param,param1,ap);\
        va_end(ap);

int kprintf(const char* fmt, ...);
int kprintfcol(uint8_t forgcol, uint8_t backcol, bool blink, const char* fmt, ...);
int kprintfstrcol(atrbyt font,const char* fmt, va_list appar);
#endif
