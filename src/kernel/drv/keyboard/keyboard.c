#include <stdint.h>
#include <drv/keyboard/keyboard.h>
#include <dbg/console.h>
#include <drv/io/ioport.h>
#include <stdbool.h>

static void clearkbcbuffer();

static bool init_done = FALSE;

void init_keyboard(void){
    
    clearkbcbuffer();
    // Leds alle ausloeschen
    send_command(0xED);
    outb(0x60, 0);

    // Schnellste Wiederholrate
    send_command(0xF3);
    outb(0x60, 0);

    // Tastatur aktivieren
    send_command(0xF4);
    
    // Tastatur aktivieren
    send_command(0xF4);
    
    clearkbcbuffer();
    init_done = TRUE;
};

// Befehl an die Tastatur senden 
void send_command(uint8_t command)
{
    // Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
    while ((inb(0x64) & 0x2)) {}

    outb(0x60, command);
};
void kbc_handler(uint8_t irq) {
    
    uint8_t scancode;
    //uint8_t keycode = 0;
    
    // Abbrechen wenn die Initialisierung noch nicht abgeschlossen wurde
    if (!init_done) {
        return;
    }

    scancode = inb(0x60);
    // Zum Testen sollte folgendes verwendet werden:
    kprintf("0x%x", scancode);
    //Nach erfolgreichen Tests, könnte eine send_key_event Funtkion wie bei Týndur verwendet werden
};
static void clearkbcbuffer(){
    // Tastaturpuffer leeren
    while (inb(0x64) & 0x1) {
        inb(0x60);
    }
    return;
}