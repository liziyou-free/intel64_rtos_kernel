/*
 * cpu_driver.h
 *
 *  Created on: 2023年9月1日
 *      Author: liziyou
 */

#ifndef X64_DRIVER_X64_DT_H_
#define X64_DRIVER_X64_DT_H_

#include <stdint.h>
#include "x64_segment_def.h"

#pragma pack(push)
#pragma pack(1)

/** \brief Gloabl descriptor table */
typedef union x86_gdt_struct{
    struct {
        /*
         * For all types of segments except expand-down data segments, the
         * value of the limit is one less than the size (expressed in bytes)
         * of the segment.
         */
        uint64_t    seg_limit_low16:16;  /* segment limit: 0~15 bit */
        uint64_t    seg_base_low16:16;   /* segment base: 0~15 bit */
        uint64_t    seg_base_middle:8;  /* segment base: 16~23 bit */
        uint64_t    type:5;             /* Reference:@ x86_dt_type_t */
        uint64_t    dpl:2;              /* Reference:@ x86_dt_privilege_t */
        uint64_t    p:1;                /* segment present */
        uint64_t    seg_limit_high4:4;  /* segment limit: 16~19bit */
        uint64_t    avl:1;              /* use by systems programmers */
        uint64_t    :1;       /* Reserved for x86_64, don’t worry about x86 */
        uint64_t    d_b_x:1;  /* different situations has different meaning */
        uint64_t    g:1;      /* 0:limit unit is byte   1:limit unit is 4KB */
        uint64_t    seg_base_high8:8;   /* segment base: 24~31 bit */
    } detail;
    uint64_t    u64;
}x86_gdt_struct_t;


/** \brief IDT: task gate */
typedef union x86_idt_task_gate{
    struct {
        uint64_t    :16;           /* unused */
        uint64_t    selector:16;   /* selector */
        uint64_t    :8;            /* unused */
        uint64_t    type:5;        /* Reference:@ x86_dt_type_t */
        uint64_t    dpl:2;         /* Reference:@ x86_dt_privilege_t */
        uint64_t    p:1;           /* */
        uint64_t    :16;           /* unused */
    } detail;
    uint64_t    u64;
}x86_idt_task_gate_t;


/** \brief IDT: interrupt gate and trap gate */
typedef union x86_idt_int_trap_gate{
    struct {
        uint64_t    offset_low16:16;   /* offset[0:15] */
        uint64_t    selector:16;       /* selector */
        uint64_t    :5;                /* unused */
        uint64_t    always0:3;         /* always 0 */
        uint64_t    type:5;            /* Reference: @ x86_dt_type_t */
        uint64_t    dpl:2;             /* Reference: @ x86_dt_privilege_t */
        uint64_t    p:1;               /*  */
        uint64_t    offset_high16:16;  /* offset[16:31] */
    } detail;
    uint64_t    u64;
}x86_idt_int_trap_gate_t;


typedef union x86_selector{
    struct {
        uint16_t    rpl:2;     /* Reference: @ x86_dt_privilege_t */
        uint16_t    ti:1;      /* Reference: @ table_indicator_type_t */
        uint16_t    index:13;
    } detail;
    uint16_t u16;
} x86_selector_t;

#pragma pack(pop)



uint64_t x86_generate_segment_descriptor(uint32_t addr_s,
                                         uint32_t segment_len,
										 uint8_t type,
										 uint8_t level);

uint64_t x86_generate_task_gate_descriptor(uint16_t selector,
		                                   uint8_t type,
		                                   uint8_t dpl);

/**
 * \brief  get interrupt or trap gate descriptor
 *
 * \offset[in]    isr_hander = segment-base + offset
 * \selector[in]  selector of interrupt or trap descriptors
 * \type[in]      descriptor privilege level:@ x86_dt_privilege_t
 * \dpl[in]       @x86_dt_type_t: interrupt_gate_386 or trap_gate_386,
 *
 * \retval  segment register value
 */
uint64_t x86_generate_int_trap_gate_descriptor(uint32_t offset,
                                               uint16_t selector,
                                               uint8_t type,
											   uint8_t dpl);

void x86_add_item_to_gdt(x86_gdt_struct_t gdt[], uint16_t index, uint64_t item);

void x86_add_item_to_idt(uint64_t idt[], uint16_t index, uint64_t item);

uint16_t x86_generate_selector(uint16_t index,
                               uint8_t ti,
                               x86_dt_privilege_t rpl);

void x86_gdtr_flush(uint32_t *p_gdt_addr, uint16_t gdt_length);

void x86_idtr_flush(uint32_t *p_idt_addr, uint16_t idt_length);

#endif /* X64_DRIVER_X64_DT_H_ */









