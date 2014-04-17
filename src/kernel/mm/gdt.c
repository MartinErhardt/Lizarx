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
#include <hw_structs.h>
#include <macros.h>
#include <mm/vheap.h>

#ifndef LOADER

#include<mm/vmm.h>

#endif
/*
 *  * INFO: In this File macros from HAL/x86/macros.h and hw_structs from HAL/x86/hw_structs.h are used
 */
#ifdef ARCH_X86
struct gdt_entry gdtable[GDT_SIZE];//gdt entries
#endif
#ifdef ARCH_X86_64
struct gdt_entry * gdtable=(struct gdt_entry *)0x114000;
#endif
void gdt_set_entry(struct gdt_entry * gdttable_tofill,uint8_t i,uint16_t limit,uint32_t base,uint8_t accessbyte,uint8_t flags) // fill in an entry in the gdttable
{
	gdttable_tofill[i].limit=limit;
	gdttable_tofill[i].base=base & 0xffffffLL;
	gdttable_tofill[i].accessbyte=accessbyte & 0xffLL;
	gdttable_tofill[i].limit2=(limit>>16) & 0xfLL;
	gdttable_tofill[i].flags=flags & 0xfLL;
	gdttable_tofill[i].base2=(base>>24) & 0xfLL;
};
void init_gdt(void)
{
	struct {
	    uint16_t limit;
	    uintptr_t pointer;
	} __attribute__((packed)) gdtp = {
#ifdef ARCH_X86
		.limit = GDT_SIZE * sizeof(struct gdt_entry) - 1,
#endif
#ifdef ARCH_X86_64
		.limit = ( (GDT_SIZE-1) * sizeof(struct gdt_entry) ) + sizeof( struct gdt_tss_entry ) - 1,
#endif
	    .pointer = ((uintptr_t)gdtable),
	};
	kprintf("[GDT] I: init_gdt...");
	memset(&tss,0x00000000, sizeof(struct tss_t));
	// We are going to fill in the structs in gdtable
	gdt_set_entry(gdtable,0, 0, 0, 0,0);
	gdt_set_entry(gdtable,1, 0xffff,0, GDT_ACCESS_SEGMENT |GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT | GDT_FLAG_4KUNIT);
	gdt_set_entry(gdtable,2, 0xffff,0, GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(gdtable,3, 0xffff,0,  GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(gdtable,4, 0xffff,0,  GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(gdtable,5,sizeof(struct tss_t),(uintptr_t) &tss,  GDT_ACCESS_TSS | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,0);
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
#ifndef LOADER
void init_gdt_AP(void)
{
	
#ifdef ARCH_X86
	uint16_t gdt_limit = GDT_SIZE * sizeof(struct gdt_entry) - 1;
#endif
#ifdef ARCH_X86_64
	uint16_t gdt_limit= ( (GDT_SIZE-1) * sizeof(struct gdt_entry) ) + sizeof( struct gdt_tss_entry ) - 1;
#endif
	
	struct gdt_entry * gdtable_ap=((struct gdt_entry *)kmalloc(gdt_limit+1));
	struct {
	    uint16_t limit;
	    uintptr_t pointer;
	} __attribute__((packed)) gdtp = {
	    .limit = gdt_limit,
	    .pointer = ((uintptr_t)gdtable_ap),
	};
	gdt_set_entry(gdtable_ap,0, 0, 0, 0,0);
	gdt_set_entry(gdtable_ap,1, 0xffff,0, GDT_ACCESS_SEGMENT |GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT | GDT_FLAG_4KUNIT);
	gdt_set_entry(gdtable_ap,2, 0xffff,0, GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(gdtable_ap,3, 0xffff,0,  GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_CODESEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	gdt_set_entry(gdtable_ap,4, 0xffff,0,  GDT_ACCESS_SEGMENT |
	    GDT_ACCESS_DATASEG | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,GDT_FLAG_32_BIT |GDT_FLAG_4KUNIT);
	
	struct tss_t * tss_ap =(struct tss_t *) kmalloc(sizeof(struct tss_t));
	memset(tss_ap,0x00000000, sizeof(struct tss_t));
	
	gdt_set_entry(gdtable_ap,5,sizeof(struct tss_t),(uintptr_t) tss_ap,  GDT_ACCESS_TSS | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3,0);
#ifdef ARCH_X86
	tss_ap->esp0=((uintptr_t)kvmm_malloc(0x1000))+0xff0;
	tss_ap->ss0=0x10;
	asm volatile("lgdt %0" : : "m" (gdtp));
	// reload the gdt segmentregisters, so that they are really used
	asm volatile(
		"mov $0x10, %ax;"
		"mov %ax, %ds;"
		"mov %ax, %es;"
		"mov %ax, %ss;"
		"ljmp $0x8, $.2;"
		".2:"
	);
	asm volatile("ltr %%ax" : : "a" (5 << 3));
	return;
#endif
	
#ifdef ARCH_X86_64
	tss_ap->rsp0=((uintptr_t)kvmm_malloc(0x2000))+0xff0;
	struct gdt_tss_entry * tss_desc = (struct gdt_tss_entry *)&(gdtable_ap[5]);
	tss_desc->limit		= sizeof(struct gdt_tss_entry) & 0xffffLL;
	tss_desc->base		= ((uintptr_t) tss_ap) & 0xffffffLL;
	tss_desc->accessbyte	= (GDT_ACCESS_TSS | GDT_ACCESS_PRESENT | GDT_ACCESS_RING3) & 0xffLL;
	tss_desc->limit2	= (sizeof(struct gdt_tss_entry)>>16) & 0xfLL;
	tss_desc->flags		= GDT_FLAG_32_BIT & 0xfLL;
	tss_desc->base2		= (((uintptr_t) tss_ap)>>24) & 0xffffffffffLL;
	tss_desc->reserved	= 0;
	tss_desc->zero		= 0;
	tss_desc->reserved2	= 0;
	asm volatile("lgdt %0" : : "m" (gdtp));
	asm volatile("ltr %%ax" : : "a" (0x2b));
#endif
}
#endif
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
	tss_desc->reserved=0;
	tss_desc->zero=0;
	tss_desc->reserved2=0;
	asm volatile("ltr %%ax" : : "a" (0x2b));
}
#endif
