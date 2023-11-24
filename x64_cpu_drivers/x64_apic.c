/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* ALL rights reserved.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*
* The License of this software follows LGPL v2.1, See the LICENSE for more details:
* https://opensource.org/licenses/LGPL-2.1
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#include "../x64_cpu_drivers/x64_apic.h"
#include "../startup/x64_arch_def.h"
#include <stdbool.h>
#include <cpuid.h>
#include "stdint.h"


#define LAPIC_TIMER_INUM                    245
#define LAPIC_LINT0_INUM                    246
#define LAPIC_LINT1_INUM                    247
#define LAPIC_ERROR_INUM                    248
#define LAPIC_CMCI_INUM                     249
#define LAPIC_THERMSL_MONITOR_INUM          250
#define LAPIC_PERFORMANCE_COUNTER_INUM      251


/* transplant */
#define atomic_read32(reg)  (*((uint32_t*)reg))
#define atomic_write32(reg, value)  ((*((uint32_t*)reg)) = (value))


static bool cpu_local_apic_check (void)
{
    uint32_t eax=0;
    uint32_t ebx=0;
    uint32_t ecx=0;
    uint32_t edx=0;
    uint32_t operand = 1;
    __asm__ __volatile__ ( \
              "cpuid\n\t"
              : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
              : "0" (operand)
              );

    if (edx & (1 << 9 )) {
        return true;
    }
    return false;
}


void cpu_local_apic_open (void)
{
    uint32_t  msr_addr;
    uint32_t  enable_bit;

    if (cpu_local_apic_check() == false) {
        for(;;);
    }

    msr_addr = IA32_APIC_BASE_MSR;
    enable_bit = APIC_GLOBAL_ENABLE;

    __asm__ __volatile__ ( \
              "movl  %0,  %%ecx \n\t"
              "rdmsr            \n\t"
              "orl   %1,  %%eax \n\t"
              "wrmsr"
              :
              :"r"(msr_addr), "r"(enable_bit)
              :"rcx", "rax", "rdx"
              );
    return;
}


/**
 * \note Each core has a separate Local-APIC, so this function will always return
 * the Local-APIC address of the core on which the function is running.
 */
uint64_t get_local_apic_base_addr (void)
{
  uint32_t  msr_addr;
  uint32_t  msr_value;
  uint64_t  apic_addr;

  msr_addr = IA32_APIC_BASE_MSR;
  apic_addr = 0;
  __asm__ __volatile__ ( \
            "movl  %1,  %%ecx \n\t"
            "rdmsr            \n\t"
            "movl  %%eax, %0  \n\t"
            :"=r"(msr_value)
            :"r"(msr_addr)
            :"rcx", "rax", "rdx"
            );

  apic_addr = msr_value & 0xfffff000;
  return apic_addr;
}


void local_apic_disable (void)
{
  uint64_t loapic_addr;
  uint32_t value;

  loapic_addr = get_local_apic_base_addr();
  value = atomic_read32(loapic_addr + LAPIC_SPURIOUS_INTERRUPT_REG_OFFSET);
  value &= ~(1 << LAPIC_SOFTWARE_ENABLE_SHIFT);
  atomic_write32(loapic_addr + LAPIC_SPURIOUS_INTERRUPT_REG_OFFSET, value);
  return;
}


void local_apic_enable (void)
{
  uint64_t loapic_addr;
  uint32_t value;

  loapic_addr = get_local_apic_base_addr();
  value = atomic_read32(loapic_addr + LAPIC_SPURIOUS_INTERRUPT_REG_OFFSET);
  value |= (1 << LAPIC_SOFTWARE_ENABLE_SHIFT);
  atomic_write32(loapic_addr + LAPIC_SPURIOUS_INTERRUPT_REG_OFFSET, value);
  return;
}





void local_apic_set_int_vector (uint8_t inum, uint8_t vector)
{
  uint64_t loapic_addr;
  uint32_t reg_value;
  uint16_t reg_offset;

  loapic_addr = get_local_apic_base_addr();

    switch (inum) {
    case LAPIC_CMCI_INUM:
        reg_offset = LAPIC_CMCI_REG_OFFSET;
        break;
    case LAPIC_TIMER_INUM:
        reg_offset = LAPIC_TIMER_REG_OFFSET;
      break;
    case LAPIC_LINT0_INUM:
        reg_offset = LAPIC_LINT_0_REG_OFFSET;
        break;
    case LAPIC_LINT1_INUM:
        reg_offset = LAPIC_LINT_1_REG_OFFSET;
        break;
    case LAPIC_ERROR_INUM:
        reg_offset = LAPIC_LVT_ERR_REG_OFFSET;
        break;
    case LAPIC_THERMSL_MONITOR_INUM:
        reg_offset = LAPIC_THERMAL_SENSOR_REG_OFFSET;
        break;
    case LAPIC_PERFORMANCE_COUNTER_INUM:
        reg_offset = LAPIC_MONITOR_COUNTER_REG_OFFSET;
        break;
    default:
        break;
    }
    reg_value = atomic_read32(loapic_addr + reg_offset);
    reg_value &= ~0x000000ff;
    reg_value |= vector;
    atomic_write32(loapic_addr + LAPIC_TIMER_REG_OFFSET, reg_value);
    return;
}


void intel_local_apic_init(uint16_t core_id)
{
    uint64_t loapic_addr;
    uint32_t loapic_id;

    loapic_addr = get_local_apic_base_addr();

    loapic_id = atomic_read32(loapic_addr + LAPIC_ID_REG_OFFSET);

    /* Write core_id to LDR as logic apic-id */
    atomic_write32(loapic_addr + LAPIC_LOGICAL_DEST_REG_OFFSET, \
                   (1 << (core_id + LAPIC_LDR_ID_SHIFT)));

    /* Enable Local APIC */
    atomic_write32(loapic_addr + LAPIC_LOGICAL_DEST_REG_OFFSET, \
                   (1 << (core_id + LAPIC_LDR_ID_SHIFT)));


    return;
}


static __inline__
void apic_eoi_hook(void)
{

    return;
}






