/*  <src-path>/src/kernel/lib/libOS/lock.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef LOCK_H
#define LOCK_H

#include<stdint.h>

#define LOCK_FREE 0
#define LOCK_USED 1

typedef uint8_t lock_t;

void spinlock_release(lock_t * lock);
void spinlock_lock(lock_t * lock);

static inline void spinlock_ackquire(lock_t* lock)
{
	    asm volatile(
        	"spin_lock: cmpb $0,(%0);"
		"je get_lock;"
		"pause;"
		"jmp spin_lock;"
	        "get_lock: movb $1, %%cl;"
		"lock cmpxchgb %%cl, (%0);"
		"jne spin_lock;" : : "D" (lock) : "eax", "ecx");
		  /*  asm volatile("movb $1, %%cl;"
	        "lock_loop: xorb %%al, %%al;"
        	"lock cmpxchgb %%cl, (%0);"
        	"jnz lock_loop;" : : "D" (lock) : "eax", "ecx");*/
}
#endif

