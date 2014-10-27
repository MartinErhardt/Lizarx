/*  <src-path>/src/kernel/inc/boot/multiboot.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv3 look at <src-path>/LICENSE for more info
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
#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

struct multiboot_info {
    uint32_t    mbs_flags;
    uint32_t    mbs_mem_lower;
    uint32_t    mbs_mem_upper;
    uint32_t    mbs_bootdevice;
    uint32_t    mbs_cmdline;
    uint32_t    mbs_mods_count;
    uint32_t       mbs_mods_addr;
    uint32_t    mbs_syms[4];
    uint32_t    mbs_mmap_length;
    uint32_t       mbs_mmap_addr;
} __attribute__((packed));

struct multiboot_mmap {
    uint32_t    entry_size;
    uint64_t    base;
    uint64_t    length;
    uint32_t    type;
} __attribute__((packed));

struct multiboot_module {
    uint32_t    mod_start;
    uint32_t    mod_end;
    uint32_t    cmdline;
    uint32_t    reserved;
} __attribute__((packed));

#endif
