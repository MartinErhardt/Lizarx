/*   <src-path>src/kernel/inc/mt/elf.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef LD_H
#define LD_H
#include"elf.h"
#include"lib_stat/st_stdint.h"
void link_against(struct elf_header* elf_main,struct elf_header* elf_lib, st_uintptr_t B);
void link_lib_against(struct elf_header* elf_main,struct elf_header* elf_lib);
void foo_stat();
void * init_shared_lib(void* image, st_size_t size);
#endif
