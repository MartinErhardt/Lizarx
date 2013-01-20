.extern handle_exception
.extern handle_irq
.extern handle_syscall
.extern save_cpu
.extern restore_cpu

.macro SAVE_CPU
    // save CPU
    push %ebp
    push %edi
    push %esi
    push %edx
    push %ecx
    push %ebx
    push %eax
    
    // Kernel-Datensegmente laden
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
.endm
.macro RESTORE_CPU
    // User-Datensegmente laden
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es
    
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

    // Fehlercode und Interruptnummer vom Stack nehmen
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

    // Fehlercode und Interruptnummer vom Stack nehmen
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
    RESTORE_CPU

    // Fehlercode und Interruptnummer vom Stack nehmen
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
    add $4, %esp
    RESTORE_CPU

    // Fehlercode und Interruptnummer vom Stack nehmen
    add $8, %esp

    iret
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

// IRQs
INTR_STUB_IRQ 32
INTR_STUB_IRQ 33

// Syscall
INTR_STUB_SYSCALL 48
