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


#ifndef COMPILER_ASM_FILE
#define __BIGNUM__(x)    x##ULL
#else
#define __BIGNUM__(x)    x
#endif

#define X64_FLAT_SEGMENT_BASE       0x00000
#define X64_FLAT_SEGMENT_LIMIT      0xFFFFF


#define X64_DEFAULT_CODE32_INDEX    0x01
#define X64_DEFAULT_CODE64_INDEX    0x02
#define X64_DEFAULT_DATA_INDEX      0x03
#define X64_DEFUALT_GATE_INDEX      0x04


/**
 *\brief x86_dt_type_t
 *\note  distinguishes between various kinds of descriptors.
 */
/* system, interrupt and trap gate segment descriptor type */
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
//64bit code segment (extra enable 'L' bit)
#define APP_CODE64_X_SEGMENT          0x2018
#define APP_CODE64_XR_SEGMENT         0x201a
#define APP_CODE64_XC_SEGMENT         0x201c
#define APP_CODE64_XRC_SEGMENT        0x201e


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
 * \note  for protected mode.
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
                           (1 << 54)                        | \
                           (unit << 55)                     | \
                           ((addr >> 24) << 56) )

/**
 * \brief Generates a segment descriptor by passing in arguments.
 *
 * \note  All parameters type must be uint64_t.
 *
 * \param addr[in]  segment base address (linear address space)
 * \param len[in]   size of the segment (In units of one byte, MAX:1MB)
 * \param type[in]  kinds of descriptors: @ x86_dt_type_t
 * \param level[in] descriptor privilege level: @ dt_privilege_t
 *
 * \retval segment-descriptor
 */
#define X64_CREATE_GDT_ITEM_UNIT_BYTE(addr, seg_len, type, dpl)  \
                         __CREATE_GDT_ITEM__(__BIGNUM__(addr),     \
                                             __BIGNUM__(seg_len),  \
                                             __BIGNUM__(type),     \
                                             __BIGNUM__(dpl),    \
                                             __BIGNUM__(0))


/**
 * \brief Generates a segment descriptor by passing in arguments.
 *
 * \note  All parameters type must be uint64_t.
 *
 * \param addr[in]  segment base address (linear address space)
 * \param len[in]   size of the segment (In units of 4 Kilobytes, MAX:4GB)
 * \param type[in]  kinds of descriptors: @ x86_dt_type_t
 * \param dpl[in]   descriptor privilege level: @ dt_privilege_t
 *
 * \retval segment descriptor
 */
#define X64_CREATE_GDT_ITEM_UNIT_4KB(addr, seg_len, type, dpl)    \
                         __CREATE_GDT_ITEM__(__BIGNUM__(addr),     \
                                             __BIGNUM__(seg_len),  \
                                             __BIGNUM__(type),     \
                                             __BIGNUM__(dpl),    \
                                             __BIGNUM__(1))


#ifndef COMPILER_ASM_FILE
#pragma pack(push)
#pragma pack(1)
/**
 * \brief IDT(64bit mode): interrupt gate and trap gate.
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
 * \brief IDTR register define(64bit mode).
 */
typedef struct {
    uint16_t    limit;
    uint64_t    idt_addr;
}x64_idtr_t;

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
uint8_t x64_create_gate_descriptor(x64_idt_int_trap_gate_t *p_obj,
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
uint8_t x64_add_descriptor_to_idt(x64_idt_int_trap_gate_t *p_obj,
                                  void* p_idt,
                                  uint16_t index);


/**
 * \brief Update idtr register.
 *
 * \param p_idt_addr[in] the base address of IDT
 * \param limit[in]      IDT total bytes - 1
 *
 * \retval none
 */
void x64_flush_idtr_register(void *p_idt_addr, uint16_t limit);


#endif /* COMPILER_ASM_FILE */
#endif /* X64_SEGMENT_DEF_H_ */





