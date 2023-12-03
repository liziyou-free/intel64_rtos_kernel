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
#define X64_MMU_PS_BIT                  (1ULL << 7)
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



typedef struct {
  /**
   * \brief The paging type used to map this physical address.
   *
   * \note  1: 1GB page   2: 2MB page  4: 4KB page
   */
  uint8_t   page_type;

  uint64_t  phy_addr;  /* Returned physical address */
} phyaddr_info;


typedef uint64_t table_unit_t[512];


/**
 * \brief x64_cr3_memory_flag_t
 *
 * \note Only works if PCID is disabled.
 */
#define CR3_PWT_BIT        (1 << 3)
#define CR3_PCD_BIT        (1 << 4)


/**
 * \brief Align check
 *
 * \param addr[in]  address to check
 * \param align[in] number of bytes to align
 *
 * \retval 0: align
 * \retval 1: not align
 */
#define ALIGN_CHECK(addr, align)   ((((uint64_t)addr) & (~(align##ULL))) ? 0 : 1)


/**
 * \brief Update CR3 register(Write the MMU translation table root address to cr3).
 *
 * \param addr[in] Physical address of the 4-KByte aligned PML4 table or PML5
 * table used for linear-address translation1.
 * \param attr_flag[in] indirectly determines the memory type used to access
 * the PML4 table or PML5 table. @ x64_cr3_memory_flag_t
 *
 * \retval none
 */
void x64_flush_tb_addr(uint64_t addr, uint8_t attr_flag);

uint8_t x64_lookup_phyaddr (phyaddr_info *p_info, uint64_t va);

table_unit_t *x64_mmu_alloc_table (uint32_t blocks);

void x64_mmu_free_table (void *p_addr);

void mmu_print_tb_poor_bitmap (void);


#endif /* __ASSEMBLY__ */
#endif /* X64_DRIVER_MMU_H_ */










