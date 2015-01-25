/*   <src-path>/src/kernel/HAL/x86_common/local_apic.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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

#include<local_apic.h>
#include<dbg/console.h>
#include<stdint.h>
#include <mm/vmm.h>
#include <boot/init.h>
#include <macros.h>
#include <hw_structs.h>
#include <asm_inline.h>
#include <libOS/mmio.h>
#include <libOS/lock.h>
#include <cpu.h>
#include <mm/pmm.h>

#define LAPIC_TIMER_PERIODIC                            0x00020000
#define LAPIC_MASKED                                    0x00010000

#define INITIAL_COUNT_VALUE (78125*40)

uintptr_t local_apic_virt=0x0;
static void local_apic_init_timer();
uint32_t tmp;
//void break_point();
//void break_point(){kprintf("hi");};
void local_apic_init(uintptr_t local_apic_addr_phys)
{
	// read apic id

	uintptr_t local_apic_virt_infunc = vmm_find_freemem_glob(&startup_context, PAGE_SIZE, 0x0, KERNEL_SPACE);
	vmm_map_inallcon_glob(local_apic_addr_phys,local_apic_virt_infunc, FLG_IN_MEM  | FLG_WRITABLE);
	vmm_mark_used_inallcon_glob(local_apic_virt_infunc/PAGE_SIZE);
	local_apic_virt = local_apic_virt_infunc;
	if((rdmsr(0x1b)&0x800)==0)
		wrmsr(0x1b, 0x800);
	/*if((mmio_read32(local_apic_virt,LOCAL_APIC_SIV_REG)&0x100)!=0)
	{
		kprintf("reg 0x%x",mmio_read32(local_apic_virt,LOCAL_APIC_SIV_REG));
	}*/
	mmio_write32(local_apic_virt, LOCAL_APIC_SIV_REG, (mmio_read32(local_apic_virt,LOCAL_APIC_SIV_REG) & 0xfffffe00) | 31 | 0x100); // 256 = 1 0000 0000 enable local APIC with the eight bit
	mmio_write32(local_apic_virt, LOCAL_APIC_LE_REG,(mmio_read32(local_apic_virt,LOCAL_APIC_LE_REG) & 0xfffeef00) | 30);
	
	local_apic_init_timer();
	//
	apic_ready=1;
}
uint8_t get_real_apic_id()
{
	return (mmio_read32(local_apic_virt, LOCAL_APIC_ID_REG)>>24);
}
struct cpu_info * get_cur_cpu()
{
	if(local_apic_virt==0x0) return &bsp_info;
	uint8_t apic_id = (mmio_read32(local_apic_virt,LOCAL_APIC_ID_REG)>>24);
	struct cpu_info * ret = (struct cpu_info *) alist_get_by_entry8(&cpu_list, GET_OFF(&bsp_info, &(bsp_info.apic_id)), apic_id);
//	kprintf("ret: 0x%p id: 0x%x\n",ret, apic_id);
	//if(!ret){
	//	break_point();
	//}
	return ret;
}
void startup_APs()
{
	uintptr_t trampoline_stack_virt = TRAMPOLINE;
	uint8_t vector = TRAMPOLINE/PAGE_SIZE;

	memcpy((void*)trampoline_stack_virt, &trampoline_entry_func, TRAMPOLINE_ENTRY_FUNC_SIZE);
	memcpy((void*)TRAMPOLINE_GDT, &gdt, TRAMPOLINE_GDT_SIZE);
	memcpy((void*)TRAMPOLINE_GDTR, &gdtr,TRAMPOLINE_GDTR_SIZE);
#ifdef ARCH_X86_64
	memcpy((void*)TRAMPOLINE_GDT64, &gdt64, TRAMPOLINE_GDT64_SIZE);
	memcpy((void*)TRAMPOLINE_GDTR64, &gdtr64, TRAMPOLINE_GDTR64_SIZE);
	memcpy((void*)TRAMPOLINE_LMODE, &lmode, TRAMPOLINE_LMODE_SIZE);
	memcpy((void*)TRAMPOLINE_VBEINFO, &VbeInfoBlock, 0x20);
#endif
	memcpy((void*)TRAMPOLINE_PMODE, &pmode, TRAMPOLINE_PMODE_SIZE);
	
	all_APs_booted = LOCK_USED;
	*((lock_t *)TRAMPOLINE_LOCK) = LOCK_FREE;
	
	local_apic_ipi_all_excluding_self(IPI_DELIVERY_MODE_INIT,vector,1);
	
	local_apic_ipi_all_excluding_self(IPI_DELIVERY_MODE_STARTUP,vector,0); // try to start processor with APIC id;
	spinlock_ackquire(&all_APs_booted);
	
}
void local_apic_eoi()
{
// clean up EOI REG = EOI
	mmio_write32(local_apic_virt, LOCAL_APIC_EOI_REG, 0x000000);
	mmio_write32(local_apic_virt, LOCAL_APIC_EOI_REG+4, 0);
}
void local_apic_init_AP()
{
	
	if((rdmsr(0x1b)&0x800)==0)
		wrmsr(0x1b, 0x800);
	
	mmio_write32(local_apic_virt, LOCAL_APIC_SIV_REG, (mmio_read32(local_apic_virt,LOCAL_APIC_SIV_REG) & 0xfffffe00) | 31 | 0x100); // 256 = 1 0000 0000 enable local APIC with the eight bit
	mmio_write32(local_apic_virt, LOCAL_APIC_LE_REG,(mmio_read32(local_apic_virt,LOCAL_APIC_LE_REG) & 0xfffeef00) | 30);
	local_apic_init_timer();
}
static void local_apic_init_timer()
{
	/*uint8_t in	= 0;
	uint32_t cpubusfreq;
	
	mmio_write32(local_apic_virt, LOCAL_APIC_DF_REG, 0xffffffff);
	mmio_write32(local_apic_virt, LOCAL_APIC_LD_REG, (mmio_read32(local_apic_virt,LOCAL_APIC_IC_REG)&0x00ffffff)|1);
	mmio_write32(local_apic_virt, LOCAL_APIC_LT_REG,APIC_DISABLE);
	
	mmio_write32(local_apic_virt, LOCAL_APIC_LINT0_REG, APIC_DISABLE);
	mmio_write32(local_apic_virt, LOCAL_APIC_LINT1_REG, APIC_DISABLE);
	mmio_write32(local_apic_virt, LOCAL_APIC_TP_REG, 0);
	
	mmio_write32(local_apic_virt, LOCAL_APIC_LPMCR_REG, APIC_NMI);
	
	*/mmio_write32(local_apic_virt, LOCAL_APIC_LT_REG, LAPIC_TIMER_PERIODIC | 28);
	mmio_write32(local_apic_virt, LOCAL_APIC_ICNT_REG, INITIAL_COUNT_VALUE/100);
	mmio_write32(local_apic_virt, LOCAL_APIC_DC_REG, 0xa);
	/*
	if((get_cur_cpu()==&bsp_info))
	{	
		//map APIC timer to an interrupt, and by that enable it in one-shot mode
		mmio_write32(local_apic_virt, LOCAL_APIC_LT_REG, 28);
		//set up divide value to 16
		mmio_write32(local_apic_virt, LOCAL_APIC_DC_REG, 0x03);
		
		INB(0x61, in);
		OUTB(0x61,((in&0xFD)|1));
		OUTB(0x43,0xb0)       // Switch to Mode 0, if you get problems!!!
		//1193180/100 Hz = 11931 = 2e9bh
		OUTB(0x42,0x9B)		//LSB
		INB(0x60, in)	//short delay
		OUTB(0x42,0x2E)		//MSB
		INB(0x61,in)
		tmp=(uint32_t)in&0xFE;
		OUTB(0x61,(uint8_t)tmp)		//gate low
		OUTB(0x61,(((uint8_t)tmp)|1))		//gate high
		//reset APIC timer (set counter to -1)
		mmio_write32(local_apic_virt, LOCAL_APIC_ICNT_REG,0xFFFFFFFF);
		//now wait until PIT counter reaches zero
	        asm volatile(
		
        	"calib_timer: in $0x61, %al;"
			"andb $0x20, %al;"
			"jnz calib_timer_loop;"
			"pause;"
			"jmp calib_timer;"
			"calib_timer_loop: movb $0x20, %cl;"
				"cmpb %cl, %al;"
				"jne calib_timer;");
		
		//do
		//{
		//	INB(0x61, in);
		//}
		while(!(in&0x20));
		//stop APIC timer
		mmio_write32(local_apic_virt, LOCAL_APIC_LT_REG, APIC_DISABLE);
		
		//now do the math...
		cpubusfreq=(0xFFFFFFFF-mmio_read32(local_apic_virt,LOCAL_APIC_CC_REG)+1)*16*100;
		
		tmp = cpubusfreq/16;
	}	
	//sanity check, now tmp holds appropriate number of ticks, use it as APIC timer counter initializer
	mmio_write32(local_apic_virt, LOCAL_APIC_ICNT_REG, (tmp<16?16:tmp));
	//finally re-enable timer in periodic mode
	mmio_write32(local_apic_virt, LOCAL_APIC_LT_REG, 28|0x20000);
	
	mmio_write32(local_apic_virt, LOCAL_APIC_DC_REG, 0x03);*/
}
void local_apic_ipi(uint8_t destinationId, uint8_t deliveryMode, uint8_t vector, uint8_t trigger_mode)
{
	while ((mmio_read32(local_apic_virt, LOCAL_APIC_IC_REG) & 0x1000) != 0);
	mmio_write32(local_apic_virt, (LOCAL_APIC_IC_REG_HI), destinationId << 24);
	mmio_write32(local_apic_virt, LOCAL_APIC_IC_REG,0x4000 | (trigger_mode<<15) | (deliveryMode<<8) | vector);
	
}
void local_apic_ipi_all_excluding_self(uint8_t deliveryMode,uint8_t vector, uint8_t trigger_mode)
{
	while ((mmio_read32(local_apic_virt, LOCAL_APIC_IC_REG) & 0x1000) != 0);
	// 0xc0000 = 011b << 18(Destination shorthand = all exclude self)
	// 0x4000 = 1 << 14 (Level enable if deliverymode != deprecated INIT Level De Assert)
	mmio_write32(local_apic_virt, LOCAL_APIC_IC_REG,0x4000 | (trigger_mode<<15) | (deliveryMode<<8) | vector | 0xc0000);
}
