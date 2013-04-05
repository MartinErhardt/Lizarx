/*   <src-path>/src/kernel/mt/inc/proc.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef PROC_H
#define PROC_H

#include<mm/vmm.h>
#include<stdint.h>
#include"user.h"
#include<time.h>
struct proc {
    uint32_t 	p_id;
    struct user*user;
//    char[10]	desc;
    clock_t clock;
    vmm_context*	context;
    struct proc*next;
};
struct proc* first_proc;
struct proc* cur_proc;

struct proc* create_proc();
struct proc* get_proc(uint32_t p_id);

#endif