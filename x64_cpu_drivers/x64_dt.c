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
#include "../startup/multiboot.h"


uint64_t x86_generate_segment_descriptor(uint32_t addr_s,
                                         uint32_t segment_len,
                                         x86_dt_type_t type,
                                         x86_dt_privilege_t level)
{
    uint8_t      unit;
    x86_gdt_struct_t  code_segment;

    if (segment_len > 0x100000) {
        segment_len /= 4096;
        segment_len += 1;
        unit = 1;
    } else {
        unit = 0;
    }
    code_segment.detail.seg_base_low16 = (addr_s & 0x0000ffffu);
    code_segment.detail.seg_limit_low16 = (segment_len & 0x0000ffffu);
    code_segment.detail.type = type;
    code_segment.detail.d_b_x = 1;
    code_segment.detail.avl = 0;
    code_segment.detail.dpl = level;
    code_segment.detail.g = unit;
    code_segment.detail.p = 1;
    code_segment.detail.seg_base_middle = ((addr_s >> 16) & 0x00ff);
    code_segment.detail.seg_base_high8 = (addr_s >> 24);
    code_segment.detail.seg_limit_high4 = ((segment_len >> 16) & 0x00ff);

    return  code_segment.u64;
}


uint64_t x86_generate_task_gate_descriptor(uint16_t selector,
                                           x86_dt_type_t type,
                                           x86_dt_privilege_t dpl)
{
    x86_idt_task_gate_t task_gate_descriptor;
    task_gate_descriptor.detail.selector = selector;
    task_gate_descriptor.detail.type = type;
    task_gate_descriptor.detail.dpl = dpl;
    task_gate_descriptor.detail.p = 1;
    return task_gate_descriptor.u64;
}


uint64_t x86_generate_int_trap_gate_descriptor(uint32_t offset,
                                               uint16_t selector,
                                               x86_dt_type_t type,
                                               x86_dt_privilege_t dpl)
{
    x86_idt_int_trap_gate_t int_trap_gate_descriptor;

    int_trap_gate_descriptor.detail.offset_high16 = (offset >> 16);
    int_trap_gate_descriptor.detail.offset_low16 = (offset & 0x0000FFFFu);
    int_trap_gate_descriptor.detail.always0 = 0x00;
    int_trap_gate_descriptor.detail.selector = selector;
    int_trap_gate_descriptor.detail.type = type;
    int_trap_gate_descriptor.detail.dpl = dpl;
    int_trap_gate_descriptor.detail.p = 1;
    return int_trap_gate_descriptor.u64;
}



__inline__ void x86_add_item_to_gdt(x86_gdt_struct_t gdt[], uint16_t index, uint64_t item){
    gdt[index].u64 = item;
    return;
}

__inline__ void x86_add_item_to_idt(uint64_t idt[], uint16_t index, uint64_t item){
    idt[index] = item;
    return;
}


uint16_t x86_generate_selector(uint16_t index,
                               uint8_t ti,
                               x86_dt_privilege_t rpl)
{
    x86_selector_t selector;
    selector.detail.rpl = rpl;
    selector.detail.ti = ti;
    selector.detail.index = index;
    return selector.u16;
}



extern uint64_t _g_gdt[1];

void x86_gdtr_flush(uint32_t *p_gdt_addr, uint16_t gdt_length)
{
    uint64_t  gdtr = 0;
    gdtr = (((uint64_t)p_gdt_addr) << 16) | gdt_length;
    _g_gdt[0] = gdtr;
    __asm__ __volatile__ ("lgdt  _g_gdt");
}


extern uint64_t _g_idt[1];

void x86_idtr_flush(uint32_t *p_idt_addr, uint16_t idt_length)
{
    uint64_t  idtr = 0;
    idtr = (((uint64_t)p_idt_addr) << 16) | idt_length;
    _g_idt[0] = idtr;
    __asm__ __volatile__ ("lidt  _g_idt");
}






