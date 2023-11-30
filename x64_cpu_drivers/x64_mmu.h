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

#ifndef X64_DRIVER_MMU_H_
#define X64_DRIVER_MMU_H_

#define X64_MMU_32BIT_PAGE_ENTRIES        1024
#define X64_MMU_4LEVEL_PAGE_ENTRIES       512
#define X64_MMU_5LEVEL_PAGE_ENTRIES       512
#define X64_MMU_PAE_PAGE_ENTRIES          2


#define X64_MMU_PAGE_ALIGN                4096
#define X86_MMU_PAGE_ENTRY_SIZE           8


/**
 *\brief Entry field definition
 */
#define X64_MMU_PRESENT_BIT             (1ULL << 0)
#define X64_MMU_RW_BIT                  (1ULL << 1)
#define X64_MMU_US_BIT                  (1ULL << 2)
#define X64_MMU_PWT_BIT                 (1ULL << 3)
#define X64_MMU_PCD_BIT                 (1ULL << 4)
#define X64_MMU_ACCESSED_BIT            (1ULL << 5)
#define X64_MMU_PS_BIT                  (1ULL << 7)  /* Reserved (must be 0) */
#define X64_MMU_GLOBAL_BIT              (1ULL << 8)
#define X64_MMU_R_BIT                   (1ULL << 11)
#define X64_MMU_PAT_BIT                 (1ULL << 12)
#define X64_MMU_XD_BIT                  (1ULL << 63)


#ifndef __ASSEMBLY__
/**
 *\brief Entry type
 */
typedef enum {
  x64_mmu_pml5e = 0,  /* current, not support */
  x64_mmu_pml4e,
  x64_mmu_pdpte_pde,
  x64_mmu_pdpte_1g,
  x64_mmu_pde_pt,
  x64_mmu_pde_2m,
  x64_mmu_pte_4k
} x64_mmu_entry_t;



/**
 * \brief Align check
 *
 * \param addr[in]  address to check
 * \param align[in] number of bytes to align
 *
 * \retval 0: align
 * \retval 1: not align
 */
#define ALIGN_CHECK(addr, align)   (addr & (~(align##ULL))) ? 0 : 1


#endif /* __ASSEMBLY__ */
#endif /* X64_DRIVER_MMU_H_ */










