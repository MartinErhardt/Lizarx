#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vram.h>
#include "console.h"

// printed chars
static int kprintf_res = 0;

void kprintn(unsigned long x, int base, atrbyt font)
{
    kputs(itoa(x,base),font);
}

// Kernel print standard function

int kprintf(const char* fmt, ...)
{
    va_list ap;
    atrbyt font={0xF,0x0};
    
    va_start(ap,fmt);
    kprintfstrcol(font,fmt,ap);
    va_end(ap);
    //va_list ap;
    return kprintf_res;//kprintfcol(0xF,0,0x0,0,*fmt, va_list);
}


//Kernel printcolor prints a colored string

int kprintfcol(uint8_t forgcol, uint8_t backcol, bool blink, const char* fmt, ...){
    va_list ap;
    atrbyt font={forgcol|(blink<<3),backcol};
    //CALL_VA_FUNC(kprintfstrcol,font,fmt);
    
    va_start(ap,fmt);
    kprintfstrcol(font,fmt,ap);
    va_end(ap);

    return kprintf_res;
}

//Kernel print string colored prints a colored string

int kprintfstrcol(atrbyt font, const char* fmt, va_list appar){
    const char* s;
    unsigned long n;

    kprintf_res = 0;
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 's':
                    s = va_arg(appar, char*);
                    kputs(s,font);
                    break;
                case 'd':
                case 'u':
                    n = va_arg(appar, unsigned long int);
                    kprintn(n, 10,font);
                    break;
                case 'x':
                case 'p':
                    n = va_arg(appar, unsigned long int);
                    kprintn(n, 16,font);
                    break;
                case '%':
                    kput('%',font);
                    break;
                case '\0':
		   drawcurs();
                    goto out;
                default:
                    kput('%',font);
                    kput(*fmt,font);
                    break;
            }
        } else {
            kput(*fmt,font);
        }
	
        fmt++;
    }

out:
    va_end(appar);
    
    return kprintf_res;
}