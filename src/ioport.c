#include<stdint.h>
#include"ioport.h"
/* writes a byte into an I/O-Port */
void outb(uint16_t port, uint8_t data)
{
    asm volatile ("outb %0, %1" : : "a" (data), "Nd" (port));
}
/* reads a Byte from an I/O-Port */
uint8_t inb(uint16_t port){
	uint8_t result;
	asm ("inb %1, %0" : "=a" (result) : "Nd" (port));
	return result;
}
