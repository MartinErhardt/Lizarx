/* We need 8 segments */
#include <stdint.h>
#include <console.h>
#include "gdt.h"
#define GDT_SIZE 5
/*Accessbyte
7	 0x80	 Present bit
6 u. 5	 0x60	 Privilege
4	 0x10	 Segment bit
3	 0x08	 Executable bit
2	 0x04	 Direction bit/Conforming bit
1	 0x02	 Readable bit/Writable bit
0	 0x01	 Accessed bit
*/
#define GDT_ACCESS_DATASEG 0x02
#define GDT_ACCESS_CODESEG 0x0a
#define GDT_ACCESS_TSS     0x09

#define GDT_ACCESS_SEGMENT 0x10
#define GDT_ACCESS_RING0   0x00
#define GDT_ACCESS_RING3   0x60
#define GDT_ACCESS_PRESENT 0x80
/*Flags
3	 0x8	 Granularity bit
2	 0x4	 Size bit
1	 0x2	 Long Mode bit
0	 0x1	 Available Bit
*/
#define GDT_FLAG_4KUNIT      0x08
#define GDT_FLAG_32_BIT  0x04

struct gdt_entry gdtable[GDT_SIZE];//gdt entries
void gdt_set_entry(uint8_t i,uint32_t limit,uint32_t base,uint8_t accessbyte,uint8_t flags){ // fill in entry i in gdtable
	gdtable[i].limit=limit& 0xffffLL;//
	gdtable[i].base=base & 0xffffffLL;
	gdtable[i].accessbyte=accessbyte & 0xffLL;
	gdtable[i].limit2=(limit>>16) & 0xfLL;
	gdtable[i].flags=flags & 0xfLL;
	gdtable[i].base2=(base>>24) & 0xfLL;
};
void init_gdt(void)
{
    struct {
        uint16_t limit;
        void* pointer;
    } __attribute__((packed)) gdtp = {
        .limit = GDT_SIZE * 8 - 1,
        .pointer = gdtable,
    };

    // We are going to fill in the structs in gdtable
    gdt_set_entry(0, 0, 0, 0,0);
    gdt_set_entry(1, 0xfffff,0, GDT_ACCESS_SEGMENT |
        GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT | GDT_FLAG_4KUNIT);
    gdt_set_entry(2, 0xfffff,0, GDT_ACCESS_SEGMENT |
        GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
    gdt_set_entry(3, 0xfffff,0,  GDT_ACCESS_SEGMENT |
        GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
    gdt_set_entry(4, 0xfffff,0,  GDT_ACCESS_SEGMENT |
        GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);

    // reload GDT
    asm volatile("lgdt %0" : : "m" (gdtp));

    // reload the gdt segmentregisters, so that they are really used
    asm volatile(
        "mov $0x10, %ax;"
        "mov %ax, %ds;"
        "mov %ax, %es;"
        "mov %ax, %ss;"
        "ljmp $0x8, $.1;"
        ".1:"
    );
    kprintf("gdtsuccessfullyinitialized\n",0xA,0x0);
}
