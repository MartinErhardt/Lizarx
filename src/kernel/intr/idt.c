/*   <src-path>/src/kernel/intr/idt.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <drv/io/ioport.h>
#include <intr/idt.h>
#include <dbg/console.h>
#include <intr/irq.h>
#include <intr/err.h>
#include <intr/syscall.h>
#include <mt/cpustate.h>
#include <mm/vmm.h>
#include <stdbool.h>
/*----------------------------------------------------------------------------PIC-----------------------------------------------------------------------------------------*/
#define MASTER_PIC_COMMAND 0x20
#define MASTER_PiC_DATA 0x21
#define SLAVE_PIC_COMMAND 0xa0
#define SLAVE_PiC_DATA 0xa1

#define PIC_INIT 0x11
#define ICW_4 0x01

#define IRQ_BASE 0x20

/*----------------------------------------------------------------------------IDT-----------------------------------------------------------------------------------------*/
#define IDT_SIZE 256
#define GDT_KERNEL_CODE_SEGMENT 0x8
#define IDT_FLAG_INTERRUPT_GATE 0xe
#define IDT_FLAG_PRESENT 0x80
#define IDT_FLAG_RING0 0x00
#define IDT_FLAG_RING3 0x60

struct idt_entry idt[IDT_SIZE];

bool intr_activated=FALSE;
static void idt_set_entry(int i, void (*fn)(), unsigned int selector,
    int flags)
{
    unsigned long int handler = (unsigned long int) fn;
    idt[i].isr_offset = handler & 0xffffLL;
    idt[i].selector = selector & 0xffffLL;
    idt[i].ignore = 0x00;
    idt[i].flags = flags & 0xffLL;
    idt[i].isr_offset2 = (handler>> 16) & 0xffffLL;
}
void init_idt(void)
{
    struct {
        unsigned short int limit;
        void* pointer;
    } __attribute__((packed)) idtp = {
        .limit = IDT_SIZE * 8 - 1,
        .pointer = idt,
    };
    // Interruptnummern der IRQs umbiegen
    
    // init master-PIC
    outb(MASTER_PIC_COMMAND, PIC_INIT);
    outb(MASTER_PiC_DATA, IRQ_BASE);
    outb(MASTER_PiC_DATA, 0x04); // slave at IRQ 2
    outb(MASTER_PiC_DATA, ICW_4);

    // init slave-PIC
    outb(SLAVE_PIC_COMMAND, PIC_INIT);
    outb(SLAVE_PiC_DATA, 0x28); // interruptnumber for IRQ 8
    outb(SLAVE_PiC_DATA, 0x02); // slave at IRQ 2
    outb(SLAVE_PiC_DATA, ICW_4);

    // activate all IRQs(demasc)
    outb(MASTER_PIC_COMMAND, 0x0);
    outb(SLAVE_PIC_COMMAND, 0x0);
    
    // Exception-Handler
    idt_set_entry(0, intr_stub_0, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(1, intr_stub_1, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(2, intr_stub_2, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(3, intr_stub_3, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(4, intr_stub_4, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(5, intr_stub_5, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(6, intr_stub_6, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(7, intr_stub_7, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(8, intr_stub_8, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(9, intr_stub_9, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(10, intr_stub_10, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(11, intr_stub_11, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(12, intr_stub_12, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(13, intr_stub_13, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(14, intr_stub_14, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(15, intr_stub_15, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(16, intr_stub_16, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(17, intr_stub_17, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(18, intr_stub_18, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

    // IRQ-Handler
    idt_set_entry(32, intr_stub_32, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);
    idt_set_entry(33, intr_stub_33, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING0 | IDT_FLAG_PRESENT);

    // Syscall
    idt_set_entry(48, intr_stub_48, GDT_KERNEL_CODE_SEGMENT, IDT_FLAG_INTERRUPT_GATE | IDT_FLAG_RING3 | IDT_FLAG_PRESENT);
    asm volatile("lidt %0" : : "m" (idtp));
    kprintf("[INIT] IDT setup... SUCCESS\n");
}
bool is_intrenabled(){
	return intr_activated;
}
void enable_intr()
{
    intr_activated=TRUE;
    //while(1){}
    asm volatile("sti");
    kprintf("[INIT] Interrrupts enabled\n");
};
