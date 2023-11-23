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
#include <stdint.h>


static bool cpu_apic_check (void)
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


static void cpu_apic_enable (void)
{
    uint32_t  msr_addr;
    uint32_t  enable_bit;

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


uint64_t apic_get_base_addr (void)
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


void intel_apic_init(void)
{
  uint64_t apic_addr;

    if (cpu_apic_check() == false) {
        for(;;);
    }
    cpu_apic_enable();
    apic_addr = apic_get_base_addr();
}


static __inline__
void apic_eoi_hook(void)
{
    return;
}






