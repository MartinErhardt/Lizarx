/*   <src-path>/src/kernel/drv/keyvoard/keyboard.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include <stdint.h>
#include <drv/keyboard/keyboard.h>
#include <dbg/console.h>
#include <hal.h>
#include <stdbool.h>

static void clearkbcbuffer();

static bool init_done = FALSE;

void init_keyboard(void)
{
    clearkbcbuffer();
    // Leds alle ausloeschen
    send_command(0xED);
    OUTB(0x60, 0);

    // Schnellste Wiederholrate
    send_command(0xF3);
    OUTB(0x60, 0);

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
    uint8_t kbc_buffer_state=0x0;
    INB(0x64,kbc_buffer_state)
    // Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
    while (kbc_buffer_state & 0x2) {INB(0x64,kbc_buffer_state)}

    OUTB(0x60, command);
};
void kbc_handler(uint8_t irq) {
    
    uint8_t scancode=0;
    
    // Abbrechen wenn die Initialisierung noch nicht abgeschlossen wurde
    if (!init_done) {
        return;
    }

    INB(0x60,scancode);
    // Zum Testen sollte folgendes verwendet werden:
    kprintf("0x%x", scancode);
    //Nach erfolgreichen Tests, könnte eine send_key_event Funtkion wie bei Týndur verwendet werden
};
static void clearkbcbuffer()
{
    uint8_t kbc_buffer_state=0x0;
    INB(0x64,kbc_buffer_state)
    while (kbc_buffer_state & 0x1) {
        INB(0x64,kbc_buffer_state)
    }
    return;
}