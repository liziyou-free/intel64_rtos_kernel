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

#ifndef X64_SEGMENT_DEF_H_
#define X64_SEGMENT_DEF_H_


#define X64_FLAT_SEGMENT_BASE       0x00000u
#define X64_FLAT_SEGMENT_LIMIT      0xFFFFFu


#define X64_DEFAULT_CODE32_INDEX    0x01u
#define X64_DEFAULT_CODE64_INDEX    0x02u
#define X64_DEFAULT_DATA_INDEX      0x03u


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
#define APP_CODE32_X_SEGMENT          0x18
#define APP_CODE32_XR_SEGMENT         0x1a
#define APP_CODE32_XC_SEGMENT         0x1c
#define APP_CODE32_XRC_SEGMENT        0x1e
#define APP_CODE64_X_SEGMENT          (0x18 | (1u << 52))
#define APP_CODE64_XR_SEGMENT         (0x1a | (1u << 52))
#define APP_CODE64_XC_SEGMENT         (0x1c | (1u << 52))
#define APP_CODE64_XRC_SEGMENT        (0x1e | (1u << 52))

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
#define GET_SEGMENT_REG_VALUE(index, ti, rpl)  ((index << 3) | (ti << 2) | rpl)


#define __CREATE_GDT_ITEM__(addr, len, type, level, unit)        \
                           ( ((len & 0x0000ffffull) << 0)      | \
                           ((addr & 0x0000ffffull) << 16)      | \
                           (((addr >> 16) & 0x00ffull) << 32)  | \
                           (type << 40)                        | \
                           (level << 45)                       | \
                           (1ull << 47)                        | \
                           (((len >> 16) & 0x0f) << 48)        | \
                           (1ull << 54)                        | \
                           (unit << 55)                        | \
                           ((addr >> 24) << 56) )

/**
 * \brief Generates a segment descriptor by passing in arguments.
 *
 * \param addr[in]  segment base address (linear address space)
 * \param len[in]   size of the segment (In units of one byte, MAX:1MB)
 * \param type[in]  kinds of descriptors: @ x86_dt_type_t
 * \param level[in] descriptor privilege level: @ dt_privilege_t
 *
 * \retval segment-descriptor
 */
#define CREATE_GDT_ITEM_UNIT_BYTE(addr, seg_len, type, level)    \
                         __CREATE_GDT_ITEM__(addr, seg_len, type, level, 0ull)


/**
 * \brief Generates a segment descriptor by passing in arguments.
 *
 * \param addr[in]  segment base address (linear address space)
 * \param len[in]   size of the segment (In units of 4 Kilobytes, MAX:4GB)
 * \param type[in]  kinds of descriptors: @ x86_dt_type_t
 * \param level[in] descriptor privilege level: @ dt_privilege_t
 *
 * \retval segment-descriptor
 */
#define CREATE_GDT_ITEM_UNIT_4KB(addr, seg_len, type, level)    \
                         __CREATE_GDT_ITEM__(addr, seg_len, type, level, 1)

#endif /* X64_SEGMENT_DEF_H_ */





