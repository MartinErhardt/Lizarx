/*   <src-path>/src/kernel/intr/local_apic.h is a source file of Lizarx an unixoid Operating System, which is licensed under GPLv2 look at <src-path>/COPYRIGHT.txt for more info
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
#ifndef LOCAL_APIC_H
#define LOCAL_APIC_H

#include<stdint.h>

#define LOCAL_APIC_ID_REG	0x20  // rw
#define LOCAL_APIC_VER_REG	0x30  // r
#define LOCAL_APIC_TP_REG	0x80  // Task Priority rw
#define LOCAL_APIC_AP_REG	0x90  // Arbitration Priority r
#define LOCAL_APIC_PP_REG	0xa0  // Process Priority r
#define LOCAL_APIC_EOI_REG	0xb0  // w
#define LOCAL_APIC_LD_REG	0xd0  // Logical Destination rw
#define LOCAL_APIC_DF_REG	0xe0  // Destination Format rw
#define LOCAL_APIC_SIV_REG	0xf0  // Spurious Interrupt Vector rw
#define LOCAL_APIC_IS_REG	0x100 // In Service r
#define LOCAL_APIC_TM_REG	0x180 // Trigger Mode r
#define LOCAL_APIC_IR_REG	0x200 // Interrupt Request rw
#define LOCAL_APIC_ES_REG	0x280 // Error Status r
#define LOCAL_APIC_IC_REG	0x300 // Interrupt Command rw
#define LOCAL_APIC_IC_REG_HI	0x310 // Interrupt Command rw
#define LOCAL_APIC_LT_REG	0x320 // LVT Timer rw
#define LOCAL_APIC_LTS_REG	0x330 // LVT Thermal Sensor rw
#define LOCAL_APIC_LPMCR_REG	0x340 // LVT Performance Monitoring Counters Register rw
#define LOCAL_APIC_LINT0_REG	0x350 // LVT LINT0 rw
#define LOCAL_APIC_LINT1_REG	0x360 // LVT LINT1 rw
#define LOCAL_APIC_LE_REG	0x370 // LVT Error rw
#define LOCAL_APIC_ICNT_REG	0x380 // Initial Count rw
#define LOCAL_APIC_CC_REG	0x390 // Current Count r
#define LOCAL_APIC_DC_REG	0x3e0 // Divide Configuration rw

#define IPI_DELIVERY_MODE_FIXED		0x0
#define IPI_DELIVERY_MODE_LOWPRIORITY	0x1
#define IPI_DELIVERY_MODE_NMI		0x4
#define IPI_DELIVERY_MODE_INIT		0x5
#define IPI_DELIVERY_MODE_STARTUP	0x6

#define IPI_DESTINATION_SHORTHAND_NONE			0x0
#define IPI_DESTINATION_SHORTHAND_SELF			0x1
#define IPI_DESTINATION_SHORTHAND_ALL			0x2
#define IPI_DESTINATION_SHORTHAND_ALL_EXCLUDE_SELF	0x3

extern void trampoline_entry_func(void);

void local_apic_init(uintptr_t local_apic_addr_phys);
void startup_APs();

#endif
