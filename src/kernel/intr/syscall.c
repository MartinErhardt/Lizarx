#include<mt/cpustate.h>
//#include<dbg/console.h>
#include <drv/vga-txt_graphics/vram.h>
#include<intr/syscall.h>

void handle_syscall(cpu_state* cpu){
        //kprintf("syscall: EAX: %p EBX: %p EDX %p",cpu->eax,cpu->ebx,cpu->edx);
	char* copybuf_ptr;
	uint8_t font;
	switch(cpu->eax){
	    case(SYS_DRAW):
	        
		copybuf_ptr =(void*)cpu->ebx;
		font = (uint8_t)cpu->edx;
		kprintfcol_scr((font>>4),font,copybuf_ptr);
		
		break;
	    case(SYS_INFO): break;
	    case(SYS_GETTID):break;
	    case(SYS_KILLTID):break;
	    case(SYS_FORKTID):break;
	    case(SYS_GETPID):break;
	    case(SYS_KILLPID):break;
	    case(SYS_FORKPID):break;
	    default:break;
	}
}
