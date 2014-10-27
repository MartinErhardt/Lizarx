ENTRY(_start)
OUTPUT_FORMAT(elf32-i386)
SECTIONS
{
    . = 0x100000;
    loader_start = .;
    .text : {
        *(.multiboot)
        *(.text)
    } 
    .data ALIGN(4096) : {
        *(.data)
    }
    .rodata ALIGN(4096) : {
        *(.rodata)
    } 
    .bss ALIGN(4096) : {
        *(.bss)
    }
    . = ALIGN(4096);
    loader_end = .;
}
