#include <stdint.h>
#include "keyboard.h"
#include <console.h>
#include <ioport.h>


void init_keyboard(void){
    // Tastaturpuffer leeren
    while (inb(0x64) & 0x1) {
        inb(0x60);
    }

    // Tastatur aktivieren
    send_command(0xF4);

};

// Befehl an die Tastatur senden 
void send_command(uint8_t command)
{
    // Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
    while ((inb(0x64) & 0x2)) {}

    outb(0x60, command);
};
void irq_handler(uint8_t irq) {
  
    uint8_t scancode=0;
    
    scancode = inb(0x60);
    kprintf("Keycode: 0x%p",scancode);
};
