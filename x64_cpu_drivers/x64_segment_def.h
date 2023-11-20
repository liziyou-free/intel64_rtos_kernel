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

/**
 *\file  x64_segment_def.h
 *\note  If this file is included in the assembly, please use the following format:
 * #define COMPILER_ASM_FILE
 * #include "x64_segment_def.h"
 * #undef COMPILER_ASM_FILE
 */

#ifndef X64_SEGMENT_DEF_H_
#define X64_SEGMENT_DEF_H_


/**
 *\brief x86_dt_type_t
 *\note  distinguishes between various kinds of descriptors.
 */

/* IA-32 Mode: system segment and gate-descriptor type */
#define AVAILABLE_286_TSS           1
#define LOCAL_DESCRIPTOT_TABLE      2
#define BUSY_286_TSS                3
#define CALL_GATE                   4
#define TASK_GATE                   5
#define INTERRUPT_GATE_286          6
#define TRAP_GATE_286               7
#define AVAILABLE_386_TSS           9
#define BUSY_386_TSS                11
#define CALL_GATE_386               12
#define INTERRUPT_GATE_386          14
#define TRAP_GATE_386               15

/* IA-32e Mode: system segment and gate-descriptor type */
#define LOCAL_DESCRIPTOT_TABLE      2
#define AVAILABLE_64BIT_TSS         9
#define BUSY_64BIT_TSS              11
#define CALL_GATE_64BIT             12
#define INTERRUPT_GATE_64BIT        14
#define TRAP_GATE_64BIT             15


/* data segment descriptor type
 * -r: readable
 * -w: writable
 * -e: expand-down
 */
#define APP_DATA_RO_SEGMENT         0x11
#define APP_DATA_RW_SEGMENT         0x12
#define APP_DATA_RE_SEGMENT         0x14
#define APP_DATA_RWE_SEGMENT        0x16
/* code segment descriptor type
 * -x: executable
 * -r: readable
 * -c: conforming
 */
//32bit code segment
#define APP_CODE32_X_SEGMENT          0x18
#define APP_CODE32_XR_SEGMENT         0x1a
#define APP_CODE32_XC_SEGMENT         0x1c
#define APP_CODE32_XRC_SEGMENT        0x1e
//IA-32e 64bit-mode code segment (extra enable 'L' bit)
#define APP_CODE64_X_SEGMENT          0x2018
#define APP_CODE64_XR_SEGMENT         0x201a
#define APP_CODE64_XC_SEGMENT         0x201c
#define APP_CODE64_XRC_SEGMENT        0x201e
//IA-32e compatibility-mode code segment (extra enable 'L' and 'D' bit)
#define APP_COMPATIBLE_X_SEGMENT      0x6018
#define APP_COMPATIBLE_XR_SEGMENT     0x601a
#define APP_COMPATIBLE_XC_SEGMENT     0x601c
#define APP_COMPATIBLE_XRC_SEGMENT    0x601e


/**
 *\brief x86_dt_privilege_t
 *\note descriptor privilege level: used by the protection mechanism.
 */
#define PRIVILEGE_LEVEL_0          0  /* High */
#define PRIVILEGE_LEVEL_1          1
#define PRIVILEGE_LEVEL_2          2
#define PRIVILEGE_LEVEL_3          3  /* Low */


/**
 *\brief table_indicator_type_t
 *\note  specifies to which descriptor table the selector refers.
 */
#define TABLE_INDICATOR_GDT        0
#define TABLE_INDICATOR_LDT        1


/**
 * \brief Generates a selector by passing in arguments.
 *
 * \param index[in] the processor looks up the GDT by this index value
 * \param ti[in]    table Indicator: @ table_indicator_type_t
 * \param rpl[in]   requested privilege level:@ x86_dt_privilege_t
 *
 * \retval selector
 */
#define X64_GET_SEGMENT_SELECTOR_VALUE(index, ti, rpl)    \
                             ((index << 3) | (ti << 2) | rpl)


#define __CREATE_GDT_ITEM__(addr, len, type, level, unit)     \
                           ( ((len & 0x0000ffff) << 0)      | \
                           ((addr & 0x0000ffff) << 16)      | \
                           (((addr >> 16) & 0x00ff) << 32)  | \
                           (type << 40)                     | \
                           (level << 45)                    | \
                           (1 << 47)                        | \
                           (((len >> 16) & 0x0f) << 48)     | \
                           (unit << 55)                     | \
                           ((addr >> 24) << 56) )


/**
 * \brief Generates a segment descriptor by passing in arguments.
 *
 * \note  All parameters must be uint64_t!
 * \note  Not applicable to TSS, LDT and 64bit-code-segment descriptor.
 *
 * \param addr[in]  segment base address (linear address space)
 * \param len[in]   size of the segment (In units of one byte, MAX:1MB)
 * \param type[in]  kinds of descriptors: @ x86_dt_type_t
 * \param level[in] descriptor privilege level: @ dt_privilege_t
 *
 * \retval segment-descriptor
 */
#define X64_CREATE_GDT_ITEM_UNIT_BYTE(addr, seg_len, type, dpl)  \
                         __CREATE_GDT_ITEM__(addr, seg_len, type, dpl, 0)


/**
 * \brief Generates a segment descriptor by passing in arguments.
 *
 * \note  All parameters must be uint64_t!
 * \note  Not applicable to TSS, LDT and 64bit-code-segment descriptor.
 *
 * \param addr[in]  segment base address (linear address space)
 * \param len[in]   size of the segment (In units of 4 Kilobytes, MAX:4GB)
 * \param type[in]  kinds of descriptors: @ x86_dt_type_t
 * \param dpl[in]   descriptor privilege level: @ dt_privilege_t
 *
 * \retval segment descriptor
 */
#define X64_CREATE_GDT_ITEM_UNIT_4KB(addr, seg_len, type, dpl) \
                         __CREATE_GDT_ITEM__(addr, seg_len, type, dpl, 1)



#define X64_CREATE_GDT_ITEM_CODE64(type, dpl, d) \
                        (type << 40 | dpl << 45 | 1 << 47 | d << 54 | 1 << 55)


#ifndef COMPILER_ASM_FILE
#pragma pack(push)
#pragma pack(1)

/**
 * \brief IDT(ia-32e mode): interrupt gate and trap gate.
 */
typedef struct x64_idt_int_trap_gate{
    uint16_t    offset_low16;
    uint16_t    segment_selector;
    struct {
        uint16_t    ist:3;
        uint16_t    :5;
        uint16_t    type:4;
        uint16_t    :1;
        uint16_t    dpl:2;
        uint16_t    present:1;
    }fields;
    uint16_t    offset_middle16;
    uint32_t    offset_hight32;
    uint32_t    reserve_zero;
}x64_idt_int_trap_gate_t;


/**
 * \brief TSS (or LDT) descriptor in IA-32e mode.
 */
typedef struct x64_tss_ldt_dt {
    uint16_t    limit_low16;        /* segment limit: 0~15 bit */
    uint16_t    base_addr_0_15;     /* segment base: 0~15 bit */
    uint8_t     base_addr_16_23;    /* segment base: 16~23 bit */
    struct {
        uint16_t    type:4;         /* descriptor type */
        uint16_t    :1;
        uint16_t    dpl:2;          /* description privilege level */
        uint16_t    present:1;      /* segment present flag */
        uint16_t    limit_high4:4;    /* segment limit: 16~19bit */
        uint16_t    avl:1;          /* use by systems programmers */
        uint16_t    :2;
        uint16_t    g:1;            /* 0:limit unit is byte 1:limit unit is 4KB */
    }fields;
    uint8_t     base_addr_24_31;    /* segment base: 24~31 bit */
    uint32_t    base_addr_32_63;    /* segment base: 32~63 bit */
    uint32_t    reserved;
} x64_tss_ldt_dt_t;


/**
 * \brief TSS format(64bit mode)
 */
typedef struct {
    uint32_t reserved0;
    uint64_t rsp0;        /* stack for ring 0 */
    uint64_t rsp1;        /* stack for ring 1 */
    uint64_t rsp2;        /* stack for ring 2 */
    uint64_t reserved1;
    uint64_t ist1;        /* interrupt or exception stack 1 */
    uint64_t ist2;        /* interrupt or exception stack 2 */
    uint64_t ist3;        /* interrupt or exception stack 3 */
    uint64_t ist4;        /* interrupt or exception stack 4 */
    uint64_t ist5;        /* interrupt or exception stack 5 */
    uint64_t ist6;        /* interrupt or exception stack 6 */
    uint64_t ist7;        /* interrupt or exception stack 7 */
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t io_map_ba;   /* I/O map base address */
}x64_tss_t;


struct x64_dtr_str {
    uint16_t    limit;
    uint64_t    idt_addr;
};


/**
 * \brief GDTR register define(64bit mode).
 */
typedef struct x64_dtr_str x64_gdtr_t;


/**
 * \brief IDTR register define(64bit mode).
 */
typedef struct x64_dtr_str x64_idtr_t;


/**
 * \brief LDTR register define(Compatible with all modes).
 */
typedef uint16_t x64_ldtr_t;


/**
 * \brief TR register define(Compatible with all modes).
 */
typedef uint16_t x64_tr_t;


#pragma pack(pop)


/**
 * \brief Generate an interrupt or trap gate descriptor in ia-32e mode.
 *
 * \param p_obj[out]  interrupt or trap gate descriptor
 * \param gp_isrh[in] interrupt or exception handle function
 * \param ist[in]     index of interrupt stack table
 * \param type[in]    descriptor type: INTERRUPT_GATE_386 or TRAP_GATE_386
 * \param dpl[in]     descriptor privilege level: @ dt_privilege_t
 *
 * \retval 0:success
 * \retval 1:fail
 */
uint8_t x64_create_gate_descriptor (x64_idt_int_trap_gate_t *p_obj,
                                    void(*gp_isrh)(void),
                                    uint8_t ist,
                                    uint8_t type,
                                    uint8_t rpl);


/**
 * \brief Add an interrupt or trap gate descriptor to IDT in ia-32e mode.
 *
 * \param p_obj[in]  descriptor
 * \param p_idt[out] the base address of IDT
 * \param index[in]  indicates where to add to IDT
 *
 * \retval 0:success
 * \retval 1:fail
 */
uint8_t x64_add_descriptor_to_idt (x64_idt_int_trap_gate_t *p_obj,
                                   x64_idt_int_trap_gate_t* p_idt,
                                   uint16_t index);


/**
 * \brief Update idtr register.
 *
 * \param p_idt_addr[in] the base address of IDT
 * \param limit[in]      IDT total bytes - 1
 *
 * \retval 0:success
 * \retval 1:fail
 */
uint8_t x64_flush_idtr_register (void *p_idt_addr, uint16_t limit);


/**
 * \brief Update tr register.
 *
 * \param tr[in] segment selector
 *
 * \retval 0:success
 * \retval 1:fail
 */
uint8_t x64_flush_tr_register(x64_tr_t tr);




#endif /* COMPILER_ASM_FILE */
#endif /* X64_SEGMENT_DEF_H_ */





