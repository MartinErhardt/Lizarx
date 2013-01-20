#ifndef INTR_H
#define INTR_H
#include<stdint.h>
struct idt_entry
{
    uint16_t isr_offset;
    uint16_t selector;
    uint8_t ignore;
    uint8_t flags;
    uint16_t isr_offset2;
}__attribute__((packed));
void init_idt(void);
#endif
