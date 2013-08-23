/*   <src-path>/src/kernel/intr/local_apic.c is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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

#include<intr/local_apic.h>
#include<dbg/console.h>
#include<stdint.h>
#include <mm/vmm.h>
#include <hal.h>

#define TRAMPOLINE_SIZE 0x4

static uintptr_t local_apic_virt;

static uint32_t read_local_apic(uint32_t reg);
static void write_local_apic(uint32_t reg, uint32_t value);
static void local_apic_ipi(uint8_t destinationId, uint8_t deliveryMode, uint8_t vector);
static void local_apic_ipi_all_excluding_self(uint8_t deliveryMode,uint8_t vector);

void local_apic_init(uintptr_t local_apic_addr_phys)
{
	// read apic id
	local_apic_virt = vmm_find_freemem(&startup_context, 0x1000, 0x0, KERNEL_SPACE);
	vmm_map_inallcon(local_apic_addr_phys,local_apic_virt, FLG_IN_MEM  | FLG_WRITABLE);
	vmm_mark_used_inallcon(local_apic_virt/PAGE_SIZE);
	if((rdmsr(0x1b)&0x800)==0)
		wrmsr(0x1b, 0x800);
	write_local_apic(LOCAL_APIC_LE_REG,(read_local_apic(LOCAL_APIC_LE_REG) & 0xfffeef00) | 30);
	write_local_apic(LOCAL_APIC_SIV_REG, (read_local_apic(LOCAL_APIC_LE_REG) & 0xfffffe00) | 31 | 256); // 256 = 1 0000 0000 enable local APIC with the eight bit
}
void startup_APs()
{
	void * trampoline_virt = kvmm_malloc(PAGE_SIZE);
	memcpy(trampoline_virt, &trampoline_entry_func, TRAMPOLINE_SIZE);
	uint8_t vector = virt_to_phys(&startup_context, ((uintptr_t)trampoline_virt))/PAGE_SIZE;
	local_apic_ipi_all_excluding_self(IPI_DELIVERY_MODE_STARTUP,vector);
	local_apic_ipi(0x1,6,vector);
}

static void local_apic_ipi(uint8_t destinationId, uint8_t deliveryMode, uint8_t vector)
{
	while ((read_local_apic(LOCAL_APIC_IC_REG) & 0x1000) != 0);
	write_local_apic(LOCAL_APIC_IC_REG+4,destinationId << 24);
	write_local_apic(LOCAL_APIC_IC_REG,0x4000 | (deliveryMode<<8) | vector);
}
static void local_apic_ipi_all_excluding_self(uint8_t deliveryMode,uint8_t vector)
{
	while ((read_local_apic(LOCAL_APIC_IC_REG) & 0x1000) != 0);
	// 0x6000 = 011b << 18(Destination shorthand = all exclude self)
	// 0x4000 = 1 << 14 (Level enable if deliverymode != deprecated INIT Level De Assert)
	write_local_apic(LOCAL_APIC_IC_REG,0x4000 | 0xc0000 | (deliveryMode<<8) | vector);
}
static uint32_t read_local_apic(uint32_t reg)
{
	return *((volatile uint32_t * )local_apic_virt+reg);
}
static void write_local_apic(uint32_t reg, uint32_t value)
{
	*((volatile uint32_t * )local_apic_virt+reg)=value;
}
