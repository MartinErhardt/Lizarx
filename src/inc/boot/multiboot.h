#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

typedef struct  {
    uint32_t    mbs_flags;
    uint32_t    mbs_mem_lower;
    uint32_t    mbs_mem_upper;
    uint32_t    mbs_bootdevice;
    uint32_t    mbs_cmdline;
    uint32_t    mbs_mods_count;
    void*       mbs_mods_addr;
    uint32_t    mbs_syms[4];
    uint32_t    mbs_mmap_length;
    void*       mbs_mmap_addr;
} __attribute__((packed)) multiboot_info;

struct multiboot_mmap {
    uint32_t    entry_size;
    uint64_t    base;
    uint64_t    length;
    uint32_t    type;
} __attribute__((packed));

#endif