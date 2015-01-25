/*  <src-path>/src/kernel/HAL/x86_64/asm/trampoline.S is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
// WARNING compare to src/kernel/inc/HAL/x86_common/local_apic.h

#define TRAMPOLINE_ENTRY_FUNC_SIZE 	0x300
#define TRAMPOLINE_GDT_SIZE		0x20
#define TRAMPOLINE_GDTR_SIZE		0x8
#define TRAMPOLINE_GDT64_SIZE		0x20
#define TRAMPOLINE_GDTR64_SIZE		0x8
#define TRAMPOLINE_PMODE_SIZE		0x100
#define TRAMPOLINE_LMODE_SIZE		0x50

// WARNING compare to src/kernel/inc/HAL/x86_64/macros.h
.extern trampoline
#define TRAMPOLINE			0x8000
#define TRAMPOLINE_GDT			TRAMPOLINE+TRAMPOLINE_ENTRY_FUNC_SIZE
#define TRAMPOLINE_GDTR			TRAMPOLINE_GDT+TRAMPOLINE_GDT_SIZE
#define TRAMPOLINE_GDT64		TRAMPOLINE_GDTR+TRAMPOLINE_GDTR_SIZE
#define TRAMPOLINE_GDTR64		TRAMPOLINE_GDT64+TRAMPOLINE_GDT64_SIZE
#define TRAMPOLINE_LMODE		TRAMPOLINE_GDTR64+TRAMPOLINE_GDTR64_SIZE
#define TRAMPOLINE_PMODE		TRAMPOLINE_LMODE+TRAMPOLINE_LMODE_SIZE
#define TRAMPOLINE_LOCK			TRAMPOLINE_PMODE+TRAMPOLINE_PMODE_SIZE
#define TRAMPOLINE_VBEINFO		TRAMPOLINE_LOCK+0x8

.extern AP_init
.extern startup_context

.code16
.global trampoline_entry_func
trampoline_entry_func:
	cli
	mov $(TRAMPOLINE+0xf00), %ax
	mov %ax, %sp
	
	call vesa_setup
	
	xor %ax, %ax
	mov %ax, %ds
	
	lgdt TRAMPOLINE_GDTR
	mov %cr0, %eax
	or $0x01, %eax
	mov %eax, %cr0
	
	ljmp $0x08, $(TRAMPOLINE_PMODE)
.global vesa_setup
vesa_setup:
	//mov $TRAMPOLINE_VBEINFO, %di
	//xor %ax, %ax
	//mov %ax, %es
	//mov $0x4f00, %ax
	//int $0x10
	//jmp .
	ret
.code32
.global pmode
pmode:
	
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
		
	mov startup_context, %eax
	mov %eax, %cr3
	
	mov %cr4, %eax
	or $0x20, %eax
	mov %eax, %cr4
	
	mov $0xC0000080, %ecx
	rdmsr
	or $0x00000100, %eax
	wrmsr
	
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	
	lgdt TRAMPOLINE_GDTR64
	
	ljmp $0x08, $(TRAMPOLINE_LMODE)
.code64
.global lmode
lmode:
	cli
	mov $AP_init, %rax
	jmp *%rax
	jmp .
.global gdt
gdt:
	// NULL descriptor
	.int	0x00000000
	.int	0x00000000
	// kernel-code
	.short	0xffff
	.short	0x0000
	.byte	0x00
	.byte	0x9a
	.byte	0xcf
	.byte	0x00
	// kernel-data
	.short	0xffff
	.short	0x0000
	.byte	0x00
	.byte	0x92
	.byte	0xcf
	.byte	0x00
.global gdtr
gdtr:
	.short	0x17
	.long	TRAMPOLINE_GDT
.global gdt64
gdt64:
	// NULL descriptor
	.int	0x00000000
	.int	0x00000000
	// kernel-code
	.short	0xffff
	.short	0x0000
	.byte	0x00
	.byte	0x9a
	.byte	0x2f
	.byte	0x00
	// kernel-data
	.short	0xffff
	.short	0x0000
	.byte	0x00
	.byte	0x92
	.byte	0x2f
	.byte	0x00
.global gdtr64
gdtr64:
	.short	0x17
	.long	TRAMPOLINE_GDT64
.global VbeInfoBlock
VbeInfoBlock:
	.int 0
	.int 0
	.int 0
	.int 0

