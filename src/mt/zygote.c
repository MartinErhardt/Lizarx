#include <mt/zygote.h>
#include <dbg/console.h>

void task_a(void)
{
   int i;
   for (i =0;i<30;i++) {
        kprintf("A");
   }
   asm volatile ("1: jmp 1b");
}
 
void task_b(void)
{
    int i;
    for (i =0;i<30;i++) {
        kprintf("B");
    }
    asm volatile ("1: jmp 1b");
}