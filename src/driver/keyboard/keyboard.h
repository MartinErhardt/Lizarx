#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
/**
 * Scancode in einen Keycode uebersetzen
 * @return Keycode oder 0 falls der Scancode nicht bekannt ist
 */
void send_command(uint8_t command);
void init_keyboard(void);
void kbc_handler(uint8_t irq);
#endif

