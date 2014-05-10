/*   <src-path>/src/kernel/HAL/x86/asm/intr_stub.S is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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

.extern handle_exception
.extern handle_irq
.extern handle_syscall
.extern save_cpu
.extern restore_cpu
.extern syscall

#include<macros.h>

.macro SAVE_CPU
    // save CPU
    
    push %r15
    push %r14
    push %r13
    push %r12
    push %r11
    push %r10
    push %r9
    push %r8
    push %rbp
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %rbx
    push %rax
    // load kernel data segment
    mov $(KERNEL_DATA_SEG_N<<3), %ax
    mov %ax, %ds
    mov %ax, %es
.endm
.macro RESTORE_CPU
    // load user data segment
    //test (%rsp) 
    mov $((USER_DATA_SEG_N<<3)|3), %ax
    mov %ax, %ds
    mov %ax, %es
    
    // restore CPU
    pop %rax
    pop %rbx
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi
    pop %rbp
    pop %r8
    pop %r9
    pop %r10
    pop %r11
    pop %r12
    pop %r13
    pop %r14
    pop %r15
    
.endm

.macro INTR_STUB_ERROR nr
.global intr_stub_\nr
intr_stub_\nr:

    push $\nr
    SAVE_CPU
    // call handler
    mov %rsp, %rdi
    call handle_exception
    add $8, %esp
    RESTORE_CPU

    // clean stack from error and intr number
    add $0x10, %rsp

    iretq
.endm

.macro INTR_STUB_EXCEPTION nr
.global intr_stub_\nr
intr_stub_\nr:

    push $0
    push $\nr
    SAVE_CPU
    // call handler
    mov %rsp, %rdi
    call handle_exception
    add %rax, %rsp
    RESTORE_CPU

    // clean stack from error and intr number
    add $0x10, %rsp

    iretq
.endm

.macro INTR_STUB_IRQ nr
.global intr_stub_\nr
intr_stub_\nr:

    push $0
    push $\nr
    SAVE_CPU
    //
    // call handler
    mov %rsp, %rdi
    call handle_irq
    mov %rax, %rsp

    RESTORE_CPU

    // clean stack from error and intr number
    add $0x10, %rsp
    iretq
.endm


.macro INTR_STUB_SYSCALL nr
.global intr_stub_\nr
intr_stub_\nr:

    push $0
    push $\nr
    SAVE_CPU
    // call handler
    mov %rsp, %rdi
    call handle_syscall
    mov %rax, %rsp
    RESTORE_CPU

    // clean stack from error and intr number
    add $0x10, %rsp

    iretq
.endm

// Exceptions

INTR_STUB_EXCEPTION 0
INTR_STUB_EXCEPTION 1
INTR_STUB_EXCEPTION 2
INTR_STUB_EXCEPTION 3
INTR_STUB_EXCEPTION 4
INTR_STUB_EXCEPTION 5
INTR_STUB_EXCEPTION 6
INTR_STUB_EXCEPTION 7
INTR_STUB_ERROR 8
INTR_STUB_EXCEPTION 9
INTR_STUB_ERROR 10
INTR_STUB_ERROR 11
INTR_STUB_ERROR 12
INTR_STUB_ERROR 13
INTR_STUB_ERROR 14
INTR_STUB_EXCEPTION 15
INTR_STUB_EXCEPTION 16
INTR_STUB_ERROR 17

INTR_STUB_EXCEPTION 18

// Flush TLB
INTR_STUB_IRQ 28

// APIC Timer
INTR_STUB_IRQ 29
// APIC Error
INTR_STUB_IRQ 30
//APIC Spurious interrupt
INTR_STUB_IRQ 31
// IRQs
INTR_STUB_IRQ 32
INTR_STUB_IRQ 33

// Syscall
INTR_STUB_SYSCALL 48

.global syscall_stub
syscall_stub:    
	swapgs
	
	movl $((USER_DATA_SEG_N<<3)|3), %gs:-0x8 //ds
	mov %rsp, 			%gs:-0x10 // rsp
	movl $0x202, 			%gs:-0x18 //rflags
	movl $((USER_CODE_SEG64_N<<3)|3), %gs:-0x20 //cs
	movl $0,			%gs:-0x28 //rip
	movl $0,			%gs:-0x30 // err
	movl $0,			%gs:-0x38 // intr
	mov %r15,			%gs:-0x40
	mov %r14,			%gs:-0x48
	mov %r13,			%gs:-0x50
	mov %r12,			%gs:-0x58
	mov %r11,			%gs:-0x60
	mov %r10,			%gs:-0x68
	mov %r9,			%gs:-0x70
	mov %r8,			%gs:-0x78
	mov %rbp,			%gs:-0x80
	mov %rdi,			%gs:-0x88
	mov %rsi,			%gs:-0x90
	mov %rdx,			%gs:-0x98
	mov %rcx,			%gs:-0xa0
	mov %rbx,			%gs:-0xa8
	mov %rax,			%gs:-0xb0
	
	swapgs
	
	mov $0xc0000102, %rcx
	rdmsr
	mov %rax, %rsp
	
	sub $0xb0, %rsp
	
	// load kernel data segment
	mov $(KERNEL_DATA_SEG_N<<3), %ax
	mov %ax, %ds
	mov %ax, %es
	
	// call handler
	mov %rsp, %rdi
	call handle_syscall
	mov %rax, %rsp
	
	RESTORE_CPU
	
	add $0x28, %esp
	
	pop %rsp
	sti
	sysretq
.global idle_thread
idle_thread:
	hlt
	jmp idle_thread