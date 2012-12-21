#include "console.h"
#include "idt.h"
#include "driver/keyboard/keyboard.h"
#include "gdt.h"
#include <vram.h>
void init(void)
{
    clrscr();
    //kprintf("Hello World!\n",0x0A,0x00);
    
    atrbyt font={0xF,TRUE,0xA,TRUE};
    
    //kprintfcol(0xF,FALSE,0xA,FALSE,"Hello World!\n");
    //kprintfcol(0x2,TRUE,0xC,FALSE,"Hello World!\n");
    init_gdt();
    init_idt();
    //kprintf("Hello World \n Integer %s",0x20);
    kprintfstrcol(font,"Hello World\nInteger 0x%p",0x20);
    init_keyboard();

    // Pointer zum VRAM
}
