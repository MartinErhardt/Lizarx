#include "console.h"
#include "idt.h"
#include "driver/keyboard/keyboard.h"
#include "gdt.h"
#include <vram.h>
#include "mt/ts.h"
#include "mt/zygote.h"
//#include "mt/ts.h"
void init(void)
{
    clrscr();
    //kprintf("Hello World!\n",0x0A,0x00);
    
    //kprintfcol(0xF,FALSE,0xA,FALSE,"Hello World!\n");
    //kprintfcol(0x2,TRUE,0xC,FALSE,"Hello World!\n");
    
    //init_mt();
    //kprintf("Hello World \n Integer %s",0x20);
    kprintfcol(0xF,0xA,TRUE,"HelloWorld\n");
    //init_keyboard();
    init_gdt();
    init_idt();
    kprintf("printing is still working");
    init_task(task_a);
    init_task(task_b);
}
