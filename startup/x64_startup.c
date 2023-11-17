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
#include <string.h>
#include "./x64_startup.h"
#include "../x64_cpu_drivers/x64_segment_def.h"


#define ALIGNED(addr, align)    ((~(align - 1)) & addr)


/*
 * A descriptor table is simply a memory array of 8byte entries that contain
 * descriptors. A descriptor table is variable in length and may contain up
 * to 8192 (2^13) descriptors.
 */
//x86_gdt_struct_t g_x86_gdt[64]__attribute__((aligned(16)));


/**
 *\note The base addresses of the IDT should be aligned on an 8-byte boundary
 * to maximize performance of cache line fills.
 */
x64_idt_int_trap_gate_t g_x64_idt[256]__attribute__((aligned(8)));


/* stack for interrupt */
uint8_t g_interrupt_stack[1024*2]__attribute__((aligned(8)));


extern x64_tss_ldt_dt_t g_gdt_tss_pos[1];


void irq_handle (void) {
  static int temp = 5;
  int a = 0;
  a = a + 5;
  temp = a + temp;
}


void x64_tss_init (void) {

    x64_tr_t tr;
    uint32_t tss_limit;
    uint64_t ba;
    uint64_t irq_stack_top;
    x64_tss_ldt_dt_t *p_tss_descriptor;
    static x64_tss_t g_x64_tss_obj;

    irq_stack_top = ((uint64_t)&g_interrupt_stack[0]) +  \
                    sizeof(g_interrupt_stack) - 1;

    irq_stack_top = ALIGNED(irq_stack_top, 8);

    /* Init TSS */
    memset((void *)&g_x64_tss_obj, 0, sizeof(x64_tss_t));
    g_x64_tss_obj.ist1 = irq_stack_top;
    g_x64_tss_obj.ist2 = irq_stack_top;
    g_x64_tss_obj.ist3 = irq_stack_top;
    g_x64_tss_obj.ist4 = irq_stack_top;
    g_x64_tss_obj.ist5 = irq_stack_top;
    g_x64_tss_obj.ist6 = irq_stack_top;
    g_x64_tss_obj.ist7 = irq_stack_top;
    g_x64_tss_obj.rsp0 = irq_stack_top;
    g_x64_tss_obj.rsp1 = irq_stack_top;
    g_x64_tss_obj.rsp2 = irq_stack_top;

    /* Init TSS descriptor in GDT */
    ba = (uint64_t)&g_x64_tss_obj;
    tss_limit = sizeof(g_x64_tss_obj) - 1;
    p_tss_descriptor = (x64_tss_ldt_dt_t *)g_gdt_tss_pos;
    memset((void *)p_tss_descriptor, 0, sizeof(x64_tss_ldt_dt_t));
    p_tss_descriptor->limit_low16 = tss_limit & 0x0000ffff;
    p_tss_descriptor->fields.limit_high4 = (tss_limit >> 16) & 0x000f;
    p_tss_descriptor->base_addr_0_15 = ba & 0x0000ffff;
    p_tss_descriptor->base_addr_16_23 = (ba >> 16) & 0x00ff;
    p_tss_descriptor->base_addr_24_31 = (ba >> 24) & 0x00ff;
    p_tss_descriptor->base_addr_32_63 = ba >> 32;
    p_tss_descriptor->fields.avl = 0;
    p_tss_descriptor->fields.g = 0;
    p_tss_descriptor->fields.present = 1;
    p_tss_descriptor->fields.dpl = PRIVILEGE_LEVEL_0;
    p_tss_descriptor->fields.type = AVAILABLE_64BIT_TSS;

    tr = X64_GET_SEGMENT_SELECTOR_VALUE(X64_GDT_TSS_INDEX,   \
                                        TABLE_INDICATOR_GDT, \
                                        PRIVILEGE_LEVEL_0);
    /* Update to TR register */
    x64_flush_tr_register(tr);
    return;
}


void x64_idt_init (void)
{
  x64_idt_int_trap_gate_t  obj;
  uint16_t idt_elements = sizeof(g_x64_idt) / 16; /* a descriptor:16bytes */

  for (int j = 0; j < idt_elements; j++) {
      if (j < 32) {
          /* exception */ //X64_INTERRUPT_IST_INDEX
          x64_create_gate_descriptor(&obj, irq_handle, 0, \
                                     TRAP_GATE_64BIT, PRIVILEGE_LEVEL_0);
      } else {
          /* interrupt */
          x64_create_gate_descriptor(&obj, irq_handle, 0, \
                                     INTERRUPT_GATE_64BIT, PRIVILEGE_LEVEL_0);
      }
      x64_add_descriptor_to_idt(&obj, &g_x64_idt[0], j);
  }

  /* Update to IDTR register */
  x64_flush_idtr_register(g_x64_idt, sizeof(g_x64_idt) - 1);

  return;
}


int divided_zero (void) {
  int a = 0, b = 0;
  a = 1000;
  a = a / b;
  return a;
}

