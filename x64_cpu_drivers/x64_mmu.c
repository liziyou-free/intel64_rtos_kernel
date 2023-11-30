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

#include <stdint.h>
#include "x64_mmu.h"

/**
 * Only support 4 level page, not support PCID, Most access 256 TBytes.
 * condition: (CR0.PG=1, CR4.PAE=1, IA32_EFER.LME=1, CR4.LA57=0, CR4.PCIDE=0)
 */


void x64_flush_tb_addr(uint64_t addr, uint8_t attribute)
{
    uint64_t cr3 = 0;

    if (ALIGN_CHECK(addr, 4096)) {
        /* Not 4K aligned */
        for (;;);
    }
    cr3 = addr | attribute;
    __asm__ __volatile__("mov %0, %%cr3"::"r"(cr3));
    return;
}




uint8_t x64_mmu_create_entry (uint64_t *p_entry,
                              x64_mmu_entry_t type,
                              uint64_t addr,
                              uint8_t  pat,
                              uint64_t flag)
{
    uint8_t  ret = -1;

    switch (type) {
    case x64_mmu_pdpte_1g:
        flag |= X64_MMU_PS_BIT;
        flag |= pat ? (1 << 12) : (0 << 12);
        ret = ALIGN_CHECK(addr, 0x40000000); /* 1GB align check */
        break;

    case x64_mmu_pde_2m:
        flag |= X64_MMU_PS_BIT;
        flag |= pat ? (1 << 12) : (0 << 12);
        ret = ALIGN_CHECK(addr, 0x200000);  /* 2MB align check */
        break;

    case x64_mmu_pml5e:
    case x64_mmu_pml4e:
    case x64_mmu_pdpte_pde:
    case x64_mmu_pde_pt:
    case x64_mmu_pte_4k:
        flag &= ~X64_MMU_PS_BIT; /* These types have no PS bit */
        flag |= pat ? (1 << 7) : (0 << 7);
        ret = ALIGN_CHECK(addr, 0x1000);  /* 4KB align check */
        break;
    default:
      for (;;);
    }

    if (!ret){
        *p_entry = addr | flag;
    }

    return ret;
}




