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
#define X64_MMU_PAE_PAGE_ENTRIES          2
#define X64_MMU_4LEVEL_PAGE_ENTRIES       512
#define X64_MMU_5LEVEL_PAGE_ENTRIES       512

#define X64_MMU_PAGE_ALIGN                4096
#define X86_MMU_PAGE_ENTRY_SIZE           8



/**
 *\brief Field of a PML4 Entry (PML4E)
 */
#define X64_PML4E_PRESENT_BIT             (1 << 0)
#define X64_PML4E_RW_BIT                  (1 << 1)
#define X64_PML4E_US_BIT                  (1 << 2)
#define X64_PML4E_PWT_BIT                 (1 << 3)
#define X64_PML4E_PCD_BIT                 (1 << 4)
#define X64_PML4E_ACCESSED_BIT            (1 << 5)
#define X64_PML4E_PS_BIT                  (1 << 7)  /* Reserved (must be 0) */
#define X64_PML4E_R_BIT                   (1 << 11)
#define X64_PML4E_XD_BIT                  (1 << 63)


/**
 *\brief Field of a Page-Directory-Pointer-Table Entry (PDPTE)
 * that Maps a 1-GByte Page
 */
#define X64_PDPTE_1GB_PRESENT_BIT         (1 << 0)
#define X64_PDPTE_1GB_RW_BIT              (1 << 1)
#define X64_PDPTE_1GB_US_BIT              (1 << 2)
#define X64_PDPTE_1GB_PWT_BIT             (1 << 3)
#define X64_PDPTE_1GB_PCD_BIT             (1 << 4)
#define X64_PDPTE_1GB_ACCESSED_BIT        (1 << 5)
#define X64_PDPTE_1GB_DIRTY_BIT           (1 << 6)
#define X64_PDPTE_1GB_PS_BIT              (1 << 7)
#define X64_PDPTE_1GB_GLOBAL_BIT          (1 << 8)
#define X64_PDPTE_1GB_R_BIT               (1 << 11)
/*The PAT is supported on all processors that support 4-level paging*/
#define X64_PDPTE_1GB_PAT_BIT             (1 << 12)
#define X64_PDPTE_1GB_XD_BIT              (1 << 63)


/**
 *\brief Field of a Page-Directory-Pointer-Table Entry (PDPTE)
 * that References a Page Directory
 */
#define X64_PDPTE_PDE_PRESENT_BIT         (1 << 0)
#define X64_PDPTE_PDE_RW_BIT              (1 << 1)
#define X64_PDPTE_PDE_US_BIT              (1 << 2)
#define X64_PDPTE_PDE_PWT_BIT             (1 << 3)
#define X64_PDPTE_PDE_PCD_BIT             (1 << 4)
#define X64_PDPTE_PDE_ACCESSED_BIT        (1 << 5)
#define X64_PDPTE_PDE_PS_BIT              (1 << 7)
#define X64_PDPTE_PDE_R_BIT               (1 << 11)
#define X64_PDPTE_PDE_XD_BIT              (1 << 63)


/**
 *\brief Field of a Page-Directory Entry that Maps a 2-MByte Page
 */
#define X64_PDE_2MB_PRESENT_BIT           (1 << 0)
#define X64_PDE_2MB_RW_BIT                (1 << 1)
#define X64_PDE_2MB_US_BIT                (1 << 2)
#define X64_PDE_2MB_PWT_BIT               (1 << 3)
#define X64_PDE_2MB_PCD_BIT               (1 << 4)
#define X64_PDE_2MB_ACCESSED_BIT          (1 << 5)
#define X64_PDE_2MB_DIRTY_BIT             (1 << 6)
#define X64_PDE_2MB_PS_BIT                (1 << 7)
#define X64_PDE_2MB_GLOBAL_BIT            (1 << 8)
#define X64_PDE_2MB_PAT_BIT               (1 << 12)


/**
 *\brief Field of a Page-Directory Entry that References a Page Table
 */
#define X64_PDE_PT_PRESENT_BIT           (1 << 0)
#define X64_PDE_PT_RW_BIT                (1 << 1)
#define X64_PDE_PT_US_BIT                (1 << 2)
#define X64_PDE_PT_PWT_BIT               (1 << 3)
#define X64_PDE_PT_PCD_BIT               (1 << 4)
#define X64_PDE_PT_ACCESSED_BIT          (1 << 5)
#define X64_PDE_PT_PS_BIT                (1 << 7)
#define X64_PDE_PT_R_BIT                 (1 << 11)


/**
*\brief Field of a Page-Table Entry that Maps a 4-KByte Page
*/
#define X64_PT_4KB_PRESENT_BIT           (1 << 0)
#define X64_PT_4KB_RW_BIT                (1 << 1)
#define X64_PT_4KB_US_BIT                (1 << 2)
#define X64_PT_4KB_PWT_BIT               (1 << 3)
#define X64_PT_4KB_PCD_BIT               (1 << 4)
#define X64_PT_4KB_ACCESSED_BIT          (1 << 5)
#define X64_PT_4KB_DIRTY_BIT             (1 << 6)
#define X64_PT_4KB_PS_BIT                (1 << 7)
#define X64_PT_4KB_GLOBAL_BIT            (1 << 8)
#define X64_PT_4KB_R_BIT                 (1 << 11)


#endif /* X64_DRIVER_MMU_H_ */










