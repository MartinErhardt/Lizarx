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

#include<macros.h>
.macro SAVE_CPU
    // save CPU
    push %ebp
    push %edi
    push %esi
    push %edx
    push %ecx
    push %ebx
    push %eax
    pushl $0
    // load Kernel data segment
    mov $(KERNEL_DATA_SEG_N<<3), %ax
    mov %ax, %ds
    mov %ax, %es
.endm
.macro RESTORE_CPU
    // restore CPU
    pop %eax
    pop %ebx
    pop %ecx
    pop %edx
    pop %esi
    pop %edi
    pop %ebp
.endm

.macro INTR_STUB_ERROR nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $\nr
    SAVE_CPU
    // call handler
    push %esp
    call handle_exception
    add $4, %esp
    RESTORE_CPU

    // pop number
    add $8, %esp

    iret
.endm

.macro INTR_STUB_EXCEPTION nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $0
    pushl $\nr
    SAVE_CPU
    // call handler
    push %esp
    call handle_exception
    add $4, %esp
    RESTORE_CPU

    // clean stack from error and intr number
    add $8, %esp

    iret
.endm

.macro INTR_STUB_IRQ nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $0
    pushl $\nr
    SAVE_CPU
    // call handler
    push %esp
    call handle_irq
    mov %eax, %esp
    pop %eax
    cmp $1, %eax
je kernel_mode\nr
    mov $((USER_DATA_SEG_N<<3)|3), %ax
    mov %ax, %ds
    mov %ax, %es
kernel_mode\nr:
    RESTORE_CPU

    // clean stack from error and intr number
    add $8, %esp

    iret
.endm


.macro INTR_STUB_SYSCALL nr
.global intr_stub_\nr
intr_stub_\nr:
    pushl $0
    pushl $\nr
    SAVE_CPU
    // call handler
    push %esp
    call handle_syscall
    mov %eax, %esp
    pop %eax
    cmp $1, %eax
je kernel_mode\nr
    mov $((USER_DATA_SEG_N<<3)|3), %ax
    mov %ax, %ds
    mov %ax, %es
kernel_mode\nr:
    RESTORE_CPU

    // clean stack from error and intr number
    add $8, %esp

    iret
.endm

.global idle_thread
idle_thread:
	hlt
	jmp idle_thread
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
// APIC Error Interrupt
INTR_STUB_IRQ 30
// APIC Spurious Interrupt
INTR_STUB_IRQ 31
// IRQs
INTR_STUB_IRQ 32
INTR_STUB_IRQ 33


// Syscall
INTR_STUB_SYSCALL 48

