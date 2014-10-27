/*  <src-path>/src/usr/ld_a/ld.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef LD_H
#define LD_H
#include"elf.h"
#include"lib_stat/st_stdint.h"
#include"../hlib/cdefs.h"
/*
 * OS-specific format
 */
struct elf_lib
{
	struct elf_header * header;
	st_uintptr_t runtime_addr;
};
__BEGIN_DECLS
void link_daemon_mod(struct elf_lib* mod);
void link_lib_against(struct elf_lib* first_elf,struct elf_lib * second_elf);
void link_daemon_mod(struct elf_lib* mod);
st_uintptr_t get_func(struct elf_lib * mylib, const char* sym_name);
void * init_shared_lib(void* image);
__END_DECLS
struct dynamic
{
	struct elf_header * header;
	st_uintptr_t runtime_addr;
	
	struct elf_symbol * sym;
	
	struct	elf_rel * jmp_rel;
	st_int_t	jmp_rel_count;
	
	struct	elf_rel * dyn_rel;
	st_int_t	dyn_rel_count;
	
	struct	elf_rela * dyn_rela;
	st_int_t	dyn_rela_count;
	
	struct	elf_rela * jmp_rela;
	st_int_t	jmp_rela_count;
	
	st_int32_t * hash;
	char * str;
};

#endif
