/*   <src-path>/src/kernel/mm/gdt.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *   Copyright (C) 2013  martin.erhardt98@googlemail.com
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <dbg/console.h>
#include <mm/gdt.h>
#include <string.h>
#include <stdint.h>
#include <hal.h>
/*
 * INFO: In this File macros from HAL/x86/macros.h and hw_structs from HAL/x86/hw_structs.h are used
 */
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
    uint32_t tssloc[32] = { 0, 0, 0x10 };
    kprintf("[GDT_INIT] I: GDT setup...");
    memmove(&tss,&tssloc,sizeof(tssloc));
#ifdef ARCH_X86
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
    gdt_set_entry(5,sizeof(tss),(uint32_t) tss,  GDT_ACCESS_TSS | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,0);
#else
    #error lizarx build: No valid arch found in src/kernel/mm/gdt.c
#endif
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
    kprintf("SUCCESS\n");
    // Taskregister neu laden
    asm volatile("ltr %%ax" : : "a" (5 << 3));
}
