/*
 * x86_apic.h
 *
 *  Created on: 2023年9月20日
 *      Author: liziyou
 */

#ifndef X86_DRIVER_X86_APIC_H_
#define X86_DRIVER_X86_APIC_H_

#include <stdint.h>

/*
 * The Pentium 4, Intel Xeon, and P6 family processors permit the starting addr-
 * ess of the APIC registers to be relocated from FEE00000H to another physical
 * address by modifying the value in the base address field of the APIC_BASE MSR
 */
#define  X86_APIC_BASE_ADDR    0xFEE00000UL



/* Define local API register. */
#define X86_APIC_ID_REGISTER               ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x20UL  ) ) )
#define X86_APIC_SPURIOUS_INT              ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0xF0UL  ) ) )
#define X86_APIC_LVT_TIMER                 ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x320UL ) ) )
#define X86_APIC_TIMER_INITIAL_COUNT       ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x380UL ) ) )
#define X86_APIC_TIMER_CURRENT_COUNT       ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x390UL ) ) )
#define X86_APIC_TASK_PRIORITY             ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x80UL  ) ) )
#define X86_APIC_LVT_ERROR                 ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x370UL ) ) )
#define X86_APIC_ERROR_STATUS              ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x280UL ) ) )
#define X86_APIC_LDR                       ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0xD0UL  ) ) )
#define X86_APIC_TMRDIV                    ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x3E0UL ) ) )
#define X86_APIC_LVT_PERF                  ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x340UL ) ) )
#define X86_APIC_LVT_LINT0                 ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x350UL ) ) )
#define X86_APIC_LVT_LINT1                 ( *( ( volatile uint32_t * ) ( X86_APIC_BASE_ADDR + 0x360UL ) ) )

#endif /* X86_DRIVER_X86_APIC_H_ */
