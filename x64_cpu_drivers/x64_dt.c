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

#include "x64_dt.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "./x64_segment_def.h"
#include "../startup/x64_startup.h"


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
    /* The target code segment referenced by the interrupt gate must be a
     * 64-bit code segment (CS.L = 1, CS.D = 0). If the target is not a
     * 64-bit code segment, a general-protection exception (#GP) is gene-
     * rated with the IDT vector number reported as the error code.
     */
    p_obj->segment_selector = X64_GET_SEGMENT_SELECTOR_VALUE( \
                                          X64_GDT_INT_TRAP_GATE_INDEX, \
                                          dpl, \
                                          TABLE_INDICATOR_GDT);
    p_obj->fields.dpl = rpl;
    p_obj->fields.ist = ist;
    p_obj->fields.present = 1;
    p_obj->fields.type = type;
    return 0;
}


uint8_t x64_add_descriptor_to_idt (x64_idt_int_trap_gate_t *p_obj,
                                   x64_idt_int_trap_gate_t* p_idt,
                                   uint16_t index)
{
    uint32_t count = sizeof(x64_idt_int_trap_gate_t);
    x64_idt_int_trap_gate_t *p_des = NULL;

    if (p_obj == NULL) {
        return -1;
    }
    p_des = p_idt + index;
    memcpy((void*)p_des, (void*)p_obj, count);
    return 0;
}


uint8_t x64_flush_idtr_register (void *p_idt_addr, uint16_t limit)
{
    x64_idtr_t idtr = {0, 0};

    if (p_idt_addr == NULL) {
        return -1;
    }
    idtr.limit = limit;
    idtr.idt_addr = (uint64_t)p_idt_addr;
    __asm__ __volatile__("lidt  %0"::"m"(idtr):"memory");
    return 0;
}


uint8_t x64_flush_tr_register(x64_tr_t tr)
{
    if (tr == 0) {
        return -1;
    }
    __asm__ __volatile__("ltr  %0"::"R"(tr));
    return 0;
}





