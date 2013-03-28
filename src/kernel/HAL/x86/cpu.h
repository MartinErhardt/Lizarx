/*   <src-path>/src/kernel/HAL/x86/cpu.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef X86_CPU_H
#define X86_CPU_H

#include<stdint.h>

#define CPU_STATE 	x86_cpu_state
#define REG_FUNCRET 	eax
#define REG_IP 		eip
#define REG_DATA0 	edx
#define REG_DATA1 	ebx
#define REG_DATA2 	ecx
#define REG_STACKPTR 	esp
#define REG_FLAGS 	eflags
#define INFO_INTR 	intr

typedef struct {
    // manually secured registers
    uint32_t   eax;
    uint32_t   ebx;
    uint32_t   ecx;
    uint32_t   edx;
    uint32_t   esi;
    uint32_t   edi;
    uint32_t   ebp;

    uint32_t   intr;
    uint32_t   error;

    // secured by cpu
    uint32_t   eip;
    uint32_t   cs;
    uint32_t   eflags;
    uint32_t   esp;
    uint32_t   ss;
} x86_cpu_state;

#define NEW_STATE     CPU_STATE new_state = {\
        .eax = 0,\
        .ebx = 0,\
        .ecx = 0,\
        .edx = 0,\
        .esi = 0,\
        .edi = 0,\
        .ebp = 0,\
        .esp = 0,\
        .eip = 0,\
        .cs  = 0,\
        .ss  = 0,\
        .eflags = 0,\
    };
    
#define SET_IRQ(FLAGS) FLAGS=0x202
         /* Ring-0-Segmentregister nicht mehr benutzt*/
        //.cs  = 0x08,
	/* Ring-3-Segmentregister */   
	/* IRQs einschalten (IF = 1) */
#endif