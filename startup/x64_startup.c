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
#include "./x64_common.h"
#include "../x64_cpu_drivers/x64_apic.h"
#include "../x64_cpu_drivers/x64_segment_def.h"


#define ALIGNED(addr, align)    ((~(align - 1)) & addr)


typedef void (*pf_isr_handler_t)(void);


/**
 *\note The base addresses of the IDT should be aligned on an 8-byte boundary
 * to maximize performance of cache line fills.
 */
x64_idt_int_trap_gate_t g_x64_idt[256]__attribute__((aligned(8)));


/* stack for interrupt */
uint8_t g_interrupt_stack[1024*2]__attribute__((aligned(16)));


/* stack for exception */
uint8_t g_exception_stack[1024*2]__attribute__((aligned(16)));


extern x64_tss_ldt_dt_t g_gdt_tss_pos[1];
extern uint64_t g_exception_handler_table_addr;
extern uint16_t g_exception_handler_table_bytes;
extern uint64_t g_interrupt_handler_table_addr;
extern uint16_t g_interrupt_handler_table_bytes;


#define __EXCEPTION_TABLE_ADDR__   \
                    (&g_exception_handler_table_addr)

#define __EXCEPTION_TABLE_BYTES__  \
                    (*((uint16_t*)&g_exception_handler_table_bytes))

#define __INTERRUPT_TABLE_ADDR__   \
                    (&g_interrupt_handler_table_addr)

#define __INTERRUPT_TABLE_BYTES__  \
                    (*((uint16_t*)&g_interrupt_handler_table_bytes))



void x64_tss_init (void) {

    x64_tr_t tr;
    uint32_t tss_limit;
    uint64_t base_addr;
    uint64_t interrupt_stack_top;
    uint64_t exception_stack_top;
    x64_tss_ldt_dt_t *p_tss_descriptor;
    static x64_tss_t g_x64_tss_obj;

    print_func_name();

    /* Get stack top */
    interrupt_stack_top = ((uint64_t)&g_interrupt_stack[0]) +  \
                          sizeof(g_interrupt_stack) - 1;

    exception_stack_top = ((uint64_t)&g_exception_stack[0]) +  \
                          sizeof(g_exception_stack) - 1;

    /* Aligned to 16 bytes to maximize XMM register performance. */
    interrupt_stack_top = ALIGNED(interrupt_stack_top, 16);

    exception_stack_top = ALIGNED(exception_stack_top, 16);

    /* Init TSS, Stack memory is only allocated for interrupts and
     * exceptions. Other mechanisms are not currently used and will
     * not be considered for the time being.
     */
    memset((void *)&g_x64_tss_obj, 0, sizeof(x64_tss_t));
    g_x64_tss_obj.ist1 = interrupt_stack_top;
    g_x64_tss_obj.ist2 = exception_stack_top;

    /* Init TSS-descriptor in GDT */
    base_addr = (uint64_t)&g_x64_tss_obj;
    tss_limit = sizeof(g_x64_tss_obj) - 1;
    p_tss_descriptor = (x64_tss_ldt_dt_t *)g_gdt_tss_pos;
    memset((void *)p_tss_descriptor, 0, sizeof(x64_tss_ldt_dt_t));
    p_tss_descriptor->limit_low16 = tss_limit & 0x0000ffff;
    p_tss_descriptor->fields.limit_high4 = (tss_limit >> 16) & 0x000f;
    p_tss_descriptor->base_addr_0_15 = base_addr & 0x0000ffff;
    p_tss_descriptor->base_addr_16_23 = (base_addr >> 16) & 0x00ff;
    p_tss_descriptor->base_addr_24_31 = (base_addr >> 24) & 0x00ff;
    p_tss_descriptor->base_addr_32_63 = base_addr >> 32;
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
    uint16_t idt_elements;
    x64_idt_int_trap_gate_t  obj;
    uint16_t interrupt_table_elements;
    uint16_t exceptiont_table_elements;
    pf_isr_handler_t *pf_interrupt_table;
    pf_isr_handler_t *pf_exception_table;

    idt_elements = sizeof(g_x64_idt) / 16; /* a descriptor:16bytes */
    interrupt_table_elements = __INTERRUPT_TABLE_BYTES__ / 8;
    exceptiont_table_elements = __EXCEPTION_TABLE_BYTES__ / 8;
    pf_interrupt_table = ((pf_isr_handler_t *)__INTERRUPT_TABLE_ADDR__);
    pf_exception_table = ((pf_isr_handler_t *)__EXCEPTION_TABLE_ADDR__);

    print_func_name();

    for (int j = 0; j < idt_elements; j++) {
        if (j < 32) {
            /* exception */
            if (exceptiont_table_elements) {
                x64_create_gate_descriptor(&obj, \
                                           *pf_exception_table++, \
                                           X64_EXCEPTION_IST_INDEX, \
                                           TRAP_GATE_64BIT, \
                                           PRIVILEGE_LEVEL_0);
                --exceptiont_table_elements;
            }
            else {
                continue;
            }
        }
        else if (j >= 32) {
            /* interrupt */
            if (interrupt_table_elements) {
                x64_create_gate_descriptor(&obj, \
                                           *pf_interrupt_table++, \
                                           X64_INTERRUPT_IST_INDEX, \
                                           INTERRUPT_GATE_64BIT, \
                                           PRIVILEGE_LEVEL_0);
                --interrupt_table_elements;
            }
            else {
                break;
            }
        }
        /* Add to IDT */
        x64_add_descriptor_to_idt(&obj, &g_x64_idt[0], j);
    }

    /* Update to IDTR register */
    x64_flush_idtr_register(g_x64_idt, sizeof(g_x64_idt) - 1);

    return;
}


static void disable_intel_8259_intc (void)
{
  /**
   * \note interl 8259 does not have a real disable bit, we mask all
   * interrupt sources to achieve this purpose.
   */
    __asm__ __volatile__("movb  $0xff,  %al  \n\t"
                         "outb  %al,   $0x21 \n\t"
                         "outb  %al,   $0xA1 \n\t");
    return;
}



#define X64_USE_APIC_INTC    1

void x64_arch_init (void)
{
  print_func_name();

    /* Disable 8259, avoiding false triggering */
    disable_intel_8259_intc();

#if X64_USE_APIC_INTC
    cpu_local_apic_open();
    intel_local_apic_init(0);
    intel_ioapic_init();
#else

#endif
}


int divided_zero (void) {
  int a = 0, b = 0;
  a = 1000;
  a = a / b;
  return a;
}


