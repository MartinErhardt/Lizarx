#ifndef GDT_H
#define GDT_H
#include <stdint.h>
struct gdt_entry{
	uint16_t limit;
	uint32_t base :24;
	uint8_t accessbyte;
	uint32_t limit2 :4;
	uint32_t flags :4;
	uint8_t base2;
}__attribute__((packed));
void gdt_set_entry(uint8_t i,uint32_t limit,uint32_t base,uint8_t accessbyte,uint8_t flags);
void init_gdt(void);
#endif
