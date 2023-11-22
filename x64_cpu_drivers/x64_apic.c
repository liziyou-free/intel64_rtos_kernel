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

#include "../startup/x64_arch_def.h"
#include <stdbool.h>
#include <cpuid.h>
#include <stdint.h>


bool cpu_apic_check (void)
{
    uint32_t eax=0;
    uint32_t ebx=0;
    uint32_t ecx=0;
    uint32_t edx=0;
    uint32_t operand = 1;
    __asm__ __volatile__ ( \
               "cpuid\n\t" \
               : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx) \
               : "0" (operand) \
               );

    if (edx & (1 << 9 )) {
        return true;
    }
    return false;
}


void cpu_apic_enable (void)
{
    uint32_t  msr_low;

    msr_low = 0;
    __asm__ __volatile__ ("mov $0x1b,  %%ecx \n\t"
                          "rdmsr             \n\t"
                          "mov %%eax,  %0    \n\t"
                          "or  $0x800, %0    \n\t"
                          "wrmsr"
                          :"=r"(msr_low)
                          ::"rcx", "rax", "rdx");
}


void apic_init(void)
{
  if (cpu_apic_check() == false) {
      for(;;);
  }

}


static __inline__ apic_eoi_hook(void)
{

}






