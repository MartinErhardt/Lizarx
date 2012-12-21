#ifndef CONSOLE_H
#define CONSOLE_H

#include<stdint.h>
#include<stdbool.h>
#include<vram.h>

#define CALL_VA_FUNC(func,param,param1) do {\
        va_list ap; \
        va_start(ap,param);\
        func(param,param1,ap);\
        va_end(ap); } while(0);
        
int kprintf(const char* fmt, ...);
int kprintfcol(uint8_t forgcol, bool bold, uint8_t backcol, bool blink, const char* fmt, ...);
int kprintfstrcol(atrbyt font, const char* fmt, ...);

#endif
