/*   <src-path>/src/kernel/inc/drv/keyboard/keyboard.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
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
uint8_t translate_scancode(int set, uint16_t scancode);

void send_command(uint8_t command);
void init_keyboard(void);
void kbc_handler(uint8_t irq);
#endif
