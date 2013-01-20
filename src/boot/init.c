#include <dbg/console.h>
#include <intr/idt.h>
#include <mm/gdt.h>
#include <drv/keyboard/keyboard.h>
#include <mt/ts.h>
#include <mt/zygote.h>
#include <mm/pmm.h>
#include <boot/multiboot.h>

void init(multiboot_info *mb_info)
{
    /*
     * Nachdem die physische Speicherverwaltung initialisiert ist, duerfen wir
     * die Multiboot-Infostruktur nicht mehr benutzen, weil sie bei der
     * Initialisierung nicht reserviert wurde und daher nach dem ersten
     * pmm_alloc() ueberschrieben werden koennte.
     */
    pmm_init(mb_info);
    clrscr();
    //kprintf("Hello World!\n",0x0A,0x00);
    
    //kprintfcol(0xF,FALSE,0xA,FALSE,"Hello World!\n");
    //kprintfcol(0x2,TRUE,0xC,FALSE,"Hello World!\n");
    
    //init_mt();
    //kprintf("Hello World \n Integer %s",0x20);
    kprintfcol(0xF,0xA,TRUE,"HelloWorld\n");
    init_keyboard();
    init_gdt();
    init_idt();
    
    init_task(task_a);
    init_task(task_b);
}
