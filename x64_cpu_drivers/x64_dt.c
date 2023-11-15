/*
 * x86_dt.c
 *
 *   i386 descriptors table driver.
 *
 *      Created on: 2023年9月1日
 *      Author: liziyou
 */

#include "x64_dt.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../x64_cpu_drivers/x64_segment_def.h"


uint8_t x64_create_gate_descriptor(x64_idt_int_trap_gate_t *p_obj,
                                   void(*gp_isrh)(void),
                                   uint8_t ist,
                                   uint8_t type,
                                   uint8_t rpl)
{
    uint64_t  handle_addr = (uint64_t)gp_isrh;
    uint8_t   dpl = rpl;

    if (p_obj == NULL) {
        return -1;
    }
    memset((void *)p_obj, 0, sizeof(x64_idt_int_trap_gate_t));
    p_obj->offset_low16 = handle_addr & 0x0000ffff;
    p_obj->offset_middle16 = (handle_addr >> 16) & 0x0000ffff;
    p_obj->offset_hight32 = handle_addr >> 32;
    p_obj->segment_selector = X64_GET_SEGMENT_SELECTOR_VALUE( \
                                X64_DEFUALT_GATE_INDEX, \
                                dpl,
                                TABLE_INDICATOR_GDT);
    p_obj->fields.dpl = rpl;
    p_obj->fields.ist = ist;
    p_obj->fields.present = 1;
    p_obj->fields.type = type;
    return 0;
}


uint8_t x64_add_descriptor_to_idt(x64_idt_int_trap_gate_t *p_obj,
                                  void* p_idt,
                                  uint16_t index)
{
    uint32_t count = sizeof(x64_idt_int_trap_gate_t);
    x64_idt_int_trap_gate_t *p_des = NULL;

    if (p_obj == NULL) {
        return -1;
    }
    p_des = p_obj + index;
    memcpy((void*)p_des, (void*)p_obj, count);
    return 0;
}


void x64_flush_idtr_register(void *p_idt_addr, uint16_t limit)
{
    x64_idtr_t    idtr = {0, 0};
    idtr.limit = limit;
    idtr.idt_addr = (uint64_t)p_idt_addr;
    __asm__ __volatile__("lidt %0"::"m"(idtr):"memory");
}






