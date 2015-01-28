/*  <src-path>/src/kernel/boot/boot_longmode.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
 *  Copyright (C) 2013, 2014  martin.erhardt98@googlemail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <boot/multiboot.h>
#include <dbg/console.h>
#include <gdt.h>
#include <boot/easy_map.h>
#include <mt/elf.h>
#include <string.h>
#include <macros.h>

extern const void kernel_stack;
static void load_kernel(struct elf_header* header);

void init_LM(struct multiboot_info * mb_info)
{
	struct multiboot_module* modules = (void*)mb_info->mbs_mods_addr;
	
	clrscr(VGA_BLACK,VGA_WHITE);
	/*
	 * Prepare some stuff for later (load kernel ELF setup GDT and Paging structures )
	 */
	init_gdt();
	kprintf("[LM_loader] I: init_LM ...");
	load_kernel((struct elf_header*)(modules[0].mod_start));
	pmm_init_mmap(mb_info);
	init_easymap();

	asm volatile("nop" :: "c" (easy_map_tbl));
	asm volatile("mov %%eax, %%edi" : : "a" ((uint32_t)mb_info));
	asm volatile("nop":: "a"(bsp_stack+STDRD_STACKSIZ-0x10));
	asm volatile("mov %eax , %esp;");
	asm volatile
	(
		/*
		 * 64 bit supported ? 
		 * I outcommented that, because it doesn't work on real HW yet 
		 */
		/*
		"mov 0x80000001, %eax;"
		"cpuid;"
		"and $0x20000000, %edx;"
		"test %edx, %edx;"
		"jz LM_nosupport;"
		"mov $0x00000001, %eax;"
		"cpuid;"
		"and $0x00000040, %edx;"
		"test %edx, %edx;"
		"jz LM_nosupport;"
		*/
		/* 
		 * Load 64 bit segment registers
		 */
		"mov $0x18, %ax;"
		"mov %ax, %ss;"
		"mov %ax, %fs;"
		"mov %ax, %ds;"
		"mov %ax, %gs;"
		/*
		 * Let's set the PAE(Physical address extension) bit
		 */
		"mov %cr4, %eax ; "
		"or $0x20, %eax ;"
		"mov %eax, %cr4 ;"
		/* 
		 * load Pagemap level4 address into cr3 so that we can problemless enable Paging later 
		 */
		"mov %ecx, %eax;"
		"mov %eax, %cr3;"
		/*
		 * here EFER.NXE was set, if I outcommented it.
		 * EFER.NXE allows non-executable Pages which can improve security in further versions
		 * but we are only in Kernel Mode yet so it doesn't really help us
		 
		
		"mov $0xC0000008, %ecx ;"
		"rdmsr ;"
		"or $0x00000001, %eax ;"
		"wrmsr ;"
		*/

		
		/*
		 * Here we actually enable Long mode in the MSR(Machine specific Register) 
		 */
		"mov $0xC0000080, %ecx ;"
		"rdmsr ;"
		"or $0x00000100, %eax ;"
		"wrmsr ;"
		/*
		 * now we are going to enable Paging(from now on we are in Long mode)
		 */
		"mov %cr0, %eax;"
		"or $0x80000000, %eax ;"
		"mov %eax, %cr0;"

		/* 
		 * But we need to jump inside a Long mode code segment to start executing AMD64 code
		 */
		"ljmp $0x8, $0x120000;"
/* 
 * we only come here if we are running on a i386 like architecture
 */
		//"LM_nosupport:"
	);
	kprintf( "ERROR\n [LM_LOADER] init_LM: longmode not supported ... FAIL\n" );
	
}
void load_kernel(struct elf_header* header)
{
	struct elf_program_header * ph = (struct elf_program_header*) (((char*) header) + header->ph_offset);
	int i = 0;
	for (i = 0; i < header->ph_entry_count; i++, ph++)
	{
		void* dest = (void*) ((uint32_t)ph->virt_addr);
		void* src = ((char*) header) + ph->offset;
		/* Nur Program Header vom Typ LOAD laden */
		
		if (ph->type != ELF_PROGRAM_TYPE_LOAD)
		{
			continue;
		}
		memset(dest, 0x00000000, ph->mem_size);
		memcpy(dest, src, ph->file_size);
	}
}
