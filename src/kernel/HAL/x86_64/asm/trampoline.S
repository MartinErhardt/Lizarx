/*   <src-path>/src/kernel/HAL/x86_64/asm/trampoline.S is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
	
	lgdt 0x7120
	mov %cr0, %eax
	or $0x01, %eax
	mov %eax, %cr0
	
	ljmp $0x08, $0x71a0
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
	
	lgdt 0x7148
	
	ljmp $0x08, $0x7150
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
	.long	0x7100
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
	.long	0x7128
