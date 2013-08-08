/*  <src-path>/src/kernel/mm/gdt.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *  Copyright (C) 2013  martin.erhardt98@googlemail.com
 *
 *  Lizarx is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Lizarx is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU LESSER General Public License
 *  along with Lizarx.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <dbg/console.h>
#include <mm/gdt.h>
#include <string.h>
#include <stdint.h>
#include <hal.h>
#include <mm/gdt.h>

/*
#ifndef GDT_FLAG_32_BIT
	#define GDT_FLAG_32_BIT 0x2
#endif*/
/*
 * INFO: In this File macros from HAL/x86/macros.h and hw_structs from HAL/x86/hw_structs.h are used
 */
#ifdef ARCH_X86
struct gdt_entry gdtable[GDT_SIZE];//gdt entries
#endif
#ifdef ARCH_X86_64
struct gdt_entry * gdtable=(struct gdt_entry *)0x114000;
#endif
void gdt_set_entry(unsigned char i,unsigned int limit,unsigned int base,unsigned char accessbyte,unsigned char flags) // fill in an entry in the gdttable
{
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
#ifdef ARCH_X86
		.limit = GDT_SIZE * sizeof(struct gdt_entry) - 1,
#endif
#ifdef ARCH_X86_64
		.limit = ( (GDT_SIZE-1) * sizeof(struct gdt_entry) ) + sizeof( struct gdt_tss_entry ) - 1,
#endif
	    .pointer = gdtable,
	};
	
	kprintf("[GDT] I: init_gdt...");
	memset(&tss,0x00000000, sizeof(struct tss_t));
	// We are going to fill in the structs in gdtable
	gdt_set_entry(0, 0, 0, 0,0);
	gdt_set_entry(1, 0xffffffff,0, GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT | GDT_FLAG_4KUNIT);
	gdt_set_entry(2, 0xffffffff,0, GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(3, 0xffffffff,0,  GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(4, 0xffffffff,0,  GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(5,sizeof(tss),(uintptr_t) &tss,  GDT_ACCESS_TSS | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,0);
	// reload GDT
	asm volatile("lgdt %0" : : "m" (gdtp));
#ifdef ARCH_X86
	tss.ss0=0x10;
	// reload the gdt segmentregisters, so that they are really used
	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.1;"
		".1:"
	);
	asm volatile("ltr %%ax" : : "a" (5 << 3));
#endif
	kprintf("SUCCESS\n");
      //while(1);
	// Taskregister neu laden
}
#ifdef ARCH_X86_64
void setup_tss()
{
	struct gdt_tss_entry * tss_desc = (struct gdt_tss_entry *)&gdtable[5];
	tss_desc->limit=sizeof(struct gdt_tss_entry) & 0xffffLL;
	tss_desc->base=(uintptr_t) &tss & 0xffffffLL;
	tss_desc->accessbyte=(GDT_ACCESS_TSS | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3) & 0xffLL;
	tss_desc->limit2=(sizeof(struct gdt_tss_entry)>>16) & 0xfLL;
	tss_desc->flags=GDT_FLAG_32_BIT & 0xfLL;
	tss_desc->base2=(((uintptr_t) &tss)>>24) & 0xffffffffffLL;
	asm volatile("ltr %%ax" : : "a" (0x2b));
}
#endif
