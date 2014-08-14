/*   <src-path>/src/kernel/HAL/i386/asm/trampoline.S is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
 * 
 *   Copyright (C) 2013  martin.erhardt98@googlemail.com
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
// WARNING compare to src/kernel/inc/HAL/x86_common/local_apic.h

#define TRAMPOLINE_ENTRY_FUNC_SIZE 	0x300
#define TRAMPOLINE_GDT_SIZE		0x20
#define TRAMPOLINE_GDTR_SIZE		0x8
#define TRAMPOLINE_GDT64_SIZE		0x20
#define TRAMPOLINE_GDTR64_SIZE		0x8
#define TRAMPOLINE_PMODE_SIZE		0x100
#define TRAMPOLINE_LMODE_SIZE		0x50

// WARNING compare to src/kernel/inc/HAL/x86_64/macros.h

#define TRAMPOLINE			0x7000 
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
	mov $0x7f00, %ax
	mov %ax, %sp
	
	xor %ax, %ax
	mov %ax, %ds
	
	lgdt TRAMPOLINE_GDTR
	mov %cr0, %eax
	or $0x01, %eax
	mov %eax, %cr0
	
	ljmp $0x08, $TRAMPOLINE_PMODE
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
	
	mov %cr0, %eax
	or $0x80000000, %eax
	mov %eax, %cr0
	
	mov $AP_init, %eax
	jmp *%eax
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
	.int	TRAMPOLINE_GDT
