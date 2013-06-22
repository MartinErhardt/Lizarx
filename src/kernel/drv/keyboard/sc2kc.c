/*   <src-path>/src/kernel/drv/keyboard/sc2kc.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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

#include <dbg/console.h>
#include <drv/keyboard/keyboard.h>


/**
 * Scancode => Keycode Uebersetzungstabelle fuer Scancode Set 1
 */
static uint8_t sc_to_kc[][128] = {
    // Normale Scancodes
    {
          0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
         10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
         20,  21,  22,  23,  24,  25,  26,  27,  28,  29,
         30,  31,  32,  33,  34,  35,  36,  37,  38,  39,
         40,  41,  42,  43,  44,  45,  46,  47,  48,  49,
         50,  51,  52,  53,  54,  55,  56,  57,  58,  59,
         60,  61,  62,  63,  64,  65,  66,  67,  68,  69,
         70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
         80,  81,  82,  84,  00,  00,  86,  87,  88,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00
    },

    // Extended0-Scancodes (werden mit e0 eingeleitet)
    {
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  96,  97,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  99,  00,  00, 100,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00, 102, 103, 104,  00, 105,  00, 106,  00, 107,
        108, 109, 110, 111,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00,  00,  00,
         00,  00,  00,  00,  00,  00,  00,  00
    },
};

/**
 * Scancode in einen Keycode uebersetzen
 *
 * @param set Zu Benutztende Tabelle:
 *              0: Normale Scancodes
 *              1: Extended0 Scancodes
 *              2: Extended1 Scancodes
 *
 * @param code Scancode; keine Breakcodes nur normale Scancodes
 *             Fuer e1 den zweiten Scancode im hoeherwertigen Byte uebergeben
 *
 * @return Keycode oder 0 falls der Scancode nicht bekannt ist
 */
uint8_t translate_scancode(int set, uint16_t scancode)
{
    uint8_t keycode = 0;

    switch (set) {
        // Normale scancodes
        case 0:
            keycode = sc_to_kc[0][scancode];
            break;

        // e0-Scancodes
        case 1:
            keycode = sc_to_kc[1][scancode];
            break;

        // e1-Scancodes
        case 2:
            // TODO: Hier waere eigentlich eine Tabelle auch schoen
            switch (scancode) {
                // Pause
                case 0x451D:
                    keycode = 119;
                    break;

                default:
                    keycode = 0x0;
            };
            break;
    }

    if (keycode == 0) {
        kprintf("kbc: Unbekannter Scancode: 0x%x (%d)\n", scancode, set);
    }

    return keycode;
}