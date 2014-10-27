/*  <src-path>/src/usr/arch/x86_64/asm_inline.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef ASM_INLINE_H
#define ASM_INLINE_H

#include"syscall.h"

#define SYSCALL(N)	if(N!=SYS_EXIT && N!=SYS_SLEEP && N!= SYS_SEMOP)	asm volatile ("syscall"::"a" (N) ); \
			else							asm volatile ("int $0x30"::"a" (N) );
#define SET_ARG1(ARG)	asm volatile ("nop"::"d" (ARG));
#define SET_ARG2(ARG)	asm volatile ("nop"::"b" (ARG));
#define SET_ARG3(ARG)	asm volatile ("nop"::"c" (ARG)); \
			asm volatile("push %rcx");
#define GET_RET1(RET)	asm volatile ("nop" : "=d" (RET));
#define GET_RET2(RET)	asm volatile ("nop" : "=b" (RET));
#define GET_RET1n2(RET1,RET2)	asm volatile("nop" : "=d"(RET))\
				asm volatile("nop" : "=b" (RET2))
#define GET_RET3(RET)	asm volatile ("pop %rcx");asm volatile("nop":"=c" (RET));
#define _CALL_(ADDR)	asm volatile("nop"::"a"(ADDR));asm volatile ("add $128, %rsp; call *%rax");
#endif
