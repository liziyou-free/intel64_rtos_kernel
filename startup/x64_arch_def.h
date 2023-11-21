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

#ifndef X64_ARCH_DEF_H_
#define X64_ARCH_DEF_H_


/**
 *\brief X64 exception and interrupt vectors
 *
 * Allowable range for vector numbers is 0 to 255
 * vectors[0:31] reserved by the Intel 64 and IA-32 architectures
 * vectors[32:255] are designated as user-defined interrupts
 */
#define X64_DIVIDE_ERROR                    0
#define X64_DEBUG                           1
#define X64_NON_MASKABLE_INTERRUPT          2
#define X64_BREAKPOINT                      3
#define X64_OVERFLOW                        4
#define X64_BOUND_RANGE                     5
#define X64_INVALID_OPCODE                  6
#define X64_DEVICE_NOT_AVAILABLE            7
#define X64_DOUBLE_FAULT                    8
#define X64_COPROCESSOR_SEGMENT_OVERRUN     9
#define X64_INVALID_TSS                     10
#define X64_SEGMENT_NOT_PRESENT             11
#define X64_STACK_FAULT                     12
#define X64_GENERAL_PROTECTION              13
#define X64_PAGE_FAULT                      14
#define X64_RESERVED                        15
#define X64_X87_FPU_FP_ERROR                16
#define X64_ALIGNMENT_CHECK                 17
#define X64_MACHINE_CHECK                   18
#define X64_SIMD_FP                         19
#define X64_VIRT_EXCEPTION                  20
#define X64_SECURITY_EXCEPTION              30


/**
 *\brief System control registers
 */
//CR0
#define X64_CR0_PE_BIT                 (1u << 0)
#define X64_CR0_MP_BIT                 (1u << 1)
#define X64_CR0_EM_BIT                 (1u << 2)
#define X64_CR0_TS_BIT                 (1u << 3)
#define X64_CR0_ET_BIT                 (1u << 4)
#define X64_CR0_NE_BIT                 (1u << 5)
#define X64_CR0_WP_BIT                 (1u << 16)
#define X64_CR0_AM_BIT                 (1u << 18)
#define X64_CR0_NW_BIT                 (1u << 29)
#define X64_CR0_CD_BIT                 (1u << 30)
#define X64_CR0_PG_BIT                 (1u << 31)
//CR3
#define X64_CR3_PWT_BIT                (1u << 3)
#define X64_CR3_PCD_BIT                (1u << 4)
//CR4
#define X64_CR4_VME_BIT                (1U << 0)
#define X64_CR4_PVI_BIT                (1U << 1)
#define X64_CR4_TSD_BIT                (1U << 2)
#define X64_CR4_DE_BIT                 (1U << 3)
#define X64_CR4_PSE_BIT                (1U << 4)
#define X64_CR4_PAE_BIT                (1U << 5)
#define X64_CR4_MCE_BIT                (1U << 6)
#define X64_CR4_PGE_BIT                (1U << 7)
#define X64_CR4_PCE_BIT                (1U << 8)
#define X64_CR4_OSFXSR_BIT             (1U << 9)
#define X64_CR4_OSXMMEXCPT_BIT         (1U << 10)
#define X64_CR4_UMIP_BIT               (1U << 11)
#define X64_CR4_VMXE_BIT               (1U << 13)
#define X64_CR4_SMXE_BIT               (1U << 14)
#define X64_CR4_FSGSBASE_BIT           (1U << 16)
#define X64_CR4_PCIDE_BIT              (1U << 17)
#define X64_CR4_OSXSAVE_BIT            (1U << 18)
#define X64_CR4_KL_BIT                 (1U << 19)
#define X64_CR4_SMEP_BIT               (1U << 20)
#define X64_CR4_SMAP_BIT               (1U << 21)
#define X64_CR4_PKE_BIT                (1U << 22)
#define X64_CR4_CET_BIT                (1U << 23)
#define X64_CR4_PKS_BIT                (1U << 24)
#define X64_CR4_UINTR_BIT              (1U << 25)


/**
 *\brief Model-Specific Registers
 */
#define IA32_EFER_MSR			       0xC0000080U
#define IA32_EFER_MSR_SCE		       (1u << 0)    /* R/W */
#define IA32_EFER_MSR_LME		       (1u << 8)    /* R/W */
#define IA32_EFER_MSR_LMA		       (1u << 10)   /* R */
#define IA32_EFER_MSR_NXE		       (1u << 11)   /* R/W */


#endif /* X64_ARCH_DEF_H_ */
