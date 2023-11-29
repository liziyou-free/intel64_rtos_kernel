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

#include <stdint.h>
#include "./x64_isr_handler.h"
#include "../x64_cpu_drivers/x64_apic.h"


static x64_irq_hanlder_t x64_irq_handler[256];


int8_t x64_irq_handler_register(uint8_t vector, void(*p_fun)(void*), void* param)
{
  uint32_t elements;

  elements = sizeof(x64_irq_handler) / sizeof(x64_irq_handler[0]);

  if (p_fun == NULL || vector >= elements) {
      return -1;
  }
  x64_irq_handler[vector].param = param;
  x64_irq_handler[vector].pfn_handler = p_fun;
  return 0;
}


int8_t x64_irq_handler_destroy(uint8_t vector)
{
    uint32_t elements;

    elements = sizeof(x64_irq_handler) / sizeof(x64_irq_handler[0]);

    if (vector >= elements) {
        return -1;
    }
    x64_irq_handler[vector].param = NULL;
    x64_irq_handler[vector].pfn_handler = NULL;
    return 0;
}




void x64_common_isr (uint64_t inum)
{
    uint32_t elements;

    elements = sizeof(x64_irq_handler) / sizeof(x64_irq_handler[0]);

    if (inum >= elements || x64_irq_handler[inum].pfn_handler == NULL) {
        for (;;);
    }

    (x64_irq_handler[inum].pfn_handler)(x64_irq_handler[inum].param);

    if (inum >=32 ) {
        apic_eoi_hook();
    }

    return;
}


