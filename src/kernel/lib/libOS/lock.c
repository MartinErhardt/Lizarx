/*   <src-path>/src/kernel/lib/libOS/lock.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#include<asm_inline.h>
#include<stdint.h>
#include"lock.h"
#include<smp_capabilities.h>

void spinlock_ackquire(uint8_t* lock)
{
    asm volatile(
        "spin_lock: cmpb $0,(%0);"
	"je get_lock;"
	"pause;"
	"jmp spin_lock;"
        "get_lock: movb $1, %%cl;"
		"lock cmpxchgb %%cl, (%0);"
		"jne spin_lock;" : : "D" (lock) : "eax", "ecx");
}
void spinlock_release(uint8_t * lock)
{
	*lock = LOCK_FREE;
}
void spinlock_lock(uint8_t * lock)
{
	*lock = LOCK_USED;
}