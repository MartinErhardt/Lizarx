/*  <src-path>/src/kernel/inc/intr/irq.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
 *  Copyright (C) 2013, 2014  martin.erhardt98@googlemail.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef IRQ_H
#define IRQ_H

#include<cpu.h>
extern void intr_stub_28(void);
extern void intr_stub_29(void);

extern void intr_stub_32(void);
extern void intr_stub_33(void);

uint_t to_flush;

uint8_t testl;

struct cpu_state* handle_irq(struct cpu_state* cpu);

#endif
