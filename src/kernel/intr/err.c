#include<intr/err.h>
#include<mt/cpustate.h>
#include<dbg/console.h>
#include<drv/vga-txt_graphics/vram.h>

void handle_exception(cpu_state* cpu){
	kprintf("Error: %d", cpu->intr);
	redscreen(cpu);
	while(1) {
	   // Prozessor anhalten
	   asm volatile("cli; hlt");
	}
}
void redscreen(cpu_state* cpu){
    clrscr(VGA_BLACK, VGA_RED);
    kprintfcol_scr(VGA_RED,VGA_BLACK,"A CRITICAL ERROR HAS OCURRED:\n"\
	    "Error: %d \n"\
	    "EAX: \t0x%p \t   EBX: \t0x%p \t   ECX: \t0x%p \t   EDX: \t0x%p \n"\
	    "ESI: \t0x%p \t   EDI: \t0x%p \n"\
	    "ESP: \t0x%p \t   EBP \t0x%p \n"\
	    "EIP: \t0x%p \n"\
	    "EFLAGS:\t0x%p \n"\
	    "CS:  \t0x%p  \t   SS: \t0x%p", 
	    cpu->intr,
	    cpu->eax, cpu->ebx, cpu->ecx, cpu->edx,
	    cpu->esi, cpu->edi,
	    cpu->esi, cpu->ebp,
	    cpu->eip,
	    cpu->eflags, 
	    cpu->cs, cpu->ss
 	  );
}
