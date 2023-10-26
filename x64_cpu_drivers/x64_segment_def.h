/*
 * x64_segment_type.h
 *
 *  Created on: 2023年10月26日
 *      Author: FreedomLi
 */

#ifndef X64_SEGMENT_DEF_H_
#define X64_SEGMENT_DEF_H_


/**
 *\brief x86_dt_type_t
 *\note  distinguishes between various kinds of descriptors.
 */
/* system, interrupt and trap gate segment descriptor type */
#define  AVAILABLE_286_TSS           1
#define  LOCAL_DESCRIPTOT_TABLE      2
#define  BUSY_286_TSS                3
#define  CALL_GATE                   4
#define  TASK_GATE                   5
#define  INTERRUPT_GATE_286          6
#define  TRAP_GATE_286               7
#define  AVAILABLE_386_TSS           9
#define  BUSY_386_TSS                11
#define  CALL_GATE_386               12
#define  INTERRUPT_GATE_386          14
#define  TRAP_GATE_386               15
/* data segment descriptor type
 * -r: readable
 * -w: writable
 * -e: expand-down
 */
#define  APP_DATA_RO_SEGMENT         0x11
#define  APP_DATA_RW_SEGMENT         0x12
#define  APP_DATA_RE_SEGMENT         0x14
#define  APP_DATA_RWE_SEGMENT        0x16
/* code segment descriptor type
 * -x: executable
 * -r: readable
 * -c: conforming
 */
#define  APP_CODE_X_SEGMENT          0x18,
#define  APP_CODE_XR_SEGMENT         0x1a,
#define  APP_CODE_XC_SEGMENT         0x1c,
#define  APP_CODE_XRC_SEGMENT        0x1e,


/**
 *\brief x86_dt_privilege_t
 *\note descriptor privilege level: used by the protection mechanism.
 */
#define  PRIVILEGE_LEVEL_0          0  /* High */
#define  PRIVILEGE_LEVEL_1          1
#define  PRIVILEGE_LEVEL_2          2
#define  PRIVILEGE_LEVEL_3          3  /* Low */


/**
 *\brief table_indicator_type_t
 *\note  specifies to which descriptor table the selector refers.
 */
#define  TABLE_INDICATOR_GDT        0
#define  TABLE_INDICATOR_LDT        1


/**
 *\brief x86_int_except_id_t
 *\note  reserved exceptions and interrupts vector number.
 */
#define  DIVIDE_ERROR                  0
#define  DEBUG_EXCEPTIONS              1
#define  NOMASKABLE                    2
#define  BREAKPOINT                    3
#define  OVERFLOW                      4
#define  BOUNDS_CHECK                  5
#define  INVALID_OPCODE                6
#define  COPROCESSOR_NOT_AVAILABLE     7
#define  DOUBLE_FAULT                  8
#define  COPROCESSOR_SEGMENT_OVERRUN   9
#define  INVALID_TSS                   10
#define  SEGMENT_NOT_PRESENT           11
#define  STACK_EXCEPTION               12
#define  GENERAL_PROTECTION            13
#define  PAGE_FAULT                    14
#define  COPRECESSOR_ERROR             16


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
#define  GET_SEGMENT_REG_VALUE_FOR_PM(index, ti, rpl) \
	                       ((index << 3) | (ti << 2) | rpl)



#define __CREATE_GDT_ITEM__(addr, len, type, level, unit)        \
                           (((len & 0x0000ffffull) << 0)       | \
                           ((addr & 0x0000ffffull) << 16)      | \
                           (((addr >> 16) & 0x00ffull) << 32)  | \
                           (type << 40)                        | \
                           (level << 45)                       | \
                           (1ull << 47)                        | \
                           (((len >> 16) & 0x0full) << 48)     | \
                           (1ull << 54)                        | \
                           (unit << 55)                        | \
                           ((addr >> 24) << 56))

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
#define CREATE_GDT_ITEM_UNIT_BYTE(addr, seg_len, type, level)              \
                         __CREATE_GDT_ITEM__(addr, len, type, level, 0ull)

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
#define CREATE_GDT_ITEM_UNIT_4KB(addr, seg_len, type, level)              \
                         __CREATE_GDT_ITEM__(addr, len, type, level, 1ull)



#endif /* X64_SEGMENT_DEF_H_ */





