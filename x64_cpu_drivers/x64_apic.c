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


#define APIC_TIMER_INUM                    245
#define APIC_LINT0_INUM                    246
#define APIC_LINT1_INUM                    247
#define APIC_ERROR_INUM                    248
#define APIC_CMCI_INUM                     249
#define APIC_THERMSL_MONITOR_INUM          250
#define APIC_PERFORMANCE_COUNTER_INUM      251


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
  value = atomic_read32(loapic_addr + APIC_SPURIOUS_INTERRUPT_REG_OFFSET);
  value &= ~(1 << APIC_SOFTWARE_ENABLE_SHIFT);
  atomic_write32(loapic_addr + APIC_SPURIOUS_INTERRUPT_REG_OFFSET, value);
  return;
}


void local_apic_enable (void)
{
  uint64_t loapic_addr;
  uint32_t value;

  loapic_addr = get_local_apic_base_addr();
  value = atomic_read32(loapic_addr + APIC_SPURIOUS_INTERRUPT_REG_OFFSET);
  value |= (1 << APIC_SOFTWARE_ENABLE_SHIFT);
  atomic_write32(loapic_addr + APIC_SPURIOUS_INTERRUPT_REG_OFFSET, value);
  return;
}


void intel_local_apic_init(uint16_t core_id)
{
    uint64_t loapic_addr;

    loapic_addr = get_local_apic_base_addr();

//    loapic_id = atomic_read32(loapic_addr + APIC_ID_REG_OFFSET);

    atomic_write32(loapic_addr + APIC_DEST_FORMAT_REG_OFFSET, \
                   APIC_DFR_FLAT_MODEL);

    /* Write core_id to LDR as logic apic-id */
    atomic_write32(loapic_addr + APIC_LOGICAL_DEST_REG_OFFSET, \
                   (1 << (core_id + APIC_LDR_ID_SHIFT)));

    /* Disable all LVT */
    atomic_write32(loapic_addr + APIC_LVT_CMCI_REG_OFFSET, \
                   1 << LVT_MASK_SHIFT);
    atomic_write32(loapic_addr + APIC_LVT_TIMER_REG_OFFSET, \
                   1 << LVT_MASK_SHIFT);
    atomic_write32(loapic_addr + APIC_LVT_THERMAL_SENSOR_REG_OFFSET, \
                   1 << LVT_MASK_SHIFT);
    atomic_write32(loapic_addr + APIC_LVT_MONITOR_COUNTER_REG_OFFSET, \
                   1 << LVT_MASK_SHIFT);
    atomic_write32(loapic_addr + APIC_LVT_LINT_0_REG_OFFSET, \
                   1 << LVT_MASK_SHIFT);
    atomic_write32(loapic_addr + APIC_LVT_LINT_1_REG_OFFSET, \
                   1 << LVT_MASK_SHIFT);
    atomic_write32(loapic_addr + APIC_LVT_ERR_REG_OFFSET, \
                   1 << LVT_MASK_SHIFT);

    /* Ack any outstanding interrupts. */
    atomic_write32(loapic_addr + APIC_EOI_REG_OFFSET, 0);

    /* Enable Local APIC */
    local_apic_enable();
    return;
}




static __inline__
void apic_eoi_hook(void)
{
  uint64_t loapic_addr;

  loapic_addr = get_local_apic_base_addr();
  /* Ack any outstanding interrupts. */
  atomic_write32(loapic_addr + APIC_EOI_REG_OFFSET, 0);
  return;
}



/*******************************************************************************
 * I/O  APIC
*******************************************************************************/

void ioapic_config_int_line (uint8_t intline, ioapic_intline_config_t *p_cfg)
{
    /* Redirection Entry register */
    uint32_t *p_rer = (uint32_t *)p_cfg;

    atomic_write32((IOAPIC_REG_TABLE + intline * 2), p_rer[0]);
    atomic_write32((IOAPIC_REG_TABLE + intline * 2 + 1), p_rer[1]);
    return;
}


void intel_ioapic_init (void)
{
    uint32_t vector_cnt;
    uint32_t ver_info;
    uint32_t max_irq_num;
    ioapic_intline_config_t int_cfg;

    ver_info = atomic_read32(IOAPIC_BASE_ADDR + IOAPIC_REG_VER);
    max_irq_num = (ver_info >> 16) & 0x000f;

    int_cfg.trigger_mode = TRIGGER_RISING_EDGE;
    int_cfg.mask = 1;  /* defualt mask interrupt */
    int_cfg.destination_mode = LOGICAL_DESTINATION;  /* use logic id */
    int_cfg.destination = 1;  /* BSP core defualt logic id */
    int_cfg.delivery_mode = DELIVERY_MODE_FIXED;    /* fixed mode */

    for (vector_cnt = 0; vector_cnt < max_irq_num; vector_cnt++) {
        int_cfg.vector = IRQ0 + vector_cnt;
        ioapic_config_int_line(vector_cnt, &int_cfg);
    }

    return;
}






//void local_apic_set_int_vector (uint8_t inum, uint8_t vector)
//{
//  uint64_t loapic_addr;
//  uint32_t reg_value;
//  uint16_t reg_offset;
//
//  loapic_addr = get_local_apic_base_addr();
//
//    switch (inum) {
//    case APIC_CMCI_INUM:
//        reg_offset = APIC_CMCI_REG_OFFSET;
//        break;
//    case APIC_TIMER_INUM:
//        reg_offset = APIC_TIMER_REG_OFFSET;
//      break;
//    case APIC_LINT0_INUM:
//        reg_offset = APIC_LINT_0_REG_OFFSET;
//        break;
//    case APIC_LINT1_INUM:
//        reg_offset = APIC_LINT_1_REG_OFFSET;
//        break;
//    case APIC_ERROR_INUM:
//        reg_offset = APIC_LVT_ERR_REG_OFFSET;
//        break;
//    case APIC_THERMSL_MONITOR_INUM:
//        reg_offset = APIC_THERMAL_SENSOR_REG_OFFSET;
//        break;
//    case APIC_PERFORMANCE_COUNTER_INUM:
//        reg_offset = APIC_MONITOR_COUNTER_REG_OFFSET;
//        break;
//    default:
//        break;
//    }
//    reg_value = atomic_read32(loapic_addr + reg_offset);
//    reg_value &= ~0x000000ff;
//    reg_value |= vector;
//    atomic_write32(loapic_addr + APIC_TIMER_REG_OFFSET, reg_value);
//    return;
//}

