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
#include <mm/gdt.h>
#include <boot/init.h>
#include <macros.h>
#include <asm_inline.h>
#include <libOS/mmio.h>

#define TRAMPOLINE_SIZE 0x100

static uintptr_t local_apic_virt;

//static void local_apic_ipi(uint8_t destinationId, uint8_t deliveryMode, uint8_t vector, uint8_t trigger_mode);
static void local_apic_ipi_all_excluding_self(uint8_t deliveryMode,uint8_t vector, uint8_t trigger_mode);

void local_apic_init(uintptr_t local_apic_addr_phys)
{
	// read apic id
	
	local_apic_virt = vmm_find_freemem(&startup_context, 0x1000, 0x0, KERNEL_SPACE);
	vmm_map_inallcon(local_apic_addr_phys,local_apic_virt, FLG_IN_MEM  | FLG_WRITABLE);
	vmm_mark_used_inallcon(local_apic_virt/PAGE_SIZE);
	
	if((rdmsr(0x1b)&0x800)==0)
		wrmsr(0x1b, 0x800);
	/*if((mmio_read32(local_apic_virt,LOCAL_APIC_SIV_REG)&0x100)!=0)
	{
		kprintf("reg 0x%x",mmio_read32(local_apic_virt,LOCAL_APIC_SIV_REG));
	}*/
	mmio_write32(local_apic_virt, LOCAL_APIC_SIV_REG, (mmio_read32(local_apic_virt,LOCAL_APIC_SIV_REG) & 0xfffffe00) | 0x31 | 0x100); // 256 = 1 0000 0000 enable local APIC with the eight bit
	mmio_write32(local_apic_virt, LOCAL_APIC_LE_REG,(mmio_read32(local_apic_virt,LOCAL_APIC_LE_REG) & 0xfffeef00) | 30);
	
	mmio_write32(local_apic_virt, LOCAL_APIC_LD_REG, (1 << 24));
	mmio_write32(local_apic_virt, LOCAL_APIC_LD_REG, (0xf << 28));
}
void startup_APs()
{
	uint32_t z =0;
	void * trampoline_virt = kvmm_malloc(PAGE_SIZE);
	memcpy(trampoline_virt, &trampoline_entry_func, TRAMPOLINE_SIZE);
	uint8_t vector = virt_to_phys(&startup_context, ((uintptr_t)trampoline_virt))/PAGE_SIZE;
	
	uintptr_t trampoline_stack_virt = 0x7000;
	
	memcpy(((void*)trampoline_stack_virt), &gdt, 0x18);
	memcpy(((void*)trampoline_stack_virt+0x20), &gdtr, 0x8);
#ifdef ARCH_X86_64
	memcpy(((void*)trampoline_stack_virt+0x28), &gdt64, 0x18);
	memcpy(((void*)trampoline_stack_virt+0x40), &gdtr64, 0x8);
	memcpy(((void*)trampoline_stack_virt+0x100), &lmode, 0x50);
#endif
	memcpy(((void*)trampoline_stack_virt+0x48), &pmode, 0xb0);
	
	local_apic_ipi_all_excluding_self(IPI_DELIVERY_MODE_INIT,vector,1);
	
	local_apic_ipi_all_excluding_self(IPI_DELIVERY_MODE_STARTUP,vector,0); // try to start processor with APIC id;
	for(z = 0; z < 0x90000; z++);
	//local_apic_ipi_all_excluding_self(IPI_DELIVERY_MODE_STARTUP,vector); // here I try to start all APs with a single IPI; not sure whether this is possible
}
/*
static void local_apic_ipi(uint8_t destinationId, uint8_t deliveryMode, uint8_t vector, uint8_t trigger_mode)
{
	while ((mmio_read32(local_apic_virt,LOCAL_APIC_IC_REG) & 0x1000) != 0);
	mmio_write32(local_apic_virt, (LOCAL_APIC_IC_REG_HI),destinationId << 24);
	mmio_write32(local_apic_virt, LOCAL_APIC_IC_REG,0x4000 | (trigger_mode<<15) | (deliveryMode<<8) | vector);
	
}*/
static void local_apic_ipi_all_excluding_self(uint8_t deliveryMode,uint8_t vector, uint8_t trigger_mode)
{
	while ((mmio_read32(local_apic_virt, LOCAL_APIC_IC_REG) & 0x1000) != 0);
	// 0xc0000 = 011b << 18(Destination shorthand = all exclude self)
	// 0x4000 = 1 << 14 (Level enable if deliverymode != deprecated INIT Level De Assert)
	mmio_write32(local_apic_virt, LOCAL_APIC_IC_REG,0x4000 | (trigger_mode<<15) | (deliveryMode<<8) | vector | 0xc0000);
}
